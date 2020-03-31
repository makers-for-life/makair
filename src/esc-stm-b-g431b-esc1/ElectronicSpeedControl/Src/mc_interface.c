
/**
  ******************************************************************************
  * @file    mc_interface.c
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file provides firmware functions that implement the features
  *          of the MC Interface component of the Motor Control SDK:
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
#include "mc_math.h"
#include "speed_torq_ctrl.h"

#include "mc_interface.h"

/** @addtogroup MCSDK
  * @{
  */

/** @defgroup MCInterface Motor Control Interface
  * @brief MC Interface component of the Motor Control SDK
  *
  * @todo Document the MC Interface "module".
  *
  * @{
  */

/* Private macros ------------------------------------------------------------*/
/**
  * @brief This macro converts the exported enum from the state machine to the corresponding bit field.
  */
#define BC(state) (1u<<((uint16_t)((uint8_t)(state))))

/* Functions -----------------------------------------------*/

/**
  * @brief  Initializes all the object variables, usually it has to be called
  *         once right after object creation. It is also used to assign the
  *         state machine object, the speed and torque controller, and the FOC
  *         drive object to be used by MC Interface.
  * @param  pHandle pointer on the component instance to initialize.
  * @param  pSTM the state machine object used by the MCI.
  * @param  pSTC the speed and torque controller used by the MCI.
  * @param  pFOCVars pointer to FOC vars to be used by MCI.
  * @retval none.
  */
__weak void MCI_Init( MCI_Handle_t * pHandle, STM_Handle_t * pSTM, SpeednTorqCtrl_Handle_t * pSTC, pFOCVars_t pFOCVars )
{
  pHandle->pSTM = pSTM;
  pHandle->pSTC = pSTC;
  pHandle->pFOCVars = pFOCVars;

  /* Buffer related initialization */
  pHandle->lastCommand = MCI_NOCOMMANDSYET;
  pHandle->hFinalSpeed = 0;
  pHandle->hFinalTorque = 0;
  pHandle->hDurationms = 0;
  pHandle->CommandState = MCI_BUFFER_EMPTY;
}

/**
  * @brief  This is a buffered command to set a motor speed ramp. This commands
  *         don't become active as soon as it is called but it will be executed
  *         when the pSTM state is START_RUN or RUN. User can check the status
  *         of the command calling the MCI_IsCommandAcknowledged method.
  * @param  pHandle Pointer on the component instance to operate on.
  * @param  hFinalSpeed is the value of mechanical rotor speed reference at the
  *         end of the ramp expressed in tenths of HZ.
  * @param  hDurationms the duration of the ramp expressed in milliseconds. It
  *         is possible to set 0 to perform an instantaneous change in the
  *         value.
  * @retval none.
  */
__weak void MCI_ExecSpeedRamp( MCI_Handle_t * pHandle,  int16_t hFinalSpeed, uint16_t hDurationms )
{
  pHandle->lastCommand = MCI_EXECSPEEDRAMP;
  pHandle->hFinalSpeed = hFinalSpeed;
  pHandle->hDurationms = hDurationms;
  pHandle->CommandState = MCI_COMMAND_NOT_ALREADY_EXECUTED;
  pHandle->LastModalitySetByUser = STC_SPEED_MODE;
}

/**
  * @brief  This is a buffered command to set a motor torque ramp. This commands
  *         don't become active as soon as it is called but it will be executed
  *         when the pSTM state is START_RUN or RUN. User can check the status
  *         of the command calling the MCI_IsCommandAcknowledged method.
  * @param  pHandle Pointer on the component instance to work on.
  * @param  hFinalTorque is the value of motor torque reference at the end of
  *         the ramp. This value represents actually the Iq current expressed in
  *         digit.
  *         To convert current expressed in Amps to current expressed in digit
  *         is possible to use the formula:
  *         Current (digit) = [Current(Amp) * 65536 * Rshunt * Aop] / Vdd micro.
  * @param  hDurationms the duration of the ramp expressed in milliseconds. It
  *         is possible to set 0 to perform an instantaneous change in the
  *         value.
  * @retval none.
  */
