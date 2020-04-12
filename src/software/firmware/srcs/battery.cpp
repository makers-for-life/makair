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
#include "../includes/alarm_controller.h"
#include "../includes/debug.h"
#include "../includes/parameters.h"

// PROGRAM =====================================================================

static uint32_t rawBatterySample[20];      // Array to store battery voltage samples
static uint32_t batteryCurrentSample = 0;  // Current battery sample index
static uint32_t batteryTotalSamples = 0;   // Battery total samples
static uint32_t rawBatteryMeanVoltage = RAW_VOLTAGE_MAINS;  // Mean battery voltage in volts
static bool isRunningOnBattery = false;

void initBattery() {
    for (uint16_t i = 0; i < BATTERY_MAX_SAMPLES; i++) {
        rawBatterySample[i] = 0;
    }
}

void updateBatterySample() {
    uint16_t rawVout = analogRead(PIN_BATTERY);

    // Substract previous sample
    batteryTotalSamples = batteryTotalSamples - rawBatterySample[batteryCurrentSample];

    // Assign sample from Vout
    rawBatterySample[batteryCurrentSample] = rawVout;

    // Add sample
    batteryTotalSamples = batteryTotalSamples + rawBatterySample[batteryCurrentSample];

    // Increment sample
    batteryCurrentSample++;

    // If we reached max samples
    if (batteryCurrentSample >= BATTERY_MAX_SAMPLES) {
        batteryCurrentSample = 0;
    }

    // Updates mean voltage
    rawBatteryMeanVoltage = (batteryTotalSamples / BATTERY_MAX_SAMPLES);
}

void updateBatteryState(uint32_t p_cycleNumber) {
    if (rawBatteryMeanVoltage < RAW_VOLTAGE_ON_BATTERY_LOW) {
        alarmController.detectedAlarm(RCM_SW_12, p_cycleNumber);
        isRunningOnBattery = true;
    } else {
        alarmController.notDetectedAlarm(RCM_SW_12);
    }

    if (!isRunningOnBattery && (rawBatteryMeanVoltage < RAW_VOLTAGE_ON_BATTERY)) {
        alarmController.detectedAlarm(RCM_SW_11, p_cycleNumber);
        isRunningOnBattery = true;
    } else {
        alarmController.notDetectedAlarm(RCM_SW_11);
    }

    if (!isRunningOnBattery && (rawBatteryMeanVoltage < RAW_VOLTAGE_MAINS_MIN)) {
        alarmController.detectedAlarm(RCM_SW_16, p_cycleNumber);
        isRunningOnBattery = true;
    } else {
        alarmController.notDetectedAlarm(RCM_SW_16);
    }
}

void batteryLoop(uint32_t p_cycleNumber) {
    updateBatterySample();
    updateBatteryState(p_cycleNumber);
}

uint32_t getBatteryLevel() { return rawBatteryMeanVoltage * 0.0296484375; }
