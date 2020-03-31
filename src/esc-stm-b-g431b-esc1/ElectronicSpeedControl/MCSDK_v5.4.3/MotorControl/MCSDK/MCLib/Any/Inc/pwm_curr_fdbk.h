/**
  ******************************************************************************
  * @file    pwm_curr_fdbk.h
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file contains all definitions and functions prototypes for the
  *          PWM & Current Feedback component of the Motor Control SDK.
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
  * @ingroup pwm_curr_fdbk
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef PWMNCURRFDBK_H
#define PWMNCURRFDBK_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "mc_type.h"

/** @addtogroup MCSDK
  * @{
  */

/** @addtogroup pwm_curr_fdbk
  * @{
  */

/* Exported defines ------------------------------------------------------------*/

#define SECTOR_1  0u
#define SECTOR_2  1u
#define SECTOR_3  2u
#define SECTOR_4  3u
#define SECTOR_5  4u
#define SECTOR_6  5u
#define SQRT3FACTOR (uint16_t) 0xDDB4 /* = (16384 * 1.732051 * 2)*/

/* Exported types ------------------------------------------------------------*/

/** @brief PWM & Current Sensing component handle type */
typedef struct PWMC_Handle PWMC_Handle_t;

/**
  * @brief Pointer on callback functions used by PWMC components
  *
  * This type is needed because the actual functions to use can change at run-time.
  *
  * See the following items:
  * - PWMC_Handle::pFctSwitchOffPwm
  * - PWMC_Handle::pFctSwitchOnPwm
  * - PWMC_Handle::pFctCurrReadingCalib
  * - PWMC_Handle::pFctTurnOnLowSides
  * - PWMC_Handle::pFctRLDetectionModeEnable
  * - PWMC_Handle::pFctRLDetectionModeDisable
  *
  *
  */
typedef void ( *PWMC_Generic_Cb_t )( PWMC_Handle_t * pHandle );

/**
  * @brief Pointer on the interrupt handling function of the PWMC component instance.
  *
  * This type is needed because the actual function to use can change at run-time
  * (See PWMC_Handle::pFctIrqHandler).
  *
  */
typedef void * ( *PWMC_IrqHandler_Cb_t )( PWMC_Handle_t * pHandle, unsigned char flag );

/**
  * @brief Pointer on the function provided by the PMWC component instance to get the phase current.
  *
  * This type is needed because the actual function to use can change at run-time
  * (See PWMC_Handle::pFctGetPhaseCurrents).
  *
  */
typedef void ( *PWMC_GetPhaseCurr_Cb_t )( PWMC_Handle_t * pHandle, ab_t * Iab );

/**
  * @brief Pointer on the function provided by the PMWC component instance to set the reference
  *        voltage for the over current protection.
  *
  * This type is needed because the actual function to use can change at run-time
  * (See PWMC_Handle::pFctOCPSetReferenceVoltage).
  *
  */
typedef void ( *PWMC_SetOcpRefVolt_Cb_t )( PWMC_Handle_t * pHandle, uint16_t hDACVref );

/**
  * @brief Pointer on the functions provided by the PMWC component instance to set the ADC sampling
  *        point for each sectors.
  *
  * This type is needed because the actual function to use can change at run-time. See:
  * - PWMC_Handle::pFctSetADCSampPointSect1
  * - PWMC_Handle::pFctSetADCSampPointSect2
  * - PWMC_Handle::pFctSetADCSampPointSect3
  * - PWMC_Handle::pFctSetADCSampPointSect4
  * - PWMC_Handle::pFctSetADCSampPointSect5
  * - PWMC_Handle::pFctSetADCSampPointSect6
  *
  */
typedef uint16_t ( *PWMC_SetSampPointSectX_Cb_t )( PWMC_Handle_t * pHandle);

/**
  * @brief Pointer on the function provided by the PMWC component instance to check if an over current
  *        condition has occured.
  *
  * This type is needed because the actual function to use can change at run-time
  * (See PWMC_Handle::pFctIsOverCurrentOccurred).
  *
  */
typedef uint16_t ( *PWMC_OverCurr_Cb_t )( PWMC_Handle_t * pHandle );

/**
  * @brief Pointer on the function provided by the PMWC component instance to set the PWM duty cycle
  *        in RL detection mode.
  *
  * This type is needed because the actual function to use can change at run-time
  * (See PWMC_Handle::pFctRLDetectionModeSetDuty).
  *
  */
typedef uint16_t ( *PWMC_RLDetectSetDuty_Cb_t )( PWMC_Handle_t * pHandle, uint16_t hDuty );

/**
  * @brief This structure is used to handle the data of an instance of the PWM & Current Feedback component
  *
  */
