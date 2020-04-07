/*
    Copyright (C) 2020 Makers For Life
*/
/******************************************************************************
 * @author Makers For Life
 * @file pression.h
 * @brief Pressure sensor related functions
 *****************************************************************************/

#pragma once

#ifndef UNIT_TEST
#include <Arduino.h>
#endif

#include <stdint.h>

/**
 * @brief Convert the analog value from sensor to a pressure value
 * 
 * @param sensorValue 
 * @return the pressure in mmH2O 
 */
int16_t convertSensor2Pressure(uint16_t sensorValue);

/**
 * Get the measured or simulated pressure for the feedback control (in mmH2O)
 *
 * @param centiSec Duration in hundredth of second from the beginning of the current cycle (only
 * used when in simulation mode)
 * @return         The current pressure in mmH20
 */
int16_t readPressureSensor(uint16_t centiSec);
