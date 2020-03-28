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
 * This file defines the pressure control functions
 */

#pragma once

// INCLUDES ===================================================================

// External libraries
#include <Servo.h>

// Internal libraries
#include "air_transistor.h"
#include "common.h"

// CLASS ======================================================================

class PressureController
{
    // METHODS ----------------------------------------------------------------

public:
    //! Default constructor
    PressureController();

    //! Parameterized constructor
    PressureController(int16_t p_cyclesPerMinute,
                       int16_t p_minPeep,
                       int16_t p_maxPlateauPressure,
                       int16_t p_aperture,
                       int16_t p_maxPeakPressure,
                       AirTransistor p_blower,
                       AirTransistor p_patient,
                       AirTransistor p_y);

    //! This function initializes the actuators
    void setup();

    //! This function initializes the respiratory cycle
    void initRespiratoryCycle();

    /*! This function updates the pressure given the sensor's measure
     *  \param p_pressure     Measured pressure
     */
    void updatePressure(int16_t p_pressure);

    /*! This function performs the pressure control
     *  \param p_centiSec     Current progress in the respiratory cycle in
     *                        hundredth of second
     */
    void compute(uint16_t p_centiSec);

    //! This function decreases the desired number of cycles per minute
    void onCycleMinus();

    //! This function increases the desired number of cycles per minute
    void onCyclePlus();

    //! This function decreases the minimal PEEP desired
    void onPressionPepMinus();

    //! This function increases the minimal PEEP desired
    void onPressionPepPlus();

    //! This function decreases the desired plateau pressure
    void onPressionPlateauMinus();

    //! This function increases the desired plateau pressure
    void onPressionPlateauPlus();

    //! This function decreases the desired crete pressure
    void onPressionCreteMinus();

    //! This function increases the desired crete pressure
    void onPressionCretePlus();

    //! This function returns the number of cycles per minute desired by the operator
    inline uint16_t cyclesPerMinuteCommand() const { return m_cyclesPerMinuteCommand; }

    //! This function returns the minimal PEEP desired by the operator
    inline uint16_t minPeepCommand() const { return m_minPeepCommand; }

    //! This function returns the maximal plateau pressure desired by the operator
    inline uint16_t maxPlateauPressureCommand() const { return m_maxPlateauPressureCommand; }

    //! This function returns the blower aperture desired by the operator
    inline uint16_t apertureCommand() const { return m_apertureCommand; }

    //! This function returns the number of cycles per minute
    inline uint16_t cyclesPerMinute() const { return m_cyclesPerMinute; }

    //! This function returns the number of hundredth of second per cycle
    inline uint16_t centiSecPerCycle() const { return m_centiSecPerCycle; }

    //! This function returns the number of hundredth of second per inhalation
    inline uint16_t centiSecPerInhalation() const { return m_centiSecPerInhalation; }

    //! This function returns the current measured pressure
    inline int16_t pressure() const { return m_pressure; }

    // This function returns the peak pressure
    inline int16_t peakPressure() const { return m_peakPressure; }

    //! This function returns the plateau pressure
    inline int16_t plateauPressure() const { return m_plateauPressure; }

    //! This function returns the PEEP
    inline int16_t peep() const { return m_peep; }

    //! This function returns the current cycle phase
    inline CyclePhases phase() const { return m_phase; }

    //! This function returns the blower's transistor
    inline const AirTransistor& blower() const { return m_blower; }

    //! This function returns the patient's transistor
    inline const AirTransistor& patient() const { return m_patient; }

    //! This function returns the Y's transistor
    inline const AirTransistor& y() const { return m_y; }

private:
    /*! This function updates the cycle phase
     *  \param p_centiSec     Current progress in the respiratory cycle in hundredth of second
     */
    void updatePhase(uint16_t p_centiSec);

    /*! This function performs the pressure control and computes the transistors commands
     *  during the inhalation phase
     */
    void inhale();

    /*! This function performs the pressure control and computes the transistors commands
     *  during the plateau phase
     */
    void plateau();

    /*! This function performs the pressure control and computes the transistors
     * commands during the exhalation phase
     */
    void exhale();

    /*! This function implements safeguards
     *  \param p_centiSec     Current progress in the respiratory cycle in hundredth of second
     */
    void safeguards(uint16_t p_centiSec);

    /*! This function computes:
     *  - the number of hundredth of second per cycle
     *  - the number of hundredth of second per inhalation
     *  given the number of cycles per minute predefined by the operator
     *  N.B.: Inhalation lasts 1/3 of a cycle while exhalation lasts 2/3 of a cycle
     */
    void computeCentiSecParameters();

    //! This function makes the actuators execute the computed commands
    void executeCommands();

    // ATTRIBUTES ---------------------------------------------------------------

private:
    /// Number of cycles per minute desired by the operator
    uint16_t m_cyclesPerMinuteCommand;

    /// Minimal PEEP desired by the operator
    uint16_t m_minPeepCommand;

    /// Maximal plateau pressure desired by the operator
    uint16_t m_maxPlateauPressureCommand;

    /// Blower's valve aperture desired by the operator
    uint16_t m_apertureCommand;

    /// Number of cycles per minute
    uint16_t m_cyclesPerMinute;

    /// Number of hundredth of second per cycle
    uint16_t m_centiSecPerCycle;

    /// Number of hundredth of second per inhalation
    uint16_t m_centiSecPerInhalation;

    /// Blower's valve aperture angle
    uint16_t m_aperture;

    /// Maximal peak pressure
    uint16_t m_maxPeakPressure;

    /// Maximal plateau pressure
    uint16_t m_maxPlateauPressure;

    /// Minimal PEEP
    uint16_t m_minPeep;

    /// Measured pressure
    uint16_t m_pressure;

    /// Peak pressure
    uint16_t m_peakPressure;

    /// Plateau pressure
    uint16_t m_plateauPressure;

    /// Positive End Expiratory Pressure
    uint16_t m_peep;

    /// Current respiratory cycle phase
    CyclePhases m_phase;

    /// Blower's transistor
    AirTransistor m_blower;

    /// Patient's transistor
    AirTransistor m_patient;

    /// Y's transistor
    AirTransistor m_y;
};

// INITIALISATION =============================================================

extern PressureController pController;
