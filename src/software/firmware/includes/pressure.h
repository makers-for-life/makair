/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file pressure.h
 * @brief Pressure sensor related functions
 *****************************************************************************/

#pragma once

#include <stdint.h>

/**
 * Get the measured or simulated pressure for the feedback control (in mmH2O)
 *
 * @param centiSec Duration in hundredth of second from the beginning of the current cycle (only
 * used when in simulation mode)
 * @param pressureOffset Pressure offset in mmH2O to apply to the measure
 * @return         The current pressure in mmH20
 */
int16_t readPressureSensor(uint16_t centiSec, int16_t pressureOffset);
