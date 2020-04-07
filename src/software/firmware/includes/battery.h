/*
    Copyright (C) 2020 Makers For Life
*/
/******************************************************************************
 * @author Makers For Life
 * @file battery.h
 * @brief Battery related functions
 *
 *****************************************************************************/

#pragma once

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

/**
 * Handle battery voltage calculation
 *
 */
void updateBatterySample();

/**
 * Updates battery states
 *
 */
void updateBatteryState();

/**
 * Retrives battery voltage
 *
 *  @return    Current battery voltage
 */
int getBatteryVoltage();
