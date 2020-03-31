/*
  ******************************************************************************
  * @file    speed_torq_ctrl.c
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file provides firmware functions that implement the following features
  *          of the Speed & Torque Control component of the Motor Control SDK.
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
#include "speed_torq_ctrl.h"
#include "speed_pos_fdbk.h"

#include "mc_type.h"

#define CHECK_BOUNDARY

/** @addtogroup MCSDK
  * @{
  */

/** @defgroup SpeednTorqCtrl Speed & Torque Control
  * @brief Speed & Torque Control component of the Motor Control SDK
  *
  * @todo Document the Speed & Torque Control "module".
  *
  * @{
  */

/**
  * @brief  Initializes all the object variables, usually it has to be called
  *         once right after object creation.
  * @param  pHandle: handler of the current instance of the SpeednTorqCtrl component
  * @param  oPI the PI object used as controller for the speed regulation.
  *         It can be equal to MC_NULL if the STC is initialized in torque mode
  *         and it will never be configured in speed mode.
  * @param  oSPD the speed sensor used to perform the speed regulation.
  *         It can be equal to MC_NULL if the STC is used only in torque
  *         mode.
  * @retval none.
  */
__weak void STC_Init( SpeednTorqCtrl_Handle_t * pHandle, PID_Handle_t * pPI, SpeednPosFdbk_Handle_t * SPD_Handle )
{

  pHandle->PISpeed = pPI;
  pHandle->SPD = SPD_Handle;
  pHandle->Mode = pHandle->ModeDefault;
  pHandle->SpeedRefUnitExt = ( int32_t )pHandle->MecSpeedRefUnitDefault * 65536;
  pHandle->TorqueRef = ( int32_t )pHandle->TorqueRefDefault * 65536;
  pHandle->TargetFinal = 0;
  pHandle->RampRemainingStep = 0u;
  pHandle->IncDecAmount = 0;
}

/**
  * @brief It sets in real time the speed sensor utilized by the STC.
  * @param  pHandle: handler of the current instance of the SpeednTorqCtrl component
  * @param SPD_Handle Speed sensor component to be set.
  * @retval none
  */
__weak void STC_SetSpeedSensor( SpeednTorqCtrl_Handle_t * pHandle, SpeednPosFdbk_Handle_t * SPD_Handle )
{
  pHandle->SPD = SPD_Handle;
}

/**
  * @brief It returns the speed sensor utilized by the FOC.
  * @param  pHandle: handler of the current instance of the SpeednTorqCtrl component
  * @retval SpeednPosFdbk_Handle_t speed sensor utilized by the FOC.
  */
__weak SpeednPosFdbk_Handle_t * STC_GetSpeedSensor( SpeednTorqCtrl_Handle_t * pHandle )
{
  return ( pHandle->SPD );
}

/**
  * @brief  It should be called before each motor restart. If STC is set in
            speed mode, this method resets the integral term of speed regulator.
  * @param  pHandle: handler of the current instance of the SpeednTorqCtrl component
  * @retval none.
  */
__weak void STC_Clear( SpeednTorqCtrl_Handle_t * pHandle )
{
  if ( pHandle->Mode == STC_SPEED_MODE )
  {
    PID_SetIntegralTerm( pHandle->PISpeed, 0 );
  }
}

/**
  * @brief  Get the current mechanical rotor speed reference expressed in tenths
  *         of HZ.
  * @param  pHandle: handler of the current instance of the SpeednTorqCtrl component
  * @retval int16_t current mechanical rotor speed reference expressed in tenths
  *         of HZ.
  */
__weak int16_t STC_GetMecSpeedRefUnit( SpeednTorqCtrl_Handle_t * pHandle )
{
  return ( ( int16_t )( pHandle->SpeedRefUnitExt / 65536 ) );
}

/**
  * @brief  Get the current motor torque reference. This value represents
  *         actually the Iq current reference expressed in digit.
  *         To convert current expressed in digit to current expressed in Amps
  *         is possible to use the formula:
  *         Current(Amp) = [Current(digit) * Vdd micro] / [65536 * Rshunt * Aop]
  * @param  pHandle: handler of the current instance of the SpeednTorqCtrl component
  * @retval int16_t current motor torque reference. This value represents
  *         actually the Iq current expressed in digit.
  */
