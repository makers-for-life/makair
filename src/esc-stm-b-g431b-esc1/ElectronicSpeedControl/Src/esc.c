/**
  ******************************************************************************
  * @file    esc.c
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file provides firmware functions that implement the features of
  *          the esc component of the Motor Control SDK.
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
  */


/* Includes ------------------------------------------------------------------*/
#include "motorcontrol.h"
#include "ui_task.h"
#include "esc.h"

/* Define --------------------------------------------------------------------*/

#define STOP_DURATION_SEC 2
#define USER_TIMEBASE_FREQUENCY_HZ        400
#define STOP_DURATION  (STOP_DURATION_SEC * USER_TIMEBASE_FREQUENCY_HZ)
#define USER_TIMEBASE_OCCURENCE_TICKS  (SYS_TICK_FREQUENCY/USER_TIMEBASE_FREQUENCY_HZ)-1u

#define USER_TIMEBASE_FREQUENCY_HZ_BEEP   400
#define USER_TIMEBASE_OCCURENCE_TICKS_BEEP  (SYS_TICK_FREQUENCY/USER_TIMEBASE_FREQUENCY_HZ_BEEP)-1u

#define BEEP_FREQ_ARR 65000
#define BEEP_FREQ_ARR1 62000
#define BEEP_FREQ_ARR2  55000
#define BEEP_TIME_MAX 100
#define BEEP_TIME_MAX_CHECK 100
#define BEEP_DUTY 3000


static uint32_t esc_capture_filter(ESC_Handle_t * pHandle, uint32_t capture_value);
static void esc_reset_pwm_ch(ESC_Handle_t * pHandle);
#ifdef ESC_BEEP_FEATURE
static bool esc_beep_loop(ESC_Handle_t * pHandle, uint16_t number_beep);
static bool esc_phase_check(ESC_Handle_t * pHandle);
#endif      
extern MCI_Handle_t * pMCI[NBR_OF_MOTORS];

/**
  * @brief  Boot function to initialize the ESC board.
  * @retval none.
  */
void esc_boot(ESC_Handle_t * pHandle)
{
  TIM_TypeDef * TIMx = pHandle->pESC_params->Command_TIM;
  /*##- Start the Input Capture in interrupt mode ##########################*/
  LL_TIM_CC_EnableChannel (TIMx, LL_TIM_CHANNEL_CH2);
  LL_TIM_EnableIT_CC1 (TIMx);
  LL_TIM_CC_EnableChannel (TIMx, LL_TIM_CHANNEL_CH1);
  LL_TIM_EnableCounter(TIMx);
#ifdef ESC_BEEP_FEATURE
  pHandle->beep_state = SM_BEEP_1;
  pHandle->phase_check_status = false;
#endif
  
}


void esc_pwm_stop(ESC_Handle_t * pHandle)
{
  MCI_StopMotor( pMCI[pHandle->pESC_params->motor] );
  pHandle->sm_state = ESC_ARMING;
  pHandle->restart_delay = STOP_DURATION;  
  pHandle->arming_counter = 0;
}

