/*
    Copyright (C) 2020 Makers For Life
*/
/******************************************************************************
 * @author Makers For Life
 * @file battery.cpp
 * @brief Battery related functions
 *****************************************************************************/

#pragma once

// INCLUDES ===================================================================

// Associated header
#include "../includes/battery.h"

// Externals
#include "Arduino.h"

// Internal
#include "../includes/parameters.h"

// PROGRAM =====================================================================

uint16_t DEFAULT_BATTERY_VOLTAGE = 20;  // 20 volts;
uint16_t DEFAULT_POWER_VOLTAGE = 24;    // 24 volts;
uint16_t BATTERY_MAX_SAMPLES = 20;      // Maximum battery samples
uint16_t BATTERY_SAMPLE[20];            // Array to store battery voltage samples
uint16_t BATTERY_CURRENT_SAMPLE = 0;    // Current battery sample index
uint16_t BATTERY_TOTAL_SAMPLES = 0;     // Battery total samples
bool IS_RUNNING_ON_BATTERY = false;     // State to know if we are running on the battery or not
uint16_t BATTERY_MEAN_VOLTAGE = DEFAULT_POWER_VOLTAGE;  // Mean battery voltage

void initBattery() {
    for (int i = 0; i < BATTERY_MAX_SAMPLES; i++) {
        BATTERY_SAMPLE[i] = 0;
    }
}

void updateBatterySample() {
    uint16_t rawVout = analogRead(PIN_BATTERY);

    // Substract previous sample
    BATTERY_TOTAL_SAMPLES = BATTERY_TOTAL_SAMPLES - BATTERY_SAMPLE[BATTERY_CURRENT_SAMPLE];

    // Assign sample from Vout
    BATTERY_SAMPLE[BATTERY_CURRENT_SAMPLE] = rawVout;

    // Add sample
    BATTERY_TOTAL_SAMPLES = BATTERY_TOTAL_SAMPLES + BATTERY_SAMPLE[BATTERY_CURRENT_SAMPLE];

    // Increment sample
    BATTERY_CURRENT_SAMPLE++;

    // If we reached max samples
    if (BATTERY_CURRENT_SAMPLE >= BATTERY_MAX_SAMPLES) {
        BATTERY_CURRENT_SAMPLE = 0;
    }

    // Updates mean voltage
    BATTERY_MEAN_VOLTAGE =
        (BATTERY_TOTAL_SAMPLES / BATTERY_MAX_SAMPLES) * DEFAULT_POWER_VOLTAGE / 1024.0;
}

void updateBatteryState() {
    // If mean voltage is running below 20V it means we are running on the battery
    if (BATTERY_MEAN_VOLTAGE <= DEFAULT_BATTERY_VOLTAGE && IS_RUNNING_ON_BATTERY == false) {
        // We are not running on the battery
        // TODO: Run an alarm?
        IS_RUNNING_ON_BATTERY = true;
    } else if (BATTERY_MEAN_VOLTAGE > DEFAULT_BATTERY_VOLTAGE && IS_RUNNING_ON_BATTERY == true) {
        // We are not running on the AC
        IS_RUNNING_ON_BATTERY = false;
    }
}

int getBatteryVoltage() { return BATTERY_MEAN_VOLTAGE; }

void batteryLoop() {
    updateBatterySample();
    updateBatteryState();
}
