/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file pressure_valve.cpp
 * @brief Tools to control an Pressure Valve's servomotor
 *****************************************************************************/

#pragma once

// INCLUDES ===================================================================

// Associated header
#include "../includes/pressure_valve.h"

// External libraries
#include <algorithm>

// Internal libraries
#include "../includes/parameters.h"

// FUNCTIONS ==================================================================
PressureValve::PressureValve() {}

PressureValve::PressureValve(HardwareTimer* p_hardwareTimer,
                             uint16_t p_timerChannel,
                             uint16_t p_servoPin,
                             uint16_t p_openApertureAngle,
                             uint16_t p_closeApertureAngle) {
    actuator = p_hardwareTimer;
    timerChannel = p_timerChannel;
    servoPin = p_servoPin;
    openApertureAngle = p_openApertureAngle;
    closeApertureAngle = p_closeApertureAngle;
    minApertureAngle = min(p_closeApertureAngle, p_openApertureAngle);
    maxApertureAngle = max(p_closeApertureAngle, p_openApertureAngle);
    command = p_closeApertureAngle;
    position = -1;
}

void PressureValve::setup() {
    actuator->setMode(timerChannel, TIMER_OUTPUT_COMPARE_PWM1, servoPin);
    actuator->setCaptureCompare(timerChannel, 0, MICROSEC_COMPARE_FORMAT);
}

void PressureValve::open() { command = openApertureAngle; }

void PressureValve::close() { command = closeApertureAngle; }

void PressureValve::open(uint16_t p_command) { command = p_command; }

// return a compare value up to angle.
uint16_t ValveAngle2MicroSeconds(uint16_t value) {
#if VALVE_TYPE == VT_SERVO_V1
    // map 0 - 125 ° to 0.8 - 1.2 ms (standard PPM command is 1ms - 2ms, these servomotors handle a
    // greater range)
    return map(value, 0, 125, 800, 2200);
#endif

#if VALVE_TYPE == VT_EMERSON_ASCO
    // map 0 - 125 ° to 100% pwm - EMERSON_MIN_PWM (0% when fully closed)
    if (VALVE_CLOSED_STATE == value) {
        return 1;  // compare units are a bit weird in this micro controller. TODO: avoid 500ns
                   // spikes.
    } else {
        return map(value, 0, 125, SERVO_VALVE_PERIOD, EMERSON_MIN_PWM);
    }
#endif
}
