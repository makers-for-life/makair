/**
  ******************************************************************************
  * @file    pfc.h
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file contains all definitions and functions prototypes for the
  *          Power Factor Correction component of the Motor Control SDK.
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
  * @ingroup PFC
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PCF_H
#define __PCF_H

/** @addtogroup MCSDK
  * @{
  */

/** @addtogroup PCF
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "mc_type.h"
#include "motor_power_measurement.h"
#include "bus_voltage_sensor.h"
#include "pid_regulator.h"
#include "state_machine.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* Exported constants --------------------------------------------------------*/
/** @brief Size of the ADC Calibration array */
#define PFC_ADCCAL_ARRAY_LENGHT 32


/* Exported types ------------------------------------------------------------*/

/**
 * @brief Structure containing a PFC component's constant parameters
 */
typedef struct
{
    ADC_TypeDef * ADCx;                 /**< ADC used for PFC (Registers base address) */
    TIM_TypeDef * TIMx;                 /**< Timer used for PFC (Registers base address) */
    DMA_TypeDef * DMAx;                 /**< DMA instance used for PFC (Registers base address) */
    uint32_t DMAChannel;                /**< DMA Channel used for PFC */
    uint32_t wCPUFreq;                  /**< CPU and TIM frequency in Hertz */
    uint32_t wPWMFreq;                  /**< PWM frequency expressed in Hertz */
    uint16_t hPWMARR;                   /**< Timer Auto Reload Register value configuring the PFC PWM frequency*/
#if 0 /* ETRFault is always enabled in the current implementation */
    FunctionalState ETRFault;           /**< It enable/disable the management of
                                          *  emergency input instantaneously through ETR,
                                          *  stopping PWM generation. It must be
                                          *  either equal to ENABLE or DISABLE.
                                          *  When disabled, functionality is to be
                                          *  configured via EXTI line: in that case,
                                          *  parameters hEXTIPinSource, hEXTIPortSource
                                          *  must be defined*/
#endif /* 0 */
    uint8_t bFaultPolarity;             /**< 1 if fault detection is performed on the rising edge,
                                          *  0 if it is on the falling edge. */
    GPIO_TypeDef* hFaultPort;           /**< Fault GPIO input port (ETR/EXTI, Base address of the GPIO Port's
                                          * register base). */
    uint16_t hFaultPin;                 /**< Fault GPIO input pin (ETR/EXTI). */
#if 0 /* This is only needed when ETRFault is disabled.
         However, ETRFault is always enabled in the current implementation. */
    uint8_t hEXTIPinSource;             /**< Fault GPIO input pin, EXTI configuration:
                                          *  GPIO_PinSourcex, x=0,1,... */
#endif /* 0 */
    uint8_t bCurrentLoopRate;           /**< PWM frequency over Current control frequency ratio. */
    uint8_t bVoltageLoopRate;           /**< PFC_Scheduler() frequency to Voltage control frequency ratio. */
    int16_t hNominalCurrent;            /**< Output nominal current in s16A */
    uint16_t hMainsFreqLowTh;           /**< Minimum Mains frequency allowed for PFC operations, in tenth of Hz */
    uint16_t hMainsFreqHiTh;            /**< Maximum Mains frequency allowed for PFC operations, in tenth of Hz */
    int16_t OutputPowerActivation;      /**< Output power for PFC activation, in Watt. */
    int16_t OutputPowerDeActivation;    /**< Output power for PFC de-activation, in Watt */
    int16_t SWOverVoltageTh;            /**< Software OverVoltage threshold, in Volts */
    uint16_t hPropDelayOnTimCk;         /**< Latency at PFC switch turn-on, in TIM clock cycles*/
    uint16_t hPropDelayOffTimCk;        /**< Latency at PFC switch turn-off, in TIM clock cycles*/
    uint16_t hADCSamplingTimeTimCk;     /**< ADC sampling time, in TIM clock cycles*/
    uint16_t hADCConversionTimeTimCk;   /**< ADC conversion time, in TIM clock cycles*/
    uint16_t hADCLatencyTimeTimCk;      /**< ADC latency time, in TIM clock cycles*/
    uint16_t hMainsConversionFactor;    /**< Factor used to convert AC MAINS voltage from
                                          *  u16Volts into real Volts (V).
                                          *  1 u16Volt = 65536/hConversionFactor Volts
                                          *  For real sensors hConversionFactor it's
                                          *  equal to the product between the expected MCU
                                          *  voltage and the voltage sensing network
                                          *  attenuation */
    uint16_t hCurrentConversionFactor;  /**< Factor used to convert AC mains current from
                                          *  u16Amps into real Amperes (A) */

} PFC_Parameters_t;

