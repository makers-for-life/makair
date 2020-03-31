
/**
  ******************************************************************************
  * @file    power_stage_parameters.h
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file contains the parameters needed for the Motor Control SDK
  *          in order to configure a power stage.
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
#ifndef __POWER_STAGE_PARAMETERS_H
#define __POWER_STAGE_PARAMETERS_H

/************************
 *** Motor Parameters ***
 ************************/

/************* PWM Driving signals section **************/

#define HW_DEAD_TIME_NS               500 /*!< Dead-time inserted 
                                                         by HW if low side signals 
                                                         are not used */
														 
/*********** Bus voltage sensing section ****************/
#define VBUS_PARTITIONING_FACTOR      0.0963 /*!< It expresses how 
                                                       much the Vbus is attenuated  
                                                       before being converted into 
                                                       digital value */
#define NOMINAL_BUS_VOLTAGE_V         27 
/******** Current reading parameters section ******/
/*** Topology ***/
#define THREE_SHUNT_INDEPENDENT_RESOURCES

#define RSHUNT                        0.0030 

/*  ICSs gains in case of isolated current sensors,
        amplification gain for shunts based sensing */
#define AMPLIFICATION_GAIN            9.14 

/*** Noise parameters ***/
#define TNOISE_NS                     1000
#define TRISE_NS                      2000 
#define MAX_TNTR_NS TRISE_NS
   
/************ Temperature sensing section ***************/
/* V[V]=V0+dV/dT[V/Celsius]*(T-T0)[Celsius]*/
#define V0_V                          1.055 /*!< in Volts */
#define T0_C                          25 /*!< in Celsius degrees */
#define dV_dT                         0.023 /*!< V/Celsius degrees */
#define T_MAX                         110 /*!< Sensor measured 
                                                       temperature at maximum 
                                                       power stage working 
                                                       temperature, Celsius degrees */
                                                       

#endif /*__POWER_STAGE_PARAMETERS_H*/
/******************* (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
