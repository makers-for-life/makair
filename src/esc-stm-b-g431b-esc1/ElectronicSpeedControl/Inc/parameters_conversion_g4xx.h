/**
  ******************************************************************************
  * @file    parameters_conversion_g4xx.h
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file contains the definitions needed to convert MC SDK parameters
  *          so as to target the STM32G4 Family.
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
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PARAMETERS_CONVERSION_G4XX_H
#define __PARAMETERS_CONVERSION_G4XX_H

#include "pmsm_motor_parameters.h"
#include "drive_parameters.h"
#include "power_stage_parameters.h"
#include "mc_math.h"

/************************* CPU & ADC PERIPHERAL CLOCK CONFIG ******************/
#define SYSCLK_FREQ      170000000uL
#define TIM_CLOCK_DIVIDER  1 
#define ADV_TIM_CLK_MHz  170
#define ADC_CLK_MHz     68
#define HALL_TIM_CLK    170000000uL
#define APB1TIM_FREQ 170000000uL

/*************************  IRQ Handler Mapping  *********************/
#define TIMx_UP_M1_IRQHandler TIM1_UP_TIM16_IRQHandler

#define TIMx_BRK_M1_IRQHandler TIM1_BRK_TIM15_IRQHandler

#define ADC_TRIG_CONV_LATENCY_CYCLES 3.5
#define ADC_SAR_CYCLES 12.5 

#define M1_VBUS_SW_FILTER_BW_FACTOR     6u

#define OPAMP1_InvertingInput_PC5         LL_OPAMP_INPUT_INVERT_IO0
#define OPAMP1_InvertingInput_PA3         LL_OPAMP_INPUT_INVERT_IO1 
#define OPAMP1_InvertingInput_PGA         LL_OPAMP_INPUT_INVERT_CONNECT_NO
#define OPAMP1_InvertingInput_FOLLOWER    LL_OPAMP_MODE_FOLLOWER
#define OPAMP2_InvertingInput_PC5         LL_OPAMP_INPUT_INVERT_IO0
#define OPAMP2_InvertingInput_PA5         LL_OPAMP_INPUT_INVERT_IO1 
#define OPAMP2_InvertingInput_PGA         LL_OPAMP_INPUT_INVERT_CONNECT_NO
#define OPAMP2_InvertingInput_FOLLOWER    LL_OPAMP_MODE_FOLLOWER
#define OPAMP3_InvertingInput_PB10        LL_OPAMP_INPUT_INVERT_IO0
#define OPAMP3_InvertingInput_PB2         LL_OPAMP_INPUT_INVERT_IO1 
#define OPAMP3_InvertingInput_PGA         LL_OPAMP_INPUT_INVERT_CONNECT_NO
#define OPAMP3_InvertingInput_FOLLOWER    LL_OPAMP_MODE_FOLLOWER
#define OPAMP4_InvertingInput_PB10        LL_OPAMP_INPUT_INVERT_IO0
#define OPAMP4_InvertingInput_PD8         LL_OPAMP_INPUT_INVERT_IO1 
#define OPAMP4_InvertingInput_PGA         LL_OPAMP_INPUT_INVERT_CONNECT_NO
#define OPAMP4_InvertingInput_FOLLOWER    LL_OPAMP_MODE_FOLLOWER

#define OPAMP1_NonInvertingInput_PA1      LL_OPAMP_INPUT_NONINVERT_IO0   
#define OPAMP1_NonInvertingInput_PA3      LL_OPAMP_INPUT_NONINVERT_IO1  
#define OPAMP1_NonInvertingInput_PA7      LL_OPAMP_INPUT_NONINVERT_IO2                                                                  
#define OPAMP2_NonInvertingInput_PA7      LL_OPAMP_INPUT_NONINVERT_IO0                                
#define OPAMP2_NonInvertingInput_PB14     LL_OPAMP_INPUT_NONINVERT_IO1                                  
#define OPAMP2_NonInvertingInput_PB0      LL_OPAMP_INPUT_NONINVERT_IO2     
#define OPAMP2_NonInvertingInput_PD14     LL_OPAMP_INPUT_NONINVERT_IO3                                  
#define OPAMP3_NonInvertingInput_PB0      LL_OPAMP_INPUT_NONINVERT_IO0                                  
#define OPAMP3_NonInvertingInput_PB13     LL_OPAMP_INPUT_NONINVERT_IO1                                  
#define OPAMP3_NonInvertingInput_PA1      LL_OPAMP_INPUT_NONINVERT_IO2
#define OPAMP4_NonInvertingInput_PB13     LL_OPAMP_INPUT_NONINVERT_IO0
#define OPAMP4_NonInvertingInput_PD11     LL_OPAMP_INPUT_NONINVERT_IO1
#define OPAMP4_NonInvertingInput_PB11     LL_OPAMP_INPUT_NONINVERT_IO2
#define OPAMP5_NonInvertingInput_PB14     LL_OPAMP_INPUT_NONINVERT_IO0                                 
#define OPAMP5_NonInvertingInput_PD12     LL_OPAMP_INPUT_NONINVERT_IO1                                 
#define OPAMP5_NonInvertingInput_PC3      LL_OPAMP_INPUT_NONINVERT_IO2
#define OPAMP6_NonInvertingInput_PB12     LL_OPAMP_INPUT_NONINVERT_IO0                                 
#define OPAMP6_NonInvertingInput_PD9      LL_OPAMP_INPUT_NONINVERT_IO1                                 
#define OPAMP6_NonInvertingInput_PB13     LL_OPAMP_INPUT_NONINVERT_IO2

