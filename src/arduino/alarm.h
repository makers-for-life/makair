/*=============================================================================
 * @file alarm.cpp
 *
 * COVID Respirator
 *
 * @section copyright Copyright
 *
 * Makers For Life
 *
 * @section descr File description
 *
 * This file is the implementation of Alarms
 */

/**
  * Initialization of HardwareTimer for Alarm purpose
  */
void Alarm_Init();

/**
  * Generic function to activate an alarm.
  * Input parameters: alarm pattern array and its size
  */
void Alarm_Start(const uint32_t *Alarm, uint32_t Size);

/**
  * Activate an yellow alarm.
  * Input parameters: alarm pattern array and its size
  */
void Alarm_yellow_Start(void);

/**
  * Activate boot bip.
  * Input parameters: alarm pattern array and its size
  */
void Alarm_Boot_Start(void);

/**
  * Stop Alarm
  */
void Alarm_Stop(void);