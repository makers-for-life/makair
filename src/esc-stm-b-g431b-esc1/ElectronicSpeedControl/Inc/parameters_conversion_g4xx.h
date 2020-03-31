/* USER CODE BEGIN Additional parameters */
/* ESC parameters conversion */

/* 1060 us ON time is the minimum value to spin the motor*/
/* 1860 us ON time is the target for the maximum speed */
#define ESC_TON_MIN (APB1TIM_FREQ / 1000000 * 1060 )
#define ESC_TON_MAX (APB1TIM_FREQ / 1000000 * 1860 )
/* Arming : if 900 us <= TOn < 1060 us for 200 us, ESC is armed */
#define ESC_TON_ARMING ((APB1TIM_FREQ /1000000) * 900 )
/* USER CODE END Additional parameters */  