ESC_State_t esc_pwm_run(ESC_Handle_t * pHandle)
{ 
  uint32_t new_speed;
  ESC_Params_t const * pESC_params = pHandle->pESC_params;
  bool cmd_status;
  ESC_State_t ESC_Fault_Occured = ESC_NOERROR;
 { 
    /* First we detect that we still receive signal from PWM input */
   if(pHandle->watchdog_counter == pHandle->watchdog_counter_prev)
   {
     if(pHandle->pwm_timeout == 0)
     {
       /* Ton_Value is not updated anymore, set to 0 for safety*/
       pHandle->Ton_value  = 0;
       ESC_Fault_Occured = ESC_NOSIGNAL;
     }
     else 
     {
       pHandle->pwm_timeout--;
     }
   }
   else 
   {
     pHandle->pwm_timeout = pESC_params->PWM_TURNOFF_MAX;
     pHandle->watchdog_counter_prev = pHandle->watchdog_counter;
     esc_reset_pwm_ch(pHandle);
   }  
  /* User defined code */
  switch (pHandle->sm_state)
  {
   case ESC_ARMING:
    {
      if((pHandle->Ton_value >= pESC_params->Ton_arming) && (pHandle->Ton_value < pESC_params->Ton_min)) 
      {
        pHandle->arming_counter++;
        if(pHandle->arming_counter > pESC_params->ARMING_TIME)    
         {
           pHandle->sm_state = ESC_ARMED;   
           pHandle->arming_counter  = 0;
           pHandle->pwm_timeout = pESC_params->PWM_TURNOFF_MAX;
           pHandle->watchdog_counter = 0;
           pHandle->watchdog_counter_prev = 0;   
         }
      }
      else 
       {
         pHandle->sm_state = ESC_ARMING;          
         pHandle->arming_counter  = 0;
       }
      }
    break;  
   case ESC_ARMED:
    {
      if (pHandle->Ton_value >= pESC_params->Ton_min)
      {
        /* Next state */
        /* This command sets what will be the first speed ramp after the 
          MC_StartMotor1 command. It requires as first parameter the 
          target mechanical speed in thenth of Hz and as
          second parameter the speed ramp duration in milliseconds. */
        MCI_ExecSpeedRamp( pMCI[pESC_params->motor], (pESC_params->speed_min_valueRPM/6), 0 );
        
        /* This is a user command used to start the motor. The speed ramp shall be
          pre programmed before the command.*/
        cmd_status = MCI_StartMotor( pMCI[pESC_params->motor] );
      
        /* It verifies if the command  "MCI_StartMotor" is successfully executed 
          otherwise it tries to restart the procedure */
        if(cmd_status==false)    
        {
          pHandle->sm_state = ESC_ARMING;                       // Command NOT executed
        }
        else 
        {
          pHandle->sm_state = ESC_POSITIVE_RUN;              // Command executed
          /* From this point the motor is spinning and stop and restart requires STOP_DURATION delay*/
          pHandle->restart_delay = STOP_DURATION; 
        }
        pHandle->restart_delay = STOP_DURATION;
      }
      else 
      {
        if (pHandle->Ton_value < pESC_params->Ton_arming)
        {
          pHandle->sm_state = ESC_ARMING;          
          pHandle->arming_counter  = 0;
        }
        else
        {
          /* Nothing to do stay in ARMED state waiting for TON > TON_MIN*/
        }
      }
        
    }
    break;  
   case ESC_POSITIVE_RUN:
    {  
      if( pHandle->Ton_value < pESC_params->Ton_min)
      {
        pHandle->turnoff_delay --;
        if(pHandle->turnoff_delay <= 0)
        {
          pHandle->sm_state = ESC_STOP; 
          pHandle->turnoff_delay = pESC_params->TURNOFF_TIME_MAX;
          /* This is a user command to stop the motor */
          MCI_StopMotor( pMCI[pESC_params->motor] );
        }         
      }
      else 
      {
        pHandle->turnoff_delay = pESC_params->TURNOFF_TIME_MAX;
                         
        if(pHandle->Ton_value <= pESC_params->Ton_max)
        {
          new_speed = ((pHandle->Ton_value-pESC_params->Ton_min) * (pESC_params->speed_max_valueRPM - pESC_params->speed_min_valueRPM) / pESC_params->delta_Ton_max) + pESC_params->speed_min_valueRPM;  
        }
        else 
        {
          new_speed = pESC_params->speed_max_valueRPM;
        }
        if (MC_GetSTMStateMotor1() == RUN)
        {
          MCI_ExecSpeedRamp( pMCI[pESC_params->motor], (new_speed/6), 50 );     
        }
      }
    }
      
   break;
   case ESC_STOP:
    {    
       /* After the time "STOP_DURATION" the motor will be restarted */
       if (pHandle->restart_delay == 0)
          {
            /* Next state */ 
            pHandle->sm_state = ESC_ARMING;  
            pHandle->Ton_value  = 0;
            pHandle->arming_counter = 0;
            pHandle->buffer_completed = false;
            pHandle->index_filter = 0;
             pHandle->pwm_accumulator = 0;  
          }
          else
          {
            pHandle->restart_delay--;
          }
    }
    break;  
  }
 }
 return (ESC_Fault_Occured);
}

/**
  * @brief  This is the main function to use in the main.c in order to start the current example 
  * @param  None
  * @retval None
  */
void esc_pwm_control(ESC_Handle_t * pHandle)
{
   ESC_State_t ESC_Fault_Occured;
 
  if(UI_IdleTimeHasElapsed()) {
    UI_SetIdleTime(USER_TIMEBASE_OCCURENCE_TICKS);   
#ifdef ESC_BEEP_FEATURE    
    if ( pHandle->phase_check_status == false)
    {
      pHandle->phase_check_status = esc_phase_check (pHandle);
    }
    else
#endif      
    {
      if (MC_GetSTMStateMotor1() == FAULT_OVER)
      {
#ifdef ESC_BEEP_FEATURE
       if (MC_GetOccurredFaultsMotor1() == MC_UNDER_VOLT)
       {
         pHandle->phase_check_status = false;
         pHandle->start_check_flag = false;
       }
#endif         
       MC_AcknowledgeFaultMotor1();
       pHandle->sm_state = ESC_ARMING;   
       pHandle->arming_counter  = 0;
      }
       
      ESC_Fault_Occured = esc_pwm_run(pHandle);
     
      if (ESC_Fault_Occured == ESC_NOSIGNAL && pHandle->sm_state == ESC_ARMING)
      {  
      #ifdef ESC_BEEP_FEATURE 
        esc_beep_loop(pHandle, 1);
      #endif        
      }
      else 
      {
       /* Nothing to do */
      }
    }
  }
}

