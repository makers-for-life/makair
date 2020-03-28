/*=============================================================================
 * @file debug.h
 *
 * COVID Respirator
 *
 * @section copyright Copyright
 *
 * Makers For Life
 *
 * @section descr File description
 *
 * This file defines the debug features
 */

#pragma once

#include "config.h"

/*! General macro to expand some code if DEBUG is defined
 * \warning to be used with only one statement
 */
#ifdef DEBUG
#define DBG_DO(statement) statement
#else
#define DBG_DO(statement)
#endif

/*! Macro called at the beginning of the control loop to display:
 *  -   the number of hundredth of second per respiratory cycle
 *  -   the number of hundredth of second per inhalation
 */
#ifdef DEBUG
#define DBG_AFFICHE_CSPCYCLE_CSPINSPI(centiSecPerCycle, centiSecPerInhalation)                     \
    Serial.println();                                                                              \
    Serial.println("------ Starting Cycle ------");                                                \
    Serial.print("centiSecPerCycle = ");                                                           \
    Serial.println(centiSecPerCycle);                                                              \
    Serial.print("centiSecPerInhalation = ");                                                      \
    Serial.println(centiSecPerInhalation);
#else
#define DBG_AFFICHE_CSPCYCLE_CSPINSPI(centiSecPerCycle, centiSecPerInhalation)
#endif

/*! Macro that displays the following commands:
 *  -   the number of respiratory cycles per minute
 *  -   the valve aperture
 *  -   the PEEP
 *  -   the maximal plateau pressure
 */
#ifdef DEBUG
#define DBG_AFFICHE_CONSIGNES(cyclesPerMinute, Aperture, PEEP, maxPlateauPressure)                 \
    Serial.print("cyclesPerMinute = ");                                                            \
    Serial.println(cyclesPerMinute);                                                               \
    Serial.print("Aperture = ");                                                                   \
    Serial.println(Aperture);                                                                      \
    Serial.print("PEEP = ");                                                                       \
    Serial.println(PEEP);                                                                          \
    Serial.print("maxPlateauPressure = ");                                                         \
    Serial.println(maxPlateauPressure);
#else
#define DBG_AFFICHE_CONSIGNES(cyclesPerMinute, Aperture, PEEP, maxPlateauPressure)
#endif

//! Macro to alert about a safety procedure due to a peak pressure too high
#ifdef DEBUG
#define DBG_PRESSION_CRETE(centiSec, period)                                                       \
    if (centiSec % period == 0)                                                                    \
    {                                                                                              \
        Serial.println("Safety procedure: peak pressure too high");                                \
    }
#else
#define DBG_PRESSION_CRETE(centiSec, period)
#endif

//! Macro to alert about a safety procedure due to a plateau pressure too high
#ifdef DEBUG
#define DBG_PRESSION_PLATEAU(centiSec, period)                                                     \
    if (centiSec % period == 0)                                                                    \
    {                                                                                              \
        Serial.println("Safety procedure: plateau pressure too high");                             \
    }
#else
#define DBG_PRESSION_PLATEAU(centiSec, period)
#endif

//! Macro to alert about a safety procedure due to a PEEP too low
#ifdef DEBUG
#define DBG_PRESSION_PEP(centiSec, period)                                                         \
    if (centiSec % period == 0)                                                                    \
    {                                                                                              \
        Serial.println("Safety procedure: PEEP too low");                                          \
    }
#else
#define DBG_PRESSION_PEP(centiSec, period)
#endif

//! Macro to display the current respiratory cycle phase and the current pressure
#ifdef DEBUG
#define DBG_PHASE_PRESSION(centiSec, period, phase, pressure)                                      \
    if (centiSec % period == 0)                                                                    \
    {                                                                                              \
        Serial.print("Phase : ");                                                                  \
        Serial.println(phase);                                                                     \
        Serial.print("Pressure : ");                                                               \
        Serial.println(pressure);                                                                  \
    }
#else
#define DBG_PHASE_PRESSION(centiSec, period, phase, pressure)
#endif
