/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file pressure_controller.cpp
 * @brief Core logic to control the breathing cycle
 *****************************************************************************/

#pragma once

// INCLUDES ===================================================================

// Associated header
#include "../includes/pressure_controller.h"

// External
#include <algorithm>

// Internal
#include "../includes/alarm_controller.h"
#include "../includes/config.h"
#include "../includes/debug.h"
#include "../includes/parameters.h"
#include "../includes/pressure_valve.h"
#if HARDWARE_VERSION == 2
#include "../includes/battery.h"
#include "../includes/telemetry.h"
#endif

static const int32_t INVALID_ERROR_MARKER = INT32_MIN;

// INITIALISATION =============================================================

PressureController pController;

// FUNCTIONS ==================================================================

PressureController::PressureController()
    : m_cyclesPerMinuteCommand(INITIAL_CYCLE_NUMBER),
      m_minPeepCommand(DEFAULT_MIN_PEEP_COMMAND),                   // [mmH20]
      m_maxPlateauPressureCommand(DEFAULT_MAX_PLATEAU_COMMAND),     // [mmH20]
      m_maxPeakPressureCommand(DEFAULT_MAX_PEAK_PRESSURE_COMMAND),  // [mmH20]
      m_cyclesPerMinute(INITIAL_CYCLE_NUMBER),
      m_maxPeakPressure(CONST_MAX_PEAK_PRESSURE),        // [mmH20]
      m_maxPlateauPressure(CONST_MAX_PLATEAU_PRESSURE),  // [mmH20]
      m_minPeep(CONST_MIN_PEEP_PRESSURE),
      m_pressure(CONST_INITIAL_ZERO_PRESSURE),
      m_peakPressure(CONST_INITIAL_ZERO_PRESSURE),
      m_plateauPressure(CONST_INITIAL_ZERO_PRESSURE),
      m_peep(CONST_INITIAL_ZERO_PRESSURE),
      m_phase(CyclePhases::INHALATION),
      m_subPhase(CycleSubPhases::INSPIRATION),
      m_blower(nullptr),
      m_alarmController(nullptr),
      m_blower_increment(0),
      m_cycleNb(0),
      m_dt(0),
      m_pressureCommand(0),
      blowerIntegral(0),
      blowerLastError(0),
      patientIntegral(0),
      patientLastError(0),
      m_startPlateauComputation(false),
      m_plateauComputed(false),
      m_lastPressureValuesIndex(0),
      m_sumOfPressures(0),
      m_numberOfPressures(0),
      m_plateauStartTime(0u) {
    computeCentiSecParameters();
    for (uint8_t i = 0; i < MAX_PRESSURE_SAMPLES; i++) {
        m_lastPressureValues[i] = 0;
    }
}

PressureController::PressureController(int16_t p_cyclesPerMinute,
                                       int16_t p_minPeepCommand,
                                       int16_t p_maxPlateauPressure,
                                       int16_t p_maxPeakPressure,
                                       const PressureValve& p_blower_valve,
                                       const PressureValve& p_patient_valve,
                                       AlarmController* p_alarmController,
                                       Blower* p_blower)
    : m_cyclesPerMinuteCommand(p_cyclesPerMinute),
      m_minPeepCommand(p_minPeepCommand),
      m_maxPlateauPressureCommand(p_maxPlateauPressure),
      m_maxPeakPressureCommand(DEFAULT_MAX_PEAK_PRESSURE_COMMAND),
      m_cyclesPerMinute(p_cyclesPerMinute),
      m_maxPeakPressure(p_maxPeakPressure),
      m_maxPlateauPressure(p_maxPlateauPressure),
      m_minPeep(p_minPeepCommand),
      m_pressure(CONST_INITIAL_ZERO_PRESSURE),
      // cppcheck-suppress misra-c2012-12.3
      m_peakPressure(CONST_INITIAL_ZERO_PRESSURE),
      m_plateauPressure(CONST_INITIAL_ZERO_PRESSURE),
      m_peep(CONST_INITIAL_ZERO_PRESSURE),
      m_phase(CyclePhases::INHALATION),
      m_subPhase(CycleSubPhases::INSPIRATION),
      // cppcheck-suppress misra-c2012-12.3
      m_blower_valve(p_blower_valve),
      m_patient_valve(p_patient_valve),
      m_blower(p_blower),
      m_alarmController(p_alarmController),
      m_blower_increment(0),
      m_cycleNb(0),
      m_dt(0),
      m_pressureCommand(0),
      blowerIntegral(0),
      blowerLastError(0),
      patientIntegral(0),
      patientLastError(0),
      m_startPlateauComputation(false),
      m_plateauComputed(false),
      m_lastPressureValuesIndex(0),
      m_sumOfPressures(0),
      m_numberOfPressures(0),
      m_plateauStartTime(0u) {
    computeCentiSecParameters();
    for (uint8_t i = 0; i < MAX_PRESSURE_SAMPLES; i++) {
        m_lastPressureValues[i] = 0;
    }
}

