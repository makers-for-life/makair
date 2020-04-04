/*
    Copyright (C) 2020 Makers For Life
*/
/******************************************************************************
 * @author Makers For Life
 * @file clavier.cpp
 * @brief Analog buttons related functions
 *
 * This relies on the Analog Buttons library (https://github.com/rlogiacco/AnalogButtons).
 *****************************************************************************/

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

/// Abstraction to handle buttons connected to one analog pin through a voltage divider
static AnalogButtons analogButtons(PIN_CONTROL_BUTTONS, INPUT, 5, 30);
static OneButton btn_alarm_off(PIN_BTN_ALARM_OFF, false, false);
static OneButton btn_start(PIN_BTN_START, false, false);
static OneButton btn_stop(PIN_BTN_STOP, false, false);

/*-----------------------------------------------------------------------------
 * Button handlers
 */

/// Handler of the button to increment the crete pressure
void onPressionCretePlus() { pController.onPressionCretePlus(); }

/// Handler of the button to decrement the crete pressure
void onPressionCreteMinus() { pController.onPressionCreteMinus(); }

/// Handler of the button to decrement the plateau pressure
void onPressionPlateauMinus() { pController.onPressionPlateauMinus(); }

/// Handler of the button to increment the plateau pressure
void onPressionPlateauPlus() { pController.onPressionPlateauPlus(); }

/// Handler of the button to increment the PEP pressure
void onPressionPepPlus() { pController.onPressionPepPlus(); }

/// Handler of the button to decrement the PEP pressure
void onPressionPepMinus() { pController.onPressionPepMinus(); }

/// Handler of the button to increment the number of breathing cycles
void onCyclePlus() { pController.onCyclePlus(); }

/// Handler of the button to decrement the number of breathing cycles
void onCycleMinus() { pController.onCycleMinus(); }

/// Handler of the button to stop alarm
void onAlarmOff() { Alarm_Stop(); }

/// Handler of the button to start
void onStart() {}

/// Handler of the button to stop
void onStop() {}

/**
 * @name Bindings between analog levels and handlers
 */
///@{
Button btnPressionCretePlus(TENSION_BTN_PRESSION_P_CRETE_PLUS, &onPressionCretePlus);
Button btnPressionCreteMinus(TENSION_BTN_PRESSION_P_CRETE_MINUS, &onPressionCreteMinus);
Button btnPressionPlateauPlus(TENSION_BTN_PRESSION_PLATEAU_PLUS, &onPressionPlateauPlus);
Button btnPressionPlateauMinus(TENSION_BTN_PRESSION_PLATEAU_MINUS, &onPressionPlateauMinus);
Button btnPressionPepPlus(TENSION_BTN_PEP_PLUS, &onPressionPepPlus);
Button btnPressionPepMinus(TENSION_BTN_PEP_MINUS, &onPressionPepMinus);
Button btnCyclePlus(TENSION_BTN_CYCLE_PLUS, &onCyclePlus);
Button btnCycleMinus(TENSION_BTN_CYCLE_MINUS, &onCycleMinus);
///@}

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

    btn_alarm_off.attachClick(onAlarmOff);
    btn_start.attachClick(onStart);
    btn_stop.attachClick(onStop);
}

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
