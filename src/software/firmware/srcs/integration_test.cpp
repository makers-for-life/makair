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
#include "Arduino.h"
#include <OneButton.h>

// Internal
#include "../includes/battery.h"
#include "../includes/blower.h"
#include "../includes/buzzer.h"
#include "../includes/buzzer_control.h"
#include "../includes/debug.h"
#include "../includes/parameters.h"
#include "../includes/pressure.h"
#include "../includes/pressure_valve.h"
#include "../includes/screen.h"

// State machine states
#define STEP_WELCOME 0
#define STEP_BLOWER_TEST 1
#define STEP_VALVE_BLOWER_TEST 2
#define STEP_VALVE_PATIENT_TEST 3
#define STEP_VALVE_BLOWER_LEAK_TEST 4
#define STEP_VALVE_PATIENT_LEAK_TEST 5
#define STEP_O2_TEST 6
#define STEP_PRESSURE_TEST 7
#define STEP_PRESSURE_OFFSET_TEST 8
#define STEP_BATTERY_TEST 9
#define STEP_BUZZER_TEST 10

#define NUMBER_OF_STATES 11u

#define UNGREEDY(is_drawn, statement)                                                              \
    if (is_drawn == 0u) {                                                                          \
        statement;                                                                                 \
        is_drawn = 1u;                                                                             \
    }

static uint8_t step = STEP_WELCOME;
static uint8_t is_drawn = false;

PressureValve servoBlower;
PressureValve servoPatient;
HardwareTimer* hardwareTimer1;
HardwareTimer* hardwareTimer3;
Blower blower;

int16_t pressureOffset;
int32_t pressureOffsetSum;
uint32_t pressureOffsetCount;
int16_t minOffsetValue = 0;
int16_t maxOffsetValue = 0;


/**
 * Block execution for a given duration
 *
 * @param ms  Duration of the blocking in millisecond
 */
void waitForInMs(uint16_t ms) {
    uint16_t start = millis();
    minOffsetValue = readPressureSensor(0, 0);
    maxOffsetValue = readPressureSensor(0, 0);
    pressureOffsetSum = 0;
    pressureOffsetCount = 0;


    while ((millis() - start) < ms) {
        // Measure 1 pressure per ms we wait
        if ((millis() - start) > pressureOffsetCount) {
            int16_t pressureValue = readPressureSensor(0, 0);
            pressureOffsetSum += pressureValue;
            minOffsetValue = min(pressureValue, minOffsetValue);
            maxOffsetValue = max(pressureValue, maxOffsetValue);
            pressureOffsetCount++;
        }
        continue;
    }
}

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
    DBG_DO(Serial.println((step + 1u) % NUMBER_OF_STATES));
    changeStep((step + 1u) % NUMBER_OF_STATES);
    last_time = millis();
    Buzzer_Stop();
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
    hardwareTimer1->setOverflow(ESC_PPM_PERIOD, MICROSEC_FORMAT);
    blower = Blower(hardwareTimer1, TIM_CHANNEL_ESC_BLOWER, PIN_ESC_BLOWER);
    blower.setup();
#endif

    BuzzerControl_Init();
    Buzzer_Init();
    initBattery();
    blower.stop();

    pinMode(PIN_LED_START, OUTPUT);
    digitalWrite(PIN_LED_START, LED_START_ACTIVE);
    waitForInMs(1000);
    resetScreen();
    screen.setCursor(0, 0);
    screen.print("Calibrating P offset");
    screen.setCursor(0, 2);
    screen.print("Patient must be");
    screen.setCursor(0, 3);
    screen.print("unplugged");
    waitForInMs(3000);
    resetScreen();
    pressureOffset = pressureOffsetSum / static_cast<int32_t>(pressureOffsetCount);
    DBG_DO({
        Serial.print("pressure offset = ");
        Serial.print(pressureOffsetSum);
        Serial.print(" / ");
        Serial.print(pressureOffsetCount);
        Serial.print(" = ");
        Serial.print(pressureOffset);
        Serial.println();
    })
    // Happens when patient is plugged at starting
    if (maxOffsetValue - minOffsetValue >= 10){
        resetScreen();
        screen.setCursor(0, 0);
        char line1[SCREEN_LINE_LENGTH + 1];
        (void)snprintf(line1, SCREEN_LINE_LENGTH + 1, "P offset is unstable", pressureOffset);
        screen.print(line1);
        screen.setCursor(0, 1);
        char line2[SCREEN_LINE_LENGTH + 1];
        (void)snprintf(line2, SCREEN_LINE_LENGTH + 1, "Max-Min: %3d mmH2O", maxOffsetValue - minOffsetValue);
        screen.print(line2);
        screen.setCursor(0, 2);
        screen.print("Unplug patient and");
        screen.setCursor(0, 3);
        screen.print("reboot");
        Buzzer_High_Prio_Start();
        while (true) {
        }
    }
    if (pressureOffset >= MAX_PRESSURE_OFFSET) {
        resetScreen();
        screen.setCursor(0, 0);
        char line1[SCREEN_LINE_LENGTH + 1];
        (void)snprintf(line1, SCREEN_LINE_LENGTH + 1, "P offset: %3d mmH2O", pressureOffset);
        screen.print(line1);
        screen.setCursor(0, 1);
        char line2[SCREEN_LINE_LENGTH + 1];
        (void)snprintf(line2, SCREEN_LINE_LENGTH + 1, "P offset is > %-3d", MAX_PRESSURE_OFFSET);
        screen.print(line2);
        screen.setCursor(0, 2);
        screen.print("Unplug patient and");
        screen.setCursor(0, 3);
        screen.print("reboot");
        Buzzer_High_Prio_Start();
        while (true) {
        }
    }

    screen.setCursor(0, 3);
    char message[SCREEN_LINE_LENGTH + 1];
    (void)snprintf(message, SCREEN_LINE_LENGTH + 1, "P offset: %3d mmH2O", pressureOffset);
    screen.print(message);
    delay(3000);
    resetScreen();
}

