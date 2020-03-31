/**
  ******************************************************************************
  * @file    state_machine.c
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file provides firmware functions that implement the features
  *          of the Motor Control State Machine component of the Motor Control SDK:
  *
  *           * Check that transition from one state to another is legal
  *           * Handle the fault processing
  *           * Provide accessor to State machine internal state
  *           * Provide accessor to error state
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
#include "state_machine.h"

/** @addtogroup MCSDK
  * @{
  */

/** @defgroup STATE_MACHINE Motor Control State Machine
  * @brief Motor Control State Machine component of the Motor Control SDK
  *
  * @todo Document the Motor Control State Machine "module".
  *
  * @{
  */

/**
  * @brief  Initializes all the object variables, usually it has to be called
  *         once right after object creation.
  * @param pHandle pointer on the component instance to initialize.
  * @retval none.
  */
__weak void STM_Init( STM_Handle_t * pHandle )
{

  pHandle->bState = IDLE;
  pHandle->hFaultNow = MC_NO_FAULTS;
  pHandle->hFaultOccurred = MC_NO_FAULTS;
}

/**
  * @brief It submits the request for moving the state machine into the state
  *        specified by bState (FAULT_NOW and FAUL_OVER are not handled by this
  *        method). Accordingly with the current state, the command is really
  *        executed (state machine set to bState) or discarded (no state
  *        changes).
  *        If requested state can't be reached the return value is false and the
  *        MC_SW_ERROR is raised, but if requested state is IDLE_START,
  *        IDLE_ALIGNMENT or ANY_STOP, that corresponds with the user actions:
  *        Start Motor, Encoder Alignemnt and Stop Motor, the MC_SW_ERROR is
  *        not raised.
  * @param pHanlde pointer of type  STM_Handle_t.
  * @param bState New requested state
  * @retval bool It returns true if the state has been really set equal to
  *         bState, false if the requested state can't be reached
  */
__weak bool STM_NextState( STM_Handle_t * pHandle, State_t bState )
{
  bool bChangeState = false;
  State_t bCurrentState = pHandle->bState;
  State_t bNewState = bCurrentState;

  switch ( bCurrentState )
  {
    case ICLWAIT:
      if ( bState == IDLE )
      {
        bNewState = bState;
        bChangeState = true;
      }
      break;
    case IDLE:
      if ( ( bState == IDLE_START ) || ( bState == IDLE_ALIGNMENT )
           || ( bState == ICLWAIT ) )
      {
        bNewState = bState;
        bChangeState = true;
      }
      break;

    case IDLE_ALIGNMENT:
      if ( ( bState == ANY_STOP ) || ( bState == ALIGN_CHARGE_BOOT_CAP )
           || ( bState == ALIGN_OFFSET_CALIB ) )
      {
        bNewState = bState;
        bChangeState = true;
      }
      break;

    case ALIGN_CHARGE_BOOT_CAP:
      if ( ( bState == ALIGN_OFFSET_CALIB ) || ( bState == ANY_STOP ) )
      {
        bNewState = bState;
        bChangeState = true;
      }
      break;

    case ALIGN_OFFSET_CALIB:
      if ( ( bState == ALIGN_CLEAR ) || ( bState == ANY_STOP ) )
      {
        bNewState = bState;
        bChangeState = true;
      }
      break;

    case ALIGN_CLEAR:
      if ( ( bState == ALIGNMENT ) || ( bState == ANY_STOP ) )
      {
        bNewState = bState;
        bChangeState = true;
      }
      break;

    case ALIGNMENT:
      if ( bState == ANY_STOP )
      {
        bNewState = bState;
        bChangeState = true;
      }
      break;

    case IDLE_START:
      if ( ( bState == ANY_STOP ) || ( bState == CHARGE_BOOT_CAP ) ||
           ( bState == START ) ||
           ( bState == OFFSET_CALIB ) || ( bState == IDLE_ALIGNMENT ) )
      {
        bNewState = bState;
        bChangeState = true;
      }
      break;

    case CHARGE_BOOT_CAP:
      if ( ( bState == OFFSET_CALIB ) || ( bState == ANY_STOP ) )
      {
        bNewState = bState;
        bChangeState = true;
      }
      break;

    case OFFSET_CALIB:
      if ( ( bState == CLEAR ) || ( bState == ANY_STOP ) || ( bState == WAIT_STOP_MOTOR ) )
      {
        bNewState = bState;
        bChangeState = true;
      }
      break;

     case WAIT_STOP_MOTOR:
      if ( ( bState == CLEAR ) || ( bState == ANY_STOP ) )
      {
        bNewState = bState;
        bChangeState = true;
      }
      break;

    case CLEAR:
      if ( ( bState == START ) || ( bState == ANY_STOP ) )
      {
        bNewState = bState;
        bChangeState = true;
      }
      break;

    case START:
      if ( ( bState == SWITCH_OVER ) || ( bState == ANY_STOP ) || (bState == START_RUN) )
      {
        bNewState = bState;
        bChangeState = true;
      }
      break;

    case SWITCH_OVER:
      if ( ( bState == START ) || ( bState == ANY_STOP ) || (bState == START_RUN) )
      {
        bNewState = bState;
        bChangeState = true;
      }
      break;

    case START_RUN:
      if ( ( bState == RUN ) || ( bState == ANY_STOP ) )
      {
        bNewState = bState;
        bChangeState = true;
      }
      break;

    case RUN:
      if ( bState == ANY_STOP )
      {
        bNewState = bState;
        bChangeState = true;
      }
      break;

    case ANY_STOP:
      if ( bState == STOP )
      {
        bNewState = bState;
        bChangeState = true;
      }
      break;

    case STOP:
      if ( bState == STOP_IDLE )
      {
        bNewState = bState;
        bChangeState = true;
      }
      break;

    case STOP_IDLE:
      if ( ( bState == IDLE ) || ( bState == ICLWAIT ) )
      {
        bNewState = bState;
        bChangeState = true;
      }
      break;
    default:
      break;
  }

  if ( bChangeState )
  {
    pHandle->bState = bNewState;
  }
  else
  {
    if ( !( ( bState == IDLE_START ) || ( bState == IDLE_ALIGNMENT )
            || ( bState == ANY_STOP ) ) )
    {
      /* If new state is not a user command START/STOP raise a software error */
      STM_FaultProcessing( pHandle, MC_SW_ERROR, 0u );
    }
  }

  return ( bChangeState );
}

