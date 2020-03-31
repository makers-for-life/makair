/**
  ******************************************************************************
  * @file    sto_pll_speed_pos_fdbk.h
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file contains all definitions and functions prototypes for the
  *          State Observer + PLL Speed & Position Feedback component of the Motor
  *          Control SDK.
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
#ifndef __STO_PLL_SPEEDNPOSFDBK_H
#define __STO_PLL_SPEEDNPOSFDBK_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "speed_pos_fdbk.h"
#include "sto_speed_pos_fdbk.h"
#include "pid_regulator.h"

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

/**
  * @brief This structure is used to handle an instance of the STO_SpeednPosFdbk component
  *
  */

typedef struct
{
  SpeednPosFdbk_Handle_t _Super;

  int16_t  hC1;                 /*!< Variable containing state observer constant
                                     C1 to speed-up computations */
  int16_t  hC2;                /*!< Variable containing state observer constant
                                     C2, it can be computed as F1 * K1/ State
                                     observer execution rate [Hz] being K1 one
                                     of the two observer gains   */
  int16_t  hC3;                 /*!< Variable containing state observer constant
                                     C3 */
  int16_t  hC4;                  /*!< State Observer constant C4, it can
                                      be computed as K2 * max measurable
                                      current (A) / (Max application speed
                                      [#SPEED_UNIT] * Motor B-emf constant
                                      [Vllrms/krpm] * sqrt(2) * F2 * State
                                      observer execution rate [Hz]) being
                                       K2 one of the two observer gains  */
  int16_t  hC5;                 /*!< Variable containing state observer constant
                                     C5 */
  int16_t  hC6;                 /*!< State observer constant C6, computed with a
                                    specific procedure starting from the other
                                    constants */
  int16_t  hF1;                 /*!< Variable containing state observer scaling
                                     factor F1 */
  int16_t  hF2;                 /*!< Variable containing state observer scaling factor F2*/
  int16_t  hF3;                  /*!< State observer scaling factor F3 */
  uint16_t F3POW2;              /*!< State observer scaling factor F3 expressed as power of 2.
                                     E.g. if gain divisor is 512 the value
                                     must be 9 because 2^9 = 512 */
  PID_Handle_t PIRegulator;     /*!< PI regulator component handle, used for
                                     PLL implementation */
  int32_t Ialfa_est;           /*!< Estimated Ialfa current in int32 format */
  int32_t Ibeta_est;           /*!< Estimated Ibeta current in int32 format */
  int32_t wBemf_alfa_est;       /*!< Estimated B-emf alfa in int32_t format */
  int32_t wBemf_beta_est;       /*!< Estimated B-emf beta in int32_t format */
  int16_t hBemf_alfa_est;       /*!< Estimated B-emf alfa in int16_t format */
  int16_t hBemf_beta_est;       /*!< Estimated B-emf beta in int16_t format */
  int16_t Speed_Buffer[64];    /*!< Estimated DPP speed FIFO, it contains latest
                                     SpeedBufferSizeDpp speed measurements*/
  uint8_t Speed_Buffer_Index;  /*!< Position of latest speed estimation in
                                     estimated speed FIFO */
  bool IsSpeedReliable;        /*!< Latest private speed reliability information,
                                     updated by SPD_CalcAvrgMecSpeedUnit, it is
                                     true if the speed measurement variance is
                                     lower then threshold corresponding to
                                     hVariancePercentage */
  uint8_t ConsistencyCounter;  /*!< Counter of passed tests for start-up
                                     validation */
  uint8_t ReliabilityCounter; /*!< Counter for reliability check */
  bool IsAlgorithmConverged;   /*!< Boolean variable containing observer
                                     convergence information */
  bool IsBemfConsistent;       /*!< Sensor reliability information, updated by
                                     SPD_CalcAvrgMecSpeedUnit, it is true if the
                                     observed back-emfs are consistent with
                                     expectation*/

  int32_t Obs_Bemf_Level;      /*!< Observed back-emf Level*/
  int32_t Est_Bemf_Level;      /*!< Estimated back-emf Level*/
  bool EnableDualCheck;        /*!< Consistency check enabler*/
  int32_t DppBufferSum;        /*!< summation of speed buffer elements [dpp]*/
  int16_t SpeedBufferOldestEl; /*!< Oldest element of the speed buffer*/

  uint8_t SpeedBufferSizeUnit;       /*!< Depth of FIFO used to average
                                           estimated speed exported by
                                           SPD_GetAvrgMecSpeedUnit. It
                                           must be an integer number between 1
                                           and 64 */
  uint8_t SpeedBufferSizeDpp;       /*!< Depth of FIFO used for both averaging
                                           estimated speed exported by
                                           SPD_GetElSpeedDpp and state
                                           observer equations. It must be an
                                           integer number between 1 and
                                           bSpeedBufferSizeUnit */
  uint16_t VariancePercentage;        /*!< Parameter expressing the maximum
                                           allowed variance of speed estimation
                                           */
  uint8_t SpeedValidationBand_H;   /*!< It expresses how much estimated speed
                                           can exceed forced stator electrical
                                           frequency during start-up without
                                           being considered wrong. The
                                           measurement unit is 1/16 of forced
                                           speed */
  uint8_t SpeedValidationBand_L;   /*!< It expresses how much estimated speed
                                           can be below forced stator electrical
                                           frequency during start-up without
                                           being considered wrong. The
                                           measurement unit is 1/16 of forced
                                           speed */
  uint16_t MinStartUpValidSpeed;     /*!< Absolute value of minimum mechanical
                                            speed  required to validate the start-up.
                                            Expressed in the unit defined by #SPEED_UNIT. */
  uint8_t StartUpConsistThreshold;   /*!< Number of consecutive tests on speed
                                           consistency to be passed before
                                           validating the start-up */
  uint8_t Reliability_hysteresys;    /*!< Number of reliability failed
                                           consecutive tests before a speed
                                           check fault is returned to _Super.bSpeedErrorNumber
                                           */
  uint8_t BemfConsistencyCheck;      /*!< Degree of consistency of the observed
                                           back-emfs, it must be an integer
                                           number ranging from 1 (very high
                                           consistency) down to 64 (very poor
                                           consistency) */
  uint8_t BemfConsistencyGain;       /*!< Gain to be applied when checking
                                           back-emfs consistency; default value
                                           is 64 (neutral), max value 105
                                           (x1.64 amplification), min value 1
                                           (/64 attenuation) */
  uint16_t MaxAppPositiveMecSpeedUnit; /*!< Maximum positive value of rotor speed. Expressed in
                                             the unit defined by #SPEED_UNIT. It can be
                                             x1.1 greater than max application speed*/
  uint16_t F1LOG;                    /*!< F1 gain divisor expressed as power of 2.
                                            E.g. if gain divisor is 512 the value
                                            must be 9 because 2^9 = 512 */
  uint16_t F2LOG;                    /*!< F2 gain divisor expressed as power of 2.
                                            E.g. if gain divisor is 512 the value
                                            must be 9 because 2^9 = 512 */
  uint16_t SpeedBufferSizeDppLOG;    /*!< bSpeedBufferSizedpp expressed as power of 2.
                                            E.g. if gain divisor is 512 the value
                                            must be 9 because 2^9 = 512 */
  bool ForceConvergency;       /*!< Variable to force observer convergence.*/
  bool ForceConvergency2;      /*!< Variable to force observer convergence.*/

  int16_t hForcedAvrSpeed_VSS;  
  
} STO_PLL_Handle_t;


