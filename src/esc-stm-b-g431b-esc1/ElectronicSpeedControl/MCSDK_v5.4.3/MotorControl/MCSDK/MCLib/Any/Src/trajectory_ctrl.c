/**
 ******************************************************************************
 * @file    trajectory_ctrl.c
 * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file provides firmware functions that implement the features
  *          of Position Control Mode component of the Motor Control SDK.
  *
 ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "trajectory_ctrl.h"
#include "speed_pos_fdbk.h"


/**
  * @brief  It initializes the static variables used by the position control modes.
  * @param  pHandle: handler of the current instance of the Position Control component.
  * @param  pPIDPosReg pointer on the handler of the current instance of PID used for the position regulation.
  * @param  pSTC pointer on the handler of the current instance of the SpeednTorqCtrl component.
  * @param  pENC handler of the current instance of the EncAlignCtrl component.
  * @retval none
  */
void TC_Init(PosCtrl_Handle_t *pHandle, PID_Handle_t * pPIDPosReg, SpeednTorqCtrl_Handle_t * pSTC, ENCODER_Handle_t * pENC)
{

  pHandle->MovementDuration = 0.0f;
  pHandle->AngleStep = 0.0f;
  pHandle->SubStep[0] = 0.0f;
  pHandle->SubStep[1] = 0.0f;
  pHandle->SubStep[2] = 0.0f;
  pHandle->SubStep[3] = 0.0f;
  pHandle->SubStep[4] = 0.0f;
  pHandle->SubStep[5] = 0.0f;

  pHandle->SubStepDuration = 0;

  pHandle->Jerk = 0.0f;
  pHandle->CruiseSpeed = 0.0f;
  pHandle->Acceleration = 0.0f;
  pHandle->Omega = 0.0f;
  pHandle->OmegaPrev = 0.0f;
  pHandle->Theta = 0.0f;
  pHandle->ThetaPrev = 0.0f;
  pHandle->ReceivedTh = 0.0f;
  pHandle->TcTick = 0;
  pHandle->ElapseTime = 0.0f;

  pHandle->PositionControlRegulation = DISABLE;
  pHandle->PositionCtrlStatus = TC_READY_FOR_COMMAND;

  pHandle->pENC = pENC;
  pHandle->pSTC = pSTC;
  pHandle->PIDPosRegulator = pPIDPosReg;

  pHandle->MecAngleOffset = 0;
}

/**
  * @brief  It configures the trapezoidal speed trajectory.
  * @param  pHandle: handler of the current instance of the Position Control component.
  * @param  startingAngle Current mechanical position.
  * @param  angleStep Target mechanical position.
  * @param  movementDuration Duration to reach the final position.
  * @retval true  = Trajectory command programmed
  *         false = Not ready for a new trajectory configuration.
  */
