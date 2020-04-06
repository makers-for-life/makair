/*
    Copyright (C) 2020 Makers For Life
*/
/******************************************************************************
 * @author Makers For Life
 * @file pressure_valve.cpp
 * @brief Tools to control an Pressure Valve's servomotor
 *****************************************************************************/

#pragma once

// INCLUDES ===================================================================

// Associated header
#include "../includes/pressure_valve.h"

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
    // TODO: use min() and max() instead of a condition
    if (p_openApertureAngle > p_closeApertureAngle) {
        minApertureAngle = p_closeApertureAngle;
        maxApertureAngle = p_openApertureAngle;
    } else {
        maxApertureAngle = p_closeApertureAngle;
        minApertureAngle = p_openApertureAngle;
    }
    command = p_closeApertureAngle;
    position = -1;
}

void PressureValve::setup() {
    actuator->setMode(timerChannel, TIMER_OUTPUT_COMPARE_PWM1, servoPin);
    actuator->setCaptureCompare(timerChannel, 0, MICROSEC_COMPARE_FORMAT);
}

void PressureValve::open() { command = openApertureAngle; }

void PressureValve::close() { command = closeApertureAngle; }

void PressureValve::open(int32_t p_command) { command = p_command; }
