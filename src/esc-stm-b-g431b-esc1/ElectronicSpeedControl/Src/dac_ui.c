
/**
  ******************************************************************************
  * @file    dac_ui.c
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file provides firmware functions that implement the following features
  *          of the dac component of the Motor Control SDK:
  *           + dac initialization
  *           + dac execution
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
#include "dac_ui.h"

#define DACOFF 32768

/** @addtogroup MCSDK
  * @{
  */

/** @addtogroup MCUI
  * @{
  */

/**
 * @defgroup DAC_UserInterface DAC User Interface
 *
 * @brief DAC User Interface
 *
 * @todo Complete Documentation. What difference with dac_common_ui?
 * @{
 */

/**
  * @brief  Hardware and software initialization of the DAC object. This is the
  *         implementation of the virtual function.
  * @param  pHandle pointer on related component instance.
  */
__weak void DAC_Init(UI_Handle_t *pHandle)
{  
  /* Enable DAC Channel1 */
  LL_DAC_Enable(DAC1, LL_DAC_CHANNEL_1);
}

/**
  * @brief  This method is used to update the DAC outputs. The selected 
  *         variables will be provided in the related output channels. This is 
  *         the implementation of the virtual function.
  * @param  pHandle pointer on related component instance. 
  */
__weak void DAC_Exec(UI_Handle_t *pHandle)
{
  DAC_UI_Handle_t *pDacHandle = (DAC_UI_Handle_t *)pHandle;
  MC_Protocol_REG_t bCh_var;
  
  bCh_var = pDacHandle->bChannel_variable[DAC_CH0];
  LL_DAC_ConvertData12LeftAligned(DAC1, LL_DAC_CHANNEL_1,
                                  DACOFF + ((int16_t)UI_GetReg(pHandle,bCh_var,MC_NULL)));
  LL_DAC_TrigSWConversion(DAC1, LL_DAC_CHANNEL_1);
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
