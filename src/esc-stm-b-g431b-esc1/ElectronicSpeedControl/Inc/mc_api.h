/**
  ******************************************************************************
  * @file    mc_api.h
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file defines the high level interface of the Motor Control SDK.
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
  * @ingroup MCIAPI
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MC_API_H
#define __MC_API_H

#include "mc_type.h"
#include "mc_interface.h"
#include "state_machine.h"

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

/** @addtogroup MCSDK
  * @{
  */

/** @addtogroup MCIAPI
  * @{
  */

/* Starts Motor 1 */
bool MC_StartMotor1(void);

/* Stops Motor 1 */
bool MC_StopMotor1(void);

/* Programs a Speed ramp for Motor 1 */
void MC_ProgramSpeedRampMotor1( int16_t hFinalSpeed, uint16_t hDurationms );

/* Programs a Torque ramp for Motor 1 */
void MC_ProgramTorqueRampMotor1( int16_t hFinalTorque, uint16_t hDurationms );

/* Programs a current reference for Motor 1 */
void MC_SetCurrentReferenceMotor1( qd_t Iqdref );

/* Returns the state of the last submited command for Motor 1 */
MCI_CommandState_t  MC_GetCommandStateMotor1( void);

/* Stops the execution of the current speed ramp for Motor 1 if any */
bool MC_StopSpeedRampMotor1(void);

/* Stops the execution of the on going ramp for Motor 1 if any */
void MC_StopRampMotor1(void);

/* Returns true if the last submited ramp for Motor 1 has completed, false otherwise */
bool MC_HasRampCompletedMotor1(void);

/* Returns the current mechanical rotor speed reference set for Motor 1, expressed in the unit defined by #SPEED_UNIT */
int16_t MC_GetMecSpeedReferenceMotor1(void);

/* Returns the last computed average mechanical rotor speed for Motor 1, expressed in the unit defined by #SPEED_UNIT */
int16_t MC_GetMecSpeedAverageMotor1(void);

/* Returns the final speed of the last ramp programmed for Motor 1, if this ramp was a speed ramp */
int16_t MC_GetLastRampFinalSpeedMotor1(void);

/* Returns the current Control Mode for Motor 1 (either Speed or Torque) */
STC_Modality_t MC_GetControlModeMotor1(void);

/* Returns the direction imposed by the last command on Motor 1 */
int16_t MC_GetImposedDirectionMotor1(void);

/* Returns the current reliability of the speed sensor used for Motor 1 */
bool MC_GetSpeedSensorReliabilityMotor1(void);

/* returns the amplitude of the phase current injected in Motor 1 */
int16_t MC_GetPhaseCurrentAmplitudeMotor1(void);

/* returns the amplitude of the phase voltage applied to Motor 1 */
int16_t MC_GetPhaseVoltageAmplitudeMotor1(void);

/* returns current Ia and Ib values for Motor 1 */
ab_t MC_GetIabMotor1(void);

/* returns current Ialpha and Ibeta values for Motor 1 */
alphabeta_t MC_GetIalphabetaMotor1(void);

/* returns current Iq and Id values for Motor 1 */
qd_t MC_GetIqdMotor1(void);

/* returns Iq and Id reference values for Motor 1 */
qd_t MC_GetIqdrefMotor1(void);

/* returns current Vq and Vd values for Motor 1 */
qd_t MC_GetVqdMotor1(void);

/* returns current Valpha and Vbeta values for Motor 1 */
alphabeta_t MC_GetValphabetaMotor1(void);

/* returns the electrical angle of the rotor of Motor 1, in DDP format */
int16_t MC_GetElAngledppMotor1(void);

/* returns the current electrical torque reference for Motor 1 */
int16_t MC_GetTerefMotor1(void);

/* Sets the reference value for Id */
void MC_SetIdrefMotor1( int16_t hNewIdref );

/* re-initializes Iq and Id references to their default values */
void MC_Clear_IqdrefMotor1(void);

/* Acknowledge a Motor Control fault on Motor 1 */
bool MC_AcknowledgeFaultMotor1( void );

/* Returns a bitfiled showing faults that occured since the State Machine of Motor 1 was moved to FAULT_NOW state */
uint16_t MC_GetOccurredFaultsMotor1(void);

/* Returns a bitfield showing all current faults on Motor 1 */
uint16_t MC_GetCurrentFaultsMotor1(void);

/* returns the current state of Motor 1 state machine */
State_t  MC_GetSTMStateMotor1(void);

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif /* __cpluplus */

#endif /* __MC_API_H */
/******************* (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