__weak int16_t STC_GetTorqueRef( SpeednTorqCtrl_Handle_t * pHandle )
{
  return ( ( int16_t )( pHandle->TorqueRef / 65536 ) );
}

/**
  * @brief  Set the modality of the speed and torque controller. Two modality
  *         are available Torque mode and Speed mode.
  *         In Torque mode is possible to set directly the motor torque
  *         reference or execute a motor torque ramp. This value represents
  *         actually the Iq current reference expressed in digit.
  *         In Speed mode is possible to set the mechanical rotor speed
  *         reference or execute a speed ramp. The required motor torque is
  *         automatically calculated by the STC.
  *         This command interrupts the execution of any previous ramp command
  *         maintaining the last value of Iq.
  * @param  pHandle: handler of the current instance of the SpeednTorqCtrl component
  * @param  bMode modality of STC. It can be one of these two settings:
  *         STC_TORQUE_MODE to enable the Torque mode or STC_SPEED_MODE to
  *         enable the Speed mode.
  * @retval none
  */
__weak void STC_SetControlMode( SpeednTorqCtrl_Handle_t * pHandle, STC_Modality_t bMode )
{
  pHandle->Mode = bMode;
  pHandle->RampRemainingStep = 0u; /* Interrupts previous ramp. */
}

/**
  * @brief  Get the modality of the speed and torque controller.
  * @param  pHandle: handler of the current instance of the SpeednTorqCtrl component
  * @retval STC_Modality_t It returns the modality of STC. It can be one of
  *         these two values: STC_TORQUE_MODE or STC_SPEED_MODE.
  */
__weak STC_Modality_t STC_GetControlMode( SpeednTorqCtrl_Handle_t * pHandle )
{
  return pHandle->Mode;
}

/**
  * @brief  Starts the execution of a ramp using new target and duration. This
  *         command interrupts the execution of any previous ramp command.
  *         The generated ramp will be in the modality previously set by
  *         STC_SetControlMode method.
  * @param  pHandle: handler of the current instance of the SpeednTorqCtrl component
  * @param  hTargetFinal final value of command. This is different accordingly
  *         the STC modality.
  *         If STC is in Torque mode hTargetFinal is the value of motor torque
  *         reference at the end of the ramp. This value represents actually the
  *         Iq current expressed in digit.
  *         To convert current expressed in Amps to current expressed in digit
  *         is possible to use the formula:
  *         Current(digit) = [Current(Amp) * 65536 * Rshunt * Aop]  /  Vdd micro
  *         If STC is in Speed mode hTargetFinal is the value of mechanical
  *         rotor speed reference at the end of the ramp expressed in tenths of
  *         HZ.
  * @param  hDurationms the duration of the ramp expressed in milliseconds. It
  *         is possible to set 0 to perform an instantaneous change in the value.
  * @retval bool It return false if the absolute value of hTargetFinal is out of
  *         the boundary of the application (Above max application speed or max
  *         application torque or below min application speed depending on
  *         current modality of TSC) in this case the command is ignored and the
  *         previous ramp is not interrupted, otherwise it returns true.
  */
