/**
  ******************************************************************************
  * @file    open_loop.c
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file provides firmware functions that implement the features
  *          of the Open Loop component.
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
#include "open_loop.h"

/** @addtogroup MCSDK
  * @{
  */

/** @defgroup OpenLoop Open Loop Control
  * @brief Open Loop component of the Motor Control SDK
  *
  * Used to run the motor in open loop mode.
  *
  * @todo Document the Bus Open Loop "module".
  *
  * @{
  */

/* Private defines -----------------------------------------------------------*/

/**
  * @brief  Initialize OpenLoop variables.
  * @param  pHandle: Pointer on Handle structure of OpenLoop feature.
  * @param  pVSS: Pointer on virtual speed sensor structure.
  *  @retval none
  */
__weak void OL_Init( OpenLoop_Handle_t * pHandle, VirtualSpeedSensor_Handle_t * pVSS )
{
  pHandle->hVoltage = pHandle->hDefaultVoltage;
  pHandle->pVSS = pVSS;
}

/**
  * @brief  Set Vqd according to open loop phase voltage.
  * @param  pHandle: Pointer on Handle structure of OpenLoop feature.
  *  @retval qd_t Vqd conditioned values.
  */
__weak qd_t OL_VqdConditioning( OpenLoop_Handle_t * pHandle )
{
  qd_t Vqd;

  Vqd.q = pHandle->hVoltage;
  Vqd.d = 0;

  return ( Vqd );
}

/**
  * @brief  Allow to set new open loop phase voltage.
  * @param  pHandle: Pointer on Handle structure of OpenLoop feature.
  * @param  hNewVoltage: New voltage value to apply.
  * @retval None
  */
__weak void OL_UpdateVoltage( OpenLoop_Handle_t * pHandle, int16_t hNewVoltage )
{
  pHandle->hVoltage = hNewVoltage;
}

/**
  * @brief  Compute phase voltage to apply according to average mechanical speed (V/F Mode).
  * @param  pHandle: Pointer on Handle structure of OpenLoop feature.
  * @retval None
  */
__weak void OL_Calc( OpenLoop_Handle_t * pHandle )
{
  if ( pHandle->VFMode == true )
  {
    /* V/F mode true means enabled */
    if (pHandle->pVSS->_Super.hAvrMecSpeedUnit >= 0)
    {
      pHandle->hVoltage = pHandle->hVFOffset + ( pHandle->hVFSlope * pHandle->pVSS->_Super.hAvrMecSpeedUnit );
    }
    else
    {
      pHandle->hVoltage = pHandle->hVFOffset - ( pHandle->hVFSlope * pHandle->pVSS->_Super.hAvrMecSpeedUnit );
    }
  }
}

/**
  * @brief  Allow activation of the Voltage versus Frequency mode (V/F mode).
  * @param  pHandle: Pointer on Handle structure of OpenLoop feature.
  * @param  VFEnabling: Flag to enable the V/F mode.
  * @retval None
  */
__weak void OL_VF( OpenLoop_Handle_t * pHandle, bool VFEnabling )
{
  pHandle->VFMode = VFEnabling;
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
