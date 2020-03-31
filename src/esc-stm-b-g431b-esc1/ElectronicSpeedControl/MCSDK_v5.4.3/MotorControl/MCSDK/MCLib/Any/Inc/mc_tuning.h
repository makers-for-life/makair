/**
  ******************************************************************************
  * @file    mc_tuning.h
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file contains all definitions and functions prototypes for the
  *          motor control tuning component of the Motor Control SDK.
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
  * @ingroup MCTuning
  */



/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MC_TUNING_H
#define __MC_TUNING_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "mc_type.h"
#include "pid_regulator.h"
#include "revup_ctrl.h"
#include "sto_pll_speed_pos_fdbk.h"
#include "sto_cordic_speed_pos_fdbk.h"
#include "virtual_speed_sensor.h"
#include "speed_torq_ctrl.h"
#include "digital_output.h"
#ifdef HFINJECTION
#include "hifreqinj_fpu_ctrl.h"
#endif /* HFINJECTION */
#include "motor_power_measurement.h"
#include "pqd_motor_power_measurement.h"
#include "open_loop.h"
#include "ntc_temperature_sensor.h"
#include "bus_voltage_sensor.h"
#include "feed_forward_ctrl.h"
#include "flux_weakening_ctrl.h"
#include "state_machine.h"
#include "mp_self_com_ctrl.h"
#include "trajectory_ctrl.h"

/**
 * @addtogroup MCSDK
  * @{
  */

/**
 * @defgroup MCTuning Motor Control Fine Tuning interface
 *
 * @brief This interface provides access to the internals of the Motor Control Subsystem
 *
 * @todo Complete documentation
 * @{
 */

/**
 * @internal
 * @brief  Public DigitalOutput class definition
 */
#ifndef __DIGITALOUTPUTCLASS_H
typedef struct CDOUT_t * CDOUT;
#endif



/**
  * @brief  MC tuning internal objects initialization structure type;
  */
typedef struct
{
  PID_Handle_t * pPIDSpeed;
  PID_Handle_t * pPIDIq;
  PID_Handle_t * pPIDId;
  PID_Handle_t * pPIDFluxWeakening;
  PWMC_Handle_t * pPWMnCurrFdbk;
  RevUpCtrl_Handle_t * pRevupCtrl;
  SpeednPosFdbk_Handle_t * pSpeedSensorMain;
  SpeednPosFdbk_Handle_t * pSpeedSensorAux;
  VirtualSpeedSensor_Handle_t * pSpeedSensorVirtual;
  SpeednTorqCtrl_Handle_t * pSpeednTorqueCtrl;
  STM_Handle_t * pStateMachine;
  NTC_Handle_t * pTemperatureSensor;
  BusVoltageSensor_Handle_t * pBusVoltageSensor;
  DOUT_handle_t * pBrakeDigitalOutput;
  DOUT_handle_t * pNTCRelay;
  MotorPowMeas_Handle_t * pMPM;
  FW_Handle_t  * pFW;
  FF_Handle_t  * pFF;
  PosCtrl_Handle_t *  pPosCtrl;  
#ifdef HFINJECTION
  HFI_FP_Ctrl_Handle_t * pHFI;
#endif /* HFINJECTION */
  SCC_Handle_t *  pSCC;
  OTT_Handle_t *  pOTT;
} MCT_Handle_t;


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

#if 0
/**
  * @brief  Use this method to set new values for the constants utilized by
  *         feed-forward algorithm.
  * @param  pHandle Feed forward  strutcture.
  * @param  sNewConstants The FF_TuningStruct_t containing constants utilized by
  *         feed-forward algorithm.
  * @retval none
  */
void FF_SetFFConstants( FF_Handle_t * pHandle, FF_TuningStruct_t sNewConstants );

/**
  * @brief  Use this method to get present values for the constants utilized by
  *         feed-forward algorithm.
  * @param  pHandle Feed forward  strutcture.
  * @retval FF_TuningStruct_t Values of the constants utilized by
  *         feed-forward algorithm.
  */
FF_TuningStruct_t FF_GetFFConstants( FF_Handle_t * pHandle );

/**
  * @brief  Use this method to get present values for the Vqd feed-forward
  *         components.
  * @param  pHandle Feed forward  strutcture.
  * @retval qd_t Vqd feed-forward components.
  */
qd_t FF_GetVqdff( FF_Handle_t * pHandle );

