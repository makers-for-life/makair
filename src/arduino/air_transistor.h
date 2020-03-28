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

// External libraries
#include <Servo.h>

struct AirTransistor
{
    //! Default constructor
    AirTransistor();

    //! Parameterized constructor
    AirTransistor(uint16_t p_minApertureAngle,
                  uint16_t p_maxApertureAngle,
                  uint16_t p_defaultCommand,
                  uint16_t p_failsafeCommand);

    inline void reset()
    {
        command = defaultCommand;
        position = defaultCommand;
    }

    inline void execute()
    {
        if (command != position)
        {
            actuator.write(command);
            position = command;
        }
    }

    /// Minimal aperture angle of the transistor [°]
    uint16_t minApertureAngle;

    /// Maximal aperture angle of the transistor [°]
    uint16_t maxApertureAngle;

    /// Default angle command for resets [°]
    uint16_t defaultCommand;

    /// Failsafe angle command [°]
    uint16_t failsafeCommand;

    /// Angle command [°]
    uint16_t command;

    /// Actual angle [°]
    uint16_t position;

    /// Actuator
    Servo actuator;
};
