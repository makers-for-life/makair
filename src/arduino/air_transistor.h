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

// servomoteur blower : connecte le flux d'air vers le Air Transistor patient ou
// vers l'extérieur 90° → tout est fermé entre 45° (90 - ANGLE_OUVERTURE_MAXI)
// et 82° (90 - ANGLE_OUVERTURE_MINI) → envoi du flux vers l'extérieur entre 98°
// (90 + ANGLE_OUVERTURE_MINI) et 135° (90 + ANGLE_OUVERTURE_MAXI) → envoi du
// flux vers le Air Transistor patient
struct AirTransistor
{
    //! Default constructor
    AirTransistor();

    //! Parameterized constructor
    AirTransistor(uint16_t p_minApertureAngle,
                  uint16_t p_maxApertureAngle,
                  uint16_t p_defaultCommand,
                  uint16_t p_failsafeCommand);

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
