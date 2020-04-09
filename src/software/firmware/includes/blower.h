/*
    Copyright (C) 2020 Makers For Life
*/
/******************************************************************************
 * @author Makers For Life
 * @file blower.h
 * @brief Tools to control a blower
 *****************************************************************************/

#pragma once

// INCLUDES ===================================================================

// Internal libraries
#include "debug.h"
#include "parameters.h"

#define MIN_BLOWER_SPEED 95
#define MAX_BLOWER_SPEED 180

// Convert a speed to a value in microseconds for the blower controller
#define BlowerSpeed2MicroSeconds(value) map(value, 0, 180, 1000, 2000)



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
    Blower(HardwareTimer* p_hardwareTimer,
                             uint16_t p_timerChannel,
                             uint16_t p_blowerPin);

    void setup();


    /**
     * Run the Blower to a speed 
     * @param p_speed  speed between min :0 and max :180.
     */
    void runSpeed(int16_t p_speed);

    // Stops the Blower
    void stop();

    // Get speed value

    int getSpeed();

 private:

    HardwareTimer* actuator;
    uint16_t timerChannel;
    uint16_t blowerPin;
    int16_t m_speed;




};