bool TC_MoveCommand(PosCtrl_Handle_t *pHandle, float startingAngle, float angleStep, float movementDuration)
{

  bool RetConfigStatus = false;
  float fMinimumStepDuration;

  if (pHandle->PositionCtrlStatus == TC_READY_FOR_COMMAND)
  {
    pHandle->PositionControlRegulation = ENABLE;

    fMinimumStepDuration = (9.0f * pHandle->SamplingTime);

    // WARNING: Movement duration value is rounded to the nearest valid value
    //          [(DeltaT/9) / SamplingTime]:  shall be an integer value
    pHandle->MovementDuration = (float)((int)(movementDuration / fMinimumStepDuration)) * fMinimumStepDuration;
    
    pHandle->StartingAngle = startingAngle;
    pHandle->AngleStep = angleStep;
    pHandle->FinalAngle = startingAngle + angleStep;

    // SubStep duration = DeltaT/9  (DeltaT represents the total duration of the programmed movement)
    pHandle->SubStepDuration = pHandle->MovementDuration / 9.0f;

    // Sub step of acceleration phase
    pHandle->SubStep[0] = 1 * pHandle->SubStepDuration;   /* Sub-step 1 of acceleration phase */
    pHandle->SubStep[1] = 2 * pHandle->SubStepDuration;   /* Sub-step 2 of acceleration phase */
    pHandle->SubStep[2] = 3 * pHandle->SubStepDuration;   /* Sub-step 3 of acceleration phase */

    // Sub step of  deceleration Phase
    pHandle->SubStep[3] = 6 * pHandle->SubStepDuration;   /* Sub-step 1 of deceleration phase */
    pHandle->SubStep[4] = 7 * pHandle->SubStepDuration;   /* Sub-step 2 of deceleration phase */
    pHandle->SubStep[5] = 8 * pHandle->SubStepDuration;   /* Sub-step 3 of deceleration phase */

    // Jerk (J) to be used by the trajectory calculator to integrate (step by step) the target position.
    // J = DeltaTheta/(12 * A * A * A)  => DeltaTheta = final position and A = Sub-Step duration
    pHandle->Jerk = pHandle->AngleStep / (12 * pHandle->SubStepDuration * pHandle->SubStepDuration * pHandle->SubStepDuration);

    // Speed cruiser = 2*J*A*A)
    pHandle->CruiseSpeed = 2 * pHandle->Jerk * pHandle->SubStepDuration * pHandle->SubStepDuration;

    pHandle->ElapseTime = 0.0f;

    pHandle->Omega = 0.0f;
    pHandle->Acceleration = 0.0f;
    pHandle->Theta = startingAngle;

    pHandle->PositionCtrlStatus = TC_MOVEMENT_ON_GOING;   /* new trajectory has been programmed */

    RetConfigStatus = true;

  }
  return (RetConfigStatus);
}

/**
  * @brief  Used to follow an angular position command.
  * @param  pHandle: handler of the current instance of the Position Control component.
  * @param  Angle Target mechanical position.
  * @retval none
  */
void TC_FollowCommand(PosCtrl_Handle_t *pHandle, float Angle)
{
  float omega = 0, acceleration = 0, dt = 0;
  
  // Estimate speed
  if (pHandle->ReceivedTh > 0) 
  {
    // Calculate dt
    dt = pHandle->TcTick * pHandle->SysTickPeriod;
    pHandle->TcTick = 0;
  
    omega = (Angle - pHandle->ThetaPrev) / dt;
  }
  
  // Estimated acceleration
  if (pHandle->ReceivedTh > 1) {
    acceleration = (omega - pHandle->OmegaPrev) / dt;
  }
  
  // Update state variable
  pHandle->ThetaPrev  = Angle;
  pHandle->OmegaPrev = omega;
  if (pHandle->ReceivedTh < 2)
  {
    pHandle->ReceivedTh++;
  }
  
  pHandle->Acceleration = acceleration;
  pHandle->Omega = omega;
  pHandle->Theta = Angle;
  
  pHandle->PositionCtrlStatus = TC_FOLLOWING_ON_GOING;   /* follow mode has been programmed */
  
  return;
}

/**
  * @brief  It proceeds on the position control loop.
  * @param  pHandle: handler of the current instance of the Position Control component.
  * @retval none
  */
void TC_PositionRegulation(PosCtrl_Handle_t *pHandle)
{

  int32_t wMecAngleRef;
  int32_t wMecAngle;
  int32_t wError;
  int32_t hTorqueRef_Pos;

  if ( pHandle->PositionCtrlStatus == TC_MOVEMENT_ON_GOING )
  {
    TC_MoveExecution(pHandle);
  }
  
  if ( pHandle->PositionCtrlStatus == TC_FOLLOWING_ON_GOING )
  {
    TC_FollowExecution(pHandle);
  }
  
  if (pHandle->PositionControlRegulation == ENABLE)
  {
    wMecAngleRef = (int32_t)(pHandle->Theta * RADTOS16);

    wMecAngle = SPD_GetMecAngle(STC_GetSpeedSensor(pHandle->pSTC));
    wError = wMecAngleRef - wMecAngle;
    hTorqueRef_Pos = PID_Controller(pHandle->PIDPosRegulator, wError);

    STC_SetControlMode( pHandle->pSTC, STC_TORQUE_MODE );
    STC_ExecRamp( pHandle->pSTC, hTorqueRef_Pos, 0 );
  }

}