void PressureController::setup() {
    DBG_DO(Serial.println(VERSION);)
    DBG_DO(Serial.println("mise en secu initiale");)

    m_blower_valve.close();
    m_patient_valve.close();

    m_blower_valve.execute();
    m_patient_valve.execute();

    m_peakPressure = 0;
    m_plateauPressure = 0;
    m_peep = 0;

    m_cycleNb = 0;
}

void PressureController::initRespiratoryCycle() {
    m_phase = CyclePhases::INHALATION;
    setSubPhase(CycleSubPhases::INSPIRATION, 0);
    m_cycleNb++;
    m_plateauPressure = 0;

    // Reset PID integrals
    blowerIntegral = 0;
    blowerLastError = INVALID_ERROR_MARKER;
    patientIntegral = 0;
    patientLastError = INVALID_ERROR_MARKER;

    m_peakPressure = 0;
    computeCentiSecParameters();

    DBG_AFFICHE_CSPCYCLE_CSPINSPI(m_centiSecPerCycle, m_centiSecPerInhalation)

    m_cyclesPerMinute = m_cyclesPerMinuteCommand;
    m_minPeep = m_minPeepCommand;
    m_maxPlateauPressure = m_maxPlateauPressureCommand;

    // Apply blower ramp-up
    m_blower->runSpeed(m_blower->getSpeed() + m_blower_increment);
    m_blower_increment = 0;

    for (uint8_t i = 0; i < MAX_PRESSURE_SAMPLES; i++) {
        m_lastPressureValues[i] = 0;
    }
    m_lastPressureValuesIndex = 0;
    m_startPlateauComputation = false;
    m_plateauComputed = false;

    m_sumOfPressures = 0u;
    m_numberOfPressures = 0u;

    m_plateauStartTime = 0u;
}

void PressureController::endRespiratoryCycle() {
    updatePeakPressure();
    checkCycleAlarm();

    // If plateau is not detected or is too close to PEEP, mark it as "unknown"
    if ((m_plateauPressure == 0u) || (abs(m_plateauPressure - m_peep) < 10)) {
        m_plateauPressure = UINT16_MAX;
    }

    // RCM-SW-18
    if (m_pressure <= ALARM_THRESHOLD_MAX_PRESSURE) {
        m_alarmController->notDetectedAlarm(RCM_SW_18);
    }

#if HARDWARE_VERSION == 2
    uint16_t plateauPressureToDisplay = m_plateauPressure;
    if (plateauPressureToDisplay == UINT16_MAX) {
        plateauPressureToDisplay = 0;
    }

    sendMachineStateSnapshot(m_cycleNb, mmH2OtoCmH2O(m_maxPeakPressureCommand),
                             mmH2OtoCmH2O(m_maxPlateauPressureCommand),
                             mmH2OtoCmH2O(m_minPeepCommand), m_cyclesPerMinuteCommand,
                             m_peakPressure, plateauPressureToDisplay, m_peep,
                             m_alarmController->triggeredAlarms());
#endif
}

