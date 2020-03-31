/**
  ******************************************************************************
  * @file    circle_limitation.c
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file provides the functions that implement the circle
  *          limitation feature of the STM32 Motor Control SDK.
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
#include "circle_limitation.h"
#include "mc_math.h"
#include "mc_type.h"

/** @addtogroup MCSDK
  * @{
  */

/** @defgroup CircleLimitation Circle Limitation
  * @brief Circle Limitation component of the Motor Control SDK
  *
  * @todo Document the Circle Limitation "module".
  *
  * @{
  */

#if defined (CIRCLE_LIMITATION_VD)
__weak qd_t Circle_Limitation(CircleLimitation_Handle_t * pHandle, qd_t Vqd)
{
  int32_t MaxModule;
  int32_t square_q;
  int32_t square_temp;
  int32_t square_d;
  int32_t square_sum;
  int32_t square_limit;
  int32_t vd_square_limit;
  int32_t new_q;
  int32_t new_d;
  qd_t Local_Vqd=Vqd;

  MaxModule = pHandle->MaxModule;

  square_q = (int32_t)(Vqd.q) * Vqd.q;
  square_d = (int32_t)(Vqd.d) * Vqd.d;
  square_limit = MaxModule * MaxModule;
  vd_square_limit = pHandle->MaxVd * pHandle->MaxVd;
  square_sum = square_q + square_d;

  if (square_sum > square_limit)
  {
    if(square_d <= vd_square_limit)
    {
      square_temp = square_limit - square_d;
      new_q = MCM_Sqrt(square_temp);
      if(Vqd.q < 0)
      {
        new_q = -new_q;
      }
      new_d = Vqd.d;
    }
    else
    {
      new_d = pHandle->MaxVd;
      if(Vqd.d < 0)
      {
        new_d = -new_d;
      }

      square_temp = square_limit - vd_square_limit;
      new_q = MCM_Sqrt(square_temp);
      if(Vqd.q < 0)
      {
        new_q = - new_q;
      }
    }
    Local_Vqd.q = new_q;
    Local_Vqd.d = new_d;
  }
  return(Local_Vqd);
}
#else
/**
  * @brief Check whether Vqd.q^2 + Vqd.d^2 <= 32767^2
  *        and if not it applies a limitation keeping constant ratio
  *        Vqd.q / Vqd.d
  * @param  pHandle pointer on the related component instance
  * @param  Vqd Voltage in qd reference frame
  * @retval qd_t Limited Vqd vector
  */
__weak qd_t Circle_Limitation( CircleLimitation_Handle_t * pHandle, qd_t Vqd )
{
  uint16_t table_element;
  uint32_t uw_temp;
  int32_t  sw_temp;
  qd_t local_vqd = Vqd;

  sw_temp = ( int32_t )( Vqd.q ) * Vqd.q +
            ( int32_t )( Vqd.d ) * Vqd.d;

  uw_temp = ( uint32_t ) sw_temp;

  /* uw_temp min value 0, max value 32767*32767 */
  if ( uw_temp > ( uint32_t )( pHandle->MaxModule ) * pHandle->MaxModule )
  {

    uw_temp /= ( uint32_t )( 16777216 );

    /* wtemp min value pHandle->Start_index, max value 127 */
    uw_temp -= pHandle->Start_index;

    /* uw_temp min value 0, max value 127 - pHandle->Start_index */
    table_element = pHandle->Circle_limit_table[( uint8_t )uw_temp];

    sw_temp = Vqd.q * ( int32_t )table_element;
    local_vqd.q = ( int16_t )( sw_temp / 32768 );

    sw_temp = Vqd.d * ( int32_t )( table_element );
    local_vqd.d = ( int16_t )( sw_temp / 32768 );
  }

  return ( local_vqd );
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/