/**
  * @brief  Use this method to get values of the averaged output of qd axes
  *         currents PI regulators.
  * @param  pHandle Feed forward  strutcture.
  * @retval qd_t Averaged output of qd axes currents PI regulators.
  */
qd_t FF_GetVqdAvPIout( FF_Handle_t * pHandle );
#endif
#ifdef HFINJECTION
/**
  * @brief  It returns the rotor angle lock value
  * @param  pHandle related HFI_FP_Ctrl_Handle
  * @retval int16_t Rotor angle lock value
  */
int16_t HFI_FP_GetRotorAngleLock( HFI_FP_Ctrl_Handle_t * pHandle );

/**
  * @brief  It returns the saturation difference measured during the last
  *         north/south identification stage.
  * @param  pHandle related HFI_FP_Ctrl_Handle
  * @retval int16_t Saturation difference measured during the last north/south
  *         identification stage.
  */
int16_t HFI_FP_GetSaturationDifference( HFI_FP_Ctrl_Handle_t * pHandle );

/**
  * @brief  It return the quantity that shall be put in the DAC to tune the HFI
  * @param  pHandle related HFI_FP_Ctrl_Handle
  * @retval int16_t HFI current
  */
int16_t HFI_FP_GetCurrent( HFI_FP_Ctrl_Handle_t * pHandle );

/**
  * @brief  It returns the Track PI
  * @param  pHandle related HFI_FP_Ctrl_Handle
  * @retval CPI Track PI
  */
PID_Handle_t * HFI_FP_GetPITrack( HFI_FP_Ctrl_Handle_t * pHandle );

/**
  * @brief  It set the min saturation difference used to validate the
  *         north/south identification stage.
  * @param  pHandle related HFI_FP_Ctrl_Handle
  * @param  hMinSaturationDifference Min Saturation difference used to validate
  *         the north/south identification stage.
  *         identification stage.
  * @retval none
  */
void HFI_FP_SetMinSaturationDifference( HFI_FP_Ctrl_Handle_t * pHandle, int16_t MinSaturationDifference );
#endif /* HFINJECTION */

/**
  * @brief  It allows changing applied open loop phase voltage.
  * @param  pHandle related object of class COL
  * @param  hNewVoltage New voltage value to be applied by the open loop.
  * @retval None
  */
void OL_UpdateVoltage( OpenLoop_Handle_t * pHandle, int16_t hNewVoltage );

/**
  * @brief  It updates the Kp gain
  * @param  CPI PI object
  * @param  int16_t New Kp gain
  * @retval None
  */
void PID_SetKP( PID_Handle_t * pHandle, int16_t hKpGain );

/**
  * @brief  It updates the Ki gain
  * @param  CPI PI object
  * @param  int16_t New Ki gain
  * @retval None
  */
void PID_SetKI( PID_Handle_t * pHandle, int16_t hKiGain );

/**
  * @brief  It returns the Kp gain of the passed PI object
  * @param  CPI PI regulator object
  * @retval int16_t Kp gain
  */
int16_t PID_GetKP( PID_Handle_t * pHandle );

/**
  * @brief  It returns the Kp gain divisor of the passed PI object
  * @param  CPI PI regulator object
  * @retval int16_t Kp gain
  */
uint16_t PID_GetKPDivisor( PID_Handle_t * pHandle );

/**
  * @brief  It returns the Ki gain of the passed PI object
  * @param  CPI PI regulator object
  * @retval int16_t Ki gain
  */
int16_t PID_GetKI( PID_Handle_t * pHandle );

/**
  * @brief  It returns the Ki gain divisor of the passed PI object
  * @param  CPI PI regulator object
  * @retval int16_t Ki gain
  */
uint16_t PID_GetKIDivisor( PID_Handle_t * pHandle );

/**
  * @brief  It returns the Default Kp gain of the passed PI object
  * @param  CPI PI regulator object
  * @retval int16_t Kp gain
  */
int16_t PID_GetDefaultKP( PID_Handle_t * pHandle );

/**
  * @brief  It returns the Default Ki gain of the passed PI object
  * @param  CPI PI regulator object
  * @retval int16_t Ki gain
  */
int16_t PID_GetDefaultKI( PID_Handle_t * pHandle );

/**
  * @brief  It set a new value into the PI integral term
  * @param  CPI PI regulator object
  * @param  int32_t New integral term value
  * @retval None
  */
void PID_SetIntegralTerm( PID_Handle_t * pHandle, int32_t wIntegralTermValue );