__weak bool STC_ExecRamp( SpeednTorqCtrl_Handle_t * pHandle, int16_t hTargetFinal, uint32_t hDurationms )
{
  bool AllowedRange = true;
  uint32_t wAux;
  int32_t wAux1;
  int16_t hCurrentReference;

  /* Check if the hTargetFinal is out of the bound of application. */
  if ( pHandle->Mode == STC_TORQUE_MODE )
  {
    hCurrentReference = STC_GetTorqueRef( pHandle );
#ifdef CHECK_BOUNDARY
    if ( ( int32_t )hTargetFinal > ( int32_t )pHandle->MaxPositiveTorque )
    {
      AllowedRange = false;
    }
    if ( ( int32_t )hTargetFinal < ( int32_t )pHandle->MinNegativeTorque )
    {
      AllowedRange = false;
    }
#endif
  }
  else
  {
    hCurrentReference = ( int16_t )( pHandle->SpeedRefUnitExt / 65536 );

#ifdef CHECK_BOUNDARY
    if ( ( int32_t )hTargetFinal > ( int32_t )pHandle->MaxAppPositiveMecSpeedUnit )
    {
      AllowedRange = false;
    }
    else if ( hTargetFinal < pHandle->MinAppNegativeMecSpeedUnit )
    {
      AllowedRange = false;
    }
    else if ( ( int32_t )hTargetFinal < ( int32_t )pHandle->MinAppPositiveMecSpeedUnit )
    {
      if ( hTargetFinal > pHandle->MaxAppNegativeMecSpeedUnit )
      {
        AllowedRange = false;
      }
    }
    else {}
#endif
  }

  if ( AllowedRange == true )
  {
    /* Interrupts the execution of any previous ramp command */
    if ( hDurationms == 0u )
    {
      if ( pHandle->Mode == STC_SPEED_MODE )
      {
        pHandle->SpeedRefUnitExt = ( int32_t )hTargetFinal * 65536;
      }
      else
      {
        pHandle->TorqueRef = ( int32_t )hTargetFinal * 65536;
      }
      pHandle->RampRemainingStep = 0u;
      pHandle->IncDecAmount = 0;
    }
    else
    {
      /* Store the hTargetFinal to be applied in the last step */
      pHandle->TargetFinal = hTargetFinal;

      /* Compute the (wRampRemainingStep) number of steps remaining to complete
      the ramp. */
      wAux = ( uint32_t )hDurationms * ( uint32_t )pHandle->STCFrequencyHz;
      wAux /= 1000u;
      pHandle->RampRemainingStep = wAux;
      pHandle->RampRemainingStep++;

      /* Compute the increment/decrement amount (wIncDecAmount) to be applied to
      the reference value at each CalcTorqueReference. */
      wAux1 = ( ( int32_t )hTargetFinal - ( int32_t )hCurrentReference ) * 65536;
      wAux1 /= ( int32_t )pHandle->RampRemainingStep;
      pHandle->IncDecAmount = wAux1;
    }
  }

  return AllowedRange;
}

/**
  * @brief  This command interrupts the execution of any previous ramp command.
  *         If STC has been set in Torque mode the last value of Iq is
  *         maintained.
  *         If STC has been set in Speed mode the last value of mechanical
  *         rotor speed reference is maintained.
  * @param  pHandle: handler of the current instance of the SpeednTorqCtrl component
  * @retval none
  */
__weak void STC_StopRamp( SpeednTorqCtrl_Handle_t * pHandle )
{

  pHandle->RampRemainingStep = 0u;
  pHandle->IncDecAmount = 0;
}

/**
  * @brief  It is used to compute the new value of motor torque reference. It
  *         must be called at fixed time equal to hSTCFrequencyHz. It is called
  *         passing as parameter the speed sensor used to perform the speed
  *         regulation.
  * @param  pHandle: handler of the current instance of the SpeednTorqCtrl component
  * @retval int16_t motor torque reference. This value represents actually the
  *         Iq current expressed in digit.
  *         To convert current expressed in Amps to current expressed in digit
  *         is possible to use the formula:
  *         Current(digit) = [Current(Amp) * 65536 * Rshunt * Aop]  /  Vdd micro
  */
__weak int16_t STC_CalcTorqueReference( SpeednTorqCtrl_Handle_t * pHandle )
{
  int32_t wCurrentReference;
  int16_t hTorqueReference = 0;
  int16_t hMeasuredSpeed;
  int16_t hTargetSpeed;
  int16_t hError;

  if ( pHandle->Mode == STC_TORQUE_MODE )
  {
    wCurrentReference = pHandle->TorqueRef;
  }
  else
  {
    wCurrentReference = pHandle->SpeedRefUnitExt;
  }

  /* Update the speed reference or the torque reference according to the mode
     and terminates the ramp if needed. */
  if ( pHandle->RampRemainingStep > 1u )
  {
    /* Increment/decrement the reference value. */
    wCurrentReference += pHandle->IncDecAmount;

    /* Decrement the number of remaining steps */
    pHandle->RampRemainingStep--;
  }
  else if ( pHandle->RampRemainingStep == 1u )
  {
    /* Set the backup value of hTargetFinal. */
    wCurrentReference = ( int32_t )pHandle->TargetFinal * 65536;
    pHandle->RampRemainingStep = 0u;
  }
  else
  {
    /* Do nothing. */
  }

  if ( pHandle->Mode == STC_SPEED_MODE )
  {
    /* Run the speed control loop */

    /* Compute speed error */
    hTargetSpeed = ( int16_t )( wCurrentReference / 65536 );
    hMeasuredSpeed = SPD_GetAvrgMecSpeedUnit( pHandle->SPD );
    hError = hTargetSpeed - hMeasuredSpeed;
    hTorqueReference = PI_Controller( pHandle->PISpeed, ( int32_t )hError );

    pHandle->SpeedRefUnitExt = wCurrentReference;
    pHandle->TorqueRef = ( int32_t )hTorqueReference * 65536;
  }
  else
  {
    pHandle->TorqueRef = wCurrentReference;
    hTorqueReference = ( int16_t )( wCurrentReference / 65536 );
  }

  return hTorqueReference;
}