void PressureController::updatePressure(int16_t p_currentPressure) {
    m_pressure = p_currentPressure;

    // Store the current pressure to compute aggregates
    m_lastPressureValues[m_lastPressureValuesIndex] = p_currentPressure;
    m_lastPressureValuesIndex++;

    // Start over if we reached the max samples number
    if (m_lastPressureValuesIndex >= MAX_PRESSURE_SAMPLES) {
        m_lastPressureValuesIndex = 0;
    }
}

void PressureController::compute(uint16_t p_centiSec) {
    // Update the cycle phase
    updatePhase(p_centiSec);

    // Compute metrics for alarms
    m_sumOfPressures += m_pressure;
    m_numberOfPressures++;

    // Act accordingly
    switch (m_subPhase) {
    case CycleSubPhases::INSPIRATION: {
        inhale();
        break;
    }
    case CycleSubPhases::HOLD_INSPIRATION: {
        plateau();
        break;
    }
    case CycleSubPhases::EXHALE:
    default: {
        exhale();
        // Plateau happens with delay related to the pressure command
        computePlateau(p_centiSec);
        break;
    }
    }

    // RCM-SW-18
    if (m_pressure > ALARM_THRESHOLD_MAX_PRESSURE) {
        m_alarmController->detectedAlarm(RCM_SW_18, m_cycleNb, ALARM_THRESHOLD_MAX_PRESSURE,
                                         m_pressure);
    }

    DBG_PHASE_PRESSION(m_cycleNb, p_centiSec, 1u, m_phase, m_subPhase, m_pressure,
                       m_blower_valve.command, m_blower_valve.position, m_patient_valve.command,
                       m_patient_valve.position)

#if HARDWARE_VERSION == 2
    m_alarmController->updateCoreData(p_centiSec, m_pressure, m_phase, m_subPhase, m_cycleNb);
    sendDataSnapshot(p_centiSec, m_pressure, m_phase, m_subPhase, m_blower_valve.position,
                     m_patient_valve.position, m_blower->getSpeed(), getBatteryLevel());
#endif

    executeCommands();
}

void PressureController::computePlateau(uint16_t p_centiSec) {
    uint16_t minValue = m_lastPressureValues[0u];
    uint16_t maxValue = m_lastPressureValues[0u];
    uint16_t totalValues = m_lastPressureValues[0u];

    for (uint8_t index = 1u; index < MAX_PRESSURE_SAMPLES; index++) {
        minValue = min(minValue, m_lastPressureValues[index]);
        maxValue = max(maxValue, m_lastPressureValues[index]);
        totalValues += m_lastPressureValues[index];
    }

    uint16_t diff = (maxValue - minValue);

    // Start computing plateau pressure when:
    // - the last pressure values were close enough
    // - the hold inspiration phase is about to end
    // - plateau pressure computation was not already started
    if (!m_plateauComputed && (diff < 10u)
        && (p_centiSec >= ((m_centiSecPerInhalation * 95u) / 100u))) {
        m_startPlateauComputation = true;
    }

    // Stop computing plateau pressure when pressure drops
    if (m_startPlateauComputation && (diff > 10u)) {
        m_startPlateauComputation = false;
        m_plateauComputed = true;
    }

    if (m_startPlateauComputation) {
        m_plateauPressure = totalValues / MAX_PRESSURE_SAMPLES;
    }
}

void PressureController::onCycleDecrease() {
    DBG_DO(Serial.println("Cycle --");)

    m_cyclesPerMinuteCommand--;

    if (m_cyclesPerMinuteCommand < CONST_MIN_CYCLE) {
        m_cyclesPerMinuteCommand = CONST_MIN_CYCLE;
    }
}