__weak void MCI_ExecTorqueRamp( MCI_Handle_t * pHandle,  int16_t hFinalTorque, uint16_t hDurationms )
{
  pHandle->lastCommand = MCI_EXECTORQUERAMP;
  pHandle->hFinalTorque = hFinalTorque;
  pHandle->hDurationms = hDurationms;
  pHandle->CommandState = MCI_COMMAND_NOT_ALREADY_EXECUTED;
  pHandle->LastModalitySetByUser = STC_TORQUE_MODE;
}

/**
  * @brief  This is a buffered command to set directly the motor current
  *         references Iq and Id. This commands don't become active as soon as
  *         it is called but it will be executed when the pSTM state is
  *         START_RUN or RUN. User can check the status of the command calling
  *         the MCI_IsCommandAcknowledged method.
  * @param  pHandle Pointer on the component instance to work on.
  * @param  Iqdref current references on qd reference frame in qd_t
  *         format.
  * @retval none.
  */
__weak void MCI_SetCurrentReferences( MCI_Handle_t * pHandle, qd_t Iqdref )
{
  pHandle->lastCommand = MCI_SETCURRENTREFERENCES;
  pHandle->Iqdref.q = Iqdref.q;
  pHandle->Iqdref.d = Iqdref.d;
  pHandle->CommandState = MCI_COMMAND_NOT_ALREADY_EXECUTED;
  pHandle->LastModalitySetByUser = STC_TORQUE_MODE;
}

/**
  * @brief  This is a user command used to begin the start-up procedure.
  *         If the state machine is in IDLE state the command is executed
  *         instantaneously otherwise the command is discarded. User must take
  *         care of this possibility by checking the return value.
  *         Before calling MCI_StartMotor it is mandatory to execute one of
  *         these commands:\n
  *         MCI_ExecSpeedRamp\n
  *         MCI_ExecTorqueRamp\n
  *         MCI_SetCurrentReferences\n
  *         Otherwise the behaviour in run state will be unpredictable.\n
  *         <B>Note:</B> The MCI_StartMotor command is used just to begin the
  *         start-up procedure moving the state machine from IDLE state to
  *         IDLE_START. The command MCI_StartMotor is not blocking the execution
  *         of project until the motor is really running; to do this, the user
  *         have to check the state machine and verify that the RUN state (or
  *         any other state) has been reached.
  * @param  pHandle Pointer on the component instance to work on.
  * @retval bool It returns true if the command is successfully executed
  *         otherwise it return false.
  */
__weak bool MCI_StartMotor( MCI_Handle_t * pHandle )
{
  bool RetVal = STM_NextState( pHandle->pSTM, IDLE_START );

  if ( RetVal == true )
  {
    pHandle->CommandState = MCI_COMMAND_NOT_ALREADY_EXECUTED;
  }

  return RetVal;
}

/**
  * @brief  This is a user command used to begin the stop motor procedure.
  *         If the state machine is in RUN or START states the command is
  *         executed instantaneously otherwise the command is discarded. User
  *         must take care of this possibility by checking the return value.\n
  *         <B>Note:</B> The MCI_StopMotor command is used just to begin the
  *         stop motor procedure moving the state machine to ANY_STOP.
  *         The command MCI_StopMotor is not blocking the execution of project
  *         until the motor is really stopped; to do this, the user have to
  *         check the state machine and verify that the IDLE state has been
  *         reached again.
  * @param  pHandle Pointer on the component instance to work on.
  * @retval bool It returns true if the command is successfully executed
  *         otherwise it return false.
  */
__weak bool MCI_StopMotor( MCI_Handle_t * pHandle )
{
  return STM_NextState( pHandle->pSTM, ANY_STOP );
}

/**
  * @brief  This is a user command used to indicate that the user has seen the
  *         error condition. If is possible, the command is executed
  *         instantaneously otherwise the command is discarded. User must take
  *         care of this possibility by checking the return value.
  * @param  pHandle Pointer on the component instance to work on.
  * @retval bool It returns true if the command is successfully executed
  *         otherwise it return false.
  */
__weak bool MCI_FaultAcknowledged( MCI_Handle_t * pHandle )
{
  return STM_FaultAcknowledged( pHandle->pSTM );
}

