/**
  ******************************************************************************
  * @file    encoder_speed_pos_fdbk.c
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file provides firmware functions that implement the following features
  *          of the Encoder component of the Motor Control SDK:
  *
  *           - computes & stores average mechanical speed
  *           - computes & stores average mechanical acceleration
  *           - computes & stores  the instantaneous electrical speed
  *           - calculates the rotor electrical and mechanical angle
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
#include "encoder_speed_pos_fdbk.h"

#include "mc_type.h"

/** @addtogroup MCSDK
  * @{
  */

/** @addtogroup SpeednPosFdbk
  * @{
  */

/** @defgroup Encoder Encoder Speed & Position Feedback
  * @brief Quadrature Encoder based Speed & Position Feedback implementation
  *
  * This component is used in applications controlling a motor equipped with a quadrature encoder.
  *
  * This component uses the output of a quadrature encoder to provide a measure of the speed and
  * the position of the rotor of the motor.
  *
  * @todo Document the Encoder Speed & Position Feedback "module".
  *
  * @{
  */

/* Private defines -----------------------------------------------------------*/


/**
  * @brief  It initializes the hardware peripherals (TIMx, GPIO and NVIC)
            required for the speed position sensor management using ENCODER
            sensors.
  * @param  pHandle: handler of the current instance of the encoder component
  * @retval none
  */
__weak void ENC_Init( ENCODER_Handle_t * pHandle )
{

  TIM_TypeDef * TIMx = pHandle->TIMx;
  uint8_t BufferSize;
  uint8_t Index;


#ifdef TIM_CNT_UIFCPY
  LL_TIM_EnableUIFRemap ( TIMx );
#define ENC_MAX_OVERFLOW_NB     ((uint16_t)2048) /* 2^11*/
#else
#define ENC_MAX_OVERFLOW_NB     (1)
#endif
  /* Reset counter */
  LL_TIM_SetCounter ( TIMx, 0 );

  /*Calculations of convenience*/
  pHandle->U32MAXdivPulseNumber = UINT32_MAX / ( uint32_t )( pHandle->PulseNumber );
  pHandle->SpeedSamplingFreqUnit = pHandle->SpeedSamplingFreqHz * SPEED_UNIT;

  /* Set IC filter for both channel 1 & 2*/
  LL_TIM_IC_SetFilter(TIMx, LL_TIM_CHANNEL_CH1, ( uint32_t )(pHandle->ICx_Filter << 20));
  LL_TIM_IC_SetFilter(TIMx, LL_TIM_CHANNEL_CH2, ( uint32_t )(pHandle->ICx_Filter << 20));

  LL_TIM_ClearFlag_UPDATE ( TIMx );
  LL_TIM_EnableIT_UPDATE ( TIMx );

  /* Enable the counting timer*/
  LL_TIM_EnableCounter ( TIMx );

  /* Erase speed buffer */
  BufferSize = pHandle->SpeedBufferSize;

  for ( Index = 0u; Index < BufferSize; Index++ )
  {
    pHandle->DeltaCapturesBuffer[Index] = 0;
  }
}

/**
* @brief  Clear software FIFO where are "pushed" rotor angle variations captured
*         This function must be called before starting the motor to initialize
*         the speed measurement process.
* @param  pHandle: handler of the current instance of the encoder component
* @retval none
*/
__weak void ENC_Clear( ENCODER_Handle_t * pHandle )
{
  uint8_t Index;
  for ( Index = 0u; Index < pHandle->SpeedBufferSize; Index++ )
  {
    pHandle->DeltaCapturesBuffer[Index] = 0;
  }
  pHandle->SensorIsReliable = true;
}

/**
* @brief  It calculates the rotor electrical and mechanical angle, on the basis
*         of the instantaneous value of the timer counter.
* @param  pHandle: handler of the current instance of the encoder component
* @retval int16_t Measured electrical angle in s16degree format.
*/
__weak int16_t ENC_CalcAngle( ENCODER_Handle_t * pHandle )
{
  int32_t wtemp1;
  int16_t elAngle;  /* s16degree format */
  int16_t mecAngle; /* s16degree format */
  /* PR 52926 We need to keep only the 16 LSB, bit 31 could be at 1 
   if the overflow occurs just after the entry in the High frequency task */
  wtemp1 = ( int32_t )( LL_TIM_GetCounter( pHandle->TIMx ) & 0xffff ) *
           ( int32_t )( pHandle->U32MAXdivPulseNumber );

  /*Computes and stores the rotor mechanical angle*/
  mecAngle = ( int16_t )( wtemp1 / 65536 );

  int16_t hMecAnglePrev = pHandle->_Super.hMecAngle;

  pHandle->_Super.hMecAngle = mecAngle;
  
  /*Computes and stores the rotor electrical angle*/
  elAngle = mecAngle * pHandle->_Super.bElToMecRatio;

  pHandle->_Super.hElAngle = elAngle;
  
  int16_t hMecSpeedDpp = mecAngle - hMecAnglePrev;
  pHandle->_Super.wMecAngle += (int32_t)(hMecSpeedDpp);

  /*Returns rotor electrical angle*/
  return ( elAngle );
}

