/**
  ******************************************************************************
  * @file    usart_frame_communication_protocol.h
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file contains all definitions and functions prototypes for the
  *          Frame Communication Protocol on USART component of the Motor Control SDK.
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
#ifndef __UFCP_H
#define __UFCP_H

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "frame_communication_protocol.h"

/** @addtogroup MCSDK
  * @{
  */

/**
 * @addtogroup MCUI
 * @{
 */

/** @addtogroup UFCP
  * @{
  */

 /**
  * @brief Error Code for an Overrun FCP message frame.
  *
  * Such a payload is sent to the remote when an USART overrun error occurs.
  *
  * The value of the error code is set by the Frame Communication Protocol and is taken in
  * a value space that is shared with the Motor Control Protocol. It thus cannot be
  * changed.
  */
#define UFCP_MSG_OVERRUN 0x08

 /* Exported types ------------------------------------------------------------*/
typedef struct {
  FCP_Handle_t _Super;

  USART_TypeDef * USARTx;
  uint32_t USARTRemapping;
  uint32_t USARTClockSource;
  GPIO_TypeDef * RxPort;
  uint16_t RxPin;
  GPIO_TypeDef * TxPort;
  uint16_t TxPin;
  uint8_t UIIRQn;

} UFCP_Handle_t;

/* Exported functions ------------------------------------------------------- */

void UFCP_Init( UFCP_Handle_t * pHandle );

uint8_t UFCP_Receive( FCP_Handle_t * pHandle );

uint8_t UFCP_Send( FCP_Handle_t * pHandle, uint8_t code, uint8_t *buffer, uint8_t size);

void UFCP_AbortReceive( FCP_Handle_t * pHandle );

void * UFCP_RX_IRQ_Handler( UFCP_Handle_t * pHandle, unsigned short rx_data );

void UFCP_TX_IRQ_Handler( UFCP_Handle_t * pHandle );

void UFCP_OVR_IRQ_Handler( UFCP_Handle_t * pHandle );

void UFCP_TIMEOUT_IRQ_Handler( UFCP_Handle_t * pHandle );

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

#endif /* __UFCP_H */

/******************* (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
