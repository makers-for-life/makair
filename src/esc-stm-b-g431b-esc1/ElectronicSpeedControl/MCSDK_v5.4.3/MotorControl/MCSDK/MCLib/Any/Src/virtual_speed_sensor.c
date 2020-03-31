/**
  ******************************************************************************
  * @file    virtual_speed_sensor.c
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file provides firmware functions that implement the features
  *          of the Virtual Speed Sensor component of the Motor Control SDK.
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
#include "virtual_speed_sensor.h"


/** @addtogroup MCSDK
  * @{
  */

/** @addtogroup SpeednPosFdbk
  * @{
  */

/** @defgroup VirtualSpeedSensor Virtual Speed & Position Feedback
  * @brief Virtual Speed Speed & Position Feedback implementation
  *
  * This component provides a "virtual" implementation of the speed and position feedback features.
  * This implementation provides a theoretical estimation of the speed and position of the rotor of
  * the motor based on a mechanical acceleration and an initial angle set by the application.
  *
  * This component is used during the revup phases of the motor in a sensorless subsystem.
  *
  * @todo Document the Virtual Speed Sensor "module".
  *
  * @{
  */

/**
* @brief  Software initialization of VirtualSpeedSensor component
* @param  pHandle: handler of the current instance of the VirtualSpeedSensor component
* @retval none
*/
__weak void VSS_Init( VirtualSpeedSensor_Handle_t * pHandle )
{
#ifdef FASTDIV
  FD_Init( &( pHandle->fd ) );
#endif

  VSS_Clear( pHandle );
}

/**
* @brief  Software initialization of VSS object to be performed at each restart
*         of the motor.
* @param  pHandle: handler of the current instance of the VirtualSpeedSensor component
* @retval none
*/
__weak void VSS_Clear( VirtualSpeedSensor_Handle_t * pHandle )
{

  pHandle->_Super.bSpeedErrorNumber = 0u;
  pHandle->_Super.hElAngle = 0;
  pHandle->_Super.hMecAngle = 0;
  pHandle->_Super.hAvrMecSpeedUnit = 0;
  pHandle->_Super.hElSpeedDpp = 0;
  pHandle->_Super.hMecAccelUnitP = 0;
  pHandle->_Super.bSpeedErrorNumber = 0u;

  pHandle->wElAccDppP32 = 0;
  pHandle->wElSpeedDpp32 = 0;
  pHandle->hRemainingStep = 0u;
  pHandle->hElAngleAccu = 0;

  pHandle->bTransitionStarted = false;
  pHandle->bTransitionEnded = false;
  pHandle->hTransitionRemainingSteps = pHandle->hTransitionSteps;
  pHandle->bTransitionLocked = false;

  pHandle->bCopyObserver = false;

#ifdef FASTDIV
  /* (Fast division optimization for cortex-M0 micros)*/
  /* Dummy division to speed up next executions */
  FD_FastDiv( &( pHandle->fd ), 1, ( int32_t )( pHandle->_Super.bElToMecRatio ) );
  FD_FastDiv( &( pHandle->fd ), 1, ( int32_t )( pHandle->hTransitionSteps ) );
#endif
}

