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

// cppcheck-suppress unusedFunction
void initTelemetry(void) {
#if HARDWARE_VERSION == 2
    Serial6.begin(115200);
#endif
}

// cppcheck-suppress unusedFunction
void sendDataSnapshot(uint16_t centileValue,
                      uint16_t pressureValue,
                      CyclePhases phase,
                      CycleSubPhases subPhase,
                      uint8_t blowerValvePosition,
                      uint8_t patientValvePosition,
                      uint8_t blowerRpm,
                      uint8_t batteryLevel) {
#if HARDWARE_VERSION == 1
    (void)centileValue;
    (void)pressureValue;
    (void)phase;
    (void)subPhase;
    (void)blowerValvePosition;
    (void)patientValvePosition;
    (void)blowerRpm;
    (void)batteryLevel;
#elif HARDWARE_VERSION == 2
    uint8_t phaseValue;
    if ((phase == CyclePhases::INHALATION) && (subPhase == CycleSubPhases::INSPIRATION)) {
        phaseValue = 17u;  // 00010001
    } else if ((phase == CyclePhases::INHALATION)
               && (subPhase == CycleSubPhases::HOLD_INSPIRATION)) {
        phaseValue = 18u;  // 00010010
    } else if ((phase == CyclePhases::EXHALATION) && (subPhase == CycleSubPhases::EXHALE)) {
        phaseValue = 68u;  // 01000100
    } else {
        phaseValue = 0u;
    }

    Serial6.write("D:");
    Serial6.write((uint8_t)1u);

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

// cppcheck-suppress unusedFunction
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
#if HARDWARE_VERSION == 1
    (void)cycleValue;
    (void)peakCommand;
    (void)plateauCommand;
    (void)peepCommand;
    (void)cpmCommand;
    (void)previousPeakPressure;
    (void)previousPlateauPressure;
    (void)previousPeepPressure;
    (void)currentAlarmCodes;
    (void)previousAlarmCodes;
#elif HARDWARE_VERSION == 2
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
    Serial6.write((uint8_t)1u);

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

// cppcheck-suppress unusedFunction
void sendAlarmTrap(uint16_t centileValue,
                   uint16_t pressureValue,
                   CyclePhases phase,
                   CycleSubPhases subPhase,
                   uint32_t cycleValue,
                   uint8_t alarmCode,
                   AlarmPriority alarmPriority,
                   bool triggered,
                   uint32_t expectedValue,
                   uint32_t measuredValue,
                   uint32_t cyclesSinceTriggerValue) {
#if HARDWARE_VERSION == 1
    (void)centileValue;
    (void)pressureValue;
    (void)phase;
    (void)subPhase;
    (void)cycleNumber;
    (void)alarmCode;
    (void)alarmPriority;
    (void)triggered;
    (void)expectedValue;
    (void)measuredValue;
    (void)cyclesSinceTrigger;
#elif HARDWARE_VERSION == 2
    uint8_t phaseValue;
    if ((phase == CyclePhases::INHALATION) && (subPhase == CycleSubPhases::INSPIRATION)) {
        phaseValue = 17u;  // 00010001
    } else if ((phase == CyclePhases::INHALATION)
               && (subPhase == CycleSubPhases::HOLD_INSPIRATION)) {
        phaseValue = 18u;  // 00010010
    } else if ((phase == CyclePhases::EXHALATION) && (subPhase == CycleSubPhases::EXHALE)) {
        phaseValue = 68u;  // 01000100
    } else {
        phaseValue = 0u;
    }

    uint8_t triggeredValue;
    if (triggered) {
        triggeredValue = 240u;  // 11110000
    } else {
        triggeredValue = 15u;  // 00001111
    }

    uint8_t alarmPriorityValue;
    if (alarmPriority == AlarmPriority::ALARM_HIGH) {
        alarmPriorityValue = 4u;  // 00000100
    } else if (alarmPriority == AlarmPriority::ALARM_MEDIUM) {
        alarmPriorityValue = 2u;  // 00000010
    } else if (alarmPriority == AlarmPriority::ALARM_LOW) {
        alarmPriorityValue = 1u;  // 00000001
    } else {
        alarmPriorityValue = 0u;  // 00000000
    }

    Serial6.write("T:");
    Serial6.write((uint8_t)1u);

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

    byte cycle[4];  // 32 bits
    cycle[0] = (cycleValue >> 24) & FIRST_BYTE;
    cycle[1] = (cycleValue >> 16) & FIRST_BYTE;
    cycle[2] = (cycleValue >> 8) & FIRST_BYTE;
    cycle[3] = cycleValue & FIRST_BYTE;
    Serial6.write(cycle, 4);

    Serial6.print("\t");
    Serial6.write(alarmCode);
    Serial6.print("\t");
    Serial6.write(alarmPriorityValue);
    Serial6.print("\t");
    Serial6.write(triggeredValue);

    Serial6.print("\t");

    byte expected[4];  // 32 bits
    expected[0] = (expectedValue >> 24) & FIRST_BYTE;
    expected[1] = (expectedValue >> 16) & FIRST_BYTE;
    expected[2] = (expectedValue >> 8) & FIRST_BYTE;
    expected[3] = expectedValue & FIRST_BYTE;
    Serial6.write(expected, 4);

    Serial6.print("\t");

    byte measured[4];  // 32 bits
    measured[0] = (measuredValue >> 24) & FIRST_BYTE;
    measured[1] = (measuredValue >> 16) & FIRST_BYTE;
    measured[2] = (measuredValue >> 8) & FIRST_BYTE;
    measured[3] = measuredValue & FIRST_BYTE;
    Serial6.write(measured, 4);

    Serial6.print("\t");

    byte cyclesSinceTrigger[4];  // 32 bits
    cyclesSinceTrigger[0] = (cyclesSinceTriggerValue >> 24) & FIRST_BYTE;
    cyclesSinceTrigger[1] = (cyclesSinceTriggerValue >> 16) & FIRST_BYTE;
    cyclesSinceTrigger[2] = (cyclesSinceTriggerValue >> 8) & FIRST_BYTE;
    cyclesSinceTrigger[3] = cyclesSinceTriggerValue & FIRST_BYTE;
    Serial6.write(cyclesSinceTrigger, 4);

    Serial6.print("\n");
#endif
}

#endif
