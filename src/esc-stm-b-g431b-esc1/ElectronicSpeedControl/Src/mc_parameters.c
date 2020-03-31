/* USER CODE BEGIN Additional include */
#include "esc.h"
/* USER CODE END Additional include */  

/* USER CODE BEGIN Additional parameters */
const ESC_Params_t ESC_ParamsM1 =
{
  .Command_TIM = TIM2,
  .Motor_TIM = TIM1,
  .ARMING_TIME = 200,
  .PWM_TURNOFF_MAX  = 500,   
  .TURNOFF_TIME_MAX = 500, 
  .Ton_max =  ESC_TON_MAX,                      /*!<  Maximum ton value for PWM (by default is 1800 us) */
  .Ton_min =  ESC_TON_MIN,                      /*!<  Minimum ton value for PWM (by default is 1080 us) */ 
  .Ton_arming = ESC_TON_ARMING,                 /*!<  Minimum value to start the arming of PWM */ 
  .delta_Ton_max = ESC_TON_MAX - ESC_TON_MIN,      
  .speed_max_valueRPM = MOTOR_MAX_SPEED_RPM,    /*!< Maximum value for speed reference from Workbench */
  .speed_min_valueRPM = 1000,                   /*!< Set the minimum value for speed reference */
  .motor = M1,
};
/* USER CODE END Additional parameters */  