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

// External libraries
#include <algorithm>

// Internal libraries
#include "../includes/alarm_controller.h"
#include "../includes/buzzer.h"
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
      m_triggerHoldExpiDetectionTickDeletion(0) {
    computeCentiSecParameters();
}

PressureController::PressureController(int16_t p_cyclesPerMinute,
                                       int16_t p_minPeepCommand,
                                       int16_t p_maxPlateauPressure,
                                       int16_t p_maxPeakPressure,
                                       PressureValve p_blower,
                                       PressureValve p_patient,
                                       AlarmController p_alarmController)
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
      m_blower(p_blower),
      m_patient(p_patient),
      m_triggerHoldExpiDetectionTick(0),
      m_triggerHoldExpiDetectionTickDeletion(0),
      m_alarmController(p_alarmController) {
    computeCentiSecParameters();
}

void PressureController::setup() {
    DBG_DO(Serial.println(VERSION);)
    DBG_DO(Serial.println("mise en secu initiale");)

    m_blower.close();
    m_patient.close();

    m_blower.execute();
    m_patient.execute();

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
}

void PressureController::updatePressure(int16_t p_currentPressure) {
    m_pressure = p_currentPressure;
}

void PressureController::compute(uint16_t p_centiSec) {
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

    DBG_PHASE_PRESSION(m_cycleNb, p_centiSec, 1u, m_phase, m_subPhase, m_pressure, m_blower.command,
                       m_blower.position, m_patient.command, m_patient.position)

    executeCommands();

    m_alarmController.manageAlarm();
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
    m_blower.open(pidBlower(m_consignePression, m_pressure, m_dt));

    // Open the air stream towards the patient's lungs
    m_patient.close();

    // TODO vérifier si la pression de crête est toujours la dernière pression mesurée sur la
    // sous-phase INSPI

    // Update the peak pressure
    m_peakPressure = m_pressure;
}

void PressureController::plateau() {
    // Deviate the air stream outside
    m_blower.close();

    // Close the air stream towards the patient's lungs
    m_patient.close();
    // Update the plateau pressure
    m_plateauPressure = m_pressure;
}

void PressureController::exhale() {
    // Deviate the air stream outside
    m_blower.close();

    // Open the valve so the patient can exhale outside
    m_patient.open(pidPatient(m_consignePression, m_pressure, m_dt));

    // Update the PEEP
    m_peep = m_pressure;
}

void PressureController::holdExhalation() {
    // Deviate the air stream outside
    m_blower.close();

    // Close the valve so the patient can exhale outside
    m_patient.close();
}

void PressureController::updateDt(int32_t p_dt) { m_dt = p_dt; }

void PressureController::safeguards(uint16_t p_centiSec) {
    // TODO rework safeguards
    // safeguardPressionCrete(p_centiSec);
    // safeguardPressionPlateau(p_centiSec);
    // safeguardHoldExpiration(p_centiSec);
    // safeguardMaintienPeep(p_centiSec);
}

void PressureController::safeguardPressionCrete(uint16_t p_centiSec) {
    if (m_subPhase == CycleSubPhases::INSPIRATION) {
        if (m_pressure >= (m_maxPeakPressureCommand - 30u)) {
            // m_blower.ouvrirIntermediaire();
            m_vigilance = true;
        }

        if (m_pressure >= m_maxPeakPressureCommand) {
            setSubPhase(CycleSubPhases::HOLD_INSPIRATION);
            Buzzer_Medium_Start();
            plateau();
        }
    }

    if (m_pressure >= (m_maxPeakPressureCommand + 10u)) {
        // m_patient.augmenterOuverture();
        Buzzer_Medium_Start();
    }
}

void PressureController::safeguardPressionPlateau(uint16_t p_centiSec) {
    if (m_subPhase == CycleSubPhases::HOLD_INSPIRATION) {
        if (m_pressure >= m_maxPlateauPressureCommand) {
            // TODO vérifier avec médical si on doit délester la pression
            // TODO alarme franchissement plateau haut
        } else {
            plateau();
        }
    }
}

void PressureController::safeguardHoldExpiration(uint16_t p_centiSec) {
    if (m_phase == CyclePhases::EXHALATION) {
        // TODO asservir m_minPeepCommand + X à la vitesse du volume estimé
        if (m_pressure <= (m_minPeepCommand + 20u)) {
            setSubPhase(CycleSubPhases::HOLD_EXHALE);
            holdExhalation();
        }
    }
}

void PressureController::safeguardMaintienPeep(uint16_t p_centiSec) {
    if (m_pressure <= m_minPeepCommand) {
        // m_blower.augmenterOuverture();
        Buzzer_Long_Start();
    }
}

void PressureController::computeCentiSecParameters() {
    m_centiSecPerCycle = 60u * 100u / m_cyclesPerMinute;
    // Inhalation = 1/3 of the cycle duration,
    // Exhalation = 2/3 of the cycle duration
    m_centiSecPerInhalation = m_centiSecPerCycle / 3u;
}

void PressureController::executeCommands() {
    m_blower.execute();
    m_patient.execute();
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
                            + ((PID_BLOWER_KD * derivative) / 1000);  // calcul de la commande

    int32_t minAperture = m_blower.minAperture();
    int32_t maxAperture = m_blower.maxAperture();

    uint32_t blowerAperture =
        max(minAperture,
            min(maxAperture,
                maxAperture - (blowerCommand + 1000) * (maxAperture - minAperture) / 2000));

    return blowerAperture;
}

int32_t
PressureController::pidPatient(int32_t targetPressure, int32_t currentPressure, int32_t dt) {
    // Compute error
    int32_t error = targetPressure - currentPressure;

    // Compute integral
    patientIntegral = patientIntegral + ((PID_PATIENT_KI * error * dt) / 1000000);
    patientIntegral = max(PID_PATIENT_INTEGRAL_MIN, min(PID_PATIENT_INTEGRAL_MAX, patientIntegral));

    // Compute derivative
    int32_t derivative = ((patientLastError == INVALID_ERROR_MARKER) || (dt == 0))
                             ? 0
                             : ((1000000 * (error - patientLastError)) / dt);
    patientLastError = error;

    int32_t patientCommand = (PID_PATIENT_KP * error) + patientIntegral
                             + ((PID_PATIENT_KD * derivative) / 1000);  // calcul de la commande

    int32_t minAperture = m_blower.minAperture();
    int32_t maxAperture = m_blower.maxAperture();

    uint32_t patientAperture =
        max(minAperture,
            min(maxAperture, minAperture
                                 + (patientCommand + targetPressure) * (maxAperture - minAperture)
                                       / (2 * targetPressure)));

    return patientAperture;
}
