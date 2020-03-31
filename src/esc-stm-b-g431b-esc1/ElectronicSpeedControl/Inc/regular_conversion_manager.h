
/**
  ******************************************************************************
  * @file    regular_conversion_manager.h
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file contains all definitions and functions prototypes for the
  *          regular_conversion_manager component of the Motor Control SDK.
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
#ifndef __regular_conversion_manager_h
#define __regular_conversion_manager_h

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "stdbool.h"
#include "mc_stm_types.h"

/** @addtogroup MCSDK
  * @{
  */

/** @addtogroup RCM
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/**
  * @brief RegConv_t contains all the parameters required to execute a regular conversion
  *
  * it is used by all regular_conversion_manager's client
  *
  */
typedef struct 
{
  ADC_TypeDef * regADC;
  uint8_t  channel;
  uint32_t samplingTime;
} RegConv_t;

typedef enum
{
  RCM_USERCONV_IDLE,
  RCM_USERCONV_REQUESTED,
  RCM_USERCONV_EOC  
}RCM_UserConvState_t;

typedef void (*RCM_exec_cb_t)(uint8_t handle, uint16_t data, void *UserData); 

/* Exported functions ------------------------------------------------------- */

/*  Function used to register a regular conversion */
uint8_t RCM_RegisterRegConv(RegConv_t * regConv);

/*  Function used to register a regular conversion with a callback attached*/
uint8_t RCM_RegisterRegConv_WithCB (RegConv_t * regConv, RCM_exec_cb_t fctCB, void *data);

/*  Function used to execute an already registered regular conversion */
uint16_t RCM_ExecRegularConv (uint8_t handle);

/* select the handle conversion to be executed during the next call to RCM_ExecUserConv */
bool RCM_RequestUserConv(uint8_t handle);

/* return the latest user conversion value*/
uint16_t RCM_GetUserConv(void);

/* Must be called by MC_TASK only to grantee proper scheduling*/
void RCM_ExecUserConv (void);

/* return the state of the user conversion state machine*/
RCM_UserConvState_t RCM_GetUserConvState(void);

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif /* __cpluplus */

#endif /* __regular_conversion_manager_h */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
