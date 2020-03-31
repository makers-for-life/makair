/**
  ******************************************************************************
  * @file    ICS_g4xx_pwm_curr_fdbk.h
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file contains all definitions and functions prototypes for the
  *          ICS_G4XX_pwm_curr_fdbk component of the Motor Control SDK.
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
  * @ingroup ICS_G4XX_pwm_curr_fdbk
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef ICS_G4XX_PWMNCURRFDBK_H
#define ICS_G4XX_PWMNCURRFDBK_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "pwm_curr_fdbk.h"

/* Exported defines --------------------------------------------------------*/
#define NONE    ((uint8_t)(0x00))
#define EXT_MODE  ((uint8_t)(0x01))
#define INT_MODE  ((uint8_t)(0x02))

/** @addtogroup MCSDK
  * @{
  */

/** @addtogroup pwm_curr_fdbk
  * @{
  */

/** @addtogroup ICS_G4XX_pwm_curr_fdbk
  * @{
  */

/* Exported types ------------------------------------------------------- */

/**
  * @brief  r3_4_f30X_pwm_curr_fdbk component parameters definition
  */
typedef const struct
{
  /* HW IP involved -----------------------------*/
  ADC_TypeDef * ADCx_1;            /*!< First ADC peripheral to be used.*/
  ADC_TypeDef * ADCx_2;            /*!< Second ADC peripheral to be used.*/
  TIM_TypeDef * TIMx;              /*!< timer used for PWM generation.*/
  GPIO_TypeDef * pwm_en_u_port;    /*!< Channel 1N (low side) GPIO output */
  GPIO_TypeDef * pwm_en_v_port;    /*!< Channel 2N (low side) GPIO output*/
  GPIO_TypeDef * pwm_en_w_port;    /*!< Channel 3N (low side)  GPIO output */

  uint32_t ADCConfig1; /*!< values of JSQR for first ADC for 6 sectors */
  uint32_t ADCConfig2; /*!< values of JSQR for Second ADC for 6 sectors */
  
  uint16_t pwm_en_u_pin;                    /*!< Channel 1N (low side) GPIO output pin */
  uint16_t pwm_en_v_pin;                    /*!< Channel 2N (low side) GPIO output pin */
  uint16_t pwm_en_w_pin;                    /*!< Channel 3N (low side)  GPIO output pin */
  
    /* PWM Driving signals initialization ----------------------------------------*/
  LowSideOutputsFunction_t LowSideOutputs; /*!< Low side or enabling signals
                                                generation method are defined
                                                here.*/

  uint8_t  RepetitionCounter;         /*!< It expresses the number of PWM
                                            periods to be elapsed before compare
                                            registers are updated again. In
                                            particular:
                                            RepetitionCounter= (2* #PWM periods)-1*/
  /* Emergency input (BKIN2) signal initialization -----------------------------*/
  uint8_t BKIN2Mode;                 /*!< It defines the modality of emergency
                                           input 2. It must be any of the
                                           the following:
                                           NONE - feature disabled.
                                           INT_MODE - Internal comparator used
                                           as source of emergency event.
                                           EXT_MODE - External comparator used
                                           as source of emergency event.*/

  /* Dual MC parameters --------------------------------------------------------*/
  uint8_t  FreqRatio;             /*!< It is used in case of dual MC to
                                        synchronize TIM1 and TIM8. It has
                                        effect only on the second instanced
                                        object and must be equal to the
                                        ratio between the two PWM frequencies
                                        (higher/lower). Supported values are
                                        1, 2 or 3 */
  uint8_t  IsHigherFreqTim;       /*!< When bFreqRatio is greather than 1
                                        this param is used to indicate if this
                                        instance is the one with the highest
                                        frequency. Allowed value are: HIGHER_FREQ
                                        or LOWER_FREQ */                                           

} ICS_Params_t, *pICS_Params_t;

/**
  * @brief  This structure is used to handle an instance of the
  *         r3_4_f30X_pwm_curr_fdbk component.
  */
typedef struct
{
  PWMC_Handle_t _Super;     /*!<   */
  uint32_t PhaseAOffset;   /*!< Offset of Phase A current sensing network  */
  uint32_t PhaseBOffset;   /*!< Offset of Phase B current sensing network  */
  uint16_t Half_PWMPeriod;  /*!< Half PWM Period in timer clock counts */
  uint8_t  PolarizationCounter;

  bool OverCurrentFlag;     /*!< This flag is set when an overcurrent occurs.*/
  bool OverVoltageFlag;     /*!< This flag is set when an overvoltage occurs.*/
  bool BrakeActionLock;     /*!< This flag is set to avoid that brake action is
                                 interrupted.*/
  pICS_Params_t pParams_str;
} PWMC_ICS_Handle_t;


/* Exported functions ------------------------------------------------------- */

/**
  * It initializes TIMx, ADC, GPIO, DMA1 and NVIC for current reading
  * in three shunt topology using STM32F30X and shared ADC
  */
void ICS_Init( PWMC_ICS_Handle_t * pHandle );

/**
  * It stores into the handle the voltage present on Ia and
  * Ib current feedback analog channels when no current is flowin into the
  * motor
  */
void ICS_CurrentReadingPolarization( PWMC_Handle_t * pHdl );

/**
  * It computes and return latest converted motor phase currents motor
  *
  */
void ICS_GetPhaseCurrents( PWMC_Handle_t * pHdl, ab_t * Iab );

/**
  * It turns on low sides switches. This function is intended to be
  * used for charging boot capacitors of driving section. It has to be
  * called each motor start-up when using high voltage drivers
  */
void ICS_TurnOnLowSides( PWMC_Handle_t * pHdl );

/**
  * It enables PWM generation on the proper Timer peripheral acting on MOE
  * bit
  */
void ICS_SwitchOnPWM( PWMC_Handle_t * pHdl );

/**
  * It disables PWM generation on the proper Timer peripheral acting on
  * MOE bit
  */
void ICS_SwitchOffPWM( PWMC_Handle_t * pHdl );

/**
  * Configure the ADC for the current sampling 
  * It means set the sampling point via TIMx_Ch4 value and polarity
  * ADC sequence length and channels.
  * And call the WriteTIMRegisters method.
  */
uint16_t ICS_WriteTIMRegisters( PWMC_Handle_t * pHdl );


/**
  *  It contains the TIMx Update event interrupt
  */
void * ICS_TIMx_UP_IRQHandler( PWMC_ICS_Handle_t * pHdl );

/**
  *  It contains the TIMx Break2 event interrupt
  */
void * ICS_BRK2_IRQHandler( PWMC_ICS_Handle_t * pHdl );

/**
  *  It contains the TIMx Break1 event interrupt
  */
void * ICS_BRK_IRQHandler( PWMC_ICS_Handle_t * pHdl );

/**
  * It is used to check if an overcurrent occurred since last call.
  */
uint16_t ICS_IsOverCurrentOccurred( PWMC_Handle_t * pHdl );
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif /* __cpluplus */

#endif /*ICS_G4XX_PWMNCURRFDBK_H*/

/************************ (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
