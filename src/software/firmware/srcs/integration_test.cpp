/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file integration_test.cpp
 * @brief Entry point of integration test program
 *****************************************************************************/

#pragma once

#include "../includes/config.h"
#if MODE == MODE_INTEGRATION_TEST

// INCLUDES ===================================================================

// External
#include <Arduino.h>
#include <OneButton.h>

// Internal
#include "../includes/battery.h"
#include "../includes/blower.h"
#include "../includes/buzzer.h"
#include "../includes/buzzer_control.h"
#include "../includes/debug.h"
#include "../includes/parameters.h"
#include "../includes/pression.h"
#include "../includes/pressure_valve.h"
#include "../includes/screen.h"

// State machine states
#define STEP_WELCOME 0
#define STEP_BLOWER_TEST 1
#define STEP_VALVE_BLOWER_TEST 2
#define STEP_VALVE_PATIENT_TEST 3
#define STEP_VALVE_BLOWER_LEAK_TEST 4
#define STEP_VALVE_PATIENT_LEAK_TEST 5
#define STEP_PRESSURE_TEST 6
#define STEP_BATTERY_TEST 7
#define STEP_BUZZER_TEST 8

#define NUMBER_OF_STATES 9

#define UNGREEDY(is_drawn, statement)                                                              \
    if (is_drawn == 0) {                                                                           \
        statement;                                                                                 \
        is_drawn = 1;                                                                              \
    }

static uint8_t step = STEP_WELCOME;
static uint8_t is_drawn = false;

PressureValve servoBlower;
PressureValve servoPatient;
HardwareTimer* hardwareTimer1;
HardwareTimer* hardwareTimer3;
Blower* blower_pointer;
Blower blower;

int32_t last_time = millis();

void changeStep(uint8_t new_step) {
    step = new_step;
    is_drawn = 0;
}

//! This function displays 2 lines of 20 characters (or less)
void display(char line1[], char line2[]) {
    resetScreen();
    screen.setCursor(0, 0);
    screen.print(line1);
    screen.setCursor(0, 1);
    screen.print(line2);
}

//! This function displays only a message on line
void displayLine(char msg[], uint8_t line) {
    screen.setCursor(0, line);
    screen.print("                    ");
    screen.setCursor(0, line);
    screen.print(msg);
}

void onStartClick() {
    DBG_DO(Serial.print("Go to step: "));
    DBG_DO(Serial.println((step + 1) % NUMBER_OF_STATES));
    changeStep((step + 1) % NUMBER_OF_STATES);
    last_time = millis();
    Buzzer_Stop();
    blower.stop();
}

OneButton btn_start(PIN_BTN_START, false, false);

void setup() {
    DBG_DO(Serial.begin(115200);)
    DBG_DO(Serial.println("Booting the system in integration mode...");)

    btn_start.attachClick(onStartClick);

    startScreen();

    pinMode(PIN_PRESSURE_SENSOR, INPUT);
    pinMode(PIN_BATTERY, INPUT);
    pinMode(PIN_BUZZER, OUTPUT);


#if HARDWARE_VERSION == 1
    // Timer for servoBlower
    hardwareTimer1 = new HardwareTimer(TIM1);
    hardwareTimer1->setOverflow(SERVO_VALVE_PERIOD, MICROSEC_FORMAT);

    // Timer for servoPatient and escBlower
    hardwareTimer3 = new HardwareTimer(TIM3);
    hardwareTimer3->setOverflow(SERVO_VALVE_PERIOD, MICROSEC_FORMAT);

    // Servo blower setup
    servoBlower = PressureValve(hardwareTimer1, TIM_CHANNEL_SERVO_VALVE_BLOWER, PIN_SERVO_BLOWER,
                                VALVE_OPEN_STATE, VALVE_CLOSED_STATE);
    servoBlower.setup();
    hardwareTimer1->resume();

    // Servo patient setup
    servoPatient = PressureValve(hardwareTimer3, TIM_CHANNEL_SERVO_VALVE_PATIENT, PIN_SERVO_PATIENT,
                                 VALVE_OPEN_STATE, VALVE_CLOSED_STATE);
    servoPatient.setup();

    blower = Blower(hardwareTimer3, TIM_CHANNEL_ESC_BLOWER, PIN_ESC_BLOWER);
    blower.setup();
    blower_pointer = &blower;
#elif HARDWARE_VERSION == 2
    // Timer for servos
    hardwareTimer3 = new HardwareTimer(TIM3);
    hardwareTimer3->setOverflow(SERVO_VALVE_PERIOD, MICROSEC_FORMAT);

    // Servo blower setup
    servoBlower = PressureValve(hardwareTimer3, TIM_CHANNEL_SERVO_VALVE_BLOWER, PIN_SERVO_BLOWER,
                                VALVE_OPEN_STATE, VALVE_CLOSED_STATE);
    servoBlower.setup();
    hardwareTimer3->resume();

    // Servo patient setup
    servoPatient = PressureValve(hardwareTimer3, TIM_CHANNEL_SERVO_VALVE_PATIENT, PIN_SERVO_PATIENT,
                                 VALVE_OPEN_STATE, VALVE_CLOSED_STATE);
    servoPatient.setup();
    hardwareTimer3->resume();

    hardwareTimer1 = new HardwareTimer(TIM1);
    blower = Blower(hardwareTimer1, TIM_CHANNEL_ESC_BLOWER, PIN_ESC_BLOWER);
    blower.setup();
    blower_pointer = &blower;
#endif

    BuzzerControl_Init();
    Buzzer_Init();
    initBattery();

    blower.stop();
}

