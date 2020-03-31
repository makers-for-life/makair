/**
  ******************************************************************************
  * @file    ics_g4xx_pwm_curr_fdbk.c
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file provides firmware functions that implement current sensor
  *          class to be stantiated when the three shunts current sensing
  *          topology is used. It is specifically designed for STM32G4X
  *          microcontrollers and implements the successive sampling of two motor
  *          current using shared ADC.
  *           + MCU peripheral and handle initialization function
  *           + three shunt current sesnsing
  *           + space vector modulation function
  *           + ADC sampling function
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
#include "ics_g4xx_pwm_curr_fdbk.h"
#include "pwm_common.h"
#include "mc_type.h"

/** @addtogroup MCSDK
  * @{
  */

/** @addtogroup pwm_curr_fdbk
  * @{
  */

/**
 * @defgroup ICS_G4XX_pwm_curr_fdbk ICS 2 ADCs G4 PWM & Current Feedback
 *
 * @brief STM32G4, Shared Resources, ICS PWM & Current Feedback implementation
 *
 * This component is used in applications based on an STM32G4 MCU, using an ICS
 * current sensing topology and 2 ADC peripherals to acquire the current values.
 *
 *
 * @todo: TODO: complete documentation.
 *
 * @{
 */

/* Private defines -----------------------------------------------------------*/
#define TIMxCCER_MASK_CH123        ((uint16_t)  (LL_TIM_CHANNEL_CH1|LL_TIM_CHANNEL_CH1N|\
                                                 LL_TIM_CHANNEL_CH2|LL_TIM_CHANNEL_CH2N|\
                                                 LL_TIM_CHANNEL_CH3|LL_TIM_CHANNEL_CH3N))

/* Private typedef -----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void ICS_TIMxInit( TIM_TypeDef * TIMx, PWMC_Handle_t * pHdl );
static void ICS_ADCxInit( ADC_TypeDef * ADCx );
static void ICS_HFCurrentsPolarization( PWMC_Handle_t * pHdl,ab_t * Iab );

/**
  * @brief  It initializes TIMx, ADC, GPIO, DMA1 and NVIC for current reading
  *         in ICS topology using STM32G4X and shared ADC
  * @param  pHandle: handler of the current instance of the PWM component
  * @retval none
  */
__weak void ICS_Init( PWMC_ICS_Handle_t * pHandle )
{
  TIM_TypeDef * TIMx = pHandle->pParams_str->TIMx;
  ADC_TypeDef * ADCx_1 = pHandle->pParams_str->ADCx_1;
  ADC_TypeDef * ADCx_2 = pHandle->pParams_str->ADCx_2;

  /*Check that _Super is the first member of the structure PWMC_ICS_Handle_t */
  if ( ( uint32_t )pHandle == ( uint32_t )&pHandle->_Super )
  {
    /* disable IT and flags in case of LL driver usage
     * workaround for unwanted interrupt enabling done by LL driver */
    LL_ADC_DisableIT_EOC( ADCx_1 );
    LL_ADC_ClearFlag_EOC( ADCx_1 );
    LL_ADC_DisableIT_JEOC( ADCx_1 );
    LL_ADC_ClearFlag_JEOC( ADCx_1 );
    LL_ADC_DisableIT_EOC( ADCx_2 );
    LL_ADC_ClearFlag_EOC( ADCx_2 );
    LL_ADC_DisableIT_JEOC( ADCx_2 );
    LL_ADC_ClearFlag_JEOC( ADCx_2 );

    if ( TIMx == TIM1 )
    {
      /* TIM1 Counter Clock stopped when the core is halted */
      LL_DBGMCU_APB2_GRP1_FreezePeriph( LL_DBGMCU_APB2_GRP1_TIM1_STOP );
    }
    else
    {
      /* TIM8 Counter Clock stopped when the core is halted */
      LL_DBGMCU_APB2_GRP1_FreezePeriph( LL_DBGMCU_APB2_GRP1_TIM8_STOP );
    }
    

    if (LL_ADC_IsEnabled (ADCx_1) == 0)
    {
      ICS_ADCxInit (ADCx_1);
      /* Only the Interrupt of the first ADC is enabled. 
       * As Both ADCs are fired by HW at the same moment 
       * It is safe to consider that both conversion are ready at the same time*/
      LL_ADC_ClearFlag_JEOS( ADCx_1 );
      LL_ADC_EnableIT_JEOS( ADCx_1 );
    }
    else 
    {
      /* Nothing to do ADCx_1 already configured */
    }
    if (LL_ADC_IsEnabled (ADCx_2) == 0)
    {
      ICS_ADCxInit (ADCx_2);
    }    
    else 
    {
      /* Nothing to do ADCx_2 already configured */
    }
    ICS_TIMxInit( TIMx, &pHandle->_Super );
  }
}

