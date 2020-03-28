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
                       AirTransistor p_patient);

    /*! This function initializes the actuators
     *  This function modifies the followint attributes:
     *  -   m_blower
     *  -   m_patient
     */
    void setup();

    /*! This function initializes the respiratory cycle
     *  This function modifies the followint attributes:
     *  -   m_peakPressure
     *  -   m_plateauPressure
     *  -   m_peep
     *  -   m_phase
     *  -   m_blower
     *  -   m_patient
     *  -   m_centiSecPerCycle
     *  -   m_centiSecPerInhalation
     *  -   m_cyclesPerMinute
     *  -   m_aperture
     *  -   m_maxPlateauPressure
     */
    void initRespiratoryCycle();

    /*! This function updates the pressure given the sensor's measure
     *  This function modifies the followint attributes:
     *  -   m_pressure
     *
     *  \param p_pressure     Measured pressure
     */
    void updatePressure(int16_t p_pressure);

    /*! This function performs the pressure control
     *  This function modifies the followint attributes:
     *  -   m_blower
     *  -   m_patient
     *  -   m_phase
     *  -   m_plateauPressure
     *  -   m_peakPressure
     *  -   m_peep
     *  \param p_centiSec     Current progress in the respiratory cycle in
     *                        hundredth of second
     */
    void compute(uint16_t p_centiSec);

    /*! This function decreases the desired number of cycles per minute
     *  This function modifies the followint attributes:
     *  -   m_cyclesPerMinuteCommand
     */
    void onCycleMinus();

    /*! This function increases the desired number of cycles per minute
     *  This function modifies the followint attributes:
     *  -   m_cyclesPerMinuteCommand
     */
    void onCyclePlus();

    /*! This function decreases the minimal PEEP desired
     *  This function modifies the followint attributes:
     *  -   m_minPeepCommand
     */
    void onPressionPepMinus();

    /*! This function increases the minimal PEEP desired
     *  This function modifies the followint attributes:
     *  -   m_minPeepCommand
     */
    void onPressionPepPlus();

    /*! This function decreases the desired plateau pressure
     *  This function modifies the followint attributes:
     *  -   m_maxPlateauPressureCommand
     */
    void onPressionPlateauMinus();

    /*! This function increases the desired plateau pressure
     *  This function modifies the followint attributes:
     *  -   m_maxPlateauPressureCommand
     */
    void onPressionPlateauPlus();

    //! This function decreases the desired crete pressure (TODO)
    void onPressionCreteMinus();

    //! This function increases the desired crete pressure (TODO)
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

private:
    /*! This function updates the cycle phase
     *  This function modifies the followint attributes:
     *  -   m_phase
     *  \param p_centiSec     Current progress in the respiratory cycle in hundredth of second
     */
    void updatePhase(uint16_t p_centiSec);

    /*! This function performs the pressure control and computes the transistors commands
     *  during the inhalation phase
     *  This function modifies the followint attributes:
     *  -   m_blower
     *  -   m_patient
     *  -   m_peakPressure
     */
    void inhale();

    /*! This function performs the pressure control and computes the transistors commands
     *  during the plateau phase
     *  This function modifies the followint attributes:
     *  -   m_blower
     *  -   m_patient
     *  -   m_plateauPressure
     */
    void plateau();

    /*! This function performs the pressure control and computes the transistors
     * commands during the exhalation phase
     *  This function modifies the followint attributes:
     *  -   m_blower
     *  -   m_patient
     *  -   m_peep
     */
    void exhale();

    /*! This function implements safeguards
     *  This function modifies the followint attributes:
     *  -   m_blower
     *  -   m_patient
     *  -   m_phase
     *  \param p_centiSec     Current progress in the respiratory cycle in hundredth of second
     */
    void safeguards(uint16_t p_centiSec);

    /*! This function computes:
     *  - the number of hundredth of second per cycle
     *  - the number of hundredth of second per inhalation
     *  given the number of cycles per minute predefined by the operator
     *  N.B.: Inhalation lasts 1/3 of a cycle while exhalation lasts 2/3 of a cycle
     *  This function modifies the followint attributes:
     *  -   m_centiSecPerCycle
     *  -   m_centiSecPerInhalation
     */
    void computeCentiSecParameters();

    /*! This function makes the actuators execute the computed commands
     *  This function modifies the followint attributes:
     *  -   m_blower
     *  -   m_patient
     */
    void executeCommands();

    // ATTRIBUTES ---------------------------------------------------------------

private:
    /// Number of cycles per minute desired by the operator
    uint16_t m_cyclesPerMinuteCommand;

    /// Minimal PEEP desired by the operator [mmH2O]
    uint16_t m_minPeepCommand;

    /// Maximal plateau pressure desired by the operator [mmH2O]
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

    /// Maximal peak pressure [mmH2O]
    uint16_t m_maxPeakPressure;

    /// Maximal plateau pressure [mmH2O]
    uint16_t m_maxPlateauPressure;

    /// Minimal PEEP [mmH2O]
    uint16_t m_minPeep;

    /// Measured pressure [mmH2O]
    uint16_t m_pressure;

    /// Peak pressure [mmH2O]
    uint16_t m_peakPressure;

    /// Plateau pressure [mmH2O]
    uint16_t m_plateauPressure;

    /// Positive End Expiratory Pressure [mmH2O]
    uint16_t m_peep;

    /// Current respiratory cycle phase
    CyclePhases m_phase;

    /*! Blower's transistor
     *  This transistor connects the air stream towards the patient or towards the outlet
     *  - at 90°, the valve is completely closed
     *  - between 45° and 82°, the air stream goes outside
     *  - between 98° and 135°, the air stream goes towards the patient's lungs
     *
     *  N.B.:   - 90° = ANGLE_FERMETURE
     *          - 8° = ANGLE_OUVERTURE_MINI
     *          - 45° = ANGLE_OUVERTURE_MAXI
     */
    AirTransistor m_blower;

    /// Patient's transistor
    AirTransistor m_patient;
};

// INITIALISATION =============================================================

extern PressureController pController;