/* Exported functions ------------------------------------------------------- */

/* It initializes the state observer object */
void STO_PLL_Init( STO_PLL_Handle_t * pHandle );

/* It only returns, necessary to implement fictitious IRQ_Handler */
void STO_PLL_Return( STO_PLL_Handle_t * pHandle, uint8_t flag );

/* It clears state observer object by re-initializing private variables*/
void STO_PLL_Clear( STO_PLL_Handle_t * pHandle );

/* It executes Luenberger state observer and calls PLL to compute a new speed
*  estimation and update the estimated electrical angle.
*/
int16_t STO_PLL_CalcElAngle( STO_PLL_Handle_t * pHandle, Observer_Inputs_t * pInputVars_str );

/* Computes the rotor average mechanical speed in the unit defined by #SPEED_UNIT and returns it in pMecSpeedUnit. */
bool STO_PLL_CalcAvrgMecSpeedUnit( STO_PLL_Handle_t * pHandle, int16_t * pMecSpeedUnit );

/* It resets integral term of PLL during on-the-fly startup */
void STO_OTF_ResetPLL( STO_Handle_t * pHandle );

/* It resets integral term of PLL*/
void STO_ResetPLL( STO_PLL_Handle_t * pHandle );

/* It checks whether the state observer algorithm converged.*/
bool STO_PLL_IsObserverConverged( STO_PLL_Handle_t * pHandle, int16_t hForcedMecSpeedUnit );

