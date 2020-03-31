/**
  ******************************************************************************
  * @file    state_machine.h
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file contains all definitions and functions prototypes for the
  *          Motor Control State Machine component of the Motor Control SDK.
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
  * @ingroup STATE_MACHINE
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STATEMACHINE_H
#define __STATEMACHINE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "mc_type.h"

/** @addtogroup MCSDK
  * @{
  */

/** @addtogroup STATE_MACHINE
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  State_t enum type definition, it lists all the possible state machine states
  */
typedef enum
{
  ICLWAIT = 12,         /*!< Persistent state, the system is waiting for ICL
                           deactivation. Is not possible to run the motor if
                           ICL is active. Until the ICL is active the state is
                           forced to ICLWAIT, when ICL become inactive the state
                           is moved to IDLE */
  IDLE = 0,             /*!< Persistent state, following state can be IDLE_START
                           if a start motor command has been given or
                           IDLE_ALIGNMENT if a start alignment command has been
                           given */
  IDLE_ALIGNMENT = 1,   /*!< "Pass-through" state containg the code to be executed
                           only once after encoder alignment command.
                           Next states can be ALIGN_CHARGE_BOOT_CAP or
                           ALIGN_OFFSET_CALIB according the configuration. It
                           can also be ANY_STOP if a stop motor command has been
                           given. */
  ALIGN_CHARGE_BOOT_CAP = 13,/*!< Persistent state where the gate driver boot
                           capacitors will be charged. Next states will be
                           ALIGN_OFFSET_CALIB. It can also be ANY_STOP if a stop
                           motor command has been given. */
  ALIGN_OFFSET_CALIB = 14,/*!< Persistent state where the offset of motor currents
                           measurements will be calibrated. Next state will be
                           ALIGN_CLEAR. It can also be ANY_STOP if a stop motor
                           command has been given. */
  ALIGN_CLEAR = 15,     /*!< "Pass-through" state in which object is cleared and
                           set for the startup.
                           Next state will be ALIGNMENT. It can also be ANY_STOP
                           if a stop motor command has been given. */
  ALIGNMENT = 2,        /*!< Persistent state in which the encoder are properly
                           aligned to set mechanical angle, following state can
                           only be ANY_STOP */
  IDLE_START = 3,       /*!< "Pass-through" state containg the code to be executed
                           only once after start motor command.
                           Next states can be CHARGE_BOOT_CAP or OFFSET_CALIB
                           according the configuration. It can also be ANY_STOP
                           if a stop motor command has been given. */
  CHARGE_BOOT_CAP = 16, /*!< Persistent state where the gate driver boot
                           capacitors will be charged. Next states will be
                           OFFSET_CALIB. It can also be ANY_STOP if a stop motor
                           command has been given. */
  OFFSET_CALIB = 17,    /*!< Persistent state where the offset of motor currents
                           measurements will be calibrated. Next state will be
                           CLEAR. It can also be ANY_STOP if a stop motor
                           command has been given. */
  CLEAR = 18,           /*!< "Pass-through" state in which object is cleared and
                           set for the startup.
                           Next state will be START. It can also be ANY_STOP if
                           a stop motor command has been given. */
  START = 4,            /*!< Persistent state where the motor start-up is intended
                           to be executed. The following state is normally
                           SWITCH_OVER or RUN as soon as first validated speed is
                           detected. Another possible following state is
                           ANY_STOP if a stop motor command has been executed */
  SWITCH_OVER = 19,     /**< TBD */
  START_RUN = 5,        /*!< "Pass-through" state, the code to be executed only
                           once between START and RUN states itâ€™s intended to be
                           here executed. Following state is normally  RUN but
                           it can also be ANY_STOP  if a stop motor command has
                           been given */
  RUN = 6,              /*!< Persistent state with running motor. The following
                           state is normally ANY_STOP when a stop motor command
                           has been executed */
  ANY_STOP = 7,         /*!< "Pass-through" state, the code to be executed only
                           once between any state and STOP itâ€™s intended to be
                           here executed. Following state is normally STOP */
  STOP = 8,             /*!< Persistent state. Following state is normally
                           STOP_IDLE as soon as conditions for moving state
                           machine are detected */
  STOP_IDLE = 9,        /*!< "Pass-through" state, the code to be executed only
                           once between STOP and IDLE itâ€™s intended to be here
                           executed. Following state is normally IDLE */
  FAULT_NOW = 10,       /*!< Persistent state, the state machine can be moved from
                           any condition directly to this state by
                           STM_FaultProcessing method. This method also manage
                           the passage to the only allowed following state that
                           is FAULT_OVER */
  FAULT_OVER = 11,       /*!< Persistent state where the application is intended to
                          stay when the fault conditions disappeared. Following
                          state is normally STOP_IDLE, state machine is moved as
                          soon as the user has acknowledged the fault condition.
                      */
  WAIT_STOP_MOTOR = 20

} State_t;

/**
  * @brief  StateMachine class members definition
  */
typedef struct
{
  State_t   bState;          /*!< Variable containing state machine current
                                    state */
  uint16_t  hFaultNow;       /*!< Bit fields variable containing faults
                                    currently present */
  uint16_t  hFaultOccurred;  /*!< Bit fields variable containing faults
                                    historically occurred since the state
                                    machine has been moved to FAULT_NOW state */
} STM_Handle_t;

/* Exported constants --------------------------------------------------------*/


/* Exported functions ------------------------------------------------------- */

/*  Initializes all the component variables. */
void STM_Init( STM_Handle_t * pHandle );

/* It submits the request for moving the state machine into the state bState */
bool STM_NextState( STM_Handle_t * pHandle, State_t bState );

/* It clocks both HW and SW faults processing and update the state machine accordingly */
State_t STM_FaultProcessing( STM_Handle_t * pHandle, uint16_t hSetErrors, uint16_t
                             hResetErrors );

/* Returns the current state machine state */
State_t STM_GetState( STM_Handle_t * pHandle );

/**
  * It reports to the state machine that the fault state has been
  *        acknowledged by the user.
  */
bool STM_FaultAcknowledged( STM_Handle_t * pHandle );

/**
  * It returns two 16 bit fields containing information about both faults
  * currently present and faults historically occurred since the state
  * machine has been moved into state
  */
uint32_t STM_GetFaultState( STM_Handle_t * pHandle );

/**
  * @}
  */

/**
  * @}
  */


#ifdef __cplusplus
}
#endif /* __cpluplus */

#endif /* __STATEMACHINE_H */

/******************* (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
