/**
  ******************************************************************************
  * @file    dac_ui.h
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file contains all definitions and functions prototypes for the
  *          dac_ui component of the Motor Control SDK.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DACUI_H
#define __DACUI_H

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "user_interface.h"
#include "dac_common_ui.h"


 /** @addtogroup MCSDK
   * @{
   */

 /** @addtogroup MCUI
   * @{
   */

 /** @addtogroup DAC_UserInterface
   * @{
   */

#define DAC_CH_NBR 2
#define DAC_CH_USER 2


void DAC_Init(UI_Handle_t *pHandle);

void DAC_Exec(UI_Handle_t *pHandle);

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
 * @}
 */
#ifdef __cplusplus
}
#endif /* __cpluplus */

#endif /*__DACUI_H*/

/******************* (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
