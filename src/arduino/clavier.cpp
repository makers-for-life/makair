/*=============================================================================
 * @file clavier.h
 *
 * COVID Respirator
 *
 * @section copyright Copyright
 *
 * Makers For Life
 *
 * @section descr File description
 *
 * This file is the implementation for input keyboard
 */

#pragma once

// INCLUDES ===================================================================

// Associated header
#include "clavier.h"

// External
#include <AnalogButtons.h>
#include <OneButton.h>

// Internal
#include "alarm.h"
#include "debug.h"
#include "parameters.h"
#include "pressure_controller.h"

// INITIALISATION =============================================================

static AnalogButtons analogButtons(PIN_CONTROL_BUTTONS, INPUT, 5, 30);
static OneButton btn_alarm_off(PIN_BTN_ALARM_OFF, false, false);
static OneButton btn_start(PIN_BTN_START, false, false);
static OneButton btn_stop(PIN_BTN_STOP, false, false);

/*-----------------------------------------------------------------------------
 * Button handlers
 */

/* Handler of the button to increment the Crete pressure */
void onPressionCretePlus() { pController.onPressionCretePlus(); }

/* Handler of the button to decrement the Crete pressure */
void onPressionCreteMinus() { pController.onPressionCreteMinus(); }

/* Handler of the button to decrement the plateau pressure */
void onPressionPlateauMinus() { pController.onPressionPlateauMinus(); }

/* Handler of the button to increment the plateau pressure */
void onPressionPlateauPlus() { pController.onPressionPlateauPlus(); }

/* Handler of the button to increment the PEP pressure */
void onPressionPepPlus() { pController.onPressionPepPlus(); }

/* Handler of the button to decrement the PEP pressure */
void onPressionPepMinus() { pController.onPressionPepMinus(); }

/* Handler of the button to increment the number of breathing cycles */
void onCyclePlus() { pController.onCyclePlus(); }

/* Handler of the button to decrement the number of breathing cycles */
void onCycleMinus() { pController.onCycleMinus(); }

/*
 * Buttons associations with analog levels and handlers
 */

Button btnPressionCretePlus(TENSION_BTN_PRESSION_P_CRETE_PLUS, &onPressionCretePlus);
Button btnPressionCreteMinus(TENSION_BTN_PRESSION_P_CRETE_MINUS, &onPressionCreteMinus);
Button btnPressionPlateauPlus(TENSION_BTN_PRESSION_PLATEAU_PLUS, &onPressionPlateauPlus);
Button btnPressionPlateauMinus(TENSION_BTN_PRESSION_PLATEAU_MINUS, &onPressionPlateauMinus);
Button btnPressionPepPlus(TENSION_BTN_PEP_PLUS, &onPressionPepPlus);
Button btnPressionPepMinus(TENSION_BTN_PEP_MINUS, &onPressionPepMinus);
Button btnCyclePlus(TENSION_BTN_CYCLE_PLUS, &onCyclePlus);
Button btnCycleMinus(TENSION_BTN_CYCLE_MINUS, &onCycleMinus);

/*-----------------------------------------------------------------------------
 * initKeyboard does the initializations to use a keyboard
 */
void initKeyboard()
{
    analogButtons.add(btnPressionCretePlus);
    analogButtons.add(btnPressionCreteMinus);
    analogButtons.add(btnPressionPlateauPlus);
    analogButtons.add(btnPressionPlateauMinus);
    analogButtons.add(btnPressionPepPlus);
    analogButtons.add(btnPressionPepMinus);
    analogButtons.add(btnCyclePlus);
    analogButtons.add(btnCycleMinus);

    btn_alarm_off.attachClick(onAlarmOffClick);
    btn_start.attachClick(onStartClick);
    btn_stop.attachClick(onStopClick);
}

/*-----------------------------------------------------------------------------
 * keyboardLoop triggers the reading of the buttons and call the handlers
 * Has to be called within the Pressure controller loop
 */
void keyboardLoop()
{
    analogButtons.check();
    btn_alarm_off.tick();
    btn_start.tick();
    btn_stop.tick();
}

void calibrateButtons()
{
    uint16_t value = analogRead(PIN_CONTROL_BUTTONS);
    Serial.println(value);
    delay(250);
}

void onAlarmOffClick() { Alarm_Stop(); }

void onStartClick() {}

void onStopClick() {}