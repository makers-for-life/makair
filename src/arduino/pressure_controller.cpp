/*=============================================================================
 * @file pressure_control.h
 *
 * COVID Respirator
 *
 * @section copyright Copyright
 *
 * Makers For Life
 *
 * @section descr File description
 *
 * This file implements the pressure control algorithm
 */

// INCLUDES ===================================================================

// Associated header
#include "pressure_controller.h"

// Internal libraries
#include "config.h"
#include "debug.h"

// INITIALISATION =============================================================

PressureController pController(AirTransistor(), AirTransistor());

// FUNCTION ===================================================================

PressureController::PressureController(const AirTransistor &p_blower,
                                       const AirTransistor &p_patient)
    : m_cyclesPerMinuteCommand(20), m_minPeepCommand(5),
      m_maxPlateauPressureCommand(30), m_apertureCommand(ANGLE_OUVERTURE_MAXI),
      m_cyclesPerMinute(20), m_aperture(ANGLE_OUVERTURE_MAXI),
      m_maxPeakPressure(60), m_maxPlateauPressure(30), m_minPeep(5),
      m_currentPressure(-1), m_peakPressure(-1), m_plateauPressure(-1),
      m_peep(-1), m_phase(CyclePhases::INHALATION), m_blower(p_blower),
      m_patient(p_patient)

{
  computeCentiSecParameters();
}

void PressureController::setup()
{
  m_blower.actuator.attach(PIN_SERVO_BLOWER);
  m_patient.actuator.attach(PIN_SERVO_PATIENT);

  DBG_DO(Serial.print("mise en secu initiale");)

  m_blower.actuator.write(m_blower.failsafe);
  m_patient.actuator.write(m_patient.failsafe);
}

void PressureController::initLoop() {
  m_peakPressure = -1;
  m_plateauPressure = -1;
  m_peep = -1;
  m_phase = CyclePhases::INHALATION;
  m_blower.reset();
  m_patient.reset();
  computeCentiSecParameters();

  DBG_AFFICHE_CSPCYCLE_CSPINSPI(m_centiSecPerCycle, m_centiSecPerInhalation)

  m_cyclesPerMinute = m_cyclesPerMinuteCommand;
  m_aperture = m_apertureCommand;
  m_minPeep = m_minPeepCommand;
  m_maxPlateauPressure = m_maxPlateauPressureCommand;

  DBG_AFFICHE_CONSIGNES(m_cyclesPerMinute, m_aperture, m_minPeep,
                        m_maxPlateauPressure)
}

void PressureController::updateCurrentPressure(int16_t p_currentPressure)
{
    m_currentPressure = p_currentPressure;
}

void PressureController::compute(uint16_t p_currentCentieme) {
  // Update the cycle phase
  updatePhase(p_currentCentieme);

  // Act accordingly
  switch (m_phase) {
    case CyclePhases::INHALATION: {
      inhale();
      break;
    }
    case CyclePhases::PLATEAU: {
      plateau();
      break;
    }
    case CyclePhases::EXHALATION: {
      exhale();
      break;
    }
    case CyclePhases::HOLD_EXHALATION:
    {
        break;
    }
    default:
    {
        inhale();
    }
  }

  safeguards(p_currentCentieme);

  DBG_PHASE_PRESSION(p_currentCentieme, 50, m_phase, m_currentPressure)
}

void PressureController::updatePhase(uint16_t p_currentCentieme) {
  if (p_currentCentieme <= m_centiSecPerInhalation) {
    m_phase = m_currentPressure >= m_peakPressure ? CyclePhases::INHALATION
                                                  : CyclePhases::PLATEAU;
  }
  else
  {
    m_phase = CyclePhases::EXHALATION;
  }
}

void PressureController::inhale()
{
  // Open the air stream towards the patient's lungs
  m_blower.command = ANGLE_FERMETURE - ANGLE_MULTIPLICATEUR * m_aperture;

  // Open the air stream towards the patient's lungs
  m_patient.command =
      ANGLE_FERMETURE + ANGLE_MULTIPLICATEUR * ANGLE_OUVERTURE_MAXI;

  // Update the peak pressure
  m_peakPressure = m_currentPressure;
}

