/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file pressure_controller.h
 * @brief Core logic to control the breathing cycle
 *****************************************************************************/

#pragma once

// INCLUDES ===================================================================

// Internal
#include "../includes/alarm_controller.h"
#include "../includes/blower.h"
#include "../includes/cycle.h"
#include "../includes/pressure_valve.h"

/// Number of values to aggregate when computing plateau pressure
#define MAX_PRESSURE_SAMPLES 10u

/// Max value for increment / decrement of peak command
static const uint16_t MAX_PEAK_INCREMENT = 30u;

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
     * @param p_minPeepCommand      Initial minimum PEEP pressure (in mmH2O)
     * @param p_maxPlateauPressure  Initial maximum plateau pressure (in mmH2O)
     * @param p_maxPeakPressure     Initial maximum peak pressure (in mmH2O)
     * @param p_blower_valve        Pressure Valve between blower and patient
     * @param p_patient_valve       Pressure Valve between patient and atmosphere
     * @param p_alarmController     Alarm controller
     * @param p_blower              Blower
     */
    // cppcheck-suppress misra-c2012-2.7
    PressureController(int16_t p_cyclesPerMinute,
                       int16_t p_minPeepCommand,
                       int16_t p_maxPlateauPressure,
                       int16_t p_maxPeakPressure,
                       const PressureValve& p_blower_valve,
                       const PressureValve& p_patient_valve,
                       AlarmController* p_alarmController,
                       Blower* p_blower);

    /// Initialize actuators
    void setup();

    /// Begin a respiratory cycle
    void initRespiratoryCycle();

    /// End a respiratory cycle
    void endRespiratoryCycle();

    /**
     * Input a pressure reading
     * @param p_currentPressure  Measured pressure
     */
    void updatePressure(int16_t p_currentPressure);

    /**
     * Perform the pressure control
     *
     * @param p_centiSec  Duration in hundredth of second from the begining of the cycle
     */
    void compute(uint16_t p_centiSec);

    /// Decrease the desired number of cycles per minute
    void onCycleDecrease();

    /// Increase the desired number of cycles per minute
    void onCycleIncrease();

    /// Decrease the minimal PEEP desired
    void onPeepPressureDecrease();

    /// Increase the minimal PEEP desired
    void onPeepPressureIncrease();

    /// Decrease the desired plateau pressure
    void onPlateauPressureDecrease();

    /// Increase the desired plateau pressure
    void onPlateauPressureIncrease();

    /**
     * Decrease the desired peak pressure
     *
     * @param p_decrement Positive value of decrement
     */
    void onPeakPressureDecrease(uint8_t p_decrement);

    /**
     * Increase the desired peak pressure
     *
     * @param p_increment Positive value of increment
     */
    void onPeakPressureIncrease(uint8_t p_increment);

    /// Get the desired number of cycles per minute
    inline uint16_t cyclesPerMinuteCommand() const { return m_cyclesPerMinuteCommand; }

    /// Get the desired max peak
    inline uint16_t maxPeakPressureCommand() const { return m_maxPeakPressureCommand; }

    /// Get the desired minimal PEEP
    inline uint16_t minPeepCommand() const { return m_minPeepCommand; }

    /// Get the desired maximal plateau pressure
    inline uint16_t maxPlateauPressureCommand() const { return m_maxPlateauPressureCommand; }

    /// Get the number of cycles per minute
    inline uint16_t cyclesPerMinute() const { return m_cyclesPerMinute; }

    /// Get the number of past cycles since the beginning
    inline uint32_t cycleNumber() const { return m_cycleNb; }

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

    /// Get the blower's Pressure Valve instance
    inline const PressureValve& blower_valve() const { return m_blower_valve; }

    /// Get the patient's Pressure Valve instance
    inline const PressureValve& patient_valve() const { return m_patient_valve; }

    /**
     * Input the real duration since the last pressure controller computation
     *
     * @param p_dt Duration in microsecond
     */
    void updateDt(int32_t p_dt);

 private:
    /**
     * Update the cycle phase
     *
     * @param p_centiSec  Duration from the begining of the cycle in hundredth of second
     */
    void updatePhase(uint16_t p_centiSec);

    /// Update peak pressure and blower ramp up
    void updatePeakPressure();

    /// Perform the pressure control and compute the transistors commands during the inhalation
    /// phase
    void inhale();

    /// Perform the pressure control and compute the transistors commands during the plateau phase
    void plateau(uint16_t p_centiSec);

    /// Perform the pressure control and compute the transistors commands during the exhalation
    /// phase
    void exhale();

    /**
     * Compute various cycle durations given the desired number of cycles per minute
     *
     * - duration of a cycle in hundredth of second
     * - duration of the inhalation phase in hundredth of second
     *
     *  N.B.: Inhalation lasts 1/3 of a cycle while exhalation lasts 2/3 of a cycle
     */
    void computeCentiSecParameters();

    /**
     * Compute plateau pressure
     *
     * @param p_centiSec  Duration from the begining of the cycle in hundredth of second
     */
    void computePlateau(uint16_t p_centiSec);

    /// Give the computed commands to actuators
    void executeCommands();

    /**
     * Make a transition toward another subphase
     *
     * @param p_subPhase  Next cycle step
     * @param p_centiSec  Duration from the begining of the cycle in hundredth of second
     */
    void setSubPhase(CycleSubPhases p_subPhase, uint16_t p_centiSec);

    /**
     * PID to controller the blower valve during some specific steps of the cycle
     *
     * @param targetPressure The pressure we want (in mmH2O)
     * @param currentPressure The pressure measured by the sensor (in mmH2O)
     * @param dt Time since the last computation (in microsecond)
     */
    int32_t pidBlower(int32_t targetPressure, int32_t currentPressure, int32_t dt);

    /**
     * PID to controller the patient valve during some specific steps of the cycle
     *
     * @param targetPressure The pressure we want (in mmH2O)
     * @param currentPressure The pressure measured by the sensor (in mmH2O)
     * @param dt Time since the last computation (in microsecond)
     */
    int32_t pidPatient(int32_t targetPressure, int32_t currentPressure, int32_t dt);

    /// At the end of a respiratory cycle, check if some alarms are triggered
    void checkCycleAlarm();

 private:
    /// Number of cycles per minute desired by the operator
    uint16_t m_cyclesPerMinuteCommand;

    /// Maximal peak pressure desired by the operator
    uint16_t m_maxPeakPressureCommand;

    /// Maximal plateau pressure desired by the operator
    uint16_t m_maxPlateauPressureCommand;

    /// Minimal PEEP desired by the operator
    uint16_t m_minPeepCommand;

    /// Number of cycles per minute
    uint16_t m_cyclesPerMinute;

    /// Number of hundredth of second per cycle
    uint16_t m_centiSecPerCycle;

    /// Number of hundredth of second per inhalation
    uint16_t m_centiSecPerInhalation;

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

    /// Blower valve angle at peak
    uint32_t m_peakBlowerValveAngle;

    /// Current respiratory cycle phase
    CyclePhases m_phase;

    /// Current respiratory cycle phase
    CycleSubPhases m_subPhase;

    /// Blower's transistor
    PressureValve m_blower_valve;

    /// Patient's transistor
    PressureValve m_patient_valve;

    /// Blower
    Blower* m_blower;

    /// Blower increment
    int32_t m_blower_increment;

    /// Number of passed cycles
    uint32_t m_cycleNb;

    /// Time since the last computation (in microsecond)
    int32_t m_dt;

    /// Requested pressure at a given point in time
    int32_t m_pressureCommand;

    /**
     * Integral gain of the blower PID
     *
     * @note This must be persisted between computations
     */
    int32_t blowerIntegral;

    /**
     * Error of the last computation of the blower PID
     *
     * @note This must be persisted between computation in order to compute derivative gain
     */
    int32_t blowerLastError;

    /**
     * Integral gain of the patient PID
     *
     * @note This must be persisted between computations
     */
    int32_t patientIntegral;

    /**
     * Error of the last computation of the patient PID
     *
     * @note This must be persisted between computation in order to compute derivative gain
     */
    int32_t patientLastError;

    /// Alarm controller
    AlarmController* m_alarmController;

    /// True if we started to compute plateau measure, false otherwise
    bool m_startPlateauComputation;

    /// True if plateau is computed, false otherwise
    bool m_plateauComputed;

    /// Last pressure values
    uint16_t m_lastPressureValues[MAX_PRESSURE_SAMPLES];

    /// Index of array for last pressure storage
    uint16_t m_lastPressureValuesIndex;

    /// Sum of the current cycle's pressures
    uint32_t m_sumOfPressures;

    /// Number of the current cycle's pressures
    uint16_t m_numberOfPressures;

    /// Number of hundredth of second from the begining of the cycle till the plateau phase
    uint16_t m_plateauStartTime;
};

// INITIALISATION =============================================================

/// Instance of the pressure controller
extern PressureController pController;
