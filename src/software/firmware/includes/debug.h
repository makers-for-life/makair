/*
    Copyright (C) 2020 Makers For Life
*/
/******************************************************************************
 * @author Makers For Life
 * @file debug.h
 * @brief Debug helpers
 *****************************************************************************/

#pragma once

#include "config.h"

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
 * Send data metrics to serial ( almost evrey 1/100s) 
 * Take notice that sending 30 bytes of data can take up to 3ms (1/3 of the 10ms loop) at a 115200 baudrate.
 * Please consider reducing CPU charge using a DMA on the UART TX.
 *
 * @param cycle            Number of passed cycles
 * @param centieme         Duration from the begining of the cycle in hundredth of second
 * @param periode
 * @param phase            Current phase
 * @param subPhase         Current subphase
 * @param pression         Current pressure
 * @param positionBlower   Current aperture of the blower Pressure Valve
 * @param positionPatient  Current aperture of the patient Pressure Valve
 * @param blowerSpeed      Current speed of the blower
 * @param baterryLevel     Current battery level if any
 */
#if DEBUG == 1
#define DBG_TELEMERTY_DATA(cycle, centieme, periode, phase, subPhase, pression,    \
                           positionBlower, positionPatient, blowerSpeed, batteryLevel)                       \
    if ((centieme % periode) == 0u) {                                                              \
        Serial.print("D"); \
		Serial.print("\t"); \
		Serial.println("UID [HEX] : " + String(*(uint32_t*)(UID_BASE), HEX) + " " + String(*(uint32_t*)(UID_BASE + 0x04), HEX) + " " + String(*(uint32_t*)(UID_BASE + 0x08), HEX)); \
        Serial.print("\t"); \
		Serial.print((cycle * 300u) + centieme); \
        Serial.print("\t"); \
        Serial.print(cycle);                                                                       \
        Serial.print("\t"); \
        Serial.print(centieme);                                                                    \
        Serial.print("\t"); \
        Serial.print(subPhase + phase);                                                            \
        Serial.print("\t"); \
        Serial.print(pression);                                                                    \
        Serial.print("\t"); \
        Serial.print(150u - positionBlower);                                                       \
        Serial.print("\t"); \
        Serial.println(150u - positionPatient); \
		Serial.print("\t"); \
		Serial.print(blowerSpeed); \
		Serial.print("\t"); \
		Serial.print(batteryLevel); \
		Serial.print(";"); \
    }
#else
#define DBG_TELEMERTY_DATA(cycle, centieme, periode, phase, subPhase, pression, consigneBlower,    \
                           positionBlower, consignePatient, positionPatient)
#endif


/**
 * The machine state snapshot, sent at the beginning of a cycle.
 * This message summarize the machine state and the respiration metric of the last cycle (called previous cycle)
 *
 * @param cycleNb       Number of passed cycles
 * @param csgPP         Consigne Maximal plateau pressure
 * @param csgPPEP       Consigne Minimal PEEP
 * @param csgBR         Consigne Breath Rate per minute
 * @param csgPW         Consigne Patient weight (or volume)
 * @param prevPeakP     Previous peak pressure
 * @param prevPP        Previous plateau pressure
 * @param prevPPEP      Previous peep pressure
 * @param currAlams     Current alarms (the list of the alarms triggered during the previous cycle)
 * @param prevAlarms    Current alarms (the list of the alarms triggered during the previous cycle)
 */
#if DEBUG == 1
#define DBG_TELEMERTY_STATE(cycleNb, csgPP, csgPPEP, csgBR, csgPW, prevPeakP, prevPP, prevPPEP,   \
                           currAlams, prevAlarms)                                                \
    if ((centieme % periode) == 0u) {                                                              \
        Serial.print("D"); \
		Serial.print("\t"); \
		Serial.println("UID [HEX] : " + String(*(uint32_t*)(UID_BASE), HEX) + " " + String(*(uint32_t*)(UID_BASE + 0x04), HEX) + " " + String(*(uint32_t*)(UID_BASE + 0x08), HEX)); \
        Serial.print("\t"); \
        Serial.print(cycleNb); \
        Serial.print("\t"); \
        Serial.print(csgPP); \
        Serial.print("\t"); \
        Serial.print(csgPPEP); \
        Serial.print("\t"); \
        Serial.print(csgBR); \
        Serial.print("\t"); \
        Serial.print(csgPW); \
        Serial.print("\t"); \
        Serial.println(prevPeakP); \
		Serial.print("\t"); \
		Serial.print(prevPP); \
		Serial.print("\t"); \
		Serial.print(prevPPEP); \
		Serial.print(";"); \
		Serial.print(currAlams); \
		Serial.print(";"); \
		Serial.print(prevAlarms); \
		Serial.print(";"); \
    }
#else
#define DBG_TELEMERTY_STATE(cycle, centieme, periode, phase, subPhase, pression, consigneBlower,    \
                           positionBlower, consignePatient, positionPatient)
#endif
