/*=============================================================================
 * @file respi-tests.cpp
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
 * This file execute the Makair program
 */

// INCLUDES ===================================================================
#if 0
// External
#include <AnalogButtons.h>
#include <Arduino.h>
#include <LiquidCrystal.h>
#include <OneButton.h>
#include <Servo.h>

// Internal
#include "common.h"
#include "config.h"
#include "debug.h"
#include "display.h"
#include "parameters.h"
#include "pressure_controller.h"

// TODO extraire ces 2 lignes dans de la configuration
const int LCD_LINE_LENGTH = 16;
const int LCD_LINE_NUMBER = 2;

const int SECONDS = 1000;

/**
 * Liste de toutes les étapes de test du montage.
 *
 * - TEST_STEP_START                  Affichage d'un pattern sur l'écran LCD
 * - TEST_BTN_PRESSION_PLATEAU_MINUS  Test du bouton pression plateau minus
 * - TEST_BTN_PRESSION_PLATEAU_PLUS   Test du bouton pression plateau plus
 * - TEST_BTN_PRESSION_PEP_MINUS      Test du bouton pression pression PEP minus
 * - TEST_BTN_PRESSION_PEP_PLUS       Test du bouton pression pression PEP plus
 * - TEST_BTN_CYCLE_MINUS             Test du bouton pression cyle minus
 * - TEST_BTN_CYCLE_PLUS              Test du bouton pression cycle plus
 * - TEST_STEP_STOP                   Fin du test
 */
const int TEST_STEP_START = 0;
const int TEST_BTN_PRESSION_PLATEAU_PLUS = TEST_STEP_START + 1;
const int TEST_BTN_PRESSION_PLATEAU_MINUS = TEST_BTN_PRESSION_PLATEAU_PLUS + 1;
const int TEST_BTN_PEP_PLUS = TEST_BTN_PRESSION_PLATEAU_MINUS + 1;
const int TEST_BTN_PEP_MINUS = TEST_BTN_PEP_PLUS + 1;
const int TEST_BTN_RB_PLUS = TEST_BTN_PEP_MINUS + 1;
const int TEST_BTN_RB_MINUS = TEST_BTN_RB_PLUS + 1;
const int TEST_BTN_ALARME_ON = TEST_BTN_RB_MINUS + 1;
const int TEST_BTN_ALARME_OFF = TEST_BTN_ALARME_ON + 1;
const int TEST_BTN_VALVE_BLOWER_PLUS = TEST_BTN_ALARME_OFF + 1;
const int TEST_BTN_VALVE_BLOWER_MINUS = TEST_BTN_VALVE_BLOWER_PLUS + 1;
const int TEST_BTN_VALVE_PATIENT_PLUS = TEST_BTN_VALVE_BLOWER_MINUS + 1;
const int TEST_BTN_VALVE_PATIENT_MINUS = TEST_BTN_VALVE_PATIENT_PLUS + 1;
const int TEST_STEP_STOP = TEST_BTN_VALVE_PATIENT_MINUS + 1;

int validatedStep = TEST_STEP_START;

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
void printPatternOnLcd()
{
    screen.setCursor(0, 0);

    for (int line = 0; line < LCD_LINE_NUMBER; line++)
    {
        for (int character = 0; character < LCD_LINE_LENGTH; character++)
        {
            screen.print((character + line) % 10);
        }
        screen.println();
    }
}

void onPressionPlateauPlusClick()
{
#ifdef DEBUG
    Serial.println("pression plateau ++");
#endif
    if (validatedStep == TEST_STEP_START)
    {
        screen.setCursor(0, 0);
        screen.println("Btn Plateau + OK");
        screen.print("Push Plateau -");
        validatedStep = TEST_BTN_PRESSION_PLATEAU_PLUS;
    }
}

void onPressionPlateauMinusClick()
{
#ifdef DEBUG
    Serial.println("pression plateau --");
#endif
    if (validatedStep == TEST_BTN_PRESSION_PLATEAU_PLUS)
    {
        screen.setCursor(0, 0);
        screen.println("Btn Plateau - OK");
        screen.print("Push PEP +");
        validatedStep = TEST_BTN_PRESSION_PLATEAU_MINUS;
    }
}

void onPepPlusClick()
{
#ifdef DEBUG
    Serial.println("pression pep ++");
#endif
    if (validatedStep == TEST_BTN_PRESSION_PLATEAU_MINUS)
    {
        screen.setCursor(0, 0);
        screen.println("Btn PEP + OK");
        screen.print("Push PEP -");
        validatedStep = TEST_BTN_PEP_PLUS;
    }
}

void onPepMinusClick()
{
#ifdef DEBUG
    Serial.println("pression pep --");
#endif
    if (validatedStep == TEST_BTN_PEP_PLUS)
    {
        screen.setCursor(0, 0);
        screen.println("Btn PEP - OK");
        screen.print("Push RB +");
        validatedStep = TEST_BTN_PEP_MINUS;
    }
}

void onRbPlusClick()
{
#ifdef DEBUG
    Serial.println("pression RB ++ ");
#endif
    if (validatedStep == TEST_BTN_PEP_MINUS)
    {
        screen.setCursor(0, 0);
        screen.println("Btn RB + OK");
        screen.print("Push RB -");
        validatedStep = TEST_BTN_RB_PLUS;
    }
}

