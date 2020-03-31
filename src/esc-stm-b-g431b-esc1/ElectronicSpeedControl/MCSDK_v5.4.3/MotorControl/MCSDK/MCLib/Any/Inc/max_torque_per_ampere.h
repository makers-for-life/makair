/**
 ******************************************************************************
 * @file    max_torque_per_ampere.h
 * @author  Motor Control SDK Team, ST Microelectronics
 * @brief   Maximum torque per ampere (MTPA) control for I-PMSM motors
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
 * @ingroup MTPA
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAXTORQUEPERAMPERE_H
#define __MAXTORQUEPERAMPERE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "mc_type.h"
/** @addtogroup MCSDK
  * @{
  */

/** @addtogroup MTPA Maximum Torque Per Ampere Control
  * @{
  */

/* Exported types ------------------------------------------------------------*/
#define SEGMENT_NUM         ((uint8_t)7)          /* coeff no. -1 */
#define MTPA_ARRAY_SIZE     SEGMENT_NUM+1
/**
 * @brief  Handle structure of max_torque_per_ampere.c
 */
typedef struct
{
  int16_t  SegDiv;               /**< Segments divisor */
  int32_t  AngCoeff[MTPA_ARRAY_SIZE];          /**< Angular coefficients table */
  int32_t  Offset[MTPA_ARRAY_SIZE];            /**< Offsets table */
} MTPA_Handle_t;

/* Exported functions ------------------------------------------------------- */

/*  Function used to set configure an instance of the CCC Component *****/
void MTPA_CalcCurrRefFromIq( MTPA_Handle_t * pHandle, qd_t *Iqdref );

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif /* __cpluplus */

#endif /* __MAXTORQUEPERAMPERE_H */

/************************ (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
