/**
  ******************************************************************************
  * @file    frame_communication_protocol.h
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file contains all definitions and functions prototypes for the
  *          Frame Communication Protocol component of the Motor Control SDK.
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
#ifndef __FCP_H
#define __FCP_H

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */


#include "mc_type.h"

/** @addtogroup MCSDK
  * @{
  */

 /**
  * @addtogroup MCUI
  * @{
  */

/** @addtogroup FCP
  * @{
  */

/* Imported types (delayed definition). This type is needed to link with
 * the client using the Frame Control Protocol. As of this release, the
 * Motor Control Protocol is the only client. */
struct MCP_Handle_s;


/* Exported constants --------------------------------------------------------*/

/** @brief Size of the Header of an FCP frame */
#define FCP_HEADER_SIZE       2
/** @brief Maximum size of the payload of an FCP frame */
#define FCP_MAX_PAYLOAD_SIZE  128
/** @brief Size of the Control Sum (actually not a CRC) of an FCP frame */
#define FCP_CRC_SIZE          1
/** @brief Maximum size of an FCP frame, all inclusive */
#define FCP_MAX_FRAME_SIZE    (FCP_HEADER_SIZE + FCP_MAX_PAYLOAD_SIZE + FCP_CRC_SIZE)

#define FCP_CODE_ACK  0xf0
#define FCP_CODE_NACK 0xff

#define FCP_STATUS_TRANSFER_ONGOING  0x01
#define FCP_STATUS_WAITING_TRANSFER  0x02
#define FCP_STATUS_INVALID_PARAMETER 0x03
#define FCP_STATUS_TIME_OUT          0x04
#define FCP_STATUS_INVALID_FRAME     0x05


/**
 * @brief Error Code for an RX Timeout FCP message frame.
 *
 * Such a payload is sent to the remote when a reception timeout occurs
 * (too much time elapses between the reception of the start of a frame
 * and that of its end).
 *
 * The value of the error code is set by the Frame Communication Protocol and is taken in
 * a value space that is shared with the Motor Control Protocol. It thus cannot be
 * changed.
 */
#define FCP_MSG_RX_TIMEOUT  0x09
/**
 * @brief Error Code for an RX Bad CRC FCP message frame.
 *
 * Such a payload is sent to the remote on reception of a frame with a bad CRC.
 *
 * The value of the error code is set by the Frame Communication Protocol and is taken in
 * a value space that is shared with the Motor Control Protocol. It thus cannot be
 * changed.
 */
#define FCP_MSG_RX_BAD_CRC  0x0A

/* Exported types ------------------------------------------------------------*/
typedef enum FCP_FrameTransferState_e
{
  FCP_TRANSFER_IDLE,
  FCP_TRANSFER_ONGOING,
  FCP_TRANSFER_HEADER,
  FCP_TRANSFER_BUFFER,
  FCP_TRANSFER_CRC,
  FCP_TRANSFER_PENDING,
} FCP_FrameTransferState_t;

/**
 * @brief This structure contains and formats a Frame Communication Protocol's frame
 */
typedef struct FCP_Frame_s {
  uint8_t Code;                         /**< Identifier of the Frame. States the nature of the Frame. */
  uint8_t Size;                         /**< Size of the Payload of the frame in bytes. */
  uint8_t Buffer[FCP_MAX_PAYLOAD_SIZE]; /**< buffer containing the Payload of the frame. */
  uint8_t FrameCRC;                     /**< "CRC" of the Frame. Computed on the whole frame (Code,
                                             */
} FCP_Frame_t;

/** @brief Callback to invoke on Frame Transmission completion in order to notify the sender. */
typedef void (* FCP_SentFrameCallback_t)( struct MCP_Handle_s * o );
/** @brief Callback to invoke on Frame Reception completion in order to notify the receiver. */
typedef void (* FCP_ReceivedFrameCallback_t)( struct MCP_Handle_s * o, uint8_t Code, uint8_t *Buffer, uint8_t Size );
/** @brief Callback to invoke on a Reception Timeout -- Seems unused today */
typedef void (* FCP_RxTimeoutCallback_t)( struct MCP_Handle_s * o );

/**
 * @brief Frame Communication Protocol component handle structure
 */
typedef struct {
  struct MCP_Handle_s * ClientEntity;                       /**< A pointer on the protocol entity client */
  FCP_SentFrameCallback_t ClientFrameSentCallback;          /**< Client's callback function, executed when sending a frame has completed */
  FCP_ReceivedFrameCallback_t ClientFrameReceivedCallback;  /**< Client's callback function, executed when a frame has been received */
  FCP_RxTimeoutCallback_t ClientRxTimeoutCallback;          /**< Client's callback function, executed on a frame transmission time out */

  uint16_t RxTimeout;                     /**< Frame reception timeout. Currently unused */
  uint16_t RxTimeoutCountdown;            /**< Time remaining before a reception timeout occurs. Currently unused */

  FCP_Frame_t TxFrame;                    /**< Structure storing a frame to transmit */
  FCP_FrameTransferState_t TxFrameState;  /**< Transmission state of the frame to transmit */
  uint8_t TxFrameLevel;                   /**< Number of bytes already sent for the frame to transmit */

  FCP_Frame_t RxFrame;                    /**< Structure storing a frame being received */
  FCP_FrameTransferState_t RxFrameState;  /**< Reception of the frame being received */
  uint8_t RxFrameLevel;                   /**< Number of bytes already received for the frame to receive */
} FCP_Handle_t;

/**
 * @brief Prototype of a Start Receive function
 */
typedef uint8_t (*FCP_ReceiveFct_t)( FCP_Handle_t * pHandle );

/**
 * @brief Prototype of a Start Send function
 */
typedef uint8_t (*FCP_SendFct_t)( FCP_Handle_t * pHandle, uint8_t code, uint8_t *buffer, uint8_t size);

/**
 * @brief Prototype of an Abort Receive function
 */
typedef void (* FCP_AbortReceiveFct_t)( FCP_Handle_t * pHandle );

/* Exported functions ------------------------------------------------------- */

/* Initializes an FCP Component */
void FCP_Init( FCP_Handle_t * pHandle );
/*  */
void FCP_SetClient( FCP_Handle_t * pHandle,
                    struct MCP_Handle_s * pClient,
                    FCP_SentFrameCallback_t pSentFrameCb,
                    FCP_ReceivedFrameCallback_t pReceviedFrameCb,
                    FCP_RxTimeoutCallback_t pRxTimeoutCb );

void FCP_SetTimeout( FCP_Handle_t * pHandle, uint16_t Timeout );

uint8_t FCP_CalcCRC( FCP_Frame_t * pFrame );

uint8_t FCP_IsFrameValid( FCP_Frame_t * pFrame );

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

#endif /* __FCP_H */

/******************* (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
