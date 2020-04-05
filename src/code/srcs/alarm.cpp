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

/// Externals

#include "Arduino.h"

/// Internals

#include "../includes/alarm.h"
#include "../includes/parameters.h"

/* Definition of all bip duration */
/* 4 TICK = 1 ms  ( not possible to have 1TICK = 1millisec because prescalor is 16bit anf input
frequency eitehr 84 or 100 MHz*/
#define TIMER_TICK_PER_MS 4
#define BIP (100 * TIMER_TICK_PER_MS)
#define BIP_PAUSE BIP
#define BEEEEP (250 * TIMER_TICK_PER_MS)
#define BEEEEP_PAUSE BEEEEP
#define PAUSE_20S (20 * 1000 * TIMER_TICK_PER_MS)
#define PAUSE_10S (10 * 1000 * TIMER_TICK_PER_MS)
#define PAUSE_1S (1 * 1000 * TIMER_TICK_PER_MS)

/* Yellow Alarm pattern size */
#define ALARM_YELLOW_SIZE 8

/* Yellow alarm pattern definition, composed of multiple couple of states (Actif/Inactif) and
 * duration (miliseconds) */
const uint32_t Alarm_Yellow[ALARM_YELLOW_SIZE] = {
    TIMER_OUTPUT_COMPARE_FORCED_ACTIVE, BEEEEP, TIMER_OUTPUT_COMPARE_FORCED_INACTIVE, BEEEEP_PAUSE,
    TIMER_OUTPUT_COMPARE_FORCED_ACTIVE, BEEEEP, TIMER_OUTPUT_COMPARE_FORCED_INACTIVE, PAUSE_20S};

/* Red Alarm pattern size */
#define ALARM_RED_SIZE 32

/* Red alarm pattern definition, composed of multiple couple of states (Actif/Inactif) and duration
 * (miliseconds) */
const uint32_t Alarm_Red[ALARM_RED_SIZE] = {
    TIMER_OUTPUT_COMPARE_FORCED_ACTIVE, BIP,    TIMER_OUTPUT_COMPARE_FORCED_INACTIVE, BIP_PAUSE,
    TIMER_OUTPUT_COMPARE_FORCED_ACTIVE, BIP,    TIMER_OUTPUT_COMPARE_FORCED_INACTIVE, BIP_PAUSE,
    TIMER_OUTPUT_COMPARE_FORCED_ACTIVE, BIP,    TIMER_OUTPUT_COMPARE_FORCED_INACTIVE, BIP_PAUSE,
    TIMER_OUTPUT_COMPARE_FORCED_ACTIVE, BEEEEP, TIMER_OUTPUT_COMPARE_FORCED_INACTIVE, PAUSE_1S,
    TIMER_OUTPUT_COMPARE_FORCED_ACTIVE, BIP,    TIMER_OUTPUT_COMPARE_FORCED_INACTIVE, BIP_PAUSE,
    TIMER_OUTPUT_COMPARE_FORCED_ACTIVE, BIP,    TIMER_OUTPUT_COMPARE_FORCED_INACTIVE, BIP_PAUSE,
    TIMER_OUTPUT_COMPARE_FORCED_ACTIVE, BIP,    TIMER_OUTPUT_COMPARE_FORCED_INACTIVE, BIP_PAUSE,
    TIMER_OUTPUT_COMPARE_FORCED_ACTIVE, BEEEEP, TIMER_OUTPUT_COMPARE_FORCED_INACTIVE, PAUSE_10S};

/* Boot Alarm pattern size */
#define ALARM_BOOT_SIZE 4

/* Yellow alarm pattern definition, composed of multiple couple of states (Actif/Inactif) and
 * duration (miliseconds) */
const uint32_t Alarm_Boot[ALARM_BOOT_SIZE] = {TIMER_OUTPUT_COMPARE_FORCED_ACTIVE, BEEEEP,
                                              TIMER_OUTPUT_COMPARE_FORCED_INACTIVE, BEEEEP_PAUSE};

// INITIALISATION =============================================================

/* variables definition */
const uint32_t* Active_Alarm = nullptr;
uint32_t Active_Alarm_Index = 0;
uint32_t Active_Alarm_Size = 2;