struct PWMC_Handle
{
  /** @{ */
  PWMC_IrqHandler_Cb_t pFctIrqHandler;                   /**< pointer on the interrupt handling function. */
  PWMC_GetPhaseCurr_Cb_t
  pFctGetPhaseCurrents;           /**< pointer on the function the component instance uses to retrieve pahse currents */
  PWMC_Generic_Cb_t
  pFctSwitchOffPwm;                    /**< pointer on the function the component instance uses to switch PWM off */
  PWMC_Generic_Cb_t
  pFctSwitchOnPwm;                     /**< pointer on the function the component instance uses to switch PWM on */
  PWMC_Generic_Cb_t
  pFctCurrReadingCalib;                /**< pointer on the function the component instance uses to calibrate the current reading ADC(s) */
  PWMC_Generic_Cb_t
  pFctTurnOnLowSides;                  /**< pointer on the function the component instance uses to turn low sides on */
  PWMC_SetSampPointSectX_Cb_t
  pFctSetADCSampPointSectX;  /**< pointer on the function the component instance uses to set the ADC sampling point  */
  PWMC_OverCurr_Cb_t
  pFctIsOverCurrentOccurred;          /**< pointer on the function the component instance uses to return the over current status */
  PWMC_SetOcpRefVolt_Cb_t
  pFctOCPSetReferenceVoltage;    /**< pointer on the function the component instance uses to set the over current reference voltage */
  PWMC_Generic_Cb_t
  pFctRLDetectionModeEnable;           /**< pointer on the function the component instance uses to enable RL detection mode */
  PWMC_Generic_Cb_t
  pFctRLDetectionModeDisable;          /**< pointer on the function the component instance uses to disable RL detection mode */
  PWMC_RLDetectSetDuty_Cb_t
  pFctRLDetectionModeSetDuty;  /**< pointer on the function the component instance uses to set the PWM duty cycle in RL detection mode */
  /** @} */
  uint16_t  hT_Sqrt3;                                    /**< a constant utilized by PWM algorithm (@f$\sqrt{3}@f$) */
  uint16_t  CntPhA;                                     /**< PWM Duty cycle for phase A */
  uint16_t  CntPhB;                                     /**< PWM Duty cycle for phase B */
  uint16_t  CntPhC;                                     /**< PWM Duty cycle for phase C */
  uint16_t  SWerror;                                     /**< Contains status about SW error */
  uint8_t   Sector;                                     /**< the space vector sector number */
  uint16_t  lowDuty;
  uint16_t  midDuty;
  uint16_t  highDuty;
  bool TurnOnLowSidesAction;                            /**< true if TurnOnLowSides action is active,
                                                              false otherwise. */
  uint16_t  OffCalibrWaitTimeCounter;                   /**< Counter to wait fixed time before motor
                                                              current measurement offset calibration. */
  uint8_t   Motor;                                      /**< Motor reference number */
  bool      RLDetectionMode;                             /**< true if enabled, false if disabled. */
  int16_t   Ia;                                         /**< Last @f$I_{A}@f$ measurement. */
  int16_t   Ib;                                         /**< Last @f$I_{B}@f$ measurement. */
  int16_t   Ic;                                         /**< Last @f$I_{C}@f$ measurement. */
  uint16_t  DTTest;                                      /**< Reserved */
 
  /* former  PWMnCurrFdbkParams_t */
  uint16_t PWMperiod;                                   /**< PWM period expressed in timer clock cycles unit:
                                                           *  @f$hPWMPeriod = TimerFreq_{CLK} / F_{PWM}@f$    */
  uint16_t OffCalibrWaitTicks;                          /**< Wait time duration before current reading
                                                           *  calibration expressed in number of calls
                                                           *  of PWMC_CurrentReadingCalibr() with action
                                                           *  #CRC_EXEC */
  uint16_t DTCompCnt;                                   /**< Half of Dead time expressed
                                                           *  in timer clock cycles unit:
                                                           *  @f$hDTCompCnt = (DT_s \cdot TimerFreq_{CLK})/2@f$ */
  uint16_t  Ton;                                         /**< Reserved */
  uint16_t  Toff;                                        /**< Reserved */

};

/**
  * @brief  Current reading calibration definition
  */
typedef enum CRCAction
{
  CRC_START, /**< Initialize the current reading calibration.*/
  CRC_EXEC   /**< Execute the current reading calibration.*/
} CRCAction_t;


/* Used to get the motor phase current in ElectricalValue format as read by AD converter */
void PWMC_GetPhaseCurrents( PWMC_Handle_t * pHandle,
                            ab_t * Iab );

/*  Converts input voltage components Valfa, beta into duty cycles and feed it to the inverter */
uint16_t PWMC_SetPhaseVoltage( PWMC_Handle_t * pHandle,
                               alphabeta_t Valfa_beta );

/* Switches the PWM generation off, setting the outputs to inactive */
void PWMC_SwitchOffPWM( PWMC_Handle_t * pHandle );

/* Switches the PWM generation on */
void PWMC_SwitchOnPWM( PWMC_Handle_t * pHandle );

/* Calibrates ADC current conversions by reading the offset voltage
 * present on ADC pins when no motor current is flowing.
 * This function should be called before each motor start-up */
bool PWMC_CurrentReadingCalibr( PWMC_Handle_t * pHandle,
                                CRCAction_t action );

