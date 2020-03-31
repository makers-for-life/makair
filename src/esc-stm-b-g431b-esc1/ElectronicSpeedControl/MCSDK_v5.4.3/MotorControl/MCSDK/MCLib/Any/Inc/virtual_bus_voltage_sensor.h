/**
  ******************************************************************************
  * @file    virtual_bus_voltage_sensor.h
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file contains all definitions and functions prototypes for the
  *          Virtual Bus Voltage Sensor component of the Motor Control SDK.
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
  * @ingroup VirtualBusVoltageSensor
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __VIRTUAL_BUSVOLTAGESENSOR_H
#define __VIRTUAL_BUSVOLTAGESENSOR_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "bus_voltage_sensor.h"

/** @addtogroup MCSDK
  * @{
  */

/** @addtogroup BusVoltageSensor
  * @{
  */

/** @addtogroup VirtualBusVoltageSensor
  * @{
  */

/**
  * @brief  Virtual Vbus sensor class parameters definition
  */
typedef struct
{
  BusVoltageSensor_Handle_t _Super;

  uint16_t ExpectedVbus_d;            /*!< Expected Vbus voltage expressed in
                                           digital value
                                           hOverVoltageThreshold(digital value)=
                                           Over Voltage Threshold (V) * 65536
                                           / 500 */
} VirtualBusVoltageSensor_Handle_t;

/* Exported functions ------------------------------------------------------- */
void VVBS_Init( VirtualBusVoltageSensor_Handle_t * pHandle );
void VVBS_Clear( VirtualBusVoltageSensor_Handle_t * pHandle );
uint16_t VVBS_NoErrors( VirtualBusVoltageSensor_Handle_t * pHandle );

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

#endif /* __CCC_H */

/************************ (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/

