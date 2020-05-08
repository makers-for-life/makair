/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file blower.h
 * @brief Tools to control the blower
 *****************************************************************************/

#pragma once

// INCLUDES ===================================================================

// Internal libraries
#include "../includes/debug.h"
#include "../includes/parameters.h"

// Convert a speed to a value in microseconds for the blower controller
// There is an error margin on the max ppm, because the blower do not handle values greater
// than 2.01 ms, and there is no quartz anywhere
#define BlowerSpeed2MicroSeconds(value) map(value, 0, 1800, 1000, 1950)

// CLASS =================================================================

/// Controls a blower
class Blower {
 public:
    Blower();

    /**
     * Parameterized constructor
     *
     * @param p_hardwareTimer       Hardware time for the blower
     * @param p_timerChannel        TIM channel for this servomotor
     * @param p_blowerPin            Data pin for this blower
     */
    Blower(HardwareTimer* p_hardwareTimer, uint16_t p_timerChannel, uint16_t p_blowerPin);

    void setup();

    /**
     * Run the Blower to a speed
     * @param p_speed  speed between MIN_BLOWER_SPEED and MAX_BLOWER_SPEED.
     */
    void runSpeed(uint16_t p_speed);

    /// Stops the blower
    void stop();

    /// Get speed value
    uint16_t getSpeed() const;

 private:
    HardwareTimer* actuator;
    uint16_t timerChannel;
    uint16_t blowerPin;
    uint16_t m_speed;
    bool m_stopped;
};
