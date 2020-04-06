/*
    Copyright (C) 2020 Makers For Life
*/
/******************************************************************************
 * @author Makers For Life
 * @file pression.cpp
 * @brief Pressure sensor related functions
 *****************************************************************************/


// INCLUDES ==================================================================

// Associated header
#include "../includes/pression.h"

#ifndef UNIT_TEST
// External
#include <Arduino.h>
#endif

// Internal
#include "../includes/parameters.h"

// PROGRAM =====================================================================

double filteredVout = 0;
const double RATIO_VOLTAGE_DIVIDER = 0.8192;
const double V_SUPPLY = 5.08;
const double KPA_MMH2O = 101.97162129779;

int convertSensor2Pressure(uint16_t sensorValue) {
    double rawVout =  sensorValue * 3.3 / 1024.0;
    filteredVout = filteredVout + (rawVout - filteredVout) * 0.2;

    // Voltage divider ratio
    double vOut = filteredVout / RATIO_VOLTAGE_DIVIDER;

    // Pressure converted to kPA
    double pressure = (vOut / V_SUPPLY - 0.04) / 0.09;

    if (pressure <= 0.0) {
        return 0;
    }

    return pressure * KPA_MMH2O;
}

// Get the measured or simulated pressure for the feedback control (in mmH2O)

#if SIMULATION == 1

// Dummy function to read pressure during simulation
int readPressureSensor(uint16_t centiSec) {
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

int readPressureSensor(uint16_t centiSec) {
    (void) centiSec;
#ifndef UNIT_TEST
    return convertSensor2Pressure(analogRead(PIN_PRESSURE_SENSOR));
#else
    return 0;
#endif
}

#endif