HardwareTimer* AlarmTim;
uint32_t AlarmTimchannel;

/**
 * When timer period expires, switch to next state in the pattern of the alarm
 * Not: API update since version 1.9.0 of Arduino_Core_STM32
 */
#if (STM32_CORE_VERSION < 0x01090000)
void Update_IT_callback(HardwareTimer*)  // NOLINT(readability/casting)
#else
void Update_IT_callback(void)
#endif
{
    /* patterns are composed of multiple couple of states (Actif/Inactif) and duration
     * (miliseconds)*/
    /* Previous state is finished, switch to next one */
    AlarmTim->setMode(AlarmTimchannel, (TimerModes_t)Active_Alarm[Active_Alarm_Index], PIN_ALARM);
    AlarmTim->setOverflow(Active_Alarm[Active_Alarm_Index + 1], TICK_FORMAT);
    Active_Alarm_Index = (Active_Alarm_Index + 2) % Active_Alarm_Size;
    AlarmTim->resume();
}

/**
 * Initialization of HardwareTimer for Alarm purpose
 */
void Alarm_Init() {
    // Automatically retrieve timer instance and channel associated with the pin
    // Useful in case of board change
    TIM_TypeDef* Instance = reinterpret_cast<TIM_TypeDef*>(
        pinmap_peripheral(digitalPinToPinName(PIN_ALARM), PinMap_PWM));
    AlarmTimchannel = STM_PIN_CHANNEL(pinmap_function(digitalPinToPinName(PIN_ALARM), PinMap_PWM));

    // Alarm HardwareTimer object creation
    AlarmTim = new HardwareTimer(Instance);

    AlarmTim->setMode(AlarmTimchannel, TIMER_OUTPUT_COMPARE_FORCED_INACTIVE, PIN_ALARM);
    /* 4 TICK = 1 ms  ( not possible to have 1TICK = 1millisec because prescalor is 16bit anf input
     * frequency eitehr 84 or 100 MHz*/
    /* Use of TICK format o avoid computation within interrupt handler */
    AlarmTim->setPrescaleFactor(AlarmTim->getTimerClkFreq() / (TIMER_TICK_PER_MS * 1000));
    AlarmTim->setOverflow(100 * TIMER_TICK_PER_MS, TICK_FORMAT);  // Default 100milisecondes

    /* Start with inactive state without interruptions */
    AlarmTim->resume();
}

/**
 * Generic function to activate an alarm.
 * Input parameters: alarm pattern array and its size
 */
void Alarm_Start(const uint32_t* Alarm, uint32_t Size) {
    AlarmTim->setCount(0);
    Active_Alarm = Alarm;
    Active_Alarm_Index = 0;
    Active_Alarm_Size = Size;

    /* patterns are composed of multiple couple of states (Actif/Inactif) and duration (miliseconds)
     */
    /* Configuration of first etat of pattern */
    AlarmTim->setMode(AlarmTimchannel, (TimerModes_t)Active_Alarm[Active_Alarm_Index], PIN_ALARM);
    AlarmTim->setOverflow(Active_Alarm[Active_Alarm_Index + 1], TICK_FORMAT);

    /* Activate interrupt callback to handle further states */
    AlarmTim->attachInterrupt(Update_IT_callback);
    Active_Alarm_Index = (Active_Alarm_Index + 2) % Active_Alarm_Size;

    /* Time stars. Required to configure output on GPIO */
    AlarmTim->resume();
}

/**
 * Activate an yellow alarm.
 * Input parameters: alarm pattern array and its size
 */
void Alarm_Yellow_Start(void) { Alarm_Start(Alarm_Yellow, ALARM_YELLOW_SIZE); }

/**
 * Activate an red alarm.
 * Input parameters: alarm pattern array and its size
 */
void Alarm_Red_Start(void) { Alarm_Start(Alarm_Red, ALARM_RED_SIZE); }

/**
 * Activate boot bip.
 * Input parameters: alarm pattern array and its size
 */
void Alarm_Boot_Start(void) { Alarm_Start(Alarm_Boot, ALARM_BOOT_SIZE); }

/**
 * Stop Alarm
 */
void Alarm_Stop(void) { AlarmTim->pause(); }
