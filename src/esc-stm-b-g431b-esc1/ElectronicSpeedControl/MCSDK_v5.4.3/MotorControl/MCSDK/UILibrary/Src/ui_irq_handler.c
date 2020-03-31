/**
  ******************************************************************************
  * @file    ui_irq_handler.c
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file provides firmware functions that implement the UI IRQ Handler
  *          component of the Motor Control SDK.
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
#include "ui_irq_handler.h"
#include "mc_type.h"


/** @addtogroup MCSDK
  * @{
  */

/**
 * @addtogroup MCUI
 * @{
 */

/** @defgroup UI_IRQ_HANDLER MC IRQ Handler Component
  * @brief Performs registration and execution of Interrupts handlers for the UI
  *
  *  This component is a temporary work around allowing to use the former IRQ Handler
  * registration mechanism with both the old Classes and the new Components.
  *
  *  The former mechanism is defined in the UIIRQHandlerClass and is basically playing
  * two roles.
  *
  *  The first one is to register an Object and a method of that object to be executed
  * when an given interrupt occurs. Interrupt sources are identified by a number; 1
  * such sources are defined:
  *
  * - #UI_IRQ_USART
  *
  *  The second is to provide a function that executes the Interrupt handler
  * registered for a given interrupt source.
  *
  *  The implementation of the UIIRQHandlerClass mandated that the first field of the
  * structure of the registered Object be a pointer on the interrupt handling function to
  * execute.
  *
  *  This constraint is unacceptable for Components and this is the reason why
  * this work-around is proposed.
  *
  *  It is only a work around as the actual interrupt handling method to be used
  * on the 5.0.0 onwards will rely on Cube FW mechanisms and is still TBD in details.
  *
  *  This work around is designed to work both with remanants of the former 4.3 architecture and
  * with the new components from the 5.0 architecture. It minimizes the changes to be made in the
  * legacy 4.3 code by providing two macros named after the UIIRQHandlerClass methods:
  *
  * - #Set_UI_IRQ_Handler
  * - #Exec_UI_IRQ_Handler
  *
  *  And it provides two functions, similar to the former UIIRQHandlerClass's methods, but designed
  * to work with the new components.
  * @{
  */


/* Private typedef -----------------------------------------------------------*/
/**
  * @brief Interrupt handling configuration structure
  *
  *  The #Handler field is a pointer on the Interrupt handling function, while the
  * #Handle field is a pointer that is passed as the first parameter to the interrupt
  * handling function when it is executed.
  */
typedef struct
{
    UIIRQ_Handler_t Handler;
    void *          Handle;
} UIIRQ_HandlerConfigItem_t;


/* Private defines -----------------------------------------------------------*/
/** @brief Number of Interrupt handlers managed by the UI IRQ component */
#define MAX_UI_IRQ_NUM 1


/* Static variables ----------------------------------------------------------*/
/** @brief Table containing the Interrupt handling configurations */
static UIIRQ_HandlerConfigItem_t oUI_IRQTable[MAX_UI_IRQ_NUM];


/* Functions ---------------------------------------------------- */
/**
  * @brief Sets the function to invoque when the interrupt identified by bIRQAddr occurs.
  * @p Handler as the handler for the interrupt identified by @p bIRQAddr.
  * @p Handle is also registered and passed as first argument to the @p Handler function when it
  * is executed.
  */

void UIIRQ_SetIrqHandler(uint8_t bIRQAddr, UIIRQ_Handler_t Handler, void * Handle)
{
    if ( bIRQAddr < MAX_UI_IRQ_NUM )
    {
        oUI_IRQTable[ bIRQAddr ].Handler = Handler;
        oUI_IRQTable[ bIRQAddr ].Handle  = Handle;
    }
}


/** @brief Executes the function registered for the interrupt identified by
  * @p bIRQAddr and returns its return value.
  * @p flag is passed as second argument to the handler function, the first being the pointer
  * that was registered with it.
  * @p rx_data is passed as third argument to the handler function.
  */
void* UIIRQ_ExecIrqHandler(uint8_t bIRQAddr, uint8_t flag, uint16_t rx_data)
{  
  void * ret_val = MC_NULL;

  if ( bIRQAddr < MAX_UI_IRQ_NUM )
  {
      ret_val = oUI_IRQTable[ bIRQAddr ].Handler( oUI_IRQTable[ bIRQAddr ].Handle, flag, rx_data );
  }

  return ret_val;
}

/**
  * @}
  */

/**
  * @}
  */

/**
 * @}
 */

/************************ (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
