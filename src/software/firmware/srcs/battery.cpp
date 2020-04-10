/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
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
#include "../includes/debug.h"
#include "../includes/parameters.h"

// PROGRAM =====================================================================

uint32_t batterySample[20];         // Array to store battery voltage samples
uint32_t batteryCurrentSample = 0;  // Current battery sample index
uint32_t batteryTotalSamples = 0;   // Battery total samples
bool isRunningOnBattery = false;    // State to know if we are running on the battery or not
uint32_t batteryMeanVoltage = DEFAULT_POWER_VOLTAGE;  // Mean battery voltage in volts

void initBattery() {
    for (int i = 0; i < BATTERY_MAX_SAMPLES; i++) {
        batterySample[i] = 0;
    }
}

void updateBatterySample() {
    uint16_t rawVout = analogRead(PIN_BATTERY);

    // Substract previous sample
    batteryTotalSamples = batteryTotalSamples - batterySample[batteryCurrentSample];

    // Assign sample from Vout
    batterySample[batteryCurrentSample] = rawVout;

    // Add sample
    batteryTotalSamples = batteryTotalSamples + batterySample[batteryCurrentSample];

    // Increment sample
    batteryCurrentSample++;

    // If we reached max samples
    if (batteryCurrentSample >= BATTERY_MAX_SAMPLES) {
        batteryCurrentSample = 0;
    }

    // Updates mean voltage
    batteryMeanVoltage = (batteryTotalSamples / BATTERY_MAX_SAMPLES) * DEFAULT_POWER_VOLTAGE / 1024;
}

void updateBatteryState() {
    // If mean voltage is running below 20V it means we are running on the battery
    if (batteryMeanVoltage <= DEFAULT_BATTERY_VOLTAGE && isRunningOnBattery == false) {
        // We are not running on the battery
        // TODO: Run an alarm?
        isRunningOnBattery = true;

        DBG_DO(Serial.println("Running on the battery");)
    } else if (batteryMeanVoltage > DEFAULT_BATTERY_VOLTAGE && isRunningOnBattery == true) {
        // We are not running on the AC
        isRunningOnBattery = false;

        DBG_DO(Serial.println("Running on the AC");)
    }
}

uint32_t getBatteryVoltage() { return batteryMeanVoltage; }

void batteryLoop() {
    updateBatterySample();
    updateBatteryState();
}
