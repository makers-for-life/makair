/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file telemetry.h
 * @brief Implementation of the telemetry protocol
 *****************************************************************************/

#pragma once

#include <stdint.h>
#include "../includes/pressure_controller.h"

void initTelemetry(void);

void sendDataSnapshot(uint16_t centileValue,
                      uint16_t pressureValue,
                      CyclePhases phase,
                      CycleSubPhases subPhase,
                      uint8_t blowerValvePosition,
                      uint8_t patientValvePosition,
                      uint8_t blowerRpm,
                      uint8_t batteryLevel);

void sendMachineStateSnapshot(uint32_t cycleValue,
                              uint8_t peakCommand,
                              uint8_t plateauCommand,
                              uint8_t peepCommand,
                              uint8_t cpmCommand,
                              uint8_t previousPeakPressure,
                              uint8_t previousPlateauPressure,
                              uint8_t previousPeepPressure,
                              uint8_t currentAlarmCodes[ALARMS_SIZE],
                              uint8_t previousAlarmCodes[ALARMS_SIZE]);