void loop() {
    btn_start.tick();

    switch (step) {
    case STEP_WELCOME:
    default: {
        UNGREEDY(is_drawn, {
            display("MakAir test", "Press start button");
            displayLine(VERSION, 3);
            servoBlower.open();
            servoBlower.execute();
            servoPatient.open();
            servoPatient.execute();
            blower.stop();
        });
        break;
    }
    case STEP_BLOWER_TEST: {
        UNGREEDY(is_drawn, display("Test blower", "Continuer : Start"));
        blower.runSpeed(1500);
        break;
    }
    case STEP_VALVE_BLOWER_TEST: {
        UNGREEDY(is_drawn, {
            display("Test Valve inspi", "Continuer : Start");
            blower.stop();
        });

        if ((millis() - last_time) < 5000) {
            servoBlower.open();
            servoBlower.execute();
            displayLine("Etat : Ouvert", 3);

        } else if ((millis() - last_time) < 10000) {
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
        if ((millis() - last_time) < 5000) {
            servoPatient.open();
            servoPatient.execute();
            displayLine("Etat : Ouvert", 3);

        } else if ((millis() - last_time) < 10000) {
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
        blower.runSpeed(1500);
        break;
    }
    case STEP_VALVE_PATIENT_LEAK_TEST: {
        UNGREEDY(is_drawn, display("Test fuite expi", "Continuer : Start"));
        servoPatient.close();
        servoPatient.execute();
        servoBlower.open();
        servoBlower.execute();
        blower.runSpeed(1500);
        break;
    }

    case STEP_O2_TEST: {
        UNGREEDY(is_drawn, display("Test sortie 02", "Continuer : Start"));
        servoPatient.open();
        servoPatient.execute();
        servoBlower.close();
        servoBlower.execute();
        blower.runSpeed(1500);
        break;
    }

    case STEP_PRESSURE_TEST: {
        servoPatient.close();
        servoPatient.execute();
        servoBlower.open();
        servoBlower.execute();
        blower.runSpeed(1790);
        UNGREEDY(is_drawn, display("Test pression", "Continuer : Start"));
        if ((millis() - last_time) >= 200) {
            int pressure = readPressureSensor(0, pressureOffset);
            char msg[SCREEN_LINE_LENGTH + 1];
            (void)snprintf(msg, SCREEN_LINE_LENGTH + 1, "Pression : %3d mmH2O", pressure);
            displayLine(msg, 3);
            last_time = millis();
        }
        break;
    }

    case STEP_PRESSURE_OFFSET_TEST: {
        servoPatient.close();
        servoPatient.execute();
        servoBlower.open();
        servoBlower.execute();
        blower.stop();
        UNGREEDY(is_drawn, display("Test offset pression", "Continuer : Start"));
        if ((millis() - last_time) >= 200) {
            int pressure = readPressureSensor(0, pressureOffset);
            char msg[SCREEN_LINE_LENGTH + 1];
            (void)snprintf(msg, SCREEN_LINE_LENGTH + 1, "Pression : %3d mmH2O", pressure);
            displayLine(msg, 3);
            last_time = millis();
        }
        break;
    }

    case STEP_BATTERY_TEST: {
        UNGREEDY(is_drawn, {
            display("Test batterie", "Continuer : Start");
            blower.stop();
        });
        updateBatterySample();
        if ((millis() - last_time) >= 200) {
            char msg[SCREEN_LINE_LENGTH + 1];
            (void)snprintf(msg, SCREEN_LINE_LENGTH + 1, "Batterie : %3u V", getBatteryLevel());
            displayLine(msg, 3);
            last_time = millis();
        }
        break;
    }

    case STEP_BUZZER_TEST: {
        UNGREEDY(is_drawn, {
            display("Test alarme", "Continuer : Start");
            Buzzer_High_Prio_Start();
        });
        break;
    }
    }

    delay(10);
}

#endif
