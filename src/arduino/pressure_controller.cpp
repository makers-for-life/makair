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
#include "pressure_controller.h"

// External libraries
#include <algorithm>

// Internal libraries
#include "air_transistor.h"
#include "alarm.h"
#include "config.h"
#include "debug.h"
#include "parameters.h"

// INITIALISATION =============================================================

PressureController pController;

// FUNCTIONS ==================================================================

PressureController::PressureController()
    : m_cyclesPerMinuteCommand(INITIAL_CYCLE_NB),
      m_minPeepCommand(DEFAULT_MIN_PEEP_COMMAND),                   // [mmH20]
      m_maxPlateauPressureCommand(DEFAULT_MAX_PLATEAU_COMMAND),     // [mmH20]
      m_maxPeakPressureCommand(DEFAULT_MAX_PEAK_PRESSURE_COMMAND),  // [mmH20]
      m_apertureCommand(ANGLE_OUVERTURE_MAXI),
      m_cyclesPerMinute(INITIAL_CYCLE_NB),
      m_aperture(ANGLE_OUVERTURE_MAXI),
      m_maxPeakPressure(BORNE_SUP_PRESSION_CRETE),       // [mmH20]
      m_maxPlateauPressure(BORNE_SUP_PRESSION_PLATEAU),  // [mmH20]
      m_minPeep(BORNE_INF_PRESSION_PEP),                 // TODO revoir la valeur [mmH20]
      m_pressure(INITIAL_ZERO_PRESSURE),
      m_peakPressure(INITIAL_ZERO_PRESSURE),
      m_plateauPressure(INITIAL_ZERO_PRESSURE),
      m_peep(INITIAL_ZERO_PRESSURE),
      m_phase(CyclePhases::INHALATION),
      m_franchissementSeuilHoldExpiDetectionTick(0),
      m_franchissementSeuilHoldExpiDetectionTickSupprime(0) {
    computeCentiSecParameters();
}

PressureController::PressureController(int16_t p_cyclesPerMinute,
                                       int16_t p_minPeepCommand,
                                       int16_t p_maxPlateauPressure,
                                       int16_t p_aperture,
                                       int16_t p_maxPeakPressureCommand,
                                       AirTransistor p_blower,
                                       AirTransistor p_patient)
    : m_cyclesPerMinuteCommand(p_cyclesPerMinute),
      m_minPeepCommand(p_minPeepCommand),
      m_maxPlateauPressureCommand(p_maxPlateauPressure),
      m_maxPeakPressureCommand(p_maxPeakPressureCommand),
      m_apertureCommand(p_aperture),
      m_cyclesPerMinute(p_cyclesPerMinute),
      m_aperture(p_aperture),
      m_maxPeakPressure(p_maxPeakPressureCommand),
      m_maxPlateauPressure(p_maxPlateauPressure),
      m_minPeep(p_minPeepCommand),  // TODO revoir la valeur de démarage
      m_pressure(INITIAL_ZERO_PRESSURE),
      m_peakPressure(INITIAL_ZERO_PRESSURE),
      m_plateauPressure(INITIAL_ZERO_PRESSURE),
      m_peep(INITIAL_ZERO_PRESSURE),
      m_phase(CyclePhases::INHALATION),
      m_blower(p_blower),
      m_patient(p_patient),
      m_franchissementSeuilHoldExpiDetectionTick(0),
      m_franchissementSeuilHoldExpiDetectionTickSupprime(0) {
    computeCentiSecParameters();
}

void PressureController::setup() {
    DBG_DO(Serial.println(VERSION);)
    DBG_DO(Serial.println("mise en secu initiale");)

    m_blower.close();
    m_blower.position = -1;
    m_patient.close();
    m_patient.position = -1;
    m_blower.execute();
    m_patient.execute();

    m_peakPressure = 0;
    m_plateauPressure = 0;
    m_peep = 0;

    m_cycleNb = 0;
}

