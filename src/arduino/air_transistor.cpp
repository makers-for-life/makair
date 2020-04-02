/*=============================================================================
 * @file air_transistor.cpp
 *
 * COVID Respirator
 *
 * @section copyright Copyright
 *
 * Makers For Life
 *
 * @section descr File description
 *
 * This file implements the AirTransistor object
 */

#pragma once

// INCLUDES ===================================================================

// Associated header
#include "air_transistor.h"

// Internal libraries
#include "parameters.h"

// FUNCTIONS ==================================================================
AirTransistor::AirTransistor() {}

AirTransistor::AirTransistor(uint16_t p_minApertureAngle,
                             uint16_t p_maxApertureAngle,
                             TIM_TypeDef* p_timInstance,
                             uint16_t p_timChannel,
                             uint16_t p_servoPin)
    : minApertureAngle(p_minApertureAngle),
      maxApertureAngle(p_maxApertureAngle),
      timInstance(p_timInstance),
      timChannel(p_timChannel),
      servoPin(p_servoPin)
{
}

void AirTransistor::setup()
{
    actuator = new HardwareTimer(timInstance);
    actuator->setMode(timChannel, TIMER_OUTPUT_COMPARE_PWM1, servoPin);
    actuator->setOverflow(SERVO_VALVE_PERIOD, MICROSEC_FORMAT);
    actuator->setCaptureCompare(timChannel, 0, MICROSEC_COMPARE_FORMAT);
    actuator->resume();
}