static uint32_t esc_capture_filter(ESC_Handle_t * pHandle, uint32_t capture_value)
{ 
uint32_t pwm_filtered;
uint32_t pwm_max =0;

  if(pHandle->buffer_completed == false)
  {
     pHandle->pwm_accumulator += capture_value;
     pHandle->pwm_buffer[pHandle->index_filter] = capture_value;    
     pHandle->index_filter++;
     pwm_filtered = pHandle->pwm_accumulator/pHandle->index_filter;
     if(pHandle->index_filter >= ESC_FILTER_DEEP) 
       {
         pHandle->index_filter = 0;
         pHandle->buffer_completed = true;
       }
  }  
  else
  {
     /* We compute moving average, index_filter is the first data to remove*/
     pHandle->pwm_accumulator -= pHandle->pwm_buffer[pHandle->index_filter];
     pHandle->pwm_buffer[pHandle->index_filter] = capture_value;
     pHandle->pwm_accumulator += capture_value;
     for (uint8_t i =0; i< ESC_FILTER_DEEP; i++)
     {
       pwm_max = (pHandle->pwm_buffer[i] > pwm_max) ? pHandle->pwm_buffer[i] : pwm_max ;
     }
     pHandle->index_filter++;
     if(pHandle->index_filter >= ESC_FILTER_DEEP)
     {
      pHandle->index_filter = 0;
     }
     /* Remove the max pwm input from the average computation*/
     pwm_filtered = (pHandle->pwm_accumulator - pwm_max ) / (ESC_FILTER_DEEP -1);
  }
  pwm_filtered = (pwm_filtered==0) ? 1 : pwm_filtered ;
  
return(pwm_filtered);
}

#ifdef ESC_BEEP_FEATURE

static bool esc_beep_loop(ESC_Handle_t * pHandle, uint16_t number_beep)
{ 
   TIM_TypeDef * TIMx = pHandle->pESC_params->Motor_TIM;
  
   bool ESC_Beep_loop_STATUS = false;
        
 /* TIMx Peripheral Configuration -------------------------------------------*/     
  if( pHandle-> start_check_flag == false)
  {
     pHandle-> start_check_flag = true;
     ESC_Beep_loop_STATUS = false;
   
    /* Set the Output State */
    LL_TIM_SetAutoReload (TIMx, BEEP_FREQ_ARR);

    LL_TIM_CC_DisableChannel (TIMx, LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH2
                                   | LL_TIM_CHANNEL_CH3 | LL_TIM_CHANNEL_CH1N 
                                   | LL_TIM_CHANNEL_CH2N | LL_TIM_CHANNEL_CH3N);
    
    LL_TIM_CC_EnableChannel (TIMx, LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH2
                                   | LL_TIM_CHANNEL_CH3 );
    
    LL_TIM_EnableAllOutputs (TIMx);
  }  
 { 
  /* User defined code */
  switch (pHandle->beep_state)
  {
   case SM_BEEP_1:
    { 
      if(pHandle->beep_counter == 0)
      {
        LL_TIM_OC_SetCompareCH1 (TIMx,BEEP_DUTY);
        LL_TIM_OC_SetCompareCH2 (TIMx,BEEP_FREQ_ARR);
        LL_TIM_OC_SetCompareCH3 (TIMx,BEEP_FREQ_ARR);
        LL_TIM_CC_DisableChannel (TIMx,LL_TIM_CHANNEL_CH2 | LL_TIM_CHANNEL_CH3 );
        LL_TIM_CC_EnableChannel (TIMx, (LL_TIM_CHANNEL_CH2N | LL_TIM_CHANNEL_CH3N
                                     | LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH1N));
      }
      pHandle->beep_counter++;
      
      if(pHandle->beep_counter > BEEP_TIME_MAX)
      {
        if(number_beep == 1)
        {
          pHandle->beep_stop_time = 570;
          pHandle->beep_state =  SM_BEEP_4; 
        }
        if(number_beep == 2)
        {
          pHandle->beep_num ++;
          if(pHandle->beep_num <= 2)
          {
          LL_TIM_OC_SetCompareCH1 (TIMx,0);  
          pHandle->beep_state =  SM_BEEP_3; 
          }
          else 
          {
            pHandle->beep_stop_time = 410;
            pHandle->beep_state = SM_BEEP_4;
            pHandle->beep_num = 1;
          }
        } 
        if(number_beep == 3)
        {
          pHandle->beep_num ++;
          if(pHandle->beep_num <= 3)
          {
          LL_TIM_OC_SetCompareCH1 (TIMx,0); 
          pHandle->beep_state =  SM_BEEP_3; 
          }
          else 
          {
            pHandle->beep_stop_time = 270;
            pHandle->beep_state = SM_BEEP_4;
            pHandle->beep_num = 1;
          }
        }         
       pHandle->beep_counter = 0;
      }
    }
   break; 
  case SM_BEEP_3:
    {
    if(pHandle->beep_counter == 0)
      {
         LL_TIM_OC_SetCompareCH1 (TIMx,0); 
      }
      pHandle->beep_counter++;
      
      if(pHandle->beep_counter > 50)
      {
        pHandle->beep_state =  SM_BEEP_1; 
        pHandle->beep_counter = 0;
      }
    }
   break;      
   case SM_BEEP_4:
    {
    if(pHandle->beep_counter == 0)
      {
        LL_TIM_OC_SetCompareCH1 (TIMx,0);        
        LL_TIM_OC_SetCompareCH2 (TIMx,0);  
        LL_TIM_OC_SetCompareCH3 (TIMx,0); 
      }
      pHandle->beep_counter++;
      
      if(pHandle->beep_counter > pHandle->beep_stop_time)
      {
        pHandle->beep_state =  SM_BEEP_1; 
        pHandle->beep_counter = 0;
        esc_reset_pwm_ch(pHandle);
        pHandle-> start_check_flag = false;
        ESC_Beep_loop_STATUS = true;
      }
    }
   break;   
  }
 }
 return (ESC_Beep_loop_STATUS);
}

