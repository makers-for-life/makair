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

int DEFAULT_BATTERY_VOLTAGE = 24;  // 24 volts;
int BATTERY_MAX_SAMPLES = 20;      // Maximum battery samples
int BATTERY_SAMPLE[20];            // Array to store battery voltage samples
int BATTERY_CURRENT_SAMPLE = 0;    // Current battery sample index
int BATTERY_TOTAL_SAMPLES = 0;     // Battery total samples
float BATTERY_MEAN_VOLTAGE = 24;   // Mean battery voltage

void initBattery() {
    for (int i = 0; i < BATTERY_MAX_SAMPLES; i++) {
        BATTERY_SAMPLE[i] = 0;
    }
}

void batteryLoop() {
    double rawVout = analogRead(PIN_BATTERY) * DEFAULT_BATTERY_VOLTAGE / 1024.0;

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
    BATTERY_MEAN_VOLTAGE = BATTERY_TOTAL_SAMPLES / BATTERY_MAX_SAMPLES;

    Serial.print("battery = ");
    Serial.println(BATTERY_MEAN_VOLTAGE);
}