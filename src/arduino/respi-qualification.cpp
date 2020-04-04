/*=============================================================================
 * @file respi-tests.h
 *
 * COVID Respirator
 *
 * @section copyright Copyright
 *
 * Makers For Life
 *
 * @section descr File description
 *
 * This file execute the MakAir test program
 */

#pragma once

#include "config.h"
#if MODE == MODE_QUALIFICATION

// INCLUDES ===================================================================

// External
#include <Arduino.h>
#include <OneButton.h>

// Internal
#include "affichage.h"
#include "air_transistor.h"
#include "common.h"
#include "debug.h"
#include "parameters.h"
#include "pression.h"

/**
 * Liste de toutes les étapes de test du montage.
 */
#define STEP_LCD 1
#define STEP_WELCOME 2
#define STEP_BTN_PRESSION_CRETE_PLUS 3
#define STEP_BTN_PRESSION_CRETE_MINUS 4
#define STEP_BTN_PRESSION_PLATEAU_PLUS 5
#define STEP_BTN_PRESSION_PLATEAU_MINUS 6
#define STEP_BTN_PEP_PLUS 7
#define STEP_BTN_PEP_MINUS 8
#define STEP_BTN_CYCLE_PLUS 9
#define STEP_BTN_CYCLE_MINUS 10
#define STEP_BTN_ALARM_OFF 11
#define STEP_BTN_START 12
#define STEP_BTN_STOP 13
#define STEP_LED_RED 14
#define STEP_LED_YELLOW 15
#define STEP_LED_GREEN 16
#define STEP_ALARM 17
#define STEP_SERVO_BLOWER_OPEN 18
#define STEP_SERVO_BLOWER_CLOSE 19
#define STEP_SERVO_PATIENT_OPEN 20
#define STEP_SERVO_PATIENT_CLOSE 21
#define STEP_SERVOS 22
#define STEP_BLOWER 23
#define STEP_PRESSURE_EMPTY 24
#define STEP_PRESSURE_VAL1 25
#define STEP_PRESSURE_VAL2 26
#define STEP_PRESSURE_VAL3 27
#define STEP_DONE 28

static uint8_t step = STEP_LCD;

static uint8_t is_drawn = false;
#define UNGREEDY(is_drawn, statement) if (is_drawn == 0) { statement; is_drawn = 1; }

void changeStep(uint8_t new_step) {
    step = new_step;
    is_drawn = 0;
}

static uint8_t errors = 0;

#define PRESSURE_MARGIN_PER_CENT 5
#define PRESSURE_EMPTY_MARGIN 5 // [mmH2O]
#define PRESSURE_EMPTY 0 // [mmH2O]
#define PRESSURE_VAL1 700 // [mmH2O]
#define PRESSURE_VAL2 150 // [mmH2O]
#define PRESSURE_VAL3 300 // [mmH2O]

bool isPressureValueGoodEnough(int expected, int pressure) {
    int epsylon = expected * PRESSURE_MARGIN_PER_CENT / 100;
    return (pressure >= (expected - epsylon)) && (pressure <= (expected + epsylon));
}

/**
 * Affiche un pattern de la forme suivante sur l'écran LCD.
 * La taille du pattern s'adapte en fonction du nombre de ligne et
 * de caractères de l'écran LCD.
 *
 * 01234567890123456789
 * 12345678901234567890
 * 23456789012345678901
 * 34567890123456789012
 */
void displayTestPattern()
{
    for (int line = 0; line < SCREEN_LINE_NUMBER; line++)
    {
        screen.setCursor(0, line);
        for (int character = 0; character < SCREEN_LINE_LENGTH; character++)
        {
            screen.print((character + line) % 10);
        }
    }
}

//! This function displays 2 lines of 16 characters (or less)
void display(char line1[], char line2[]) {
    resetScreen();
    screen.setCursor(0, 0);
    screen.print(line1);
    screen.setCursor(0, 1);
    screen.print(line2);
}

//! This function displays only adds a status message in the 4th line
void displayStatus(char msg[], uint8_t line = 3) {
    screen.setCursor(0, line);
    screen.print("                    ");
    screen.setCursor(0, line);
    screen.print(msg);
}

AirTransistor servoBlower;
AirTransistor servoPatient;
HardwareTimer* hardwareTimer1;
HardwareTimer* hardwareTimer3;

