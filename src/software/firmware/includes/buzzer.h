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
 * @param Size of the buzzer pattern array
 */
void Buzzer_Start(const uint32_t* Buzzer, uint32_t Size);

/// Activate a medium buzzer pattern
void Buzzer_Medium_Start(void);

/// Activate a very short buzzer pattern
void Buzzer_Short_Start(void);

/// Activate a long buzzer pattern
void Buzzer_Long_Start(void);

/// Activate boot bip
void Buzzer_Boot_Start(void);

/// Stop Buzzer
void Buzzer_Stop(void);
