/**
  ******************************************************************************
  * @file    r3_2_g4xx_pwm_curr_fdbk.c
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file provides firmware functions that implement current sensor
  *          class to be stantiated when the three shunts current sensing
  *          topology is used. It is specifically designed for STM32F30X
  *          microcontrollers and implements the successive sampling of two motor
  *          current using shared ADC.
  *           + MCU peripheral and handle initialization function
  *           + three shunts current sensing
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
#include "r3_2_g4xx_pwm_curr_fdbk.h"
#include "pwm_common.h"
#include "mc_type.h"

/** @addtogroup MCSDK
  * @{
  */

/** @addtogroup pwm_curr_fdbk
  * @{
  */

/**
 * @defgroup R3_2_G4XX_pwm_curr_fdbk R3 2 ADCs G4 PWM & Current Feedback
 *
 * @brief STM32G4, Shared Resources, 3-Shunt PWM & Current Feedback implementation
 *
 * This component is used in applications based on an STM32G4 MCU, using a three
 * shunt resistors current sensing topology and 2 ADC peripherals to acquire the current
 * values.
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
static void R3_2_TIMxInit( TIM_TypeDef * TIMx, PWMC_Handle_t * pHdl );
static void R3_2_ADCxInit( ADC_TypeDef * ADCx );
__STATIC_INLINE uint16_t R3_2_WriteTIMRegisters( PWMC_Handle_t * pHdl, uint16_t hCCR4Reg  );
static void R3_2_HFCurrentsPolarizationAB( PWMC_Handle_t * pHdl,ab_t * Iab );
static void R3_2_HFCurrentsPolarizationC( PWMC_Handle_t * pHdl, ab_t * Iab );
static void R3_2_SetAOReferenceVoltage( uint32_t DAC_Channel, DAC_TypeDef * DACx, uint16_t hDACVref );
uint16_t R3_2_SetADCSampPointPolarization( PWMC_Handle_t * pHdl) ;
static void R3_2_RLGetPhaseCurrents( PWMC_Handle_t * pHdl, ab_t * pStator_Currents );
static void R3_2_RLTurnOnLowSides( PWMC_Handle_t * pHdl );
static void R3_2_RLSwitchOnPWM( PWMC_Handle_t * pHdl );
/**
  * @brief  It initializes TIMx, ADC, GPIO, DMA1 and NVIC for current reading
  *         in three shunt topology using STM32F30X and shared ADC
  * @param  pHandle: handler of the current instance of the PWM component
  * @retval none
  */
__weak void R3_2_Init( PWMC_R3_2_Handle_t * pHandle )
{
  R3_3_OPAMPParams_t * OPAMPParams = pHandle->pParams_str->OPAMPParams;
  COMP_TypeDef * COMP_OCPAx = pHandle->pParams_str->CompOCPASelection;
  COMP_TypeDef * COMP_OCPBx = pHandle->pParams_str->CompOCPBSelection;
  COMP_TypeDef * COMP_OCPCx = pHandle->pParams_str->CompOCPCSelection;
  COMP_TypeDef * COMP_OVPx = pHandle->pParams_str->CompOVPSelection;
  DAC_TypeDef * DAC_OCPAx = pHandle->pParams_str->DAC_OCP_ASelection;
  DAC_TypeDef * DAC_OCPBx = pHandle->pParams_str->DAC_OCP_BSelection;
  DAC_TypeDef * DAC_OCPCx = pHandle->pParams_str->DAC_OCP_CSelection;
  DAC_TypeDef * DAC_OVPx = pHandle->pParams_str->DAC_OVP_Selection;
  TIM_TypeDef * TIMx = pHandle->pParams_str->TIMx;
  ADC_TypeDef * ADCx_1 = pHandle->pParams_str->ADCx_1;
  ADC_TypeDef * ADCx_2 = pHandle->pParams_str->ADCx_2;

  /*Check that _Super is the first member of the structure PWMC_R3_2_Handle_t */
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
    if ( OPAMPParams != NULL )
    {
    /* Testing of all OPAMP one by one is required as 2 or 3 OPAMPS cfg may exist*/
     if (OPAMPParams -> OPAMPx_1 != NULL ) 
     {
      LL_OPAMP_Enable( OPAMPParams->OPAMPx_1 );
     }
     if (OPAMPParams -> OPAMPx_2 != NULL ) 
     {
      LL_OPAMP_Enable( OPAMPParams->OPAMPx_2 );
     }
     if (OPAMPParams -> OPAMPx_3 != NULL ) 
     {
      LL_OPAMP_Enable( OPAMPParams->OPAMPx_3 );
     }     
    }

    /* Over current protection phase A */
    if ( COMP_OCPAx != NULL )
    {
      /* Inverting input*/
      if ( pHandle->pParams_str->CompOCPAInvInput_MODE != EXT_MODE )
      {
        R3_2_SetAOReferenceVoltage( pHandle->pParams_str->DAC_Channel_OCPA, DAC_OCPAx, ( uint16_t )( pHandle->pParams_str->DAC_OCP_Threshold ) );
      }
      /* Output */
      LL_COMP_Enable ( COMP_OCPAx );
      LL_COMP_Lock( COMP_OCPAx );
    }

    /* Over current protection phase B */
    if ( COMP_OCPBx != NULL )
    {
      if ( pHandle->pParams_str->CompOCPBInvInput_MODE != EXT_MODE )
      {
        R3_2_SetAOReferenceVoltage( pHandle->pParams_str->DAC_Channel_OCPB, DAC_OCPBx,( uint16_t )( pHandle->pParams_str->DAC_OCP_Threshold ) );
      }
      LL_COMP_Enable ( COMP_OCPBx );
      LL_COMP_Lock( COMP_OCPBx );
    }

    /* Over current protection phase C */
    if ( COMP_OCPCx != NULL )
    {
      if ( pHandle->pParams_str->CompOCPCInvInput_MODE != EXT_MODE )
      {
        R3_2_SetAOReferenceVoltage( pHandle->pParams_str->DAC_Channel_OCPC, DAC_OCPCx,( uint16_t )( pHandle->pParams_str->DAC_OCP_Threshold ) );
      }
      LL_COMP_Enable ( COMP_OCPCx );
      LL_COMP_Lock( COMP_OCPCx );
    }

    /* Over voltage protection */
    if ( COMP_OVPx != NULL )
    {
      /* Inverting input*/
      if ( pHandle->pParams_str->CompOVPInvInput_MODE != EXT_MODE )
      {
          R3_2_SetAOReferenceVoltage( pHandle->pParams_str->DAC_Channel_OVP, DAC_OVPx,( uint16_t )( pHandle->pParams_str->DAC_OVP_Threshold ) );
      }
      /* Output */
      LL_COMP_Enable ( COMP_OVPx );
      LL_COMP_Lock( COMP_OVPx );
    }
    
    if (LL_ADC_IsEnabled (ADCx_1) == 0)
    {
      R3_2_ADCxInit (ADCx_1);
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
      R3_2_ADCxInit (ADCx_2);
    }    
    else 
    {
      /* Nothing to do ADCx_2 already configured */
    }
    R3_2_TIMxInit( TIMx, &pHandle->_Super );
  }
}

