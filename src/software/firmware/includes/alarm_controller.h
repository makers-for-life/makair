/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file alarm_controller.h
 * @brief Core logic to manage alarm features
 *****************************************************************************/

#pragma once

// INCLUDES ===================================================================

// Internals
#include "../includes/alarm.h"
#include "../includes/cycle.h"

// CONSTANTS ==================================================================

#define ALARMS_SIZE 10u

#define RCM_SW_1 12u
#define RCM_SW_2 11u
#define RCM_SW_3 14u
// #define RCM_SW_6 15u - NOT IN THIS VERSION
// #define RCM_SW_8 18u - NOT IN THIS VERSION
#define RCM_SW_11 21u
#define RCM_SW_12 13u
#define RCM_SW_14 22u
#define RCM_SW_15 23u
#define RCM_SW_16 31u
#define RCM_SW_18 17u
#define RCM_SW_19 24u

// CLASS =====================================================================

class AlarmController {
 public:
    /// Default constructor
    AlarmController();

    /**
     * Snooze alarm for 2 minutes
     *
     * There is no more buzzer during the alarm
     */
    void snooze();

    /**
     * Mark a specific alarm as detected
     *
     * @param p_alarmCode The code of the alarm
     * @param p_cycleNumber The cycle number since the device startup
     * @param p_expected The expected value
     * @param p_measured The measured value that was different from the expected value thus
     * triggering the alarm
     */
    void detectedAlarm(uint8_t p_alarmCode,
                       uint32_t p_cycleNumber,
                       uint32_t p_expected,
                       uint32_t p_measured);

    /**
     * Reset detection of a specific alarm
     * @param p_alarmCode The code of the alarm
     */
    void notDetectedAlarm(uint8_t p_alarmCode);

    /**
     * Run effects (buzzer, LCD message, LED) according to the currently triggered alarms
     *
     * @param p_centiSec Centile in the respiratory cycle
     */
    void runAlarmEffects(uint16_t p_centiSec);

    /// Update internal state of alarm controller with data from pressure controller
    void updateCoreData(uint16_t p_centile,
                        uint16_t p_pressure,
                        CyclePhases p_phase,
                        CycleSubPhases p_subphase,
                        uint32_t p_cycle_number);

    /// Get the alarms triggered during this cycle
    uint8_t* triggeredAlarms() { return m_triggeredAlarms; }

 private:
    /// Highest priority of the currently triggered alarms
    AlarmPriority m_highestPriority;

    /// Time when snoozed was triggered
    uint32_t m_snoozeTime;

    /// Collections of available alarms
    Alarm m_alarms[ALARMS_SIZE];

    /// Collections of snoozed alarms
    bool m_snoozedAlarms[ALARMS_SIZE];

    /// Alarms currently triggered
    uint8_t m_triggeredAlarms[ALARMS_SIZE];

    /// Current pressure
    uint16_t m_centile;

    /// Current pressure
    uint16_t m_pressure;

    /// Current phase
    CyclePhases m_phase;

    /// Current subphase
    CycleSubPhases m_subphase;

    /// Current cycle number
    uint32_t m_cycle_number;
};

// INITIALISATION =============================================================

/// Instance of the alarm controller
extern AlarmController alarmController;
