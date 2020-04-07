/*
    Copyright (C) 2020 Makers For Life
*/
/******************************************************************************
 * @author Makers For Life
 * @file alarm_controller.cpp
 * @brief Core logic to control alarms
 *****************************************************************************/

#pragma once

// INCLUDES ===================================================================

// Externals

// Internals

#include "../includes/alarm_controller.h"

// FUNCTIONS ==================================================================

AlarmController::AlarmController()
    : m_alarms({

        /**
         * RCM-SW-2
         * The device shall embed a high priority alarm 11 when the pressure is too low < 2cmH2O
         * from the 4th cycle.
         */
        Alarm(AlarmPriority::HIGH, 11, 4),

        /* RCM-SW-1
         * The device shall embed a high priority Alarm 12 when the pressure is > 35cmH20 from the
         * 4th respiratory cycle.
         */
        Alarm(AlarmPriority::HIGH, 12, 4),

        /**
         * RCM-SW-12
         * The device shall monitor the battery voltage and trig a High priority alarm 13 when
         * voltage is at 12.0V.
         */
        Alarm(AlarmPriority::HIGH, 13, 1),

        /**
         * RCM-SW-3
         * The device shall embed a high priority alarm 14 when the PEEP target is not reached
         * (absolute difference > 2cmH2O) from the 4th respiratory cycle.
         */
        Alarm(AlarmPriority::HIGH, 14, 4),

        /**
         * RCM-SW-6 - NOT in #107
         * The software shall monitor the motor temperature, and raise a high priority alarm 15 if
         * temperature is over 80C.
         */
        Alarm(AlarmPriority::HIGH, 15, 1),

        /**
         * RCM-SW-18
         * The device shall embed a high priority alarm 17 when the Plateau pressure is > 80cmH2O.
         */
        Alarm(AlarmPriority::HIGH, 17, 1),

        /**
         * RCM-SW-8
         * The software shall detect pressure out-of-range value in case of pressure sensor
         * disconnection or shortcut ( <0.250v & >3.1V) and a High Priority Alarm 18 shall be
         * triggered.
         */
        Alarm(AlarmPriority::HIGH, 18, 1),

        /**
         * RCM-SW-11
         * The device shall monitor the battery voltage and trig a medium priority alarm 21 when
         * voltage is at 12.3V.
         */

        Alarm(AlarmPriority::MEDIUM, 21, 1),

        /**
         * RCM-SW-14
         * The device shall embed a medium priority alarm 22 when the Plateau pressure is not
         * reached (absolute différence > 20% in absolute value) until the 2nd respiratory cycle.
         */
        Alarm(AlarmPriority::MEDIUM, 22, 2),

        /**
         * RCM-SW-15
         * The device shall embed a medium priority alarm 23 when the PEEP target is not reached
         * (absolute difference > 2cmH2O) until the 3th respiratory cycle.
         */
        Alarm(AlarmPriority::MEDIUM, 23, 3),

        /**
         * RCM-SW-19
         * The device shall embed a low priority alarm 24 when the Plateau pressure < 2cm H2O at
         * the second cycle (Patient disconnection).
         */
        Alarm(AlarmPriority::LOW, 24, 2),

        /**
         * RCM-SW-16
         * The device shall embed an information (audible) signal 31 when the mains are
         * disconnected to alert the user.
         */
        Alarm(AlarmPriority::LOW, 31, 1)}) {}

void AlarmController::snooze() {
    // TODO disable buzzer sound for 120s
}

void AlarmController::detectedAlarm(uint8_t p_alarmCode) {
    for (int i = 0; i < ALARMS_SIZE; i++) {
        Alarm current = m_alarms[0];
        if (current.code() == p_alarmCode) {
            current.detected();
        }
    }
}

void AlarmController::notDetectedAlarm(uint8_t p_alarmCode) {
    for (int i = 0; i < ALARMS_SIZE; i++) {
        Alarm current = m_alarms[0];
        if (current.code() == p_alarmCode) {
            current.notDetected();
        }
    }
}

void AlarmController::manageAlarm() {
    // TODO launch corresponding buzzer
    // Display alarm code on LCD screen
    // Blink the corresponding LED
}