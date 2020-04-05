/*
    Copyright (C) 2020 Makers For Life
*/
/******************************************************************************
 * @author Makers For Life
 * @file pressure_valve.h
 * @brief Tools to control an Pressure Valve's servomotor
 *****************************************************************************/

#pragma once

// INCLUDES ===================================================================

// Internal libraries
#include "parameters.h"

// MACROS =================================================================

/// Convert an angle in degrees to a value in microseconds for the servomotor controller
#define Angle2MicroSeconds(value) map(value, 0, 180, 1000, 2000)

// CLASS =================================================================

/// Controls an Pressure Valve's servomotor
class PressureValve {
 public:
    /// Default constructor
    PressureValve();

    /**
     * Parameterized constructor
     *
     * @param p_minApertureAngle  Minimum servomotor aperture angle in degrees
     * @param p_maxApertureAngle  Maximum servomotor aperture angle in degrees
     * @param hardwareTimer       Hardware time for this servomotor
     * @param p_timerChannel        TIM channel for this servomotor
     * @param p_servoPin          Data pin for this servomotor
     */
    PressureValve(uint16_t p_minApertureAngle,
                  uint16_t p_maxApertureAngle,
                  HardwareTimer* hardwareTimer,
                  uint16_t p_timerChannel,
                  uint16_t p_servoPin);

    /**
     * Initialize this servomotor
     *
     * This must be called once to be able to use this Pressure Valve
     */
    void setup();

    /// Request opening of the Pressure Valve
    void open();

    /// Request half-opening of the Pressure Valve
    void halfOpen();

    /// Request closing of the Pressure Valve
    void close();

    /// Request reducing the opening of the Pressure Valve
    void decrease();

    /// Request increasing the opening of the Pressure Valve
    void increase();

    /**
     * Command the servomotor to go to the requested aperture
     *
     * @note Nothing will happen if this function is not called after requesting a new aperture
     */
    inline void execute() {
        // On évite d'aller plus loin que les limites de la valve
        if (command < minApertureAngle) {
            command = minApertureAngle;
        } else if (command > maxApertureAngle) {
            command = maxApertureAngle;
        }

        if (command != position) {
            actuator->setCaptureCompare(timerChannel, Angle2MicroSeconds(command),
                                        MICROSEC_COMPARE_FORMAT);
            position = command;
        }
    }

    /// Value of the requested aperture
    uint16_t command;

    /// Current aperture
    uint16_t position;

 private:
    /// Minimum servomotor aperture angle in degrees
    uint16_t minApertureAngle;

    /// Maximum servomotor aperture angle in degrees
    uint16_t maxApertureAngle;

    /// TIM channel for this servomotor
    uint16_t timerChannel;

    /// Data pin for this servomotor
    uint16_t servoPin;

    /// Hardware time for this servomotor
    HardwareTimer* actuator;
};