/* Turns low sides on. This function is intended to be used for
 *  charging boot capacitors of driving section. It has to be called on each
 *  motor start-up when using high voltage drivers. */
void PWMC_TurnOnLowSides( PWMC_Handle_t * pHandle );

/* Executes a regular conversion using the first ADC used for current reading.
 * The function is not re-entrant (it cannot executed twice at the same time).
 * Returns 0xFFFF in case of conversion error. */
uint16_t PWMC_ExecRegularConv( PWMC_Handle_t * pHandle,
                               uint8_t bChannel );

/* Sets the specified sampling time for the specified ADC channel on the first ADC used for current
 *  reading. Must be called once for each channel utilized by user */
void PWMC_ADC_SetSamplingTime( PWMC_Handle_t * pHandle,
                               ADConv_t ADConv_struct );

/* Checks if an over current occurred since last call. */
uint16_t PWMC_CheckOverCurrent( PWMC_Handle_t * pHandle );

/* Sets the over current threshold through the DAC reference voltage. */
void PWMC_OCPSetReferenceVoltage( PWMC_Handle_t * pHandle,
                                  uint16_t hDACVref );

/* Retrieves the status of the "TurnOnLowSides" action. */
bool PWMC_GetTurnOnLowSidesAction( PWMC_Handle_t * pHandle );

/* Enables the RL Detection mode. */
void PWMC_RLDetectionModeEnable( PWMC_Handle_t * pHandle );

/* Disables the RL Detection mode and sets the standard PWM. */
void PWMC_RLDetectionModeDisable( PWMC_Handle_t * pHandle );

/* Sets the PWM duty cycle in the RL Detection mode. */
uint16_t PWMC_RLDetectionModeSetDuty( PWMC_Handle_t * pHandle,
                                      uint16_t hDuty );

/* Sets the Callback that the PWMC component shall invoke to get phases current. */
void PWMC_RegisterGetPhaseCurrentsCallBack( PWMC_GetPhaseCurr_Cb_t pCallBack,
    PWMC_Handle_t * pHandle );

/* Sets the Callback that the PWMC component shall invoke to switch off PWM
 *        generation. */
void PWMC_RegisterSwitchOffPwmCallBack( PWMC_Generic_Cb_t pCallBack,
                                        PWMC_Handle_t * pHandle );

/* Sets the Callback that the PWMC component shall invoke to switch on PWM
 *        generation. */
void PWMC_RegisterSwitchonPwmCallBack( PWMC_Generic_Cb_t pCallBack,
                                       PWMC_Handle_t * pHandle );

/* Sets the Callback that the PWMC component shall invoke to execute a calibration
 * of the current sensing system. */
void PWMC_RegisterReadingCalibrationCallBack( PWMC_Generic_Cb_t pCallBack,
    PWMC_Handle_t * pHandle );

/* Sets the Callback that the PWMC component shall invoke to turn on low sides. */
void PWMC_RegisterTurnOnLowSidesCallBack( PWMC_Generic_Cb_t pCallBack,
    PWMC_Handle_t * pHandle );

/* Sets the Callback that the PWMC component shall invoke to ADC sampling point for sector 1. */
void PWMC_RegisterSampPointSectXCallBack( PWMC_SetSampPointSectX_Cb_t pCallBack,
    PWMC_Handle_t * pHandle );



/* Sets the Callback that the PWMC component shall invoke to the over current status. */
void PWMC_RegisterIsOverCurrentOccurredCallBack( PWMC_OverCurr_Cb_t pCallBack,
    PWMC_Handle_t * pHandle );
/* Sets the Callback that the PWMC component shall invoke to set the reference voltage for the over current protection */
void PWMC_RegisterOCPSetRefVoltageCallBack( PWMC_SetOcpRefVolt_Cb_t pCallBack,
    PWMC_Handle_t * pHandle );

/* Sets the Callback that the PWMC component shall invoke to set the R/L detection mode */
void PWMC_RegisterRLDetectionModeEnableCallBack( PWMC_Generic_Cb_t pCallBack,
    PWMC_Handle_t * pHandle );

/* Sets the Callback that the PWMC component shall invoke to disable the R/L detection mode */
void PWMC_RegisterRLDetectionModeDisableCallBack( PWMC_Generic_Cb_t pCallBack,
    PWMC_Handle_t * pHandle );

/* Sets the Callback that the PWMC component shall invoke to set the duty cycle for the R/L detection mode */
void PWMC_RegisterRLDetectionModeSetDutyCallBack( PWMC_RLDetectSetDuty_Cb_t pCallBack,
    PWMC_Handle_t * pHandle );

/* Sets the Callback that the PWMC component shall invoke to call PWMC instance IRQ handler */
void PWMC_RegisterIrqHandlerCallBack( PWMC_IrqHandler_Cb_t pCallBack,
                                      PWMC_Handle_t * pHandle );

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif /* __cpluplus */

#endif /* PWMNCURRFDBK_H */

/******************* (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