/**
 * @brief Structure used to handle a PFC component
 */
typedef struct
{
  PFC_Parameters_t const * pParams;           /**< pointer on the constant parameters of the component */
  MotorPowMeas_Handle_t * pMPW1;        /**< pointer on the component measuring the power consumed by motor 1 */
  MotorPowMeas_Handle_t * pMPW2;        /**< pointer on the component measuring the power consumed by motor 2 */
  BusVoltageSensor_Handle_t *pVBS;      /**< pointer on the bus voltage measurement component */
  int16_t *pBusVoltage;                 /**< pointer on the measured bus voltage (only used if no MCs)
                                          *  @todo TODO: check whether this should really be an int16_t or an uint16_t */
  PID_Handle_t cPICurr;                 /**< PFC current loop PI regulator */
  PID_Handle_t cPIVolt;                 /**< PFC voltage loop PI regulator */
  STM_Handle_t pSTM;                    /**< PFC state machine */
  bool bPFCen;                          /**< enabling to start control */
  int16_t hTargetVoltageReference;      /**< DC output voltage, target reference level */
  int16_t hVoltageReference;            /**< DC output voltage, actual reference level */
  int16_t hVoltageLevel;                /**< DC output voltage, actual level */
  int16_t hStartUpVoltageIncrease;      /**< DC output voltage increase during startup */
  int16_t hStartUpInitialLevel;         /**< Initial startup voltage level */
  int16_t hStartUpDuration;             /**< Duration of the startup phase, number of steps
                                          *  (A "step" is one execution of the PFC Voltage regulation function.
                                          *  So, the duration of a "step" is 1/@p PFC_VOLTFREQUENCY). */
  int16_t hStartUpStep;                 /**< Startup phase, current step (@sa PFC_Handle_t::hStartUpStep). */
  int16_t hCurrentReferenceAmpl;        /**< DC output current, amplitude reference level */
  int16_t hCurrentReference;            /**< DC output current, reference level. */
#if 0 /* This field is unused in the current implementation */
  int16_t hCurrentLevel;                /**< DC output current, actual level TODO: If this is not used, remove it. */
#endif /* 0 */
  int16_t hDuty;                        /**< PWM duty cycle, int16_t */
  int16_t hFirstTrig;                   /**< First ADC trigger */
  int16_t hSecondTrig;                  /**< Second ADC trigger */
  bool bDualSampling;                   /**< Dual sampling enabled*/
  uint8_t bVoltLoopCnt;                 /**< voltage loop task, counter*/
  uint8_t bCurrLoopCnt;                 /**< current loop task, counter*/
  uint16_t hMainsSync;                  /**< Mains Voltage synchronization period counter.
                                          *  This counter is incremented each time the high frequency task of the PFC is executed and
                                          *  reset on each V Mains period (triggered by Timer's Channel 1 CC input). This counts the number
                                          *  of High Frequency PFC task cycles for a V Mains period, allowing to measure the frequency of
                                          *  the Mains AC Voltage supply. */
  uint16_t hMainsPeriod;                /**< Mains Voltage period. Synchronized with Mains frequency and set to the same value. */
  uint16_t hMainsFrequency;             /**< Mains frequency, tenth of Hertz*/
  uint16_t hMainsFrequencyHiCyc;        /**< Mains frequency high threshold, PWM Cycles */
  uint16_t hMainsFrequencyLoCyc;        /**< Mains frequency low threshold, PWM Cycles */
  uint16_t hMainsVoltageS16;            /**< Mains voltage, s16V*/
  int16_t hMainsVoltageVpk;             /**< Mains voltage, volts, peak*/
  bool bMainsPk;                        /**< Mains voltage is at peak level. Indicates that the current measure
                                          *  is a peak measurement. */
  int16_t hMotorPower;                  /**< Motor power, watts */
  uint32_t hADCCalibration;             /**< used for calibration purpose */
  State_t STMState;                     /**< state machine state sample */
  bool bStartUpEnd;                     /**< startup profile generation has ended */
  uint16_t hADCCalArray[PFC_ADCCAL_ARRAY_LENGHT];   /*!< array used for current reading calibration*/
  uint8_t bADCCalArrayIndex;            /**< array used for current reading calibration, index*/
  bool bADCCalibrated;                  /**< ADC current reading is calibrated*/
  int16_t hMinDutyDualTrig;             /**< min duty cycle for the dual sampling strategy */
  int16_t h1SamplTrig;                  /** sampling point correction for 1 sampling*/
  int16_t h2SamplTrig;                  /** sampling point correction for 2 samplings*/
  uint16_t hADCTotalCk;                 /**< ADC total conversion time, TIM cks*/
  int16_t hMinDuty;                     /**< Min duty for current reading*/
  uint16_t hMaxTimCnt;                  /**< Max TIM counter allowed to set CMR regs*/
  uint8_t bMainsSynthStage;             /**< Mains synth stages completed*/
  int16_t hMainsSynth;                  /**< AC Mains sinusoid synthesized*/
  uint16_t uhindex;                     /**< AC Mains sinusoid index*/
  uint8_t bMainsFreqErrorNo;            /**< Number of errors allowed on Mains frequency calc*/
  uint8_t bMainsVoltageErrorNo;         /**< Number of errors allowed on Mains amplitude calc*/
  uint32_t JSWSTART_CR2;
} PFC_Handle_t;


