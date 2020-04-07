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

void batteryLoop() {
    double rawVout = analogRead(PIN_BATTERY) * 24 / 1024.0;
    Serial.print("battery = ");
    Serial.println(rawVout);
}