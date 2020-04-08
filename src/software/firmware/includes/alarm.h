/*
    Copyright (C) 2020 Makers For Life
*/
/******************************************************************************
 * @author Makers For Life
 * @file alarm.h
 * @brief Alarm structure
 *****************************************************************************/

#pragma once

// INCLUDES =====================================================================

// Externals

#include <stdint.h>

// Internals

// ENUMS =====================================================================

enum AlarmPriority { ALARM_NONE, ALARM_LOW, ALARM_MEDIUM, ALARM_HIGH };

// CLASS =====================================================================

class Alarm {
 public:
    /**
     *
     * @param p_priority Alarm priority
     * @param p_code Alarm code
     * @param p_detectionThreshold Alarm detection threshold
     *
     */
    Alarm(AlarmPriority p_priority, uint8_t p_code, uint8_t p_detectionThreshold);

    /**
     * Get the alarm priority
     */
    AlarmPriority getPriority();

    /**
     * Get the alarm code
     */
    uint8_t getCode();

    /**
     * True if the number of detection is equals or above the detection threshold, false otherwise
     */
    bool isTriggered();

    /**
     * If the alarm is detected, it increments the number of detection until the detection
     * threshold.
     *
     * @param p_cycleNumber The cycle where the detection is done
     */
    void detected(uint32_t p_cycleNumber);

    /**
     * Reset alarm.
     * Reset to zero the number of detection.
     */
    void notDetected();

 private:
    /// Alarm priority
    AlarmPriority m_priority;

    /// Alarm code
    uint8_t m_code;

    /// Alarm detection threshold
    uint8_t m_detectionThreshold;

    /// Number of detection
    uint8_t m_detectionNumber;

    /// Cycle number
    uint32_t m_cycleNumber;
};

// INITIALISATION =============================================================
