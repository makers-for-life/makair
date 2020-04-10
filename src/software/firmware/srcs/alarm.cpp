/*
    Copyright (C) 2020 Makers For Life
*/
/******************************************************************************
 * @author Makers For Life
 * @file alarm.cpp
 * @brief Describes an alarm and handle its dynamic state
 *****************************************************************************/

#pragma once

// INCLUDES ===================================================================

// Externals
#include <Arduino.h>

// Internals
#include "../includes/alarm.h"

// FUNCTIONS ==================================================================

Alarm::Alarm(AlarmPriority p_priority, uint8_t p_code, uint8_t p_detectionThreshold) {
    m_priority = p_priority;
    m_code = p_code;
    m_detectionThreshold = p_detectionThreshold;
    m_cycleNumber = 0u;
    m_detectionNumber = 0u;
}

AlarmPriority Alarm::getPriority() { return m_priority; }

uint8_t Alarm::getCode() { return m_code; }

bool Alarm::isTriggered() { return m_detectionNumber >= m_detectionThreshold; }

void Alarm::detected(uint32_t p_cycleNumber) {
    if (m_cycleNumber != p_cycleNumber) {
        m_cycleNumber = p_cycleNumber;
        m_detectionNumber++;
        if (m_detectionNumber > m_detectionThreshold) {
            m_detectionNumber = m_detectionThreshold;
        }
    }
}

void Alarm::notDetected() {
    m_cycleNumber = 0;
    m_detectionNumber = 0;
}
