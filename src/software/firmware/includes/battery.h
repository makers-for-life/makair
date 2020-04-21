/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file battery.h
 * @brief Battery related functions
 *****************************************************************************/

#pragma once

#include "Arduino.h"

/**
 * Expected voltage in volts when power cord is plugged.
 * Calculated by analogRead(PIN) * 0,0296484375 = 27,6 => 27,6 / 0,0296484375 = 930,9
 */
#define RAW_VOLTAGE_MAINS 931

/**
 * RCM_SW_16
 * Expected voltage in volts when power cord is unplugged.
 * Calculated by analogRead(PIN) * 0,0296484375 = 27 => 27 / 0,0296484375 = 911
 */
#define RAW_VOLTAGE_ON_BATTERY_HIGH 911u

// analogRead(PIN) * 0,0296484375 = 0,1 => 0,1 / 0,0296484375 = 3
#define RAW_VOLTAGE_HYSTERESIS 3u

/**
 * RCM_SW_11
 * Calculated by analogRead(PIN) * 0,0296484375 = 24,6 => 24,6 / 0,0296484375 = 829,7
 */
#define RAW_VOLTAGE_ON_BATTERY 830u

/**
 * RCM_SW_12
 * Calculated by analogRead(PIN) * 0,0296484375 = 24 => 24 / 0,0296484375 = 809,4
 */
#define RAW_VOLTAGE_ON_BATTERY_LOW 809u

/// Number of samples of the moving average
#define BATTERY_MAX_SAMPLES 20u

/**
 * Initialize battery abstraction
 *
 * @warning It must be called once to be able to check battery level
 */
void initBattery();

/**
 * Handle battery events
 *
 * @param p_cycleNumber Number of cycles since boot
 * @warning It must be called in the program loop
 */
void batteryLoop(uint32_t p_cycleNumber);

/// Handle battery voltage calculation
void updateBatterySample();

/**
 * Updates battery states
 *
 * @param p_cycleNumber Number of cycle since start
 */
void updateBatteryState(uint32_t p_cycleNumber);

/**
 * Returns battery level
 *
 * @return Battery level in volts
 */
uint32_t getBatteryLevel();


/**
 * Returns battery level X10 for better accuracy
 *
 * @return Battery level in voltsX10
 */
uint32_t getBatteryLevelX10();
