/**
  ******************************************************************************
  * @file    dac_common_ui.h
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file contains all definitions and functions prototypes for the
  *          dac_common_ui component of the Motor Control SDK.
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
#ifndef __DACCOMMONUI_H
#define __DACCOMMONUI_H

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "user_interface.h"


/** @addtogroup MCSDK
  * @{
  */

/**
 * @addtogroup MCUI
 * @{
 */

/** @addtogroup dac_common_ui
  * @{
  */

#define DAC_CH_NBR 2
#define DAC_CH_USER 2



typedef struct
{
  UI_Handle_t _Super;      							/*!<   */

  MC_Protocol_REG_t bChannel_variable[DAC_CH_NBR]; /*!< Array of codes of
                                                        variables to be provided
                                                        in out to the related
                                                        channel.*/
  int16_t hUserValue[DAC_CH_USER];                 /*!< Array of user defined
                                                        DAC values.*/

  uint16_t hDAC_CH1_ENABLED;  /*!< Set to ENABLE to assign the channel 1 to the
                                   DAC object otherwise set DISABLE */
  uint16_t hDAC_CH2_ENABLED;  /*!< Set to ENABLE to assign the channel 2 to the
                                   DAC object otherwise set DISABLE */
} DAC_UI_Handle_t;


void DAC_SetChannelConfig(UI_Handle_t *pHandle, DAC_Channel_t bChannel,
                              MC_Protocol_REG_t bVariable);

MC_Protocol_REG_t DAC_GetChannelConfig(UI_Handle_t *pHandle, DAC_Channel_t bChannel);

void DAC_SetUserChannelValue(UI_Handle_t *pHandle, uint8_t bUserChNumber,
                              int16_t hValue);

int16_t DAC_GetUserChannelValue(UI_Handle_t *pHandle, uint8_t bUserChNumber);

/** @} */
/** @} */
/** @} */
/** @} */

#ifdef __cplusplus
}
#endif /* __cpluplus */

#endif /* __DACCOMMONUI_H */

 /************************ (C) COPYRIGHT 2018 STMicroelectronics *****END OF FILE****/
