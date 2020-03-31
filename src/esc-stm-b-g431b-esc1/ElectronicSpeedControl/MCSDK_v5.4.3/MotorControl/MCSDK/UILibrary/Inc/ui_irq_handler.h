/**
  ******************************************************************************
  * @file    ui_irq_handler.h
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file contains all definitions and functions prototypes for the
  *          UI IRQ Handler component of the Motor Control SDK. This component is
  *          a temporary work around allowing to use the former IRQ Handler
  *          registration mechanism with both the old Classes and the new Components.
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
#ifndef __UI_IRQ_HANDLER_H
#define __UI_IRQ_HANDLER_H

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "mc_type.h"

/** @addtogroup MCSDK
  * @{
  */

 /**
  * @addtogroup MCUI
  * @{
  */

/** @addtogroup UI_IRQ_HANDLER
  * @{
  */

 /* Exported constants -------------------------------------------------------*/
#define UI_IRQ_USART            0u /*!< Reserved for UI serial communication. */

//** @brief Fake definition used avoid build issues with the users of the former UIIRQHandlerClass */
#define _CUIIRQ void *


/* Exported macros -----------------------------------------------------------*/
/** @brief Bridges the Set_UI_IRQ_Handler from the 4.3 implementation to the
  * new and temporary UIIRQ_SetIrqHandler function
  *
  * This function registers @p Obj as the IRQ handler for the Interrupt
  * identified with @p Id.
  * @p Obj is both a pointer on the instance of a class @b and
  * a pointer a pointer on the interrupt handling function.
  * This means that the first field of the object's structure must be a pointer
  * on the interrupt handling function. This hidden constraint only exists until
  * 4.3, not from 5.0 onwards.
  *
  * @b BEWARE: This macro is meant to be used by 4.3 code only. 5.0 code
  * (components and Cockpit) shall NOT use it.
  */
#define Set_UI_IRQ_Handler( Id, Obj ) UIIRQ_SetIrqHandler( (Id), *(UIIRQ_Handler_t *)(Obj), (void *)(Obj) )

/** @brief Bridges the Exec_UI_IRQ_Handler from the 4.3 implementation to the
  * new and temporary UIIRQ_ExecIrqHandler function
  *
  * This function executes the UI IRQ handler for the Interrupt identified
  * with @p Id.
  * The executed UI IRQ handler must have been registered prior to calling this
  * macro. See Set_UI_IRQ_Handler for this.
  *
  * The executed UI IRQ Handling function is passed three parameters.
  * The first one is a pointer on the object the function is to work on,
  * as registered with the Set_UI_IRQ_Handler function.
  * The second one is the uint8_t value @p flag.
  * The third one is the uint16_t value @p rx_data.
  *
  * @b BEWARE: This macro is meant to be used by 4.3 code only. 5.0 code
  * (components and Cockpit) shall NOT use it.
  */
#define Exec_UI_IRQ_Handler( Id, Flag, Rx_Data ) UIIRQ_ExecIrqHandler( Id, Flag, Rx_Data )

   
/* Exported types ------------------------------------------------------------*/
/**
 * @brief Type of an Interrupt handling function
 *
 *  Interrupt handling functions accept three parameters:
 *
 * - a void pointer that is supposed to contain either a pointer on the handle on
 * the component the function operates on or a pointer on the object of which the
 * function is a method. This pointer is registered with UIIRQ_SetIrqHandler().
 * - a uint8_t flag that is passed to the UIIRQ_ExecIrqHandler() function.
 * - a uint16_t rx_data that is passed to the UIIRQ_ExecIrqHandler() function.
 *
 * Interrupt handling functions return a void pointer.
 */
typedef void * (*UIIRQ_Handler_t)( void *, uint8_t, uint16_t );


/* Exported functions ------------------------------------------------------- */
void UIIRQ_SetIrqHandler(uint8_t bIRQAddr, UIIRQ_Handler_t Handler, void * Handle);
void* UIIRQ_ExecIrqHandler(uint8_t bIRQAddr, uint8_t flag, uint16_t rx_data);


/**
  * @}
  */

/**
  * @}
  */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif /* __cpluplus */

#endif /* __UI_IRQ_HANDLER_H */

/************************ (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