static void ICS_ADCxInit( ADC_TypeDef * ADCx )
{
  /* - Exit from deep-power-down mode */     
  LL_ADC_DisableDeepPowerDown(ADCx);
   
  if ( LL_ADC_IsInternalRegulatorEnabled(ADCx) == 0u)
  {
    /* Enable ADC internal voltage regulator */
    LL_ADC_EnableInternalRegulator(ADCx);
  
    /* Wait for Regulator Startup time, once for both */
    /* Note: Variable divided by 2 to compensate partially              */
    /*       CPU processing cycles, scaling in us split to not          */
    /*       exceed 32 bits register capacity and handle low frequency. */
    volatile uint32_t wait_loop_index = ((LL_ADC_DELAY_INTERNAL_REGUL_STAB_US / 10UL) * (SystemCoreClock / (100000UL * 2UL)));      
    while(wait_loop_index != 0UL)
    {
      wait_loop_index--;
    }
  }
  
  LL_ADC_StartCalibration( ADCx, LL_ADC_SINGLE_ENDED );
  while ( LL_ADC_IsCalibrationOnGoing( ADCx) == 1u) 
  {}
  /* ADC Enable (must be done after calibration) */
  /* ADC5-140924: Enabling the ADC by setting ADEN bit soon after polling ADCAL=0 
  * following a calibration phase, could have no effect on ADC 
  * within certain AHB/ADC clock ratio.
  */
  while (  LL_ADC_IsActiveFlag_ADRDY( ADCx ) == 0u)  
  { 
    LL_ADC_Enable(  ADCx );
  }
  /* Clear JSQR from CubeMX setting to avoid not wanting conversion*/
  LL_ADC_INJ_StartConversion( ADCx ); 
  LL_ADC_INJ_StopConversion(ADCx);
  /* TODO: check if not already done by MX */
  LL_ADC_INJ_SetQueueMode( ADCx, LL_ADC_INJ_QUEUE_2CONTEXTS_END_EMPTY );
 }

/**
  * @brief  It initializes TIMx peripheral for PWM generation
  * @param TIMx: Timer to be initialized
  * @param pHandle: handler of the current instance of the PWM component
  * @retval none
  */
