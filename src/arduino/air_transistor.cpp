/*
    Copyright (C) 2020 Makers For Life
*/
/******************************************************************************
 * @author Makers For Life
 * @file air_transistor.cpp
 * @brief Tools to control an Air Transistor's servomotor
 *****************************************************************************/

#pragma once

// INCLUDES ===================================================================

// Associated header
#include "air_transistor.h"

// Internal libraries
#include "parameters.h"

// FUNCTIONS ==================================================================

AirTransistor::AirTransistor() {}

AirTransistor::AirTransistor(HardwareTimer* p_hardwareTimer,
                             uint16_t p_timChannel,
                             uint16_t p_servoPin,
                             uint16_t p_openApertureAngle,
                             uint16_t p_closeApertureAngle) {
    actuator = p_hardwareTimer;
    timChannel = p_timChannel;
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

void AirTransistor::setup() {
    actuator->setMode(timChannel, TIMER_OUTPUT_COMPARE_PWM1, servoPin);
    actuator->setCaptureCompare(timChannel, 0, MICROSEC_COMPARE_FORMAT);
}

void AirTransistor::open() { command = openApertureAngle; }

void AirTransistor::close() { command = closeApertureAngle; }

void AirTransistor::open(int32_t p_command) { command = p_command; }