/**
  * @brief  This method must be called with the periodicity defined by parameter
  *         hSpeedSamplingFreqUnit. The method generates a capture event on
  *         a channel, computes & stores average mechanical speed [expressed in the unit
  *         defined by #SPEED_UNIT] (on the basis of the buffer filled by CCx IRQ),
  *         computes & stores average mechanical acceleration [#SPEED_UNIT/SpeedSamplingFreq],
  *         computes & stores the instantaneous electrical speed [dpp], updates the index of the
  *         speed buffer, then checks & stores & returns the reliability state
  *         of the sensor.
  * @param  pHandle: handler of the current instance of the encoder component
  * @param  pMecSpeedUnit pointer used to return the rotor average mechanical speed
  *         (expressed in the unit defined by #SPEED_UNIT)
  * @retval true = sensor information is reliable
  *         false = sensor information is not reliable
  */
__weak bool ENC_CalcAvrgMecSpeedUnit( ENCODER_Handle_t * pHandle, int16_t * pMecSpeedUnit )
{
  TIM_TypeDef * TIMx = pHandle->TIMx;
  int32_t wOverallAngleVariation = 0;
  int32_t wtemp1;
  int32_t wtemp2;
  uint8_t bBufferIndex = 0u;
  bool bReliability = true;
  uint8_t bBufferSize = pHandle->SpeedBufferSize;
  uint32_t OverflowCntSample;
  uint32_t CntCapture;
  uint32_t directionSample;
  uint8_t OFbit = 0;

#ifdef TIM_CNT_UIFCPY
  /* disable Interrupt generation */
  LL_TIM_DisableIT_UPDATE ( TIMx );
#endif
  CntCapture =  LL_TIM_GetCounter ( TIMx );
  OverflowCntSample = pHandle->TimerOverflowNb;
  pHandle->TimerOverflowNb = 0;
  directionSample =  LL_TIM_GetDirection( TIMx );
#ifdef TIM_CNT_UIFCPY
  OFbit = __LL_TIM_GETFLAG_UIFCPY( CntCapture );
  if ( OFbit )
  {
    /* If OFbit is set, overflow has occured since IT has been disabled.
    We have to take this overflow into account in the angle computation,
    but we must not take it into account a second time in the accmulator,
    so we have to clear the pending flag. If the OFbit is not set, it does not mean
    that an Interrupt has not occured since the last read, but it has not been taken
    into accout, we must not clear the interrupt in order to accumulate it */
    LL_TIM_ClearFlag_UPDATE( TIMx );
  }
  LL_TIM_EnableIT_UPDATE ( TIMx );
  CLEAR_BIT( CntCapture, TIM_CNT_UIFCPY );
#endif
  /* If UIFCPY is not present, OverflowCntSample can not be used safely for
  speed computation, but we still use it to check that we do not exceed one overflow
  (sample frequency not less than mechanical motor speed */
  if ( ( OverflowCntSample + OFbit ) > ENC_MAX_OVERFLOW_NB )
  {
    pHandle->TimerOverflowError = true;
  }

  /*Calculation of delta angle*/
  if ( directionSample == LL_TIM_COUNTERDIRECTION_DOWN )
  {
    /* encoder timer down-counting*/
    /* if UIFCPY not present Overflow counter can not be safely used -> limitation to 1 OF. */
#ifndef TIM_CNT_UIFCPY
    OverflowCntSample = ( CntCapture > pHandle->PreviousCapture ) ? 1 : 0;
#endif
    pHandle->DeltaCapturesBuffer[pHandle->DeltaCapturesIndex] =
      ( int32_t )( CntCapture ) - ( int32_t )( pHandle->PreviousCapture ) -
      ( ( int32_t )( OverflowCntSample ) + OFbit ) * ( int32_t )( pHandle->PulseNumber );
  }
  else
  {
    /* encoder timer up-counting*/
    /* if UIFCPY not present Overflow counter can not be safely used -> limitation to 1 OF. */
#ifndef TIM_CNT_UIFCPY
    OverflowCntSample = ( CntCapture < pHandle->PreviousCapture ) ? 1 : 0;
#endif
    pHandle->DeltaCapturesBuffer[pHandle->DeltaCapturesIndex] =
      ( int32_t )( CntCapture ) - ( int32_t )( pHandle->PreviousCapture ) +
      ( ( int32_t )( OverflowCntSample ) + OFbit ) * ( int32_t )( pHandle->PulseNumber );
  }


  /*Computes & returns average mechanical speed */
  for ( bBufferIndex = 0u; bBufferIndex < bBufferSize; bBufferIndex++ )
  {
    wOverallAngleVariation += pHandle->DeltaCapturesBuffer[bBufferIndex];
  }
  wtemp1 = wOverallAngleVariation * ( int32_t )( pHandle->SpeedSamplingFreqUnit );
  wtemp2 = ( int32_t )( pHandle->PulseNumber ) *
           ( int32_t )( pHandle->SpeedBufferSize );
  wtemp1 /= wtemp2;
  *pMecSpeedUnit = ( int16_t )( wtemp1 );

  /*Computes & stores average mechanical acceleration */
  pHandle->_Super.hMecAccelUnitP = ( int16_t )( wtemp1 -
                                   pHandle->_Super.hAvrMecSpeedUnit );

  /*Stores average mechanical speed */
  pHandle->_Super.hAvrMecSpeedUnit = ( int16_t )wtemp1;

  /*Computes & stores the instantaneous electrical speed [dpp], var wtemp1*/
  wtemp1 = pHandle->DeltaCapturesBuffer[pHandle->DeltaCapturesIndex] *
           ( int32_t )( pHandle->SpeedSamplingFreqHz ) *
           ( int32_t )pHandle->_Super.bElToMecRatio;
  wtemp1 /= ( int32_t )( pHandle->PulseNumber );
  wtemp1 *= ( int32_t )( pHandle->_Super.DPPConvFactor);
  wtemp1 /= ( int32_t )( pHandle->_Super.hMeasurementFrequency );

  pHandle->_Super.hElSpeedDpp = ( int16_t )wtemp1;

  /*last captured value update*/
  pHandle->PreviousCapture = CntCapture;
  /*Buffer index update*/
  pHandle->DeltaCapturesIndex++;

  if ( pHandle->DeltaCapturesIndex == pHandle->SpeedBufferSize )
  {
    pHandle->DeltaCapturesIndex = 0u;
  }

  /*Checks the reliability status, then stores and returns it*/
  if ( pHandle->TimerOverflowError )
  {
    bReliability = false;
    pHandle->SensorIsReliable = false;
    pHandle->_Super.bSpeedErrorNumber = pHandle->_Super.bMaximumSpeedErrorsNumber;

  }
  else
  {
    bReliability = SPD_IsMecSpeedReliable( &pHandle->_Super, pMecSpeedUnit );
  }

  return ( bReliability );
}

