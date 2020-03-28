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

    uint16_t minApertureAngle;
    uint16_t maxApertureAngle;
    uint16_t defaultCommand;
    uint16_t failsafeCommand;
    uint16_t command;
    uint16_t position;
    Servo actuator;
};
