/*
    Copyright (C) 2020 Makers For Life
*/
/******************************************************************************
 * @author Makers For Life
 * @file buzzer.cpp
 * @brief Buzzer related functions
 *****************************************************************************/

#pragma once

// INCLUDES ===================================================================

// Externals
#include "Arduino.h"

/// Internals

#include "../includes/buzzer.h"
#include "../includes/parameters.h"

// PROGRAM =====================================================================

/**
 * @name Definition of bips durations
 * 4 ticks = 1 ms
 * @warning it is not possible to have 1 tick = 1 ms because prescaler is 16 bits and input
 * frequency either 84 or 100 MHz
 */
///@{
#define TIMER_TICK_PER_MS 4
#define BIP (100 * TIMER_TICK_PER_MS)
#define BIP_PAUSE BIP
#define BEEEEP (250 * TIMER_TICK_PER_MS)
#define BEEEEP_PAUSE BEEEEP
#define PAUSE_20S (20 * 1000 * TIMER_TICK_PER_MS)
#define PAUSE_10S (10 * 1000 * TIMER_TICK_PER_MS)
#define PAUSE_1S (1 * 1000 * TIMER_TICK_PER_MS)
///@}

/// Short buzzer pattern size
#define BUZZER_SHORT_SIZE 8

/// Short buzzer pattern definition, composed of multiple couple of states (Actif/Inactif) and
/// duration (miliseconds)
const uint32_t Buzzer_Short[BUZZER_SHORT_SIZE] = {
    TIMER_OUTPUT_COMPARE_FORCED_ACTIVE, BIP, TIMER_OUTPUT_COMPARE_FORCED_INACTIVE, BIP_PAUSE,
    TIMER_OUTPUT_COMPARE_FORCED_ACTIVE, BIP, TIMER_OUTPUT_COMPARE_FORCED_INACTIVE, PAUSE_20S};

/// Medium buzzer pattern size
#define BUZZER_MEDIUM_SIZE 8

/// Medium buzzer pattern definition, composed of multiple couple of states (Actif/Inactif) and
/// duration (miliseconds)
const uint32_t Buzzer_Medium[BUZZER_MEDIUM_SIZE] = {
    TIMER_OUTPUT_COMPARE_FORCED_ACTIVE, BEEEEP, TIMER_OUTPUT_COMPARE_FORCED_INACTIVE, BEEEEP_PAUSE,
    TIMER_OUTPUT_COMPARE_FORCED_ACTIVE, BEEEEP, TIMER_OUTPUT_COMPARE_FORCED_INACTIVE, PAUSE_20S};

/// Long buzzer pattern size
#define BUZZER_LONG_SIZE 32

/// Long buzzer pattern definition, composed of multiple couple of states (Actif/Inactif) and
/// duration (miliseconds)
const uint32_t Buzzer_Long[BUZZER_LONG_SIZE] = {
    TIMER_OUTPUT_COMPARE_FORCED_ACTIVE, BIP,    TIMER_OUTPUT_COMPARE_FORCED_INACTIVE, BIP_PAUSE,
    TIMER_OUTPUT_COMPARE_FORCED_ACTIVE, BIP,    TIMER_OUTPUT_COMPARE_FORCED_INACTIVE, BIP_PAUSE,
    TIMER_OUTPUT_COMPARE_FORCED_ACTIVE, BIP,    TIMER_OUTPUT_COMPARE_FORCED_INACTIVE, BIP_PAUSE,
    TIMER_OUTPUT_COMPARE_FORCED_ACTIVE, BEEEEP, TIMER_OUTPUT_COMPARE_FORCED_INACTIVE, PAUSE_1S,
    TIMER_OUTPUT_COMPARE_FORCED_ACTIVE, BIP,    TIMER_OUTPUT_COMPARE_FORCED_INACTIVE, BIP_PAUSE,
    TIMER_OUTPUT_COMPARE_FORCED_ACTIVE, BIP,    TIMER_OUTPUT_COMPARE_FORCED_INACTIVE, BIP_PAUSE,
    TIMER_OUTPUT_COMPARE_FORCED_ACTIVE, BIP,    TIMER_OUTPUT_COMPARE_FORCED_INACTIVE, BIP_PAUSE,
    TIMER_OUTPUT_COMPARE_FORCED_ACTIVE, BEEEEP, TIMER_OUTPUT_COMPARE_FORCED_INACTIVE, PAUSE_10S};

/// Boot buzzer pattern size
#define BUZZER_BOOT_SIZE 4