/**
  * @brief  It set instantaneous rotor mechanical angle.
  *         As a consequence, timer counted is computed and updated.
  * @param  pHandle: handler of the current instance of the encoder component
  * @param  hMecAngle new value of rotor mechanical angle (s16degrees)
  * @retval none
  */
__weak void ENC_SetMecAngle( ENCODER_Handle_t * pHandle, int16_t hMecAngle )
{
  TIM_TypeDef * TIMx = pHandle->TIMx;

  uint16_t hAngleCounts;
  uint16_t hMecAngleuint;

  pHandle->_Super.hMecAngle = hMecAngle;
  pHandle->_Super.hElAngle = hMecAngle * pHandle->_Super.bElToMecRatio;
  if ( hMecAngle < 0 )
  {
    hMecAngle *= -1;
    hMecAngleuint = 65535u - ( uint16_t )hMecAngle;
  }
  else
  {
    hMecAngleuint = ( uint16_t )hMecAngle;
  }

  hAngleCounts = ( uint16_t )( ( ( uint32_t )hMecAngleuint *
                                 ( uint32_t )pHandle->PulseNumber ) / 65535u );

  TIMx->CNT = ( uint16_t )( hAngleCounts );
  
}

/**
  * @brief  IRQ implementation of the TIMER ENCODER
  * @param  pHandle: handler of the current instance of the encoder component
  * @param  flag used to distinguish between various IRQ sources
  * @retval none
  */
__weak void * ENC_IRQHandler( void * pHandleVoid )
{
  ENCODER_Handle_t * pHandle = ( ENCODER_Handle_t * ) pHandleVoid;

  /*Updates the number of overflows occurred*/
  /* the handling of overflow error is done in ENC_CalcAvrgMecSpeedUnit */
  pHandle->TimerOverflowNb += 1u;

  return MC_NULL;
}
/**
  * @}
  */

/**
  * @}
  */

/** @} */


/******************* (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