/**
  * @brief  This is a user command used to begin the encoder alignment procedure.
  *         If the state machine is in IDLE state the command is executed
  *         instantaneously otherwise the command is discarded. User must take
  *         care of this possibility by checking the return value.\n
  *         <B>Note:</B> The MCI_EncoderAlign command is used just to begin the
  *         encoder alignment procedure moving the state machine from IDLE state
  *         to IDLE_ALIGNMENT. The command MCI_EncoderAlign is not blocking the
  *         execution of project until the encoder is really calibrated; to do
  *         this, the user have to check the state machine and verify that the
  *         IDLE state has been reached again.
  * @param  pHandle Pointer on the component instance to work on.
  * @retval bool It returns true if the command is successfully executed
  *         otherwise it return false.
  */
__weak bool MCI_EncoderAlign( MCI_Handle_t * pHandle )
{
  return STM_NextState( pHandle->pSTM, IDLE_ALIGNMENT );
}

/**
  * @brief  This is usually a method managed by task. It must be called
  *         periodically in order to check the status of the related pSTM object
  *         and eventually to execute the buffered command if the condition
  *         occurs.
  * @param  pHandle Pointer on the component instance to work on.
  * @retval none.
  */
__weak void MCI_ExecBufferedCommands( MCI_Handle_t * pHandle )
{
  if ( pHandle != MC_NULL )
  {
    if ( pHandle->CommandState == MCI_COMMAND_NOT_ALREADY_EXECUTED )
    {
      bool commandHasBeenExecuted = false;
      switch ( pHandle->lastCommand )
      {
        case MCI_EXECSPEEDRAMP:
        {
          pHandle->pFOCVars->bDriveInput = INTERNAL;
          STC_SetControlMode( pHandle->pSTC, STC_SPEED_MODE );
          commandHasBeenExecuted = STC_ExecRamp( pHandle->pSTC, pHandle->hFinalSpeed, pHandle->hDurationms );
        }
        break;
        case MCI_EXECTORQUERAMP:
        {
          pHandle->pFOCVars->bDriveInput = INTERNAL;
          STC_SetControlMode( pHandle->pSTC, STC_TORQUE_MODE );
          commandHasBeenExecuted = STC_ExecRamp( pHandle->pSTC, pHandle->hFinalTorque, pHandle->hDurationms );
        }
        break;
        case MCI_SETCURRENTREFERENCES:
        {
          pHandle->pFOCVars->bDriveInput = EXTERNAL;
          pHandle->pFOCVars->Iqdref = pHandle->Iqdref;
          commandHasBeenExecuted = true;
        }
        break;
        default:
          break;
      }

      if ( commandHasBeenExecuted )
      {
        pHandle->CommandState = MCI_COMMAND_EXECUTED_SUCCESFULLY;
      }
      else
      {
        pHandle->CommandState = MCI_COMMAND_EXECUTED_UNSUCCESFULLY;
      }
    }
  }
}

/**
  * @brief  It returns information about the state of the last buffered command.
  * @param  pHandle Pointer on the component instance to work on.
  * @retval CommandState_t  It can be one of the following codes:
  *         - MCI_BUFFER_EMPTY if no buffered command has been called.
  *         - MCI_COMMAND_NOT_ALREADY_EXECUTED if the buffered command
  *         condition hasn't already occurred.
  *         - MCI_COMMAND_EXECUTED_SUCCESFULLY if the buffered command has
  *         been executed successfully. In this case calling this function reset
  *         the command state to BC_BUFFER_EMPTY.
  *         - MCI_COMMAND_EXECUTED_UNSUCCESFULLY if the buffered command has
  *         been executed unsuccessfully. In this case calling this function
  *         reset the command state to BC_BUFFER_EMPTY.
  */
__weak MCI_CommandState_t  MCI_IsCommandAcknowledged( MCI_Handle_t * pHandle )
{
  MCI_CommandState_t retVal = pHandle->CommandState;

  if ( ( retVal == MCI_COMMAND_EXECUTED_SUCCESFULLY ) |
       ( retVal == MCI_COMMAND_EXECUTED_UNSUCCESFULLY ) )
  {
    pHandle->CommandState = MCI_BUFFER_EMPTY;
  }
  return retVal;
}

/**
  * @brief  It returns information about the state of the related pSTM object.
  * @param  pHandle Pointer on the component instance to work on.
  * @retval State_t It returns the current state of the related pSTM object.
  */
