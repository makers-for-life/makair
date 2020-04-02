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

// Internal
#include "air_transistor.h"
#include "affichage.h"
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
#define STEP_DONE 11

static uint16_t step = STEP_LCD;

static uint16_t is_drawn = false;
#define UNGREEDY(is_drawn, statement) if (is_drawn == 0) { statement; is_drawn = 1; }

void changeStep(uint16_t new_step) {
    step = new_step;
    is_drawn = 0;
}

static uint16_t errors = 0;

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
    uint16_t line_number;
    uint16_t line_length;
    switch (screenSize)
    {
    case ScreenSize::CHARS_20:
    {
        line_number = 4;
        line_length = 20;
        break;
    }
    default:
    {
        line_number = 2;
        line_length = 16;
    }
    }

    for (int line = 0; line < line_number; line++)
    {
        screen.setCursor(0, line);
        for (int character = 0; character < line_length; character++)
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
void displayStatus(char msg[]) {
    screen.setCursor(0, 3);
    screen.print(msg);
}

void onPressionCretePlusClick()
{
    DBG_DO(Serial.println("pression crete ++"));
    if (step == STEP_LCD || step == STEP_WELCOME) {
        changeStep(step + 1);
    } else if (step == STEP_BTN_PRESSION_CRETE_PLUS) {
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

static AnalogButtons analogButtons(PIN_CONTROL_BUTTONS, INPUT);

Button btn_pression_crete_plus = Button(TENSION_BTN_PRESSION_P_CRETE_PLUS, &onPressionCretePlusClick);
Button btn_pression_crete_minus = Button(TENSION_BTN_PRESSION_P_CRETE_MINUS, &onPressionCreteMinusClick);
Button btn_pression_plateau_plus = Button(TENSION_BTN_PRESSION_PLATEAU_PLUS, &onPressionPlateauPlusClick);
Button btn_pression_plateau_minus = Button(TENSION_BTN_PRESSION_PLATEAU_MINUS, &onPressionPlateauMinusClick);
Button btn_pep_plus = Button(TENSION_BTN_PEP_PLUS, &onPepPlusClick);
Button btn_pep_minus = Button(TENSION_BTN_PEP_MINUS, &onPepMinusClick);
Button btn_cycle_plus = Button(TENSION_BTN_CYCLE_PLUS, &onCyclePlusClick);
Button btn_cycle_minus = Button(TENSION_BTN_CYCLE_MINUS, &onCycleMinusClick);
/*Button btn_rb_plus = Button(TENSION_BTN_RB_PLUS, &onRbPlusClick);
Button btn_rb_minus = Button(TENSION_BTN_RB_MINUS, &onRbMinusClick);
Button btn_valve_blower_plus = Button(TENSION_BTN_VALVE_BLOWER_PLUS, &onValveBlowerPlusClick);
Button btn_valve_blower_minus = Button(TENSION_BTN_VALVE_BLOWER_MINUS, &onValveBlowerMinusClick);
Button btn_valve_patient_plus = Button(TENSION_BTN_VALVE_PATIENT_PLUS, &onValvePatientPlusClick);
Button btn_valve_patient_minus = Button(TENSION_BTN_VALVE_PATIENT_MINUS, &onValvePatientMinusClick);*/

AirTransistor servoBlower;
AirTransistor servoPatient;

void setup()
{
    DBG_DO(Serial.begin(115200));
    DBG_DO(Serial.println("demarrage"));

    /*pinMode(PIN_CAPTEUR_PRESSION, INPUT);

    servoBlower = AirTransistor(
        BLOWER_FERME,
        BLOWER_OUVERT,
        BLOWER_FERME,
        BLOWER_OUVERT
    );

    servoPatient = AirTransistor(
        PATIENT_FERME,
        PATIENT_OUVERT,
        PATIENT_FERME,
        PATIENT_FERME
    );*/

    analogButtons.add(btn_pression_crete_plus);
    analogButtons.add(btn_pression_crete_minus);
    analogButtons.add(btn_pression_plateau_plus);
    analogButtons.add(btn_pression_plateau_minus);
    analogButtons.add(btn_pep_plus);
    analogButtons.add(btn_pep_minus);
    analogButtons.add(btn_cycle_plus);
    analogButtons.add(btn_cycle_minus);

    startScreen();
}

void loop() {
    analogButtons.check();

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
        case STEP_DONE: {
            UNGREEDY(is_drawn, {
                display("End of testing", "Success");
                if (errors > 0) {
                    char error_msg[20];
                    sprintf(error_msg, "Errors: %d", errors);
                    displayStatus(error_msg);
                }
            });
            break;
        }
    }

    delay(10);
}

#endif
