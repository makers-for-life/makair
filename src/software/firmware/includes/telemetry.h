/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file telemetry.h
 * @brief Implementation of the telemetry protocol
 *****************************************************************************/

#pragma once

#include <stdint.h>

#include "../includes/alarm_controller.h"
#include "../includes/cycle.h"

/// Prepare Serial6 to send telemetry data
void initTelemetry(void);

/// Send a "boot" message
void sendBootMessage(void);

/// Send a "stopped" message
void sendStoppedMessage(void);

/// Send a "data snapshot" message
void sendDataSnapshot(uint16_t centileValue,
                      uint16_t pressureValue,
                      CyclePhases phase,
                      CycleSubPhases subPhase,
                      uint8_t blowerValvePosition,
                      uint8_t patientValvePosition,
                      uint8_t blowerRpm,
                      uint8_t batteryLevel);

/// Send a "machine state snapshot" message
void sendMachineStateSnapshot(uint32_t cycleValue,
                              uint8_t peakCommand,
                              uint8_t plateauCommand,
                              uint8_t peepCommand,
                              uint8_t cpmCommand,
                              uint16_t previousPeakPressureValue,
                              uint16_t previousPlateauPressureValue,
                              uint16_t previousPeepPressureValue,
                              uint8_t currentAlarmCodes[ALARMS_SIZE]);

/// Send a "alarm trap" message
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
                   uint32_t cyclesSinceTriggerValue);

/**
 * Convert and round a pressure in mmH2O to a pressure in cmH2O
 *
 * @param pressure A pressure in mmH2O
 * @return A pressure in cmH2O
 */
uint8_t mmH2OtoCmH2O(uint16_t pressure);
