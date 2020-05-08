/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file debug.h
 * @brief Debug helpers
 *****************************************************************************/

#pragma once

#include "../includes/config.h"

/**
 * Expand arbitrary code only when in debug mode
 *
 * @param statement  A statement or a block of statements
 */
#if DEBUG == 1
#define DBG_DO(statement) statement
#else
#define DBG_DO(statement)
#endif

/**
 * Send debug traces to serial when starting a new cycle
 *
 * @param csparcycle  Duration of a cycle in hundredth of second
 * @param csparinspi  Duration of the inspiration phase in hundredth of second
 */
#if DEBUG == 1
#define DBG_AFFICHE_CSPCYCLE_CSPINSPI(csparcycle, csparinspi)                                      \
    Serial.println();                                                                              \
    Serial.println("------ Starting cycle ------");                                                \
    Serial.print("nbreCentiemeSecParCycle = ");                                                    \
    Serial.println(csparcycle);                                                                    \
    Serial.print("nbreCentiemeSecParInspi = ");                                                    \
    Serial.println(csparinspi);
#else
#define DBG_AFFICHE_CSPCYCLE_CSPINSPI(cspc, cspi)
#endif

/**
 * Send debug traces to serial with current settings
 *
 * @param csgNBcycle    Number of cycles per minute
 * @param csgOuverture  Blower's valve aperture angle
 * @param csgPPEP       Minimal PEEP
 * @param csgPPM        Maximal plateau pressure
 */
#if DEBUG == 1
#define DBG_AFFICHE_CONSIGNES(csgNBcycle, csgOuverture, csgPPEP, csgPPM)                           \
    Serial.print("consigneNbCycle = ");                                                            \
    Serial.println(csgNBcycle);                                                                    \
    Serial.print("consigneOuverture = ");                                                          \
    Serial.println(csgOuverture);                                                                  \
    Serial.print("consignePressionPEP = ");                                                        \
    Serial.println(csgPPEP);                                                                       \
    Serial.print("consignePressionPlateauMax = ");                                                 \
    Serial.println(csgPPM);
#else
#define DBG_AFFICHE_CONSIGNES(csgNBcycle, csgOuverture, csgPPEP, csgPPM)
#endif

/// Send debug trace to serial when crete pressure safeguard is activated
#if DEBUG == 1
#define DBG_PRESSION_CRETE(centieme, periode)                                                      \
    if (centieme % periode == 0) {                                                                 \
        Serial.println("Mise en securite : pression crete trop importante");                       \
    }
#else
#define DBG_PRESSION_CRETE(centieme, periode)
#endif

/// Send debug trace to serial when plateau pressure safeguard is activated
#if DEBUG == 1
#define DBG_PRESSION_PLATEAU(centieme, periode)                                                    \
    if (centieme % periode == 0) {                                                                 \
        Serial.println("Mise en securite : pression plateau trop importante");                     \
    }
#else
#define DBG_PRESSION_PLATEAU(centieme, periode)
#endif

/// Send debug trace to serial when PEEP pressure safeguard is activated
#if DEBUG == 1
#define DBG_PRESSION_PEP(centieme, periode)                                                        \
    if (centieme % periode == 0) {                                                                 \
        Serial.println("Mise en securite : pression d'expiration positive (PEP) trop faible");     \
    }
#else
#define DBG_PRESSION_PEP(centieme, periode)
#endif

/**
 * Send metrics to serial
 *
 * @param cycle            Number of passed cycles
 * @param centieme         Duration from the begining of the cycle in hundredth of second
 * @param periode
 * @param phase            Current phase
 * @param subPhase         Current subphase
 * @param pression         Current pressure
 * @param consigneBlower   Requested aperture for the blower Pressure Valve
 * @param positionBlower   Current aperture of the blower Pressure Valve
 * @param consignePatient  Requested aperture for the patient Pressure Valve
 * @param positionPatient  Current aperture of the patient Pressure Valve
 */
#if DEBUG == 1
#define DBG_PHASE_PRESSION(cycle, centieme, periode, phase, subPhase, pression, consigneBlower,    \
                           positionBlower, consignePatient, positionPatient)                       \
    if ((centieme % periode) == 0u) {                                                              \
        Serial.print((cycle * 300u) + centieme);                                                   \
        Serial.print(";");                                                                         \
        Serial.print(cycle);                                                                       \
        Serial.print(";");                                                                         \
        Serial.print(centieme);                                                                    \
        Serial.print(";");                                                                         \
        Serial.print(300u - (static_cast<uint8_t>(phase) * 100u));                                 \
        Serial.print(";");                                                                         \
        Serial.print(300u - (static_cast<uint8_t>(subPhase) * 50u));                               \
        Serial.print(";");                                                                         \
        Serial.print(pression);                                                                    \
        Serial.print(";");                                                                         \
        Serial.print(150u - consigneBlower);                                                       \
        Serial.print(";");                                                                         \
        Serial.print(150u - positionBlower);                                                       \
        Serial.print(";");                                                                         \
        Serial.print(150u - consignePatient);                                                      \
        Serial.print(";");                                                                         \
        Serial.println(150u - positionPatient);                                                    \
    }
#else
#define DBG_PHASE_PRESSION(cycle, centieme, periode, phase, subPhase, pression, consigneBlower,    \
                           positionBlower, consignePatient, positionPatient)
#endif