/**
  * @brief  It executes the programmed trajectory movement.
  * @param  pHandle: handler of the current instance of the Position Control component.
  * @retval none
  */
void TC_MoveExecution(PosCtrl_Handle_t *pHandle)
{

  float jerkApplied = 0;

  if (pHandle->ElapseTime < pHandle->SubStep[0])              // 1st Sub-Step interval time of acceleration phase
  {
    jerkApplied = pHandle->Jerk;
  }
  else if (pHandle->ElapseTime < pHandle->SubStep[1])         // 2nd Sub-Step interval time of acceleration phase
  {
  }
  else if (pHandle->ElapseTime < pHandle->SubStep[2])         // 3rd Sub-Step interval time of acceleration phase
  {
    jerkApplied = -(pHandle->Jerk);
  }
  else if (pHandle->ElapseTime < pHandle->SubStep[3])         // Speed Cruise phase (after acceleration and before deceleration phases)
  {
    pHandle->Acceleration = 0.0f;
    pHandle->Omega = pHandle->CruiseSpeed;
  }
  else if (pHandle->ElapseTime < pHandle->SubStep[4])         // 1st Sub-Step interval time of deceleration phase
  {
    jerkApplied = -(pHandle->Jerk);
  }
  else if (pHandle->ElapseTime < pHandle->SubStep[5])         // 2nd Sub-Step interval time of deceleration phase
  {

  }
  else if (pHandle->ElapseTime < pHandle->MovementDuration)   // 3rd Sub-Step interval time of deceleration phase
  {
    jerkApplied = pHandle->Jerk;
  }
  else
  {
    pHandle->Theta = pHandle->FinalAngle;
    pHandle->PositionCtrlStatus = TC_TARGET_POSITION_REACHED;
  }

  if ( pHandle->PositionCtrlStatus == TC_MOVEMENT_ON_GOING )
  {
    pHandle->Acceleration += jerkApplied * pHandle->SamplingTime;
    pHandle->Omega += pHandle->Acceleration * pHandle->SamplingTime;
    pHandle->Theta += pHandle->Omega * pHandle->SamplingTime;
  }

  pHandle->ElapseTime += pHandle->SamplingTime;

  if (TC_RampCompleted(pHandle))
  {
    if (pHandle->AlignmentStatus == TC_ZERO_ALIGNMENT_START) {
      // Ramp is used to search the zero index, if completed there is no z signal
      pHandle->AlignmentStatus = TC_ALIGNMENT_ERROR;
    }
    pHandle->ElapseTime = 0;
    pHandle->PositionCtrlStatus = TC_READY_FOR_COMMAND;
  }
}

/**
  * @brief  It updates the angular position.
  * @param  pHandle: handler of the current instance of the Position Control component.
  * @retval none
  */
void TC_FollowExecution(PosCtrl_Handle_t *pHandle)
{
  pHandle->Omega += pHandle->Acceleration * pHandle->SamplingTime;
  pHandle->Theta += pHandle->Omega        * pHandle->SamplingTime;
}

/**
  * @brief  It handles the alignment phase at starting before any position commands.
  * @param  pHandle: handler of the current instance of the Position Control component.
  * @retval none
  */
void TC_EncAlignmentCommand(PosCtrl_Handle_t *pHandle)
{
  int32_t wMecAngleRef;
  
  if (pHandle->AlignmentStatus == TC_ALIGNMENT_COMPLETED) {
    // Do nothing - EncAlignment must be done only one time after the power on
  } else {
    if (pHandle->AlignmentCfg == TC_ABSOLUTE_ALIGNMENT_SUPPORTED)
    {
      // If index is supported start the search of the zero
      pHandle->EncoderAbsoluteAligned = false;
      wMecAngleRef = SPD_GetMecAngle(STC_GetSpeedSensor(pHandle->pSTC));
      TC_MoveCommand(pHandle, (float)(wMecAngleRef) / RADTOS16, Z_ALIGNMENT_NB_ROTATION, Z_ALIGNMENT_DURATION);
      pHandle->AlignmentStatus = TC_ZERO_ALIGNMENT_START;
    }
    else
    {
      // If index is not supprted set the alignment angle as zero reference
      pHandle->pENC->_Super.wMecAngle = 0;
      pHandle->AlignmentStatus = TC_ALIGNMENT_COMPLETED;
      pHandle->PositionCtrlStatus = TC_READY_FOR_COMMAND;
      pHandle->PositionControlRegulation = ENABLE;
    }
  }
}