void onPressionCretePlusClick()
{
    DBG_DO(Serial.println("pression crete ++"));
    if (step == STEP_LCD || step == STEP_WELCOME) {
        changeStep(step + 1);
    } else if (step == STEP_BTN_PRESSION_CRETE_PLUS) {
        changeStep(step + 1);
    } else if (step == STEP_SERVO_BLOWER_OPEN) {
        changeStep(step + 1);
    } else if (step != STEP_DONE) {
        displayStatus("WRONG BUTTON PUSHED");
        errors++;
    }
}

void onPressionCreteMinusClick()
{
    DBG_DO(Serial.println("pression crete --"));
    if (step == STEP_LCD || step == STEP_WELCOME) {
        changeStep(step + 1);
    } else if (step == STEP_BTN_PRESSION_CRETE_MINUS) {
        changeStep(step + 1);
    } else if (step == STEP_SERVO_BLOWER_CLOSE) {
        changeStep(step + 1);
    } else if (step != STEP_DONE) {
        displayStatus("WRONG BUTTON PUSHED");
        errors++;
    }
}

void onPressionPlateauPlusClick()
{
    DBG_DO(Serial.println("pression plateau ++"));
    if (step == STEP_LCD || step == STEP_WELCOME) {
        changeStep(step + 1);
    } else if (step == STEP_BTN_PRESSION_PLATEAU_PLUS) {
        changeStep(step + 1);
    } else if (step == STEP_SERVO_PATIENT_OPEN) {
        changeStep(step + 1);
    } else if (step != STEP_DONE) {
        displayStatus("WRONG BUTTON PUSHED");
        errors++;
    }
}

void onPressionPlateauMinusClick()
{
    DBG_DO(Serial.println("pression plateau --"));
    if (step == STEP_LCD || step == STEP_WELCOME) {
        changeStep(step + 1);
    } else if (step == STEP_BTN_PRESSION_PLATEAU_MINUS) {
        changeStep(step + 1);
    } else if (step == STEP_SERVO_PATIENT_CLOSE) {
        changeStep(step + 1);
    } else if (step != STEP_DONE) {
        displayStatus("WRONG BUTTON PUSHED");
        errors++;
    }
}

void onPepPlusClick()
{
    DBG_DO(Serial.println("pression pep ++"));
    if (step == STEP_LCD || step == STEP_WELCOME) {
        changeStep(step + 1);
    } else if (step == STEP_BTN_PEP_PLUS) {
        changeStep(step + 1);
    } else if (step == STEP_SERVOS) {
        changeStep(step + 1);
    } else if (step != STEP_DONE) {
        displayStatus("WRONG BUTTON PUSHED");
        errors++;
    }
}

void onPepMinusClick()
{
    DBG_DO(Serial.println("pression pep --"));
    if (step == STEP_LCD || step == STEP_WELCOME) {
        changeStep(step + 1);
    } else if (step == STEP_BTN_PEP_MINUS) {
        changeStep(step + 1);
    } else if (step == STEP_BLOWER) {
        hardwareTimer3->setCaptureCompare(TIM_CHANNEL_ESC_BLOWER, Angle2MicroSeconds(0), MICROSEC_COMPARE_FORMAT);
        changeStep(step + 1);
    } else if (step != STEP_DONE) {
        displayStatus("WRONG BUTTON PUSHED");
        errors++;
    }
}

void onCyclePlusClick()
{
    DBG_DO(Serial.println("cycle ++"));
    if (step == STEP_LCD || step == STEP_WELCOME) {
        changeStep(step + 1);
    } else if (step == STEP_BTN_CYCLE_PLUS) {
        changeStep(step + 1);
    } else if (step != STEP_DONE) {
        displayStatus("WRONG BUTTON PUSHED");
        errors++;
    }
}

void onCycleMinusClick()
{
    DBG_DO(Serial.println("cycle --"));
    if (step == STEP_LCD || step == STEP_WELCOME) {
        changeStep(step + 1);
    } else if (step == STEP_BTN_CYCLE_MINUS) {
        changeStep(step + 1);
    } else if (step != STEP_DONE) {
        displayStatus("WRONG BUTTON PUSHED");
        errors++;
    }
}

void onAlarmOffClick()
{
    DBG_DO(Serial.println("alarm off"));
    if (step == STEP_LCD || step == STEP_WELCOME) {
        changeStep(step + 1);
    } else if (step == STEP_BTN_ALARM_OFF) {
        changeStep(step + 1);
    } else if (step == STEP_ALARM) {
        changeStep(step + 1);
    } else if (step != STEP_DONE) {
        displayStatus("WRONG BUTTON PUSHED");
        errors++;
    }
}