static void ICS_TIMxInit( TIM_TypeDef * TIMx, PWMC_Handle_t * pHdl )
{
#if defined (__ICCARM__)
  #pragma cstat_disable = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  PWMC_ICS_Handle_t * pHandle = ( PWMC_ICS_Handle_t * )pHdl;
#if defined (__ICCARM__)
  #pragma cstat_restore = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  uint32_t Brk2Timeout = 1000;

  /* disable main TIM counter to ensure
   * a synchronous start by TIM2 trigger */
  LL_TIM_DisableCounter( TIMx );
  
  LL_TIM_SetTriggerOutput(TIMx, LL_TIM_TRGO_RESET);

  /* Enables the TIMx Preload on CC1 Register */
  LL_TIM_OC_EnablePreload( TIMx, LL_TIM_CHANNEL_CH1 );
  /* Enables the TIMx Preload on CC2 Register */
  LL_TIM_OC_EnablePreload( TIMx, LL_TIM_CHANNEL_CH2 );
  /* Enables the TIMx Preload on CC3 Register */
  LL_TIM_OC_EnablePreload( TIMx, LL_TIM_CHANNEL_CH3 );
  /* Enables the TIMx Preload on CC4 Register */
  LL_TIM_OC_EnablePreload( TIMx, LL_TIM_CHANNEL_CH4 );
  /* Prepare timer for synchronization */
  LL_TIM_GenerateEvent_UPDATE( TIMx );
  if ( pHandle->pParams_str->FreqRatio == 2u )
  {
    if ( pHandle->pParams_str->IsHigherFreqTim == HIGHER_FREQ )
    {
      if ( pHandle->pParams_str->RepetitionCounter == 3u )
      {
        /* Set TIMx repetition counter to 1 */
        LL_TIM_SetRepetitionCounter( TIMx, 1 );
        LL_TIM_GenerateEvent_UPDATE( TIMx );
        /* Repetition counter will be set to 3 at next Update */
        LL_TIM_SetRepetitionCounter( TIMx, 3 );
      }
    }
    LL_TIM_SetCounter( TIMx, ( uint32_t )( pHandle->Half_PWMPeriod ) - 1u );
  }
  else /* bFreqRatio equal to 1 or 3 */
  {
    if ( pHandle->_Super.Motor == M1 )
    {
      if ( pHandle->pParams_str->RepetitionCounter == 1u )
      {
        LL_TIM_SetCounter( TIMx, ( uint32_t )( pHandle->Half_PWMPeriod ) - 1u );
      }
      else if ( pHandle->pParams_str->RepetitionCounter == 3u )
      {
        /* Set TIMx repetition counter to 1 */
        LL_TIM_SetRepetitionCounter( TIMx, 1 );
        LL_TIM_GenerateEvent_UPDATE( TIMx );
        /* Repetition counter will be set to 3 at next Update */
        LL_TIM_SetRepetitionCounter( TIMx, 3 );
      }
      else
      {
      }
    }
    else
    {
    }
  }
  LL_TIM_ClearFlag_BRK( TIMx );
  
  if ( ( pHandle->pParams_str->BKIN2Mode ) != NONE )
  {
    while ((LL_TIM_IsActiveFlag_BRK2 (TIMx) == 1u) && (Brk2Timeout != 0u) )
    {
      LL_TIM_ClearFlag_BRK2( TIMx );
      Brk2Timeout--;
    }   
  }
  LL_TIM_EnableIT_BRK( TIMx );
}

/**
  * @brief  It stores into the component the voltage present on Ia and
  *         Ib current feedback analog channels when no current is flowing into the
  *         motor
  * @param  pHdl: handler of the current instance of the PWM component
  * @retval none
  */
