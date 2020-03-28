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

// INCLUDES ===================================================================

// Associated header
#include "air_transistor.h"

// Internal libraries
#include "parameters.h"

// FUNCTIONS ==================================================================

AirTransistor::AirTransistor()
    : minApertureAngle(ANGLE_OUVERTURE_MINI),
      maxApertureAngle(ANGLE_OUVERTURE_MAXI),
      defaultCommand(ANGLE_FERMETURE),
      failsafeCommand(ANGLE_FERMETURE - ANGLE_OUVERTURE_MAXI),
      command(ANGLE_FERMETURE),
      position(ANGLE_FERMETURE)
{
}

AirTransistor::AirTransistor(uint16_t p_minApertureAngle,
                             uint16_t p_maxApertureAngle,
                             uint16_t p_defaultCommand,
                             uint16_t p_failsafeCommand)
    : minApertureAngle(p_minApertureAngle),
      maxApertureAngle(p_maxApertureAngle),
      defaultCommand(p_defaultCommand),
      failsafeCommand(p_failsafeCommand),
      command(ANGLE_FERMETURE),
      position(ANGLE_FERMETURE)
{
}
