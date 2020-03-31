/**
  ******************************************************************************
  * @file    R3_2_g4xx_pwm_curr_fdbk.h
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file contains all definitions and functions prototypes for the
  *          R3_2_G4XX_pwm_curr_fdbk component of the Motor Control SDK.
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
  * @ingroup R3_2_G4XX_pwm_curr_fdbk
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef R3_2_G4XX_PWMNCURRFDBK_H
#define R3_2_G4XX_PWMNCURRFDBK_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "pwm_curr_fdbk.h"

/* Exported defines --------------------------------------------------------*/
#define GPIO_NoRemap_TIM1 ((uint32_t)(0))
#define SHIFTED_TIMs      ((uint8_t) 1)
#define NO_SHIFTED_TIMs   ((uint8_t) 0)

#define NONE    ((uint8_t)(0x00))
#define EXT_MODE  ((uint8_t)(0x01))
#define INT_MODE  ((uint8_t)(0x02))

/** @addtogroup MCSDK
  * @{
  */

/** @addtogroup pwm_curr_fdbk
  * @{
  */

/** @addtogroup R3_2_G4XX_pwm_curr_fdbk
  * @{
  */

/* Exported types ------------------------------------------------------- */

/**
  * @brief  R3_2_G4XX_pwm_curr_fdbk component OPAMP parameters definition
  */
typedef const struct
{
  /* First OPAMP settings ------------------------------------------------------*/
   OPAMP_TypeDef * OPAMPx_1; /* OPAMP dedicated to phase A */
   OPAMP_TypeDef * OPAMPx_2; /* OPAMP dedicated to phase B */
   OPAMP_TypeDef * OPAMPx_3; /* OPAMP dedicated to phase C */
   OPAMP_TypeDef * OPAMPSelect_1 [6] ; /*!< define for each sector first conversion which OPAMP is involved - Null otherwise */
   OPAMP_TypeDef * OPAMPSelect_2 [6] ; /*!< define for each sector second conversion which OPAMP is involved - Null otherwise */
   uint32_t OPAMPConfig1 [6]; /*!< Define the OPAMP_CSR_OPAMPINTEN and the OPAMP_CSR_VPSEL config for each ADC conversions*/ 
   uint32_t OPAMPConfig2 [6]; /*!< Define the OPAMP_CSR_OPAMPINTEN and the OPAMP_CSR_VPSEL config for each ADC conversions*/ 
} R3_3_OPAMPParams_t;

/**
  * @brief  r3_4_f30X_pwm_curr_fdbk component parameters definition
  */