__weak void ICS_CurrentReadingPolarization( PWMC_Handle_t * pHdl )
{
#if defined (__ICCARM__)
  #pragma cstat_disable = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  PWMC_ICS_Handle_t * pHandle = ( PWMC_ICS_Handle_t * )pHdl;
#if defined (__ICCARM__)
  #pragma cstat_restore = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  TIM_TypeDef * TIMx = pHandle->pParams_str->TIMx;
  ADC_TypeDef * ADCx_1 = pHandle->pParams_str->ADCx_1;
  ADC_TypeDef * ADCx_2 = pHandle->pParams_str->ADCx_2;

  pHandle->PhaseAOffset = 0u;
  pHandle->PhaseBOffset = 0u;

  pHandle->PolarizationCounter = 0u;

  /* It forces inactive level on TIMx CHy and CHyN */
  LL_TIM_CC_DisableChannel(TIMx, TIMxCCER_MASK_CH123);

  /* Offset calibration for all phases */
  /* Change function to be executed in ADCx_ISR */
  pHandle->_Super.pFctGetPhaseCurrents = &ICS_HFCurrentsPolarization;
  ICS_SwitchOnPWM( &pHandle->_Super );
  
  /* IF CH4 is enabled, it means that JSQR is now configured to sample polarization current*/
  while ( ((TIMx->CR2) & TIM_CR2_MMS_Msk) != LL_TIM_TRGO_OC4REF )
  {
  }
  /* It is the right time to start the ADC without unwanted conversion */
  /* Start ADC to wait for external trigger. This is series dependent*/
  LL_ADC_INJ_StartConversion( ADCx_1 );
  LL_ADC_INJ_StartConversion( ADCx_2 );
  
  /* Wait for NB_CONVERSIONS to be executed */
  waitForPolarizationEnd( TIMx,
  		                  &pHandle->_Super.SWerror,
  						  pHandle->pParams_str->RepetitionCounter,
  						  &pHandle->PolarizationCounter );

  ICS_SwitchOffPWM( &pHandle->_Super );
  
  pHandle->PhaseAOffset /= NB_CONVERSIONS;
  pHandle->PhaseBOffset /= NB_CONVERSIONS;

  /* Change back function to be executed in ADCx_ISR */
  pHandle->_Super.pFctGetPhaseCurrents = &ICS_GetPhaseCurrents;
  pHandle->_Super.pFctSetADCSampPointSectX = &ICS_WriteTIMRegisters;

  /* It over write TIMx CCRy wrongly written by FOC during calibration so as to
     force 50% duty cycle on the three inverer legs */
  /* Disable TIMx preload */
  LL_TIM_OC_DisablePreload(TIMx,  LL_TIM_CHANNEL_CH1);
  LL_TIM_OC_DisablePreload(TIMx,  LL_TIM_CHANNEL_CH2);  
  LL_TIM_OC_DisablePreload(TIMx,  LL_TIM_CHANNEL_CH3);
  LL_TIM_OC_SetCompareCH1 (TIMx, pHandle->Half_PWMPeriod);
  LL_TIM_OC_SetCompareCH2 (TIMx, pHandle->Half_PWMPeriod);
  LL_TIM_OC_SetCompareCH3 (TIMx, pHandle->Half_PWMPeriod);
  /* Enable TIMx preload */
  LL_TIM_OC_EnablePreload(TIMx,  LL_TIM_CHANNEL_CH1);
  LL_TIM_OC_EnablePreload(TIMx,  LL_TIM_CHANNEL_CH2);  
  LL_TIM_OC_EnablePreload(TIMx,  LL_TIM_CHANNEL_CH3);

  /* It re-enable drive of TIMx CHy and CHyN by TIMx CHyRef*/
  LL_TIM_CC_EnableChannel(TIMx, TIMxCCER_MASK_CH123);

  pHandle->BrakeActionLock = false;

}

#if defined (CCMRAM)
#if defined (__ICCARM__)
#pragma location = ".ccmram"
#elif defined (__CC_ARM)
__attribute__( ( section ( ".ccmram" ) ) )
#endif
#endif
/**
  * @brief  It computes and return latest converted motor phase currents motor
  * @param  pHdl: handler of the current instance of the PWM component
  * @retval Ia and Ib current in Curr_Components format
  */ 
