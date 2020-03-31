/**
  ******************************************************************************
  * @file    pwm_curr_fdbk.h
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file contains all definitions and functions prototypes for the
  *          PWM & Current Feedback component of the Motor Control SDK.
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
  * @ingroup pwm_curr_fdbk
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PWMNCOMMON_H
#define __PWMNCOMMON_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "mc_type.h"
#define NB_CONVERSIONS 16u
/** @addtogroup MCSDK
  * @{
  */

/** @addtogroup pwm_curr_fdbk
  * @{
  */

/* Exported defines ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
/**
 * @brief  It perform the start of all the timers required by the control.
 *          It utilizes TIM2 as temporary timer to achieve synchronization between
 *          PWM signals.
 *          When this function is called, TIM1 and/or TIM8 must be in frozen state
 *          with CNT, ARR, REP RATE and trigger correctly set (these setting are
 *          usually performed in the Init method accordingly with the configuration)
 */
void startTimers( void );

/**
 * @brief  It waits for the end of the polarization. If the polarization exceeds the
 *         the number of needed PWM cycles, it reports an error.
 */
void waitForPolarizationEnd( TIM_TypeDef*  TIMx, uint16_t  *SWerror, uint8_t repCnt, uint8_t *cnt );
/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif /* __cpluplus */

#endif /* __PWMNCOMMON_H */

/******************* (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
