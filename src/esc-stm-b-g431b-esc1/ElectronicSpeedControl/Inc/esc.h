/**
  ******************************************************************************
  * @file    esc.h
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file contains all definitions and functions prototypes for the
  *          esc component of the Motor Control SDK.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  * @ingroup esc
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef ESC_H
#define ESC_H
 
#define ESC_FILTER_DEEP 4    
#define ESC_BEEP_FEATURE    
    
typedef enum
{
  ESC_ARMING       = 0x00,
  ESC_ARMED        = 0x01,
  ESC_POSITIVE_RUN = 0x02,
  ESC_STOP         = 0x03,
} ESC_sm_t;
  
typedef enum
{
  ESC_NOERROR = 0,
  ESC_NOSIGNAL = 1,
  ESC_PWM_BELOW_MIN = 2
} ESC_State_t;

typedef enum
{
  SM_BEEP_1  = 0x01,
  SM_BEEP_2  = 0x02,
  SM_BEEP_3  = 0x03,
  SM_BEEP_4  = 0x04,
} ESC_Beep_State;

typedef struct
{
  TIM_TypeDef * Command_TIM;
  TIM_TypeDef * Motor_TIM;
  uint32_t ARMING_TIME;
  uint32_t PWM_TURNOFF_MAX;
  uint32_t TURNOFF_TIME_MAX;
  uint32_t Ton_max;
  uint32_t Ton_min;
  uint32_t Ton_arming;
  uint32_t delta_Ton_max;     
  uint16_t speed_max_valueRPM;
  uint16_t speed_min_valueRPM;
  uint8_t motor;
} ESC_Params_t;
  
typedef struct
{
  ESC_Params_t const * pESC_params;
  uint32_t pwm_buffer[ESC_FILTER_DEEP];                 /*!<  PWM filter variable */
  uint32_t index_filter;
  uint32_t pwm_accumulator;
  uint32_t arming_counter;
  uint32_t pwm_timeout;
  int32_t turnoff_delay;
  volatile uint32_t Ton_value;
  int16_t restart_delay;
#ifdef ESC_BEEP_FEATURE
  uint16_t beep_stop_time;
  uint16_t beep_counter;
  ESC_Beep_State beep_state;
  uint8_t  beep_num;
  bool phase_check_status;
  bool start_check_flag;
#endif  
  ESC_sm_t sm_state;
  uint8_t  watchdog_counter;
  uint8_t  watchdog_counter_prev;
  bool buffer_completed;
} ESC_Handle_t;

void esc_boot(ESC_Handle_t * pHandle);

void esc_pwm_stop(ESC_Handle_t * pHandle);

ESC_State_t esc_pwm_run(ESC_Handle_t * pHandle);

void esc_pwm_control(ESC_Handle_t * pHandle);

#endif  /*ESC_H*/