/**
  * @brief  It set a new value into the PID Previous error variable required to
  *         compute derivative term
  * @param  pHandle regulator object
  * @param  wPrevProcessVarError New integral term value
  * @retval None
  */
void PID_SetPrevError( PID_Handle_t * pHandle, int32_t wPrevProcessVarError );

/**
  * @brief  It updates the Kd gain
  * @param  pHandle PID regulator object
  * @param  hKdGain New Kd gain
  * @retval None
  */
void PID_SetKD( PID_Handle_t * pHandle, int16_t hKdGain );

/**
  * @brief  It returns the Kd gain of the PID object passed
  * @param  pHandle PID regulator object
  * @retval int16_t Kd gain
  */
int16_t PID_GetKD( PID_Handle_t * pHandle );

/**
* @brief  Execute a regular conversion using ADC1.
*         The function is not re-entrant (can't executed twice at the same time)
*         It returns 0xFFFF in case of conversion error.
* @param  pHandle PWM component handler
* @param  bChannel ADC channel used for the regular conversion
* @retval It returns converted value or oxFFFF for conversion error
*/
uint16_t PWMC_ExecRegularConv( PWMC_Handle_t * pHandle, uint8_t bChannel );

/**
* @brief  It sets the specified sampling time for the specified ADC channel
*         on ADC1. It must be called once for each channel utilized by user
* @param  pHandle PWM component handler
* @param  ADConv_struct struct containing ADC channel and sampling time
* @retval none
*/
void PWMC_ADC_SetSamplingTime( PWMC_Handle_t * pHandle, ADConv_t ADConv_struct );

/**
  * @brief  It is used to modify the default value of duration of a specific
  *         rev up phase.
  *         Note: The module can be also compiled commenting the
  *         define RUC_ALLOWS_TUNING to optimize the flash memory occupation
  *         and the RAM usage if the tuning is not required in this case this
  *         function has no effect.
  * @param  pHandle related object of class CRUC.
  * @param  bPhase is the rev up phase, zero based, to be modified.
  * @param  hDurationms is the new value of duration for that phase.
  * @retval none.
  */
void RUC_SetPhaseDurationms( RevUpCtrl_Handle_t * pHandle, uint8_t bPhase, uint16_t hDurationms );

/* Function used to set the targeted motor speed at the end of a specific phase. */
void RUC_SetPhaseFinalMecSpeedUnit( RevUpCtrl_Handle_t * pHandle, uint8_t bPhase,
                                    int16_t hFinalMecSpeedUnit );

/**
  * @brief  It is used to modify the default value of motor torque at the end of
  *         a specific rev up phase.
  *         Note: The module can be also compiled commenting the
  *         define RUC_ALLOWS_TUNING to optimize the flash memory occupation
  *         and the RAM usage if the tuning is not required in this case this
  *         function has no effect.
  * @param  pHandle related object of class CRUC.
  * @param  bPhase is the rev up phase, zero based, to be modified.
  * @param  hFinalTorque is the new value of motor torque at the end of that
  *         phase. This value represents actually the Iq current expressed in
  *         digit.
  * @retval none.
  */
void RUC_SetPhaseFinalTorque( RevUpCtrl_Handle_t * pHandle, uint8_t bPhase, int16_t hFinalTorque );

/**
  * @brief  It is used to read the current value of duration of a specific rev
  *         up phase.
  *         Note: The module can be also compiled commenting the
  *         define RUC_ALLOWS_TUNING to optimize the flash memory occupation
  *         and the RAM usage if the tuning is not required in this case this
  *         function has no effect.
  * @param  pHandle related object of class CRUC.
  * @param  bPhase is the rev up phase, zero based, to be read.
  * @retval uint16_t The current value of duration for that phase expressed in
  *         milliseconds.
  */
uint16_t RUC_GetPhaseDurationms( RevUpCtrl_Handle_t * pHandle, uint8_t bPhase );

/* Function used to read the targeted mechanical speed of a specific RevUp phase */
int16_t RUC_GetPhaseFinalMecSpeedUnit( RevUpCtrl_Handle_t * pHandle, uint8_t bPhase );

/**
  * @brief  It is used to read the current value of motor torque at the end of a
  *         specific rev up phase.
  *         Note: The module can be also compiled commenting the
  *         define RUC_ALLOWS_TUNING to optimize the flash memory occupation
  *         and the RAM usage if the tuning is not required in this case this
  *         function has no effect.
  * @param  pHandle related object of class CRUC.
  * @param  bPhase is the rev up phase, zero based, to be read.
  * @retval int16_t The current value of motor torque at the end of that phase.
  *         This value represents actually the Iq current expressed in digit.
  */
