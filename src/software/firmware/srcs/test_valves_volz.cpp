/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file qualification.cpp
 * @brief Entry point of electrical wiring qualification program
 *****************************************************************************/

#pragma once

#include "../includes/config.h"
#if MODE == MODE_VALVE_TEST_VOLZ

// External
#include "../includes/config.h"
#include <Arduino.h>
#include <OneButton.h>
#include <Wire.h>

// Internal
#include "../includes/parameters.h"
#include "../includes/screen.h"

HardwareTimer* testTimer;
HardwareTimer* timer1;
HardwareTimer* timer3;

static OneButton buttonAlarmOff(PIN_BTN_ALARM_OFF, false, false);
static OneButton buttonStart(PIN_BTN_START, false, false);
static OneButton buttonStop(PIN_BTN_STOP, false, false);

int periodValve = 500;

// 10 ms task
int count = 0;
boolean ValveOpened = true;

uint32_t cycleCount = 0;
int currentPPM = 0;

#define PPM_OPENED 1000
#define PPM_CLOSED 2000

enum statemachine { CYCLING, FORCEOPENED, FORCECLOSED, MANUAL };
enum statemachine state = CYCLING;

void setPPM(int duty);


void Test_Timer_Callback(HardwareTimer*) {
    buttonAlarmOff.tick();
    buttonStart.tick();
    buttonStop.tick();

    if (state == CYCLING) {
        count++;
        if (count > periodValve / 10) {
            ValveOpened = !ValveOpened;

            count = 0;
            if (ValveOpened) {
                cycleCount++;
            }
            currentPPM = ValveOpened ? PPM_OPENED : PPM_CLOSED;
            setPPM(currentPPM);
        }
    }
    if (state == FORCEOPENED) {
        ValveOpened = true;
        currentPPM = PPM_OPENED;
        setPPM(currentPPM);
    }
    if (state == FORCECLOSED) {
        ValveOpened = false;
        currentPPM = PPM_CLOSED;
        setPPM(currentPPM);
    }
    digitalWrite(PIN_LED_START, !ValveOpened);  // active low
}

void onStopClick() {
    if (state == CYCLING) {
        if (periodValve < 5000) {
            periodValve += 50;
        }
    }
    if (state == MANUAL) {
        if (currentPPM < 2200) {
            currentPPM+=10;
            setPPM(currentPPM);
        }
    }
}
void onStartClick() {
    if (state == CYCLING) {
        if (periodValve > 0) {
            periodValve -= 50;
        }
    }
    if (state == MANUAL) {
        if (currentPPM > 800) {
            currentPPM-=10;
            setPPM(currentPPM);
        }
    }
}
void onAlarmOffClick() {
    if (state == CYCLING) {
        state = FORCEOPENED;
    } else if (state == FORCEOPENED) {
        state = FORCECLOSED;
    } else if (state == FORCECLOSED) {
        state = MANUAL;
        setPPM(currentPPM);
    } else if (state == MANUAL) {
        state = CYCLING;
    }
}

void setPPM(int ppm) {

    timer3->setCaptureCompare(TIM_CHANNEL_SERVO_VALVE_BLOWER, ppm, MICROSEC_COMPARE_FORMAT);
    timer3->setCaptureCompare(TIM_CHANNEL_SERVO_VALVE_PATIENT, ppm, MICROSEC_COMPARE_FORMAT);
    timer1->setCaptureCompare(TIM_CHANNEL_ESC_BLOWER, ppm, MICROSEC_COMPARE_FORMAT);
}

void setup(void) {

    Serial.begin(115200);

    pinMode(PIN_SERIAL_TX, OUTPUT);
    pinMode(PIN_LED_START, OUTPUT);

    startScreen();
    resetScreen();
    screen.setCursor(0, 0);
    screen.print("Test FaulHaber");

    buttonAlarmOff.attachClick(onAlarmOffClick);
    buttonStart.attachClick(onStartClick);
    buttonStop.attachClick(onStopClick);
    buttonStart.attachDuringLongPress(onStartClick);
    buttonStop.attachDuringLongPress(onStopClick);

    // set the timer
    testTimer = new HardwareTimer(TIM10);

    // prescaler. stm32f411 clock is 100mhz
    testTimer->setPrescaleFactor((testTimer->getTimerClkFreq() / 10000) - 1);

    // set the period
    testTimer->setOverflow(100);  // 10 ms
    testTimer->setMode(1, TIMER_OUTPUT_COMPARE, NC);
    testTimer->attachInterrupt(Test_Timer_Callback);

    // interrupt priority is documented here:
    // https://stm32f4-discovery.net/2014/05/stm32f4-stm32f429-nvic-or-nested-vector-interrupt-controller/
    testTimer->setInterruptPriority(2, 0);

    testTimer->resume();

    timer1 = new HardwareTimer(TIM1);
    timer3 = new HardwareTimer(TIM3);

    pinMode(PIN_ESC_BLOWER, OUTPUT);
    pinMode(PIN_SERVO_PATIENT, OUTPUT);
    pinMode(PIN_SERVO_BLOWER, OUTPUT);
    digitalWrite(PIN_ESC_BLOWER, HIGH);
    digitalWrite(PIN_SERVO_PATIENT, HIGH);
    digitalWrite(PIN_SERVO_BLOWER, HIGH);

    timer1->setOverflow(20000, MICROSEC_FORMAT);
    timer3->setOverflow(20000, MICROSEC_FORMAT);

    timer3->setMode(TIM_CHANNEL_SERVO_VALVE_BLOWER, TIMER_OUTPUT_COMPARE_PWM1,
                    PIN_SERVO_BLOWER);  // tim1ch3
    timer3->setMode(TIM_CHANNEL_SERVO_VALVE_PATIENT, TIMER_OUTPUT_COMPARE_PWM1, PIN_SERVO_PATIENT);
    timer1->setMode(TIM_CHANNEL_ESC_BLOWER, TIMER_OUTPUT_COMPARE_PWM1, PIN_ESC_BLOWER);
    setPPM(50);
    timer1->resume();
    timer3->resume();
}

void loop(void) {

    delay(300);

    char buffer[30];

    resetScreen();
    screen.setCursor(0, 0);
    screen.print("Test Volz");
    screen.setCursor(0, 1);
    screen.print("half period=");
    screen.print(periodValve);
    screen.print("ms");

    if (state == CYCLING) {
        screen.setCursor(0, 2);
        screen.print("Cycling...");
        screen.print(cycleCount);
    }
    if (state == FORCECLOSED) {
        screen.setCursor(0, 2);
        screen.print("Paused ...");
        screen.setCursor(0, 3);
        screen.print("CLOSED");
    }
    if (state == FORCEOPENED) {
        screen.setCursor(0, 2);
        screen.print("Paused ...");
        screen.setCursor(0, 3);
        screen.print("OPENED");
    }
    if (state == MANUAL) {
        screen.setCursor(0, 2);
        screen.print("Manual ...");
        screen.setCursor(0, 3);
        screen.print("PPM :");
        screen.print(currentPPM);
        screen.print("ms");
    }
}

#endif
