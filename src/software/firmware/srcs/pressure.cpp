/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file pressure.cpp
 * @brief Pressure sensor related functions
 *****************************************************************************/

// INCLUDES ==================================================================

// Associated header
#include "../includes/pressure.h"
#include "../includes/pressure_utl.h"

// External
#include <Arduino.h>

// Internal
#include "../includes/parameters.h"

// PROGRAM =====================================================================

// Get the measured or simulated pressure for the feedback control (in mmH2O)

#if SIMULATION == 1

// Dummy function to read pressure during simulation
int16_t readPressureSensor(uint16_t centiSec) {
    if (centiSec < uint16_t(10)) {
        return 350;
    } else if (centiSec < uint16_t(15)) {
        return 400;
    } else if (centiSec < uint16_t(30)) {
        return 600;
    } else if (centiSec < uint16_t(45)) {
        return 700;
    } else if (centiSec < uint16_t(60)) {
        return 500;
    } else if (centiSec < uint16_t(100)) {
        return 300;
    } else if (centiSec < 200) {
        return 110;
    } else if (centiSec < 250) {
        return 90;
    } else {
        return 70;
    }
}
#else

int16_t readPressureSensor(uint16_t centiSec) {
    (void)centiSec;
    return convertSensor2Pressure(analogRead(PIN_PRESSURE_SENSOR));
}

#endif
