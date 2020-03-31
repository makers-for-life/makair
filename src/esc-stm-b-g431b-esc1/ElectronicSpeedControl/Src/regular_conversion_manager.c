
/**
  ******************************************************************************
  * @file    regular_conversion_manager.c
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file provides firmware functions that implement the following features
  *          of the regular_conversion_manager component of the Motor Control SDK:
  *           Register conversion with or without callback
  *           Execute regular conv directly from Temperature and VBus sensors
  *           Execute user regular conversion scheduled by medium frequency task
  *           Manage user conversion state machine
  *           +
  *           +
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
#include "regular_conversion_manager.h"
#include "mc_config.h"

/** @addtogroup MCSDK
  * @{
  */

/** @defgroup RCM Regular Conversion Manager 
  * @brief Regular Conversion Manager component of the Motor Control SDK
  *
  * MotorControl SDK makes an extensive usage of ADCs. Some conversions are timing critical
  * like current reading, and some have less constraints. If an ADC offers both Injected and Regular,
  * channels, critical conversions will be systematically done on Injected channels, because they 
  * interrupt any ongoing regular conversion so as to be executed without delay.
  * Others conversions, mainly Bus voltage, and Temperature sensing are performed with regular channels.
  * If users wants to perform ADC conversions with an ADC already used by MC SDK, they must use regular
  * conversions. It is forbidden to use Injected channel on an ADC that is already in use for current reading.
  * As usera and MC-SDK may share ADC regular scheduler, this component intents to manage all the 
  * regular conversions.
  * 
  * If users wants to execute their own conversion, they first have to register it through the 
  * RCM_RegisterRegConv_WithCB() or RCM_RegisterRegConv() APIs. Multiple conversions can be registered, 
  * but only one can be scheduled at a time .
  *
  * A requested user regular conversion will be executed by the medium frequency task after the 
  * MC-SDK regular safety conversions: Bus voltage and Temperature.
  *
  * If a callback is registered, particular care must be taken with the code executed inside the CB.
  * The callback code is executed under Medium frequency task IRQ context (Systick).
  *
  * If the Users do not register a callback, they must poll the RCM state machine to know if
  * a conversion is ready to be read, scheduled, or free to be scheduled. This is performed through 
  * the RCM_GetUserConvState() API.
  *
  * If the state is RCM_USERCONV_IDLE, a conversion is ready to be scheduled.
  * if a conversion is already scheduled, the returned value is RCM_USERCONV_REQUESTED.
  * if a conversion is ready to be read, the returned value is RCM_USERCONV_EOC.
  * In RCM_USERCONV_EOC state, a call to RCM_GetUserConv will consume the value, and set the state machine back
  * to RCM_USERCONV_IDLE state. It implies that a second call without new conversion performed,
  * will send back 0xffff which is an error value meaning that the data is not available.
  * If a conversion request is executed, but the previous conversion has not been completed, nor consumed, 
  * the request is discarded and the RCM_RequestUserConv return false.
  * 
  * If a callback is registered, the data read is sent back to the callback parameters, and therefor consumed.
  * @{
  */

/* Private typedef -----------------------------------------------------------*/

typedef struct 
{
  RCM_exec_cb_t cb;
  void * data;
} RCM_callback_t;

/* Private defines -----------------------------------------------------------*/
/**
  * @brief Number of regular conversion allowed By default.
  *  
  * In single drive configuration, it is defined to 4. 2 of them are consumed by 
  * Bus voltage and temperature reading. This leaves 2 handles available for 
  * user conversions
  *
  * In dual drives configuration, it is defined to 6. 2 of them are consumed by 
  * Bus voltage and temperature reading for each motor. This leaves 2 handles 
  * available for user conversion.
  *
  * Defined to 4 here. 
  */
#define RCM_MAX_CONV  4 

/* Global variables ----------------------------------------------------------*/

RegConv_t * RCM_handle_array [RCM_MAX_CONV];
RCM_callback_t RCM_CB_array [RCM_MAX_CONV];

