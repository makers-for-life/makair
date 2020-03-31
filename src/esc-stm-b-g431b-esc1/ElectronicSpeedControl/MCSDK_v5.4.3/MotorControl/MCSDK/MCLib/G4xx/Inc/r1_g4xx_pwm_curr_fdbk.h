/**
  ******************************************************************************
  * @file    R1_g4xx_pwm_curr_fdbk.h
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file contains all definitions and functions prototypes for the
  *          R1_F30X_pwm_curr_fdbk component of the Motor Control SDK.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  * @ingroup R1_F30X_pwm_curr_fdbk
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef R1_G4XX_PWMNCURRFDBK_H
#define R1_G4XX_PWMNCURRFDBK_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "pwm_curr_fdbk.h"

/* Exported defines --------------------------------------------------------*/

#define NONE    ((uint8_t)(0x00))
#define EXT_MODE  ((uint8_t)(0x01))
#define INT_MODE  ((uint8_t)(0x02))
#define STBD3 0x02u /*!< Flag to indicate which phase has been distorted
                           in boudary 3 zone (A or B)*/
#define DSTEN 0x04u /*!< Flag to indicate if the distortion must be performed
                           or not (in case of charge of bootstrap capacitor phase
                           is not required)*/


/** @addtogroup MCSDK
  * @{
  */

/** @addtogroup pwm_curr_fdbk
  * @{
  */

/** @addtogroup R1_F30X_pwm_curr_fdbk
  * @{
  */

/* Exported types ------------------------------------------------------- */

/**
  * @brief  r3_4_f30X_pwm_curr_fdbk component parameters definition
  */
typedef const struct
{
  /* HW IP involved -----------------------------*/
  ADC_TypeDef * ADCx;            /*!< First ADC peripheral to be used.*/
  TIM_TypeDef * TIMx;              /*!< timer used for PWM generation.*/
  OPAMP_TypeDef * OPAMP_Selection;
  COMP_TypeDef * CompOCPSelection; /*!< Internal comparator used for Phases protection.*/
  COMP_TypeDef * CompOVPSelection;  /*!< Internal comparator used for Over Voltage protection.*/
  GPIO_TypeDef * pwm_en_u_port;    /*!< Channel 1N (low side) GPIO output */
  GPIO_TypeDef * pwm_en_v_port;    /*!< Channel 2N (low side) GPIO output*/
  GPIO_TypeDef * pwm_en_w_port;    /*!< Channel 3N (low side)  GPIO output */
  DAC_TypeDef  * DAC_OCP_Selection; /*!< DAC used for Over Current protection.*/
  DAC_TypeDef  * DAC_OVP_Selection; /*!< DAC used for Over Voltage protection.*/
  uint32_t DAC_Channel_OCP;        /*!< DAC channel used for Phase A current protection.*/
  uint32_t DAC_Channel_OVP;        /*!< DAC channel used for over voltage protection.*/
  uint16_t pwm_en_u_pin;                    /*!< Channel 1N (low side) GPIO output pin */
  uint16_t pwm_en_v_pin;                    /*!< Channel 2N (low side) GPIO output pin */
  uint16_t pwm_en_w_pin;                    /*!< Channel 3N (low side)  GPIO output pin */
  
  
 /* PWM generation parameters --------------------------------------------------*/

  uint16_t TMin;                    /* */
  uint16_t HTMin;                    /* */
  uint16_t CHTMin;                    /* */
  uint16_t Tbefore;                 /* */
  uint16_t Tafter;                  /* */
  uint16_t TSample;  

  /* DAC settings --------------------------------------------------------------*/
  uint16_t DAC_OCP_Threshold;        /*!< Value of analog reference expressed
                                           as 16bit unsigned integer.
                                           Ex. 0 = 0V 65536 = VDD_DAC.*/
  uint16_t DAC_OVP_Threshold;        /*!< Value of analog reference expressed
                                           as 16bit unsigned integer.
                                           Ex. 0 = 0V 65536 = VDD_DAC.*/   
  /* PWM Driving signals initialization ----------------------------------------*/
  LowSideOutputsFunction_t LowSideOutputs; /*!< Low side or enabling signals
                                                generation method are defined
                                                here.*/
  uint8_t  IChannel;                                                

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
 
  /* Internal COMP settings ----------------------------------------------------*/                                 
  uint8_t       CompOCPInvInput_MODE;    /*!< COMPx inverting input mode. It must be either
                                                equal to EXT_MODE or INT_MODE. */                            
  uint8_t       CompOVPInvInput_MODE;     /*!< COMPx inverting input mode. It must be either
                                                equal to EXT_MODE or INT_MODE. */
  
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

} R1_Params_t;