/**
  * @brief It clocks both HW and SW faults processing and update the state
  *        machine accordingly with hSetErrors, hResetErrors and present state.
  *        Refer to State_t description for more information about fault states.
  * @param pHanlde pointer of type  STM_Handle_t
  * @param hSetErrors Bit field reporting faults currently present
  * @param hResetErrors Bit field reporting faults to be cleared
  * @retval State_t New state machine state after fault processing
  */
__weak State_t STM_FaultProcessing( STM_Handle_t * pHandle, uint16_t hSetErrors, uint16_t
                             hResetErrors )
{
  State_t LocalState =  pHandle->bState;

  /* Set current errors */
  pHandle->hFaultNow = ( pHandle->hFaultNow | hSetErrors ) & ( ~hResetErrors );
  pHandle->hFaultOccurred |= hSetErrors;

  if ( LocalState == FAULT_NOW )
  {
    if ( pHandle->hFaultNow == MC_NO_FAULTS )
    {
      pHandle->bState = FAULT_OVER;
      LocalState = FAULT_OVER;
    }
  }
  else
  {
    if ( pHandle->hFaultNow != MC_NO_FAULTS )
    {
      pHandle->bState = FAULT_NOW;
      LocalState = FAULT_NOW;
    }
  }

  return ( LocalState );
}

#if defined (CCMRAM)
#if defined (__ICCARM__)
#pragma location = ".ccmram"
#elif defined (__CC_ARM) || defined(__GNUC__)
__attribute__( ( section ( ".ccmram" ) ) )
#endif
#endif
/**
  * @brief  Returns the current state machine state
  * @param  pHanlde pointer of type  STM_Handle_t
  * @retval State_t Current state machine state
  */
__weak State_t STM_GetState( STM_Handle_t * pHandle )
{
  return ( pHandle->bState );
}


/**
  * @brief It reports to the state machine that the fault state has been
  *        acknowledged by the user. If the state machine is in FAULT_OVER state
  *        then it is moved into STOP_IDLE and the bit field variable containing
  *        information about the faults historically occured is cleared.
  *        The method call is discarded if the state machine is not in FAULT_OVER
  * @param pHanlde pointer of type  STM_Handle_t
  * @retval bool true if the state machine has been moved to IDLE, false if the
  *        method call had no effects
  */
__weak bool STM_FaultAcknowledged( STM_Handle_t * pHandle )
{
  bool bToBeReturned = false;

  if ( pHandle->bState == FAULT_OVER )
  {
    pHandle->bState = STOP_IDLE;
    pHandle->hFaultOccurred = MC_NO_FAULTS;
    bToBeReturned = true;
  }

  return ( bToBeReturned );
}


/**
  * @brief It returns two 16 bit fields containing information about both faults
  *        currently present and faults historically occurred since the state
  *        machine has been moved into state
  * @param pHanlde pointer of type  STM_Handle_t.
  * @retval uint32_t  Two 16 bit fields: in the most significant half are stored
  *         the information about currently present faults. In the least
  *         significant half are stored the information about the faults
  *         historically occurred since the state machine has been moved into
  *         FAULT_NOW state
  */
__weak uint32_t STM_GetFaultState( STM_Handle_t * pHandle )
{
  uint32_t LocalFaultState;

  LocalFaultState = ( uint32_t )( pHandle->hFaultOccurred );
  LocalFaultState |= ( uint32_t )( pHandle->hFaultNow ) << 16;

  return LocalFaultState;
}

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