static bool esc_phase_check(ESC_Handle_t * pHandle)
{ 
   TIM_TypeDef * TIMx = pHandle->pESC_params->Motor_TIM;
   bool ESC_phase_check_status = false;
 
 /* TIMx Peripheral Configuration -------------------------------------------*/    
  if(pHandle-> start_check_flag == false)
  {
    pHandle-> start_check_flag = true;
    /* Set the Output State */
    ESC_phase_check_status = false;   
    LL_TIM_SetAutoReload (TIMx, BEEP_FREQ_ARR);
    
    LL_TIM_CC_DisableChannel (TIMx, LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH2
                                   | LL_TIM_CHANNEL_CH3 | LL_TIM_CHANNEL_CH1N 
                                   | LL_TIM_CHANNEL_CH2N | LL_TIM_CHANNEL_CH3N);
  
    LL_TIM_CC_EnableChannel (TIMx, LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH2
                                   | LL_TIM_CHANNEL_CH3 );
    
    LL_TIM_EnableAllOutputs (TIMx);
    pHandle->beep_state = SM_BEEP_1;
    pHandle->beep_counter = 0;

  }  
 { 
  /* User defined code */
  switch (pHandle->beep_state)
  {
   case SM_BEEP_1:
    { 
      if(pHandle->beep_counter == 0)
      {
        LL_TIM_OC_SetCompareCH3 (TIMx,BEEP_DUTY); 
        LL_TIM_OC_SetCompareCH2 (TIMx,BEEP_FREQ_ARR);
        LL_TIM_OC_SetCompareCH1 (TIMx,BEEP_FREQ_ARR);

        LL_TIM_CC_DisableChannel (TIMx,LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH2 );
        LL_TIM_CC_EnableChannel (TIMx, (LL_TIM_CHANNEL_CH1N | LL_TIM_CHANNEL_CH2N
                                     | LL_TIM_CHANNEL_CH3N | LL_TIM_CHANNEL_CH3));        

      }
      pHandle->beep_counter++;
      if(pHandle->beep_counter > BEEP_TIME_MAX_CHECK)
      {
        pHandle->beep_state =  SM_BEEP_2; 
        pHandle->beep_counter = 0;
        LL_TIM_OC_SetCompareCH2 (TIMx,BEEP_DUTY);
        LL_TIM_SetAutoReload (TIMx, BEEP_FREQ_ARR1);
      }
    }
   break; 
   case SM_BEEP_2:
    {
    if(pHandle->beep_counter == 0)
      {
        LL_TIM_OC_SetCompareCH2 (TIMx,BEEP_DUTY);
        LL_TIM_OC_SetCompareCH1 (TIMx,BEEP_FREQ_ARR1);
        LL_TIM_OC_SetCompareCH3 (TIMx,BEEP_FREQ_ARR1);
        LL_TIM_CC_DisableChannel (TIMx,LL_TIM_CHANNEL_CH3 | LL_TIM_CHANNEL_CH1 );
        LL_TIM_CC_EnableChannel (TIMx, (LL_TIM_CHANNEL_CH3N | LL_TIM_CHANNEL_CH1N
                                     | LL_TIM_CHANNEL_CH2 | LL_TIM_CHANNEL_CH2N));
      }
      pHandle->beep_counter++;
      
      if(pHandle->beep_counter > BEEP_TIME_MAX_CHECK)
      {
        pHandle->beep_state =  SM_BEEP_3; 
        pHandle->beep_counter = 0;
        LL_TIM_OC_SetCompareCH1 (TIMx,BEEP_DUTY);
        LL_TIM_SetAutoReload (TIMx, BEEP_FREQ_ARR2);          
      }
    }
   break;  
   case SM_BEEP_3:
    {
    if(pHandle->beep_counter == 0)
      {
        LL_TIM_OC_SetCompareCH1 (TIMx,BEEP_DUTY);
        LL_TIM_OC_SetCompareCH2 (TIMx,BEEP_FREQ_ARR2);
        LL_TIM_OC_SetCompareCH3 (TIMx,BEEP_FREQ_ARR2);
        LL_TIM_CC_DisableChannel (TIMx,LL_TIM_CHANNEL_CH2 | LL_TIM_CHANNEL_CH3 );
        LL_TIM_CC_EnableChannel (TIMx, (LL_TIM_CHANNEL_CH2N | LL_TIM_CHANNEL_CH3N
                                     | LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH1N));       
      }
      pHandle->beep_counter++;
      
      if(pHandle->beep_counter > BEEP_TIME_MAX_CHECK)
      {
        pHandle->beep_state =  SM_BEEP_4; 
        pHandle->beep_counter = 0;
      }    
    }
   break;  
   case SM_BEEP_4:
    {
    if(pHandle->beep_counter == 0)
      {
        LL_TIM_OC_SetCompareCH1 (TIMx,0);
        LL_TIM_OC_SetCompareCH2 (TIMx,0);
        LL_TIM_OC_SetCompareCH3 (TIMx,0);
      }
      pHandle->beep_counter++;
      
      if(pHandle->beep_counter > 1000)
      {
        pHandle->beep_state =  SM_BEEP_1; 
        pHandle->beep_counter = 0; 
        esc_reset_pwm_ch(pHandle);
        pHandle-> start_check_flag = false;
        ESC_phase_check_status = true;       
      }
    }
   break;    
  }
 }
 return(ESC_phase_check_status);
}

