/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file telemetry.h
 * @brief Implementation of the telemetry protocol
 *****************************************************************************/

#pragma once

#include "../includes/config.h"
#if MODE == MODE_PROD

// INCLUDES ===================================================================

// Associated header
#include "../includes/telemetry.h"

// Externals
#if HARDWARE_VERSION == 2
#include "Arduino.h"
#include "LL/stm32yyxx_ll_utils.h"
#endif

/// Internals
#include "../includes/pressure_controller.h"

// FUNCTIONS ==================================================================

#define FIRST_BYTE (uint8_t)0xFF

void initTelemetry(void) {
#if HARDWARE_VERSION == 2
    Serial6.begin(115200);
#endif
}

void sendDataSnapshot(uint16_t centileValue,
                      uint16_t pressureValue,
                      CyclePhases phase,
                      CycleSubPhases subPhase,
                      uint8_t blowerValvePosition,
                      uint8_t patientValvePosition,
                      uint8_t blowerRpm,
                      uint8_t batteryLevel) {
#if HARDWARE_VERSION == 2
    uint8_t phaseValue;
    if ((phase == CyclePhases::INHALATION) && (subPhase == CycleSubPhases::INSPIRATION)) {
        phaseValue = 17;  // 00010001
    } else if ((phase == CyclePhases::INHALATION)
               && (subPhase == CycleSubPhases::HOLD_INSPIRATION)) {
        phaseValue = 18;  // 00010010
    } else if ((phase == CyclePhases::EXHALATION) && (subPhase == CycleSubPhases::EXHALE)) {
        phaseValue = 68;  // 01000100
    } else {
        phaseValue = 0;
    }

    Serial6.write("D:");
    Serial6.write((uint8_t)1);

    Serial6.write(static_cast<uint8_t>(strlen(VERSION)));
    Serial6.print(VERSION);

    byte deviceId1[4];  // 32 bits
    deviceId1[0] = (LL_GetUID_Word0() >> 24) & FIRST_BYTE;
    deviceId1[1] = (LL_GetUID_Word0() >> 16) & FIRST_BYTE;
    deviceId1[2] = (LL_GetUID_Word0() >> 8) & FIRST_BYTE;
    deviceId1[3] = LL_GetUID_Word0() & FIRST_BYTE;
    Serial6.write(deviceId1, 4);

    byte deviceId2[4];  // 32 bits
    deviceId2[0] = (LL_GetUID_Word1() >> 24) & FIRST_BYTE;
    deviceId2[1] = (LL_GetUID_Word1() >> 16) & FIRST_BYTE;
    deviceId2[2] = (LL_GetUID_Word1() >> 8) & FIRST_BYTE;
    deviceId2[3] = LL_GetUID_Word1() & FIRST_BYTE;
    Serial6.write(deviceId2, 4);

    byte deviceId3[4];  // 32 bits
    deviceId3[0] = (LL_GetUID_Word2() >> 24) & FIRST_BYTE;
    deviceId3[1] = (LL_GetUID_Word2() >> 16) & FIRST_BYTE;
    deviceId3[2] = (LL_GetUID_Word2() >> 8) & FIRST_BYTE;
    deviceId3[3] = LL_GetUID_Word2() & FIRST_BYTE;
    Serial6.write(deviceId3, 4);

    Serial6.print("\t");

    byte systick[8];  // 64 bits
    uint64_t systickValue = (millis() * 1000) + (micros() % 1000);
    systick[0] = (systickValue >> 56) & FIRST_BYTE;
    systick[1] = (systickValue >> 48) & FIRST_BYTE;
    systick[2] = (systickValue >> 40) & FIRST_BYTE;
    systick[3] = (systickValue >> 32) & FIRST_BYTE;
    systick[4] = (systickValue >> 24) & FIRST_BYTE;
    systick[5] = (systickValue >> 16) & FIRST_BYTE;
    systick[6] = (systickValue >> 8) & FIRST_BYTE;
    systick[7] = systickValue & FIRST_BYTE;
    Serial6.write(systick, 8);

    Serial6.print("\t");

    byte centile[2];  // 16 bits
    centile[0] = (centileValue >> 8) & FIRST_BYTE;
    centile[1] = centileValue & FIRST_BYTE;
    Serial6.write(centile, 2);

    Serial6.print("\t");

    byte pressure[2];  // 16 bits
    pressure[0] = (pressureValue >> 8) & FIRST_BYTE;
    pressure[1] = pressureValue & FIRST_BYTE;
    Serial6.write(pressure, 2);

    Serial6.print("\t");
    Serial6.write(phaseValue);
    Serial6.print("\t");
    Serial6.write(blowerValvePosition);
    Serial6.print("\t");
    Serial6.write(patientValvePosition);
    Serial6.print("\t");
    Serial6.write(blowerRpm);
    Serial6.print("\t");
    Serial6.write(batteryLevel);
    Serial6.print("\n");
#endif
}

