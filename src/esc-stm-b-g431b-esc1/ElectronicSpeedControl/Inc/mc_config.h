/**
  ******************************************************************************
  * @file    mc_config.h 
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   Motor Control Subsystem components configuration and handler 
  *          structures declarations.
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
  
#ifndef __MC_CONFIG_H
#define __MC_CONFIG_H

#include "pid_regulator.h"
#include "revup_ctrl.h"
#include "speed_torq_ctrl.h"
#include "virtual_speed_sensor.h"
#include "ntc_temperature_sensor.h"
#include "pwm_curr_fdbk.h"
#include "r_divider_bus_voltage_sensor.h"
#include "virtual_bus_voltage_sensor.h"
#include "pqd_motor_power_measurement.h"
 #include "user_interface.h"

#include "motor_control_protocol.h"
#include "usart_frame_communication_protocol.h"

#include "dac_common_ui.h"
#include "dac_ui.h"

#include "r3_2_g4xx_pwm_curr_fdbk.h"

extern RevUpCtrl_Handle_t RevUpControlM1;
#include "ramp_ext_mngr.h"
#include "circle_limitation.h"

#include "sto_speed_pos_fdbk.h"
#include "sto_pll_speed_pos_fdbk.h"
/* USER CODE BEGIN Additional include */

#include "esc.h"

/* USER CODE END Additional include */  
extern PID_Handle_t PIDSpeedHandle_M1;
extern PID_Handle_t PIDIqHandle_M1;
extern PID_Handle_t PIDIdHandle_M1;
extern NTC_Handle_t TempSensorParamsM1;
extern PWMC_R3_2_Handle_t PWM_Handle_M1;
extern SpeednTorqCtrl_Handle_t SpeednTorqCtrlM1;
extern PQD_MotorPowMeas_Handle_t PQD_MotorPowMeasM1;
extern PQD_MotorPowMeas_Handle_t *pPQD_MotorPowMeasM1; 
extern VirtualSpeedSensor_Handle_t VirtualSpeedSensorM1;
extern STO_Handle_t STO_M1;
extern STO_PLL_Handle_t STO_PLL_M1;
extern RDivider_Handle_t RealBusVoltageSensorParamsM1;
extern CircleLimitation_Handle_t CircleLimitationM1;
extern RampExtMngr_Handle_t RampExtMngrHFParamsM1;
extern UI_Handle_t UI_Params;

extern DAC_UI_Handle_t DAC_UI_Params;

extern UFCP_Handle_t pUSART;

/* USER CODE BEGIN Additional extern */

extern ESC_Handle_t ESC_M1;

/* USER CODE END Additional extern */  
#define NBR_OF_MOTORS 1
#endif /* __MC_CONFIG_H */
/******************* (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