#define OPAMP1_PGAConnect_PC5             OPAMP_CSR_PGGAIN_3
#define OPAMP1_PGAConnect_PA3             (OPAMP_CSR_PGGAIN_3|OPAMP_CSR_PGGAIN_2)
#define OPAMP2_PGAConnect_PC5             OPAMP_CSR_PGGAIN_3
#define OPAMP2_PGAConnect_PA5             (OPAMP_CSR_PGGAIN_3|OPAMP_CSR_PGGAIN_2)
#define OPAMP3_PGAConnect_PB10            OPAMP_CSR_PGGAIN_3
#define OPAMP3_PGAConnect_PB2             (OPAMP_CSR_PGGAIN_3|OPAMP_CSR_PGGAIN_2)
#define OPAMP4_PGAConnect_PB10            OPAMP_CSR_PGGAIN_3
#define OPAMP4_PGAConnect_PD8             (OPAMP_CSR_PGGAIN_3|OPAMP_CSR_PGGAIN_2)

#define COMP1_InvertingInput_PA0          LL_COMP_INPUT_MINUS_IO1
#define COMP2_InvertingInput_PA2          LL_COMP_INPUT_MINUS_IO1
#define COMP3_InvertingInput_PD15         LL_COMP_INPUT_MINUS_IO1
#define COMP3_InvertingInput_PB12         LL_COMP_INPUT_MINUS_IO2
#define COMP4_InvertingInput_PE8          LL_COMP_INPUT_MINUS_IO1
#define COMP4_InvertingInput_PB2          LL_COMP_INPUT_MINUS_IO2
#define COMP5_InvertingInput_PD13         LL_COMP_INPUT_MINUS_IO1
#define COMP5_InvertingInput_PB10         LL_COMP_INPUT_MINUS_IO2
#define COMP6_InvertingInput_PD10         LL_COMP_INPUT_MINUS_IO1
#define COMP6_InvertingInput_PB15         LL_COMP_INPUT_MINUS_IO2
#define COMP7_InvertingInput_PC0          LL_COMP_INPUT_MINUS_IO1

#define COMPX_InvertingInput_DAC1        LL_COMP_INPUT_MINUS_DAC1_CH1
#define COMPX_InvertingInput_DAC2        LL_COMP_INPUT_MINUS_DAC1_CH2
#define COMPX_InvertingInput_VREF        LL_COMP_INPUT_MINUS_VREFINT
#define COMPX_InvertingInput_VREF_1_4    LL_COMP_INPUT_MINUS_1_4VREFINT
#define COMPX_InvertingInput_VREF_1_2    LL_COMP_INPUT_MINUS_1_2VREFINT
#define COMPX_InvertingInput_VREF_3_4    LL_COMP_INPUT_MINUS_3_4VREFINT

#define COMP1_NonInvertingInput_PA1    LL_COMP_INPUT_PLUS_IO1
#define COMP2_NonInvertingInput_PA3    LL_COMP_INPUT_PLUS_IO2
#define COMP2_NonInvertingInput_PA7    LL_COMP_INPUT_PLUS_IO1
#define COMP3_NonInvertingInput_PB14   LL_COMP_INPUT_PLUS_IO1
#define COMP3_NonInvertingInput_PD14   LL_COMP_INPUT_PLUS_IO2
#define COMP4_NonInvertingInput_PB0    LL_COMP_INPUT_PLUS_IO1
#define COMP4_NonInvertingInput_PE7    LL_COMP_INPUT_PLUS_IO2
#define COMP5_NonInvertingInput_PB13   LL_COMP_INPUT_PLUS_IO2
#define COMP5_NonInvertingInput_PD12   LL_COMP_INPUT_PLUS_IO1
#define COMP6_NonInvertingInput_PB11   LL_COMP_INPUT_PLUS_IO2
#define COMP6_NonInvertingInput_PD11   LL_COMP_INPUT_PLUS_IO1
#define COMP7_NonInvertingInput_PC1    LL_COMP_INPUT_PLUS_IO2
#define COMP7_NonInvertingInput_PA0    LL_COMP_INPUT_PLUS_IO1

/* USER CODE BEGIN Additional parameters */
/* ESC parameters conversion */

/* 1060 us ON time is the minimum value to spin the motor*/
/* 1860 us ON time is the target for the maximum speed */
#define ESC_TON_MIN (APB1TIM_FREQ / 1000000 * 1060 )
#define ESC_TON_MAX (APB1TIM_FREQ / 1000000 * 1860 )
/* Arming : if 900 us <= TOn < 1060 us for 200 us, ESC is armed */
#define ESC_TON_ARMING ((APB1TIM_FREQ /1000000) * 900 )
/* USER CODE END Additional parameters */  

#endif /*__PARAMETERS_CONVERSION_F30X_H*/

/******************* (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
