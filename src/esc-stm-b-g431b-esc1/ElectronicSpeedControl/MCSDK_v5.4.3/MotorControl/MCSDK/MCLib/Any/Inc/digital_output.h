/**
 ******************************************************************************
 * @file    digital_output.h
 * @author  Motor Control SDK Team, ST Microelectronics
 * @brief   This file contains all definitions and functions prototypes for the
 *          digital output component of the Motor Control SDK.
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
 * @ingroup DigitalOutput
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DIGITALOUTPUT_H
#define __DIGITALOUTPUT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "mc_type.h"

/** @addtogroup MCSDK
 * @{
 */

/** @addtogroup DigitalOutput
 * @{
 */


/* Exported constants --------------------------------------------------------*/
#define DOutputActiveHigh       1u
#define DOutputActiveLow        0u


/* Exported types ------------------------------------------------------------*/

/**
 * @brief  digital output handler definition
 */
typedef struct
{
  DOutputState_t OutputState;       /*!< indicates the state of the digital output */
  GPIO_TypeDef * hDOutputPort;       /*!< GPIO output port. It must be equal
                                       to GPIOx x= A, B, ...*/
  uint16_t hDOutputPin;             /*!< GPIO output pin. It must be equal to
                                       GPIO_Pin_x x= 0, 1, ...*/
  uint8_t  bDOutputPolarity;        /*!< GPIO output polarity. It must be equal
                                       to DOutputActiveHigh or DOutputActiveLow */
} DOUT_handle_t;

/**
 * @brief  Initializes object variables, port and pin. It must be called only
 *         after PWMnCurrFdbk object initialization and DigitalOutput object
 *         creation.
 * @param pHandle handler address of the digital output component.
 * @retval none.
 */
void DOUT_Init( DOUT_handle_t * pHandle );

/**
 * @brief Accordingly with selected polarity, it sets to active or inactive the
 *        digital output
 * @param pHandle handler address of the digital output component.
 * @param OutputState_t New requested state
 * @retval none
 */
void DOUT_SetOutputState( DOUT_handle_t * pHandle, DOutputState_t State );

/**
 * @brief It returns the state of the digital output
 * @param pHandle pointer on component's handle
 * @retval OutputState_t Digital output state (ACTIVE or INACTIVE)
 */
DOutputState_t DOUT_GetOutputState( DOUT_handle_t * pHandle );

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif /* __cpluplus */

#endif /* __DIGITALOUTPUT_H */

/************************ (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
