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
#include "../includes/buzzer.h"
#include "../includes/screen.h"

// FUNCTIONS ==================================================================

AlarmController::AlarmController()
    : m_snoozed(nullptr),
      m_alarms({

          /**
           * RCM-SW-2
           * The device shall embed a high priority alarm 11 when the pressure is too low < 2cmH2O
           * from the 4th cycle.
           */
          Alarm(AlarmPriority::ALARM_HIGH, 11u, 4u),

          /* RCM-SW-1
           * The device shall embed a high priority Alarm 12 when the pressure is > 35cmH20 from the
           * 4th respiratory cycle.
           */
          Alarm(AlarmPriority::ALARM_HIGH, 12u, 4u),

          /**
           * RCM-SW-12
           * The device shall monitor the battery voltage and trig a High priority alarm 13 when
           * voltage is at 12.0V.
           */
          Alarm(AlarmPriority::ALARM_HIGH, 13u, 1u),

          /**
           * RCM-SW-3
           * The device shall embed a high priority alarm 14 when the PEEP target is not reached
           * (absolute difference > 2cmH2O) from the 4th respiratory cycle.
           */
          Alarm(AlarmPriority::ALARM_HIGH, 14u, 4u),

          /**
           * RCM-SW-6 - NOT in #107
           * The software shall monitor the motor temperature, and raise a high priority alarm 15 if
           * temperature is over 80C.
           */
          Alarm(AlarmPriority::ALARM_HIGH, 15u, 1u),

          /**
           * RCM-SW-18
           * The device shall embed a high priority alarm 17 when the Plateau pressure is > 80cmH2O.
           */
          Alarm(AlarmPriority::ALARM_HIGH, 17u, 1u),

          /**
           * RCM-SW-8
           * The software shall detect pressure out-of-range value in case of pressure sensor
           * disconnection or shortcut ( <0.250v & >3.1V) and a High Priority Alarm 18 shall be
           * triggered.
           */
          Alarm(AlarmPriority::ALARM_HIGH, 18u, 1u),

          /**
           * RCM-SW-11
           * The device shall monitor the battery voltage and trig a medium priority alarm 21 when
           * voltage is at 12.3V.
           */

          Alarm(AlarmPriority::ALARM_MEDIUM, 21u, 1u),

          /**
           * RCM-SW-14
           * The device shall embed a medium priority alarm 22 when the Plateau pressure is not
           * reached (absolute différence > 20% in absolute value) until the 2nd respiratory cycle.
           */
          Alarm(AlarmPriority::ALARM_MEDIUM, 22u, 2u),

          /**
           * RCM-SW-15
           * The device shall embed a medium priority alarm 23 when the PEEP target is not reached
           * (absolute difference > 2cmH2O) until the 3th respiratory cycle.
           */
          Alarm(AlarmPriority::ALARM_MEDIUM, 23u, 3u),

          /**
           * RCM-SW-19
           * The device shall embed a low priority alarm 24 when the Plateau pressure < 2cm H2O at
           * the second cycle (Patient disconnection).
           */
          Alarm(AlarmPriority::ALARM_LOW, 24u, 2u),

          /**
           * RCM-SW-16
           * The device shall embed an information (audible) signal 31 when the mains are
           * disconnected to alert the user.
           */
          Alarm(AlarmPriority::ALARM_LOW, 31u, 1u)}) {}

void AlarmController::snooze() {
    // TODO disable buzzer sound for 120s
}

void AlarmController::detectedAlarm(uint8_t p_alarmCode) {
    for (int i = 0; i < ALARMS_SIZE; i++) {
        Alarm* current = &m_alarms[i];
        if (current->getCode() == p_alarmCode) {
            current->detected();
            break;
        }
    }
}

void AlarmController::notDetectedAlarm(uint8_t p_alarmCode) {
    for (int i = 0; i < ALARMS_SIZE; i++) {
        Alarm* current = &m_alarms[i];
        if (current->getCode() == p_alarmCode) {
            current->notDetected();
            break;
        }
    }
}

void AlarmController::manageAlarm() {
    AlarmPriority highestPriority;
    uint8_t triggeredAlarmCodes[ALARMS_SIZE];
    uint8_t numberOfTriggeredAlarms = 0;

    for (int i = 0; i < ALARMS_SIZE; i++) {
        Alarm* current = &m_alarms[i];
        if (current->isTriggered()) {
            if (numberOfTriggeredAlarms == 0) {
                highestPriority = current->getPriority();
            }

            triggeredAlarmCodes[numberOfTriggeredAlarms] = current->getCode();
            numberOfTriggeredAlarms++;
        }
    }
    displayAlarmInformation(triggeredAlarmCodes, numberOfTriggeredAlarms);

    if (!highestPriority) {
        Serial.println("buzzer off");
        Buzzer_Stop();
    } else if (highestPriority == AlarmPriority::ALARM_HIGH
               && m_highestPriority != highestPriority) {
        Buzzer_Long_Start();
    } else if (highestPriority == AlarmPriority::ALARM_MEDIUM
               && m_highestPriority != highestPriority) {
        Buzzer_Medium_Start();
    } else if (highestPriority == AlarmPriority::ALARM_LOW
               && m_highestPriority != highestPriority) {
        Buzzer_Boot_Start();
    }

    m_highestPriority = highestPriority;

    // TODO launch corresponding buzzer
    // Display alarm code on LCD screen
    // Blink the corresponding LED
}
