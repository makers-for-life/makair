/*=============================================================================
 * @file air_transistor.h
 *
 * COVID Respirator
 *
 * @section copyright Copyright
 *
 * Makers For Life
 *
 * @section descr File description
 *
 * This file defines the AirTransistor object
 */

#pragma once

// INCLUDES ===================================================================

// External
#include <HardwareTimer.h>

// Internal libraries
#include "parameters.h"

// MACROS =================================================================

#define Angle2MicroSeconds(value) map(value, 0, 180, 1000, 2000)

// CLASS =================================================================

class AirTransistor
{
public:
    //! Default constructor
    AirTransistor();

    //! Parameterized constructor
    AirTransistor(uint16_t p_minApertureAngle,
                  uint16_t p_maxApertureAngle,
                  HardwareTimer* hardwareTimer,
                  uint16_t p_timChannel,
                  uint16_t p_servoPin);

    void setup();

    inline void execute()
    {
        // On évite d'aller plus loin que les limites de la valve
        if (command < minApertureAngle)
        {
            command = minApertureAngle;
        }
        else if (command > maxApertureAngle)
        {
            command = maxApertureAngle;
        }

        if (command != position)
        {
            actuator->setCaptureCompare(timChannel, Angle2MicroSeconds(command),
                                        MICROSEC_COMPARE_FORMAT);
            position = command;
        }
    }

    uint16_t command;
    uint16_t position;

private:
    uint16_t minApertureAngle;
    uint16_t maxApertureAngle;
    uint16_t timChannel;
    uint16_t servoPin;
    HardwareTimer* actuator;
};
