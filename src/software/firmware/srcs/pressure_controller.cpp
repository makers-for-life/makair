/*
    Copyright (C) 2020 Makers For Life
*/
/******************************************************************************
 * @author Makers For Life
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

static const int32_t INVALID_ERROR_MARKER = INT32_MIN;

// INITIALISATION =============================================================

PressureController pController;

// FUNCTIONS ==================================================================

PressureController::PressureController()
    : m_cyclesPerMinuteCommand(INITIAL_CYCLE_NUMBER),
      m_vigilance(false),
      m_minPeepCommand(DEFAULT_MIN_PEEP_COMMAND),                   // [mmH20]
      m_maxPlateauPressureCommand(DEFAULT_MAX_PLATEAU_COMMAND),     // [mmH20]
      m_maxPeakPressureCommand(DEFAULT_MAX_PEAK_PRESSURE_COMMAND),  // [mmH20]
      m_maxPeakPressure(CONST_MAX_PEAK_PRESSURE),                   // [mmH20]
      m_maxPlateauPressure(CONST_MAX_PLATEAU_PRESSURE),             // [mmH20]
      m_minPeep(CONST_MIN_PEEP_PRESSURE),                           // TODO revoir la valeur [mmH20]
      m_pressure(CONST_INITIAL_ZERO_PRESSURE),
      m_peakPressure(CONST_INITIAL_ZERO_PRESSURE),
      m_plateauPressure(CONST_INITIAL_ZERO_PRESSURE),
      m_peep(CONST_INITIAL_ZERO_PRESSURE),
      m_phase(CyclePhases::INHALATION),
      m_triggerHoldExpiDetectionTick(0),
      m_triggerHoldExpiDetectionTickDeletion(0),
      m_blower_increment(0) {
    computeCentiSecParameters();
}

PressureController::PressureController(int16_t p_cyclesPerMinute,
                                       int16_t p_minPeepCommand,
                                       int16_t p_maxPlateauPressure,
                                       int16_t p_maxPeakPressure,
                                       PressureValve p_blower_valve,
                                       PressureValve p_patient_valve,
                                       AlarmController p_alarmController,
                                       Blower* p_blower)
    : m_cyclesPerMinuteCommand(p_cyclesPerMinute),

      m_vigilance(false),
      m_minPeepCommand(p_minPeepCommand),
      m_maxPlateauPressureCommand(p_maxPlateauPressure),
      m_maxPeakPressureCommand(DEFAULT_MAX_PEAK_PRESSURE_COMMAND),
      m_cyclesPerMinute(p_cyclesPerMinute),
      m_maxPeakPressure(p_maxPeakPressure),
      m_maxPlateauPressure(p_maxPlateauPressure),
      m_minPeep(p_minPeepCommand),  // TODO revoir la valeur de démarage
      m_pressure(CONST_INITIAL_ZERO_PRESSURE),
      m_peakPressure(CONST_INITIAL_ZERO_PRESSURE),
      m_plateauPressure(CONST_INITIAL_ZERO_PRESSURE),
      m_peep(CONST_INITIAL_ZERO_PRESSURE),
      m_phase(CyclePhases::INHALATION),
      m_blower_valve(p_blower_valve),
      m_patient_valve(p_patient_valve),
      m_blower(p_blower),
      m_triggerHoldExpiDetectionTick(0),
      m_triggerHoldExpiDetectionTickDeletion(0),
      m_alarmController(p_alarmController),
      m_blower_increment(0) {
    computeCentiSecParameters();
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
    setSubPhase(CycleSubPhases::INSPIRATION);
    m_cycleNb++;

    // Reset PID integrals
    blowerIntegral = 0;
    blowerLastError = INVALID_ERROR_MARKER;
    patientIntegral = 0;
    patientLastError = INVALID_ERROR_MARKER;

    m_max_pressure = 0;
    computeCentiSecParameters();

    /*
    DBG_AFFICHE_CSPCYCLE_CSPINSPI(m_centiSecPerCycle, m_centiSecPerInhalation)
    */

    m_cyclesPerMinute = m_cyclesPerMinuteCommand;
    m_minPeep = m_minPeepCommand;
    m_maxPlateauPressure = m_maxPlateauPressureCommand;

    // Reset Tick Alarms to zero
    m_triggerHoldExpiDetectionTick = 0;
    m_triggerHoldExpiDetectionTickDeletion = 0;

    // Apply blower ramp-up
    m_blower->runSpeed(m_blower->getSpeed() + m_blower_increment);
    m_blower_increment = 0;
}

