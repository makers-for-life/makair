/**
  ******************************************************************************
  * @file    virtual_speed_sensor.h
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file contains all definitions and functions prototypes for the
  *          Virtual Speed Sensor component of the Motor Control SDK.
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
  * @ingroup VirtualSpeedSensor
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _VIRTUALSPEEDSENSOR_H
#define _VIRTUALSPEEDSENSOR_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "speed_pos_fdbk.h"
#ifdef FASTDIV
#include "fast_div.h"
#endif
/** @addtogroup MCSDK
  * @{
  */

/** @addtogroup SpeednPosFdbk
  * @{
  */

/** @addtogroup VirtualSpeedSensor
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/**
  * @brief  This structure is used to handle an instance of the Virtual Speed
  *         sensor component
  */
typedef struct
{
  SpeednPosFdbk_Handle_t   _Super;
  int32_t wElAccDppP32;   /*!< Delta electrical speed expressed in dpp per speed
                               sampling period to be appied each time is called
                               SPD_CalcAvrgMecSpeedUnit multiplied by scaling
                               factor of 65536.*/
  int32_t wElSpeedDpp32;  /*!< Electrical speed expressed in dpp multiplied by
                               scaling factor 65536.*/
  uint16_t hRemainingStep;/*!< Number of steps remaining to reach the final
                               speed.*/
  int16_t hFinalMecSpeedUnit;/*!< Backup of hFinalMecSpeedUnit to be applied in
                               the last step.*/
  bool bTransitionStarted;    /*!< Retaining information about Transition status.*/
  bool bTransitionEnded;      /*!< Retaining information about ransition status.*/
  int16_t hTransitionRemainingSteps;  /*!< Number of steps remaining to end
                               transition from CVSS_SPD to other CSPD*/
  int16_t hElAngleAccu;        /*!< Electrical angle accumulator*/
  bool bTransitionLocked;      /*!< Transition acceleration started*/
  bool bCopyObserver;          /*!< Command to set VSPD output same as state observer*/

  uint16_t hSpeedSamplingFreqHz; /*!< Frequency (Hz) at which motor speed is to
                             be computed. It must be equal to the frequency
                             at which function SPD_CalcAvrgMecSpeedUnit
                             is called.*/
  int16_t hTransitionSteps; /*< Number of steps to perform the transition phase
                             from CVSS_SPD to other CSPD; if the Transition PHase
                             should last TPH milliseconds, and the FOC Execution
                             Frequency is set to FEF kHz, then
                             hTransitionSteps = TPH * FEF*/
#ifdef FASTDIV
  /* (Fast division optimization for cortex-M0 micros)*/
  FastDiv_Handle_t fd;                       /*!< Fast division obj.*/
#endif

} VirtualSpeedSensor_Handle_t;

/* It initializes the Virtual Speed Sensor component */
void VSS_Init( VirtualSpeedSensor_Handle_t * pHandle );

/* It clears Virtual Speed Sensor by re-initializing private variables*/
void VSS_Clear( VirtualSpeedSensor_Handle_t * pHandle );

/* It compute a theorical speed and update the theorical electrical angle. */
int16_t VSS_CalcElAngle( VirtualSpeedSensor_Handle_t * pHandle, void * pInputVars_str );

/* Computes the rotor average theoretical mechanical speed in the unit defined by SPEED_UNIT and returns it in pMecSpeedUnit. */
bool VSS_CalcAvrgMecSpeedUnit( VirtualSpeedSensor_Handle_t * pHandle, int16_t * hMecSpeedUnit );

/* It set istantaneous information on VSS mechanical and  electrical angle.*/
void VSS_SetMecAngle( VirtualSpeedSensor_Handle_t * pHandle, int16_t hMecAngle );

/* Set the mechanical acceleration of virtual sensor. */
void  VSS_SetMecAcceleration( VirtualSpeedSensor_Handle_t * pHandle, int16_t  hFinalMecSpeedUnit,
                              uint16_t hDurationms );
/* Checks if the ramp executed after a VSPD_SetMecAcceleration command has been completed*/
bool VSS_RampCompleted( VirtualSpeedSensor_Handle_t * pHandle );

/* Get the final speed of last setled ramp of virtual sensor expressed in 0.1Hz*/
int16_t  VSS_GetLastRampFinalSpeed( VirtualSpeedSensor_Handle_t * pHandle );

/* Set the command to Start the transition phase from VirtualSpeedSensor to other SpeedSensor.*/
bool VSS_SetStartTransition( VirtualSpeedSensor_Handle_t * pHandle, bool bCommand );

/* Return the status of the transition phase.*/
bool VSS_IsTransitionOngoing( VirtualSpeedSensor_Handle_t * pHandle );

bool VSS_TransitionEnded( VirtualSpeedSensor_Handle_t * pHandle );

/* It set istantaneous information on rotor electrical angle copied by state observer */
void VSS_SetCopyObserver( VirtualSpeedSensor_Handle_t * pHandle );

/* It  set istantaneous information on rotor electrical angle */
void VSS_SetElAngle( VirtualSpeedSensor_Handle_t * pHandle, int16_t hElAngle );

/** @} */
/** @} */
/** @} */

#ifdef __cplusplus
}
#endif /* __cpluplus */

#endif /* _VIRTUALSPEEDSENSOR_H */

/************************ (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