__weak void ICS_GetPhaseCurrents( PWMC_Handle_t * pHdl, ab_t * Iab )
{
#if defined (__ICCARM__)
  #pragma cstat_disable = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  PWMC_ICS_Handle_t * pHandle = ( PWMC_ICS_Handle_t * )pHdl;
#if defined (__ICCARM__)
  #pragma cstat_restore = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  TIM_TypeDef * TIMx = pHandle->pParams_str->TIMx;
  ADC_TypeDef * ADCx_1 = pHandle->pParams_str->ADCx_1;
  ADC_TypeDef * ADCx_2 = pHandle->pParams_str->ADCx_2;
  int32_t aux;
  uint16_t reg;
  
  /* disable ADC trigger source */
  LL_TIM_SetTriggerOutput(TIMx, LL_TIM_TRGO_RESET);
  
  /* Ia = (hPhaseAOffset)-(PHASE_A_ADC_CHANNEL value)  */
  reg = ( uint16_t )( ADCx_1->JDR1 );
  aux = ( int32_t )( reg ) - ( int32_t )( pHandle->PhaseAOffset );

  /* Saturation of Ia */
  if ( aux < -INT16_MAX )
  {
	  Iab->a = -INT16_MAX;
  }
  else  if ( aux > INT16_MAX )
  {
	  Iab->a = INT16_MAX;
  }
  else
  {
	  Iab->a = ( int16_t )aux;
  }

  /* Ib = (hPhaseBOffset)-(PHASE_B_ADC_CHANNEL value) */
  reg = ( uint16_t )( ADCx_2->JDR1 );
  aux = ( int32_t )( reg ) - ( int32_t )( pHandle->PhaseBOffset );

  /* Saturation of Ib */
  if ( aux < -INT16_MAX )
  {
	  Iab->b = -INT16_MAX;
  }
  else  if ( aux > INT16_MAX )
  {
	  Iab->b = INT16_MAX;
  }
  else
  {
	  Iab->b = ( int16_t )aux;
  }

  pHandle->_Super.Ia = Iab->a;
  pHandle->_Super.Ib = Iab->b;
  pHandle->_Super.Ic = -Iab->a - Iab->b;
}

#if defined (CCMRAM)
#if defined (__ICCARM__)
#pragma location = ".ccmram"
#elif defined (__CC_ARM)
__attribute__( ( section ( ".ccmram" ) ) )
#endif
#endif
/**
  * @brief  Stores into the component's handle the voltage present on Ia and
  *         Ib current feedback analog channels when no current is flowing into the
  *         motor
  * @param  pHandle handler of the current instance of the PWM component
  * @retval none
  */
__weak uint16_t ICS_WriteTIMRegisters( PWMC_Handle_t * pHdl )
{
#if defined (__ICCARM__)
  #pragma cstat_disable = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  PWMC_ICS_Handle_t * pHandle = ( PWMC_ICS_Handle_t * )pHdl;
#if defined (__ICCARM__)
  #pragma cstat_restore = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  TIM_TypeDef * TIMx = pHandle->pParams_str->TIMx;
  uint16_t Aux;


  LL_TIM_OC_SetCompareCH1 ( TIMx, (uint32_t) pHandle->_Super.CntPhA );
  LL_TIM_OC_SetCompareCH2 ( TIMx, (uint32_t) pHandle->_Super.CntPhB );
  LL_TIM_OC_SetCompareCH3 ( TIMx, (uint32_t) pHandle->_Super.CntPhC );

  /* Limit for update event */
  if (((TIMx->CR2) & TIM_CR2_MMS_Msk) != LL_TIM_TRGO_RESET )
  {
    Aux = MC_FOC_DURATION;
  }
  else
  {
    Aux = MC_NO_ERROR;
  }
  return Aux;
}
/**
  * @brief  Implementation of PWMC_GetPhaseCurrents to be performed during
  *         calibration. It sum up injected conversion data into PhaseAOffset and
  *         PhaseBOffset to compute the offset introduced in the current feedback
  *         network. It is required to proper configure ADC inputs before to enable
  *         the offset computation.
  * @param  pHdl Pointer on the target component instance
  * @retval It always returns {0,0} in Curr_Components format
  */
static void ICS_HFCurrentsPolarization( PWMC_Handle_t * pHdl, ab_t * Iab )
{
#if defined (__ICCARM__)
  #pragma cstat_disable = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  PWMC_ICS_Handle_t * pHandle = ( PWMC_ICS_Handle_t * )pHdl;
#if defined (__ICCARM__)
  #pragma cstat_restore = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  TIM_TypeDef * TIMx = pHandle->pParams_str->TIMx;
  ADC_TypeDef * ADCx_1 = pHandle->pParams_str->ADCx_1;
  ADC_TypeDef * ADCx_2 = pHandle->pParams_str->ADCx_2;
   
  /* disable ADC trigger source */
    LL_TIM_SetTriggerOutput(TIMx, LL_TIM_TRGO_RESET);

  if ( pHandle->PolarizationCounter < NB_CONVERSIONS )
  {
    pHandle-> PhaseAOffset += ADCx_1->JDR1;
    pHandle-> PhaseBOffset += ADCx_2->JDR1;
    pHandle->PolarizationCounter++;
  }

  /* during offset calibration no current is flowing in the phases */
  Iab->a = 0;
  Iab->b = 0;
}