void onStartClick()
{
    DBG_DO(Serial.println("start"));
    if (step == STEP_LCD || step == STEP_WELCOME || step == STEP_LED_RED || step == STEP_LED_GREEN) {
        changeStep(step + 1);
    } else if (step == STEP_BTN_START) {
        changeStep(step + 1);
    } else if (step == STEP_PRESSURE_EMPTY) {
        int pressure = readPressureSensor(0);
        if (pressure <= PRESSURE_EMPTY_MARGIN) {
            changeStep(step + 1);
        } else {
            char error_msg[SCREEN_LINE_LENGTH];
            sprintf(error_msg, "BAD PRESSURE: %d", pressure);
            displayStatus(error_msg);
        }
    } else if (step == STEP_PRESSURE_VAL2) {
        int pressure = readPressureSensor(0);
        if (isPressureValueGoodEnough(PRESSURE_VAL2, pressure)) {
            changeStep(step + 1);
        } else {
            char error_msg[SCREEN_LINE_LENGTH];
            sprintf(error_msg, "BAD PRESSURE: %d", pressure);
            displayStatus(error_msg);
        }
    } else if (step != STEP_DONE) {
        displayStatus("WRONG BUTTON PUSHED");
        errors++;
    }
}

void onStopClick()
{
    DBG_DO(Serial.println("stop"));
    if (step == STEP_LCD || step == STEP_WELCOME || step == STEP_LED_YELLOW) {
        changeStep(step + 1);
    } else if (step == STEP_BTN_STOP) {
        changeStep(step + 1);
    } else if (step == STEP_PRESSURE_VAL1) {
        int pressure = readPressureSensor(0);
        if (isPressureValueGoodEnough(PRESSURE_VAL1, pressure)) {
            changeStep(step + 1);
        } else {
            char error_msg[SCREEN_LINE_LENGTH];
            sprintf(error_msg, "BAD PRESSURE: %d", pressure);
            displayStatus(error_msg);
        }
    } else if (step == STEP_PRESSURE_VAL3) {
        int pressure = readPressureSensor(0);
        if (isPressureValueGoodEnough(PRESSURE_VAL3, pressure)) {
            changeStep(step + 1);
        } else {
            char error_msg[SCREEN_LINE_LENGTH];
            sprintf(error_msg, "BAD PRESSURE: %d", pressure);
            displayStatus(error_msg);
        }
    } else if (step != STEP_DONE) {
        displayStatus("WRONG BUTTON PUSHED");
        errors++;
    }
}

static AnalogButtons analogButtons(PIN_CONTROL_BUTTONS, INPUT);

Button btn_pression_crete_plus = Button(TENSION_BTN_PRESSION_P_CRETE_PLUS, &onPressionCretePlusClick);
Button btn_pression_crete_minus = Button(TENSION_BTN_PRESSION_P_CRETE_MINUS, &onPressionCreteMinusClick);
Button btn_pression_plateau_plus = Button(TENSION_BTN_PRESSION_PLATEAU_PLUS, &onPressionPlateauPlusClick);
Button btn_pression_plateau_minus = Button(TENSION_BTN_PRESSION_PLATEAU_MINUS, &onPressionPlateauMinusClick);
Button btn_pep_plus = Button(TENSION_BTN_PEP_PLUS, &onPepPlusClick);
Button btn_pep_minus = Button(TENSION_BTN_PEP_MINUS, &onPepMinusClick);
Button btn_cycle_plus = Button(TENSION_BTN_CYCLE_PLUS, &onCyclePlusClick);
Button btn_cycle_minus = Button(TENSION_BTN_CYCLE_MINUS, &onCycleMinusClick);

OneButton btn_alarm_off(PIN_BTN_ALARM_OFF, false, false);
OneButton btn_start(PIN_BTN_START, false, false);
OneButton btn_stop(PIN_BTN_STOP, false, false);

