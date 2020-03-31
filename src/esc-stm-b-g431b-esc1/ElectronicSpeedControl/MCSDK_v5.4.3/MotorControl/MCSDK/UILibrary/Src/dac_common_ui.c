/**
  ******************************************************************************
  * @file    dac_common_ui.c
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file provides firmware functions that implement the following features
  *          of the CCC component of the Motor Control SDK:
  *           + Set up the DAC outputs
  *           + get the current DAC channel selected output.
  *           + Set the value of the "User DAC channel".
  *           + get the value of the "User DAC channel".
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
#include "dac_common_ui.h"

/** @addtogroup MCSDK
  * @{
  */

/**
 * @addtogroup MCUI
 * @{
 */

/** @defgroup dac_common_ui Common DAC User Interface
  * @brief DAC User Interface implementation
  *
  * Digital to Analog Converter common component. In Motor Control context, the DAC is used for
  * debug purposes by outputting information with low overhead. It is particularly useful to
  * output fast changing analog-like data as voltage references or measured currents to compare
  * them with measurements made with other means, for instance.
  *
  * @{
  */


/**
  * @brief  Set up the DAC outputs. The selected
  *         variables will be provided in the related output channels after next
  *         DACExec.
  * @param  user interface handle.
  * @param  bChannel the DAC channel to be programmed. It must be one of the
  *         exported channels Ex. DAC_CH0.
  * @param  bVariable the variables to be provided in out through the selected
  *         channel. It must be one of the exported UI register Ex.
  *         MC_PROTOCOL_REG_I_A.
  * @retval none.
  */
__weak void DAC_SetChannelConfig(UI_Handle_t *pHandle, DAC_Channel_t bChannel,
                              MC_Protocol_REG_t bVariable)
{
  DAC_UI_Handle_t *pDacHandle = (DAC_UI_Handle_t *)pHandle;
  pDacHandle->bChannel_variable[bChannel] = bVariable;
}

/**
  * @brief  get the current DAC channel selected output.
  * @param  user interface handle.
  * @param  bChannel the inspected DAC channel. It must be one of the
  *         exported channels (Ex. DAC_CH0).
  * @retval MC_Protocol_REG_t The variables provided in out through the inspected
  *         channel. It will be one of the exported UI register (Ex.
  *         MC_PROTOCOL_REG_I_A).
  */
__weak MC_Protocol_REG_t DAC_GetChannelConfig(UI_Handle_t *pHandle, DAC_Channel_t bChannel)
{
  DAC_UI_Handle_t *pDacHandle = (DAC_UI_Handle_t *)pHandle;
  return (pDacHandle->bChannel_variable[bChannel]);
}

/**
  * @brief  Set the value of the "User DAC channel".
  * @param  user interface handle
  * @param  bUserChNumber the "User DAC channel" to be programmed.
  * @param  hValue the value to be put in output.
  * @retval none.
  */
__weak void DAC_SetUserChannelValue(UI_Handle_t *pHandle, uint8_t bUserChNumber,
                              int16_t hValue)
{
  DAC_UI_Handle_t *pDacHandle = (DAC_UI_Handle_t *)pHandle;
  pDacHandle->hUserValue[bUserChNumber] = hValue;
}

/**
  * @brief  get the value of the "User DAC channel".
  * @param  user interface handle
  * @param  bUserChNumber the "User DAC channel" to be programmed.
  * @retval none.
  */
__weak int16_t DAC_GetUserChannelValue(UI_Handle_t *pHandle, uint8_t bUserChNumber)
{
  DAC_UI_Handle_t *pDacHandle = (DAC_UI_Handle_t *)pHandle;
  return (pDacHandle->hUserValue[bUserChNumber]);
}

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

/******************* (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