#if defined (CCMRAM)
#if defined (__ICCARM__)
#pragma location = ".ccmram"
#elif defined (__CC_ARM) || defined(__GNUC__)
__attribute__( ( section ( ".ccmram" ) ) )
#endif
#endif
/**
* @brief  Update the rotor electrical angle integrating the last setled
*         instantaneous electrical speed express in dpp.
* @param  pHandle: handler of the current instance of the VirtualSpeedSensor component
* @retval int16_t Measured electrical angle in s16degree format.
*/
__weak int16_t VSS_CalcElAngle( VirtualSpeedSensor_Handle_t * pHandle, void * pInputVars_str )
{

  int16_t hRetAngle;
  int16_t hAngleDiff;
  int16_t hAngleCorr;
  int32_t wAux;
  int16_t hSignCorr = 1;

  if ( pHandle->bCopyObserver == true )
  {
    hRetAngle = *( int16_t * )pInputVars_str;
  }
  else
  {
    pHandle->hElAngleAccu += pHandle->_Super.hElSpeedDpp;

#ifdef FASTDIV
    pHandle->_Super.hMecAngle += ( int16_t )( FD_FastDiv( &( pHandle->fd ),
                                 ( int32_t )pHandle->_Super.hElSpeedDpp,
                                 ( int32_t )pHandle->_Super.bElToMecRatio ) );
#else
    pHandle->_Super.hMecAngle += pHandle->_Super.hElSpeedDpp /
                                 ( int16_t )pHandle->_Super.bElToMecRatio;
#endif

    if ( pHandle->bTransitionStarted == true )
    {
      if ( pHandle->hTransitionRemainingSteps == 0 )
      {
        hRetAngle = *( int16_t * )pInputVars_str;
        pHandle->bTransitionEnded = true;
        pHandle->_Super.bSpeedErrorNumber = 0u;
      }
      else
      {
        pHandle->hTransitionRemainingSteps--;

        if ( pHandle->_Super.hElSpeedDpp >= 0 )
        {
          hAngleDiff = *( int16_t * )pInputVars_str - pHandle->hElAngleAccu;
        }
        else
        {
          hAngleDiff = pHandle->hElAngleAccu - *( int16_t * )pInputVars_str;
          hSignCorr = -1;
        }

        wAux = ( int32_t )hAngleDiff * pHandle->hTransitionRemainingSteps;

#ifdef FASTDIV
        hAngleCorr = ( int16_t )( FD_FastDiv( &( pHandle->fd ),
                                              wAux,
                                              ( int32_t )( pHandle->hTransitionSteps ) ) );
#else
        hAngleCorr = ( int16_t )( wAux / pHandle->hTransitionSteps );
#endif

        hAngleCorr *= hSignCorr;

        if ( hAngleDiff >= 0 )
        {
          pHandle->bTransitionLocked = true;
          hRetAngle = *( int16_t * )pInputVars_str - hAngleCorr;
        }
        else
        {
          if ( pHandle->bTransitionLocked == false )
          {
            hRetAngle = pHandle->hElAngleAccu;
          }
          else
          {
            hRetAngle = *( int16_t * )pInputVars_str + hAngleCorr;
          }
        }
      }
    }
    else
    {
      hRetAngle = pHandle->hElAngleAccu;
    }
  }

  pHandle->_Super.hElAngle = hRetAngle;
  return hRetAngle;
}

/**
  * @brief  This method must be called with the same periodicity
  *         on which speed control is executed.
  *         This method computes and stores rotor instantaneous el speed (express
  *         in dpp considering the measurement frequency) in order to provide it
  *         to SPD_CalcElAngle function and SPD_GetElAngle.
  *         Then compute store and return - through parameter
  *         hMecSpeedUnit - the rotor average mech speed, expressed in the unit
  *         defined by #SPEED_UNIT. Then return the reliability state of the
  *         sensor (always true).
  * @param  pHandle: handler of the current instance of the VirtualSpeedSensor component
  * @param  hMecSpeedUnit pointer to int16_t, used to return the rotor average
  *         mechanical speed (SPED_UNIT)
  * @retval true = sensor information is reliable
  *         false = sensor information is not reliable
  */
__weak bool VSS_CalcAvrgMecSpeedUnit( VirtualSpeedSensor_Handle_t * pHandle, int16_t * hMecSpeedUnit )
{
  bool SpeedSensorReliability = false;

  if ( pHandle->hRemainingStep > 1u )
  {
    pHandle->wElSpeedDpp32 += pHandle->wElAccDppP32;
    pHandle->_Super.hElSpeedDpp = ( int16_t )( pHandle->wElSpeedDpp32 / 65536 );

    /* Convert dpp into MecUnit */
    *hMecSpeedUnit = ( int16_t )( ( ( int32_t )pHandle->_Super.hElSpeedDpp *
                                    ( int32_t )pHandle->_Super.hMeasurementFrequency * SPEED_UNIT ) /
                                  ( ( int32_t )pHandle->_Super.DPPConvFactor * ( int32_t )pHandle->_Super.bElToMecRatio ) );

    pHandle->_Super.hAvrMecSpeedUnit = *hMecSpeedUnit;

    pHandle->hRemainingStep--;
  }
  else if ( pHandle->hRemainingStep == 1u )
  {
    *hMecSpeedUnit = pHandle->hFinalMecSpeedUnit;

    pHandle->_Super.hAvrMecSpeedUnit = *hMecSpeedUnit;

    pHandle->_Super.hElSpeedDpp = ( int16_t )( ( ( int32_t )( *hMecSpeedUnit ) *
                                  ( int32_t ) ( pHandle->_Super.DPPConvFactor) ) /
                                  ( ( int32_t )SPEED_UNIT * ( int32_t )pHandle->_Super.hMeasurementFrequency ) );

    pHandle->_Super.hElSpeedDpp *= ( int16_t )( pHandle->_Super.bElToMecRatio );

    pHandle->hRemainingStep = 0u;
  }
  else
  {
    *hMecSpeedUnit = pHandle->_Super.hAvrMecSpeedUnit;
  }
  /* If the transition is not done yet, we already know that speed is not reliable */
  if ( pHandle->bTransitionEnded == false )
  {
    pHandle->_Super.bSpeedErrorNumber = pHandle->_Super.bMaximumSpeedErrorsNumber;
    SpeedSensorReliability = false;
  }
  else
  {
    SpeedSensorReliability = SPD_IsMecSpeedReliable ( &pHandle->_Super, hMecSpeedUnit );
  }

  return ( SpeedSensorReliability );
}