void loop() {
    btn_start.tick();
    blower.stop();

    switch (step) {
    case STEP_WELCOME: {
        UNGREEDY(is_drawn, {
            display("MakAir test", "Press start button");
            displayLine(VERSION, 3);
        });
        break;
    }
    case STEP_BLOWER_TEST: {
        UNGREEDY(is_drawn, display("Test blower", "Continuer : Start"));
        blower.runSpeed(150);
        break;
    }
    case STEP_VALVE_BLOWER_TEST: {
        UNGREEDY(is_drawn, display("Test Valve inspi", "Continuer : Start"));

        if (millis() - last_time < 5000) {
            servoBlower.open();
            servoBlower.execute();
            displayLine("Etat : Ouvert", 3);

        } else if (millis() - last_time < 10000) {
            servoBlower.close();
            servoBlower.execute();
            displayLine("Etat : Ferme", 3);

        } else {
            last_time = millis();
        }
        break;
    }
    case STEP_VALVE_PATIENT_TEST: {
        UNGREEDY(is_drawn, display("Test Valve expi", "Continuer : Start"));
        if (millis() - last_time < 5000) {
            servoPatient.open();
            servoPatient.execute();
            displayLine("Etat : Ouvert", 3);

        } else if (millis() - last_time < 10000) {
            servoPatient.close();
            servoPatient.execute();
            displayLine("Etat : Ferme", 3);
        } else {
            last_time = millis();
        }
        break;
    }
    case STEP_VALVE_BLOWER_LEAK_TEST: {
        UNGREEDY(is_drawn, display("Test fuite inspi", "Continuer : Start"));
        servoPatient.open();
        servoPatient.execute();
        servoBlower.close();
        servoBlower.execute();
        blower.runSpeed(150);
        break;
    }
    case STEP_VALVE_PATIENT_LEAK_TEST: {
        UNGREEDY(is_drawn, display("Test fuite expi", "Continuer : Start"));
        servoPatient.close();
        servoPatient.execute();
        servoBlower.open();
        servoBlower.execute();
        blower.runSpeed(150);
        break;
    }
    case STEP_PRESSURE_TEST: {
        servoPatient.close();
        servoPatient.execute();
        servoBlower.open();
        servoBlower.execute();
        blower.runSpeed(179);
        UNGREEDY(is_drawn, display("Test pression", "Continuer : Start"));
        if (millis() - last_time >= 200) {
            int pressure = readPressureSensor(0);
            char msg[SCREEN_LINE_LENGTH + 1];
            snprintf(msg, SCREEN_LINE_LENGTH + 1, "Pression : %3d mmH2O", pressure);
            displayLine(msg, 3);
            last_time = millis();
        }
        break;
    }

    case STEP_BATTERY_TEST: {
        UNGREEDY(is_drawn, display("Test batterie", "Continuer : Start"));
        updateBatterySample();
        if (millis() - last_time >= 200) {
            char msg[SCREEN_LINE_LENGTH + 1];
            snprintf(msg, SCREEN_LINE_LENGTH + 1, "Batterie : %3u V", getBatteryLevel());
            displayLine(msg, 3);
            last_time = millis();
        }
        break;
    }

    case STEP_BUZZER_TEST: {
        UNGREEDY(is_drawn, display("Test Buzzer", "Continuer : Start"));
        Buzzer_High_Prio_Start();
        break;
    }
    }

    delay(10);
}

#endif