/* It computes the estimated average electrical speed ElSpeedDpp expressed in dpp */
void STO_PLL_CalcAvrgElSpeedDpp( STO_PLL_Handle_t * pHandle );

/* It exports estimated Bemf alpha-beta in alphabeta_t format */
alphabeta_t STO_PLL_GetEstimatedBemf( STO_PLL_Handle_t * pHandle );

/* It exports the stator current alpha-beta as estimated by state  observer */
alphabeta_t STO_PLL_GetEstimatedCurrent( STO_PLL_Handle_t * pHandle );

/* It set new values for observer gains*/
void STO_PLL_SetObserverGains( STO_PLL_Handle_t * pHandle, int16_t hC1, int16_t hC2 );

/* It exports current observer gains through parameters pC2 and pC4 */
void STO_PLL_GetObserverGains( STO_PLL_Handle_t * pHandle, int16_t * pC2, int16_t * pC4 );

/* It exports current PLL gains through parameters pPgain and pIgain */
void STO_GetPLLGains( STO_PLL_Handle_t * pHandle, int16_t * pPgain, int16_t * pIgain );

/* It set new values for PLL gains */
void STO_SetPLLGains( STO_PLL_Handle_t * pHandle, int16_t hPgain, int16_t hIgain );

void STO_PLL_SetMecAngle( STO_PLL_Handle_t * pHandle, int16_t hMecAngle );

void STO_SetPLL( STO_PLL_Handle_t * pHandle, int16_t hElSpeedDpp, int16_t hElAngle );

/* It exports estimated Bemf squared level*/
int32_t STO_PLL_GetEstimatedBemfLevel( STO_PLL_Handle_t * pHandle );

/* It exports observed Bemf squared level*/
int32_t STO_PLL_GetObservedBemfLevel( STO_PLL_Handle_t * pHandle );

/* It enables / disables the Bemf consistency check */
void STO_PLL_BemfConsistencyCheckSwitch( STO_PLL_Handle_t * pHandle, bool bSel );

/* It exports Bemf consistency */
bool STO_PLL_IsBemfConsistent( STO_PLL_Handle_t * pHandle );

/* It returns the result of the last variance check*/
bool STO_PLL_IsVarianceTight( const STO_Handle_t * pHandle );

/* It set internal ForceConvergency1 to true*/
void STO_PLL_ForceConvergency1( STO_Handle_t * pHandle );

/* It set internal ForceConvergency2 to true*/
void STO_PLL_ForceConvergency2( STO_Handle_t * pHandle );

/* It set absolute value of minimum mechanical speed required to validate the start-up. */
void STO_SetMinStartUpValidSpeedUnit( STO_PLL_Handle_t * pHandle, uint16_t hMinStartUpValidSpeed );

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

#endif /*__STO_PLL_SPEEDNPOSFDBK_H*/

/************************ (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
