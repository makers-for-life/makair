/******************************************************************************
 * @file pressure_utl.cpp
 * @copyright Copyright (c) 2020 Makers For Life
 * @author Makers For Life
 * @brief Pressure computing utility function
 *****************************************************************************/

#include <algorithm>

#include "../includes/pressure_utl.h"

static int32_t filteredRawPressure = 0;

static const int32_t RAW_PRESSURE_FILTER_DIVIDER = 5;

// Constants used in original conversion formula:
//      RATIO_VOLTAGE_DIVIDER = 0.8192
//      V_SUPPLY = 5.08
//      KPA_MMH2O = 101.97162129779

// RAW_PRESSURE_TO_MMH20_CONSTANT is computed from original
// formula as:
//      0.04 / 0.09 * KPA_MMH2O
static const int16_t RAW_PRESSURE_TO_MMH20_CONSTANT = 45;

// RAW_PRESSURE_TO_MMH20_NUM / RAW_PRESSURE_TO_MMH20_DEN is computed
// from original formula as:
//      3.3 / 1024.0 / RATIO_VOLTAGE_DIVIDER / V_SUPPLY / 0.09 * KPA_MMH2O
static const int32_t RAW_PRESSURE_TO_MMH20_NUM = 8774;
static const int32_t RAW_PRESSURE_TO_MMH20_DEN = 10000;

int16_t convertSensor2Pressure(uint16_t sensorValue) {
    int32_t rawPressure = static_cast<int32_t>(sensorValue);
    int32_t delta = rawPressure - filteredRawPressure;

    // Adjust delta so that the division result will be rounded away from zero.
    // This is needed to guaranty that filteredRawPressure will reach
    // rawPressure when it is constant.
    delta += (delta > 0) ? (RAW_PRESSURE_FILTER_DIVIDER - 1) : (-(RAW_PRESSURE_FILTER_DIVIDER - 1));
    filteredRawPressure += delta / RAW_PRESSURE_FILTER_DIVIDER;

    int16_t scaledRawPressure =
        filteredRawPressure * RAW_PRESSURE_TO_MMH20_NUM / RAW_PRESSURE_TO_MMH20_DEN;
    return std::max(scaledRawPressure - RAW_PRESSURE_TO_MMH20_CONSTANT, 0);
}

void resetFilteredRawPressure() { filteredRawPressure = 0; }
