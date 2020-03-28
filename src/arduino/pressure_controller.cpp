/*=============================================================================
 * @file pressure_controller.h
 *
 * COVID Respirator
 *
 * @section copyright Copyright
 *
 * Makers For Life
 *
 * @section descr File description
 *
 * This file implements the PressureController object
 */

// INCLUDES ===================================================================

// Associated header
#include "pressure_controller.h"

// Internal libraries
#include "config.h"
#include "debug.h"
#include "parameters.h"

// INITIALISATION =============================================================

PressureController pController;

// FUNCTIONS ==================================================================

PressureController::PressureController()
    : m_cyclesPerMinuteCommand(20),
      m_minPeepCommand(BORNE_INF_PRESSION_PEP), // mmH2O
      m_maxPlateauPressureCommand(BORNE_SUP_PRESSION_PLATEAU), //mmH20
      m_apertureCommand(ANGLE_OUVERTURE_MAXI),
      m_cyclesPerMinute(20),
      m_aperture(ANGLE_OUVERTURE_MAXI),
      m_maxPeakPressure(BORNE_SUP_PRESSION_CRETE), // mmH2O
      m_maxPlateauPressure(BORNE_SUP_PRESSION_PLATEAU), // mmH2O
      m_minPeep(BORNE_INF_PRESSION_PEP), //mmH2O
      m_pressure(-1),
      m_peakPressure(-1),
      m_plateauPressure(-1),
      m_peep(-1),
      m_phase(CyclePhases::INHALATION)
{
    computeCentiSecParameters();
}

PressureController::PressureController(int16_t p_cyclesPerMinute,
                                       int16_t p_minPeep,
                                       int16_t p_maxPlateauPressure,
                                       int16_t p_aperture,
                                       int16_t p_maxPeakPressure,
                                       AirTransistor p_blower,
                                       AirTransistor p_patient,
                                       AirTransistor p_y)
    : m_cyclesPerMinuteCommand(p_cyclesPerMinute),
      m_minPeepCommand(p_minPeep),
      m_maxPlateauPressureCommand(p_maxPlateauPressure),
      m_apertureCommand(p_aperture),
      m_cyclesPerMinute(p_cyclesPerMinute),
      m_aperture(p_aperture),
      m_maxPeakPressure(p_maxPeakPressure),
      m_maxPlateauPressure(p_maxPlateauPressure),
      m_minPeep(p_minPeep),
      m_pressure(-1),
      m_peakPressure(-1),
      m_plateauPressure(-1),
      m_peep(-1),
      m_phase(CyclePhases::INHALATION),
      m_blower(p_blower),
      m_patient(p_patient),
      m_y(p_y)
{
    computeCentiSecParameters();
}

void PressureController::setup()
{
    m_blower.actuator.attach(PIN_SERVO_BLOWER);
    m_patient.actuator.attach(PIN_SERVO_PATIENT);
    m_y.actuator.attach(PIN_SERVO_Y);

    DBG_DO(Serial.print("mise en secu initiale");)

    m_blower.actuator.write(m_blower.failsafeCommand);
    m_patient.actuator.write(m_patient.failsafeCommand);
    m_y.actuator.write(m_y.failsafeCommand);
}

void PressureController::initRespiratoryCycle()
{
    m_peakPressure = -1;
    m_plateauPressure = -1;
    m_peep = -1;
    m_phase = CyclePhases::INHALATION;
    m_blower.reset();
    m_patient.reset();
    m_y.reset();

    computeCentiSecParameters();

    DBG_AFFICHE_CSPCYCLE_CSPINSPI(m_centiSecPerCycle, m_centiSecPerInhalation)

    m_cyclesPerMinute = m_cyclesPerMinuteCommand;
    m_aperture = m_apertureCommand;
    m_minPeep = m_minPeepCommand;
    m_maxPlateauPressure = m_maxPlateauPressureCommand;

    DBG_AFFICHE_CONSIGNES(m_cyclesPerMinute, m_aperture, m_minPeep, m_maxPlateauPressure)
}

void PressureController::updatePressure(int16_t p_currentPressure)
{
    m_pressure = p_currentPressure;
}