/**
  * @brief  This structure is used to handle an instance of the
  *         r3_4_f30X_pwm_curr_fdbk component.
  */
typedef struct
{
  PWMC_Handle_t _Super;     /*!<   */
  DMA_Channel_TypeDef * DistortionDMAy_Chx; /*!< DMA resource used for doing the distortion*/
  uint32_t ADCConfig;
  uint32_t PhaseOffset;   /*!< Offset of Phase A current sensing network  */
  uint16_t Half_PWMPeriod;  /*!< Half PWM Period in timer clock counts */
  uint16_t DmaBuff[2]; 
  uint16_t CntSmp1; 
  uint16_t CntSmp2;
  uint16_t CurrAOld;
  uint16_t CurrBOld;
  uint8_t bIChannel;            /*!< ADC channel used for current conversion */
  uint8_t  PolarizationCounter;
  uint8_t sampCur1;
  uint8_t sampCur2;
  uint8_t Inverted_pwm_new; /* */
  uint8_t Flags; /* */
  bool UpdateFlagBuffer;  /*!< buffered version of Timer update IT flag */
  /*!< Trigger selection for ADC peripheral.*/
  bool OverCurrentFlag;     /*!< This flag is set when an overcurrent occurs.*/
  bool OverVoltageFlag;     /*!< This flag is set when an overvoltage occurs.*/
  bool BrakeActionLock;     /*!< This flag is set to avoid that brake action is
                                 interrupted.*/
  R1_Params_t * pParams_str;
} PWMC_R1_Handle_t;

/**
  * It initializes TIMx, ADC, GPIO, DMA1 and NVIC for current reading
  * in three shunt topology using STM32F30X and shared ADC
  */
void R1_Init( PWMC_R1_Handle_t * pHandle );

/**
  * It stores into the handle the voltage present on Ia and
  * Ib current feedback analog channels when no current is flowin into the
  * motor
  */
void R1_CurrentReadingPolarization( PWMC_Handle_t * pHdl );

/**
  * It computes and return latest converted motor phase currents motor
  *
  */
void R1_GetPhaseCurrents( PWMC_Handle_t * pHdl, ab_t * Iab );

/**
  * It turns on low sides switches. This function is intended to be
  * used for charging boot capacitors of driving section. It has to be
  * called each motor start-up when using high voltage drivers
  */
void R1_TurnOnLowSides( PWMC_Handle_t * pHdl );

/**
  * It enables PWM generation on the proper Timer peripheral acting on MOE
  * bit
  */
void R1_SwitchOnPWM( PWMC_Handle_t * pHdl );

/**
  * It disables PWM generation on the proper Timer peripheral acting on
  * MOE bit
  */
void R1_SwitchOffPWM( PWMC_Handle_t * pHdl );

/**
  * Configure the ADC for the current sampling 
  * It means set the sampling point via TIMx_Ch4 value and polarity
  * ADC sequence length and channels.
  * And call the WriteTIMRegisters method.
  */
uint16_t R1_CalcDutyCycles( PWMC_Handle_t * pHdl );


/**
  *  It contains the TIMx Update event interrupt
  */
void * R1_TIMx_UP_IRQHandler( PWMC_R1_Handle_t * pHdl );

/**
  *  It contains the TIMx Break2 event interrupt
  */
void * R1_BRK2_IRQHandler( PWMC_R1_Handle_t * pHdl );

/**
  *  It contains the TIMx Break1 event interrupt
  */
void * R1_BRK_IRQHandler( PWMC_R1_Handle_t * pHdl );

/**
  * It is used to check if an overcurrent occurred since last call.
  */
uint16_t R1_IsOverCurrentOccurred( PWMC_Handle_t * pHdl );


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

#endif /*R1_G4XX_PWMNCURRFDBK_H*/

/************************ (C) COPYRIGHT 2018 STMicroelectronics *****END OF FILE****/
