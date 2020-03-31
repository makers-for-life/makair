/**
  ******************************************************************************
  * @file    flux_weakening_ctrl.h
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file provides firmware functions that implement the FLUX WEAKENING
  *          CTRL component of the Motor Control SDK.
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
  * @ingroup FluxWeakeningCtrl
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FLUXWEAKENINGCTRL_H
#define __FLUXWEAKENINGCTRL_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "mc_type.h"
#include "pid_regulator.h"
/** @addtogroup MCSDK
  * @{
  */

/** @addtogroup FluxWeakeningCtrl
  * @{
  */

/**
  * @brief  Flux Weakening Control Component handle structure
  */
typedef struct
{
  PID_Handle_t        *       pFluxWeakeningPID; /**< PI object used for flux weakening */
  PID_Handle_t        *       pSpeedPID;         /**< PI object used for speed control */
  uint16_t        hFW_V_Ref;              /**< Voltage reference, tenth of
                                                 percentage points */
  qd_t            AvVolt_qd;              /**< Average stator voltage in qd
                                                 reference frame */
  int16_t         AvVoltAmpl;             /**< Average stator voltage amplitude */
  int16_t         hIdRefOffset;           /**< Id reference offset */
  uint16_t        hMaxModule;             /**< Circle limitation maximum allowed module */

  uint16_t        hDefaultFW_V_Ref;       /**< Default flux weakening voltage reference,
                                               tenth of percentage points*/
  int16_t         hDemagCurrent;          /**< Demagnetization current in s16A:
                                               Current(Amp) = [Current(s16A) * Vdd micro]/
                                               [65536 * Rshunt * Aop] */
  int32_t         wNominalSqCurr;         /**< Squared motor nominal current in (s16A)^2
                                               where:
                                               Current(Amp) = [Current(s16A) * Vdd micro]/
                                               [65536 * Rshunt * Aop] */
  uint16_t        hVqdLowPassFilterBW;    /**< Use this parameter to configure the Vqd
                                               first order software filter bandwidth.
                                               hVqdLowPassFilterBW = FOC_CurrController
                                               call rate [Hz]/ FilterBandwidth[Hz] in
                                               case FULL_MISRA_COMPLIANCY is defined.
                                               On the contrary, if FULL_MISRA_COMPLIANCY
                                               is not defined, hVqdLowPassFilterBW is
                                               equal to log with base two of previous
                                               definition */
  uint16_t        hVqdLowPassFilterBWLOG; /**< hVqdLowPassFilterBW expressed as power of 2.
                                               E.g. if gain divisor is 512 the value
                                               must be 9 because 2^9 = 512 */
} FW_Handle_t;

/**
* @}
*/

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Initializes all the object variables, usually it has to be called
  *         once right after object creation.
  * @param  pHandle Flux weakening init strutcture.
  * @param  pPIDSpeed Speed PID structure.
  * @param  pPIDFluxWeakeningHandle FW PID structure.
  * @retval none.
  */
void FW_Init( FW_Handle_t * pHandle, PID_Handle_t * pPIDSpeed, PID_Handle_t * pPIDFluxWeakeningHandle );

/**
  * @brief  It should be called before each motor restart and clears the Flux
  *         weakening internal variables with the exception of the target
  *         voltage (hFW_V_Ref).
  * @param  pHandle Flux weakening init strutcture.
  * @retval none
  */
void FW_Clear( FW_Handle_t * pHandle );

/**
  * @brief  It computes Iqdref according the flux weakening algorithm.  Inputs
  *         are the starting Iqref components.
  *         As soon as the speed increases beyond the nominal one, fluxweakening
  *         algorithm take place and handles Idref value. Finally, accordingly
  *         with new Idref, a new Iqref saturation value is also computed and
  *         put into speed PI.
  * @param  pHandle Flux weakening init strutcture.
  * @param  Iqdref The starting current components that have to be
  *         manipulated by the flux weakening algorithm.
  * @retval qd_t Computed Iqdref.
  */
qd_t FW_CalcCurrRef( FW_Handle_t * pHandle, qd_t Iqdref );

/**
  * @brief  It low-pass filters both the Vqd voltage components. Filter
  *         bandwidth depends on hVqdLowPassFilterBW parameter
  * @param  pHandle Flux weakening init strutcture.
  * @param  Vqd Voltage componets to be averaged.
  * @retval none
  */
void FW_DataProcess( FW_Handle_t * pHandle, qd_t Vqd );

/**
  * @brief  Use this method to set a new value for the voltage reference used by
  *         flux weakening algorithm.
  * @param  pHandle Flux weakening init strutcture.
  * @param  uint16_t New target voltage value, expressend in tenth of percentage
  *         points of available voltage.
  * @retval none
  */
void FW_SetVref( FW_Handle_t * pHandle, uint16_t hNewVref );

/**
  * @brief  It returns the present value of target voltage used by flux
  *         weakening algorihtm.
  * @param  pHandle Flux weakening init strutcture.
  * @retval int16_t Present target voltage value expressed in tenth of
  *         percentage points of available voltage.
  */
uint16_t FW_GetVref( FW_Handle_t * pHandle );

/**
  * @brief  It returns the present value of voltage actually used by flux
  *         weakening algorihtm.
  * @param  pHandle Flux weakening init strutcture.
  * @retval int16_t Present averaged phase stator voltage value, expressed
  *         in s16V (0-to-peak), where
  *         PhaseVoltage(V) = [PhaseVoltage(s16A) * Vbus(V)] /[sqrt(3) *32767].
  */
int16_t FW_GetAvVAmplitude( FW_Handle_t * pHandle );

/**
  * @brief  It returns the measure of present voltage actually used by flux
  *         weakening algorihtm as percentage of available voltage.
  * @param  pHandle Flux weakening init strutcture.
  * @retval uint16_t Present averaged phase stator voltage value, expressed in
  *         tenth of percentage points of available voltage.
  */
uint16_t FW_GetAvVPercentage( FW_Handle_t * pHandle );

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif /* __cpluplus */

#endif /* __FLUXWEAKENINGCTRL_H */

/************************ (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