__weak State_t  MCI_GetSTMState( MCI_Handle_t * pHandle )
{
  return STM_GetState( pHandle->pSTM );
}

/**
  * @brief It returns a 16 bit fields containing information about faults
  *        historically occurred since the state machine has been moved into
  *        FAULT_NOW state.
  * \n\link Fault_generation_error_codes Returned error codes are listed here \endlink
  * @param pHandle Pointer on the component instance to work on.
  * @retval uint16_t  16 bit fields with information about the faults
  *         historically occurred since the state machine has been moved into
  *         FAULT_NOW state.
  * \n\link Fault_generation_error_codes Returned error codes are listed here \endlink
  */
__weak uint16_t MCI_GetOccurredFaults( MCI_Handle_t * pHandle )
{
  return ( uint16_t )( STM_GetFaultState( pHandle->pSTM ) );
}

/**
  * @brief It returns a 16 bit fields containing information about faults
  *        currently present.
  * \n\link Fault_generation_error_codes Returned error codes are listed here \endlink
  * @param pHandle Pointer on the component instance to work on.
  * @retval uint16_t  16 bit fields with information about about currently
  *         present faults.
  * \n\link Fault_generation_error_codes Returned error codes are listed here \endlink
  */
__weak uint16_t MCI_GetCurrentFaults( MCI_Handle_t * pHandle )
{
  return ( uint16_t )( STM_GetFaultState( pHandle->pSTM ) >> 16 );
}

/**
  * @brief  It returns the modality of the speed and torque controller.
  * @param  pHandle Pointer on the component instance to work on.
  * @retval STC_Modality_t It returns the modality of STC. It can be one of
  *         these two values: STC_TORQUE_MODE or STC_SPEED_MODE.
  */
__weak STC_Modality_t MCI_GetControlMode( MCI_Handle_t * pHandle )
{
  return pHandle->LastModalitySetByUser;
}

/**
  * @brief  It returns the motor direction imposed by the last command
  *         (MCI_ExecSpeedRamp, MCI_ExecTorqueRamp or MCI_SetCurrentReferences).
  * @param  pHandle Pointer on the component instance to work on.
  * @retval int16_t It returns 1 or -1 according the sign of hFinalSpeed,
  *         hFinalTorque or Iqdref.q of the last command.
  */
__weak int16_t MCI_GetImposedMotorDirection( MCI_Handle_t * pHandle )
{
  int16_t retVal = 1;

  switch ( pHandle->lastCommand )
  {
    case MCI_EXECSPEEDRAMP:
      if ( pHandle->hFinalSpeed < 0 )
      {
        retVal = -1;
      }
      break;
    case MCI_EXECTORQUERAMP:
      if ( pHandle->hFinalTorque < 0 )
      {
        retVal = -1;
      }
      break;
    case MCI_SETCURRENTREFERENCES:
      if ( pHandle->Iqdref.q < 0 )
      {
        retVal = -1;
      }
      break;
    default:
      break;
  }
  return retVal;
}

/**
  * @brief  It returns information about the last ramp final speed sent by the
  *         user expressed in tenths of HZ.
  * @param  pHandle Pointer on the component instance to work on.
  * @retval int16_t last ramp final speed sent by the user expressed in tehts
  *         of HZ.
  */
__weak int16_t MCI_GetLastRampFinalSpeed( MCI_Handle_t * pHandle )
{
  int16_t hRetVal = 0;

  /* Examine the last buffered commands */
  if ( pHandle->lastCommand == MCI_EXECSPEEDRAMP )
  {
    hRetVal = pHandle->hFinalSpeed;
  }
  return hRetVal;
}

/**
  * @brief  Check if the settled speed or torque ramp has been completed.
  * @param  pHandle Pointer on the component instance to work on.
  * @retval bool It returns true if the ramp is completed, false otherwise.
  */
__weak bool MCI_RampCompleted( MCI_Handle_t * pHandle )
{
  bool retVal = false;

  if ( ( STM_GetState( pHandle->pSTM ) ) == RUN )
  {
    retVal = STC_RampCompleted( pHandle->pSTC );
  }

  return retVal;
}

/**
  * @brief  Stop the execution of speed ramp.
  * @param  pHandle Pointer on the component instance to work on.
  * @retval bool It returns true if the command is executed, false otherwise.
  */