/**
  * @brief  It turns on low sides switches. This function is intended to be
  *         used for charging boot capacitors of driving section. It has to be
  *         called each motor start-up when using high voltage drivers
  * @param  pHdl: handler of the current instance of the PWM component
  * @retval none
  */
__weak void ICS_TurnOnLowSides( PWMC_Handle_t * pHdl )
{
#if defined (__ICCARM__)
  #pragma cstat_disable = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  PWMC_ICS_Handle_t * pHandle = ( PWMC_ICS_Handle_t * )pHdl;
#if defined (__ICCARM__)
  #pragma cstat_restore = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  TIM_TypeDef * TIMx = pHandle->pParams_str->TIMx;

  pHandle->_Super.TurnOnLowSidesAction = true;

  /* Clear Update Flag */
  LL_TIM_ClearFlag_UPDATE( pHandle->pParams_str->TIMx );

  /*Turn on the three low side switches */
  LL_TIM_OC_SetCompareCH1( TIMx, 0u );
  LL_TIM_OC_SetCompareCH2( TIMx, 0u );
  LL_TIM_OC_SetCompareCH3( TIMx, 0u );

  /* Wait until next update */
  while ( LL_TIM_IsActiveFlag_UPDATE( TIMx ) == 0u )
  {}

  /* Main PWM Output Enable */
  LL_TIM_EnableAllOutputs( TIMx );

  if ( ( pHandle->pParams_str->LowSideOutputs ) == ES_GPIO )
  {
    LL_GPIO_SetOutputPin( pHandle->pParams_str->pwm_en_u_port, pHandle->pParams_str->pwm_en_u_pin );
    LL_GPIO_SetOutputPin( pHandle->pParams_str->pwm_en_v_port, pHandle->pParams_str->pwm_en_v_pin );
    LL_GPIO_SetOutputPin( pHandle->pParams_str->pwm_en_w_port, pHandle->pParams_str->pwm_en_w_pin );
  }
  return;
}


/**
  * @brief  It enables PWM generation on the proper Timer peripheral acting on MOE
  *         bit
  * @param  pHdl: handler of the current instance of the PWM component
  * @retval none
  */