/**
  * @brief  Get the Default mechanical rotor speed reference expressed in tenths
  *         of HZ.
  * @param  pHandle: handler of the current instance of the SpeednTorqCtrl component
  * @retval int16_t It returns the Default mechanical rotor speed. reference
  *         expressed in tenths of HZ.
  */
__weak int16_t STC_GetMecSpeedRefUnitDefault( SpeednTorqCtrl_Handle_t * pHandle )
{
  return pHandle->MecSpeedRefUnitDefault;
}

/**
  * @brief  Returns the Application maximum positive value of rotor speed. Expressed in the unit defined by #SPEED_UNIT.
  * @param  pHandle: handler of the current instance of the SpeednTorqCtrl component
  */
__weak uint16_t STC_GetMaxAppPositiveMecSpeedUnit( SpeednTorqCtrl_Handle_t * pHandle )
{
  return pHandle->MaxAppPositiveMecSpeedUnit;
}

/**
  * @brief  Returns the Application minimum negative value of rotor speed. Expressed in the unit defined by #SPEED_UNIT.
  * @param  pHandle: handler of the current instance of the SpeednTorqCtrl component
  */
__weak int16_t STC_GetMinAppNegativeMecSpeedUnit( SpeednTorqCtrl_Handle_t * pHandle )
{
  return pHandle->MinAppNegativeMecSpeedUnit;
}

/**
  * @brief  Check if the settled speed or torque ramp has been completed.
  * @param  pHandle: handler of the current instance of the SpeednTorqCtrl component
  * @retval bool It returns true if the ramp is completed, false otherwise.
  */
__weak bool STC_RampCompleted( SpeednTorqCtrl_Handle_t * pHandle )
{
  bool retVal = false;
  if ( pHandle->RampRemainingStep == 0u )
  {
    retVal = true;
  }
  return retVal;
}

/**
  * @brief  Stop the execution of speed ramp.
  * @param  pHandle: handler of the current instance of the SpeednTorqCtrl component
  * @retval bool It returns true if the command is executed, false otherwise.
  */
__weak bool STC_StopSpeedRamp( SpeednTorqCtrl_Handle_t * pHandle )
{
  bool retVal = false;
  if ( pHandle->Mode == STC_SPEED_MODE )
  {
    pHandle->RampRemainingStep = 0u;
    retVal = true;
  }
  return retVal;
}

/**
  * @brief It returns the default values of Iqdref.
  * @param  pHandle: handler of the current instance of the SpeednTorqCtrl component
  * @retval default values of Iqdref.
  */
__weak qd_t STC_GetDefaultIqdref( SpeednTorqCtrl_Handle_t * pHandle )
{
  qd_t IqdRefDefault;
  IqdRefDefault.q = pHandle->TorqueRefDefault;
  IqdRefDefault.d = pHandle->IdrefDefault;
  return IqdRefDefault;
}

/**
  * @brief  Change the nominal current .
  * @param  pHandle: handler of the current instance of the SpeednTorqCtrl component
  * @param  hNominalCurrent This value represents actually the maximum Iq current
            expressed in digit.
  * @retval none
  */
__weak void STC_SetNominalCurrent( SpeednTorqCtrl_Handle_t * pHandle, uint16_t hNominalCurrent )
{
  pHandle->MaxPositiveTorque = hNominalCurrent;
  pHandle->MinNegativeTorque = -hNominalCurrent;
}

/**
  * @brief  Force the speed reference to the curren speed. It is used
  *         at the START_RUN state to initialize the speed reference.
  * @param  pHandle: handler of the current instance of the SpeednTorqCtrl component
  * @retval none
  */
__weak void STC_ForceSpeedReferenceToCurrentSpeed( SpeednTorqCtrl_Handle_t * pHandle )
{
  pHandle->SpeedRefUnitExt = ( int32_t )SPD_GetAvrgMecSpeedUnit( pHandle->SPD ) * ( int32_t )65536;
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
