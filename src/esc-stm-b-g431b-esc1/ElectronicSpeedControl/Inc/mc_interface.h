/**
  ******************************************************************************
  * @file    mc_interface.h
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file contains all definitions and functions prototypes for the
  *          MC Interface component of the Motor Control SDK.
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
  * @ingroup MCInterface
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MC_INTERFACE_H
#define __MC_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "mc_type.h"
#include "state_machine.h"
#include "speed_torq_ctrl.h"
/** @addtogroup MCSDK
  * @{
  */

/** @addtogroup MCInterface
  * @{
  */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  MCI_BUFFER_EMPTY,                  /*!< If no buffered command has been
                                            called.*/
  MCI_COMMAND_NOT_ALREADY_EXECUTED,  /*!< If the buffered command condition
                                            hasn't already occurred.*/
  MCI_COMMAND_EXECUTED_SUCCESFULLY,  /*!< If the buffered command has been
                                            executed successfully.*/
  MCI_COMMAND_EXECUTED_UNSUCCESFULLY /*!< If the buffered command has been
                                            executed unsuccessfully.*/
} MCI_CommandState_t ;

typedef enum
{
  MCI_NOCOMMANDSYET,        /*!< No command has been set by the user.*/
  MCI_EXECSPEEDRAMP,        /*!< ExecSpeedRamp command coming from the user.*/
  MCI_EXECTORQUERAMP,       /*!< ExecTorqueRamp command coming from the user.*/
  MCI_SETCURRENTREFERENCES, /*!< SetCurrentReferences command coming from the
                                 user.*/
} MCI_UserCommands_t;

typedef struct
{
  STM_Handle_t * pSTM; /*!< State machine object used by MCI.*/
  SpeednTorqCtrl_Handle_t * pSTC; /*!< Speed and torque controller object used by MCI.*/
  pFOCVars_t pFOCVars;    /*!< Pointer to FOC vars used by MCI.*/
  MCI_UserCommands_t lastCommand; /*!< Last command coming from the user.*/
  int16_t hFinalSpeed;        /*!< Final speed of last ExecSpeedRamp command.*/
  int16_t hFinalTorque;       /*!< Final torque of last ExecTorqueRamp
                                   command.*/
  qd_t Iqdref;     /*!< Current component of last
                                   SetCurrentReferences command.*/
  uint16_t hDurationms;       /*!< Duration in ms of last ExecSpeedRamp or
                                   ExecTorqueRamp command.*/
  
  MCI_CommandState_t CommandState; /*!< The status of the buffered command.*/
  STC_Modality_t LastModalitySetByUser; /*!< The last STC_Modality_t set by the
                                             user. */
} MCI_Handle_t;

/* Exported functions ------------------------------------------------------- */
void MCI_Init( MCI_Handle_t * pHandle, STM_Handle_t * pSTM, SpeednTorqCtrl_Handle_t * pSTC, pFOCVars_t pFOCVars );
void MCI_ExecBufferedCommands( MCI_Handle_t * pHandle );
void MCI_ExecSpeedRamp( MCI_Handle_t * pHandle,  int16_t hFinalSpeed, uint16_t hDurationms );
void MCI_ExecTorqueRamp( MCI_Handle_t * pHandle,  int16_t hFinalTorque, uint16_t hDurationms );
void MCI_SetCurrentReferences( MCI_Handle_t * pHandle, qd_t Iqdref );

bool MCI_StartMotor( MCI_Handle_t * pHandle );
bool MCI_StopMotor( MCI_Handle_t * pHandle );
bool MCI_FaultAcknowledged( MCI_Handle_t * pHandle );
bool MCI_EncoderAlign( MCI_Handle_t * pHandle );
MCI_CommandState_t  MCI_IsCommandAcknowledged( MCI_Handle_t * pHandle );
State_t  MCI_GetSTMState( MCI_Handle_t * pHandle );
uint16_t MCI_GetOccurredFaults( MCI_Handle_t * pHandle );
uint16_t MCI_GetCurrentFaults( MCI_Handle_t * pHandle );
int16_t MCI_GetMecSpeedRefUnit( MCI_Handle_t * pHandle );
int16_t MCI_GetAvrgMecSpeedUnit( MCI_Handle_t * pHandle );
/*int16_t MCI_GetTorque( MCI_Handle_t * pHandle );*/
int16_t MCI_GetPhaseCurrentAmplitude( MCI_Handle_t * pHandle );
int16_t MCI_GetPhaseVoltageAmplitude( MCI_Handle_t * pHandle );
STC_Modality_t MCI_GetControlMode( MCI_Handle_t * pHandle );
int16_t MCI_GetImposedMotorDirection( MCI_Handle_t * pHandle );
int16_t MCI_GetLastRampFinalSpeed( MCI_Handle_t * pHandle );
bool MCI_RampCompleted( MCI_Handle_t * pHandle );
bool MCI_StopSpeedRamp( MCI_Handle_t * pHandle );
void MCI_StopRamp( MCI_Handle_t * pHandle );
bool MCI_GetSpdSensorReliability( MCI_Handle_t * pHandle );
int16_t MCI_GetAvrgMecSpeedUnit( MCI_Handle_t * pHandle );
int16_t MCI_GetMecSpeedRefUnit( MCI_Handle_t * pHandle );
ab_t MCI_GetIab( MCI_Handle_t * pHandle );
alphabeta_t MCI_GetIalphabeta( MCI_Handle_t * pHandle );
qd_t MCI_GetIqd( MCI_Handle_t * pHandle );
qd_t MCI_GetIqdHF( MCI_Handle_t * pHandle );
qd_t MCI_GetIqdref( MCI_Handle_t * pHandle );
qd_t MCI_GetVqd( MCI_Handle_t * pHandle );
alphabeta_t MCI_GetValphabeta( MCI_Handle_t * pHandle );
int16_t MCI_GetElAngledpp( MCI_Handle_t * pHandle );
int16_t MCI_GetTeref( MCI_Handle_t * pHandle );
int16_t MCI_GetPhaseCurrentAmplitude( MCI_Handle_t * pHandle );
int16_t MCI_GetPhaseVoltageAmplitude( MCI_Handle_t * pHandle );
void MCI_SetIdref( MCI_Handle_t * pHandle, int16_t hNewIdref );
void MCI_Clear_Iqdref( MCI_Handle_t * pHandle );

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif /* __cpluplus */

#endif /* __MC_INTERFACE_H */

/******************* (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/

