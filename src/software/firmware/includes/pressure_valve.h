/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file pressure_valve.h
 * @brief Tools to control an Pressure Valve's servomotor
 *****************************************************************************/

#pragma once

// INCLUDES ===================================================================

// Internal libraries
#include "../includes/parameters.h"
#include "../includes/config.h"

// MACROS =================================================================

/// Convert an angle in degrees to a value in microseconds for the servomotor controller
uint16_t ValveAngle2MicroSeconds(uint16_t value);

// CLASS =================================================================

/// Controls an Pressure Valve's servomotor
class PressureValve {
 public:
    /// Default constructor
    PressureValve();

    /**
     * Parameterized constructor
     *
     * @param p_hardwareTimer       Hardware time for this servomotor
     * @param p_timerChannel        TIM channel for this servomotor
     * @param p_servoPin            Data pin for this servomotor
     * @param p_openApertureAngle   Open aperture angle in degrees
     * @param p_closeApertureAngle  Close aperture angle in degrees
     */
    PressureValve(HardwareTimer* p_hardwareTimer,
                  uint16_t p_timerChannel,
                  uint16_t p_servoPin,
                  uint16_t p_openApertureAngle,
                  uint16_t p_closeApertureAngle);
    /**
     * Initialize this servomotor
     *
     * This must be called once to be able to use this Pressure Valve
     */
    void setup();

    /// Request opening of the Pressure Valve
    void open();

    /**
     * Request opening of the Air Transistor with a given angle
     *
     * @param p_command The angle in degree
     */
    void open(uint16_t p_command);

    /// Request closing of the Pressure Valve
    void close();

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
        } else {
        }

        if (command != position) {
            actuator->setCaptureCompare(timerChannel, ValveAngle2MicroSeconds(command),
                                        MICROSEC_COMPARE_FORMAT);
            position = command;
        }
    }

    inline uint16_t minAperture() const { return minApertureAngle; }

    inline uint16_t maxAperture() const { return maxApertureAngle; }

    /// Value of the requested aperture
    uint16_t command;

    /// Current aperture
    uint16_t position;

 private:
    /// Minimum servomotor aperture angle in degrees
    uint16_t minApertureAngle;

    /// Maximum servomotor aperture angle in degrees
    uint16_t maxApertureAngle;

    uint16_t openApertureAngle;

    uint16_t closeApertureAngle;

    /// Hardware time for this servomotor
    HardwareTimer* actuator;

    /// TIM channel for this servomotor
    uint16_t timerChannel;

    /// Data pin for this servomotor
    uint16_t servoPin;
};