void PressureController::plateau()
{
  // Deviate the air stream outside
  m_blower.command =
      ANGLE_FERMETURE + ANGLE_MULTIPLICATEUR * ANGLE_OUVERTURE_MAXI;

  // Close the air stream towards the patient's lungs
  m_patient.command = ANGLE_FERMETURE;

  // Update the plateau pressure
  m_plateauPressure = m_currentPressure;
}

void PressureController::exhale()
{
  // Deviate the air stream outside
  m_blower.command =
      ANGLE_FERMETURE + ANGLE_MULTIPLICATEUR * ANGLE_OUVERTURE_MAXI;

  // Open the valve so the patient can exhale outside
  m_patient.command = m_patient.failsafe;

  // Update the PEEP
  m_peep = m_currentPressure;
}

void PressureController::safeguards(uint16_t p_currentCentieme) {
  // si pression crête > max, alors fermeture blower de 2°
  if (m_currentPressure > m_maxPeakPressure) {
    DBG_PRESSION_CRETE(p_currentCentieme, 80)
    m_blower.command = m_blower.position - 2;
  }
  // si pression plateau > consigne param, alors ouverture expiration de 1°
  if (m_phase == CyclePhases::PLATEAU &&
      m_currentPressure > m_maxPlateauPressure) {
    DBG_PRESSION_PLATEAU(p_currentCentieme, 80)
    m_patient.command = m_blower.position + 1;
  }
  // si pression PEP < PEP mini, alors fermeture complète valve expiration
  if (m_currentPressure < m_minPeep) {
    DBG_PRESSION_PEP(p_currentCentieme, 80)
    m_patient.command = ANGLE_FERMETURE;
    m_phase = CyclePhases::HOLD_EXHALATION;
  }
}

void PressureController::computeCentiSecParameters() {
  m_centiSecPerCycle = 60 * 100 / m_cyclesPerMinute;
  // Inhalation = 1/3 of the cycle duration, Exhalation = 2/3 of the cycle
  // duration
  m_centiSecPerInhalation = m_centiSecPerCycle / 3;
}

void PressureController::executeCommands()
{
    m_blower.execute();
    m_patient.execute();
}

void PressureController::onCycleMinus() {
#ifdef DEBUG
  Serial.println("nb cycle --");
#endif
  m_cyclesPerMinuteCommand--;
  if (m_cyclesPerMinuteCommand < BORNE_INF_CYCLE) {
    m_cyclesPerMinuteCommand = BORNE_INF_CYCLE;
  }
}

void PressureController::onCyclePlus() {
#ifdef DEBUG
  Serial.println("nb cycle ++");
#endif
  m_cyclesPerMinuteCommand++;
  if (m_cyclesPerMinuteCommand > BORNE_SUP_CYCLE) {
    m_cyclesPerMinuteCommand = BORNE_SUP_CYCLE;
  }
}

void PressureController::onPressionPepMinus() {
#ifdef DEBUG
  Serial.println("pression PEP --");
#endif
  m_minPeepCommand--;
  if (m_minPeepCommand < BORNE_INF_PRESSION_PEP) {
    m_minPeepCommand = BORNE_INF_PRESSION_PEP;
  }
}

void PressureController::onPressionPepPlus() {
#ifdef DEBUG
  Serial.println("pression PEP ++");
#endif
  m_minPeepCommand++;
  if (m_minPeepCommand > BORNE_SUP_PRESSION_PEP) {
    m_minPeepCommand = BORNE_SUP_PRESSION_PEP;
  }
}

void PressureController::onPressionPlateauMinus() {
#ifdef DEBUG
  Serial.println("pression plateau --");
#endif
  m_maxPlateauPressureCommand--;
  if (m_maxPlateauPressureCommand < BORNE_INF_PRESSION_PLATEAU) {
    m_maxPlateauPressureCommand = BORNE_INF_PRESSION_PLATEAU;
  }
}

void PressureController::onPressionPlateauPlus() {
#ifdef DEBUG
  Serial.println("pression plateau ++");
#endif
  m_maxPlateauPressureCommand++;
  if (m_maxPlateauPressureCommand > BORNE_SUP_PRESSION_PLATEAU) {
    m_maxPlateauPressureCommand = BORNE_SUP_PRESSION_PLATEAU;
  }
}
