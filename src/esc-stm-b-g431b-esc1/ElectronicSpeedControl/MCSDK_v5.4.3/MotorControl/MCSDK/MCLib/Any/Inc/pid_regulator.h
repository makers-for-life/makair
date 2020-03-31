/**
  ******************************************************************************
  * @file    pid_regulator.h
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file contains all definitions and functions prototypes for the
  *          PID reulator component of the Motor Control SDK.
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
  * @ingroup PIDRegulator
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PIDREGULATOR_H
#define __PIDREGULATOR_H

/* Includes ------------------------------------------------------------------*/
#include "mc_type.h"

/** @addtogroup MCSDK
  * @{
  */

/** @addtogroup PIDRegulator
  * @{
  */

/**
  * @brief Handle of a PID component
  *
  * @detail This structure stores all the parameters needed to perform a proportional,
  * integral and derivative regulation computation. It also stores configurable limits
  * in order to saturate the integral terms and the output value. This structure is
  * passed to each PID component function.
  */
typedef struct PID_Handle
{
  int16_t   hDefKpGain;           /**< Default @f$K_{pg}@f$ gain */
  int16_t   hDefKiGain;           /**< Default @f$K_{ig}@f$ gain */
  int16_t   hKpGain;              /**< @f$K_{pg}@f$ gain used by PID component */
  int16_t   hKiGain;              /**< @f$K_{ig}@f$ gain used by PID component */
  int32_t   wIntegralTerm;        /**< integral term */
  int32_t   wUpperIntegralLimit;  /**< Upper limit used to saturate the integral
                                       term given by @f$\frac{K_{ig}}{K_{id}} @f$ * integral of
                                       process variable error */
  int32_t   wLowerIntegralLimit;  /**< Lower limit used to saturate the integral
                                       term given by Ki / Ki divisor * integral of
                                       process variable error */
  int16_t   hUpperOutputLimit;    /**< Upper limit used to saturate the PI output */
  int16_t   hLowerOutputLimit;    /**< Lower limit used to saturate the PI output */
  uint16_t  hKpDivisor;           /**< Kp gain divisor, used in conjuction with
                                       Kp gain allows obtaining fractional values.
                                       If FULL_MISRA_C_COMPLIANCY is not defined
                                       the divisor is implemented through
                                       algebrical right shifts to speed up PI
                                       execution. Only in this case this parameter
                                       specifies the number of right shifts to be
                                       executed */
  uint16_t  hKiDivisor;           /**< Ki gain divisor, used in conjuction with
                                       Ki gain allows obtaining fractional values.
                                       If FULL_MISRA_C_COMPLIANCY is not defined
                                       the divisor is implemented through
                                       algebrical right shifts to speed up PI
                                       execution. Only in this case this parameter
                                       specifies the number of right shifts to be
                                       executed */
  uint16_t  hKpDivisorPOW2;       /**< Kp gain divisor expressed as power of 2.
                                       E.g. if gain divisor is 512 the value
                                       must be 9 as 2^9 = 512 */
  uint16_t  hKiDivisorPOW2;       /**< Ki gain divisor expressed as power of 2.
                                       E.g. if gain divisor is 512 the value
                                       must be 9 as 2^9 = 512 */
  int16_t   hDefKdGain;           /**< Default Kd gain */
  int16_t   hKdGain;              /**< Kd gain used by PID component */
  uint16_t  hKdDivisor;           /**< Kd gain divisor, used in conjuction with
                                       Kd gain allows obtaining fractional values.
                                       If FULL_MISRA_C_COMPLIANCY is not defined
                                       the divisor is implemented through
                                       algebrical right shifts to speed up PI
                                       execution. Only in this case this parameter
                                       specifies the number of right shifts to be
                                       executed */
  uint16_t  hKdDivisorPOW2;       /*!< Kd gain divisor expressed as power of 2.
                                       E.g. if gain divisor is 512 the value
                                       must be 9 as 2^9 = 512 */
  int32_t   wPrevProcessVarError; /*!< previous process variable used by the
                                       derivative part of the PID component */
} PID_Handle_t;

/*
 * It initializes the handle
 */
void PID_HandleInit( PID_Handle_t * pHandle );

/*
 * It updates the Kp gain
 */
void PID_SetKP( PID_Handle_t * pHandle, int16_t hKpGain );

/*
 * It updates the Ki gain
 */
void PID_SetKI( PID_Handle_t * pHandle, int16_t hKiGain );

/*
 *  It returns the Kp gain
 */
int16_t PID_GetKP( PID_Handle_t * pHandle );

/*
 * It returns the Ki gain
 */
int16_t PID_GetKI( PID_Handle_t * pHandle );

/*
 * It returns the Default Kp gain
 */
int16_t PID_GetDefaultKP( PID_Handle_t * pHandle );

/*
 * It returns the Default Ki gain of the passed PI object
 */
int16_t PID_GetDefaultKI( PID_Handle_t * pHandle );

/*
 * It set a new value into the PI integral term
 */
void PID_SetIntegralTerm( PID_Handle_t * pHandle, int32_t wIntegralTermValue );

/*
 * It returns the Kp gain divisor
 */
uint16_t PID_GetKPDivisor( PID_Handle_t * pHandle );

/*
 * It updates the Kp divisor
 */
void PID_SetKPDivisorPOW2( PID_Handle_t * pHandle, uint16_t hKpDivisorPOW2 );

/*
 * It returns the Ki gain divisor of the passed PI object
 */
uint16_t PID_GetKIDivisor( PID_Handle_t * pHandle );

/*
 * It updates the Ki divisor
 */
void PID_SetKIDivisorPOW2( PID_Handle_t * pHandle, uint16_t hKiDivisorPOW2 );

/*
 * It set a new value for lower integral term limit
 */
void PID_SetLowerIntegralTermLimit( PID_Handle_t * pHandle, int32_t wLowerLimit );

/*
 * It set a new value for upper integral term limit
 */
void PID_SetUpperIntegralTermLimit( PID_Handle_t * pHandle, int32_t wUpperLimit );

/*
 * It set a new value for lower output limit
 */
void PID_SetLowerOutputLimit( PID_Handle_t * pHandle, int16_t hLowerLimit );

/*
 * It set a new value for upper output limit
 */
void PID_SetUpperOutputLimit( PID_Handle_t * pHandle, int16_t hUpperLimit );

/*
 * It set a new value into the PID Previous error variable required to
 * compute derivative term
 */
void PID_SetPrevError( PID_Handle_t * pHandle, int32_t wPrevProcessVarError );

/*
 * @brief  It updates the Kd gain
 */
void PID_SetKD( PID_Handle_t * pHandle, int16_t hKdGain );

/*
 * It returns the Kd gain
 */
int16_t PID_GetKD( PID_Handle_t * pHandle );

/*
 * It returns the Kd gain divisor of the PID object passed
 */
uint16_t PID_GetKDDivisor( PID_Handle_t * pHandle );

/*
 * Updates the Kd divisor
 */
void PID_SetKDDivisorPOW2( PID_Handle_t * pHandle, uint16_t hKdDivisorPOW2 );

/* This function compute the output of a PI regulator sum of its
 * proportional and integralterms
 */
int16_t PI_Controller( PID_Handle_t * pHandle, int32_t wProcessVarError );

/* This function compute the output of a PID regulator sum of its
 *  proportional, integral and derivative terms
 */
int16_t PID_Controller( PID_Handle_t * pHandle, int32_t wProcessVarError );

/**
  * @}
  */

/**
  * @}
  */

#endif /*__PIDREGULATOR_H*/

/******************* (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