void PressureController::compute(uint16_t p_centiSec)
{
    // Update the cycle phase
    updatePhase(p_centiSec);

    // Act accordingly
    switch (m_phase)
    {
    case CyclePhases::INHALATION:
    {
        inhale();
        break;
    }
    case CyclePhases::PLATEAU:
    {
        plateau();
        break;
    }
    case CyclePhases::EXHALATION:
    {
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

    safeguards(p_centiSec);

    DBG_PHASE_PRESSION(p_centiSec, 50, m_phase, m_pressure)

    executeCommands();
}

void PressureController::onCycleMinus()
{
    DBG_DO(Serial.println("nb cycle --");)
    m_cyclesPerMinuteCommand--;
    if (m_cyclesPerMinuteCommand < BORNE_INF_CYCLE)
    {
        m_cyclesPerMinuteCommand = BORNE_INF_CYCLE;
    }
}

void PressureController::onCyclePlus()
{
    DBG_DO(Serial.println("nb cycle ++");)
    m_cyclesPerMinuteCommand++;
    if (m_cyclesPerMinuteCommand > BORNE_SUP_CYCLE)
    {
        m_cyclesPerMinuteCommand = BORNE_SUP_CYCLE;
    }
}

void PressureController::onPressionPepMinus()
{
    DBG_DO(Serial.println("pression PEP --");)
    m_minPeepCommand = m_minPeepCommand - 10;
    if (m_minPeepCommand < BORNE_INF_PRESSION_PEP)
    {
        m_minPeepCommand = BORNE_INF_PRESSION_PEP;
    }
}

void PressureController::onPressionPepPlus()
{
    DBG_DO(Serial.println("pression PEP ++");)
    m_minPeepCommand = m_minPeepCommand + 10;
    if (m_minPeepCommand > BORNE_SUP_PRESSION_PEP)
    {
        m_minPeepCommand = BORNE_SUP_PRESSION_PEP;
    }
}

void PressureController::onPressionPlateauMinus()
{
    DBG_DO(Serial.println("pression plateau --");)
    m_maxPlateauPressureCommand = m_maxPlateauPressureCommand - 10;
    if (m_maxPlateauPressureCommand < BORNE_INF_PRESSION_PLATEAU)
    {
        m_maxPlateauPressureCommand = BORNE_INF_PRESSION_PLATEAU;
    }
}

void PressureController::onPressionPlateauPlus()
{
    DBG_DO(Serial.println("pression plateau ++");)
    m_maxPlateauPressureCommand = m_maxPlateauPressureCommand + 10;
    if (m_maxPlateauPressureCommand > BORNE_SUP_PRESSION_PLATEAU)
    {
        m_maxPlateauPressureCommand = BORNE_SUP_PRESSION_PLATEAU;
    }
}

void PressureController::onPressionCreteMinus()
{
    DBG_DO(Serial.println("pression crete --");)
    // TODO
}

void PressureController::onPressionCretePlus()
{
    DBG_DO(Serial.println("pression plateau ++");)
    // TODO
}

void PressureController::updatePhase(uint16_t p_centiSec)
{
    if (p_centiSec <= m_centiSecPerInhalation)
    {
        m_phase = m_pressure >= m_peakPressure ? CyclePhases::INHALATION : CyclePhases::PLATEAU;
    }
    else
    {
        m_phase = CyclePhases::EXHALATION;
    }
}

void PressureController::inhale()
{
    // Open the air stream towards the patient's lungs
    m_blower.command = 70;

    // Direct the air stream towards the patient's lungs
    m_y.command = 65;

    // Open the air stream towards the patient's lungs
    m_patient.command = 80;

    // Update the peak pressure
    m_peakPressure = m_pressure;
}

void PressureController::plateau()
{
    // Deviate the air stream outside
    m_blower.command = 50;

    // Direct the air stream towards the patient's lungs
    m_y.command = 65;

    // Close the air stream towards the patient's lungs
    m_patient.command = 80;

    // Update the plateau pressure
    m_plateauPressure = m_pressure;
}

void PressureController::exhale()
{
    // Deviate the air stream outside
    m_blower.command = 25;

    // Direct the air stream towards the patient's lungs
    m_y.command = 0;

    // Open the valve so the patient can exhale outside
    m_patient.command = 25;

    // Update the PEEP
    m_peep = m_pressure;
}

void PressureController::safeguards(uint16_t p_centiSec)
{
    if (m_pressure > m_maxPeakPressure)
    {
        DBG_PRESSION_CRETE(p_centiSec, 80)
        // Close the blower's valve by 2°
        m_blower.command = m_blower.position - 2;

        if (m_blower.command < 25) {
            m_blower.command = 25;
        }
    }

    if (m_phase == CyclePhases::PLATEAU && m_pressure > m_maxPlateauPressure)
    {
        DBG_PRESSION_PLATEAU(p_centiSec, 80)
        // Open the patient's valve by 1° to ease exhalation
        m_patient.command = m_blower.position + 1;

        if (m_patient.command > 80) {
            m_patient.command = 80;
        }
    }

    if (m_pressure < m_minPeep)
    {
        DBG_PRESSION_PEP(p_centiSec, 80)
        // Close completely the patient's valve
        m_patient.command = 80;
        m_phase = CyclePhases::HOLD_EXHALATION;
    }
}

void PressureController::computeCentiSecParameters()
{
    m_centiSecPerCycle = 60 * 100 / m_cyclesPerMinute;
    // Inhalation = 1/3 of the cycle duration,
    // Exhalation = 2/3 of the cycle duration
    m_centiSecPerInhalation = m_centiSecPerCycle / 3;
}

void PressureController::executeCommands()
{
    m_blower.execute();
    m_patient.execute();
    m_y.execute();
}
