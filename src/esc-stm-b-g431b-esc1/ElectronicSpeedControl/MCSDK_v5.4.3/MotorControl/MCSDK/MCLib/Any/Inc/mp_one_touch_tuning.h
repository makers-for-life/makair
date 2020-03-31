/**
  ******************************************************************************
  * @file    mp_one_touch_tuning.h
  * @author  STMicroelectronics - System Lab - MC Team
  * @version 4.3.0
  * @date    22-Sep-2016 15:29
  * @brief   This file contains interface of OneTouchTuning class
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  * @ingroup OneTouchTuning
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MP_ONE_TOUCH_TUNING_H
#define __MP_ONE_TOUCH_TUNING_H

/* Includes ------------------------------------------------------------------*/
#include "mc_type.h"
#include "pid_regulator.h"
#include "sto_pll_speed_pos_fdbk.h"
#include "speed_torq_ctrl.h"
#include "ramp_ext_mngr.h"

/** @addtogroup STM32_PMSM_MC_Library
  * @{
  */

/** @addtogroup OneTouchTuning
  * @{
  */

/** @defgroup OneTouchTuning_class_exported_types OneTouchTuning class exported types
* @{
*/

/**
  * @brief  Public OneTouchTuning class definition
  */


/**
  * @brief  OneTouchTuning class parameters definition
  */
typedef const struct
{
  RampExtMngr_Handle_t rampExtMngrParams; /*!< Ramp manager used by SCC.*/
  float fBWdef;               /*!< Default bandwidth of speed regulator.*/
  float fMeasWin;             /*!< Duration of measurement window for speed and
                                   current Iq, expressed in seconds.*/
  uint8_t bPolesPairs;        /*!< Number of motor poles pairs.*/
  uint16_t hMaxPositiveTorque;/*!< Maximum positive value of motor
                                   torque. This value represents
                                   actually the maximum Iq current
                                   expressed in digit.*/
  float fCurrtRegStabTimeSec; /*!< Current regulation stabilization time in seconds.*/
  float fOttLowSpeedPerc;     /*!< OTT lower speed percentage.*/
  float fOttHighSpeedPerc;    /*!< OTT higher speed percentage.*/
  float fSpeedStabTimeSec;    /*!< Speed stabilization time in seconds.*/
  float fTimeOutSec;          /*!< Timeout for speed stabilization.*/
  float fSpeedMargin;         /*!< Speed margin percentage to validate speed ctrl.*/
  int32_t wNominalSpeed;      /*!< Nominal speed set by the user expressed in RPM.*/
  float spdKp;                /*!< Initial KP factor of the speed regulator to be tuned.*/
  float spdKi;                /*!< Initial KI factor of the speed regulator to be tuned.*/
  float spdKs;                /*!< Initial antiwindup factor of the speed regulator to be tuned.*/
  float fRshunt;              /*!< Value of shunt resistor.*/
  float fAmplificationGain;   /*!< Current sensing amplification gain.*/
} OTT_Params_t, *pOTT_Params_t;

/**
* @brief  OTT_State_t enum type definition, it lists all the possible OTT state
          machine states.
*/
typedef enum
{
  OTT_IDLE,
  OTT_NOMINAL_SPEED_DET,
  OTT_DYNAMICS_DET_RAMP_DOWN,
  OTT_DYNAMICS_DET_SET_TORQUE,
  OTT_DYNAMICS_DETECTION,
  OTT_RAMP_DOWN_H_SPEED,
  OTT_H_SPEED_TEST,
  OTT_RAMP_DOWN_L_SPEED,
  OTT_L_SPEED_TEST,
  OTT_TORQUE_STEP,
  OTT_END
} OTT_State_t;

/**
  * @brief  OneTouchTuning class members definition
  */
