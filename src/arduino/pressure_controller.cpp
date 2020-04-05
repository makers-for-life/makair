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

// Internal libraries
#include "air_transistor.h"
#include "alarm.h"
#include "config.h"
#include "debug.h"
#include "parameters.h"

int32_t MAX = 145;
int32_t MIN = 25;

int32_t blower_erreur = 0;
int32_t blower_sommeErreur = 0;
int32_t blower_derivee = 0;
int32_t blower_cmd = 0;
int32_t blower_kp = 1 * 2 * 2 * 2 / 2;
int32_t blower_ki = 1 * 2 * 2 * 2;
int32_t blower_kd = 5 * 2;
int32_t blower_BORNE_MAX = 1000;
int32_t blower_BORNE_MIN = -1 * blower_BORNE_MAX;

int32_t calculConsigneVanneBlower(int32_t consigne, int32_t reel, int32_t dt) {
    blower_erreur = blower_kp * (consigne - reel);  // erreur entre la consigne et la realite
    blower_sommeErreur += blower_ki * blower_erreur * dt / 1000000.0;
    blower_sommeErreur = max(blower_BORNE_MIN, min(blower_BORNE_MAX, blower_sommeErreur));
    blower_derivee = 1000000.0 * blower_erreur / dt;

    blower_cmd = blower_erreur + blower_sommeErreur
                 + blower_kd * blower_derivee / 1000;  // calcul de la commande

    uint32_t consigneBlower = max(MIN, min(MAX, MAX - (blower_cmd + 1000) * (MAX - MIN) / 2000));
    return consigneBlower;
}

int32_t patient_erreur = 0;
int32_t patient_sommeErreur = 0;
int32_t patient_derivee = 0;
int32_t patient_cmd = 0;
int32_t patient_kp = 1 * 2 * 2 * 2 / 2;
int32_t patient_ki = 1 * 2 * 2 * 2;
int32_t patient_kd = 5 * 2;
int32_t patient_BORNE_MAX = 1000;
int32_t patient_BORNE_MIN = -1 * patient_BORNE_MAX;

int32_t calculConsigneVannePatient(int32_t consigne, int32_t reel, int32_t dt) {
    patient_erreur = patient_kp * (consigne - reel);  // erreur entre la consigne et la realite
    patient_sommeErreur += patient_ki * patient_erreur * dt / 1000000.0;
    patient_sommeErreur = max(patient_BORNE_MIN, min(patient_BORNE_MAX, patient_sommeErreur));
    patient_derivee = 1000000.0 * patient_erreur / dt;

    patient_cmd = patient_erreur + patient_sommeErreur
                  + patient_kd * patient_derivee / 1000;  // calcul de la commande

    uint32_t consignepatient = max(MIN, min(MAX, MIN + (patient_cmd + 1000) * (MAX - MIN) / 2000));
    return consignepatient;
}

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
                                       int16_t p_maxPeakPressure,
                                       AirTransistor p_blower,
                                       AirTransistor p_patient)
    : m_cyclesPerMinuteCommand(p_cyclesPerMinute),
      m_minPeepCommand(p_minPeepCommand),
      m_maxPlateauPressureCommand(p_maxPlateauPressure),
      m_maxPeakPressureCommand(DEFAULT_MAX_PEAK_PRESSURE_COMMAND),
      m_apertureCommand(p_aperture),
      m_cyclesPerMinute(p_cyclesPerMinute),
      m_aperture(p_aperture),
      m_maxPeakPressure(p_maxPeakPressure),
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

    m_blower.fermer();
    m_blower.position = -1;
    m_patient.fermer();
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
            exhale();
            // holdExhalation();
            break;
        }
        default: {
            holdExhalation();
        }
        }
    }
    safeguards(p_centiSec);

    // DBG_PHASE_PRESSION(m_cycleNb, p_centiSec, 1, m_phase, m_subPhase, m_pressure,
    // m_blower.command,  m_blower.position, m_patient.command, m_patient.position)

    executeCommands();
    DBG_PID_PLOTTER(m_consignePression, m_pressure, m_blower.position, m_patient.position);
    m_previousPhase = m_phase;
}

void PressureController::onCycleMinus() {
    DBG_DO(Serial.println("nb cycle --");)
    m_cyclesPerMinuteCommand--;
    if (m_cyclesPerMinuteCommand < BORNE_INF_CYCLE) {
        m_cyclesPerMinuteCommand = BORNE_INF_CYCLE;
    }
}

void PressureController::onCyclePlus() {
#ifndef SIMULATION
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
            && m_pressure < DEFAULT_MAX_PEAK_PRESSURE_COMMAND) {
            if (m_subPhase != CycleSubPhases::HOLD_INSPI) {
                m_consignePression = DEFAULT_MAX_PEAK_PRESSURE_COMMAND;
                setSubPhase(CycleSubPhases::INSPI);
            }
        } else {
            m_consignePression = DEFAULT_MAX_PLATEAU_COMMAND;
            setSubPhase(CycleSubPhases::HOLD_INSPI);
        }
    } else {
        m_phase = CyclePhases::EXHALATION;
        m_consignePression = DEFAULT_MIN_PEEP_COMMAND;
        if (m_subPhase != CycleSubPhases::HOLD_EXHALE) {
            setSubPhase(CycleSubPhases::EXHALE);
        }
    }
}

void PressureController::inhale() {
    // Open the air stream towards the patient's lungs
    m_blower.ouvrirImmediatement(calculConsigneVanneBlower(m_consignePression, m_pressure, m_dt));

    // Open the air stream towards the patient's lungs
    m_patient.fermer();

    // TODO vérifier si la pression de crête est toujours la dernière pression mesurée sur la
    // sous-phase INSPI

    // Update the peak pressure
    m_peakPressure = m_pressure;
}

void PressureController::plateau() {
    // Deviate the air stream outside
    m_blower.fermer();

    // Close the air stream towards the patient's lungs
    m_patient.ouvrirImmediatement(calculConsigneVannePatient(m_consignePression, m_pressure, m_dt));

    // Update the plateau pressure
    m_plateauPressure = m_pressure;
}

void PressureController::exhale() {
    // Deviate the air stream outside
    m_blower.fermer();

    // Open the valve so the patient can exhale outside
    m_patient.ouvrirImmediatement(calculConsigneVannePatient(m_consignePression, m_pressure, m_dt));

    // Update the PEEP
    m_peep = m_pressure;
}

void PressureController::holdExhalation() {
    // Deviate the air stream outside
    m_blower.fermer();

    // Close the valve so the patient can exhale outside
    m_patient.fermer();
}

void PressureController::updateDt(int32_t p_dt) { m_dt = p_dt; }

void PressureController::safeguards(uint16_t p_centiSec) {
    // safeguardPressionCrete(p_centiSec);
    // safeguardPressionPlateau(p_centiSec);
    // safeguardHoldExpiration(p_centiSec);
    // safeguardMaintienPeep(p_centiSec);
}

void PressureController::safeguardPressionCrete(uint16_t p_centiSec) {
    if (m_subPhase == CycleSubPhases::INSPI) {
        if (m_pressure >= (m_maxPeakPressureCommand - 30)) {
            m_blower.ouvrirIntermediaire();
            m_vigilance = true;
        }

        if (m_pressure >= m_maxPeakPressureCommand) {
            setSubPhase(CycleSubPhases::HOLD_INSPI);
            Alarm_Yellow_Start();
            plateau();
        }
    }

    if (m_pressure >= (m_maxPeakPressureCommand + 10)) {
        m_patient.augmenterOuverture();
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
        m_blower.augmenterOuverture();
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