void onRbMinusClick()
{
#ifdef DEBUG
    Serial.println("pression RB -- ");
#endif
    if (validatedStep == TEST_BTN_RB_PLUS)
    {
        screen.setCursor(0, 0);
        screen.println("Btn RB - OK");
        screen.print("Push Alarme ON");
        validatedStep = TEST_BTN_RB_MINUS;
    }
}

void onAlarmeOnClick()
{
#ifdef DEBUG
    Serial.println("Alarme ON ");
#endif
    if (validatedStep == TEST_BTN_RB_MINUS)
    {
        screen.setCursor(0, 0);
        screen.println("Alarme ON OK");
        screen.print("Push Alarme OFF");
        validatedStep = TEST_BTN_ALARME_ON;
    }
}

void onAlarmeOffClick()
{
#ifdef DEBUG
    Serial.println("Alarme OFF ");
#endif
    if (validatedStep == TEST_BTN_ALARME_ON)
    {
        screen.setCursor(0, 0);
        screen.println("Alarme OFF OK");
        screen.print("Push Blower +");
        validatedStep = TEST_BTN_ALARME_OFF;
    }
}

void onValveBlowerPlusClick()
{
#ifdef DEBUG
    Serial.println("Valve Blower ++ ");
#endif
    if (validatedStep == TEST_BTN_ALARME_OFF)
    {
        screen.setCursor(0, 0);
        screen.println("Btn Blower + OK");
        screen.print("Push Blower -");
        validatedStep = TEST_BTN_VALVE_BLOWER_PLUS;
    }
}

void onValveBlowerMinusClick()
{
#ifdef DEBUG
    Serial.println("Valve Blower -- ");
#endif
    if (validatedStep == TEST_BTN_VALVE_BLOWER_PLUS)
    {
        screen.setCursor(0, 0);
        screen.println("Btn Blower - OK");
        screen.print("Push Patient +");
        validatedStep = TEST_BTN_VALVE_BLOWER_MINUS;
    }
}

void onValvePatientPlusClick()
{
#ifdef DEBUG
    Serial.println("Valve Patient ++ ");
#endif
    if (validatedStep == TEST_BTN_VALVE_BLOWER_MINUS)
    {
        screen.setCursor(0, 0);
        screen.println("Btn Patient + OK");
        screen.print("Push Patient -");
        validatedStep = TEST_BTN_VALVE_PATIENT_PLUS;
    }
}

void onValvePatientMinusClick()
{
#ifdef DEBUG
    Serial.println("Valve Patient -- ");
#endif
    if (validatedStep == TEST_BTN_VALVE_PATIENT_PLUS)
    {
        screen.setCursor(0, 0);
        screen.println("Btn Patient - OK");
        screen.print("Test end.");
        validatedStep = TEST_BTN_VALVE_PATIENT_MINUS;
    }
}

Button btn_pression_plateau_plus =
    Button(TENSION_BTN_PRESSION_PLATEAU_PLUS, &onPressionPlateauPlusClick);
Button btn_pression_plateau_minus =
    Button(TENSION_BTN_PRESSION_PLATEAU_MINUS, &onPressionPlateauMinusClick);
Button btn_pep_plus = Button(TENSION_BTN_PEP_PLUS, &onPepPlusClick);
Button btn_pep_minus = Button(TENSION_BTN_PEP_MINUS, &onPepMinusClick);
Button btn_rb_plus = Button(TENSION_BTN_RB_PLUS, &onRbPlusClick);
Button btn_rb_minus = Button(TENSION_BTN_RB_MINUS, &onRbMinusClick);
Button btn_alarme_on = Button(TENSION_BTN_ALARME_ON, &onAlarmeOnClick);
Button btn_alarme_off = Button(TENSION_BTN_ALARME_OFF, &onAlarmeOffClick);
Button btn_valve_blower_plus = Button(TENSION_BTN_VALVE_BLOWER_PLUS, &onValveBlowerPlusClick);
Button btn_valve_blower_minus = Button(TENSION_BTN_VALVE_BLOWER_MINUS, &onValveBlowerMinusClick);
Button btn_valve_patient_plus = Button(TENSION_BTN_VALVE_PATIENT_PLUS, &onValvePatientPlusClick);
Button btn_valve_patient_minus = Button(TENSION_BTN_VALVE_PATIENT_MINUS, &onValvePatientMinusClick);

void setup()
{
#ifdef DEBUG
    Serial.begin(115200);
    Serial.println("demarrage");
#endif

    patient.attach(PIN_SERVO_PATIENT);
    blower.attach(PIN_SERVO_BLOWER);

    buttons.add(btn_pression_plateau_plus);
    buttons.add(btn_pression_plateau_minus);
    buttons.add(btn_pep_plus);
    buttons.add(btn_pep_minus);
    buttons.add(btn_rb_plus);
    buttons.add(btn_rb_minus);
    buttons.add(btn_alarme_on);
    buttons.add(btn_alarme_off);
    buttons.add(btn_valve_blower_plus);
    buttons.add(btn_valve_blower_minus);

    screen.begin(LCD_LINE_LENGTH, LCD_LINE_NUMBER);

    printPatternOnLcd();
}

void loop() { buttons.check(); }
#endif
