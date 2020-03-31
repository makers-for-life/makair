/**
  ******************************************************************************
  * @file    digital_output.c
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file provides firmware functions that implement the Digital
  *          Output component of the Motor Control SDK:
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
#include "digital_output.h"
#include "mc_type.h"

/** @addtogroup MCSDK
  * @{
  */

/** @defgroup DigitalOutput Digital Output
  * @brief digital output component of the Motor Control SDK
  *
  * @{
  */


/**
* @brief Accordingly with selected polarity, it sets to active or inactive the
*        digital output
* @param pHandle handler address of the digital output component.
* @param OutputState_t New requested state
* @retval none
*/
__weak void DOUT_SetOutputState( DOUT_handle_t * pHandle, DOutputState_t State )
{

  if ( State == ACTIVE )
  {
    if ( pHandle->bDOutputPolarity == DOutputActiveHigh )
    {
      LL_GPIO_SetOutputPin( pHandle->hDOutputPort, pHandle->hDOutputPin );
    }
    else
    {
      LL_GPIO_ResetOutputPin( pHandle->hDOutputPort, pHandle->hDOutputPin );
    }
  }
  else if ( pHandle->bDOutputPolarity == DOutputActiveHigh )
  {
    LL_GPIO_ResetOutputPin( pHandle->hDOutputPort, pHandle->hDOutputPin );
  }
  else
  {
    LL_GPIO_SetOutputPin( pHandle->hDOutputPort, pHandle->hDOutputPin );
  }
  pHandle->OutputState = State;
}

/**
* @brief It returns the state of the digital output
* @param pHandle pointer on related component instance
* @retval OutputState_t Digital output state (ACTIVE or INACTIVE)
*/
__weak DOutputState_t DOUT_GetOutputState( DOUT_handle_t * pHandle )
{
  return ( pHandle->OutputState );
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
