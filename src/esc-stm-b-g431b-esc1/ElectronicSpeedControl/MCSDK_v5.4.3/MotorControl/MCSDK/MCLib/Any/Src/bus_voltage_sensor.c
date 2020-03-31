/**
  ******************************************************************************
  * @file    bus_voltage_sensor.c
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file provides firmware functions that implement the features
  *          of the BusVoltageSensor component of the Motor Control SDK.
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

#include "bus_voltage_sensor.h"


/** @addtogroup MCSDK
  * @{
  */

/** @defgroup BusVoltageSensor Bus Voltage Sensor
  * @brief Bus Voltage Sensor components of the Motor Control SDK
  *
  * Two Bus Voltage Sensor implementations are provided:
  *
  * - The @ref RDividerBusVoltageSensor "Resistor Divider Bus Voltage Sensor" operates as the name suggests
  * - The @ref VirtualBusVoltageSensor "Virtual Bus Voltage Sensor" does not make measurement but rather
  *   returns a fixed, application defined value.
  *
  * @todo Document the Bus Voltage Sensor "module".
  *
  * @{
  */

/**
  * @brief  It return latest Vbus conversion result expressed in u16Volt
  * @param  pHandle related Handle of BusVoltageSensor_Handle_t
  * @retval uint16_t Latest Vbus conversion result in digit
  */
__weak uint16_t VBS_GetBusVoltage_d( BusVoltageSensor_Handle_t * pHandle )
{
  return ( pHandle->LatestConv );
}

#if defined (CCMRAM)
#if defined (__ICCARM__)
#pragma location = ".ccmram"
#elif defined (__CC_ARM) || defined(__GNUC__)
__attribute__( ( section ( ".ccmram" ) ) )
#endif
#endif
/**
  * @brief  It return latest averaged Vbus measurement expressed in u16Volt
  * @param  pHandle related Handle of BusVoltageSensor_Handle_t
  * @retval uint16_t Latest averaged Vbus measurement in digit
  */
__weak uint16_t VBS_GetAvBusVoltage_d( BusVoltageSensor_Handle_t * pHandle )
{
  return ( pHandle->AvBusVoltage_d );
}

/**
  * @brief  It return latest averaged Vbus measurement expressed in Volts
  * @param  pHandle related Handle of BusVoltageSensor_Handle_t
  * @retval uint16_t Latest averaged Vbus measurement in Volts
  */
__weak uint16_t VBS_GetAvBusVoltage_V( BusVoltageSensor_Handle_t * pHandle )
{
  uint32_t temp;

  temp = ( uint32_t )( pHandle->AvBusVoltage_d );
  temp *= pHandle->ConversionFactor;
  temp /= 65536u;

  return ( ( uint16_t )temp );
}

/**
  * @brief  It returns MC_OVER_VOLT, MC_UNDER_VOLT or MC_NO_ERROR depending on
  *         bus voltage and protection threshold values
  * @param  pHandle related Handle of BusVoltageSensor_Handle_t
  * @retval uint16_t Fault code error
  */
__weak uint16_t VBS_CheckVbus( BusVoltageSensor_Handle_t * pHandle )
{
  return ( pHandle->FaultState );
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