void setup()
{
    DBG_DO(Serial.begin(115200));
    DBG_DO(Serial.println("demarrage"));

    analogButtons.add(btn_pression_crete_plus);
    analogButtons.add(btn_pression_crete_minus);
    analogButtons.add(btn_pression_plateau_plus);
    analogButtons.add(btn_pression_plateau_minus);
    analogButtons.add(btn_pep_plus);
    analogButtons.add(btn_pep_minus);
    analogButtons.add(btn_cycle_plus);
    analogButtons.add(btn_cycle_minus);

    btn_alarm_off.attachClick(onAlarmOffClick);
    btn_start.attachClick(onStartClick);
    btn_stop.attachClick(onStopClick);

    pinMode(PIN_LED_RED, OUTPUT);
    pinMode(PIN_LED_YELLOW, OUTPUT);
    pinMode(PIN_LED_GREEN, OUTPUT);

    pinMode(PIN_ALARM, OUTPUT);

    // Timer for servoBlower
    hardwareTimer1 = new HardwareTimer(TIM1);
    hardwareTimer1->setOverflow(SERVO_VALVE_PERIOD, MICROSEC_FORMAT);

    // Timer for servoPatient and escBlower
    hardwareTimer3 = new HardwareTimer(TIM3);
    hardwareTimer3->setOverflow(SERVO_VALVE_PERIOD, MICROSEC_FORMAT);

    // Servo blower setup
    servoBlower = AirTransistor(BLOWER_OUVERT, BLOWER_FERME, hardwareTimer1,
                                TIM_CHANNEL_SERVO_VALVE_BLOWER, PIN_SERVO_BLOWER);
    servoBlower.setup();
    hardwareTimer1->resume();

    // Servo patient setup
    servoPatient = AirTransistor(PATIENT_OUVERT, PATIENT_FERME, hardwareTimer3,
                                 TIM_CHANNEL_SERVO_VALVE_PATIENT, PIN_SERVO_PATIENT);
    servoPatient.setup();

    // Manual escBlower setup
    // Output compare activation on pin PIN_ESC_BLOWER
    hardwareTimer3->setMode(TIM_CHANNEL_ESC_BLOWER, TIMER_OUTPUT_COMPARE_PWM1, PIN_ESC_BLOWER);
    // Set PPM width to 1ms
    hardwareTimer3->setCaptureCompare(TIM_CHANNEL_ESC_BLOWER, Angle2MicroSeconds(0),
                                      MICROSEC_COMPARE_FORMAT);
    hardwareTimer3->resume();

    startScreen();
}

#define CYCLE_TICKS 100
int16_t remainingTicks = -1;