typedef struct
{
	  SpeednPosFdbk_Handle_t *pSpeedSensor;       /*!< Related speed sensor used. */
	  pFOCVars_t pFOCVars;  /*!< Related structure of FOC vars.*/
	  PID_Handle_t * pPIDSpeed;          /*!< Related speed controller used. */
	  SpeednTorqCtrl_Handle_t *pSTC;            /*!< Speed and torque controller used.*/

  RampExtMngr_Handle_t *pREMng;           /*!< Ramp manager used.*/
  int16_t hFDetIq[2];      /*!< Array used to store Iq measurements done during F estimation.*/
  float fFDetOmega[2];     /*!< Array used to store Omega values during F estimation.*/
  float fF;                /*!< Stores the last F estimation.*/
  float fOmegaTh;          /*!< Stores the last omega threshold.*/
  float fTau;              /*!< Stores the last computed mechanical time constant.*/
  float fJ;                /*!< Stores the last J estimation.*/
  float fBW;               /*!< Bandwidth of speed regulator.*/
  OTT_State_t bState;      /*!< State macchine state.*/
  int32_t wIqsum;          /*!< Sum of measured Iq.*/
  int32_t wSpeed01Hzsum;   /*!< Sum of average mechanical speed.*/
  uint16_t hIqCnt;         /*!< Counter for Iq acquisitions.*/
  int32_t wCnt;            /*!< 32bit counter.*/
  int16_t hSpeed01HzMean;  /*!< Mean value of mechanical speed.*/
  int16_t hSpeed01HzDelta; /*!< Delta speed between mechanical speed.*/
  uint16_t hCurRegStabCnt; /*!< Stabilization counter.*/
  uint16_t hJdetCnt;       /*!< Counter to measure the mechanical time constant.*/
  float fEstNominalSpdRPM; /*!< Estimated nominal speed.*/
  int16_t hIqNominal;      /*!< Current measured at nominal speed steady state.*/
  int16_t hIqAcc;          /*!< Current used to accelerate the motor.*/
  int16_t hTargetLRPM;     /*!< Lower speed used for OTT.*/
  int16_t hTargetHRPM;     /*!< Higher speed used for OTT.*/
  uint16_t hMeasWinTicks;  /*!< Number of ticks of the measurement window.*/
  uint16_t hCurRegStabTks; /*!< Number of ticks for current regulation stabilization time.*/
  uint16_t hSpeedStabTks;  /*!< Number of ticks for speed stabilization time.*/
  bool bPI_Tuned;          /*!< True is PI is tuned, false otherwise.*/
  float fKp;               /*!< Computed Kp.*/
  float fKi;               /*!< Computed Ki.*/
  int8_t stabCnt;          /*!< Stabilization counter.*/
  float fSpeed;            /*!< Internal target reference.*/
  uint16_t hTimeOutTks;    /*!< Number of tick for timeout.*/
  uint8_t bPolesPairs;     /*!< Motor poles pairs.*/
  uint16_t hMaxPositiveTorque;/*!< Maximum positive value of motor
                                   torque. This value represents
                                   actually the maximum Iq current
                                   expressed in digit.*/
  float fRPMTh;            /*!< Speed threshold for mecchanical constant
                                time estimation.*/
  int32_t wNominalSpeed;   /*!< Nominal speed set by the user expressed in RPM.*/
  float spdKp;             /*!< KP factor of the speed regulator to be tuned.*/
  float spdKi;             /*!< KI factor of the speed regulator to be tuned.*/
  float spdKs;             /*!< Antiwindup factor of the speed regulator to be tuned.*/
  float spdIntTerm;        /*!< Integral term of the speed regulator to be tuned.*/
  float spdAntiWindTerm;   /*!< Antiwindup term of the speed regulator to be tuned.*/
  float fKe;               /*!< Stores the last Ke estimation.*/

  pOTT_Params_t pOTT_Params_str;  /**< OTT parameters */

} OTT_Handle_t;


/**
* @}
*/


/**
  * @brief  Initializes all the object variables, usually it has to be called
  *         once right after object creation.
  * @param  this related object of class COTT.
  * @param  pOTT_Init pointer to the OTT init structure.
  * @retval none.
  */
void OTT_Init( OTT_Handle_t * pHandle );

/**
  * @brief  It should be called before each motor restart. It initialize
  *         internal state of OTT.
  * @param  this related object of class COTT.
  * @retval none.
  */
void OTT_Clear( OTT_Handle_t * pHandle );

/**
  * @brief  It should be called at MF and execute the OTT algorithm.
  * @param  this related object of class COTT.
  * @retval none.
  */
void OTT_MF( OTT_Handle_t * pHandle );

/**
  * @brief  It should be called in START_RUN state. It begins the OTT procedure.
  * @param  this related object of class COTT.
  * @retval none.
  */
void OTT_SR( OTT_Handle_t * pHandle );

