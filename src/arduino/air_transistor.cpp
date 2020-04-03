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

#include <HardwareTimer.h>

// Associated header
#include "air_transistor.h"

// Internal libraries
#include "parameters.h"

// FUNCTIONS ==================================================================
AirTransistor::AirTransistor() {}

AirTransistor::AirTransistor(uint16_t p_minApertureAngle,
                             uint16_t p_maxApertureAngle,
                             HardwareTimer* p_hardwareTimer,
                             uint16_t p_timChannel,
                             uint16_t p_servoPin)
    : minApertureAngle(p_minApertureAngle),
      maxApertureAngle(p_maxApertureAngle),
      actuator(p_hardwareTimer),
      timChannel(p_timChannel),
      servoPin(p_servoPin)
{
}

void AirTransistor::setup()
{
    actuator->setMode(timChannel, TIMER_OUTPUT_COMPARE_PWM1, servoPin);
    actuator->setCaptureCompare(timChannel, 0, MICROSEC_COMPARE_FORMAT);
}

void AirTransistor::ouvrir() { command = VALVE_OUVERT; }

void AirTransistor::ouvrirIntermediaire() { command = VALVE_DEMI_OUVERT; }

void AirTransistor::fermer() { command = VALVE_FERME; }

void AirTransistor::reduireOuverture() { command = position + 2; }

void AirTransistor::augmenterOuverture() { command = position - 2; }