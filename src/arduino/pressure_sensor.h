/*=============================================================================
 * @file pressure_sensor.h
 *
 * COVID Respirator
 *
 * @section copyright Copyright
 *
 * Makers For Life
 *
 * This is free and unencumbered software released into the public domain.
 * Anyone is free to copy, modify, publish, use, compile, sell, or distribute
 * this software, either in source code form or as a compiled binary,
 * for any purpose, commercial or non-commercial, and by any means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the software
 * to the public domain. We make this dedication for the benefit of the public
 * at large and to the detriment of our heirs and successors. We intend this
 * dedication to be an overt act of relinquishment in perpetuity of all present
 * and future rights to this software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to [http://unlicense.org]
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
