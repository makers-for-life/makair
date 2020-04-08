/*
    Copyright (C) 2020 Makers For Life
*/
/******************************************************************************
 * @author Makers For Life
 * @file alarm_controller.h
 * @brief Alarm  controller related functions
 *****************************************************************************/

#pragma once

// INCLUDES ===================================================================

// Externals

// Internals

#include "alarm.h"

#define ALARMS_SIZE 12

#define RCM_SW_1 12u
#define RCM_SW_2 11u
#define RCM_SW_3 14u
#define RCM_SW_6 15u
#define RCM_SW_8 18u
#define RCM_SW_11 21u
#define RCM_SW_12 13u
#define RCM_SW_14 22u
#define RCM_SW_15 23u
#define RCM_SW_16 31u
#define RCM_SW_18 17u
#define RCM_SW_19 24u

class AlarmController {
 public:
    /// Default constructor
    AlarmController();

    /// Snooze alarm for 2 minutes. There is no more buzzer during the alarm
    void snooze();

    /**
     * Detect a specific alarm.
     * @param p_alarmCode The alarm code of the alarm
     * @param p_cycleNumber The cycle number since the device startup
     */
    void detectedAlarm(uint8_t p_alarmCode, uint32_t p_cycleNumber);

    /**
     * Undetect a specific alarm.
     * @param p_alarmCode The alarm code of the alarm
     */
    void notDetectedAlarm(uint8_t p_alarmCode);

    /**
     * Manage alarms and do whatever is needed as buzzer / lcd / led
     * @param p_centiSec Centile in the respiratory cycle
     */
    void manageAlarm(uint16_t p_centiSec);

 private:
    /// Collections of all available alarms
    Alarm m_alarms[ALARMS_SIZE];

    AlarmPriority m_highestPriority;

    AlarmPriority* m_snoozed;
};
