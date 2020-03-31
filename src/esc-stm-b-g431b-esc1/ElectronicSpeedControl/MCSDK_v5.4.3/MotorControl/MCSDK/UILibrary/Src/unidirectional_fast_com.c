/**
  ******************************************************************************
  * @file    unidirectional_fast_com.c
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file provides firmware functions that implement the following features
  *          of the unidirectional_fast_com_f3xx_user_interface component of the Motor Control SDK:
  *           < Add below the list of features, one per line.>
  *           +
  *           +
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
#include "user_interface.h"
#include "unidirectional_fast_com.h"
#include "mc_type.h"
#include "ui_irq_handler.h"

/* Private function prototypes -----------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/** @addtogroup MCSDK
  * @{
  */

/**
 * @addtogroup MCUI
 * @{
 */

/** @defgroup UnidirectionalFastCom Unidirectional Fast Communication
  * @brief Unidirectional Fast Serial Communication component of the Motor Control SDK
  *
  * @todo Complete documentation
  * @{
  */

/**
  * @brief  Initializes a UnidirectionalFastCom component Handle
  *
  * It initialize all vars.
  *
  * @param  pHandle Pointer on a UDFastCom_Handle_t structure to initialize
  */
__weak void UFC_Init( UDFastCom_Handle_t *pHandle, UDFastCom_Params_t *pParams )
{
  /* Init Hardware. */
  pHandle->Hw = *pParams;

  /* Init Vars */
  pHandle->_Super.bSelectedDrive = pHandle->Hw.bDefMotor;
  pHandle->bChannel[0]           = pHandle->Hw.bDefChannel1;
  pHandle->bChannel[1]           = pHandle->Hw.bDefChannel2;
  pHandle->bChByteNum[0]         = pHandle->Hw.bCh1ByteNum;
  pHandle->bChByteNum[1]         = pHandle->Hw.bCh2ByteNum;
  pHandle->comON                 = false;
  pHandle->bChTransmitted        = 0;
  pHandle->bByteTransmitted      = 0;
  pHandle->bChNum                = pHandle->Hw.bChNum ;
 
  /* Enable the USART */
  LL_USART_Enable(pHandle->Hw.USARTx);
}

/*******************************************************************************
* Function Name  : USART_IRQ_Handler
* Description    : Interrupt function for the serial communication
* Input          : none
* Return         : none
*******************************************************************************/
__weak void UFC_TX_IRQ_Handler(UDFastCom_Handle_t *pHandle)
{
  uint8_t txData = 0;
  uint8_t* pBuff;

  if (pHandle->comON)
  {
    if (pHandle->bByteTransmitted == 0)
    {
      /* First byte to be transmitted, read value and populate the buffer */
      pHandle->wBuffer = UI_GetReg(&pHandle->_Super, pHandle->bChannel[pHandle->bChTransmitted],MC_NULL) >> 8;
    }

    pBuff = (uint8_t*)(&(pHandle->wBuffer));
    txData = pBuff[pHandle->bByteTransmitted];

    /* Write one byte to the transmit data register */
    LL_USART_TransmitData8(pHandle->Hw.USARTx, txData);

    pHandle->bByteTransmitted++;
    if (pHandle->bByteTransmitted == pHandle->bChByteNum[pHandle->bChTransmitted])
    {
      pHandle->bByteTransmitted = 0;
      pHandle->bChTransmitted++;
      if (pHandle->bChTransmitted == pHandle->Hw.bChNum)
      {
        pHandle->bChTransmitted = 0;
      }
    }
  }
}

/**
  * @brief  Starts the fast unidirectional communication.
  * @param  pHandle Handle on the target unidirectional fast communication instance
  */
__weak void UFC_StartCom( UDFastCom_Handle_t *pHandle )
{
  pHandle->comON = true;
  LL_USART_TransmitData8(pHandle->Hw.USARTx, ' ');
  /* Enable USART Transmit interrupts */
  LL_USART_EnableIT_TXE(pHandle->Hw.USARTx);
}

/**
  * @brief  Stops the fast unidirectional communication.
  * @param  pHandle Handle on the target unidirectional fast communication instance
  */
__weak void UFC_StopCom(UDFastCom_Handle_t *pHandle)
{
  pHandle->comON = false;
  /* Disable USART Transmit interrupts */
  LL_USART_DisableIT_TXE(pHandle->Hw.USARTx);
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

/**
 * @}
 */

/************************ (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