__weak void ICS_SwitchOnPWM( PWMC_Handle_t * pHdl )
{
#if defined (__ICCARM__)
  #pragma cstat_disable = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  PWMC_ICS_Handle_t * pHandle = ( PWMC_ICS_Handle_t * )pHdl;
#if defined (__ICCARM__)
  #pragma cstat_restore = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  TIM_TypeDef * TIMx = pHandle->pParams_str->TIMx;

  pHandle->_Super.TurnOnLowSidesAction = false;

  /* Set all duty to 50% */
  LL_TIM_OC_SetCompareCH1(TIMx, ((uint32_t) pHandle->Half_PWMPeriod / (uint32_t) 2));
  LL_TIM_OC_SetCompareCH2(TIMx, ((uint32_t) pHandle->Half_PWMPeriod / (uint32_t) 2));
  LL_TIM_OC_SetCompareCH3(TIMx, ((uint32_t) pHandle->Half_PWMPeriod / (uint32_t) 2));
  LL_TIM_OC_SetCompareCH4(TIMx, ((uint32_t) pHandle->Half_PWMPeriod - (uint32_t) 5));

  /* wait for a new PWM period */
  LL_TIM_ClearFlag_UPDATE( TIMx );
  while ( LL_TIM_IsActiveFlag_UPDATE( TIMx ) == 0u )
  {}
  LL_TIM_ClearFlag_UPDATE( TIMx );

  /* Main PWM Output Enable */
  TIMx->BDTR |= LL_TIM_OSSI_ENABLE;
  LL_TIM_EnableAllOutputs ( TIMx );

  if ( ( pHandle->pParams_str->LowSideOutputs ) == ES_GPIO )
  {
    if ( ( TIMx->CCER & TIMxCCER_MASK_CH123 ) != 0u )
    {
      LL_GPIO_SetOutputPin( pHandle->pParams_str->pwm_en_u_port, pHandle->pParams_str->pwm_en_u_pin );
      LL_GPIO_SetOutputPin( pHandle->pParams_str->pwm_en_v_port, pHandle->pParams_str->pwm_en_v_pin );
      LL_GPIO_SetOutputPin( pHandle->pParams_str->pwm_en_w_port, pHandle->pParams_str->pwm_en_w_pin );
    }
    else
    {
      /* It is executed during calibration phase the EN signal shall stay off */
      LL_GPIO_ResetOutputPin( pHandle->pParams_str->pwm_en_u_port, pHandle->pParams_str->pwm_en_u_pin );
      LL_GPIO_ResetOutputPin( pHandle->pParams_str->pwm_en_v_port, pHandle->pParams_str->pwm_en_v_pin );
      LL_GPIO_ResetOutputPin( pHandle->pParams_str->pwm_en_w_port, pHandle->pParams_str->pwm_en_w_pin );
    }
  }
  /* Clear Update Flag */
  LL_TIM_ClearFlag_UPDATE( TIMx );
  /* Enable Update IRQ */
  LL_TIM_EnableIT_UPDATE( TIMx );
}


/**
  * @brief  It disables PWM generation on the proper Timer peripheral acting on
  *         MOE bit
  * @param  pHdl: handler of the current instance of the PWM component
  * @retval none
  */
__weak void ICS_SwitchOffPWM( PWMC_Handle_t * pHdl )
{
#if defined (__ICCARM__)
  #pragma cstat_disable = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  PWMC_ICS_Handle_t * pHandle = ( PWMC_ICS_Handle_t * )pHdl;
#if defined (__ICCARM__)
  #pragma cstat_restore = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  TIM_TypeDef * TIMx = pHandle->pParams_str->TIMx;

  /* Disable UPDATE ISR */
  LL_TIM_DisableIT_UPDATE( TIMx );

  pHandle->_Super.TurnOnLowSidesAction = false;
  
  /* Main PWM Output Disable */
  if ( pHandle->BrakeActionLock == true )
  {
  }
  else
  {
    TIMx->BDTR &= ~( ( uint32_t )( LL_TIM_OSSI_ENABLE ) );
    LL_TIM_DisableAllOutputs( TIMx );

    if ( ( pHandle->pParams_str->LowSideOutputs ) == ES_GPIO )
    {
      LL_GPIO_ResetOutputPin( pHandle->pParams_str->pwm_en_u_port, pHandle->pParams_str->pwm_en_u_pin );
      LL_GPIO_ResetOutputPin( pHandle->pParams_str->pwm_en_v_port, pHandle->pParams_str->pwm_en_v_pin );
      LL_GPIO_ResetOutputPin( pHandle->pParams_str->pwm_en_w_port, pHandle->pParams_str->pwm_en_w_pin );
    }
  }

  /* wait for a new PWM period to flush last HF task */
  LL_TIM_ClearFlag_UPDATE( TIMx );
  while ( LL_TIM_IsActiveFlag_UPDATE( TIMx ) == 0u )
  {}
  LL_TIM_ClearFlag_UPDATE( TIMx );

}




#if defined (CCMRAM)
#if defined (__ICCARM__)
#pragma location = ".ccmram"
#elif defined (__CC_ARM)
__attribute__( ( section ( ".ccmram" ) ) )
#endif
#endif
/**
  * @brief  It contains the TIMx Update event interrupt
  * @param  pHandle: handler of the current instance of the PWM component
  * @retval none
  */