void PressureController::updatePressure(int16_t p_currentPressure) {
    m_pressure = p_currentPressure;
}

void PressureController::compute(uint16_t p_centiSec) {
    updateBlower(p_centiSec);

    // Update the cycle phase
    updatePhase(p_centiSec);

    if (!m_vigilance) {
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
        case CycleSubPhases::EXHALE: {
            exhale();
            break;
        }
        case CycleSubPhases::HOLD_EXHALE: {
            holdExhalation();
            break;
        }
        default: {
            holdExhalation();
        }
        }
    }
    safeguards(p_centiSec);

    DBG_PHASE_PRESSION(m_cycleNb, p_centiSec, 1u, m_phase, m_subPhase, m_pressure,
                       m_blower_valve.command, m_blower_valve.position, m_patient_valve.command,
                       m_patient_valve.position)

    executeCommands();

    m_alarmController.runAlarmEffects(p_centiSec);
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

    if (m_maxPlateauPressureCommand > CONST_MAX_PLATEAU_PRESSURE) {
        m_maxPlateauPressureCommand = CONST_MAX_PLATEAU_PRESSURE;
    }
}

void PressureController::onPeakPressureDecrease() {
    DBG_DO(Serial.println("Peak Pressure --");)

    m_maxPeakPressureCommand = m_maxPeakPressureCommand - 10u;

    if (m_maxPeakPressureCommand < CONST_MIN_PEAK_PRESSURE) {
        m_maxPeakPressureCommand = CONST_MIN_PEAK_PRESSURE;
    }
}

void PressureController::onPeakPressureIncrease() {
    DBG_DO(Serial.println("Peak Pressure ++");)

    m_maxPeakPressureCommand = m_maxPeakPressureCommand + 10u;

    if (m_maxPeakPressureCommand > CONST_MAX_PEAK_PRESSURE) {
        m_maxPeakPressureCommand = CONST_MAX_PEAK_PRESSURE;
    }
}
void PressureController::updateBlower(uint16_t p_centiSec) {
    m_max_pressure = max(m_max_pressure, m_pressure);

    // Case blower is too low
    if (m_phase == CyclePhases::INHALATION && (p_centiSec > (m_centiSecPerInhalation * 80u) / 100u)
        && (m_max_pressure < m_maxPeakPressureCommand)) {
        m_blower_increment = 5;
    }

    // Case blower is too high
    if (m_phase == CyclePhases::INHALATION && (p_centiSec < (m_centiSecPerInhalation * 20u) / 100u)
        && m_max_pressure > m_maxPeakPressureCommand) {
        m_blower_increment = -5;
    }
}

void PressureController::updatePhase(uint16_t p_centiSec) {
    if (p_centiSec < m_centiSecPerInhalation) {
        m_phase = CyclePhases::INHALATION;

        if ((p_centiSec < ((m_centiSecPerInhalation * 80u) / 100u))
            && (m_pressure < m_maxPeakPressureCommand)) {
            if (m_subPhase != CycleSubPhases::HOLD_INSPIRATION) {
                m_consignePression = m_maxPeakPressureCommand;
                setSubPhase(CycleSubPhases::INSPIRATION);
            }
        } else {
            m_consignePression = m_maxPlateauPressureCommand;
            setSubPhase(CycleSubPhases::HOLD_INSPIRATION);
        }
    } else {
        m_phase = CyclePhases::EXHALATION;
        m_consignePression = m_minPeepCommand;

        if (m_subPhase != CycleSubPhases::HOLD_EXHALE) {
            setSubPhase(CycleSubPhases::EXHALE);
        }
    }
}

void PressureController::inhale() {
    // Open the air stream towards the patient's lungs
    m_blower_valve.open(pidBlower(m_consignePression, m_pressure, m_dt));

    // Open the air stream towards the patient's lungs
    m_patient_valve.close();

    // TODO vérifier si la pression de crête est toujours la dernière pression mesurée sur la
    // sous-phase INSPI

    // Update the peak pressure
    m_peakPressure = m_pressure;
}

void PressureController::plateau() {
    // Deviate the air stream outside
    m_blower_valve.close();

    // Close the air stream towards the patient's lungs
    m_patient_valve.close();
    // Update the plateau pressure
    m_plateauPressure = m_pressure;
}