uint8_t RCM_UserConvHandle;
uint16_t RCM_UserConvValue;
RCM_UserConvState_t RCM_UserConvState;

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Registers a regular conversion, and attaches a callback.
  * 
  * This function registers a regular ADC conversion that can be later scheduled for execution. It
  * returns a handle that uniquely identifies the conversion. This handle is used in the other API
  * of the Regular Converion Manager to reference the registered conversion.
  * 
  * A regular conversion is defined by an ADC + ADC channel pair. If a registration already exists 
  * for the requested ADC + ADC channel pair, the same handle will be reused.
  *
  * The regular conversion is registered along with a callback that is executed each time the
  * conversion has completed. The callback is invoked with two parameters:
  *
  * - the handle of the regular conversion
  * - a data pointer, supplied by uthe users at registration time.
  * 
  * The registration may fail if there is no space left for additional conversions. The 
  * maximum number of regular conversion that can be registered is defined by #RCM_MAX_CONV.
  *  
  * @note   Users who do not want a callback to be executed at the end of the conversion, 
  *         should use RCM_RegisterRegConv() instead.
  *
  * @param  regConv Pointer to the regular conversion parameters. 
  *         Contains ADC, Channel and sampling time to be used.
  *
  * @param  fctCB Function called once the regular conversion is executed.
  *
  * @param  data Used to save a user context. this parameter will be send back by 
  *               the fctCB function. @b Note: This parameter can be NULL if not used.
  *
  *  @retval the handle of the registered conversion or 255 if the registration failed
  */
uint8_t RCM_RegisterRegConv_WithCB (RegConv_t * regConv, RCM_exec_cb_t fctCB, void *data)
{
  uint8_t handle;
  handle = RCM_RegisterRegConv(regConv);
  if (handle < RCM_MAX_CONV) 
  {
    RCM_CB_array [handle].cb = fctCB;
    RCM_CB_array [handle].data = data;
  }
  return handle;
}  
  
/**
  * @brief  Registers a regular conversion.
  * 
  * This function registers a regular ADC conversion that can be later scheduled for execution. It
  * returns a handle that uniquely identifies the conversion. This handle is used in the other API
  * of the Regular Converion Manager to reference the registered conversion.
  * 
  * A regular conversion is defined by an ADC + ADC channel pair. If a registration already exists 
  * for the requested ADC + ADC channel pair, the same handle will be reused.
  *
  * The registration may fail if there is no space left for additional conversions. The 
  * maximum number of regular conversion that can be registered is defined by #RCM_MAX_CONV.
  *  
  * @note   Users who do not want a callback to be executed at the end of the conversion, 
  *         should use RCM_RegisterRegConv() instead.
  *
  * @param  regConv Pointer to the regular conversion parameters. 
  *         Contains ADC, Channel and sampling time to be used.
  *
  *  @retval the handle of the registered conversion or 255 if the registration failed
  */
uint8_t RCM_RegisterRegConv(RegConv_t * regConv)
{
  uint8_t handle=255;
  uint8_t i=0;
  
    /* Parse the array to be sure that same 
     * conversion does not already exist*/
    while (i < RCM_MAX_CONV)
    { 
      if (  RCM_handle_array [i] == 0 && handle > RCM_MAX_CONV)  
      {
        handle = i; /* First location available, but still looping to check that this config does not already exist*/ 
      }
      /* Ticket 64042 : If RCM_handle_array [i] is null access to data member will cause Memory Fault. */
      if (  RCM_handle_array [i] != 0 )
      {
        if ((RCM_handle_array [i]->channel == regConv->channel) && 
            (RCM_handle_array [i]->regADC == regConv->regADC))
          {
           handle =i; /* Reuse the same handle */
           i = RCM_MAX_CONV; /* we can skip the rest of the loop*/
          }
      }
      i++;
    }    
    if (handle < RCM_MAX_CONV )
    {    
      RCM_handle_array [handle] = regConv;
      RCM_CB_array [handle].cb = NULL; /* if a previous callback was attached, it is cleared*/
      if (LL_ADC_IsEnabled(regConv->regADC) == 0 )
      {
       LL_ADC_DisableIT_EOC(regConv->regADC);
       LL_ADC_ClearFlag_EOC(regConv->regADC);
       LL_ADC_DisableIT_JEOC(regConv->regADC);
       LL_ADC_ClearFlag_JEOC(regConv->regADC);

        LL_ADC_StartCalibration( regConv->regADC, LL_ADC_SINGLE_ENDED );
        while ( LL_ADC_IsCalibrationOnGoing( regConv->regADC ) )  
        { }
        /* ADC Enable (must be done after calibration) */
        /* ADC5-140924: Enabling the ADC by setting ADEN bit soon after polling ADCAL=0 
        * following a calibration phase, could have no effect on ADC 
        * within certain AHB/ADC clock ratio.
        */
        while (  LL_ADC_IsActiveFlag_ADRDY( regConv->regADC ) == 0)  
        { 
          LL_ADC_Enable( regConv->regADC );
        }
        
      }
      else 
      {
      }
      /* reset regular conversion sequencer length set by cubeMX */
      LL_ADC_REG_SetSequencerLength( regConv->regADC, LL_ADC_REG_SEQ_SCAN_DISABLE );
      /* configure the sampling time (should already be configured by for non user conversions)*/
      LL_ADC_SetChannelSamplingTime ( regConv->regADC, __LL_ADC_DECIMAL_NB_TO_CHANNEL(regConv->channel) ,regConv->samplingTime);
    }
    else
    {
      /* Nothing to do handle is already set to error value : 255 */
    }
  return handle;
}

