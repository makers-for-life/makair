/**
 ******************************************************************************
 * @file    trajectory_ctrl.h
 * @author  Motor Control SDK Team, ST Microelectronics
 * @brief   This file provides all definitions and functions prototypes
 *          of the Position Control component of the Motor Control SDK.
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
 * @ingroup TrajectoryCtrl
 */

/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __TRAJCTRL_H
#define __TRAJCTRL_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "mc_type.h"
#include "speed_torq_ctrl.h"
#include "enc_align_ctrl.h"

#define RADTOS16 10430.378350470452725f            /* 2^15/Pi */

#ifndef M_PI
  #define M_PI 3.14159265358979323846
#endif

#define Z_ALIGNMENT_DURATION	2	                    /* 2 seconds */
#define Z_ALIGNMENT_NB_ROTATION	(2.0f * M_PI)	        /* 1 turn in 2 seconds allowed to find the "Z" signal  */

/** @addtogroup MCSDK
  * @{
  */

/** @addtogroup PosCtrl
  * @{
  */

typedef enum {
    TC_READY_FOR_COMMAND  = 0,
    TC_MOVEMENT_ON_GOING = 1,
    TC_TARGET_POSITION_REACHED = 2,
    TC_FOLLOWING_ON_GOING = 3
} PosCtrlStatus_t;

typedef enum {
    TC_AWAITING_FOR_ALIGNMENT = 0,
    TC_ZERO_ALIGNMENT_START = 1,
    TC_ALIGNMENT_COMPLETED = 2,
    TC_ABSOLUTE_ALIGNMENT_NOT_SUPPORTED = 3,   /* Encoder sensor without "Z" output signal */
    TC_ABSOLUTE_ALIGNMENT_SUPPORTED = 4,
    TC_ALIGNMENT_ERROR = 5,
} AlignStatus_t;

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  Position Control handle definition
  */
typedef struct
{
  float MovementDuration;
  float StartingAngle;
  float FinalAngle;
  float AngleStep;
  float SubStep[6];
  float SubStepDuration;   /* Sub step time duration of sequence : acceleration / Cruise / Deceleration */
  float ElapseTime;
  float SamplingTime;
  float Jerk;
  float CruiseSpeed;
  float Acceleration;
  float Omega;
  float OmegaPrev;
  float Theta;
  float ThetaPrev;
  uint8_t ReceivedTh;
  bool PositionControlRegulation;
  bool EncoderAbsoluteAligned;
  int16_t MecAngleOffset;
  uint32_t TcTick;
  float SysTickPeriod;

  PosCtrlStatus_t PositionCtrlStatus;
  AlignStatus_t AlignmentCfg;
  AlignStatus_t AlignmentStatus;

  ENCODER_Handle_t *pENC;
  SpeednTorqCtrl_Handle_t * pSTC;
  PID_Handle_t * PIDPosRegulator;
} PosCtrl_Handle_t;

void TC_Init(PosCtrl_Handle_t *pHandle, PID_Handle_t * pPIDPosReg, SpeednTorqCtrl_Handle_t * pSTC, ENCODER_Handle_t * pENC);
bool TC_MoveCommand  (PosCtrl_Handle_t *pHandle, float startingAngle, float angleStep, float movementDuration);
void TC_FollowCommand(PosCtrl_Handle_t *pHandle, float Angle);
void TC_PositionRegulation(PosCtrl_Handle_t *pHandle);
void TC_MoveExecution  (PosCtrl_Handle_t *pHandle);
void TC_FollowExecution(PosCtrl_Handle_t *pHandle);
void TC_EncAlignmentCommand(PosCtrl_Handle_t *pHandle);
bool TC_RampCompleted(PosCtrl_Handle_t *pHandle);
void TC_EncoderReset(PosCtrl_Handle_t *pHandle);
float TC_GetCurrentPosition(PosCtrl_Handle_t *pHandle);
float TC_GetTargetPosition(PosCtrl_Handle_t *pHandle);
float TC_GetMoveDuration(PosCtrl_Handle_t *pHandle);
PosCtrlStatus_t TC_GetControlPositionStatus(PosCtrl_Handle_t *pHandle);
AlignStatus_t TC_GetAlignmentStatus(PosCtrl_Handle_t *pHandle);
void TC_IncTick(PosCtrl_Handle_t *pHandle);

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif /* __cpluplus */

#endif /* __TRAJCTRL_H */

/************************ (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
