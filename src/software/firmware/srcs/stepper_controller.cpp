/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file blower.cpp
 * @brief Tools to control the blower
 *****************************************************************************/

#pragma once

#include "../includes/config.h"
#include "Arduino.h"
#include <HardwareSerial.h>

// INCLUDES ===================================================================
#if MODE == MODE_STEPPER_CONTROLLER

#define STEPPER_EN PA10
#define STEPPER_HALFFULL PB5
#define STEPPER_CONTROL PB4
#define STEPPER_CLOCK PB10
#define STEPPER_CWCCW PA8
#define STEPPER_RESET PA9
// VrefA = VrefB, connect them together on IHM05A1
#define STEPPER_VREF PC7

HardwareTimer* Timer_Hw_Vref;

int VrefPwmPercent = 10;

void setVref(uint32_t pwm_percent) {
    Timer_Hw_Vref->setCaptureCompare(2, pwm_percent * 1, MICROSEC_COMPARE_FORMAT);
}

void loop(void) {
    // read serial port for commands

    while (Serial.available()) {
        // get the new byte:
        char inChar = (char)Serial.read();

        switch (inChar) {
        case '+':
            if (VrefPwmPercent < 99) {
                VrefPwmPercent++;
            }
            setVref(VrefPwmPercent);
            Serial.print("increase current pwm to ");
            Serial.println(VrefPwmPercent);
            break;

        case '-':
            if (VrefPwmPercent > 1) {
                VrefPwmPercent--;
            }
            setVref(VrefPwmPercent);
            Serial.print("decrease current pwm to ");
            Serial.println(VrefPwmPercent);
            break;
            
        case 'e':
            digitalWrite(STEPPER_EN, !digitalRead(STEPPER_EN));
            Serial.print("toggle power enable ");
            Serial.println(digitalRead(STEPPER_EN) == LOW ? "OFF" : "ON");
            break;
        }
    }
}

void setup(void) {
    Serial.begin(115200);
    Serial.println("Stepper Controler with L6208PD");

    pinMode(STEPPER_EN, OUTPUT);
    pinMode(STEPPER_HALFFULL, OUTPUT);
    pinMode(STEPPER_CONTROL, OUTPUT);
    pinMode(STEPPER_CLOCK, OUTPUT);
    pinMode(STEPPER_CWCCW, OUTPUT);
    pinMode(STEPPER_RESET, OUTPUT);
    pinMode(STEPPER_VREF, OUTPUT);

    // digitalWrite(STEPPER_VREF, LOW);
    digitalWrite(STEPPER_RESET, LOW);
    digitalWrite(STEPPER_CONTROL, LOW);   // low = fast decay
    digitalWrite(STEPPER_HALFFULL, LOW);  // low = full step
    digitalWrite(STEPPER_CWCCW, LOW);
    digitalWrite(STEPPER_EN, LOW);  // low = disable power
    digitalWrite(STEPPER_RESET, HIGH);

    // create Vref with a pwm on pwm3 channel 2
    Timer_Hw_Vref = new HardwareTimer(TIM3);
    // CPU Clock down to 10 kHz
    Timer_Hw_Vref->setPrescaleFactor(10000);  // 10khz
    Timer_Hw_Vref->setMode(2, TIMER_OUTPUT_COMPARE_PWM1, STEPPER_VREF);

    Timer_Hw_Vref->setOverflow(100, MICROSEC_FORMAT);
    setVref(VrefPwmPercent);
    Timer_Hw_Vref->resume();
    Serial.println("Vref set to half");
}

#endif