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

class AlarmController {
 public:
    /// Default constructor
    AlarmController();

    /// Snooze alarm for 2 minutes. There is no more buzzer during the alarm
    void snooze();

    /**
     * Detect a specific alarm.
     * @param p_alarmCode The alarm code of the alarm
     */
    void detectedAlarm(uint8_t p_alarmCode);

    /**
     * Undetect a specific alarm.
     * @param p_alarmCode The alarm code of the alarm
     */
    void notDetectedAlarm(uint8_t p_alarmCode);

    /**
     * Manage alarms and do whatever is needed as buzzer / lcd / led
     */
    void manageAlarm();

 private:
    Alarm m_alarms[ALARMS_SIZE];
};

// INITIALISATION =============================================================

/// Instance of the alarm controller
extern AlarmController alarmController;
