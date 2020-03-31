
/**
  ******************************************************************************
  * @file    mc_parameters.c
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file provides definitions of HW parameters specific to the 
  *          configuration of the subsystem.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "parameters_conversion.h"
 
 
#include "r3_2_g4xx_pwm_curr_fdbk.h"
 
 
 
 
 
 

/* USER CODE BEGIN Additional include */
#include "esc.h"
/* USER CODE END Additional include */  

#define FREQ_RATIO 1                /* Dummy value for single drive */
#define FREQ_RELATION HIGHEST_FREQ  /* Dummy value for single drive */
 

/**
  * @brief  Internal OPAMP parameters Motor 1 - three shunt - G4xx - Shared Resources
  * temporary hard coded to ESC G4 board
  */
R3_3_OPAMPParams_t R3_3_OPAMPParamsM1 =
{
   .OPAMPx_1 = OPAMP1,
   .OPAMPx_2 = OPAMP2,
   .OPAMPx_3 = OPAMP3,
    // OPAMPMatrix is used to specify if the ADC source comes from internal channel of which OPAMP. 
  .OPAMPSelect_1 = { OPAMP3 
                   , NULL   
                   , NULL   
                   , NULL   
                   , NULL   
                   , OPAMP3 
                   },
  .OPAMPSelect_2 = { NULL  
                   , OPAMP3
                   , OPAMP3
                   , NULL  
                   , NULL  
                   , NULL  
                   },                  
 // Define for each config the VPSEL and the Internal output enable bit
  .OPAMPConfig1 = {  0x0 
                    ,0x0 
                    ,0x0 
                    ,0x0 
                    ,0x0 
                    ,0x0  
                  }, 
  .OPAMPConfig2 = {   0x0   
                    , OPAMP_CSR_OPAMPINTEN  
                    , OPAMP_CSR_OPAMPINTEN  
                    , 0x0 
                    , 0x0 
                    , 0x0 
                  }, 
};
/**
  * @brief  Current sensor parameters Motor 1 - three shunt - G4 
  */
