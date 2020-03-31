/**
  ******************************************************************************
  * @file    motor_control_protocol.h
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file contains all definitions and functions prototypes for the
  *          motor_control_protocol component of the Motor Control SDK.
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
#ifndef __MOTOR_CONTROL_PROTOCOL_H
#define __MOTOR_CONTROL_PROTOCOL_H

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "frame_communication_protocol.h"

#include "user_interface.h"
#include "dac_ui.h"

/**
 * @addtogroup MCSDK
 * @{
 */

/**
 * @addtogroup MCUI
 * @{
 */

/**
 * @addtogroup motor_control_protocol
 * @{
 */

 /**
  * @brief  Handle structure of motor control protocol component.
  *
  */
typedef struct MCP_Handle_s
{
  UI_Handle_t _Super;     /**< Handle structure of User Interface. */

  FCP_Handle_t *pFCP;
  FCP_SendFct_t fFcpSend;
  FCP_ReceiveFct_t fFcpReceive;
  FCP_AbortReceiveFct_t fFcpAbortReceive;
  uint8_t BufferFrame[FCP_MAX_PAYLOAD_SIZE];  /**< buffer containing data */
  const char *s_fwVer;    /**< String of FW version used */
  DAC_UI_Handle_t * pDAC; /**< Pointer on DAC handle structure. */
  uint8_t BufferSize;      /**< Frame buffer size */

} MCP_Handle_t;

/* Exported types ------------------------------------------------------------*/

/*  Function used to initialize and configure the motor control protocol Component */
void MCP_Init( MCP_Handle_t *pHandle,
               FCP_Handle_t * pFCP,
               FCP_SendFct_t fFcpSend,
               FCP_ReceiveFct_t fFcpReceive,
               FCP_AbortReceiveFct_t fFcpAbortReceive,
               DAC_UI_Handle_t * pDAC,
               const char* s_fwVer);
void MCP_OnTimeOut(MCP_Handle_t *pHandle);

/*  Function used for data decoding */
void MCP_ReceivedFrame(MCP_Handle_t *pHandle, uint8_t Code, uint8_t *buffer, uint8_t Size);

/*  Function used for data transmission */
void MCP_SentFrame(MCP_Handle_t *pHandle, uint8_t Code, uint8_t *buffer, uint8_t Size);

/*  Function used to check next reception frame. */
void MCP_WaitNextFrame(MCP_Handle_t *pHandle);

/*  Allow to report the overrun error message. */
void MCP_SendOverrunMessage(MCP_Handle_t *pHandle);

/*  Allow to report the time out error message. */
void MCP_SendTimeoutMessage(MCP_Handle_t *pHandle);

/*  Allow to send an ATR message. */
void MCP_SendATRMessage(MCP_Handle_t *pHandle);

/*  Allow to send back a BAD CRC message. */
void MCP_SendBadCRCMessage(MCP_Handle_t *pHandle);

/**
  * @}
  */

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

#endif /* __MOTOR_CONTROL_PROTOCOL_H */

/************************ (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
