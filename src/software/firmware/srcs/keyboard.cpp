/*
    Copyright (C) 2020 Makers For Life
*/
/******************************************************************************
 * @author Makers For Life
 * @file keyboard.cpp
 * @brief Analog buttons related functions
 *
 * This relies on the Analog Buttons library (https://github.com/rlogiacco/AnalogButtons).
 *****************************************************************************/

#pragma once

// INCLUDES ===================================================================

// Associated header
#include "../includes/keyboard.h"

// External
#include <AnalogButtons.h>
#include <OneButton.h>

// Internal
#include "../includes/alarm.h"
#include "../includes/debug.h"
#include "../includes/parameters.h"
#include "../includes/pressure_controller.h"

// INITIALISATION =============================================================

/// Abstraction to handle buttons connected to one analog pin through a voltage divider
static AnalogButtons analogButtons(PIN_CONTROL_BUTTONS, INPUT, 5, 30);
static OneButton buttonAlarmOff(PIN_BTN_ALARM_OFF, false, false);
static OneButton buttonStart(PIN_BTN_START, false, false);
static OneButton buttonStop(PIN_BTN_STOP, false, false);

/*-----------------------------------------------------------------------------
 * Button handlers
 */

/// Handler of the button to increase the crete pressure
void onPeakPressureIncrease() { pController.onPeakPressureIncrease(); }

/// Handler of the button to decrease the crete pressure
void onPeakPressureDecrease() { pController.onPeakPressureDecrease(); }

/// Handler of the button to increase the plateau pressure
void onPlateauPressureIncrease() { pController.onPlateauPressureIncrease(); }

/// Handler of the button to decrease the plateau pressure
void onPlateauPressureDecrease() { pController.onPlateauPressureDecrease(); }

/// Handler of the button to increase the PEP pressure
void onPeepPressureIncrease() { pController.onPeepPressureIncrease(); }

/// Handler of the button to decrease the PEP pressure
void onPeepPressureDecrease() { pController.onPeepPressureDecrease(); }

/// Handler of the button to increase the number of breathing cycles
void onCycleIncrease() { pController.onCycleIncrease(); }

/// Handler of the button to decrease the number of breathing cycles
void onCycleDecrease() { pController.onCycleDecrease(); }

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
Button buttonPeakPressureIncrease(VOLTAGE_BUTTON_PEAK_PRESSURE_INCREASE, &onPeakPressureIncrease);
Button buttonPeakPressureDecrease(VOLTAGE_BUTTON_PEAK_PRESSURE_DECREASE, &onPeakPressureDecrease);
Button buttonPlateauPressureIncrease(VOLTAGE_BUTTON_PLATEAU_PRESSURE_INCREASE,
                                     &onPlateauPressureIncrease);
Button buttonPlateauPressureDecrease(VOLTAGE_BUTTON_PLATEAU_PRESSURE_DECREASE,
                                     &onPlateauPressureDecrease);
Button buttonPeepPressureIncrease(VOLTAGE_BUTTON_PEEP_PRESSURE_INCREASE, &onPeepPressureIncrease);
Button buttonPeepPressureDecrease(VOLTAGE_BUTTON_PEEP_PRESSURE_DECREASE, &onPeepPressureDecrease);
Button buttonCycleIncrease(VOLTAGE_BUTTON_CYCLE_INCREASE, &onCycleIncrease);
Button buttonCycleDecrease(VOLTAGE_BUTTON_CYCLE_DECREASE, &onCycleDecrease);
///@}

void initKeyboard() {
    analogButtons.add(buttonPeakPressureIncrease);
    analogButtons.add(buttonPeakPressureDecrease);
    analogButtons.add(buttonPlateauPressureIncrease);
    analogButtons.add(buttonPlateauPressureDecrease);
    analogButtons.add(buttonPeepPressureIncrease);
    analogButtons.add(buttonPeepPressureDecrease);
    analogButtons.add(buttonCycleIncrease);
    analogButtons.add(buttonCycleDecrease);

    buttonAlarmOff.attachClick(onAlarmOff);
    buttonStart.attachClick(onStart);
    buttonStop.attachClick(onStop);
}

void keyboardLoop() {
    analogButtons.check();
    buttonAlarmOff.tick();
    buttonStart.tick();
    buttonStop.tick();
}

void calibrateButtons() {
    uint16_t value = analogRead(PIN_CONTROL_BUTTONS);
    Serial.println(value);
    delay(250);
}