typedef const struct
{
  /* HW IP involved -----------------------------*/
  ADC_TypeDef * ADCx_1;            /*!< First ADC peripheral to be used.*/
  ADC_TypeDef * ADCx_2;            /*!< Second ADC peripheral to be used.*/
  TIM_TypeDef * TIMx;              /*!< timer used for PWM generation.*/
  R3_3_OPAMPParams_t * OPAMPParams;  /*!< Pointer to the OPAMP params struct.
                                       It must be MC_NULL if internal OPAMP are not used.*/
  COMP_TypeDef * CompOCPASelection; /*!< Internal comparator used for Phase A protection.*/
  COMP_TypeDef * CompOCPBSelection; /*!< Internal comparator used for Phase B protection.*/
  COMP_TypeDef * CompOCPCSelection; /*!< Internal comparator used for Phase C protection.*/
  COMP_TypeDef * CompOVPSelection;  /*!< Internal comparator used for Over Voltage protection.*/
  GPIO_TypeDef * pwm_en_u_port;    /*!< Channel 1N (low side) GPIO output */
  GPIO_TypeDef * pwm_en_v_port;    /*!< Channel 2N (low side) GPIO output*/
  GPIO_TypeDef * pwm_en_w_port;    /*!< Channel 3N (low side)  GPIO output */
  DAC_TypeDef  * DAC_OCP_ASelection; /*!< DAC used for Phase A protection.*/
  DAC_TypeDef  * DAC_OCP_BSelection; /*!< DAC used for Phase B protection.*/
  DAC_TypeDef  * DAC_OCP_CSelection; /*!< DAC used for Phase C protection.*/
  DAC_TypeDef  * DAC_OVP_Selection; /*!< DAC used for Over Voltage protection.*/
  uint32_t DAC_Channel_OCPA;      /*!< DAC channel used for Phase A current protection.*/
  uint32_t DAC_Channel_OCPB;      /*!< DAC channel used for Phase B current protection.*/
  uint32_t DAC_Channel_OCPC;      /*!< DAC channel used for Phase C current protection.*/
  uint32_t DAC_Channel_OVP;       /*!< DAC channel used for Over Voltage protection.*/
  uint32_t volatile * ADCDataReg1[6]; /*!< Contains the Address of ADC read value for one phase
                                         and all the 6 sectors */
  uint32_t volatile * ADCDataReg2[6]; /*!< Contains the Address of ADC read value for one phase
                                         and all the 6 sectors */     
  uint32_t ADCConfig1 [6] ; /*!< values of JSQR for first ADC for 6 sectors */ 
  uint32_t ADCConfig2 [6] ; /*!< values of JSQR for Second ADC for 6 sectors */ 
  
  uint16_t pwm_en_u_pin;                    /*!< Channel 1N (low side) GPIO output pin */
  uint16_t pwm_en_v_pin;                    /*!< Channel 2N (low side) GPIO output pin */
  uint16_t pwm_en_w_pin;                    /*!< Channel 3N (low side)  GPIO output pin */
  
  
 /* PWM generation parameters --------------------------------------------------*/

  uint16_t Tafter;                    /*!< It is the sum of dead time plus max
                                            value between rise time and noise time
                                            express in number of TIM clocks.*/
  uint16_t Tbefore;                   /*!< It is the sampling time express in
                                            number of TIM clocks.*/

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
  uint8_t       CompOCPAInvInput_MODE;    /*!< COMPx inverting input mode. It must be either
                                                equal to EXT_MODE or INT_MODE. */                            
  uint8_t       CompOCPBInvInput_MODE;    /*!< COMPx inverting input mode. It must be either
                                                equal to EXT_MODE or INT_MODE. */                                               
  uint8_t       CompOCPCInvInput_MODE;    /*!< COMPx inverting input mode. It must be either
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

} R3_2_Params_t, *pR3_2_Params_t;

/**
  * @brief  This structure is used to handle an instance of the
  *         r3_4_f30X_pwm_curr_fdbk component.
  */
typedef struct
{
  PWMC_Handle_t _Super;     /*!<   */
  uint32_t PhaseAOffset;   /*!< Offset of Phase A current sensing network  */
  uint32_t PhaseBOffset;   /*!< Offset of Phase B current sensing network  */
  uint32_t PhaseCOffset;   /*!< Offset of Phase C current sensing network  */                                
  uint16_t Half_PWMPeriod;  /*!< Half PWM Period in timer clock counts */
  uint16_t ADC_ExternalPolarityInjected;
  uint8_t  PolarizationCounter;
  uint8_t PolarizationSector; /*!< Sector selected during calibration phase */
  /*!< Trigger selection for ADC peripheral.*/
  bool OverCurrentFlag;     /*!< This flag is set when an overcurrent occurs.*/
  bool OverVoltageFlag;     /*!< This flag is set when an overvoltage occurs.*/
  bool BrakeActionLock;     /*!< This flag is set to avoid that brake action is
                                 interrupted.*/
  pR3_2_Params_t pParams_str;
} PWMC_R3_2_Handle_t;


/* Exported functions ------------------------------------------------------- */

/**
  * It initializes TIMx, ADC, GPIO, DMA1 and NVIC for current reading
  * in three shunt topology using STM32F30X and shared ADC
  */
void R3_2_Init( PWMC_R3_2_Handle_t * pHandle );

/**
  * It stores into the handle the voltage present on Ia and
  * Ib current feedback analog channels when no current is flowin into the
  * motor
  */
void R3_2_CurrentReadingPolarization( PWMC_Handle_t * pHdl );

/**
  * It computes and return latest converted motor phase currents motor
  *
  */
void R3_2_GetPhaseCurrents( PWMC_Handle_t * pHdl, ab_t * Iab );

/**
  * It turns on low sides switches. This function is intended to be
  * used for charging boot capacitors of driving section. It has to be
  * called each motor start-up when using high voltage drivers
  */
void R3_2_TurnOnLowSides( PWMC_Handle_t * pHdl );

/**
  * It enables PWM generation on the proper Timer peripheral acting on MOE
  * bit
  */
void R3_2_SwitchOnPWM( PWMC_Handle_t * pHdl );

/**
  * It disables PWM generation on the proper Timer peripheral acting on
  * MOE bit
  */
void R3_2_SwitchOffPWM( PWMC_Handle_t * pHdl );

/**
  * Configure the ADC for the current sampling 
  * It means set the sampling point via TIMx_Ch4 value and polarity
  * ADC sequence length and channels.
  * And call the WriteTIMRegisters method.
  */
uint16_t R3_2_SetADCSampPointSectX( PWMC_Handle_t * pHdl );


/**
  *  It contains the TIMx Update event interrupt
  */
void * R3_2_TIMx_UP_IRQHandler( PWMC_R3_2_Handle_t * pHdl );

/**
  *  It contains the TIMx Break2 event interrupt
  */
void * R3_2_BRK2_IRQHandler( PWMC_R3_2_Handle_t * pHdl );

/**
  *  It contains the TIMx Break1 event interrupt
  */
void * R3_2_BRK_IRQHandler( PWMC_R3_2_Handle_t * pHdl );

/**
  * It is used to check if an overcurrent occurred since last call.
  */
uint16_t R3_2_IsOverCurrentOccurred( PWMC_Handle_t * pHdl );

/**
  * It is used to set the PWM mode for R/L detection.
  */
void R3_2_RLDetectionModeEnable( PWMC_Handle_t * pHdl );

/**
  * It is used to disable the PWM mode for R/L detection.
  */
void R3_2_RLDetectionModeDisable( PWMC_Handle_t * pHdl );

/**
  * It is used to set the PWM dutycycle for R/L detection.
  */
uint16_t R3_2_RLDetectionModeSetDuty( PWMC_Handle_t * pHdl, uint16_t hDuty );

/**
 * @brief  It turns on low sides switches and start ADC triggering.
 *         This function is specific for MP phase.
 */
void RLTurnOnLowSidesAndStart( PWMC_Handle_t * pHdl );

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

#endif /*R3_2_G4XX_PWMNCURRFDBK_H*/

/************************ (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