/**
  * @brief  It controls if time allowed for movement is completed.
  * @param  pHandle: handler of the current instance of the Position Control component.
  * @retval true  = the programmed trajectory movement is completed
  *         false = the trajectory movement execution is still ongoing.
  */
bool TC_RampCompleted(PosCtrl_Handle_t *pHandle)
{
  bool retVal = false;

  // Check that entire sequence (Acceleration - Cruise - Deceleration) is completed.
  if (pHandle->ElapseTime > pHandle->MovementDuration + pHandle->SamplingTime)
  {
    retVal = true;
  }
  return (retVal);
}

/**
  * @brief  Set the absolute zero mechanical position.
  * @param  pHandle: handler of the current instance of the Position Control component.
  * @retval none
  */
void TC_EncoderReset(PosCtrl_Handle_t *pHandle)
{
  if ((!pHandle->EncoderAbsoluteAligned) && (pHandle->AlignmentStatus == TC_ZERO_ALIGNMENT_START))
  {
    pHandle->MecAngleOffset = pHandle->pENC->_Super.hMecAngle;
    pHandle->pENC->_Super.wMecAngle = 0;
    pHandle->EncoderAbsoluteAligned = true;
    pHandle->AlignmentStatus = TC_ALIGNMENT_COMPLETED;
    pHandle->PositionCtrlStatus = TC_READY_FOR_COMMAND;
    pHandle->Theta = 0.0f;
  }
  
  ENC_SetMecAngle(pHandle->pENC , pHandle->MecAngleOffset);
}

/**
  * @brief  Returns the current rotor mechanical angle, expressed in radiant.
  * @param  pHandle: handler of the current instance of the Position Control component.
  * @retval current mechanical position
  */
float TC_GetCurrentPosition(PosCtrl_Handle_t *pHandle)
{

  return ((float)( (SPD_GetMecAngle(STC_GetSpeedSensor(pHandle->pSTC))) / RADTOS16) );
}

/**
  * @brief  Returns the target rotor mechanical angle, expressed in radiant.
  * @param  pHandle: handler of the current instance of the Position Control component.
  * @retval Target mechanical position
  */
float TC_GetTargetPosition(PosCtrl_Handle_t *pHandle)
{
  return (pHandle->FinalAngle);
}

/**
  * @brief  Returns the duration used to execute the movement, expressed in seconds.
  * @param  pHandle: handler of the current instance of the Position Control component.
  * @retval Duration of programmed movement
  */
float TC_GetMoveDuration(PosCtrl_Handle_t *pHandle)
{
  return (pHandle->MovementDuration);
}

/**
  * @brief  Returns the status of the position control execution.
  * @param  pHandle: handler of the current instance of the Position Control component.
  * @retval Position Control Status
  */
PosCtrlStatus_t TC_GetControlPositionStatus(PosCtrl_Handle_t *pHandle)
{
  return (pHandle->PositionCtrlStatus);
}

/**
  * @brief  Returns the status after the rotor alignment phase.
  * @param  pHandle: handler of the current instance of the Position Control component.
  * @retval Alignment Status
  */
AlignStatus_t TC_GetAlignmentStatus(PosCtrl_Handle_t *pHandle)
{
  return (pHandle->AlignmentStatus);
}

/**
  * @brief  It increments Tick counter used in follow mode.
  * @param  pHandle: handler of the current instance of the Position Control component.
  * @retval none
  */
void TC_IncTick(PosCtrl_Handle_t *pHandle)
{
  pHandle->TcTick++;
}


/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