void PressureController::initRespiratoryCycle() {
    m_phase = CyclePhases::INHALATION;
    setSubPhase(CycleSubPhases::INSPI);
    m_cycleNb++;

    // Reset PID integrals
    blowerIntegral = 0;
    blowerLastError = 0;
    patientIntegral = 0;
    patientLastError = 0;

    computeCentiSecParameters();

    /*
    DBG_AFFICHE_CSPCYCLE_CSPINSPI(m_centiSecPerCycle, m_centiSecPerInhalation)
    */

    m_cyclesPerMinute = m_cyclesPerMinuteCommand;
    m_aperture = m_apertureCommand;
    m_minPeep = m_minPeepCommand;
    m_maxPlateauPressure = m_maxPlateauPressureCommand;
    /*
    DBG_AFFICHE_CONSIGNES(m_cyclesPerMinute, m_aperture, m_minPeep, m_maxPlateauPressure)
    */

    // remise à zéro tick alarmes
    m_franchissementSeuilHoldExpiDetectionTick = 0;
    m_franchissementSeuilHoldExpiDetectionTickSupprime = 0;
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
        case CycleSubPhases::INSPI: {
            inhale();
            break;
        }
        case CycleSubPhases::HOLD_INSPI: {
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

    DBG_PHASE_PRESSION(m_cycleNb, p_centiSec, 1, m_phase, m_subPhase, m_pressure, m_blower.command,
                       m_blower.position, m_patient.command, m_patient.position)

    executeCommands();
}

void PressureController::onCycleMinus() {
    DBG_DO(Serial.println("nb cycle --");)
    m_cyclesPerMinuteCommand--;
    if (m_cyclesPerMinuteCommand < BORNE_INF_CYCLE) {
        m_cyclesPerMinuteCommand = BORNE_INF_CYCLE;
    }
}

void PressureController::onCyclePlus() {
#if SIMULATION != 1
    // During simulation without electronic board there is a noise on the button pin. It increases
    // the cycle and the simulation fail.
    DBG_DO(Serial.println("nb cycle ++");)
    m_cyclesPerMinuteCommand++;
    if (m_cyclesPerMinuteCommand > BORNE_SUP_CYCLE) {
        m_cyclesPerMinuteCommand = BORNE_SUP_CYCLE;
    }
#endif
}

void PressureController::onPressionPepMinus() {
    DBG_DO(Serial.println("pression PEP --");)
    m_minPeepCommand = m_minPeepCommand - 10;
    if (m_minPeepCommand < BORNE_INF_PRESSION_PEP) {
        m_minPeepCommand = BORNE_INF_PRESSION_PEP;
    }
}

void PressureController::onPressionPepPlus() {
    DBG_DO(Serial.println("pression PEP ++");)
    m_minPeepCommand = m_minPeepCommand + 10;
    if (m_minPeepCommand > BORNE_SUP_PRESSION_PEP) {
        m_minPeepCommand = BORNE_SUP_PRESSION_PEP;
    }
}

void PressureController::onPressionPlateauMinus() {
    DBG_DO(Serial.println("pression plateau --");)
    m_maxPlateauPressureCommand = m_maxPlateauPressureCommand - 10;
    if (m_maxPlateauPressureCommand < BORNE_INF_PRESSION_PLATEAU) {
        m_maxPlateauPressureCommand = BORNE_INF_PRESSION_PLATEAU;
    }
}

void PressureController::onPressionPlateauPlus() {
    DBG_DO(Serial.println("pression plateau ++");)
    m_maxPlateauPressureCommand = m_maxPlateauPressureCommand + 10;
    if (m_maxPlateauPressureCommand > BORNE_SUP_PRESSION_PLATEAU) {
        m_maxPlateauPressureCommand = BORNE_SUP_PRESSION_PLATEAU;
    }
}

void PressureController::onPressionCreteMinus() {
    DBG_DO(Serial.println("pression crete --");)
    m_maxPeakPressureCommand = m_maxPeakPressureCommand - 10;
    if (m_maxPeakPressureCommand < BORNE_INF_PRESSION_CRETE) {
        m_maxPeakPressureCommand = BORNE_INF_PRESSION_CRETE;
    }
}

void PressureController::onPressionCretePlus() {
    DBG_DO(Serial.println("pression crete ++");)
    m_maxPeakPressureCommand = m_maxPeakPressureCommand + 10;
    if (m_maxPeakPressureCommand > BORNE_SUP_PRESSION_CRETE) {
        m_maxPeakPressureCommand = BORNE_SUP_PRESSION_CRETE;
    }
}

void PressureController::updatePhase(uint16_t p_centiSec) {
    if (p_centiSec < m_centiSecPerInhalation) {
        m_phase = CyclePhases::INHALATION;
        if (p_centiSec < (m_centiSecPerInhalation * 80 / 100)
            && m_pressure < m_maxPeakPressureCommand) {
            if (m_subPhase != CycleSubPhases::HOLD_INSPI) {
                m_consignePression = m_maxPeakPressureCommand;
                setSubPhase(CycleSubPhases::INSPI);
            }
        } else {
            m_consignePression = m_maxPlateauPressureCommand;
            setSubPhase(CycleSubPhases::HOLD_INSPI);
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
    // TODO revoir safeguards
    // safeguardPressionCrete(p_centiSec);
    // safeguardPressionPlateau(p_centiSec);
    // safeguardHoldExpiration(p_centiSec);
    // safeguardMaintienPeep(p_centiSec);
}

void PressureController::safeguardPressionCrete(uint16_t p_centiSec) {
    if (m_subPhase == CycleSubPhases::INSPI) {
        if (m_pressure >= (m_maxPeakPressureCommand - 30)) {
            // m_blower.ouvrirIntermediaire();
            m_vigilance = true;
        }

        if (m_pressure >= m_maxPeakPressureCommand) {
            setSubPhase(CycleSubPhases::HOLD_INSPI);
            Alarm_Yellow_Start();
            plateau();
        }
    }

    if (m_pressure >= (m_maxPeakPressureCommand + 10)) {
        // m_patient.augmenterOuverture();
        Alarm_Yellow_Start();
    }
}

void PressureController::safeguardPressionPlateau(uint16_t p_centiSec) {
    if (m_subPhase == CycleSubPhases::HOLD_INSPI) {
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
        if (m_pressure <= (m_minPeepCommand + 20)) {
            setSubPhase(CycleSubPhases::HOLD_EXHALE);
            holdExhalation();
        }
    }
}

void PressureController::safeguardMaintienPeep(uint16_t p_centiSec) {
    if (m_pressure <= m_minPeepCommand) {
        // m_blower.augmenterOuverture();
        Alarm_Red_Start();
    }
}

void PressureController::computeCentiSecParameters() {
    m_centiSecPerCycle = 60 * 100 / m_cyclesPerMinute;
    // Inhalation = 1/3 of the cycle duration,
    // Exhalation = 2/3 of the cycle duration
    m_centiSecPerInhalation = m_centiSecPerCycle / 3;
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
    blowerIntegral = blowerIntegral + PID_BLOWER_KI * error * dt / 1000000;
    blowerIntegral = max(PID_BLOWER_INTEGRAL_MIN, min(PID_BLOWER_INTEGRAL_MAX, blowerIntegral));

    // Compute derivative
    int32_t derivative = 1000000 * (error - blowerLastError) / dt;
    blowerLastError = error;

    int32_t blowerCommand = PID_BLOWER_KP * error + blowerIntegral
                            + PID_BLOWER_KD * derivative / 1000;  // calcul de la commande

    uint32_t consigneBlower =
        max(m_blower.minAperture(),
            min(m_blower.maxAperture(),
                m_blower.maxAperture()
                    - (blowerCommand + 1000) * (m_blower.maxAperture() - m_blower.minAperture())
                          / 2000));
    return consigneBlower;
}

int32_t
PressureController::pidPatient(int32_t targetPressure, int32_t currentPressure, int32_t dt) {
    // Compute error
    int32_t error = targetPressure - currentPressure;

    // Compute integral
    patientIntegral = patientIntegral + PID_PATIENT_KI * error * dt / 1000000.0;
    patientIntegral = max(PID_PATIENT_INTEGRAL_MIN, min(PID_PATIENT_INTEGRAL_MAX, patientIntegral));

    // Compute derivative
    int32_t derivative = 1000000.0 * (error - patientLastError) / dt;
    patientLastError = error;

    int32_t patientCommand = PID_PATIENT_KP * error + patientIntegral
                             + PID_PATIENT_KD * derivative / 1000;  // calcul de la commande

    uint32_t consignePatient =
        max(m_patient.minAperture(),
            min(m_patient.maxAperture(),
                m_patient.minAperture()
                    + (patientCommand + 1000) * (m_patient.maxAperture() - m_patient.minAperture())
                          / 2000));
    return consignePatient;
}