void PressureController::onCycleIncrease() {
#if SIMULATION != 1
    // During simulation without electronic board there is a noise on the button pin. It increases
    // the cycle and the simulation fail.
    DBG_DO(Serial.println("Cycle ++");)

    m_cyclesPerMinuteCommand++;

    if (m_cyclesPerMinuteCommand > CONST_MAX_CYCLE) {
        m_cyclesPerMinuteCommand = CONST_MAX_CYCLE;
    }

#endif
}

void PressureController::onPeepPressureDecrease() {
    DBG_DO(Serial.println("Peep Pressure --");)

    m_minPeepCommand = m_minPeepCommand - 10u;

    if (m_minPeepCommand < CONST_MIN_PEEP_PRESSURE) {
        m_minPeepCommand = CONST_MIN_PEEP_PRESSURE;
    }
}

void PressureController::onPeepPressureIncrease() {
    DBG_DO(Serial.println("Peep Pressure ++");)

    m_minPeepCommand = m_minPeepCommand + 10u;

    if (m_minPeepCommand > CONST_MAX_PEEP_PRESSURE) {
        m_minPeepCommand = CONST_MAX_PEEP_PRESSURE;
    }
}

void PressureController::onPlateauPressureDecrease() {
    DBG_DO(Serial.println("Plateau Pressure --");)

    m_maxPlateauPressureCommand = m_maxPlateauPressureCommand - 10u;

    if (m_maxPlateauPressureCommand < CONST_MIN_PLATEAU_PRESSURE) {
        m_maxPlateauPressureCommand = CONST_MIN_PLATEAU_PRESSURE;
    }
}

void PressureController::onPlateauPressureIncrease() {
    DBG_DO(Serial.println("Plateau Pressure ++");)

    m_maxPlateauPressureCommand = m_maxPlateauPressureCommand + 10u;

    if (m_maxPlateauPressureCommand
        > min(m_maxPeakPressureCommand, static_cast<uint16_t>(CONST_MAX_PLATEAU_PRESSURE))) {
        m_maxPlateauPressureCommand =
            min(m_maxPeakPressureCommand, static_cast<uint16_t>(CONST_MAX_PLATEAU_PRESSURE));
    }
}

void PressureController::onPeakPressureDecrease(uint8_t p_decrement) {
    DBG_DO(Serial.println("Peak Pressure --");)

    m_maxPeakPressureCommand = m_maxPeakPressureCommand - p_decrement;

    if (m_maxPeakPressureCommand
        < max(m_maxPlateauPressureCommand, static_cast<uint16_t>(CONST_MIN_PEAK_PRESSURE))) {
        m_maxPeakPressureCommand =
            max(m_maxPlateauPressureCommand, static_cast<uint16_t>(CONST_MIN_PEAK_PRESSURE));
    }
}

void PressureController::onPeakPressureIncrease(uint8_t p_increment) {
    DBG_DO(Serial.println("Peak Pressure ++");)

    m_maxPeakPressureCommand = m_maxPeakPressureCommand + p_increment;

    if (m_maxPeakPressureCommand > CONST_MAX_PEAK_PRESSURE) {
        m_maxPeakPressureCommand = CONST_MAX_PEAK_PRESSURE;
    }
}

void PressureController::updatePhase(uint16_t p_centiSec) {
    if (p_centiSec < m_centiSecPerInhalation) {
        m_phase = CyclePhases::INHALATION;

        if ((p_centiSec < ((m_centiSecPerInhalation * 80u) / 100u))
            && (m_pressure < (m_maxPeakPressureCommand - 10u))) {
            if (m_subPhase != CycleSubPhases::HOLD_INSPIRATION) {
                m_pressureCommand = m_maxPeakPressureCommand;
                setSubPhase(CycleSubPhases::INSPIRATION, p_centiSec);
            }
        } else {
            m_pressureCommand = m_maxPlateauPressureCommand;
            setSubPhase(CycleSubPhases::HOLD_INSPIRATION, p_centiSec);
        }
    } else {
        m_phase = CyclePhases::EXHALATION;
        m_pressureCommand = m_minPeepCommand;

        setSubPhase(CycleSubPhases::EXHALE, p_centiSec);
    }
}