__weak bool MCI_StopSpeedRamp( MCI_Handle_t * pHandle )
{
  return STC_StopSpeedRamp( pHandle->pSTC );
}

/**
  * @brief  Stop the execution of ongoing ramp.
  * @param  pHandle Pointer on the component instance to work on.
  */
__weak void MCI_StopRamp( MCI_Handle_t * pHandle)
{
   STC_StopRamp( pHandle->pSTC );
}

/**
  * @brief  It returns speed sensor reliability with reference to the sensor
  *         actually used for reference frame transformation
  * @param  pHandle Pointer on the component instance to work on.
  * @retval bool It returns true if the speed sensor utilized for reference
  *         frame transformation and (in speed control mode) for speed
  *         regulation is reliable, false otherwise
  */
__weak bool MCI_GetSpdSensorReliability( MCI_Handle_t * pHandle )
{
  SpeednPosFdbk_Handle_t * SpeedSensor = STC_GetSpeedSensor( pHandle->pSTC );

  return ( SPD_Check( SpeedSensor ) );
}

/**
  * @brief  Returns the last computed average mechanical speed, expressed in
  *         the unit defined by #SPEED_UNIT and related to the sensor actually
  *         used by FOC algorithm
  * @param  pHandle Pointer on the component instance to work on.
  */
__weak int16_t MCI_GetAvrgMecSpeedUnit( MCI_Handle_t * pHandle )
{
  SpeednPosFdbk_Handle_t * SpeedSensor = STC_GetSpeedSensor( pHandle->pSTC );

  return ( SPD_GetAvrgMecSpeedUnit( SpeedSensor ) );
}

/**
  * @brief  Returns the current mechanical rotor speed reference expressed in the unit defined by #SPEED_UNIT
  *         
  * @param  pHandle Pointer on the component instance to work on.
  *         
  */
__weak int16_t MCI_GetMecSpeedRefUnit( MCI_Handle_t * pHandle )
{
  return ( STC_GetMecSpeedRefUnit( pHandle->pSTC ) );
}

/**
  * @brief  It returns stator current Iab in ab_t format
  * @param  pHandle Pointer on the component instance to work on.
  * @retval ab_t Stator current Iab
  */
__weak ab_t MCI_GetIab( MCI_Handle_t * pHandle )
{
  return ( pHandle->pFOCVars->Iab );
}

/**
  * @brief  It returns stator current Ialphabeta in alphabeta_t format
  * @param  pHandle Pointer on the component instance to work on.
  * @retval alphabeta_t Stator current Ialphabeta
  */
__weak alphabeta_t MCI_GetIalphabeta( MCI_Handle_t * pHandle )
{
  return ( pHandle->pFOCVars->Ialphabeta );
}

/**
  * @brief  It returns stator current Iqd in qd_t format
  * @param  pHandle Pointer on the component instance to work on.
  * @retval qd_t Stator current Iqd
  */
__weak qd_t MCI_GetIqd( MCI_Handle_t * pHandle )
{
  return ( pHandle->pFOCVars->Iqd );
}

/**
  * @brief  It returns stator current IqdHF in qd_t format
  * @param  pHandle Pointer on the component instance to work on.
  * @retval qd_t Stator current IqdHF if HFI is selected as main
  *         sensor. Otherwise it returns { 0, 0}.
  */
__weak qd_t MCI_GetIqdHF( MCI_Handle_t * pHandle )
{
  return ( pHandle->pFOCVars->IqdHF );
}

/**
  * @brief  It returns stator current Iqdref in qd_t format
  * @param  pHandle Pointer on the component instance to work on.
  * @retval qd_t Stator current Iqdref
  */
__weak qd_t MCI_GetIqdref( MCI_Handle_t * pHandle )
{
  return ( pHandle->pFOCVars->Iqdref );
}

/**
  * @brief  It returns stator current Vqd in qd_t format
  * @param  pHandle Pointer on the component instance to work on.
  * @retval qd_t Stator current Vqd
  */
__weak qd_t MCI_GetVqd( MCI_Handle_t * pHandle )
{
  return ( pHandle->pFOCVars->Vqd );
}

/**
  * @brief  It returns stator current Valphabeta in alphabeta_t format
  * @param  pHandle Pointer on the component instance to work on.
  * @retval alphabeta_t Stator current Valphabeta
  */
