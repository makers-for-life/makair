/*
    Copyright (C) 2020 Makers For Life
*/
/******************************************************************************
 * @author Makers For Life
 * @file pression.h
 * @brief Pressure sensor related functions
 *****************************************************************************/

#pragma once

#include <Arduino.h>

/**
 * Get the measured or simulated pressure for the feedback control (in mmH2O)
 *
 * @param centiSec Duration in hundredth of second from the beginning of the current cycle (only
 * used when in simulation mode)
 * @return         The current pressure in mmH20
 */
int readPressureSensor(uint16_t centiSec);