/**
  * @brief  It is used to set istantaneous information on VSS mechanical and
  *         electrical angle.
  * @param  pHandle: handler of the current instance of the VirtualSpeedSensor component
  * @param  hMecAngle istantaneous measure of rotor mechanical angle
  * @retval none
  */
__weak void VSS_SetMecAngle( VirtualSpeedSensor_Handle_t * pHandle, int16_t hMecAngle )
{

  pHandle->hElAngleAccu = hMecAngle;
  pHandle->_Super.hMecAngle = pHandle->hElAngleAccu / ( int16_t )pHandle->_Super.bElToMecRatio;
  pHandle->_Super.hElAngle = hMecAngle;
}

/**
  * @brief  Set the mechanical acceleration of virtual sensor. This acceleration
            is defined starting from current mechanical speed, final mechanical
            speed expressed in 0.1Hz and duration expressed in milliseconds.
  * @param  pHandle: handler of the current instance of the VirtualSpeedSensor component
  * @param  hFinalMecSpeedUnit mechanical speed  assumed by
            the virtual sensor at the end of the duration. Expressed in the unit defined
            by #SPEED_UNIT.
  * @param  hDurationms Duration expressed in ms. It can be 0 to apply
            instantaneous the final speed.
  * @retval none
  */
__weak void  VSS_SetMecAcceleration( VirtualSpeedSensor_Handle_t * pHandle, int16_t  hFinalMecSpeedUnit,
                              uint16_t hDurationms )
{

  uint16_t hNbrStep;
  int16_t hCurrentMecSpeedDpp;
  int32_t wMecAccDppP32;
  int16_t hFinalMecSpeedDpp;

  if ( pHandle->bTransitionStarted == false )
  {
    if ( hDurationms == 0u )
    {
      pHandle->_Super.hAvrMecSpeedUnit = hFinalMecSpeedUnit;

      pHandle->_Super.hElSpeedDpp = ( int16_t )( ( ( int32_t )( hFinalMecSpeedUnit ) *
                                    ( int32_t )( pHandle->_Super.DPPConvFactor) ) /
                                    ( ( int32_t )SPEED_UNIT * ( int32_t )pHandle->_Super.hMeasurementFrequency ) );

      pHandle->_Super.hElSpeedDpp *= ( int16_t )( pHandle->_Super.bElToMecRatio );

      pHandle->hRemainingStep = 0u;

      pHandle->hFinalMecSpeedUnit = hFinalMecSpeedUnit;
    }
    else
    {
      hNbrStep = ( uint16_t )( ( ( uint32_t )hDurationms *
                                 ( uint32_t )pHandle->hSpeedSamplingFreqHz ) /
                               1000u );

      hNbrStep++;

      pHandle->hRemainingStep = hNbrStep;

      hCurrentMecSpeedDpp = pHandle->_Super.hElSpeedDpp /
                            ( int16_t )pHandle->_Super.bElToMecRatio;

      hFinalMecSpeedDpp = ( int16_t )( ( ( int32_t )hFinalMecSpeedUnit * ( int32_t )( pHandle->_Super.DPPConvFactor) ) /
                                       ( ( int32_t )SPEED_UNIT * ( int32_t )pHandle->_Super.hMeasurementFrequency ) );

      wMecAccDppP32 = ( ( ( int32_t )hFinalMecSpeedDpp - ( int32_t )hCurrentMecSpeedDpp ) *
                        ( int32_t )65536 ) / ( int32_t )hNbrStep;

      pHandle->wElAccDppP32 = wMecAccDppP32 * ( int16_t )pHandle->_Super.bElToMecRatio;

      pHandle->hFinalMecSpeedUnit = hFinalMecSpeedUnit;

      pHandle->wElSpeedDpp32 = ( int32_t )pHandle->_Super.hElSpeedDpp * ( int32_t )65536;
    }
  }
}

