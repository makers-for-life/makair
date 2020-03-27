/*=============================================================================
 * @file affichage.h
 *
 * COVID Respirator
 *
 * @section copyright Copyright
 *
 * Makers For Life
 *
 * @section descr File description
 *
 * This file defines the pressure control functions
 */

#pragma once

// INCLUDES ===================================================================

#include "common.h"

// CLASS ======================================================================

struct AirTransistor {
  AirTransistor()
      : minApertureAngle(ANGLE_OUVERTURE_MINI),
        maxApertureAngle(ANGLE_OUVERTURE_MAXI),
        defaultCommand(ANGLE_FERMETURE),
        failsafe(ANGLE_FERMETURE - ANGLE_OUVERTURE_MAXI),
        command(ANGLE_FERMETURE),
        position(ANGLE_FERMETURE) {}

  AirTransistor(int16_t p_minApertureAngle, int16_t p_maxApertureAngle,
                int16_t p_defaultCommand, int16_t p_failsafe)
      : minApertureAngle(p_minApertureAngle),
        maxApertureAngle(p_maxApertureAngle),
        defaultCommand(p_defaultCommand),
        failsafe(p_failsafe),
        command(ANGLE_FERMETURE),
        position(ANGLE_FERMETURE) {}

  void reset()
  {
      command = defaultCommand;
      position = defaultCommand;
  }

  int16_t minApertureAngle;
  int16_t maxApertureAngle;
  int16_t defaultCommand;
  int16_t failsafe;
  int16_t command;
  int16_t position;
};

class PressureController {
public:
    PressureController(const AirTransistor &p_blower, const AirTransistor &p_patient);
    void initLoop();
    void updateCurrentPressure(int16_t p_currentPressure);
    void compute(uint16_t p_currentCentieme);
    void updatePhase(uint16_t p_currentCentieme);
    void inhale();
    void plateau();
    void exhale();
    void safeguards(uint16_t p_currentCentieme);

    void computeCentiSecPerCycle();
    void computeCentiSecPerInhalation();

    void applyBlowerCommand() { m_blower.position = m_blower.command; }
    void applyPatientCommand() { m_patient.position = m_patient.command; }

  /*-----------------------------------------------------------------------------
   * The following functions allow to modify the parameters of the breathing
   * cycle and are the interface to the keyboard user input.
   */
    void onCycleMinus();
    void onCyclePlus();
    void onPressionPepMinus();
    void onPressionPepPlus();
    void onPressionPlateauMinus();
    void onPressionPlateauPlus();

    inline uint16_t cyclesPerMinuteCommand() const
    {
        return m_cyclesPerMinuteCommand;
  }
  inline uint16_t minPeepCommand() const { return m_minPeepCommand; }
  inline uint16_t maxPlateauPressureCommand() const { return m_maxPlateauPressureCommand; }
  inline uint16_t centiSecPerCycle() const { return m_centiSecPerCycle; }
  inline int16_t currentPressure() const
  {
      return m_currentPressure;
  }
  inline int16_t peakPressure() const { return m_peakPressure; }
  inline int16_t plateauPressure() const { return m_plateauPressure; }
  inline int16_t peep() const { return m_peep; }
  inline CyclePhases phase() const { return m_phase; }
  inline const AirTransistor &blower() const { return m_blower; }
  inline const AirTransistor &patient() const { return m_patient; }

private:

  // Operator commands
  uint16_t m_cyclesPerMinuteCommand;
  uint16_t m_minPeepCommand;
  uint16_t m_maxPlateauPressureCommand;
  uint16_t m_apertureCommand;

  uint16_t m_cyclesPerMinute;
  uint16_t m_centiSecPerCycle;
  uint16_t m_centiSecPerInhalation;

  int16_t m_aperture;

  int16_t m_maxPeakPressure;
  int16_t m_maxPlateauPressure;
  int16_t m_minPeep;

  int16_t m_currentPressure;
  int16_t m_peakPressure;
  int16_t m_plateauPressure;
  int16_t m_peep;

  CyclePhases m_phase;
  AirTransistor m_blower;
  AirTransistor m_patient;
};
