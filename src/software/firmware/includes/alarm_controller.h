/*
    Copyright (C) 2020 Makers For Life
*/
/******************************************************************************
 * @author Makers For Life
 * @file alarm_controller.h
 * @brief Core logic to manage alarm features
 *****************************************************************************/

#pragma once

// INCLUDES ===================================================================

// Internals
#include "../includes/alarm.h"

// CONSTANTS ==================================================================

#define ALARMS_SIZE 10

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
     */
    void detectedAlarm(uint8_t p_alarmCode, uint32_t p_cycleNumber);

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

 private:
    /// Collections of available alarms
    Alarm m_alarms[ALARMS_SIZE];

    /// Highest priority of the currently triggered alarms
    AlarmPriority m_highestPriority;

    /// Snoozed priority
    AlarmPriority m_snoozed;
};

// INITIALISATION =============================================================

/// Instance of the alarm controller
extern AlarmController alarmController;
