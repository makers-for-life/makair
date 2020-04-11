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
    switch (m_state) {
    case STOPPED:
        break;

    case RUNNING_READY_TO_STOP:
        if (millis() - m_timeOfLastStopPushed < SECOND_STOP_MAX_DELAY_MS) {
            m_state = STOPPED;
            break;
        }
        // No break here. Fallback to default ON state handling

    case RUNNING:
        m_timeOfLastStopPushed = millis();
        m_state = RUNNING_READY_TO_STOP;

        Buzzer_Boot_Start();
        break;
    }
}
