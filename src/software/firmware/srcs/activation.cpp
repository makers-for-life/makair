/*
    Copyright (C) 2020 Makers For Life
*/
/******************************************************************************
 * @author Makers For Life
 * @file activation.cpp
 * @brief Breathing activation related functions
 *****************************************************************************/

// INCLUDES ===================================================================

// Associated header
#include "../includes/activation.h"

// External libraries
#include <Arduino.h>

// Internal
#include "../includes/buzzer.h"

const uint32_t SECOND_STOP_MAX_DELAY_MS = 5000;

// INITIALISATION =============================================================

ActivationController activationController;

// FUNCTIONS ==================================================================

ActivationController::ActivationController()
  : m_state(RUNNING),
    m_timeOfLastStopPushed(0),
    m_centiSecWhileStopped(0) {
}

void ActivationController::onStartButton() {
    m_state = RUNNING;
}

void ActivationController::onStopButton() {
    switch (m_state) {
        case STOPPED:
            break;

        case RUNNING_READY_TO_STOP:
            if (millis() - m_timeOfLastStopPushed < SECOND_STOP_MAX_DELAY_MS) {
                m_state = STOPPED;
                m_centiSecWhileStopped = 0;
                break;
            }
            // else fallback to default ON state handling

        case RUNNING:
            m_timeOfLastStopPushed = millis();
            m_state = RUNNING_READY_TO_STOP;

            // TODO check what to do in case of running alarm
            Buzzer_Boot_Start();
            break;
    }
}

void ActivationController::tickWhileStopped() {
    m_centiSecWhileStopped += 1;
}