__weak alphabeta_t MCI_GetValphabeta( MCI_Handle_t * pHandle )
{
  return ( pHandle->pFOCVars->Valphabeta );
}

/**
  * @brief  It returns the rotor electrical angle actually used for reference
  *         frame transformation
  * @param  pHandle Pointer on the component instance to work on.
  * @retval int16_t Rotor electrical angle in dpp format
  */
__weak int16_t MCI_GetElAngledpp( MCI_Handle_t * pHandle )
{
  return ( pHandle->pFOCVars->hElAngle );
}

/**
  * @brief  It returns the reference eletrical torque, fed to derived class for
  *         Iqref and Idref computation
  * @param  pHandle Pointer on the component instance to work on.
  * @retval int16_t Teref
  */
__weak int16_t MCI_GetTeref( MCI_Handle_t * pHandle )
{
  return ( pHandle->pFOCVars->hTeref );
}

/**
  * @brief  It returns the motor phase current amplitude (0-to-peak) in s16A
  *         To convert s16A into Ampere following formula must be used:
  *         Current(Amp) = [Current(s16A) * Vdd micro] / [65536 * Rshunt * Aop]
  * @param  pHandle Pointer on the component instance to work on.
  * @retval int16_t Motor phase current (0-to-peak) in s16A
  */
__weak int16_t MCI_GetPhaseCurrentAmplitude( MCI_Handle_t * pHandle )
{
  alphabeta_t Local_Curr;
  int32_t wAux1, wAux2;

  Local_Curr = pHandle->pFOCVars->Ialphabeta;
  wAux1 = ( int32_t )( Local_Curr.alpha ) * Local_Curr.alpha;
  wAux2 = ( int32_t )( Local_Curr.beta ) * Local_Curr.beta;

  wAux1 += wAux2;
  wAux1 = MCM_Sqrt( wAux1 );

  if ( wAux1 > INT16_MAX )
  {
    wAux1 = ( int32_t ) INT16_MAX;
  }

  return ( ( int16_t )wAux1 );
}

/**
  * @brief  It returns the applied motor phase voltage amplitude (0-to-peak) in
  *         s16V. To convert s16V into Volts following formula must be used:
  *         PhaseVoltage(V) = [PhaseVoltage(s16A) * Vbus(V)] /[sqrt(3) *32767]
  * @param  pHandle Pointer on the component instance to work on.
  * @retval int16_t Motor phase voltage (0-to-peak) in s16V
  */
__weak int16_t MCI_GetPhaseVoltageAmplitude( MCI_Handle_t * pHandle )
{
  alphabeta_t Local_Voltage;
  int32_t wAux1, wAux2;

  Local_Voltage = pHandle->pFOCVars->Valphabeta;
  wAux1 = ( int32_t )( Local_Voltage.alpha ) * Local_Voltage.alpha;
  wAux2 = ( int32_t )( Local_Voltage.beta ) * Local_Voltage.beta;

  wAux1 += wAux2;
  wAux1 = MCM_Sqrt( wAux1 );

  if ( wAux1 > INT16_MAX )
  {
    wAux1 = ( int32_t ) INT16_MAX;
  }

  return ( ( int16_t ) wAux1 );
}

/**
  * @brief  When bDriveInput is set to INTERNAL, Idref should is normally managed
  *         by FOC_CalcCurrRef. Neverthless, this method allows forcing changing
  *         Idref value. Method call has no effect when either flux weakening
  *         region is entered or MTPA is enabled
  * @param  pHandle Pointer on the component instance to work on.
  * @param  int16_t New target Id value
  * @retval none
  */
__weak void MCI_SetIdref( MCI_Handle_t * pHandle, int16_t hNewIdref )
{
  pHandle->pFOCVars->Iqdref.d = hNewIdref;
  pHandle->pFOCVars->UserIdref = hNewIdref;
}

/**
  * @brief  It re-initializes Iqdref variables with their default values.
  * @param  pHandle Pointer on the component instance to work on.
  * @retval none
  */
__weak void MCI_Clear_Iqdref( MCI_Handle_t * pHandle )
{
  pHandle->pFOCVars->Iqdref = STC_GetDefaultIqdref( pHandle->pSTC );
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