/// Medium buzzer pattern definition, composed of multiple couple of states (Actif/Inactif) and
/// duration (miliseconds)
const uint32_t Buzzer_Boot[BUZZER_BOOT_SIZE] = {TIMER_OUTPUT_COMPARE_FORCED_ACTIVE, BEEEEP,
                                                TIMER_OUTPUT_COMPARE_FORCED_INACTIVE, BEEEEP_PAUSE};

// INITIALISATION =============================================================

const uint32_t* Active_Buzzer = nullptr;
uint32_t Active_Buzzer_Index = 0;
uint32_t Active_Buzzer_Size = 2;

HardwareTimer* BuzzerTim;
uint32_t BuzzerTimerChannel;

/**
 * When timer period expires, switch to next state in the pattern of the buzzer
 * @note API update since version 1.9.0 of Arduino_Core_STM32
 */
#if (STM32_CORE_VERSION < 0x01090000)
void Update_IT_callback(HardwareTimer*)  // NOLINT(readability/casting)
#else
void Update_IT_callback(void)
#endif
{
    // Patterns are composed of multiple couple of states (Actif/Inactif) and duration (miliseconds)
    // Previous state is finished, switch to next one
    BuzzerTim->setMode(BuzzerTimerChannel, (TimerModes_t)Active_Buzzer[Active_Buzzer_Index],
                       PIN_BUZZER);
    BuzzerTim->setOverflow(Active_Buzzer[Active_Buzzer_Index + 1u], TICK_FORMAT);
    Active_Buzzer_Index = (Active_Buzzer_Index + 2u) % Active_Buzzer_Size;
    BuzzerTim->resume();
}

void Buzzer_Init() {
    // Automatically retrieve timer instance and channel associated with the pin
    // Useful in case of board change
    TIM_TypeDef* Instance = reinterpret_cast<TIM_TypeDef*>(
        pinmap_peripheral(digitalPinToPinName(PIN_BUZZER), PinMap_PWM));
    BuzzerTimerChannel =
        STM_PIN_CHANNEL(pinmap_function(digitalPinToPinName(PIN_BUZZER), PinMap_PWM));

    // Buzzer HardwareTimer object creation
    BuzzerTim = new HardwareTimer(Instance);

    BuzzerTim->setMode(BuzzerTimerChannel, TIMER_OUTPUT_COMPARE_FORCED_INACTIVE, PIN_BUZZER);
    // 4 ticks = 1 ms  (it is not possible to have 1 tick = 1 ms because prescaler is 16 bit and
    // input frequency either 84 or 100 MHz Use of tick format to avoid computation within interrupt
    // handler
    BuzzerTim->setPrescaleFactor(BuzzerTim->getTimerClkFreq() / (TIMER_TICK_PER_MS * 1000));
    BuzzerTim->setOverflow(100 * TIMER_TICK_PER_MS, TICK_FORMAT);  // Default 100milisecondes

    // Start with inactive state without interruptions
    BuzzerTim->resume();
}

void Buzzer_Start(const uint32_t* Buzzer, uint32_t Size) {
    BuzzerTim->setCount(0);
    Active_Buzzer = Buzzer;
    Active_Buzzer_Index = 0;
    Active_Buzzer_Size = Size;

    // Patterns are composed of multiple couple of states (Actif/Inactif) and duration (miliseconds)
    // Configuration of first state of pattern
    BuzzerTim->setMode(BuzzerTimerChannel, (TimerModes_t)Active_Buzzer[Active_Buzzer_Index],
                       PIN_BUZZER);
    BuzzerTim->setOverflow(Active_Buzzer[Active_Buzzer_Index + 1u], TICK_FORMAT);

    // Activate interrupt callback to handle further states
    BuzzerTim->attachInterrupt(Update_IT_callback);
    Active_Buzzer_Index = (Active_Buzzer_Index + 2u) % Active_Buzzer_Size;

    // Timer starts. Required to configure output on GPIO
    BuzzerTim->resume();
}

void Buzzer_Short_Start(void) {
    // TODO The Low level alarms shall be:
    // Sound : short - short
    // ABOSTM : how to automatically stop buzzer after 2 bips ?

    Buzzer_Start(Buzzer_Short, BUZZER_SHORT_SIZE);
}

void Buzzer_Medium_Start(void) { Buzzer_Start(Buzzer_Medium, BUZZER_MEDIUM_SIZE); }

void Buzzer_Long_Start(void) { Buzzer_Start(Buzzer_Long, BUZZER_LONG_SIZE); }

void Buzzer_Boot_Start(void) { Buzzer_Start(Buzzer_Boot, BUZZER_BOOT_SIZE); }

void Buzzer_Stop(void) { BuzzerTim->pause(); }
