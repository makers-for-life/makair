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

PressureValve::PressureValve(uint16_t p_minApertureAngle,
                             uint16_t p_maxApertureAngle,
                             HardwareTimer* p_hardwareTimer,
                             uint16_t p_timChannel,
                             uint16_t p_servoPin)
    : minApertureAngle(p_minApertureAngle),
      maxApertureAngle(p_maxApertureAngle),
      actuator(p_hardwareTimer),
      timChannel(p_timChannel),
      servoPin(p_servoPin) {}

void PressureValve::setup() {
    actuator->setMode(timChannel, TIMER_OUTPUT_COMPARE_PWM1, servoPin);
    actuator->setCaptureCompare(timChannel, 0, MICROSEC_COMPARE_FORMAT);
}

void PressureValve::open() { command = VALVE_OUVERT; }

void PressureValve::openMiddle() { command = VALVE_DEMI_OUVERT; }

void PressureValve::close() { command = VALVE_FERME; }

void PressureValve::decrease() { command = position + 2; }

void PressureValve::increase() { command = position - 2; }
