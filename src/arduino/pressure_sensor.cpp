/*=============================================================================
 * @file pressure_sensor.cpp
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
 * This file implements the functions related to the pressure sensor
 */

// INCLUDES ===================================================================

// Associated header
#include "pressure_sensor.h"

// Internal
#include "common.h"
#include "parameters.h"

// INITIALISATION =============================================================

PressureSensor pSensor;

// FUNCTIONS ==================================================================

PressureSensor::PressureSensor()
    : m_pin(PIN_CAPTEUR_PRESSION),
      m_dividingBridgeRatio(RATIO_PONT_DIVISEUR),
      m_tensionSupply(V_SUPPLY),
      m_rawTension(0.),
      m_filteredTension(0.),
      m_finalTension(0.),
      m_pressure(0.)
{
}

PressureSensor::PressureSensor(int p_pin, double p_dividingBridgeRatio, double p_tensionSupply)
    : m_pin(p_pin),
      m_dividingBridgeRatio(p_dividingBridgeRatio),
      m_tensionSupply(p_tensionSupply),
      m_rawTension(0.),
      m_filteredTension(0.),
      m_finalTension(0.),
      m_pressure(0.)
{
}

int PressureSensor::read()
{
    // Read the raw sensor tension
    m_rawTension = analogRead(PIN_CAPTEUR_PRESSION) * 3.3 / 1024.0;

    // Smooth the tension using a low-pass filter
    m_filteredTension = m_filteredTension + (m_rawTension - m_filteredTension) * 0.2;

    // Apply the dividing bridge ratio
    m_finalTension = m_filteredTension / m_dividingBridgeRatio;

    // Compute the pressure in kPa
    m_pressure = (m_finalTension / m_tensionSupply - 0.04) / 0.09;

    // Derive the pressure in mmH2O
    m_pressure *= KPA_MMH2O;

    Serial.println(m_pressure * KPA_MMH2O);

    return int(m_pressure);
}
