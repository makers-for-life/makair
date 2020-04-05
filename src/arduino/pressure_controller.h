/*
    Copyright (C) 2020 Makers For Life
*/
/******************************************************************************
 * @author Makers For Life
 * @file pressure_controller.h
 * @brief Core logic to control the breathing cycle
 *****************************************************************************/

#pragma once

// INCLUDES ===================================================================

// External libraries

// Internal libraries
#include "air_transistor.h"
#include "common.h"

// CLASS ======================================================================

/// Controls breathing cycle
class PressureController {
 public:
    /// Default constructor
    PressureController();

    /**
     * Parameterized constructor
     *
     * @param p_cyclesPerMinute     Initial number of breathing cycles per minute
     * @param p_minPeep             Initial minimum PEEP pressure (in mmH2O)
     * @param p_maxPlateauPressure  Initial maximum plateau pressure (in mmH2O)
     * @param p_maxPeakPressure     Initial maximum peak pressure (in MMH2O)
     * @param p_aperture
     * @param p_blower              Air Transistor between blower and patient
     * @param p_patient             Air Transistor between patient and atmosphere
     */
    PressureController(int16_t p_cyclesPerMinute,
                       int16_t p_minPeep,
                       int16_t p_maxPlateauPressure,
                       int16_t p_maxPeakPressure,
                       int16_t p_aperture,
                       AirTransistor p_blower,
                       AirTransistor p_patient);

    /// Initialize actuators
    void setup();

    /// Begin a respiratory cycle
    void initRespiratoryCycle();

    /**
     * Input a pressure reading
     * @param p_pressure  Measured pressure
     */
    void updatePressure(int16_t p_pressure);

    /**
     * Perform the pressure control
     *
     * @param p_centiSec  Duration in hundredth of second from the begining of the cycle
     */
    void compute(uint16_t p_centiSec);

    /// Decrease the desired number of cycles per minute
    void onCycleMinus();

    /// Increase the desired number of cycles per minute
    void onCyclePlus();

    /// Decrease the minimal PEEP desired
    void onPressionPepMinus();

    /// Increase the minimal PEEP desired
    void onPressionPepPlus();

    /// Decrease the desired plateau pressure
    void onPressionPlateauMinus();

    /// Increase the desired plateau pressure
    void onPressionPlateauPlus();

    /// Decrease the desired crête pressure
    void onPressionCreteMinus();

    /// Increase the desired crête pressure
    void onPressionCretePlus();

    /// Get the desired number of cycles per minute
    inline uint16_t cyclesPerMinuteCommand() const { return m_cyclesPerMinuteCommand; }

    /// Get the desired max peak
    inline uint16_t maxPeakPressureCommand() const { return m_maxPeakPressureCommand; }

    /// Get the desired minimal PEEP
    inline uint16_t minPeepCommand() const { return m_minPeepCommand; }

    /// Get the desired maximal plateau pressure
    inline uint16_t maxPlateauPressureCommand() const { return m_maxPlateauPressureCommand; }

    /// Get the desired blower aperture
    inline uint16_t apertureCommand() const { return m_apertureCommand; }

    /// Get the number of cycles per minute
    inline uint16_t cyclesPerMinute() const { return m_cyclesPerMinute; }

    /// Get the duration of a cycle in hundredth of second
    inline uint16_t centiSecPerCycle() const { return m_centiSecPerCycle; }

    /// Get the duration of an inhalation in hundredth of second
    inline uint16_t centiSecPerInhalation() const { return m_centiSecPerInhalation; }

    /// Get the current measured pressure
    inline int16_t pressure() const { return m_pressure; }

    /// Get the peak pressure
    inline int16_t peakPressure() const { return m_peakPressure; }

    /// Get the plateau pressure
    inline int16_t plateauPressure() const { return m_plateauPressure; }

    /// Get the PEEP
    inline int16_t peep() const { return m_peep; }

    /// Get the current cycle phase
    inline CyclePhases phase() const { return m_phase; }

    /// Get the current cycle subphase
    inline CycleSubPhases subPhase() const { return m_subPhase; }

    /// Get the blower's Air Transistor instance
    inline const AirTransistor& blower() const { return m_blower; }

    /// Get the patient's Air Transistor instance
    inline const AirTransistor& patient() const { return m_patient; }

    void updateDt(int32_t p_dt);

 private:
    /**
     * Update the cycle phase
     *
     * @param p_centiSec  Duration from the begining of the cycle in hundredth of second
     */
    void updatePhase(uint16_t p_centiSec);