int16_t RUC_GetPhaseFinalTorque( RevUpCtrl_Handle_t * pHandle, uint8_t bPhase );

/**
  * @brief  It is used to get information about the number of phases relative to
  *         the programmed rev up.
  *         Note: The module can be also compiled commenting the
  *         define RUC_ALLOWS_TUNING to optimize the flash memory occupation
  *         and the RAM usage if the tuning is not required in this case this
  *         function has no effect.
  * @param  pHandle related object of class CRUC.
  * @retval uint8_t The number of phases relative to the programmed rev up.
  */
uint8_t RUC_GetNumberOfPhases( RevUpCtrl_Handle_t * pHandle );

/**
  * @brief  Returns latest averaged temperature measurement expressed in Celsius degrees
  *
  * @param  pHandle: Pointer on Handle structure of TemperatureSensor component
  *
  * @retval Latest averaged temperature measurement in Celsius degrees
  */
int16_t NTC_GetAvTemp_C( NTC_Handle_t * pHandle );

/**
  * @brief  Returns Temperature mesurement fault status
  * Fault can be either MC_OVER_TEMP or MC_NO_ERROR according on protection threshold values set
  *
  * @param  pHandle: Pointer on Handle structure of TemperatureSensor component.
  *
  * @retval Fault code error
  */
uint16_t NTC_CheckTemp( NTC_Handle_t * pHandle );

/**
  * @brief It returns the state of the digital output
  * @param this object of class DOUT
  * @retval OutputState_t Digital output state (ACTIVE or INACTIVE)
  */
DOutputState_t DOUT_GetOutputState( DOUT_handle_t * pHandle );

/**
  * @brief  It returns the state of Selfcommissioning procedure.
  * @param  this related object of class CSCC.
  * @retval uint8_t It returns the state of Selfcommissioning procedure.
  */
uint8_t SCC_GetState( SCC_Handle_t * pHandle );

/**
  * @brief  It returns the number of states of Selfcommissioning procedure.
  * @param  this related object of class CSCC.
  * @retval uint8_t It returns the number of states of Selfcommissioning procedure.
  */
uint8_t SCC_GetSteps( SCC_Handle_t * pHandle );

/**
  * @brief  It returns the measured Rs.
  * @param  this related object of class CSCC.
  * @retval uint32_t It returns the measured Rs, it is a floating point number
  *         codified into a 32bit integer.
  */
uint32_t SCC_GetRs( SCC_Handle_t * pHandle );

/**
  * @brief  It returns the measured Ls.
  * @param  this related object of class CSCC.
  * @retval uint32_t It returns the measured Ls, it is a floating point number
  *         codified into a 32bit integer.
  */
uint32_t SCC_GetLs( SCC_Handle_t * pHandle );

/**
  * @brief  It returns the measured Ke.
  * @param  this related object of class CSCC.
  * @retval uint32_t It returns the measured Ke, it is a floating point number
  *         codified into a 32bit integer.
  */
uint32_t SCC_GetKe( SCC_Handle_t * pHandle );

/**
  * @brief  It returns the measured VBus.
  * @param  this related object of class CSCC.
  * @retval uint32_t It returns the measured Vbus, it is a floating point number
  *         codified into a 32bit integer.
  */
uint32_t SCC_GetVbus( SCC_Handle_t * pHandle );

/**
  * @brief  It returns the nominal speed estimated from Ke.
  * @param  this related object of class CSCC.
  * @retval uint32_t It returns the nominal speed estimated from Ke, it is a
  *         floating point number codified into a 32bit integer.
  */
uint32_t SCC_GetEstNominalSpeed( SCC_Handle_t * pHandle );

/**
  * @brief  Call this method before start motor to force new motor profile.
  * @param  this related object of class CSCC.
  * @retval none
  */
void SCC_ForceProfile( SCC_Handle_t * pHandle );

/**
  * @brief  Call this method to force end of motor profile.
  * @param  this related object of class CSCC.
  * @retval none
  **/
void SCC_StopProfile( SCC_Handle_t * pHandle );

/**
  * @brief  Sets the number of motor poles pairs.
  *         This function shall be called before the start
  *         of the MP procedure.
  * @param  this related object of class CSCC.
  * @param  bPP Number of motor poles pairs to be set.
  * @retval none
  */
