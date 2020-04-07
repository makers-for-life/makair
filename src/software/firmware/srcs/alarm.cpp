/*
    Copyright (C) 2020 Makers For Life
*/
/******************************************************************************
 * @author Makers For Life
 * @file alarm.cpp
 * @brief Alarm structure
 *****************************************************************************/

#pragma once

// INCLUDES ===================================================================

// Externals

// Internals
#include "../includes/alarm.h"

// FUNCTIONS ==================================================================

Alarm::Alarm(AlarmPriority p_priority, uint8_t p_code, uint8_t p_detectionThreshold) {
    m_priority = p_priority;
    m_code = p_code;
    m_detectionThreshold = p_detectionThreshold;
    m_detectionNumber = 0u;
}

AlarmPriority Alarm::getPriority() { return m_priority; }

uint8_t Alarm::getCode() { return m_code; }

bool Alarm::isTriggered() { return m_detectionNumber >= m_detectionThreshold; }

void Alarm::detected() {
    m_detectionNumber = m_detectionNumber + 1;
    if (m_detectionNumber > m_detectionThreshold) {
        m_detectionNumber = m_detectionThreshold;
    }
}

void Alarm::notDetected() { m_detectionNumber = 0; }
