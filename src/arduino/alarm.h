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

#pragma once

/* Watchdog timeout in microseconds */
#define WATCHDOG_TIMEOUT 1000000

/**
 * Initialization of HardwareTimer for Alarm purpose
 */
void Alarm_Init();

/**
 * Generic function to activate an alarm.
 * Input parameters: alarm pattern array and its size
 */
void Alarm_Start(const uint32_t* Alarm, uint32_t Size);

/**
 * Activate an yellow alarm.
 */
void Alarm_Yellow_Start(void);

/**
 * Activate an red alarm.
 */
void Alarm_Red_Start(void);

/**
 * Activate boot bip.
 */
void Alarm_Boot_Start(void);

/**
 * Stop Alarm
 */
void Alarm_Stop(void);