#endif // ESC_BEEP_FEATURE

static void esc_reset_pwm_ch(ESC_Handle_t * pHandle)
{
    TIM_TypeDef * TIMx = TIM1;

    LL_TIM_CC_DisableChannel (TIMx, (LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH2
                           | LL_TIM_CHANNEL_CH3 | LL_TIM_CHANNEL_CH1N
                           | LL_TIM_CHANNEL_CH2N | LL_TIM_CHANNEL_CH3N ) );

    LL_TIM_SetAutoReload (TIMx, ((PWM_PERIOD_CYCLES) / 2));
   
    /* Set the Output State */ 
    LL_TIM_CC_EnableChannel (TIMx, (LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH2
                           | LL_TIM_CHANNEL_CH3 | LL_TIM_CHANNEL_CH1N
                           | LL_TIM_CHANNEL_CH2N | LL_TIM_CHANNEL_CH3N ));
}

/**
  * @brief  This function handles TIM2 global interrupt request.
  * @param  None
  * @retval None
  */

void TIM2_IRQHandler(void)
{

  /* Clear TIM1 Capture compare interrupt pending bit */
  LL_TIM_ClearFlag_CC1 (TIM2);

  /* Get Pulse width and low pass filter it to remove spurious informations */    
  ESC_M1.Ton_value = esc_capture_filter(&ESC_M1, LL_TIM_OC_GetCompareCH2(TIM2));

  /* Fail safe mechanism: stops the motor is the PWM input is disabled */
  ESC_M1.watchdog_counter++;
  if(ESC_M1.watchdog_counter == 0)
     ESC_M1.watchdog_counter = 1;
}

/******************* (C) COPYRIGHT 2016 STMicroelectronics *****END OF FILE****/
