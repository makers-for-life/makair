/**
  ******************************************************************************
  * @file    r_divider_bus_voltage_sensor.h
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file contains all definitions and functions prototypes for the
  *          Resistor Divider Bus Voltage Sensor component of the Motor Control SDK.
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
  * @ingroup RDividerBusVoltageSensor
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RDIVIDER_BUSVOLTAGESENSOR_H
#define __RDIVIDER_BUSVOLTAGESENSOR_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "regular_conversion_manager.h"
#include "bus_voltage_sensor.h"

/** @addtogroup MCSDK
  * @{
  */

/** @addtogroup BusVoltageSensor
  * @{
  */

/** @addtogroup RDividerBusVoltageSensor
  * @{
  */


/**
  * @brief  Rdivider class parameters definition
  */
typedef struct
{
  BusVoltageSensor_Handle_t _Super;

  RegConv_t      VbusRegConv; 
  uint16_t       LowPassFilterBW;       /*!< Use this number to configure the Vbus
                                             first order software filter bandwidth.
                                             hLowPassFilterBW = VBS_CalcBusReading
                                             call rate [Hz]/ FilterBandwidth[Hz] */
  uint16_t       OverVoltageThreshold;  /*!< It represents the over voltage protection
                                             intervention threshold. To be expressed
                                             in digital value through formula:
                                             hOverVoltageThreshold (digital value) =
                                             Over Voltage Threshold (V) * 65536
                                             / hConversionFactor */
  uint16_t       UnderVoltageThreshold; /*!< It represents the under voltage protection
                                             intervention threshold. To be expressed
                                             in digital value through formula:
                                             hUnderVoltageThreshold (digital value)=
                                             Under Voltage Threshold (V) * 65536
                                             / hConversionFactor */
  uint16_t       *aBuffer;                /*!< Buffer used to compute average value.*/
  uint8_t        elem;                  /*!< Number of stored elements in the average buffer.*/
  uint8_t        index;                 /*!< Index of last stored element in the average buffer.*/
  uint8_t        convHandle;            /*!< handle to the regular conversion */

} RDivider_Handle_t;

/* Exported functions ------------------------------------------------------- */
void RVBS_Init( RDivider_Handle_t * pHandle );
void RVBS_Clear( RDivider_Handle_t * pHandle );
uint16_t RVBS_CalcAvVbusFilt( RDivider_Handle_t * pHandle );
uint16_t RVBS_CalcAvVbus( RDivider_Handle_t * pHandle );
uint16_t RVBS_CheckFaultState( RDivider_Handle_t * pHandle );

/**
  * @}
  */

/**
  * @}
  */

/** @} */

#ifdef __cplusplus
}
#endif /* __cpluplus */

#endif /* __RDividerBusVoltageSensor_H */

/************************ (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/

