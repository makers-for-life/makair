/**
  ******************************************************************************
  * @file    circle_limitation.h
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file contains all definitions and functions prototypes for the
  *          Circle Limitation component of the Motor Control SDK.
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
  * @ingroup CircleLimitation
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CIRCLELIMITATION_H
#define __CIRCLELIMITATION_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "mc_type.h"

/** @addtogroup MCSDK
  * @{
  */

/** @addtogroup CircleLimitation
  * @{
  */

/**
  * @brief  CircleLimitation component parameters definition
  */
typedef struct
{
  uint16_t MaxModule;               /**<  Circle limitation maximum allowed
                                         module */
  uint16_t MaxVd;                   /**<  Circle limitation maximum allowed
                                         module */
  uint16_t Circle_limit_table[87];  /**<  Circle limitation table */
  uint8_t  Start_index;             /**<  Circle limitation table indexing
                                         start */
} CircleLimitation_Handle_t;

/* Exported functions ------------------------------------------------------- */

qd_t Circle_Limitation( CircleLimitation_Handle_t * pHandle, qd_t Vqd );

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif /* __cpluplus */

#endif /* __Circle Limitation_H */

/************************ (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/