void PressureController::inhale() {
    // Open the air stream towards the patient's lungs
    m_blower_valve.open(pidBlower(m_pressureCommand, m_pressure, m_dt));

    // Open the air stream towards the patient's lungs
    m_patient_valve.close();

    // Update the peak pressure
    m_peakPressure = max(m_pressure, m_peakPressure);
}

void PressureController::plateau() {
    // Deviate the air stream outside
    m_blower_valve.close();

    // Close the air stream towards the patient's lungs
    m_patient_valve.close();

    // Update the peak pressure
    m_peakPressure = max(m_pressure, m_peakPressure);
}

void PressureController::exhale() {
    // Deviate the air stream outside
    m_blower_valve.close();

    // Open the valve so the patient can exhale outside
    m_patient_valve.open(pidPatient(m_pressureCommand, m_pressure, m_dt));

    // Update the PEEP
    m_peep = m_pressure;
}

void PressureController::updateDt(int32_t p_dt) { m_dt = p_dt; }

void PressureController::updatePeakPressure() {
    int16_t plateauDelta = m_maxPlateauPressureCommand - m_plateauPressure;
    int16_t peakDelta = m_maxPeakPressureCommand - m_peakPressure;

    DBG_DO(Serial.println("updatePeakPressure");)

    // If a plateau was detected
    if ((m_plateauPressure > 0u) && (m_plateauPressure < UINT16_MAX)) {
        DBG_DO(Serial.println("Plateau detected");)

        if (abs(plateauDelta) > 20) {
            m_maxPeakPressureCommand =
                min((min(m_peakPressure, m_maxPeakPressureCommand) + plateauDelta),
                    static_cast<int>(CONST_MAX_PEAK_PRESSURE));
        } else if ((abs(plateauDelta) < 20) && (abs(plateauDelta) > 5)) {
            m_maxPeakPressureCommand =
                min(min(m_peakPressure, m_maxPeakPressureCommand) + (plateauDelta / 2),
                    static_cast<int>(CONST_MAX_PEAK_PRESSURE));
        } else {
            // Do nothing
        }

        DBG_DO(Serial.print("Peak command:");)
        DBG_DO(Serial.println(m_maxPeakPressureCommand);)

        DBG_DO(Serial.print("m_plateauStartTime:");)
        DBG_DO(Serial.println(m_plateauStartTime);)

        // If plateau was reached quite early
        if (m_plateauStartTime < ((m_centiSecPerInhalation * 30u) / 100u)) {
            DBG_DO(Serial.println("BLOWER -20");)

            // If the peak delta is high, decrease blower's speed a lot
            if (peakDelta < -20) {
                m_blower_increment = -60;
                DBG_DO(Serial.print("BLOWER -60, peak: ");)
                DBG_DO(Serial.println(peakDelta);)
            } else {
                m_blower_increment = -20;
                DBG_DO(Serial.println("BLOWER -20");)
            }
        } else if ((m_plateauStartTime >= ((m_centiSecPerInhalation * 30u) / 100u))
                   && (m_plateauStartTime < ((m_centiSecPerInhalation * 40u) / 100u))) {
            DBG_DO(Serial.println("BLOWER -10");)
            m_blower_increment = -10;
        } else if ((m_plateauStartTime > ((m_centiSecPerInhalation * 60u) / 100u))
                   && (m_plateauStartTime <= ((m_centiSecPerInhalation * 70u) / 100u))
                   && abs(plateauDelta) > 10) {
            DBG_DO(Serial.println("BLOWER +10");)
            m_blower_increment = +10;
        } else if (m_plateauStartTime > ((m_centiSecPerInhalation * 70u) / 100u)) {
            if (peakDelta > 60) {
                m_blower_increment = +60;
                DBG_DO(Serial.print("BLOWER +60, peak: ");)
                DBG_DO(Serial.println(peakDelta);)
            } else if (peakDelta > 40) {
                m_blower_increment = +40;
                DBG_DO(Serial.print("BLOWER +40, peak: ");)
                DBG_DO(Serial.println(peakDelta);)
            } else {
                m_blower_increment = +20;
                DBG_DO(Serial.println("BLOWER +20");)
            }
        } else {
            m_blower_increment = 0;
            DBG_DO(Serial.println("BLOWER +0");)
        }
    } else {  // If no plateau was detected: only do blower ramp-up
        DBG_DO(Serial.println("Plateau not detected");)

        if (m_plateauStartTime < ((m_centiSecPerInhalation * 30u) / 100u)) {
            DBG_DO(Serial.println("BLOWER -40");)
            m_blower_increment = -40;
        } else if ((m_plateauStartTime >= ((m_centiSecPerInhalation * 30u) / 100u))
                   && (m_plateauStartTime < ((m_centiSecPerInhalation * 40u) / 100u))) {
            DBG_DO(Serial.println("BLOWER -10");)
            m_blower_increment = -10;
        } else if ((m_plateauStartTime > ((m_centiSecPerInhalation * 60u) / 100u))
                   && (m_plateauStartTime <= ((m_centiSecPerInhalation * 70u) / 100u))) {
            DBG_DO(Serial.println("BLOWER +10");)
            m_blower_increment = +10;
        } else if (m_plateauStartTime > ((m_centiSecPerInhalation * 70u) / 100u)) {
            DBG_DO(Serial.println("BLOWER +40");)
            m_blower_increment = +40;
        } else {
            m_blower_increment = 0;
            DBG_DO(Serial.println("BLOWER +0");)
        }
    }
}

