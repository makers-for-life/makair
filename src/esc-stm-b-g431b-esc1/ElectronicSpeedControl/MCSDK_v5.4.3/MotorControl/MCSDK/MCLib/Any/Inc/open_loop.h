/**
  ******************************************************************************
  * @file    open_loop.h
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file contains all definitions and functions prototypes for the
  *          Open Loop component of the Motor Control SDK.
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
  * @ingroup OpenLoop
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __OPENLOOPCLASS_H
#define __OPENLOOPCLASS_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "mc_type.h"
#include "virtual_speed_sensor.h"

/** @addtogroup MCSDK
  * @{
  */

/** @addtogroup OpenLoop
  * @{
  */

/**
  * @brief  OpenLoop_Handle_t structure used for phases definition
  */
typedef struct
{
  int16_t hDefaultVoltage; /**< Default Open loop phase voltage. */

  bool VFMode;             /**< Flag to enable Voltage versus Frequency mode (V/F mode). */

  int16_t hVFOffset;       /**< Minimum Voltage to apply when frequency is equal to zero. */

  int16_t hVFSlope;        /**< Slope of V/F curve: Voltage = (hVFSlope)*Frequency + hVFOffset. */

  int16_t hVoltage;        /**< Current Open loop phase voltage. */

  VirtualSpeedSensor_Handle_t * pVSS; /**< Allow access on mechanical speed measured. */

} OpenLoop_Handle_t;

/**
* @}
*/

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Initialize OpenLoop variables.
  * @param  pHandle: Pointer on Handle structure of OpenLoop feature.
  * @param  pVSS: Pointer on virtual speed sensor structure.
  *  @retval none
  */
void OL_Init( OpenLoop_Handle_t * pHandle, VirtualSpeedSensor_Handle_t * pVSS );

/**
  * @brief  Set Vqd according to open loop phase voltage.
  * @param  pHandle: Pointer on Handle structure of OpenLoop feature.
  *  @retval Voltage components Vqd conditioned values.
  */
qd_t OL_VqdConditioning( OpenLoop_Handle_t * pHandle );

/**
  * @brief  Allow to set new open loop phase voltage.
  * @param  pHandle: Pointer on Handle structure of OpenLoop feature.
  * @param  hNewVoltage: New voltage value to apply.
  * @retval None
  */
void OL_UpdateVoltage( OpenLoop_Handle_t * pHandle, int16_t hNewVoltage );

/**
  * @brief  Compute phase voltage to apply according to average mechanical speed (V/F Mode).
  * @param  pHandle: Pointer on Handle structure of OpenLoop feature.
  * @retval None
  */
void OL_Calc( OpenLoop_Handle_t * pHandle );

/**
  * @brief  Allow activation of the Voltage versus Frequency mode (V/F mode).
  * @param  pHandle: Pointer on Handle structure of OpenLoop feature.
  * @param  VFEnabling: Flag to enable the V/F mode.
  * @retval None
  */
void OL_VF( OpenLoop_Handle_t * pHandle, bool VFEnabling );

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif /* __cpluplus */

#endif /* __OPENLOOPCLASS_H */

/************************ (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