__weak void * ICS_TIMx_UP_IRQHandler( PWMC_ICS_Handle_t * pHandle )
{
  TIM_TypeDef * TIMx = pHandle->pParams_str->TIMx;
  ADC_TypeDef * ADCx_1 = pHandle->pParams_str->ADCx_1;
  ADC_TypeDef * ADCx_2 = pHandle->pParams_str->ADCx_2;
  

  ADCx_1->JSQR = ( uint32_t ) pHandle->pParams_str->ADCConfig1;
  ADCx_2->JSQR = ( uint32_t ) pHandle->pParams_str->ADCConfig2;

  /* enable ADC trigger source */
  LL_TIM_SetTriggerOutput(TIMx, LL_TIM_TRGO_OC4REF);

  return &( pHandle->_Super.Motor );
}

#if defined (CCMRAM)
#if defined (__ICCARM__)
#pragma location = ".ccmram"
#elif defined (__CC_ARM)
__attribute__( ( section ( ".ccmram" ) ) )
#endif
#endif
/**
  * @brief  It contains the TIMx Break2 event interrupt
  * @param  pHandle: handler of the current instance of the PWM component
  * @retval none
  */
__weak void * ICS_BRK2_IRQHandler( PWMC_ICS_Handle_t * pHandle )
{
  if ( pHandle->BrakeActionLock == false )
  {
    if ( ( pHandle->pParams_str->LowSideOutputs ) == ES_GPIO )
    {
      LL_GPIO_ResetOutputPin( pHandle->pParams_str->pwm_en_u_port, pHandle->pParams_str->pwm_en_u_pin );
      LL_GPIO_ResetOutputPin( pHandle->pParams_str->pwm_en_v_port, pHandle->pParams_str->pwm_en_v_pin );
      LL_GPIO_ResetOutputPin( pHandle->pParams_str->pwm_en_w_port, pHandle->pParams_str->pwm_en_w_pin );
    }
  }
  pHandle->OverCurrentFlag = true;

  return &( pHandle->_Super.Motor );
}

#if defined (CCMRAM)
#if defined (__ICCARM__)
#pragma location = ".ccmram"
#elif defined (__CC_ARM)
__attribute__( ( section ( ".ccmram" ) ) )
#endif
#endif
/**
  * @brief  It contains the TIMx Break1 event interrupt
  * @param  pHandle: handler of the current instance of the PWM component
  * @retval none
  */
__weak void * ICS_BRK_IRQHandler( PWMC_ICS_Handle_t * pHandle )
{

  pHandle->pParams_str->TIMx->BDTR |= LL_TIM_OSSI_ENABLE;
  pHandle->OverVoltageFlag = true;
  pHandle->BrakeActionLock = true;

  return &( pHandle->_Super.Motor );
}


/**
  * @brief  It is used to check if an overcurrent occurred since last call.
  * @param  pHdl Pointer on the target component instance
  * @retval uint16_t It returns MC_BREAK_IN whether an overcurrent has been
  *                  detected since last method call, MC_NO_FAULTS otherwise.
  */
__weak uint16_t ICS_IsOverCurrentOccurred( PWMC_Handle_t * pHdl )
{
#if defined (__ICCARM__)
  #pragma cstat_disable = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  PWMC_ICS_Handle_t * pHandle = ( PWMC_ICS_Handle_t * )pHdl;
#if defined (__ICCARM__)
  #pragma cstat_restore = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  uint16_t retVal = MC_NO_FAULTS;

  if ( pHandle->OverVoltageFlag == true )
  {
    retVal = MC_OVER_VOLT;
    pHandle->OverVoltageFlag = false;
  }

  if ( pHandle->OverCurrentFlag == true )
  {
    retVal |= MC_BREAK_IN;
    pHandle->OverCurrentFlag = false;
  }

  return retVal;
}



/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