/* Exported functions ------------------------------------------------------------*/

/* Initializes the PFC Component pointed by pHandle */
void PFC_Init( PFC_Handle_t * pHandle );

/* Enables the PFC component pointed by pHandle. Returns true if successful, false otherwise */
bool PFC_Enable( PFC_Handle_t * pHandle );

/* Disables the PFC component pointed by pHandle. Returns true if successful, false otherwise */
bool PFC_Disable( PFC_Handle_t * pHandle );

/* Returns true if the PFC component pointed by pHandle is enabled, false otherwise */
bool PFC_IsEnabled( PFC_Handle_t * pHandle );

/* Returns a pointer on the State Machine of the PFC component pointed by pHandle */
STM_Handle_t * PFC_GetStateMachine( PFC_Handle_t * pHandle );

/* Sets the DC boost voltage reference to be maintained by the PFC component pointed by pHandle to hVoltRef volts. */
void PFC_SetBoostVoltageReference( PFC_Handle_t * pHandle, int16_t hVoltRef );

/* Returns the DC boost voltage reference to be maintained by the PFC component pointed by pHandle, in volts. */
int16_t PFC_GetBoostVoltageReference( PFC_Handle_t * pHandle );

/* Sets the duration of the PFC startup phase of the PFC component pointed by pHandle to hStartUpDuration ms. */
void PFC_SetStartUpDuration( PFC_Handle_t * pHandle, int16_t hStartUpDuration );

/* Returns the duration of the PFC startup phase of the PFC component pointed by pHandle, in ms. */
int16_t PFC_GetStartUpDuration( PFC_Handle_t * pHandle );

/* Returns a pointer on the Current PID regulator of the PFC component pointed by pHandle */
PID_Handle_t* PFC_GetCurrentLoopPI( PFC_Handle_t * pHandle );

/* Returns a pointer on the Voltage PID regulator of the PFC component pointed by pHandle */
PID_Handle_t* PFC_GetVoltageLoopPI( PFC_Handle_t * pHandle );

/* Returns the last Mains Voltage measured by the PFC component pointed by pHandle, in volts (0-to-pk) */
int16_t PFC_GetMainsVoltage( PFC_Handle_t * pHandle );

/* Returns the last Mains Frequency measured by the PFC component pointed by pHandle, in tenth of Hz */
int16_t PFC_GetMainsFrequency( PFC_Handle_t * pHandle );

/* Returns the last average VBus value known to the PFC component pointed by pHandle, in volts. */
int16_t PFC_GetAvBusVoltage_V( PFC_Handle_t * pHandle );

/* Acknowledges faults pending on the PFC component pointed by pHandle.
 * Returns true if successful, false otherwise. */
bool PFC_AcknowledgeFaults( PFC_Handle_t * pHandle );

/* Executes the medium/low frequency PFC task. This function is called periodically */
void PFC_Task( PFC_Handle_t * pHandle );

/* handles the global interrupt request of the Timer used by the PFC component pointed by pHandle*/
void PFC_TIM_CC_IRQHandler( PFC_Handle_t * pHandle );
void PFC_TIM_TRIG_IRQHandler( PFC_Handle_t * pHandle );

/* handles External lines interrupt request */
void EXTIx_IRQHandler( PFC_Handle_t * pHandle );

/**
  * @}
  */

/**
  * @}
  */


#ifdef __cplusplus
}
#endif /* __cpluplus */

#endif /* __PCF_H */

/******************* (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