void PressureController::exhale() {
    // Deviate the air stream outside
    m_blower_valve.close();

    // Open the valve so the patient can exhale outside
    m_patient_valve.open(pidPatient(m_consignePression, m_pressure, m_dt));

    // Update the PEEP
    m_peep = m_pressure;
}

void PressureController::holdExhalation() {
    // Deviate the air stream outside
    m_blower_valve.close();

    // Close the valve so the patient can exhale outside
    m_patient_valve.close();
}

void PressureController::updateDt(int32_t p_dt) { m_dt = p_dt; }

void PressureController::safeguards(uint16_t p_centiSec) {
    safeguardPlateau(p_centiSec);
    safeguardHoldExpiration(p_centiSec);

    if (m_pressure < ALARM_2_CMH2O) {
        m_alarmController.detectedAlarm(RCM_SW_2, m_cycleNb);
    } else {
        m_alarmController.notDetectedAlarm(RCM_SW_2);
    }

    if (m_pressure > ALARM_35_CMH2O) {
        m_alarmController.detectedAlarm(RCM_SW_1, m_cycleNb);
    } else {
        m_alarmController.notDetectedAlarm(RCM_SW_1);
    }
}

void PressureController::safeguardPressionCrete(uint16_t p_centiSec) {
    if (m_subPhase == CycleSubPhases::INSPIRATION) {
        if (m_pressure >= (m_maxPeakPressureCommand - 30u)) {
            // m_blower_valve.ouvrirIntermediaire();
            m_vigilance = true;
        }

        if (m_pressure >= m_maxPeakPressureCommand) {
            setSubPhase(CycleSubPhases::HOLD_INSPIRATION);
            plateau();
        }
    }

    if (m_pressure >= (m_maxPeakPressureCommand + 10u)) {
        // m_patient.augmenterOuverture();
    }
}

void PressureController::safeguardPlateau(uint16_t p_centiSec) {
    if (m_subPhase == CycleSubPhases::HOLD_INSPIRATION) {
        if (m_pressure < ALARM_THRESHOLD_PLATEAU_UNDER_2_CMH2O) {
            m_alarmController.detectedAlarm(RCM_SW_19, m_cycleNb);
        } else {
            m_alarmController.notDetectedAlarm(RCM_SW_19);
        }

        if (m_pressure > ALARM_THRESHOLD_PLATEAU_ABOVE_80_CMH2O) {
            m_alarmController.detectedAlarm(RCM_SW_18, m_cycleNb);
        } else {
            m_alarmController.notDetectedAlarm(RCM_SW_18);
        }

        uint16_t minPlateauBeforeAlarm = 80u * m_maxPlateauPressureCommand / 100u;
        uint16_t maxPlateauBeforeAlarm = 120u * m_maxPlateauPressureCommand / 100u;
        if (m_pressure < minPlateauBeforeAlarm || m_pressure > maxPlateauBeforeAlarm) {
            m_alarmController.detectedAlarm(RCM_SW_14, m_cycleNb);
        } else {
            m_alarmController.notDetectedAlarm(RCM_SW_14);
        }
    }
}

void PressureController::safeguardHoldExpiration(uint16_t p_centiSec) {
    if (m_phase == CyclePhases::EXHALATION) {
        uint16_t minPeepBeforeAlarm =
            m_minPeepCommand - ALARM_THRESHOLD_PEEP_ABOVE_OR_UNDER_2_CMH2O;
        uint16_t maxPeepBeforeAlarm =
            m_minPeepCommand + ALARM_THRESHOLD_PEEP_ABOVE_OR_UNDER_2_CMH2O;
        if (m_pressure < minPeepBeforeAlarm || m_pressure > maxPeepBeforeAlarm) {
            m_alarmController.detectedAlarm(RCM_SW_3, m_cycleNb);
            m_alarmController.detectedAlarm(RCM_SW_15, m_cycleNb);
        } else {
            m_alarmController.notDetectedAlarm(RCM_SW_3);
            m_alarmController.notDetectedAlarm(RCM_SW_15);
        }
    }
}

void PressureController::safeguardMaintienPeep(uint16_t p_centiSec) {
    if (m_pressure <= m_minPeepCommand) {
        // m_blower.augmenterOuverture();
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

void PressureController::setSubPhase(CycleSubPhases p_subPhase) {
    m_subPhase = p_subPhase;
    m_vigilance = false;
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
    // Increase target pressure by 20mm H2O for safety, to ensure from going below the target
    // pressure
    int32_t error = targetPressure + 20 - currentPressure;

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