void PressureController::computeCentiSecParameters() {
    m_centiSecPerCycle = 60u * 100u / m_cyclesPerMinute;
    // Inhalation = 1/3 of the cycle duration,
    // Exhalation = 2/3 of the cycle duration
    m_centiSecPerInhalation = m_centiSecPerCycle / 3u;
}

void PressureController::executeCommands() {
    m_blower_valve.execute();
    m_patient_valve.execute();
}

void PressureController::checkCycleAlarm() {
    // RCM-SW-1 + RCM-SW-14 : Check if plateau is reached
    uint16_t minPlateauBeforeAlarm =
        (m_maxPlateauPressureCommand * (100u - ALARM_THRESHOLD_DIFFERENCE_PERCENT)) / 100u;
    uint16_t maxPlateauBeforeAlarm =
        (m_maxPlateauPressureCommand * (100u + ALARM_THRESHOLD_DIFFERENCE_PERCENT)) / 100u;
    if ((m_plateauPressure < minPlateauBeforeAlarm)
        || (m_plateauPressure > maxPlateauBeforeAlarm)) {
        m_alarmController->detectedAlarm(RCM_SW_1, m_cycleNb, m_maxPlateauPressureCommand,
                                         m_pressure);
        m_alarmController->detectedAlarm(RCM_SW_14, m_cycleNb, m_maxPlateauPressureCommand,
                                         m_pressure);
    } else {
        m_alarmController->notDetectedAlarm(RCM_SW_1);
        m_alarmController->notDetectedAlarm(RCM_SW_14);
    }

    // RCM-SW-2 + RCM-SW-19 : Check is mean pressure was < 2 cmH2O
    uint16_t meanPressure = m_sumOfPressures / m_numberOfPressures;
    if (meanPressure <= ALARM_THRESHOLD_MIN_PRESSURE) {
        m_alarmController->detectedAlarm(RCM_SW_2, m_cycleNb, ALARM_THRESHOLD_MIN_PRESSURE,
                                         m_pressure);
        m_alarmController->detectedAlarm(RCM_SW_19, m_cycleNb, ALARM_THRESHOLD_MIN_PRESSURE,
                                         m_pressure);
    } else {
        m_alarmController->notDetectedAlarm(RCM_SW_2);
        m_alarmController->notDetectedAlarm(RCM_SW_19);
    }

    // RCM-SW-3 + RCM-SW-15
    uint16_t minPeepBeforeAlarm = m_minPeepCommand - ALARM_THRESHOLD_DIFFERENCE_PRESSURE;
    uint16_t maxPeepBeforeAlarm = m_minPeepCommand + ALARM_THRESHOLD_DIFFERENCE_PRESSURE;
    if ((m_peep < minPeepBeforeAlarm) || (m_peep > maxPeepBeforeAlarm)) {
        m_alarmController->detectedAlarm(RCM_SW_3, m_cycleNb, m_minPeepCommand, m_pressure);
        m_alarmController->detectedAlarm(RCM_SW_15, m_cycleNb, m_minPeepCommand, m_pressure);
    } else {
        m_alarmController->notDetectedAlarm(RCM_SW_3);
        m_alarmController->notDetectedAlarm(RCM_SW_15);
    }
}