void SCC_SetPolesPairs( SCC_Handle_t * pHandle, uint8_t bPP );

/**
  * @brief  Change the current used for RL determination.
            Usually is the nominal current of the motor.
  *         This function shall be called before the start
  *         of the MP procedure.
  * @param  this related object of class CSCC.
  * @param  fCurrent Current used for RL determination.
  * @retval none
  */
void SCC_SetNominalCurrent( SCC_Handle_t * pHandle, float fCurrent );

/**
  * @brief  Get the nominal current used for RL determination.
  * @param  this related object of class CSCC.
  * @retval float Nominal current used for RL determination.
  */
float SCC_GetNominalCurrent( SCC_Handle_t * pHandle );

/**
  * @brief  Set the Ld/Lq ratio.
  *         This function shall be called before the start
  *         of the MP procedure.
  * @param  this related object of class CSCC.
  * @param  fLdLqRatio New value of Lq/Lq ratio used by MP for tuning of
            current regulators.
  * @retval none
  */
void SCC_SetLdLqRatio( SCC_Handle_t * pHandle, float fLdLqRatio );

/**
  * @brief  Get the Ld/Lq ratio.
  * @param  this related object of class CSCC.
  * @retval float New value of Lq/Lq ratio used by MP for tuning of
            current regulators.
  */
float SCC_GetLdLqRatio( SCC_Handle_t * pHandle );

/**
  * @brief  Set the nominal speed according motor datasheet.
  *         This function shall be called before the start
  *         of the MP procedure.
  * @param  this related object of class CSCC.
  * @param  wNominalSpeed Nominal speed expressed in RPM.
  * @retval none
  */
void SCC_SetNominalSpeed( SCC_Handle_t * pHandle, int32_t wNominalSpeed );

/**
  * @brief  Get the last nominal speed set by SCC_SetNominalSpeed.
  *         Note that this is not the estimated one.
  * @param  this related object of class CSCC.
  * @retval int32_t Nominal speed expressed in RPM.
  */
int32_t SCC_GetNominalSpeed( SCC_Handle_t * pHandle );

/**
  * @brief  Get the estimated maximum speed that can be
  *         sustatined in the startup open loop acceleration.
  *         This function must be called only after that the
  *         MP procedure is completed succesfully.
  * @param  this related object of class CSCC.
  * @retval int32_t Estimated maximum open loop speed expressed in RPM.
  */
int32_t SCC_GetEstMaxOLSpeed( SCC_Handle_t * pHandle );

/**
  * @brief  Get the estimated maximum acceleration that can be
  *         sustatined in the startup using the estimated
  *         startup current. You can retireve the max startup
  *         current using the SCC_GetStartupCurrentX function.
  *         This function must be called only after that the
  *         MP procedure is completed succesfully.
  * @param  this related object of class CSCC.
  * @retval int32_t Estimated maximum open loop acceleration
  *         espressed in RPM/s.
  */
int32_t SCC_GetEstMaxAcceleration( SCC_Handle_t * pHandle );

/**
  * @brief  Get the estimated maximum statup current that
  *         can be applied to the selected motor.
  *         This function must be called only after that the
  *         MP procedure is completed succesfully.
  * @param  this related object of class CSCC.
  * @retval int16_t Estimated maximum open loop current
  *         espressed in s16int.
  */
int16_t SCC_GetStartupCurrentS16( SCC_Handle_t * pHandle );

/**
  * @brief  Get the estimated maximum statup current that
  *         can be applied to the selected motor.
  *         This function must be called only after that the
  *         MP procedure is completed succesfully.
  * @param  this related object of class CSCC.
  * @retval int16_t Estimated maximum open loop current
  *         espressed in Ampere.
  */
float SCC_GetStartupCurrentAmp( SCC_Handle_t * pHandle );

/**
  * @brief  Set the bandwidth used to tune the current regulators.
  *         This function shall be called before the start
  *         of the MP procedure.
  * @param  this related object of class CSCC.
  * @param  fCurrentBW Bandwidth used to tune the current regulators expressed in rad/s.
  * @retval none
  */
void SCC_SetCurrentBandwidth( SCC_Handle_t * pHandle, float fCurrentBW );

