/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file activation.cpp
 * @brief Breathing activation related functions
 *****************************************************************************/

// INCLUDES ===================================================================

// Associated header
#include "../includes/activation.h"

// Internal libraries
#include "../includes/buzzer.h"

/// Maximum delay in seconds between two pushes on the stop button to actually stop the machine
const uint32_t SECOND_STOP_MAX_DELAY_MS = 5000;

// INITIALISATION =============================================================

ActivationController activationController;

// FUNCTIONS ==================================================================

ActivationController::ActivationController() : m_state(STOPPED), m_timeOfLastStopPushed(0) {}

void ActivationController::onStartButton() { m_state = RUNNING; }

void ActivationController::onStopButton() {
    if ((m_state == RUNNING_READY_TO_STOP)
        && ((millis() - m_timeOfLastStopPushed) < SECOND_STOP_MAX_DELAY_MS)) {
        m_state = STOPPED;
    } else if ((m_state == RUNNING_READY_TO_STOP) || (m_state == RUNNING)) {
        m_timeOfLastStopPushed = millis();
        m_state = RUNNING_READY_TO_STOP;
    } else {
        // Stay in STOPPED state
    }
}

void ActivationController::refreshState() {
    // If the 2nd STOP deadline is exceeded, switch back
    // to running state. This is needed to make sure millis() counter
    // overflow does not make the test invalid
    if ((m_state == RUNNING_READY_TO_STOP)
        && ((millis() - m_timeOfLastStopPushed) >= SECOND_STOP_MAX_DELAY_MS)) {
        m_state = RUNNING;
    }
}