R3_2_Params_t R3_2_ParamsM1 =
{
/* Dual MC parameters --------------------------------------------------------*/
  .FreqRatio       = FREQ_RATIO,                         
  .IsHigherFreqTim = FREQ_RELATION,                      
                                                          
/* Current reading A/D Conversions initialization -----------------------------*/
  .ADCx_1           = ADC1,                   
  .ADCx_2           = ADC2,
 /* ESC-G4 Kit config */ 
   .ADCConfig1 = {MC_ADC_CHANNEL_12<<ADC_JSQR_JSQ1_Pos | (LL_ADC_INJ_TRIG_EXT_TIM1_TRGO & ~ADC_INJ_TRIG_EXT_EDGE_DEFAULT)
                 ,MC_ADC_CHANNEL_3<<ADC_JSQR_JSQ1_Pos | (LL_ADC_INJ_TRIG_EXT_TIM1_TRGO & ~ADC_INJ_TRIG_EXT_EDGE_DEFAULT)
                 ,MC_ADC_CHANNEL_3<<ADC_JSQR_JSQ1_Pos | (LL_ADC_INJ_TRIG_EXT_TIM1_TRGO & ~ADC_INJ_TRIG_EXT_EDGE_DEFAULT)
                 ,MC_ADC_CHANNEL_3<<ADC_JSQR_JSQ1_Pos | (LL_ADC_INJ_TRIG_EXT_TIM1_TRGO & ~ADC_INJ_TRIG_EXT_EDGE_DEFAULT)
                 ,MC_ADC_CHANNEL_3<<ADC_JSQR_JSQ1_Pos | (LL_ADC_INJ_TRIG_EXT_TIM1_TRGO & ~ADC_INJ_TRIG_EXT_EDGE_DEFAULT)
                 ,MC_ADC_CHANNEL_12<<ADC_JSQR_JSQ1_Pos | (LL_ADC_INJ_TRIG_EXT_TIM1_TRGO & ~ADC_INJ_TRIG_EXT_EDGE_DEFAULT)
                 },
   .ADCConfig2 = {  MC_ADC_CHANNEL_3<<ADC_JSQR_JSQ1_Pos | (LL_ADC_INJ_TRIG_EXT_TIM1_TRGO & ~ADC_INJ_TRIG_EXT_EDGE_DEFAULT)
                 , 18 << ADC_JSQR_JSQ1_Pos | (LL_ADC_INJ_TRIG_EXT_TIM1_TRGO & ~ADC_INJ_TRIG_EXT_EDGE_DEFAULT)// Phase C ADC2 on channel 18 (OPAMP3)
                 , 18 << ADC_JSQR_JSQ1_Pos | (LL_ADC_INJ_TRIG_EXT_TIM1_TRGO & ~ADC_INJ_TRIG_EXT_EDGE_DEFAULT)
                 , MC_ADC_CHANNEL_3<<ADC_JSQR_JSQ1_Pos | (LL_ADC_INJ_TRIG_EXT_TIM1_TRGO & ~ADC_INJ_TRIG_EXT_EDGE_DEFAULT)
                 , MC_ADC_CHANNEL_3<<ADC_JSQR_JSQ1_Pos | (LL_ADC_INJ_TRIG_EXT_TIM1_TRGO & ~ADC_INJ_TRIG_EXT_EDGE_DEFAULT)
                 , MC_ADC_CHANNEL_3<<ADC_JSQR_JSQ1_Pos | (LL_ADC_INJ_TRIG_EXT_TIM1_TRGO & ~ADC_INJ_TRIG_EXT_EDGE_DEFAULT)
                 },              
  .ADCDataReg1 = { &ADC2->JDR1 // Phase B,
                 , &ADC1->JDR1 // Phase A,
                 , &ADC1->JDR1 // Phase A,
                 , &ADC1->JDR1 // Phase A,
                 , &ADC1->JDR1 // Phase A,
                 , &ADC2->JDR1 // Phase B,                            
                 },
  .ADCDataReg2 = { &ADC1->JDR1 // Phase C
                 , &ADC2->JDR1 // Phase C
                 , &ADC2->JDR1 // Phase C
                 , &ADC2->JDR1 // Phase B
                 , &ADC2->JDR1 // Phase B
                 , &ADC1->JDR1 // Phase C                            
                 },                            
  /* PWM generation parameters --------------------------------------------------*/
  .RepetitionCounter = REP_COUNTER,                    
  .Tafter            = TW_AFTER,                       
  .Tbefore           = TW_BEFORE,                      
  .TIMx               = TIM1,            
                                                        
/* PWM Driving signals initialization ----------------------------------------*/
  .LowSideOutputs = (LowSideOutputsFunction_t)LOW_SIDE_SIGNALS_ENABLING, 
 .pwm_en_u_port      = MC_NULL,                   
 .pwm_en_u_pin       = (uint16_t) 0,
 .pwm_en_v_port      = MC_NULL,                     
 .pwm_en_v_pin       = (uint16_t) 0,                  
 .pwm_en_w_port      = MC_NULL,                   
 .pwm_en_w_pin       = (uint16_t) 0,
                

/* Emergency input (BKIN2) signal initialization -----------------------------*/
  .BKIN2Mode     = INT_MODE,                         

/* Internal OPAMP common settings --------------------------------------------*/
  .OPAMPParams     = &R3_3_OPAMPParamsM1,
/* Internal COMP settings ----------------------------------------------------*/
  .CompOCPASelection     = COMP1,
  .CompOCPAInvInput_MODE = INT_MODE,                          
  .CompOCPBSelection     = COMP2,                
  .CompOCPBInvInput_MODE = INT_MODE,              
  .CompOCPCSelection     = COMP4,  
  .CompOCPCInvInput_MODE = INT_MODE,  
  .DAC_OCP_ASelection    = MC_NULL,
  .DAC_OCP_BSelection    = MC_NULL,
  .DAC_OCP_CSelection    = MC_NULL,
  .DAC_Channel_OCPA      = (uint32_t) 0,
  .DAC_Channel_OCPB      = (uint32_t) 0,
  .DAC_Channel_OCPC      = (uint32_t) 0,  

  .CompOVPSelection      = MC_NULL,       
  .CompOVPInvInput_MODE  = NONE,
  .DAC_OVP_Selection     = MC_NULL,
  .DAC_Channel_OVP       = (uint32_t) 0,   
                                                         
/* DAC settings --------------------------------------------------------------*/
  .DAC_OCP_Threshold =  5957,                                        
  .DAC_OVP_Threshold =  23830,                                                                       
                                                    
};
     
  
 
  

/* USER CODE BEGIN Additional parameters */
const ESC_Params_t ESC_ParamsM1 =
{
  .Command_TIM = TIM2,
  .Motor_TIM = TIM1,
  .ARMING_TIME = 200,
  .PWM_TURNOFF_MAX  = 500,   
  .TURNOFF_TIME_MAX = 500, 
  .Ton_max =  ESC_TON_MAX,                      /*!<  Maximum ton value for PWM (by default is 1800 us) */
  .Ton_min =  ESC_TON_MIN,                      /*!<  Minimum ton value for PWM (by default is 1080 us) */ 
  .Ton_arming = ESC_TON_ARMING,                 /*!<  Minimum value to start the arming of PWM */ 
  .delta_Ton_max = ESC_TON_MAX - ESC_TON_MIN,      
  .speed_max_valueRPM = MOTOR_MAX_SPEED_RPM,    /*!< Maximum value for speed reference from Workbench */
  .speed_min_valueRPM = 1000,                   /*!< Set the minimum value for speed reference */
  .motor = M1,
};
/* USER CODE END Additional parameters */  

/******************* (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
