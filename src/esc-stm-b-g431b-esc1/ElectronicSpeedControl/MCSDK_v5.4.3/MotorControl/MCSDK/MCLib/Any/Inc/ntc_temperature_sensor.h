/**
  ******************************************************************************
  * @file    ntc_temperature_sensor.h
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file contains all definitions and functions prototypes for the
  *          Temperature Sensor component of the Motor Control SDK.
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
  * @ingroup TemperatureSensor
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TEMPERATURESENSOR_H
#define __TEMPERATURESENSOR_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "mc_type.h"
#include "regular_conversion_manager.h"

/** @addtogroup MCSDK
  * @{
  */

/** @addtogroup TemperatureSensor
  * @{
  */


/**
  * @brief NTC_Handle_t structure used for temperature monitoring
  *
  */
typedef struct
{
  SensorType_t  bSensorType;   /**< Type of instanced temperature.
                                    This parameter can be REAL_SENSOR or VIRTUAL_SENSOR */

  RegConv_t      TempRegConv;                                     

  uint16_t hAvTemp_d;          /**< It contains latest available average Vbus.
                                    This parameter is expressed in u16Celsius */

  uint16_t hExpectedTemp_d;    /**< Default set when no sensor available (ie virtual sensor) */

  uint16_t hExpectedTemp_C;    /**< Default value when no sensor available (ie virtual sensor).
                                    This parameter is expressed in Celsius */

  uint16_t hFaultState;        /**< Contains latest Fault code.
                                    This parameter is set to MC_OVER_TEMP or MC_NO_ERROR */

  uint16_t hLowPassFilterBW;   /**< used to configure the first order software filter bandwidth.
                                    hLowPassFilterBW = NTC_CalcBusReading
                                    call rate [Hz]/ FilterBandwidth[Hz] */
  uint16_t hOverTempThreshold; /**< Represents the over voltage protection intervention threshold.
                                    This parameter is expressed in u16Celsius through formula:
                                    hOverTempThreshold =
                                    (V0[V]+dV/dT[V/°C]*(OverTempThreshold[°C] - T0[°C]))* 65536 / MCU supply voltage */
  uint16_t hOverTempDeactThreshold; /**< Temperature threshold below which an active over temperature fault is cleared.
                                         This parameter is expressed in u16Celsius through formula:
                                         hOverTempDeactThreshold =
                                         (V0[V]+dV/dT[V/°C]*(OverTempDeactThresh[°C] - T0[°C]))* 65536 / MCU supply voltage*/
  int16_t hSensitivity;        /**< NTC sensitivity
                                    This parameter is equal to MCU supply voltage [V] / dV/dT [V/°C] */
  uint32_t wV0;                /**< V0 voltage constant value used to convert the temperature into Volts.
                                    This parameter is equal V0*65536/MCU supply
                                    Used in through formula: V[V]=V0+dV/dT[V/°C]*(T-T0)[°C] */
  uint16_t hT0;                /**< T0 temperature constant value used to convert the temperature into Volts
                                    Used in through formula: V[V]=V0+dV/dT[V/°C]*(T-T0)[°C] */
  uint8_t convHandle;            /*!< handle to the regular conversion */                         

} NTC_Handle_t;

/* Initialize temperature sensing parameters */
void NTC_Init( NTC_Handle_t * pHandle );

/* Clear static average temperature value */
void NTC_Clear( NTC_Handle_t * pHandle );

/* Temperature sensing computation */
uint16_t NTC_CalcAvTemp( NTC_Handle_t * pHandle );

/* Get averaged temperature measurement expressed in u16Celsius */
uint16_t NTC_GetAvTemp_d( NTC_Handle_t * pHandle );

/* Get averaged temperature measurement expressed in Celsius degrees */
int16_t NTC_GetAvTemp_C( NTC_Handle_t * pHandle );


/* Get the temperature measurement fault status */
uint16_t NTC_CheckTemp( NTC_Handle_t * pHandle );

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif /* __cpluplus */

#endif /* __TEMPERATURESENSOR_H */

/************************ (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