void PressureController::setSubPhase(CycleSubPhases p_subPhase, uint16_t p_centiSec) {
    if ((m_subPhase == CycleSubPhases::INSPIRATION)
        && (p_subPhase == CycleSubPhases::HOLD_INSPIRATION)) {
        m_plateauStartTime = p_centiSec;
    }
    m_subPhase = p_subPhase;
}

int32_t PressureController::pidBlower(int32_t targetPressure, int32_t currentPressure, int32_t dt) {
    // Compute error
    int32_t error = targetPressure - currentPressure;

    // Compute integral
    blowerIntegral = blowerIntegral + ((PID_BLOWER_KI * error * dt) / 1000000);
    blowerIntegral = max(PID_BLOWER_INTEGRAL_MIN, min(PID_BLOWER_INTEGRAL_MAX, blowerIntegral));

    // Compute derivative
    int32_t derivative = ((blowerLastError == INVALID_ERROR_MARKER) || (dt == 0))
                             ? 0
                             : ((1000000 * (error - blowerLastError)) / dt);
    blowerLastError = error;

    int32_t blowerCommand = (PID_BLOWER_KP * error) + blowerIntegral
                            + ((PID_BLOWER_KD * derivative) / 1000);  // Command computation

    int32_t minAperture = m_blower_valve.minAperture();
    int32_t maxAperture = m_blower_valve.maxAperture();

    uint32_t blowerAperture =
        max(minAperture,
            min(maxAperture, maxAperture + (minAperture - maxAperture) * blowerCommand / 1000));

    return blowerAperture;
}

int32_t
PressureController::pidPatient(int32_t targetPressure, int32_t currentPressure, int32_t dt) {
    // Compute error
    int32_t error = targetPressure + PID_PATIENT_SAFETY_PEEP_OFFSET - currentPressure;

    // Compute integral
    patientIntegral = patientIntegral + ((PID_PATIENT_KI * error * dt) / 1000000);
    patientIntegral = max(PID_PATIENT_INTEGRAL_MIN, min(PID_PATIENT_INTEGRAL_MAX, patientIntegral));

    // Compute derivative
    int32_t derivative = ((patientLastError == INVALID_ERROR_MARKER) || (dt == 0))
                             ? 0
                             : ((1000000 * (error - patientLastError)) / dt);
    patientLastError = error;

    int32_t patientCommand = (PID_PATIENT_KP * error) + patientIntegral
                             + ((PID_PATIENT_KD * derivative) / 1000);  // Command computation

    int32_t minAperture = m_blower_valve.minAperture();
    int32_t maxAperture = m_blower_valve.maxAperture();

    uint32_t patientAperture =
        max(minAperture,
            min(maxAperture, maxAperture + (maxAperture - minAperture) * patientCommand / 1000));

    return patientAperture;
}
