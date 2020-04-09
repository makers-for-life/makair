/*
    Copyright (C) 2020 Makers For Life
*/
/******************************************************************************
 * @author Makers For Life
 * @file buzzer.h
 * @brief Buzzer related functions
 *****************************************************************************/

#pragma once

/// Watchdog timeout in microseconds
#define WATCHDOG_TIMEOUT 1000000

/// Initialization of HardwareTimer for buzzer
void Buzzer_Init();

/**
 * Generic function to activate a buzzer
 *
 * @param Buzzer Buzzer pattern array
 * @param Size Size of the buzzer pattern array
 * @param RepeatBuzzer Is pattern repeating after its end
 */
void Buzzer_Start(const uint32_t* Buzzer, uint32_t Size, bool RepeatBuzzer);

/// Activate the buzzer pattern for low priority alarms
void Buzzer_Low_Prio_Start(void);

/// Activate the buzzer pattern for medium priority alarms
void Buzzer_Medium_Prio_Start(void);

/// Activate the buzzer pattern for high priority alarms
void Buzzer_High_Prio_Start(void);

/// Activate boot bip
void Buzzer_Boot_Start(void);

/// Mute the buzzer for 120s
void Buzzer_Mute(void);

/// Resume the muted alarm
void Buzzer_Resume(void);

/// Stop Buzzer
void Buzzer_Stop(void);
