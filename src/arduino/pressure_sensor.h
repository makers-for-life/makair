/*=============================================================================
 * @file pressure_sensor.h
 *
 * COVID Respirator
 *
 * @section copyright Copyright
 *
 * Makers For Life
 *
 * @section descr File description
 *
 * This file defines the functions related to the pressure sensor
 */

#pragma once

class PressureSensor
{
public:
    //! Default constructor
    PressureSensor();

    //! Parameterized constructor
    PressureSensor(int p_pin, double p_dividingBridgeRatio, double p_tensionSupply);

    //! This function reads the pressure sensor in [kPa] and returns it in [mmH2O]
    int read();

private:
    /// PIN
    int m_pin;

    /// Dividing bridge ratio
    double m_dividingBridgeRatio;

    /// Tension supply [V]
    double m_tensionSupply;

    /// Raw pressure sensor tension [V]
    double m_rawTension;

    /// Smoothed pressure sensor tension [V]
    double m_filteredTension;

    /// Final tension following the dividing bridge scaling [V]
    double m_finalTension;

    /// Measured pressure [mmH2O]
    double m_pressure;
};

// INITIALISATION =============================================================

extern PressureSensor pSensor;
