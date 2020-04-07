/*
    Copyright (C) 2020 Makers For Life
*/
/******************************************************************************
 * @author Makers For Life
 * @file battery.h
 * @brief Battery related functions
 *****************************************************************************/

#pragma once

/// Expected voltage in volts when on battery
#define DEFAULT_BATTERY_VOLTAGE 20

/// Expected voltage in volts when power cord is plugged
#define DEFAULT_POWER_VOLTAGE 24

/// Number of samples of the moving average
#define BATTERY_MAX_SAMPLES 20

/**
 * Initialize battery abstraction
 *
 * @warning It must be called once to be able to check battery level
 */
void initBattery();

/**
 * Handle battery events
 *
 * @warning It must be called in the program loop
 */
void batteryLoop();

/// Handle battery voltage calculation
void updateBatterySample();

/// Updates battery states
void updateBatteryState();

/**
 * Retrives battery voltage
 *
 *  @return    Current battery voltage in volts
 */
int getBatteryVoltage();
