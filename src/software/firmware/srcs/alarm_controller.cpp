/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file alarm_controller.cpp
 * @brief Core logic to manage alarm features
 *****************************************************************************/

#pragma once

// INCLUDES ===================================================================

// Externals
#include <algorithm>

// Internals
#include "../includes/alarm_controller.h"
#include "../includes/buzzer.h"
#include "../includes/cycle.h"
#include "../includes/screen.h"
#if HARDWARE_VERSION == 2
#include "../includes/telemetry.h"
#endif

// INITIALISATION =============================================================

AlarmController alarmController;

// FUNCTIONS ==================================================================

AlarmController::AlarmController()
    : m_highestPriority(AlarmPriority::ALARM_NONE),
      m_snoozeTime(0u),
      m_alarms({

          /**
           * RCM-SW-2
           * The device shall embed a high priority alarm 11 when the pressure is below < 2cmH2O
           * from the 3th cycle.
           */
          Alarm(AlarmPriority::ALARM_HIGH, RCM_SW_2, 3u),

          /* RCM-SW-1
           * The device shall embed a high priority alarm 12 when the plateau pressure is not
           * reached (absolute difference > 20% in absolute value) from the 3th respiratory cycle.
           */
          Alarm(AlarmPriority::ALARM_HIGH, RCM_SW_1, 3u),

          /**
           * RCM-SW-12
           * The device shall monitor the battery voltage and trig a high priority alarm 13 when
           * voltage is < 24V.
           */
          Alarm(AlarmPriority::ALARM_HIGH, RCM_SW_12, 1u),

          /**
           * RCM-SW-3
           * The device shall embed a high priority alarm 14 when the PEEP target is not reached
           * (absolute difference > 2cmH2O) from the 3th respiratory cycle.
           */
          Alarm(AlarmPriority::ALARM_HIGH, RCM_SW_3, 3u),

          /**
           * RCM-SW-6 - NOT IN THIS VERSION
           * The software shall monitor the motor temperature, and raise a high priority alarm 15 if
           * temperature is over 80°C.
           */
          //   Alarm(AlarmPriority::ALARM_HIGH, RCM_SW_6, 1u),

          /**
           * RCM-SW-18
           * The device shall embed a high priority alarm 17 when the peak pressure is > 80cmH2O.
           */
          Alarm(AlarmPriority::ALARM_HIGH, RCM_SW_18, 1u),

          /**
           * RCM-SW-8 - NOT IN THIS VERSION
           * The software shall detect pressure out-of-range value in case of pressure sensor
           * disconnection or shortcut (< 0.250V & > 3.1V) and a high Priority Alarm 18 shall be
           * triggered.
           */
          //   Alarm(AlarmPriority::ALARM_HIGH, RCM_SW_8, 1u),

          /**
           * RCM-SW-11
           * The device shall monitor the battery voltage and trigger a medium priority alarm 21
           * when voltage is < 24,6V.
           */
          Alarm(AlarmPriority::ALARM_MEDIUM, RCM_SW_11, 1u),

          /**
           * RCM-SW-14
           * The device shall embed a medium priority alarm 22 when the plateau pressure is not
           * reached (absolute difference > 20% in absolute value) until the 2nd respiratory
           * cycle.
           */
          Alarm(AlarmPriority::ALARM_MEDIUM, RCM_SW_14, 2u),

          /**
           * RCM-SW-15
           * The device shall embed a medium priority alarm 23 when the PEEP target is not reached
           * (absolute difference > 2cmH2O) until the 2nd respiratory cycle.
           */
          Alarm(AlarmPriority::ALARM_MEDIUM, RCM_SW_15, 2u),

          /**
           * RCM-SW-19
           * The device shall embed a medium priority alarm 24 when the pressure is < 2cmH2O at the
           * second cycle (patient disconnection).
           */
          Alarm(AlarmPriority::ALARM_MEDIUM, RCM_SW_19, 2u),

          /**
           * RCM-SW-16
           * The device shall embed an information (audible) signal 31 when the mains are
           * disconnected to alert the user (vOut < 26,5V).
           */
          Alarm(AlarmPriority::ALARM_LOW, RCM_SW_16, 1u)}),
      m_centile(0u),
      m_pressure(0u),
      m_phase(CyclePhases::INHALATION),
      m_subphase(CycleSubPhases::INSPIRATION),
      m_cycle_number(0u) {
    for (uint8_t i = 0; i < ALARMS_SIZE; i++) {
        m_snoozedAlarms[i] = false;
    }
    for (uint8_t i = 0; i < ALARMS_SIZE; i++) {
        m_triggeredAlarms[i] = 0u;
    }
}

void AlarmController::snooze() {
    if (m_snoozeTime == 0u) {
        m_snoozeTime = millis();
        for (uint8_t i = 0; i < ALARMS_SIZE; i++) {
            Alarm* current = &m_alarms[i];
            if (current->isTriggered()) {
                m_snoozedAlarms[i] = true;
            } else {
                m_snoozedAlarms[i] = false;
            }
        }

        Buzzer_Mute();
    }
}

void AlarmController::detectedAlarm(uint8_t p_alarmCode,
                                    uint32_t p_cycleNumber,
                                    uint32_t p_expected,
                                    uint32_t p_measured) {
#if HARDWARE_VERSION == 1
    (void)p_expected;
    (void)p_measured;
#endif

    for (uint8_t i = 0; i < ALARMS_SIZE; i++) {
        Alarm* current = &m_alarms[i];
        bool wasTriggered = current->isTriggered();
        if (current->getCode() == p_alarmCode) {
            current->detected(p_cycleNumber);

            if (current->isTriggered()) {
                for (uint8_t j = 0; j < ALARMS_SIZE; j++) {
                    if (m_triggeredAlarms[j] == p_alarmCode) {
                        break;
                    }
                    if (m_triggeredAlarms[j] == 0u) {
                        m_triggeredAlarms[j] = p_alarmCode;
                        break;
                    }
                }

#if HARDWARE_VERSION == 2
                if (!wasTriggered) {
                    sendAlarmTrap(m_centile, m_pressure, m_phase, m_subphase, m_cycle_number,
                                  current->getCode(), current->getPriority(), true, p_expected,
                                  p_measured, current->getCyclesSinceTrigger());
                }
#endif
            }
            break;
        }
    }
}

