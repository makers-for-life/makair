/**
  ******************************************************************************
  * @file    ramp_ext_mngr.h
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file contains all definitions and functions prototypes for the
  *          Ramp Extended Manager component of the Motor Control SDK.
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
  * @ingroup RampExtMngr
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RAMPEXTMNGR_H
#define __RAMPEXTMNGR_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "mc_type.h"

#ifdef FASTDIV
#include "fast_div.h"
#endif

/** @addtogroup MCSDK
  * @{
  */

/** @addtogroup RampExtMngr
  * @{
  */

/**
  * @brief  RampExtMngr Handle Definition.
  */
typedef struct
{
  uint32_t FrequencyHz;             /*!< Execution frequency expressed in Hz */
  int32_t  TargetFinal;             /*!< Backup of hTargetFinal to be applied in the
                                         last step.*/
  int32_t  Ext;                     /*!< Current state variable multiplied by 32768.*/
  uint32_t RampRemainingStep;       /*!< Number of steps remaining to complete the
                                         ramp.*/
  int32_t  IncDecAmount;            /*!< Increment/decrement amount to be applied to
                                         the reference value at each
                                         CalcTorqueReference.*/
  uint32_t ScalingFactor;           /*!< Scaling factor between output value and
                                         its internal representation.*/
#ifdef FASTDIV
  /* (Fast division optimization for cortex-M0 micros)*/
  FastDiv_Handle_t fd;                       /*!< Fast division obj.*/
#endif
} RampExtMngr_Handle_t;

/* Exported functions ------------------------------------------------------- */
void REMNG_Init( RampExtMngr_Handle_t * pHandle );
int32_t REMNG_Calc( RampExtMngr_Handle_t * pHandle );
bool REMNG_ExecRamp( RampExtMngr_Handle_t * pHandle, int32_t TargetFinal, uint32_t Durationms );
int32_t REMNG_GetValue( RampExtMngr_Handle_t * pHandle );
bool REMNG_RampCompleted( RampExtMngr_Handle_t * pHandle );
void REMNG_StopRamp( RampExtMngr_Handle_t * pHandle );

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif /* __cpluplus */

#endif /* __RAMPEXTMNGR_H */

/************************ (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/

