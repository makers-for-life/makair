/**
  ******************************************************************************
  * @file    ui_task.h
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   Interface of user interface tasks
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
#ifndef __UITASK_H
#define __UITASK_H

#include "user_interface.h"
#include "dac_rctimer_ui.h"
#include "dac_ui.h"

#include "motor_control_protocol.h"
#include "frame_communication_protocol.h"
#include "usart_frame_communication_protocol.h"
#include "ui_irq_handler.h"

/* Exported functions --------------------------------------------------------*/
void UI_TaskInit(uint32_t* pUICfg, uint8_t bMCNum, MCI_Handle_t * pMCIList[],
                 MCT_Handle_t* pMCTList[],const char* s_fwVer);

void UI_DACUpdate(uint8_t bMotorNbr);
UI_Handle_t * GetDAC(void);

void UI_Scheduler(void);
MCP_Handle_t * GetMCP(void);

bool UI_IdleTimeHasElapsed(void);
void UI_SetIdleTime(uint16_t SysTickCount);
bool UI_SerialCommunicationTimeOutHasElapsed(void);
bool UI_SerialCommunicationATRTimeHasElapsed(void);
void UI_SerialCommunicationTimeOutStop(void);
void UI_SerialCommunicationTimeOutStart(void);

void UI_HandleStartStopButton_cb (void);

#endif /* __UITASK_H */

/******************* (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