    /// Perform the pressure control and compute the transistors commands during the inhalation
    /// phase
    void inhale();

    /// Perform the pressure control and compute the transistors commands during the plateau phase
    void plateau();

    /// Perform the pressure control and compute the transistors commands during the exhalation
    /// phase
    void exhale();

    /// Perform the pressure control and compute the transistors commands during the hold exhalation
    /// phase
    void holdExhalation();

    /**
     * Run safeguards
     *
     * @param p_centiSec  Duration from the begining of the cycle in hundredth of second
     */
    void safeguards(uint16_t p_centiSec);

    /**
     * Implement safeguard for peak pressure
     *
     * @param p_centiSec  Duration from the begining of the cycle in hundredth of second
     */
    void safeguardPressionCrete(uint16_t p_centiSec);

    /**
     * Implement safeguard for max plateau pressure
     *
     * @param p_centiSec  Duration from the begining of the cycle in hundredth of second
     */
    void safeguardPressionPlateau(uint16_t p_centiSec);

    /**
     * Implement a first safeguard for peep pressure
     *
     * In this case, we hold the exhalation.
     * @param p_centiSec  Duration from the begining of the cycle in hundredth of second
     */
    void safeguardHoldExpiration(uint16_t p_centiSec);

    /**
     * Implement a second safeguard for peep pressure
     *
     * If the hold exhalation is not enough, we start to open the blower valve in order to maintain
     * a pressure
     * @param p_centiSec  Duration from the begining of the cycle in hundredth of second
     */
    void safeguardMaintienPeep(uint16_t p_centiSec);

    /**
     * Compute various cycle durations given the desired number of cycles per minute
     *
     * - duration of a cycle in hundredth of second
     * - duration of the inhalation phase in hundredth of second
     *
     *  N.B.: Inhalation lasts 1/3 of a cycle while exhalation lasts 2/3 of a cycle
     */
    void computeCentiSecParameters();

    /// Give the computed commands to actuators
    void executeCommands();

    /// Make a transition toward another subphase
    void setSubPhase(CycleSubPhases p_subPhase);

    int32_t pidBlower(int32_t targetPressure, int32_t currentPressure, int32_t deltaTime);

    int32_t pidPatient(int32_t targetPressure, int32_t currentPressure, int32_t deltaTime);

 private:
    /// Number of cycles per minute desired by the operator
    uint16_t m_cyclesPerMinuteCommand;

    /**
     * Vigilance mode
     *
     * True when safety mode is ON, false in normal mode
     */
    bool m_vigilance;

    /// Maximal peak pressure desired by the operator
    uint16_t m_maxPeakPressureCommand;

    /// Tick de détection initiale pour le dépassement de la consigne de crête
    uint16_t m_franchissementSeuilMaxPeakPressureDetectionTick;

    /// Tick de suppression du tick de détection initiale pour le dépassement de la consigne de
    /// crête
    uint16_t m_franchissementSeuilMaxPeakPressureDetectionTickSupprime;

    /// Maximal plateau pressure desired by the operator
    uint16_t m_maxPlateauPressureCommand;

    /// Tick de détection initiale pour le dépassement de la consigne de plateau
    uint16_t m_franchissementSeuilMaxPlateauPressureDetectionTick;

    /// Tick de suppression du tick de détection initiale pour le dépassement de la consigne de
    /// plateau
    uint16_t m_franchissementSeuilMaxPlateauPressureDetectionTickSupprime;

    /// Minimal PEEP desired by the operator
    uint16_t m_minPeepCommand;

    /// Tick de détection initiale pour le maintien de la PEEP
    uint16_t m_franchissementSeuilHoldExpiDetectionTick;

    /// Tick de suppression du tick de détection initiale que la PEEP est maintenue
    uint16_t m_franchissementSeuilHoldExpiDetectionTickSupprime;

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

    /// Current respiratory cycle phase
    CycleSubPhases m_subPhase;

    /// Blower's transistor
    AirTransistor m_blower;

    /// Patient's transistor
    AirTransistor m_patient;

    uint16_t m_previousPhase;

    /// Number of passed cycles
    uint32_t m_cycleNb;
    int32_t m_dt;
    int32_t m_consignePression;

    int32_t blowerIntegral;

    int32_t blowerLastError;

    int32_t patientIntegral;

    int32_t patientLastError;
};

// INITIALISATION =============================================================

/// Instance of the pressure controller
extern PressureController pController;