/**
  * @brief  Call this method before start motor to force new OTT procedure.
  * @param  this related object of class COTT.
  * @retval none.
  */
void OTT_ForceTuning( OTT_Handle_t * pHandle );

/**
  * @brief  It returns the nominal speed estimated by OTT.
  * @param  this related object of class COTT.
  * @retval uint32_t It returns the nominal speed estimated by OTT, it is a
  *         floating point number codified into a 32bit integer.
  */
uint32_t OTT_GetNominalSpeed( OTT_Handle_t * pHandle );

/**
  * @brief  It returns the number of states of OTT.
  * @param  this related object of class COTT.
  * @retval uint8_t It returns the number of states of Selfcommissioning procedure.
  */
uint8_t OTT_GetSteps( OTT_Handle_t * pHandle );

/**
  * @brief  It returns the state of OTT.
  * @param  this related object of class COTT.
  * @retval uint8_t It returns the state of OTT.
  */
uint8_t OTT_GetState( OTT_Handle_t * pHandle );

/**
  * @brief  It returns true if OTT procedure has been completed, false otherwise.
  * @param  this related object of class COTT.
  * @retval bool It returns true if OTT procedure has been completed, false otherwise.
  */
bool OTT_IsSpeedPITuned( OTT_Handle_t * pHandle );

/**
  * @brief  It returns the nominal speed estimated by OTT in RPM.
  * @param  this related object of class COTT.
  * @retval float It returns the nominal speed in RPM estimated by OTT.
  */
float OTT_fGetNominalSpeedRPM( OTT_Handle_t * pHandle );

/**
  * @brief  Sets the number of motor poles pairs.
  * @param  this related object of class COTT.
  * @param  bPP Number of motor poles pairs to be set.
  * @retval none
  */
void OTT_SetPolesPairs( OTT_Handle_t * pHandle, uint8_t bPP );

/**
  * @brief  Change the nominal current.
  * @param  this related object of class COTT.
  * @param  hNominalCurrent This value represents actually the maximum Iq current
            expressed in digit.
  * @retval none
  */
void OTT_SetNominalCurrent( OTT_Handle_t * pHandle, uint16_t hNominalCurrent );

/**
  * @brief  Change the speed regulator bandwidth.
  * @param  this related object of class COTT.
  * @param  fBW Current regulator bandwidth espressed in rad/s.
  * @retval none
  */
void OTT_SetSpeedRegulatorBandwidth( OTT_Handle_t * pHandle, float fBW );

/**
  * @brief  Get the speed regulator bandwidth.
  * @param  this related object of class COTT.
  * @retval float Current regulator bandwidth espressed in rad/s.
  */
float OTT_GetSpeedRegulatorBandwidth( OTT_Handle_t * pHandle );

/**
  * @brief  Get the measured inertia of the motor.
  * @param  this related object of class COTT.
  * @retval float Measured inertia of the motor expressed in Kgm^2.
  */
float OTT_GetJ( OTT_Handle_t * pHandle );

/**
  * @brief  Get the measured friction of the motor.
  * @param  this related object of class COTT.
  * @retval float Measured friction of the motor expressed in Nms.
  */
float OTT_GetF( OTT_Handle_t * pHandle );

/**
  * @brief  Set the nominal speed according motor datasheet.
  *         This function shall be called before the start
  *         of the MP procedure.
  * @param  this related object of class COTT.
  * @param  wNominalSpeed Nominal speed expressed in RPM.
  * @retval none
  */
void OTT_SetNominalSpeed( OTT_Handle_t * pHandle, int32_t wNominalSpeed );

/**
  * @brief  Store the Ke measured by the SCC for the OTT purpouses.
  * @param  this related object of class COTT.
  * @param  fKe Last measured Ke.
  * @retval none
  */
void OTT_SetKe( OTT_Handle_t * pHandle, float fKe );

/**
  * @brief  It should be called before each motor stop.
  * @param  this related object of class COTT.
  * @retval none.
  */
void OTT_Stop( OTT_Handle_t * pHandle );

/**
  * @brief  Return true if the motor has been already profiled.
  * @param  this related object of class COTT.
  * @retval bool true if the if the motor has been already profiled,
  *         false otherwise.
  */
bool OTT_IsMotorAlreadyProfiled( OTT_Handle_t * pHandle );

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#endif /* __ONETOUCHTUNINGCLASS_H */

/******************* (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