void loop() {
    if (remainingTicks < 0) {
        remainingTicks = CYCLE_TICKS;
    }

    analogButtons.check();
    btn_alarm_off.tick();
    btn_start.tick();
    btn_stop.tick();

    digitalWrite(PIN_LED_RED, LOW);
    digitalWrite(PIN_LED_YELLOW, LOW);
    digitalWrite(PIN_LED_GREEN, LOW);
    digitalWrite(PIN_ALARM, LOW);

    switch (step) {
        case STEP_LCD: {
            UNGREEDY(is_drawn, displayTestPattern());
            break;
        }
        case STEP_WELCOME: {
            UNGREEDY(is_drawn, {
                display("MakAir testing", "Press any button");
                displayStatus(VERSION);
            });
            break;
        }
        case STEP_BTN_PRESSION_CRETE_PLUS: {
            UNGREEDY(is_drawn, display("Press the button", "PCrete +"));
            break;
        }
        case STEP_BTN_PRESSION_CRETE_MINUS: {
            UNGREEDY(is_drawn, display("Press the button", "PCrete -"));
            break;
        }
        case STEP_BTN_PRESSION_PLATEAU_PLUS: {
            UNGREEDY(is_drawn, display("Press the button", "PPlateau +"));
            break;
        }
        case STEP_BTN_PRESSION_PLATEAU_MINUS: {
            UNGREEDY(is_drawn, display("Press the button", "PPlateau -"));
            break;
        }
        case STEP_BTN_PEP_PLUS: {
            UNGREEDY(is_drawn, display("Press the button", "PPep +"));
            break;
        }
        case STEP_BTN_PEP_MINUS: {
            UNGREEDY(is_drawn, display("Press the button", "PPep -"));
            break;
        }
        case STEP_BTN_CYCLE_PLUS: {
            UNGREEDY(is_drawn, display("Press the button", "Cycle +"));
            break;
        }
        case STEP_BTN_CYCLE_MINUS: {
            UNGREEDY(is_drawn, display("Press the button", "Cycle -"));
            break;
        }
        case STEP_BTN_ALARM_OFF: {
            UNGREEDY(is_drawn, display("Press the button", "Alarm OFF"));
            break;
        }
        case STEP_BTN_START: {
            UNGREEDY(is_drawn, display("Press the button", "Start"));
            break;
        }
        case STEP_BTN_STOP: {
            UNGREEDY(is_drawn, display("Press the button", "Stop"));
            break;
        }
        case STEP_LED_RED: {
            UNGREEDY(is_drawn, display("Red LED is ON", "Press start"));
            digitalWrite(PIN_LED_RED, HIGH);
            break;
        }
        case STEP_LED_YELLOW: {
            UNGREEDY(is_drawn, display("Yellow LED is ON", "Press stop"));
            digitalWrite(PIN_LED_YELLOW, HIGH);
            break;
        }
        case STEP_LED_GREEN: {
            UNGREEDY(is_drawn, display("Green LED is ON", "Press start"));
            digitalWrite(PIN_LED_GREEN, HIGH);
            break;
        }
        case STEP_ALARM: {
            UNGREEDY(is_drawn, display("Alarm is ON", "Press alarm OFF"));
            digitalWrite(PIN_ALARM, HIGH);
            break;
        }
        case STEP_SERVO_BLOWER_OPEN: {
            UNGREEDY(is_drawn, display("Servo blower opened", "Press PCrete +"));
            servoBlower.command = BLOWER_OUVERT;
            servoBlower.execute();
            break;
        }
        case STEP_SERVO_BLOWER_CLOSE: {
            UNGREEDY(is_drawn, display("Servo blower closed", "Press PCrete -"));
            servoBlower.command = BLOWER_FERME;
            servoBlower.execute();
            break;
        }
        case STEP_SERVO_PATIENT_OPEN: {
            UNGREEDY(is_drawn, display("Servo patient opened", "Press PPlateau +"));
            servoPatient.command = PATIENT_OUVERT;
            servoPatient.execute();
            break;
        }
        case STEP_SERVO_PATIENT_CLOSE: {
            UNGREEDY(is_drawn, display("Servo patient closed", "Press PPlateau -"));
            servoPatient.command = PATIENT_FERME;
            servoPatient.execute();
            break;
        }
        case STEP_SERVOS: {
            UNGREEDY(is_drawn, display("Both servos moving", "Press PPep +"));
            if (remainingTicks == (CYCLE_TICKS / 2)) {
                servoBlower.command = BLOWER_OUVERT;
                servoPatient.command = PATIENT_OUVERT;
                servoBlower.execute();
                servoPatient.execute();
            } else if (remainingTicks == 0) {
                servoBlower.command = BLOWER_FERME;
                servoBlower.execute();
                servoPatient.command = PATIENT_FERME;
                servoPatient.execute();
            }
            break;
        }
        case STEP_BLOWER: {
            UNGREEDY(is_drawn, {
                display("Blower is ON", "Press PPeP -");
                hardwareTimer3->setCaptureCompare(TIM_CHANNEL_ESC_BLOWER, Angle2MicroSeconds(120), MICROSEC_COMPARE_FORMAT);
            });
            break;
        }
        case STEP_PRESSURE_EMPTY: {
            UNGREEDY(is_drawn, display("Unplug pressure", "sensor, press start"));
            break;
        }
        case STEP_PRESSURE_VAL1: {
            UNGREEDY(is_drawn, {
                char msg[SCREEN_LINE_LENGTH];
                sprintf(msg, "Put pressure of %d", PRESSURE_VAL1);
                display(msg, "mmH2O, press stop");
            });
            break;
        }
        case STEP_PRESSURE_VAL2: {
            UNGREEDY(is_drawn, {
                char msg[SCREEN_LINE_LENGTH];
                sprintf(msg, "Put pressure of %d", PRESSURE_VAL2);
                display(msg, "mmH2O, press start");
            });
            break;
        }
        case STEP_PRESSURE_VAL3: {
            UNGREEDY(is_drawn, {
                char msg[SCREEN_LINE_LENGTH];
                sprintf(msg, "Put pressure of %d", PRESSURE_VAL3);
                display(msg, "mmH2O, press stop");
            });
            break;
        }
        case STEP_DONE: {
            UNGREEDY(is_drawn, {
                display("End of testing", "Success");
                if (errors > 0) {
                    char error_msg[SCREEN_LINE_LENGTH];
                    sprintf(error_msg, "Errors: %d", errors);
                    displayStatus(error_msg);
                }
            });
            break;
        }
    }

    if (step == STEP_PRESSURE_EMPTY || step == STEP_PRESSURE_VAL1 || step == STEP_PRESSURE_VAL2 || step == STEP_PRESSURE_VAL3) {
        char status_msg[SCREEN_LINE_LENGTH];
        sprintf(status_msg, "Pressure: %d", readPressureSensor(0));
        displayStatus(status_msg, 2);
    }

    remainingTicks--;
    delay(10);
}

#endif
