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

// Internal libraries
#include "../includes/pressure_valve.h"
#include "../includes/alarm.h"
#include "../includes/config.h"
#include "../includes/debug.h"
#include "../includes/parameters.h"

// INITIALISATION =============================================================

PressureController pController;

// FUNCTIONS ==================================================================

PressureController::PressureController()
  : m_cyclesPerMinuteCommand(INITIAL_CYCLE_NB),
    m_minPeepCommand(DEFAULT_MIN_PEEP_COMMAND),                   // [mmH20]
    m_maxPlateauPressureCommand(DEFAULT_MAX_PLATEAU_COMMAND),     // [mmH20]
    m_maxPeakPressureCommand(DEFAULT_MAX_PEAK_PRESSURE_COMMAND),  // [mmH20]
    m_cyclesPerMinute(INITIAL_CYCLE_NB),
    m_maxPeakPressure(BORNE_SUP_PRESSION_CRETE),       // [mmH20]
    m_maxPlateauPressure(BORNE_SUP_PRESSION_PLATEAU),  // [mmH20]
    m_minPeep(BORNE_INF_PRESSION_PEP),                 // TODO revoir la valeur [mmH20]
    m_pressure(INITIAL_ZERO_PRESSURE),
    m_peakPressure(INITIAL_ZERO_PRESSURE),
    m_plateauPressure(INITIAL_ZERO_PRESSURE),
    m_peep(INITIAL_ZERO_PRESSURE),
    m_phase(CyclePhases::INHALATION),
    m_triggersHoldExpiDetectionTick(0),
    m_triggersHoldExpiDetectionTickSupprime(0) {
  computeCentiSecParameters();
}

PressureController::PressureController(int16_t p_cyclesPerMinute,
                                       int16_t p_minPeepCommand,
                                       int16_t p_maxPlateauPressure,
                                       int16_t p_maxPeakPressure,
                                       PressureValve p_blower,
                                       PressureValve p_patient)
  : m_cyclesPerMinuteCommand(p_cyclesPerMinute),
    m_minPeepCommand(p_minPeepCommand),
  m_maxPlateauPressureCommand(p_maxPlateauPressure),
    m_maxPeakPressureCommand(DEFAULT_MAX_PEAK_PRESSURE_COMMAND),
    m_cyclesPerMinute(p_cyclesPerMinute),
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
    m_triggersHoldExpiDetectionTick(0),
    m_triggersHoldExpiDetectionTickSupprime(0) {
  computeCentiSecParameters();
}

void PressureController::setup() {
  DBG_DO(Serial.println(VERSION);)
  DBG_DO(Serial.println("Setup pressure controler");)

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

  computeCentiSecParameters();

  /*
  DBG_AFFICHE_CSPCYCLE_CSPINSPI(m_centiSecPerCycle, m_centiSecPerInhalation)
  */

  m_cyclesPerMinute = m_cyclesPerMinuteCommand;
  m_minPeep = m_minPeepCommand;
  m_maxPlateauPressure = m_maxPlateauPressureCommand;
  /*
  DBG_AFFICHE_CONSIGNES(m_cyclesPerMinute, m_aperture, m_minPeep, m_maxPlateauPressure)
  */

  // remise à zéro tick alarmes
  m_triggersHoldExpiDetectionTick = 0;
  m_triggersHoldExpiDetectionTickSupprime = 0;
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

  m_previousPhase = m_phase;
}

void PressureController::onCycleIncrease() {
  #ifndef SIMULATION
  // During simulation without electronic board there is a noise on the button pin. It increases
  // the cycle and the simulation fail.
  DBG_DO(Serial.println("Cycle ++");)
  
  m_cyclesPerMinuteCommand++;
  
  if (m_cyclesPerMinuteCommand > BORNE_SUP_CYCLE) {
      m_cyclesPerMinuteCommand = BORNE_SUP_CYCLE;
  }
  
  #endif
}