int compare(uint8_t a, uint8_t b) {
    int result = 0;

    if (a == b) {
        result = 0;
    } else if (a == 0u) {
        result = 1;
    } else if (b == 0u) {
        result = -1;
    } else if (a < b) {
        result = -1;
    } else {
        result = 1;
    }

    return result;
}

void AlarmController::notDetectedAlarm(uint8_t p_alarmCode) {
    for (uint8_t i = 0; i < ALARMS_SIZE; i++) {
        Alarm* current = &m_alarms[i];
        bool wasTriggered = current->isTriggered();
        if (current->getCode() == p_alarmCode) {
            current->notDetected();

            if (!current->isTriggered()) {
                for (uint8_t j = 0; j < ALARMS_SIZE; j++) {
                    if (m_triggeredAlarms[j] == p_alarmCode) {
                        m_triggeredAlarms[j] = 0u;
                        std::sort(m_triggeredAlarms, &m_triggeredAlarms[ALARMS_SIZE], compare);
                        break;
                    }
                }

#if HARDWARE_VERSION == 2
                if (wasTriggered) {
                    sendAlarmTrap(m_centile, m_pressure, m_phase, m_subphase, m_cycle_number,
                                  current->getCode(), current->getPriority(), false, 0u, 0u,
                                  current->getCyclesSinceTrigger());
                }
#endif
            }
            break;
        }
    }
}

void AlarmController::runAlarmEffects(uint16_t p_centiSec) {
    AlarmPriority highestPriority = AlarmPriority::ALARM_NONE;
    uint8_t triggeredAlarmCodes[ALARMS_SIZE];
    uint8_t numberOfTriggeredAlarms = 0;
    bool unsnooze = false;

    for (uint8_t i = 0; i < ALARMS_SIZE; i++) {
        Alarm* current = &m_alarms[i];
        if (current->isTriggered()) {
            if (numberOfTriggeredAlarms == 0u) {
                highestPriority = current->getPriority();
            }

            triggeredAlarmCodes[numberOfTriggeredAlarms] = current->getCode();
            numberOfTriggeredAlarms++;

            if ((m_snoozeTime > 0u) && !m_snoozedAlarms[i]) {
                unsnooze = true;
            }
        }
    }

    int32_t millisSinceSnooze = millis() - m_snoozeTime;
    if (!unsnooze && (m_snoozeTime > 0u) && (millisSinceSnooze >= 120000)) {
        unsnooze = true;
    }

    if ((p_centiSec % LCD_UPDATE_PERIOD) == 0u) {
        displayAlarmInformation(triggeredAlarmCodes, numberOfTriggeredAlarms);
    }

    if (highestPriority == AlarmPriority::ALARM_HIGH) {
        if ((m_highestPriority != highestPriority) || unsnooze) {
            Buzzer_High_Prio_Start();
        }

        if ((p_centiSec % 100u) == 50u) {
            digitalWrite(PIN_LED_RED, LED_RED_ACTIVE);
        } else if ((p_centiSec % 100u) == 0u) {
            digitalWrite(PIN_LED_RED, LED_RED_INACTIVE);
        } else {
        }
        digitalWrite(PIN_LED_YELLOW, LED_YELLOW_INACTIVE);
    } else if (highestPriority == AlarmPriority::ALARM_MEDIUM) {
        if ((m_highestPriority != highestPriority) || unsnooze) {
            Buzzer_Medium_Prio_Start();
        }
        digitalWrite(PIN_LED_RED, LED_RED_INACTIVE);
        if ((p_centiSec % 100u) == 50u) {
            digitalWrite(PIN_LED_YELLOW, LED_YELLOW_ACTIVE);
        } else if ((p_centiSec % 100u) == 0u) {
            digitalWrite(PIN_LED_YELLOW, LED_YELLOW_INACTIVE);
        } else {
        }
    } else if (highestPriority == AlarmPriority::ALARM_LOW) {
        if ((m_highestPriority != highestPriority) || unsnooze) {
            Buzzer_Low_Prio_Start();
        }

        digitalWrite(PIN_LED_RED, LED_RED_INACTIVE);
        digitalWrite(PIN_LED_YELLOW, LED_YELLOW_ACTIVE);
    } else {
        Buzzer_Stop();

        digitalWrite(PIN_LED_RED, LED_RED_INACTIVE);
        digitalWrite(PIN_LED_YELLOW, LED_YELLOW_INACTIVE);
    }

    if (unsnooze) {
        m_snoozeTime = 0u;
        for (uint8_t i = 0; i < ALARMS_SIZE; i++) {
            m_snoozedAlarms[i] = false;
        }
    }

    m_highestPriority = highestPriority;
}

// cppcheck-suppress unusedFunction
void AlarmController::updateCoreData(uint16_t p_centile,
                                     uint16_t p_pressure,
                                     CyclePhases p_phase,
                                     CycleSubPhases p_subphase,
                                     uint32_t p_cycle_number) {
    m_centile = p_centile;
    m_pressure = p_pressure;
    m_phase = p_phase;
    m_subphase = p_subphase;
    m_cycle_number = p_cycle_number;
}