/**
  * @brief  Checks if the ramp executed after a VSPD_SetMecAcceleration command
  *         has been completed.
  * @param  pHandle: handler of the current instance of the VirtualSpeedSensor component
  * @retval bool true if the ramp is completed, otherwise false.
  */
__weak bool VSS_RampCompleted( VirtualSpeedSensor_Handle_t * pHandle )
{
  bool retVal = false;
  if ( pHandle->hRemainingStep == 0u )
  {
    retVal = true;
  }
  return retVal;
}

/**
  * @brief  Get the final speed of last setled ramp of virtual sensor expressed
            in 0.1Hz.
  * @param  pHandle: handler of the current instance of the VirtualSpeedSensor component
  * @retval none
  */
__weak int16_t  VSS_GetLastRampFinalSpeed( VirtualSpeedSensor_Handle_t * pHandle )
{
  return pHandle->hFinalMecSpeedUnit;
}

/**
  * @brief  Set the command to Start the transition phase from VirtualSpeedSensor
            to other SpeedSensor.
            Transition is to be considered ended when Sensor information is
            declared 'Reliable' or if function returned value is false
  * @param  pHandle: handler of the current instance of the VirtualSpeedSensor component
  * @param  bool true to Start the transition phase, false has no effect
  * @retval bool true if Transition phase is enabled (started or not), false if
            transition has been triggered but it's actually disabled
            (parameter hTransitionSteps = 0)
  */
__weak bool VSS_SetStartTransition( VirtualSpeedSensor_Handle_t * pHandle, bool bCommand )
{
  bool bAux = true;
  if ( bCommand == true )
  {
    pHandle->bTransitionStarted = true;

    if ( pHandle->hTransitionSteps == 0 )
    {
      pHandle->bTransitionEnded = true;
      pHandle->_Super.bSpeedErrorNumber = 0u;
      bAux = false;
    }
  }
  return bAux;
}

/**
  * @brief  Return the status of the transition phase.
  * @param  pHandle: handler of the current instance of the VirtualSpeedSensor component
  * @retval bool true if Transition phase is ongoing, false otherwise.
  */
__weak bool VSS_IsTransitionOngoing( VirtualSpeedSensor_Handle_t * pHandle )
{
  uint16_t hTS = 0u, hTE = 0u, hAux;
  bool retVal = false;
  if ( pHandle->bTransitionStarted == true )
  {
    hTS = 1u;
  }
  if ( pHandle->bTransitionEnded == true )
  {
    hTE = 1u;
  }
  hAux = hTS ^ hTE;
  if ( hAux != 0u )
  {
    retVal = true;
  }
  return ( retVal );
}

__weak bool VSS_TransitionEnded( VirtualSpeedSensor_Handle_t * pHandle )
{
    return pHandle->bTransitionEnded;
}

/**
  * @brief  It set istantaneous information on rotor electrical angle copied by state observer;
  * @param  pHandle: handler of the current instance of the VirtualSpeedSensor component
  * @retval none
  */
__weak void VSS_SetCopyObserver( VirtualSpeedSensor_Handle_t * pHandle )
{
  pHandle->bCopyObserver = true;
}

/**
  * @brief  It  set istantaneous information on rotor electrical angle.
  * @param  pHandle: handler of the current instance of the VirtualSpeedSensor component
  * @param  hElAngle istantaneous measure of rotor electrical angle (s16degrees)
  * @retval none
  */
__weak void VSS_SetElAngle( VirtualSpeedSensor_Handle_t * pHandle, int16_t hElAngle )
{
  pHandle->hElAngleAccu = hElAngle;
  pHandle->_Super.hElAngle = hElAngle;
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