void PressureController::onCycleDecrease() {
  DBG_DO(Serial.println("Cycle --");)
  m_cyclesPerMinuteCommand--;

  if (m_cyclesPerMinuteCommand < BORNE_INF_CYCLE) {
    m_cyclesPerMinuteCommand = BORNE_INF_CYCLE;
  }
}

void PressureController::onPeepPressureIncrease() {
  DBG_DO(Serial.println("Peep Pressure ++");)
  
  m_minPeepCommand = m_minPeepCommand + 10;

  if (m_minPeepCommand > BORNE_SUP_PRESSION_PEP) {
    m_minPeepCommand = BORNE_SUP_PRESSION_PEP;
  }
}

void PressureController::onPeepPressureDecrease() {
  DBG_DO(Serial.println("Peep Pressure --");)
  
  m_minPeepCommand = m_minPeepCommand - 10;

  if (m_minPeepCommand < BORNE_INF_PRESSION_PEP) {
    m_minPeepCommand = BORNE_INF_PRESSION_PEP;
  }
}

void PressureController::onPlateauPressureIncrease() {
  DBG_DO(Serial.println("Plateau Pressure ++");)
  
  m_maxPlateauPressureCommand = m_maxPlateauPressureCommand + 10;
  
  if (m_maxPlateauPressureCommand > BORNE_SUP_PRESSION_PLATEAU) {
    m_maxPlateauPressureCommand = BORNE_SUP_PRESSION_PLATEAU;
  }
}

void PressureController::onPlateauPressureDecrease() {
  DBG_DO(Serial.println("Plateau Pressure --");)
  
  m_maxPlateauPressureCommand = m_maxPlateauPressureCommand - 10;
  
  if (m_maxPlateauPressureCommand < BORNE_INF_PRESSION_PLATEAU) {
    m_maxPlateauPressureCommand = BORNE_INF_PRESSION_PLATEAU;
  }
}

void PressureController::onPeekPressureIncrease() {
  DBG_DO(Serial.println("Peek Pressure ++");)

  m_maxPeakPressureCommand = m_maxPeakPressureCommand + 10;

  if (m_maxPeakPressureCommand > BORNE_SUP_PRESSION_CRETE) {
      m_maxPeakPressureCommand = BORNE_SUP_PRESSION_CRETE;
  }
}

void PressureController::onPeekPressureDecrease() {
  DBG_DO(Serial.println("Peek Pressure --");)
  
  m_maxPeakPressureCommand = m_maxPeakPressureCommand - 10;

  if (m_maxPeakPressureCommand < BORNE_INF_PRESSION_CRETE) {
      m_maxPeakPressureCommand = BORNE_INF_PRESSION_CRETE;
  }
}

void PressureController::updatePhase(uint16_t p_centiSec) {
  if (p_centiSec < m_centiSecPerInhalation) {
    m_phase = CyclePhases::INHALATION;
    
    if (p_centiSec < (m_centiSecPerInhalation * 80 / 100)) {
      if (m_subPhase != CycleSubPhases::HOLD_INSPI) {
        setSubPhase(CycleSubPhases::INSPI);
      }
    } else {
      setSubPhase(CycleSubPhases::HOLD_INSPI);
    }
  } else {
    m_phase = CyclePhases::EXHALATION;
    
    if (m_subPhase != CycleSubPhases::HOLD_EXHALE) {
      setSubPhase(CycleSubPhases::EXHALE);
    }
  }
}

void PressureController::inhale() {
  // Open the air stream towards the patient's lungs
  m_blower.open();

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
  m_patient.open();

  // Update the PEEP
  m_peep = m_pressure;
}

void PressureController::holdExhalation() {
  // Deviate the air stream outside
  m_blower.close();

  // Close the valve so the patient can exhale outside
  m_patient.close();
}

void PressureController::safeguards(uint16_t p_centiSec) {
  safeguardPressionCrete(p_centiSec);
  safeguardPressionPlateau(p_centiSec);
  safeguardHoldExpiration(p_centiSec);
  safeguardMaintienPeep(p_centiSec);
}

