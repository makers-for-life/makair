/**
 * @file pressure_utl.h
 * @author Makers For Life
 * @brief Pressure computing utility function
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#pragma once

#include <stdint.h>

/**
 * @brief Convert the analog value from sensor to a pressure value
 *
 * @param sensorValue Value read from the analog input connected to the sensor
 * @return the pressure in mmH2O
 */
int16_t convertSensor2Pressure(uint16_t sensorValue);

/**
 * @brief Reset the value of void filteredRawPressure to 0.
 * Mainly for testing purpose
 *
 */
void resetFilteredRawPressure();
