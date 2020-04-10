/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file alarm_controller.cpp
 * @brief Core logic to manage alarm features
 *****************************************************************************/

#pragma once

// INCLUDES ===================================================================

// Externals

// Internals
#include "../includes/alarm_controller.h"
#include "../includes/buzzer.h"
#include "../includes/screen.h"

// INITIALISATION =============================================================

AlarmController alarmController;

// FUNCTIONS ==================================================================

AlarmController::AlarmController()
    : m_highestPriority(AlarmPriority::ALARM_NONE),
      m_snoozed(AlarmPriority::ALARM_NONE),
      m_alarms({

          /**
           * RCM-SW-2
           * The device shall embed a high priority alarm 11 when the pressure is too low < 2cmH2O
           * from the 4th cycle.
           */
          Alarm(AlarmPriority::ALARM_HIGH, RCM_SW_2, 3u),

          /* RCM-SW-1
           * The device shall embed a high priority Alarm 12 when the pressure is > 35cmH20 from the
           * 4th respiratory cycle.
           */
          Alarm(AlarmPriority::ALARM_HIGH, RCM_SW_1, 3u),

          /**
           * RCM-SW-12
           * The device shall monitor the battery voltage and trig a High priority alarm 13 when
           * voltage is < 24V.
           */
          Alarm(AlarmPriority::ALARM_HIGH, RCM_SW_12, 1u),

          /**
           * RCM-SW-3
           * The device shall embed a high priority alarm 14 when the PEEP target is not reached
           * (absolute difference > 2cmH2O) from the 4th respiratory cycle.
           */
          Alarm(AlarmPriority::ALARM_HIGH, RCM_SW_3, 3u),

          /**
           * RCM-SW-6 - NOT IN THIS VERSION
           * The software shall monitor the motor temperature, and raise a high priority alarm
           15 if
           * temperature is over 80°C.
           */
          //   Alarm(AlarmPriority::ALARM_HIGH, RCM_SW_6, 1u),

          /**
           * RCM-SW-18
           * The device shall embed a high priority alarm 17 when the Plateau pressure is > 80cmH2O.
           */
          Alarm(AlarmPriority::ALARM_HIGH, RCM_SW_18, 1u),

          /**
           * RCM-SW-8 - NOT IN THIS VERSION
           * The software shall detect pressure out-of-range value in case of pressure sensor
           * disconnection or shortcut ( <0.250v & >3.1V) and a High Priority Alarm 18 shall be
           * triggered.
           */
          //   Alarm(AlarmPriority::ALARM_HIGH, RCM_SW_8, 1u),

          /**
           * RCM-SW-11
           * The device shall monitor the battery voltage and trig a medium priority alarm 21 when
           * voltage is < 24,6V .
           */
          Alarm(AlarmPriority::ALARM_MEDIUM, RCM_SW_11, 1u),

          /**
           * RCM-SW-14
           * The device shall embed a medium priority alarm 22 when the Plateau pressure is not
           * reached (absolute difference > 20% in absolute value) until the 2nd respiratory
           * cycle
           */
          Alarm(AlarmPriority::ALARM_MEDIUM, RCM_SW_14, 2u),

          /**
           * RCM-SW-15
           * The device shall embed a medium priority alarm 23 when the PEEP target is not reached
           * (absolute difference > 2cmH2O) until the 2nd respiratory cycle
           */
          Alarm(AlarmPriority::ALARM_MEDIUM, RCM_SW_15, 2u),

          /**
           * RCM-SW-19
           * The device shall embed a low priority alarm 24 when the Pressure < 2cm H2O at the
           * second cycle (Patient disconnection)
           */
          Alarm(AlarmPriority::ALARM_MEDIUM, RCM_SW_19, 2u),

          /**
           * RCM-SW-16
           * The device shall embed an information (audible) signal 31 when the mains are
           * disconnected to alert the user (vOut < 26,5V).
           */
          Alarm(AlarmPriority::ALARM_LOW, RCM_SW_16, 1u)}) {}

void AlarmController::snooze() { Buzzer_Mute(); }

void AlarmController::detectedAlarm(uint8_t p_alarmCode, uint32_t p_cycleNumber) {
    for (int i = 0; i < ALARMS_SIZE; i++) {
        Alarm* current = &m_alarms[i];
        if (current->getCode() == p_alarmCode) {
            current->detected(p_cycleNumber);
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

void AlarmController::runAlarmEffects(uint16_t p_centiSec) {
    AlarmPriority highestPriority = AlarmPriority::ALARM_NONE;
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

    if ((p_centiSec % LCD_UPDATE_PERIOD) == 0u) {
        displayAlarmInformation(triggeredAlarmCodes, numberOfTriggeredAlarms);
    }

    if (!highestPriority) {
        Buzzer_Stop();

        digitalWrite(PIN_LED_RED, LOW);
        digitalWrite(PIN_LED_YELLOW, LOW);
    } else if (highestPriority == AlarmPriority::ALARM_HIGH) {
        if (m_highestPriority != highestPriority) {
            Buzzer_High_Prio_Start();
        }

        if ((p_centiSec % 100) == 50) {
            digitalWrite(PIN_LED_RED, HIGH);
        } else if ((p_centiSec % 100) == 0) {
            digitalWrite(PIN_LED_RED, LOW);
        }
        digitalWrite(PIN_LED_YELLOW, LOW);
    } else if (highestPriority == AlarmPriority::ALARM_MEDIUM) {
        if (m_highestPriority != highestPriority) {
            Buzzer_Medium_Prio_Start();
        }
        digitalWrite(PIN_LED_RED, LOW);
        if ((p_centiSec % 100) == 50) {
            digitalWrite(PIN_LED_YELLOW, HIGH);
        } else if ((p_centiSec % 100) == 0) {
            digitalWrite(PIN_LED_YELLOW, LOW);
        }
    } else if (highestPriority == AlarmPriority::ALARM_LOW) {
        if (m_highestPriority != highestPriority) {
            Buzzer_Low_Prio_Start();
        }

        digitalWrite(PIN_LED_RED, LOW);
        digitalWrite(PIN_LED_YELLOW, HIGH);
    }

    m_highestPriority = highestPriority;
}