void PressureController::safeguardPressionCrete(uint16_t p_centiSec) {
  if (m_subPhase == CycleSubPhases::INSPI) {
    if (m_pressure >= (m_maxPeakPressureCommand - 30)) {
      if (m_triggersMaxPeakPressureDetectionTick == 0) {
        m_triggersMaxPeakPressureDetectionTick = p_centiSec;
      }

      if ((p_centiSec - m_triggersMaxPeakPressureDetectionTick) >= 5) {
        m_blower.openMiddle();
        m_vigilance = true;
      }
    } else if (m_triggersMaxPeakPressureDetectionTick != 0) {
      if (m_triggersMaxPeakPressureDetectionTickSupprime == 0) {
        m_triggersMaxPeakPressureDetectionTickSupprime = p_centiSec;
      }

      if ((p_centiSec - m_triggersMaxPeakPressureDetectionTickSupprime) >= 3) {
        m_triggersMaxPeakPressureDetectionTick = 0;
        m_triggersMaxPeakPressureDetectionTickSupprime = 0;
      }
    }

    if (m_pressure >= m_maxPeakPressureCommand) {
      setSubPhase(CycleSubPhases::HOLD_INSPI);
      Alarm_Yellow_Start();
      plateau();
    }
  }

  if (m_pressure >= (m_maxPeakPressureCommand + 10)) {
    m_patient.increase();
    Alarm_Yellow_Start();
  }
}

void PressureController::safeguardPressionPlateau(uint16_t p_centiSec) {
  if (m_subPhase == CycleSubPhases::HOLD_INSPI) {
    if (m_pressure >= m_maxPlateauPressureCommand) {
      if (m_triggersMaxPlateauPressureDetectionTick == 0) {
        m_triggersMaxPlateauPressureDetectionTick = p_centiSec;
      }

      if ((p_centiSec - m_triggersMaxPlateauPressureDetectionTick) >= 10) {
        // TODO vérifier avec médical si on doit délester la pression
        // TODO alarme franchissement plateau haut
      }
    } else if (m_triggersMaxPlateauPressureDetectionTick != 0) {
      if (m_triggersMaxPlateauPressureDetectionTickSupprime == 0) {
        m_triggersMaxPlateauPressureDetectionTickSupprime = p_centiSec;
      }

      if ((p_centiSec - m_triggersMaxPlateauPressureDetectionTickSupprime) >= 3) {
        m_triggersMaxPlateauPressureDetectionTick = 0;
        m_triggersMaxPlateauPressureDetectionTickSupprime = 0;
        plateau();
      }
    }
  }
}

void PressureController::safeguardHoldExpiration(uint16_t p_centiSec) {
  if (m_phase == CyclePhases::EXHALATION) {
    // TODO asservir m_minPeepCommand + X à la vitesse du volume estimé
    if (m_pressure <= (m_minPeepCommand + 20)) {
      if (m_triggersHoldExpiDetectionTick == 0) {
        m_triggersHoldExpiDetectionTick = p_centiSec;
      }

      if ((p_centiSec - m_triggersHoldExpiDetectionTick) >= 10) {
        setSubPhase(CycleSubPhases::HOLD_EXHALE);
        holdExhalation();
      }
    } else if (m_triggersHoldExpiDetectionTick != 0) {
      if (m_triggersHoldExpiDetectionTickSupprime == 0) {
        m_triggersHoldExpiDetectionTickSupprime = p_centiSec;
      }

      if ((p_centiSec - m_triggersHoldExpiDetectionTickSupprime) >= 3) {
        m_triggersHoldExpiDetectionTick = 0;
      m_triggersHoldExpiDetectionTickSupprime = 0;
      }
    }
  }
}

void PressureController::safeguardMaintienPeep(uint16_t p_centiSec) {
  if (m_pressure <= m_minPeepCommand) {
    m_blower.increase();
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