void sendMachineStateSnapshot(uint32_t cycleValue,
                              uint8_t peakCommand,
                              uint8_t plateauCommand,
                              uint8_t peepCommand,
                              uint8_t cpmCommand,
                              uint8_t previousPeakPressure,
                              uint8_t previousPlateauPressure,
                              uint8_t previousPeepPressure,
                              uint8_t currentAlarmCodes[ALARMS_SIZE],
                              uint8_t previousAlarmCodes[ALARMS_SIZE]) {
#if HARDWARE_VERSION == 2
    uint8_t currentAlarmSize = 0;
    for (uint8_t i = 0; i < ALARMS_SIZE; i++) {
        if (currentAlarmCodes[i] != 0u) {
            currentAlarmSize++;
        } else {
            break;
        }
    }

    uint8_t previousAlarmSize = 0;
    for (uint8_t i = 0; i < ALARMS_SIZE; i++) {
        if (previousAlarmCodes[i] != 0u) {
            previousAlarmSize++;
        } else {
            break;
        }
    }

    Serial6.write("S:");
    Serial6.write((uint8_t)1);

    Serial6.write(static_cast<uint8_t>(strlen(VERSION)));
    Serial6.print(VERSION);

    byte deviceId1[4];  // 32 bits
    deviceId1[0] = (LL_GetUID_Word0() >> 24) & FIRST_BYTE;
    deviceId1[1] = (LL_GetUID_Word0() >> 16) & FIRST_BYTE;
    deviceId1[2] = (LL_GetUID_Word0() >> 8) & FIRST_BYTE;
    deviceId1[3] = LL_GetUID_Word0() & FIRST_BYTE;
    Serial6.write(deviceId1, 4);

    byte deviceId2[4];  // 32 bits
    deviceId2[0] = (LL_GetUID_Word1() >> 24) & FIRST_BYTE;
    deviceId2[1] = (LL_GetUID_Word1() >> 16) & FIRST_BYTE;
    deviceId2[2] = (LL_GetUID_Word1() >> 8) & FIRST_BYTE;
    deviceId2[3] = LL_GetUID_Word1() & FIRST_BYTE;
    Serial6.write(deviceId2, 4);

    byte deviceId3[4];  // 32 bits
    deviceId3[0] = (LL_GetUID_Word2() >> 24) & FIRST_BYTE;
    deviceId3[1] = (LL_GetUID_Word2() >> 16) & FIRST_BYTE;
    deviceId3[2] = (LL_GetUID_Word2() >> 8) & FIRST_BYTE;
    deviceId3[3] = LL_GetUID_Word2() & FIRST_BYTE;
    Serial6.write(deviceId3, 4);

    Serial6.print("\t");

    byte cycle[4];  // 32 bits
    cycle[0] = (cycleValue >> 24) & FIRST_BYTE;
    cycle[1] = (cycleValue >> 16) & FIRST_BYTE;
    cycle[2] = (cycleValue >> 8) & FIRST_BYTE;
    cycle[3] = cycleValue & FIRST_BYTE;
    Serial6.write(cycle, 4);

    Serial6.print("\t");
    Serial6.write(peakCommand);
    Serial6.print("\t");
    Serial6.write(plateauCommand);
    Serial6.print("\t");
    Serial6.write(peepCommand);
    Serial6.print("\t");
    Serial6.write(cpmCommand);
    Serial6.print("\t");
    Serial6.write(previousPeakPressure);
    Serial6.print("\t");
    Serial6.write(previousPlateauPressure);
    Serial6.print("\t");
    Serial6.write(previousPeepPressure);

    Serial6.print("\t");

    Serial6.write(currentAlarmSize);
    Serial6.write(currentAlarmCodes, currentAlarmSize);

    Serial6.print("\t");

    Serial6.write(previousAlarmSize);
    Serial6.write(previousAlarmCodes, previousAlarmSize);

    Serial6.print("\n");
#endif
}

#endif