static void R3_2_ADCxInit( ADC_TypeDef * ADCx )
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
static void R3_2_TIMxInit( TIM_TypeDef * TIMx, PWMC_Handle_t * pHdl )
{
#if defined (__ICCARM__)
  #pragma cstat_disable = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  PWMC_R3_2_Handle_t * pHandle = ( PWMC_R3_2_Handle_t * )pHdl;
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
  *         Ib current feedback analog channels when no current is flowin into the
  *         motor
  * @param  pHdl: handler of the current instance of the PWM component
  * @retval none
  */
__weak void R3_2_CurrentReadingPolarization( PWMC_Handle_t * pHdl )
{
#if defined (__ICCARM__)
  #pragma cstat_disable = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  PWMC_R3_2_Handle_t * pHandle = ( PWMC_R3_2_Handle_t * )pHdl;
#if defined (__ICCARM__)
  #pragma cstat_restore = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  TIM_TypeDef * TIMx = pHandle->pParams_str->TIMx;
  ADC_TypeDef * ADCx_1 = pHandle->pParams_str->ADCx_1;
  ADC_TypeDef * ADCx_2 = pHandle->pParams_str->ADCx_2;

  pHandle->PhaseAOffset = 0u;
  pHandle->PhaseBOffset = 0u;
  pHandle->PhaseCOffset = 0u;

  pHandle->PolarizationCounter = 0u;

  /* It forces inactive level on TIMx CHy and CHyN */
  LL_TIM_CC_DisableChannel(TIMx, TIMxCCER_MASK_CH123);

  /* Offset calibration for all phases */
  /* Change function to be executed in ADCx_ISR */
  pHandle->_Super.pFctGetPhaseCurrents = &R3_2_HFCurrentsPolarizationAB;
  pHandle->_Super.pFctSetADCSampPointSectX = &R3_2_SetADCSampPointPolarization;
  pHandle->ADC_ExternalPolarityInjected = (uint16_t) LL_ADC_INJ_TRIG_EXT_RISING;
     
  /* We want to polarize calibration Phase A and Phase B, so we select SECTOR_5 */
  pHandle->PolarizationSector=SECTOR_5;
  /* Required to force first polarization conversion on SECTOR_5*/
  pHandle->_Super.Sector = SECTOR_5;   
  R3_2_SwitchOnPWM( &pHandle->_Super );
  
  /* IF CH4 is enabled, it means that JSQR is now configured to sample polarization current*/
  //while ( LL_TIM_CC_IsEnabledChannel(TIMx, LL_TIM_CHANNEL_CH4) == 0u )
  //{
  //}
  while ( ((TIMx->CR2) & TIM_CR2_MMS_Msk) != LL_TIM_TRGO_OC4REF )
  {
  }
  /* It is the right time to start the ADC without unwanted conversion */
  /* Start ADC to wait for external trigger. This is series dependant*/
  LL_ADC_INJ_StartConversion( ADCx_1 );
  LL_ADC_INJ_StartConversion( ADCx_2 );
  
  /* Wait for NB_CONVERSIONS to be executed */
  waitForPolarizationEnd( TIMx,
  		                  &pHandle->_Super.SWerror,
  						  pHandle->pParams_str->RepetitionCounter,
  						  &pHandle->PolarizationCounter );

  R3_2_SwitchOffPWM( &pHandle->_Super );
  
  /* Offset calibration for C phase */
  pHandle->PolarizationCounter = 0u;
  
  /* Change function to be executed in ADCx_ISR */
  pHandle->_Super.pFctGetPhaseCurrents = &R3_2_HFCurrentsPolarizationC;
  /* We want to polarize Phase C, so we select SECTOR_1 */
  pHandle->PolarizationSector=SECTOR_1;
  /* Required to force first polarization conversion on SECTOR_1*/
  pHandle->_Super.Sector = SECTOR_1;   
  R3_2_SwitchOnPWM( &pHandle->_Super );

  /* Wait for NB_CONVERSIONS to be executed */
  waitForPolarizationEnd( TIMx,
  		                  &pHandle->_Super.SWerror,
  						  pHandle->pParams_str->RepetitionCounter,
  						  &pHandle->PolarizationCounter );
  
  R3_2_SwitchOffPWM( &pHandle->_Super );
  pHandle->PhaseAOffset /= NB_CONVERSIONS;
  pHandle->PhaseBOffset /= NB_CONVERSIONS;
  pHandle->PhaseCOffset /= NB_CONVERSIONS;

  /* Change back function to be executed in ADCx_ISR */
  pHandle->_Super.pFctGetPhaseCurrents = &R3_2_GetPhaseCurrents;
  pHandle->_Super.pFctSetADCSampPointSectX = &R3_2_SetADCSampPointSectX;

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
  
  /* At the end of calibration, all phases are at 50% we will sample A&B */
  pHandle->_Super.Sector=SECTOR_5;
  
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
__weak void R3_2_GetPhaseCurrents( PWMC_Handle_t * pHdl, ab_t * Iab )
{
#if defined (__ICCARM__)
  #pragma cstat_disable = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  PWMC_R3_2_Handle_t * pHandle = ( PWMC_R3_2_Handle_t * )pHdl;  
#if defined (__ICCARM__)
  #pragma cstat_restore = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  TIM_TypeDef * TIMx = pHandle->pParams_str->TIMx;

  uint8_t Sector;
  int32_t Aux;
  uint32_t ADCDataReg1;
  uint32_t ADCDataReg2;
  
  Sector = ( uint8_t )pHandle->_Super.Sector;
  ADCDataReg1 = *pHandle->pParams_str->ADCDataReg1[Sector];
  ADCDataReg2 = *pHandle->pParams_str->ADCDataReg2[Sector];
  
  /* disable ADC trigger source */
  //LL_TIM_CC_DisableChannel(TIMx, LL_TIM_CHANNEL_CH4);  
  LL_TIM_SetTriggerOutput(TIMx, LL_TIM_TRGO_RESET);
  
  switch ( Sector )
  {
    case SECTOR_4:
    case SECTOR_5:
      /* Current on Phase C is not accessible     */
      /* Ia = PhaseAOffset - ADC converted value) */
      Aux = ( int32_t )( pHandle->PhaseAOffset ) - ( int32_t )( ADCDataReg1 );

      /* Saturation of Ia */
      if ( Aux < -INT16_MAX )
      {
        Iab->a = -INT16_MAX;
      }
      else  if ( Aux > INT16_MAX )
      {
        Iab->a = INT16_MAX;
      }
      else
      {
        Iab->a = ( int16_t )Aux;
      }

      /* Ib = PhaseBOffset - ADC converted value) */
      Aux = ( int32_t )( pHandle->PhaseBOffset ) - ( int32_t )( ADCDataReg2 );

      /* Saturation of Ib */
      if ( Aux < -INT16_MAX )
      {
        Iab->b = -INT16_MAX;
      }
      else  if ( Aux > INT16_MAX )
      {
        Iab->b = INT16_MAX;
      }
      else
      {
        Iab->b = ( int16_t )Aux;
      }
      break;

    case SECTOR_6:
    case SECTOR_1:
      /* Current on Phase A is not accessible     */
      /* Ib = PhaseBOffset - ADC converted value) */
      Aux = ( int32_t )( pHandle->PhaseBOffset ) - ( int32_t )( ADCDataReg1 );

      /* Saturation of Ib */
      if ( Aux < -INT16_MAX )
      {
        Iab->b = -INT16_MAX;
      }
      else  if ( Aux > INT16_MAX )
      {
        Iab->b = INT16_MAX;
      }
      else
      {
        Iab->b = ( int16_t )Aux;
      }

      /* Ia = -Ic -Ib */
      Aux = ( int32_t )( ADCDataReg2 ) - ( int32_t )( pHandle->PhaseCOffset ); /* -Ic */
      Aux -= ( int32_t )Iab->b;             /* Ia  */

      /* Saturation of Ia */
      if ( Aux > INT16_MAX )
      {
        Iab->a = INT16_MAX;
      }
      else  if ( Aux < -INT16_MAX )
      {
        Iab->a = -INT16_MAX;
      }
      else
      {
        Iab->a = ( int16_t )Aux;
      }
      break;

    case SECTOR_2:
    case SECTOR_3:
      /* Current on Phase B is not accessible     */
      /* Ia = PhaseAOffset - ADC converted value) */
      Aux = ( int32_t )( pHandle->PhaseAOffset ) - ( int32_t )( ADCDataReg1 );

      /* Saturation of Ia */
      if ( Aux < -INT16_MAX )
      {
        Iab->a = -INT16_MAX;
      }
      else  if ( Aux > INT16_MAX )
      {
        Iab->a = INT16_MAX;
      }
      else
      {
        Iab->a = ( int16_t )Aux;
      }

      /* Ib = -Ic -Ia */
      Aux = ( int32_t )( ADCDataReg2 ) - ( int32_t )( pHandle->PhaseCOffset ); /* -Ic */
      Aux -= ( int32_t )Iab->a;             /* Ib */

      /* Saturation of Ib */
      if ( Aux > INT16_MAX )
      {
        Iab->b = INT16_MAX;
      }
      else  if ( Aux < -INT16_MAX )
      {
        Iab->b = -INT16_MAX;
      }
      else
      {
        Iab->b = ( int16_t )Aux;
      }
      break;

    default:
      break;
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
 * @brief  Configure the ADC for the current sampling during calibration.
 *         It means set the sampling point via TIMx_Ch4 value and polarity
 *         ADC sequence length and channels.
 *         And call the WriteTIMRegisters method.
 * @param  pHandle: handler of the current instance of the PWM component
 * @retval none
 */
uint16_t R3_2_SetADCSampPointPolarization( PWMC_Handle_t * pHdl )
{
#if defined (__ICCARM__)
  #pragma cstat_disable = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  PWMC_R3_2_Handle_t * pHandle = ( PWMC_R3_2_Handle_t * )pHdl;
#if defined (__ICCARM__)
  #pragma cstat_restore = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  pHandle->_Super.Sector = pHandle->PolarizationSector;

  return R3_2_WriteTIMRegisters( &pHandle->_Super, ( pHandle->Half_PWMPeriod - (uint16_t) 1 ) );
}

#if defined (CCMRAM)
#if defined (__ICCARM__)
#pragma location = ".ccmram"
#elif defined (__CC_ARM)
__attribute__( ( section ( ".ccmram" ) ) )
#endif
#endif
/**
  * @brief  Configure the ADC for the current sampling related to sector 1.
  *         It means set the sampling point via TIMx_Ch4 value and polarity
  *         ADC sequence length and channels.
  *         And call the WriteTIMRegisters method.
  * @param  pHandle Pointer on the target component instance
  * @retval none
  */
uint16_t R3_2_SetADCSampPointSectX( PWMC_Handle_t * pHdl )
{
#if defined (__ICCARM__)
  #pragma cstat_disable = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  PWMC_R3_2_Handle_t * pHandle = ( PWMC_R3_2_Handle_t * )pHdl;
#if defined (__ICCARM__)
  #pragma cstat_restore = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  uint16_t SamplingPoint;
  uint16_t DeltaDuty;

  /* Verify that sampling is possible in the middle of PWM by checking the smallest duty cycle */
   if ( ( uint16_t )( pHandle->Half_PWMPeriod - pHdl->lowDuty ) > pHandle->pParams_str->Tafter )
  {
    /* When it is possible to sample in the middle of the PWM period, always sample the same phases
     * (AB are chosen) for all sectors in order to not induce current discontinuities when there are differences
     * between offsets */

    /* sector number needed by GetPhaseCurrent, phase A and B are sampled which corresponds
     * to sector 4 or 5  */
    pHandle->_Super.Sector = SECTOR_5;

    /* set sampling  point trigger in the middle of PWM period */
    SamplingPoint =  pHandle->Half_PWMPeriod - (uint16_t) 1;
  }
  else
  {
    /* In this case it is necessary to convert phases with Maximum and variable complementary duty cycle.*/

    /* ADC Injected sequence configuration. The stator phase with minimum value of complementary
        duty cycle is set as first. In every sector there is always one phase with maximum complementary duty,
        one with minimum complementary duty and one with variable complementary duty. In this case, phases
        with variable complementary duty and with maximum duty are converted and the first will be always
        the phase with variable complementary duty cycle */
    DeltaDuty = ( uint16_t )( pHdl->lowDuty - pHdl->midDuty );

    /* Definition of crossing point */
    if ( DeltaDuty > ( uint16_t )( pHandle->Half_PWMPeriod - pHdl->lowDuty ) * 2u )
    {
      SamplingPoint = pHdl->lowDuty - pHandle->pParams_str->Tbefore;
    }
    else
    {
      SamplingPoint = pHdl->lowDuty + pHandle->pParams_str->Tafter;

      if ( SamplingPoint >= pHandle->Half_PWMPeriod )
      {
         /* ADC trigger edge must be changed from positive to negative */
        pHandle->ADC_ExternalPolarityInjected = (uint16_t) LL_ADC_INJ_TRIG_EXT_FALLING;
        SamplingPoint = ( 2u * pHandle->Half_PWMPeriod ) - SamplingPoint - (uint16_t) 1;
      }
    }
  }
  return R3_2_WriteTIMRegisters( &pHandle->_Super, SamplingPoint );
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
  *         Ib current feedback analog channels when no current is flowin into the
  *         motor
  * @param  pHandle handler of the current instance of the PWM component
  * @retval none
  */
__STATIC_INLINE uint16_t R3_2_WriteTIMRegisters( PWMC_Handle_t * pHdl, uint16_t SamplingPoint )
{
#if defined (__ICCARM__)
  #pragma cstat_disable = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  PWMC_R3_2_Handle_t * pHandle = ( PWMC_R3_2_Handle_t * )pHdl;
#if defined (__ICCARM__)
  #pragma cstat_restore = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  TIM_TypeDef * TIMx = pHandle->pParams_str->TIMx;
  uint16_t Aux;


  LL_TIM_OC_SetCompareCH1 ( TIMx, (uint32_t) pHandle->_Super.CntPhA );
  LL_TIM_OC_SetCompareCH2 ( TIMx, (uint32_t) pHandle->_Super.CntPhB );
  LL_TIM_OC_SetCompareCH3 ( TIMx, (uint32_t) pHandle->_Super.CntPhC );
  LL_TIM_OC_SetCompareCH4( TIMx, (uint32_t) SamplingPoint );

  /* Limit for update event */

//  if ( LL_TIM_CC_IsEnabledChannel(TIMx, LL_TIM_CHANNEL_CH4) == 1u )
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
  * @brief  Implementaion of PWMC_GetPhaseCurrents to be performed during
  *         calibration. It sum up injected conversion data into PhaseAOffset and
  *         PhaseBOffset to compute the offset introduced in the current feedback
  *         network. It is requied to proper configure ADC inputs before to enable
  *         the offset computation.
  * @param  pHdl Pointer on the target component instance
  * @retval It always returns {0,0} in Curr_Components format
  */
static void R3_2_HFCurrentsPolarizationAB( PWMC_Handle_t * pHdl, ab_t * Iab )
{
#if defined (__ICCARM__)
  #pragma cstat_disable = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  PWMC_R3_2_Handle_t * pHandle = ( PWMC_R3_2_Handle_t * )pHdl;
#if defined (__ICCARM__)
  #pragma cstat_restore = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  TIM_TypeDef * TIMx = pHandle->pParams_str->TIMx;
  uint32_t ADCDataReg1 = *pHandle->pParams_str->ADCDataReg1[pHandle->PolarizationSector];
  uint32_t ADCDataReg2 = *pHandle->pParams_str->ADCDataReg2[pHandle->PolarizationSector];
   
  /* disable ADC trigger source */
  //LL_TIM_CC_DisableChannel(TIMx, LL_TIM_CHANNEL_CH4);
    LL_TIM_SetTriggerOutput(TIMx, LL_TIM_TRGO_RESET);

  if ( pHandle->PolarizationCounter < NB_CONVERSIONS )
  {
    pHandle-> PhaseAOffset += ADCDataReg1;
    pHandle-> PhaseBOffset += ADCDataReg2;
    pHandle->PolarizationCounter++;
  }

  /* during offset calibration no current is flowing in the phases */
  Iab->a = 0;
  Iab->b = 0;
}

/**
  * @brief  Implementaion of PWMC_GetPhaseCurrents to be performed during
  *         calibration. It sum up injected conversion data into PhaseAOffset and
  *         PhaseBOffset to compute the offset introduced in the current feedback
  *         network. It is requied to proper configure ADC inputs before to enable
  *         the offset computation.
  * @param  pHdl Pointer on the target component instance
  * @retval It always returns {0,0} in Curr_Components format
  */
static void R3_2_HFCurrentsPolarizationC( PWMC_Handle_t * pHdl, ab_t * Iab )
{
#if defined (__ICCARM__)
  #pragma cstat_disable = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  PWMC_R3_2_Handle_t * pHandle = ( PWMC_R3_2_Handle_t * )pHdl;
#if defined (__ICCARM__)
  #pragma cstat_restore = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  TIM_TypeDef * TIMx = pHandle->pParams_str->TIMx;
  uint32_t ADCDataReg2 = *pHandle->pParams_str->ADCDataReg2[pHandle->PolarizationSector];

  /* disable ADC trigger source */
  //LL_TIM_CC_DisableChannel(TIMx, LL_TIM_CHANNEL_CH4);
    LL_TIM_SetTriggerOutput(TIMx, LL_TIM_TRGO_RESET);

  if ( pHandle->PolarizationCounter < NB_CONVERSIONS )
  {
    /* Phase C is read from SECTOR_1, second value */
    pHandle-> PhaseCOffset += ADCDataReg2;    
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
__weak void R3_2_TurnOnLowSides( PWMC_Handle_t * pHdl )
{
#if defined (__ICCARM__)
  #pragma cstat_disable = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  PWMC_R3_2_Handle_t * pHandle = ( PWMC_R3_2_Handle_t * )pHdl;
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
__weak void R3_2_SwitchOnPWM( PWMC_Handle_t * pHdl )
{
#if defined (__ICCARM__)
  #pragma cstat_disable = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  PWMC_R3_2_Handle_t * pHandle = ( PWMC_R3_2_Handle_t * )pHdl;
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
__weak void R3_2_SwitchOffPWM( PWMC_Handle_t * pHdl )
{
#if defined (__ICCARM__)
  #pragma cstat_disable = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  PWMC_R3_2_Handle_t * pHandle = ( PWMC_R3_2_Handle_t * )pHdl;
#if defined (__ICCARM__)
  #pragma cstat_restore = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  TIM_TypeDef * TIMx = pHandle->pParams_str->TIMx;

  /* Disable UPDATE ISR */
  LL_TIM_DisableIT_UPDATE( TIMx );

  pHandle->_Super.TurnOnLowSidesAction = false;
  
  /* Main PWM Output Disable */
  LL_TIM_DisableAllOutputs( TIMx );
  if ( pHandle->BrakeActionLock == true )
  {
  }
  else
  {
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
__weak void * R3_2_TIMx_UP_IRQHandler( PWMC_R3_2_Handle_t * pHandle )
{
  TIM_TypeDef * TIMx = pHandle->pParams_str->TIMx;
  ADC_TypeDef * ADCx_1 = pHandle->pParams_str->ADCx_1;
  ADC_TypeDef * ADCx_2 = pHandle->pParams_str->ADCx_2;
  R3_3_OPAMPParams_t * OPAMPParams = pHandle->pParams_str->OPAMPParams;
  OPAMP_TypeDef * Opamp;
  uint32_t OpampConfig;
  
  if ( OPAMPParams != NULL )
  {
  /* We can not change OPAMP source if ADC acquisition is ongoing (Dual motor with internal opamp use case)*/  
    while (ADCx_1->JSQR != 0x0u)
    {}
  /* We need to manage the Operational amplifier internal output enable - Dedicated to G4 and the VPSEL selection */
    Opamp = OPAMPParams->OPAMPSelect_1[pHandle->_Super.Sector];
    if (Opamp != NULL )
    {
      OpampConfig = OPAMPParams->OPAMPConfig1[pHandle->_Super.Sector];
      MODIFY_REG (Opamp->CSR, (OPAMP_CSR_OPAMPINTEN | OPAMP_CSR_VPSEL ), OpampConfig);
    }
     Opamp = OPAMPParams->OPAMPSelect_2[pHandle->_Super.Sector];
    if (Opamp != NULL )
    {
      OpampConfig = OPAMPParams->OPAMPConfig2[pHandle->_Super.Sector];
      MODIFY_REG (Opamp->CSR, (OPAMP_CSR_OPAMPINTEN | OPAMP_CSR_VPSEL ), OpampConfig);
    }
  }
 
  ADCx_1->JSQR = pHandle->pParams_str->ADCConfig1[pHandle->_Super.Sector] | (uint32_t) pHandle->ADC_ExternalPolarityInjected;
  ADCx_2->JSQR = pHandle->pParams_str->ADCConfig2[pHandle->_Super.Sector] | (uint32_t) pHandle->ADC_ExternalPolarityInjected;

  /* enable ADC trigger source */
  
  //LL_TIM_CC_EnableChannel(TIMx, LL_TIM_CHANNEL_CH4);
  LL_TIM_SetTriggerOutput(TIMx, LL_TIM_TRGO_OC4REF);
    
  pHandle->ADC_ExternalPolarityInjected = (uint16_t)LL_ADC_INJ_TRIG_EXT_RISING;

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
__weak void * R3_2_BRK2_IRQHandler( PWMC_R3_2_Handle_t * pHandle )
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
__weak void * R3_2_BRK_IRQHandler( PWMC_R3_2_Handle_t * pHandle )
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
__weak uint16_t R3_2_IsOverCurrentOccurred( PWMC_Handle_t * pHdl )
{
#if defined (__ICCARM__)
  #pragma cstat_disable = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  PWMC_R3_2_Handle_t * pHandle = ( PWMC_R3_2_Handle_t * )pHdl;
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
  * @brief  It is used to configure the analog output used for protection
  *         thresholds.
  * @param  DAC_Channel: the selected DAC channel.
  *          This parameter can be:
  *            @arg DAC_Channel_1: DAC Channel1 selected
  *            @arg DAC_Channel_2: DAC Channel2 selected
  * @param  hDACVref Value of DAC reference expressed as 16bit unsigned integer.
  *         Ex. 0 = 0V 65536 = VDD_DAC.
  * @retval none
  */
static void R3_2_SetAOReferenceVoltage( uint32_t DAC_Channel, DAC_TypeDef * DACx, uint16_t hDACVref )
{
  LL_DAC_ConvertData12LeftAligned ( DACx, DAC_Channel, hDACVref );

  /* Enable DAC Channel */
  LL_DAC_TrigSWConversion ( DACx, DAC_Channel );
  
  if (LL_DAC_IsEnabled ( DACx, DAC_Channel ) == 1u )
  { /* If DAC is already enable, we wait LL_DAC_DELAY_VOLTAGE_SETTLING_US*/
    uint32_t wait_loop_index = ((LL_DAC_DELAY_VOLTAGE_SETTLING_US) * (SystemCoreClock / (1000000UL * 2UL)));      
    while(wait_loop_index != 0UL)
    {
      wait_loop_index--;
    }
  }
  else
  {
    /* If DAC is not enabled, we must wait LL_DAC_DELAY_STARTUP_VOLTAGE_SETTLING_US*/
    LL_DAC_Enable ( DACx, DAC_Channel );
    uint32_t wait_loop_index = ((LL_DAC_DELAY_STARTUP_VOLTAGE_SETTLING_US) * (SystemCoreClock / (1000000UL * 2UL)));      
    while(wait_loop_index != 0UL)
    {
      wait_loop_index--;
    }    
  }
}

/**
 * @brief  It is used to set the PWM mode for R/L detection.
 * @param  pHandle: handler of the current instance of the PWM component
 * @param  hDuty to be applied in uint16_t
 * @retval none
 */
void R3_2_RLDetectionModeEnable( PWMC_Handle_t * pHdl )
{
#if defined (__ICCARM__)
  #pragma cstat_disable = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  PWMC_R3_2_Handle_t * pHandle = ( PWMC_R3_2_Handle_t * )pHdl;
#if defined (__ICCARM__)
  #pragma cstat_restore = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  TIM_TypeDef * TIMx = pHandle->pParams_str->TIMx;
  
  if ( pHandle->_Super.RLDetectionMode == false )
  {
    /*  Channel1 configuration */
    LL_TIM_OC_SetMode ( TIMx, LL_TIM_CHANNEL_CH1, LL_TIM_OCMODE_PWM1 );
    LL_TIM_CC_EnableChannel( TIMx, LL_TIM_CHANNEL_CH1 );
    LL_TIM_CC_DisableChannel( TIMx, LL_TIM_CHANNEL_CH1N );
    LL_TIM_OC_SetCompareCH1( TIMx, 0u );

    /*  Channel2 configuration */
    if ( ( pHandle->pParams_str->LowSideOutputs ) == LS_PWM_TIMER )
    {
      LL_TIM_OC_SetMode ( TIMx, LL_TIM_CHANNEL_CH2, LL_TIM_OCMODE_ACTIVE );
      LL_TIM_CC_DisableChannel( TIMx, LL_TIM_CHANNEL_CH2 );
      LL_TIM_CC_EnableChannel( TIMx, LL_TIM_CHANNEL_CH2N );
    }
    else if ( ( pHandle->pParams_str->LowSideOutputs ) == ES_GPIO )
    {
      LL_TIM_OC_SetMode ( TIMx, LL_TIM_CHANNEL_CH2, LL_TIM_OCMODE_INACTIVE );
      LL_TIM_CC_EnableChannel( TIMx, LL_TIM_CHANNEL_CH2 );
      LL_TIM_CC_DisableChannel( TIMx, LL_TIM_CHANNEL_CH2N );
    }
    else
    {
    }

    /*  Channel3 configuration */
    LL_TIM_OC_SetMode ( TIMx, LL_TIM_CHANNEL_CH3, LL_TIM_OCMODE_PWM2 );
    LL_TIM_CC_DisableChannel( TIMx, LL_TIM_CHANNEL_CH3 );
    LL_TIM_CC_DisableChannel( TIMx, LL_TIM_CHANNEL_CH3N );

    pHandle->PhaseAOffset = pHandle->PhaseBOffset; /* Use only the offset of phB */
  }

  pHandle->_Super.pFctGetPhaseCurrents = &R3_2_RLGetPhaseCurrents;
  pHandle->_Super.pFctTurnOnLowSides = &R3_2_RLTurnOnLowSides;
  pHandle->_Super.pFctSwitchOnPwm = &R3_2_RLSwitchOnPWM;
  pHandle->_Super.pFctSwitchOffPwm = &R3_2_SwitchOffPWM;

  pHandle->_Super.RLDetectionMode = true;
}

/**
 * @brief  It is used to disable the PWM mode for R/L detection.
 * @param  pHandle: handler of the current instance of the PWM component
 * @retval none
 */
void R3_2_RLDetectionModeDisable( PWMC_Handle_t * pHdl )
{
#if defined (__ICCARM__)
  #pragma cstat_disable = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  PWMC_R3_2_Handle_t * pHandle = ( PWMC_R3_2_Handle_t * )pHdl;
#if defined (__ICCARM__)
  #pragma cstat_restore = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  TIM_TypeDef * TIMx = pHandle->pParams_str->TIMx;

  if ( pHandle->_Super.RLDetectionMode == true )
  {
    /*  Channel1 configuration */
    LL_TIM_OC_SetMode ( TIMx, LL_TIM_CHANNEL_CH1, LL_TIM_OCMODE_PWM1 );
    LL_TIM_CC_EnableChannel( TIMx, LL_TIM_CHANNEL_CH1 );

    if ( ( pHandle->pParams_str->LowSideOutputs ) == LS_PWM_TIMER )
    {
      LL_TIM_CC_EnableChannel( TIMx, LL_TIM_CHANNEL_CH1N );
    }
    else if ( ( pHandle->pParams_str->LowSideOutputs ) == ES_GPIO )
    {
      LL_TIM_CC_DisableChannel( TIMx, LL_TIM_CHANNEL_CH1N );
    }
    else
    {
    }

    LL_TIM_OC_SetCompareCH1( TIMx, ( uint32_t )( pHandle->Half_PWMPeriod ) >> 1 );

    /*  Channel2 configuration */
    LL_TIM_OC_SetMode ( TIMx, LL_TIM_CHANNEL_CH2, LL_TIM_OCMODE_PWM1 );
    LL_TIM_CC_EnableChannel( TIMx, LL_TIM_CHANNEL_CH2 );

    if ( ( pHandle->pParams_str->LowSideOutputs ) == LS_PWM_TIMER )
    {
      LL_TIM_CC_EnableChannel( TIMx, LL_TIM_CHANNEL_CH2N );
    }
    else if ( ( pHandle->pParams_str->LowSideOutputs ) == ES_GPIO )
    {
      LL_TIM_CC_DisableChannel( TIMx, LL_TIM_CHANNEL_CH2N );
    }
    else
    {
    }

    LL_TIM_OC_SetCompareCH2( TIMx, ( uint32_t )( pHandle->Half_PWMPeriod ) >> 1 );

    /*  Channel3 configuration */
    LL_TIM_OC_SetMode ( TIMx, LL_TIM_CHANNEL_CH3, LL_TIM_OCMODE_PWM1 );
    LL_TIM_CC_EnableChannel( TIMx, LL_TIM_CHANNEL_CH3 );

    if ( ( pHandle->pParams_str->LowSideOutputs ) == LS_PWM_TIMER )
    {
      LL_TIM_CC_EnableChannel( TIMx, LL_TIM_CHANNEL_CH3N );
    }
    else if ( ( pHandle->pParams_str->LowSideOutputs ) == ES_GPIO )
    {
      LL_TIM_CC_DisableChannel( TIMx, LL_TIM_CHANNEL_CH3N );
    }
    else
    {
    }

    LL_TIM_OC_SetCompareCH3( TIMx, ( uint32_t )( pHandle->Half_PWMPeriod ) >> 1 );

    pHandle->_Super.pFctGetPhaseCurrents = &R3_2_GetPhaseCurrents;
    pHandle->_Super.pFctTurnOnLowSides = &R3_2_TurnOnLowSides;
    pHandle->_Super.pFctSwitchOnPwm = &R3_2_SwitchOnPWM;
    pHandle->_Super.pFctSwitchOffPwm = &R3_2_SwitchOffPWM;

    pHandle->_Super.RLDetectionMode = false;
  }
}

/**
 * @brief  It is used to set the PWM dutycycle for R/L detection.
 * @param  pHandle: handler of the current instance of the PWM component
 * @param  hDuty to be applied in uint16_t
 * @retval It returns the code error 'MC_FOC_DURATION' if any, 'MC_NO_ERROR'
 *         otherwise. These error codes are defined in mc_type.h
 */
uint16_t R3_2_RLDetectionModeSetDuty( PWMC_Handle_t * pHdl, uint16_t hDuty )
{
#if defined (__ICCARM__)
  #pragma cstat_disable = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  PWMC_R3_2_Handle_t * pHandle = ( PWMC_R3_2_Handle_t * )pHdl;
#if defined (__ICCARM__)
  #pragma cstat_restore = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  TIM_TypeDef * TIMx = pHandle->pParams_str->TIMx;
  uint32_t val;
  uint16_t hAux;


  val = ( ( uint32_t )( pHandle->Half_PWMPeriod ) * ( uint32_t )( hDuty ) ) >> 16;
  pHandle->_Super.CntPhA = ( uint16_t )( val );

  /* Set CC4 as PWM mode 2 (default) */
  LL_TIM_OC_SetMode(TIMx, LL_TIM_CHANNEL_CH4, LL_TIM_OCMODE_PWM2);

  LL_TIM_OC_SetCompareCH4(TIMx, ( uint32_t )( pHandle->Half_PWMPeriod - pHandle->_Super.Ton));
  LL_TIM_OC_SetCompareCH3(TIMx, ( uint32_t )pHandle->_Super.Toff);
  LL_TIM_OC_SetCompareCH1(TIMx, ( uint32_t )pHandle->_Super.CntPhA);

  /* Enabling next Trigger */
  //LL_TIM_CC_EnableChannel(TIMx, LL_TIM_CHANNEL_CH4);
  LL_TIM_SetTriggerOutput(TIMx, LL_TIM_TRGO_OC4REF);
  /* set the sector that correspond to Phase A and B sampling */
  pHdl->Sector = SECTOR_4;

  /* Limit for update event */
  /* Check the status flag. If an update event has occurred before to set new
  values of regs the FOC rate is too high */
  if (((TIMx->CR2) & TIM_CR2_MMS_Msk) != LL_TIM_TRGO_RESET )
  {
    hAux = MC_FOC_DURATION;
  }
  else
  {
    hAux = MC_NO_ERROR;
  }
  if ( pHandle->_Super.SWerror == 1u )
  {
    hAux = MC_FOC_DURATION;
    pHandle->_Super.SWerror = 0u;
  }
  return hAux;
}

#if defined (CCMRAM)
#if defined (__ICCARM__)
#pragma location = ".ccmram"
#elif defined (__CC_ARM) || defined(__GNUC__)
__attribute__( ( section ( ".ccmram" ) ) )
#endif
#endif
/**
  * @brief  It computes and return latest converted motor phase currents motor
  *         during RL detection phase
  * @param  pHandle: handler of the current instance of the PWM component
  * @retval Ia and Ib current in ab_t format
  */
static void R3_2_RLGetPhaseCurrents( PWMC_Handle_t * pHdl, ab_t * pStator_Currents )
{
#if defined (__ICCARM__)
  #pragma cstat_disable = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  PWMC_R3_2_Handle_t * pHandle = ( PWMC_R3_2_Handle_t * )pHdl;
#if defined (__ICCARM__)
  #pragma cstat_restore = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  TIM_TypeDef * TIMx = pHandle->pParams_str->TIMx;
  int32_t wAux;

  /* disable ADC trigger source */
  LL_TIM_SetTriggerOutput(TIMx, LL_TIM_TRGO_RESET);

  wAux = (int32_t)( pHandle->PhaseBOffset ) - (int32_t) *pHandle->pParams_str->ADCDataReg2[pHandle->_Super.Sector]; 
  
  /* Check saturation */
  if ( wAux > -INT16_MAX )
  {
    if ( wAux < INT16_MAX )
    {
    }
    else
    {
      wAux = INT16_MAX;
    }
  }
  else
  {
    wAux = -INT16_MAX;
  }

  pStator_Currents->a = (int16_t)wAux;
  pStator_Currents->b = (int16_t)wAux;
}

/**
  * @brief  It turns on low sides switches. This function is intended to be
  *         used for charging boot capacitors of driving section. It has to be
  *         called each motor start-up when using high voltage drivers.
  *         This function is specific for RL detection phase.
  * @param  pHandle: handler of the current instance of the PWM component
  * @retval none
  */
static void R3_2_RLTurnOnLowSides( PWMC_Handle_t * pHdl )
{
#if defined (__ICCARM__)
  #pragma cstat_disable = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  PWMC_R3_2_Handle_t * pHandle = ( PWMC_R3_2_Handle_t * )pHdl;
#if defined (__ICCARM__)
  #pragma cstat_restore = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  TIM_TypeDef * TIMx = pHandle->pParams_str->TIMx;

  /*Turn on the phase A low side switch */
  LL_TIM_OC_SetCompareCH1 ( TIMx, 0u );

  /* Clear Update Flag */
  LL_TIM_ClearFlag_UPDATE( TIMx );

  /* Wait until next update */
  while ( LL_TIM_IsActiveFlag_UPDATE( TIMx ) == 0 )
  {}

  /* Main PWM Output Enable */
  LL_TIM_EnableAllOutputs( TIMx );

  if ( ( pHandle->pParams_str->LowSideOutputs ) == ES_GPIO )
  {
    LL_GPIO_SetOutputPin( pHandle->pParams_str->pwm_en_u_port, pHandle->pParams_str->pwm_en_u_pin );
    LL_GPIO_ResetOutputPin( pHandle->pParams_str->pwm_en_v_port, pHandle->pParams_str->pwm_en_v_pin );
    LL_GPIO_ResetOutputPin( pHandle->pParams_str->pwm_en_w_port, pHandle->pParams_str->pwm_en_w_pin );
  }
  return;
}


/**
  * @brief  It enables PWM generation on the proper Timer peripheral
  *         This function is specific for RL detection phase.
  * @param  pHandle: handler of the current instance of the PWM component
  * @retval none
  */
static void R3_2_RLSwitchOnPWM( PWMC_Handle_t * pHdl )
{
#if defined (__ICCARM__)
  #pragma cstat_disable = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  PWMC_R3_2_Handle_t * pHandle = ( PWMC_R3_2_Handle_t * )pHdl;
#if defined (__ICCARM__)
  #pragma cstat_restore = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  TIM_TypeDef * TIMx = pHandle->pParams_str->TIMx;
  ADC_TypeDef * ADCx_1 = pHandle->pParams_str->ADCx_1;
  ADC_TypeDef * ADCx_2 = pHandle->pParams_str->ADCx_2;

  /* wait for a new PWM period */
  LL_TIM_ClearFlag_UPDATE( TIMx );
  while ( LL_TIM_IsActiveFlag_UPDATE( TIMx ) == 0 )
  {}
  /* Clear Update Flag */
  LL_TIM_ClearFlag_UPDATE( TIMx );

  LL_TIM_OC_SetCompareCH1( TIMx, 1u );
  LL_TIM_OC_SetCompareCH4( TIMx, ( pHandle->Half_PWMPeriod ) - 5u );

  while ( LL_TIM_IsActiveFlag_UPDATE( TIMx ) == 0 )
  {}

  /* enable TIMx update interrupt*/
  LL_TIM_EnableIT_UPDATE( TIMx );
  
  /* Main PWM Output Enable */
  TIMx->BDTR |= LL_TIM_OSSI_ENABLE ;
  LL_TIM_EnableAllOutputs( TIMx );

  if ( ( pHandle->pParams_str->LowSideOutputs ) == ES_GPIO )
  {
    if ( ( TIMx->CCER & TIMxCCER_MASK_CH123 ) != 0u )
    {
      LL_GPIO_SetOutputPin( pHandle->pParams_str->pwm_en_u_port, pHandle->pParams_str->pwm_en_u_pin );
      LL_GPIO_SetOutputPin( pHandle->pParams_str->pwm_en_v_port, pHandle->pParams_str->pwm_en_v_pin );
      LL_GPIO_ResetOutputPin( pHandle->pParams_str->pwm_en_w_port, pHandle->pParams_str->pwm_en_w_pin );
    }
    else
    {
      /* It is executed during calibration phase the EN signal shall stay off */
      LL_GPIO_ResetOutputPin( pHandle->pParams_str->pwm_en_u_port, pHandle->pParams_str->pwm_en_u_pin );
      LL_GPIO_ResetOutputPin( pHandle->pParams_str->pwm_en_v_port, pHandle->pParams_str->pwm_en_v_pin );
      LL_GPIO_ResetOutputPin( pHandle->pParams_str->pwm_en_w_port, pHandle->pParams_str->pwm_en_w_pin );
    }
  }

  /* set the sector that correspond to Phase B and C sampling
   * B will be sampled by ADCx_1 */
  pHdl->Sector = SECTOR_4;

  LL_ADC_INJ_StartConversion( ADCx_1 );
  LL_ADC_INJ_StartConversion( ADCx_2 );

  return;
}

/**
 * @brief  It turns on low sides switches and start ADC triggering.
 *         This function is specific for MP phase.
 * @param  pHandle Pointer on the target component instance
 * @retval none
 */
void RLTurnOnLowSidesAndStart( PWMC_Handle_t * pHdl )
{
#if defined (__ICCARM__)
  #pragma cstat_disable = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  PWMC_R3_2_Handle_t * pHandle = ( PWMC_R3_2_Handle_t * )pHdl;
#if defined (__ICCARM__)
  #pragma cstat_restore = "MISRAC2012-Rule-11.3"
#endif /* __ICCARM__ */
  TIM_TypeDef * TIMx = pHandle->pParams_str->TIMx;

  LL_TIM_ClearFlag_UPDATE( TIMx );
  while ( LL_TIM_IsActiveFlag_UPDATE( TIMx ) == 0 )
  {}
  /* Clear Update Flag */
  LL_TIM_ClearFlag_UPDATE( TIMx );

  LL_TIM_OC_SetCompareCH1( TIMx, 0x0u );
  LL_TIM_OC_SetCompareCH2( TIMx, 0x0u );
  LL_TIM_OC_SetCompareCH3( TIMx, 0x0u );
  LL_TIM_OC_SetCompareCH4( TIMx, ( pHandle->Half_PWMPeriod - 5u));

  while ( LL_TIM_IsActiveFlag_UPDATE( TIMx ) == 0 )
  {}

  /* Main PWM Output Enable */
  TIMx->BDTR |= LL_TIM_OSSI_ENABLE ;
  LL_TIM_EnableAllOutputs ( TIMx );

  if ( ( pHandle->pParams_str->LowSideOutputs ) == ES_GPIO )
  {
      /* It is executed during calibration phase the EN signal shall stay off */
      LL_GPIO_SetOutputPin( pHandle->pParams_str->pwm_en_u_port, pHandle->pParams_str->pwm_en_u_pin );
      LL_GPIO_SetOutputPin( pHandle->pParams_str->pwm_en_v_port, pHandle->pParams_str->pwm_en_v_pin );
      LL_GPIO_SetOutputPin( pHandle->pParams_str->pwm_en_w_port, pHandle->pParams_str->pwm_en_w_pin );
  }

  pHdl->Sector = SECTOR_4;

  LL_TIM_CC_EnableChannel(TIMx, LL_TIM_CHANNEL_CH4);

  LL_TIM_EnableIT_UPDATE( TIMx );

  return;
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