/**
  * @brief  Get the bandwidth used to tune the current regulators.
  * @param  this related object of class CSCC.
  *         This function must be called only after that the
  *         MP procedure is completed succesfully.
  * @retval float Bandwidth used to tune the current regulators expressed in
  *         rad/s.
  */
float SCC_GetCurrentBandwidth( SCC_Handle_t * pHandle );

/**
  * @brief  Get the PWM frequency used by the test.
  * @param  this related object of class CSCC.
  * @retval uint16_t PWM frequency used by the test expressed in Hz.
  */
uint16_t SCC_GetPWMFrequencyHz( SCC_Handle_t * pHandle );

/**
  * @brief  Get the FOC repetition rate. It is the number of PWM
  *         periods elapsed before executing one FOC control cycle.
  * @param  this related object of class CSCC.
  * @retval uint8_t FOC repetition used by the test.
  */
uint8_t SCC_GetFOCRepRate( SCC_Handle_t * pHandle );

/**
  * @brief  Call this method before start motor to force new OTT procedure.
  * @param  this related object of class COTT.
  * @retval none.
  */
void OTT_ForceTuning( OTT_Handle_t * pHandle  );

/**
  * @brief  It returns the nominal speed estimated by OTT.
  * @param  this related object of class COTT.
  * @retval uint32_t It returns the nominal speed estimated by OTT, it is a
  *         floating point number codified into a 32bit integer.
  */
uint32_t OTT_GetNominalSpeed( OTT_Handle_t * pHandle  );

/**
  * @brief  It returns the number of states of OTT.
  * @param  this related object of class COTT.
  * @retval uint8_t It returns the number of states of Selfcommissioning procedure.
  */
uint8_t OTT_GetSteps( OTT_Handle_t * pHandle  );

/**
  * @brief  It returns the state of OTT.
  * @param  this related object of class COTT.
  * @retval uint8_t It returns the state of OTT.
  */
uint8_t OTT_GetState( OTT_Handle_t * pHandle  );

/**
  * @brief  It returns true if OTT procedure has been completed, false otherwise.
  * @param  this related object of class COTT.
  * @retval bool It returns true if OTT procedure has been completed, false otherwise.
  */
bool OTT_IsSpeedPITuned( OTT_Handle_t * pHandle  );

/**
  * @brief  It returns the nominal speed estimated by OTT in RPM.
  * @param  this related object of class COTT.
  * @retval float It returns the nominal speed estimated by OTT in RPM.
  */
float OTT_fGetNominalSpeedRPM( OTT_Handle_t * pHandle  );

/**
  * @brief  Sets the number of motor poles pairs.
  * @param  this related object of class COTT.
  * @param  bPP Number of motor poles pairs to be set.
  * @retval none
  */
void OTT_SetPolesPairs( OTT_Handle_t * pHandle , uint8_t bPP );

/**
  * @brief  Change the nominal current .
  * @param  this related object of class COTT.
  * @param  hNominalCurrent This value represents actually the maximum Iq current
            expressed in digit.
  * @retval none
  */
void OTT_SetNominalCurrent( OTT_Handle_t * pHandle , uint16_t hNominalCurrent );

/**
  * @brief  Change the speed regulator bandwidth.
  * @param  this related object of class COTT.
  * @param  fBW Current regulator bandwidth espressed in rad/s.
  * @retval none
  */
void OTT_SetSpeedRegulatorBandwidth( OTT_Handle_t * pHandle , float fBW );

/**
  * @brief  Get the speed regulator bandwidth.
  * @param  this related object of class COTT.
  * @retval float Current regulator bandwidth espressed in rad/s.
  */
float OTT_GetSpeedRegulatorBandwidth( OTT_Handle_t * pHandle  );

/**
  * @brief  Get the measured inertia of the motor.
  * @param  this related object of class COTT.
  * @retval float Measured inertia of the motor expressed in Kgm^2.
  */
float OTT_GetJ( OTT_Handle_t * pHandle  );

/**
  * @brief  Get the measured friction of the motor.
  * @param  this related object of class COTT.
  * @retval float Measured friction of the motor expressed in Nms.
  */
float OTT_GetF( OTT_Handle_t * pHandle  );

/**
  * @brief  Return true if the motor has been already profiled.
  * @param  this related object of class COTT.
  * @retval bool true if the if the motor has been already profiled,
  *         false otherwise.
  */
bool OTT_IsMotorAlreadyProfiled( OTT_Handle_t * pHandle  );

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif /* __cpluplus */

#endif /* __MC_TUNING_H */

/************************ (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
