/**
  ******************************************************************************
  * @file    sto_speed_pos_fdbk.h
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file contains definitions and functions prototypes common to all
  *          State Observer based Speed & Position Feedback components of the Motor
  *          Control SDK (the CORDIC and PLL implementations).
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
  * @ingroup SpeednPosFdbk_STO
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STO_SPEEDNPOSFDBK_H
#define __STO_SPEEDNPOSFDBK_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "speed_pos_fdbk.h"

/** @addtogroup MCSDK
  * @{
  */

/** @addtogroup SpeednPosFdbk
  * @{
  */

/** @addtogroup SpeednPosFdbk_STO
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/** @brief PWM & Current Sensing component handle type */
typedef struct STO_Handle STO_Handle_t;

typedef void ( *STO_ForceConvergency1_Cb_t )( STO_Handle_t * pHandle );
typedef void ( *STO_ForceConvergency2_Cb_t )( STO_Handle_t * pHandle );
typedef void ( *STO_OtfResetPLL_Cb_t )( STO_Handle_t * pHandle );
typedef bool ( *STO_SpeedReliabilityCheck_Cb_t )( const STO_Handle_t * pHandle );

/**
  * @brief  SpeednPosFdbk  handle definition
  */
struct STO_Handle
{
  SpeednPosFdbk_Handle_t     *     _Super;
  STO_ForceConvergency1_Cb_t       pFctForceConvergency1;
  STO_ForceConvergency2_Cb_t       pFctForceConvergency2;
  STO_OtfResetPLL_Cb_t             pFctStoOtfResetPLL;
  STO_SpeedReliabilityCheck_Cb_t   pFctSTO_SpeedReliabilityCheck;
};

/**
  * @}
  */

/**
  * @}
  */

/** @} */

#ifdef __cplusplus
}
#endif /* __cpluplus */

#endif /*__STO_SPEEDNPOSFDBK_H*/

/************************ (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
