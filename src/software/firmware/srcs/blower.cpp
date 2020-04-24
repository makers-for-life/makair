/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file blower.cpp
 * @brief Tools to control the blower
 *****************************************************************************/

#pragma once

// INCLUDES ===================================================================

// Associated header
#include "../includes/blower.h"

// Internal libraries
#include "../includes/parameters.h"

// FUNCTIONS ==================================================================

Blower::Blower() {}

Blower::Blower(HardwareTimer* p_hardwareTimer, uint16_t p_timerChannel, uint16_t p_blowerPin) {
    actuator = p_hardwareTimer;
    timerChannel = p_timerChannel;
    blowerPin = p_blowerPin;
    m_stopped = true;
    m_speed = DEFAULT_BLOWER_SPEED;
}

void Blower::setup() {
    actuator->setMode(timerChannel, TIMER_OUTPUT_COMPARE_PWM1, blowerPin);

    // Set PPM width to 1ms
    actuator->setCaptureCompare(timerChannel, BlowerSpeed2MicroSeconds(0), MICROSEC_COMPARE_FORMAT);
    actuator->resume();
}

void Blower::runSpeed(uint16_t p_speed) {
    if ((p_speed > MIN_BLOWER_SPEED) && (p_speed < MAX_BLOWER_SPEED)) {
        // do not forcefully set the capture compare again and again if speed do not change
        if (m_stopped || (m_speed != p_speed)) {
            actuator->setCaptureCompare(timerChannel, BlowerSpeed2MicroSeconds(p_speed),
                                        MICROSEC_COMPARE_FORMAT);
            m_speed = p_speed;
            m_stopped = false;
        }
    } else {
        DBG_DO(Serial.print("Blower value is wrong: "));
        DBG_DO(Serial.println(p_speed));

        // If the blower was stopped, the pressure controller might ask it to restart with an
        // out-of-bound speed, resulting in no start at all; hence we restart it here on its last
        // value
        if (m_stopped) {
            this->runSpeed(m_speed);
        }
    }
}

uint16_t Blower::getSpeed() const { return m_speed; }

void Blower::stop() {
    actuator->setCaptureCompare(timerChannel, BlowerSpeed2MicroSeconds(0), MICROSEC_COMPARE_FORMAT);
    m_stopped = true;
}
