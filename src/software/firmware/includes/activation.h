/*
    Copyright (C) 2020 Makers For Life
*/
/******************************************************************************
 * @author Makers For Life
 * @file activation.h
 * @brief Breathing activation related functions
 *****************************************************************************/

#pragma once

// INCLUDES ===================================================================

// External libraries
#include <Arduino.h>

// CLASS ======================================================================

/// Controls breathing activation on/off state
class ActivationController {
 public:
    ActivationController();

    /**
     * Return if breathing is activated or not
     */
    bool isRunning() const { return m_state != STOPPED; }

    /**
     * Callback to call each time the start button is pushed
     */
    void onStartButton();

    /**
     * Callback to call each time the stop button is pushed
     */
    void onStopButton();

 private:
    enum State {
        STOPPED = 0,            // Breathing is off
        RUNNING,                // Breathing is on
        RUNNING_READY_TO_STOP,  // Breathing is on, waiting for a second push
    };

 private:
    /// Activation status
    State m_state;

    /// Last time stop button was pushed
    uint32_t m_timeOfLastStopPushed;
};

// INITIALISATION =============================================================

/// Instance of the activation controller
extern ActivationController activationController;