/*
 * This function is used to read the result of a regular conversion.
 * This function polls on the ADC end of conversion.
 * As ADC have injected channels for currents sensing, 
 * There is no issue to execute regular conversion asynchronously.
 *
 * NOTE: This function is not part of the public API and users should not call it. 
 */
uint16_t RCM_ExecRegularConv (uint8_t handle)
{
  uint16_t retVal;
 
  LL_ADC_REG_SetSequencerRanks( RCM_handle_array[handle]->regADC,
                                LL_ADC_REG_RANK_1,
                                __LL_ADC_DECIMAL_NB_TO_CHANNEL( RCM_handle_array[handle]->channel ) );

  LL_ADC_REG_ReadConversionData12( RCM_handle_array[handle]->regADC );

    LL_ADC_REG_StartConversion( RCM_handle_array[handle]->regADC );
  /* Wait until end of regular conversion */
  while ( LL_ADC_IsActiveFlag_EOC( RCM_handle_array[handle]->regADC ) == 0u ) {}
  retVal = LL_ADC_REG_ReadConversionData12( RCM_handle_array[handle]->regADC );   
return retVal;
}

/**
 * @brief Schedules a regular conversion for execution.
 *  
 * This function requests the execution of the user-defined regular conversion identified
 * by @p handle. All user defined conversion requests must be performed inside routines with the
 * same priority level. If a previous regular conversion request is pending this function has no 
 * effect, for this reason is better to call RCM_GetUserConvState() and check if the state is 
 * #RCM_USERCONV_IDLE before calling RCM_RequestUserConv().
 *
 * @param  handle used for the user conversion.
 *
 * @return true if the regular conversion could be scheduled and false otherwise.
 */
bool RCM_RequestUserConv(uint8_t handle)
{
  bool retVal = false;
  if (RCM_UserConvState == RCM_USERCONV_IDLE)
  {
    RCM_UserConvHandle = handle;
    /* must be done last so that RCM_UserConvHandle already has the right value */
    RCM_UserConvState = RCM_USERCONV_REQUESTED;
    retVal = true;
  }
  return retVal;
}

/**
 * @brief  Returns the last user-defined regular conversion that was executed.
 *
 * This function returns a valid result if the state returned by
 * RCM_GetUserConvState is #RCM_USERCONV_EOC.
 *
 * @retval uint16_t The converted value or 0xFFFF in case of conversion error.
 */
uint16_t RCM_GetUserConv(void)
{
  uint16_t hRetVal = 0xFFFFu;
  if (RCM_UserConvState == RCM_USERCONV_EOC)
  {
    hRetVal = RCM_UserConvValue;
    RCM_UserConvState = RCM_USERCONV_IDLE;
  }
  return hRetVal;
}

/*
 *  This function must be scheduled by mc_task.
 *  It executes the current user conversion that has been selected by the 
 *  latest call to RCM_RequestUserConv
 *
 * NOTE: This function is not part of the public API and users should not call it. 
 */
void RCM_ExecUserConv ()
{
  if (RCM_UserConvState == RCM_USERCONV_REQUESTED)
  {
    RCM_UserConvValue = RCM_ExecRegularConv (RCM_UserConvHandle);
    RCM_UserConvState = RCM_USERCONV_EOC;
    if (RCM_CB_array [RCM_UserConvHandle].cb != NULL)
    {
      RCM_UserConvState = RCM_USERCONV_IDLE;
      RCM_CB_array [RCM_UserConvHandle].cb (RCM_UserConvHandle, RCM_UserConvValue ,RCM_CB_array [RCM_UserConvHandle].data);
    }
  }
}

/**
 * @brief  Returns the status of the last requested regular conversion.
 *
 * It can be one of the following values:
 
 * - UDRC_STATE_IDLE no regular conversion request pending.
 * - UDRC_STATE_REQUESTED regular conversion has been requested and not completed.
 * - UDRC_STATE_EOC regular conversion has been completed but not readed from the user.
 *
 * @retval The state of the last user-defined regular conversion.
 */
RCM_UserConvState_t RCM_GetUserConvState(void)
{
  return RCM_UserConvState;
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

