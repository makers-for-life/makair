/*
    Copyright (C) 2020 Makers For Life
*/
/******************************************************************************
 * @author Makers For Life
 * @file screen.cpp
 * @brief Display and LCD screen related functions
 *
 * This relies on the LiquidCrystal library (https://github.com/arduino-libraries/LiquidCrystal).
 * LCD screen must have 4 lines of 20 characters.
 *****************************************************************************/

#pragma once

// INCLUDES ===================================================================

// Associated header
#include "../includes/screen.h"

// Internal
#include "../includes/common.h"
#include "../includes/parameters.h"

// INITIALISATION =============================================================

/// Instance of the screen controller
LiquidCrystal
    screen(PIN_LCD_RS, PIN_LCD_RW, PIN_LCD_EN, PIN_LCD_D4, PIN_LCD_D5, PIN_LCD_D6, PIN_LCD_D7);

// FUNCTIONS ==================================================================

void startScreen() {
    screen.begin(SCREEN_LINE_LENGTH, SCREEN_LINE_NUMBER);
    screen.setCursor(0, 0);
    screen.print("Initialization      ");
    screen.setCursor(0, 1);
    screen.print(VERSION);
}

void resetScreen() { screen.clear(); }

void displaySubPhase(CycleSubPhases subPhase) {
    screen.setCursor(0, 0);

    switch (subPhase) {
    case CycleSubPhases::INSPIRATION: {
        screen.print("Inhalation          ");
        break;
    }
    case CycleSubPhases::HOLD_INSPIRATION: {
        screen.print("Plateau             ");
        break;
    }
    case CycleSubPhases::EXHALE: {
        screen.print("Exhalation          ");
        break;
    }
    case CycleSubPhases::HOLD_EXHALE: {
        screen.print("Hold exhalation     ");
        break;
    }
    default: {
        break;
    }
    }
}

void displayCurrentInformation(uint16_t peakPressure,
                               uint16_t plateauPressure,
                               uint16_t peep,
                               uint16_t pressure) {
    screen.setCursor(0, 1);
    char message[SCREEN_LINE_LENGTH];

    (void)snprintf(message, SCREEN_LINE_LENGTH, "%-4u %-4u %-4u %-4u", peakPressure / 10u,
                   plateauPressure / 10u, peep / 10u, pressure / 10u);

    screen.print(message);
}

void displaySettings(uint16_t peakPressureMax,
                     uint16_t plateauPressureMax,
                     uint16_t peepMin,
                     uint16_t cyclesPerMinute) {
    screen.setCursor(0, 3);
    char message[SCREEN_LINE_LENGTH];

    (void)snprintf(message, SCREEN_LINE_LENGTH, "%-4u %-4u %-4u %-4u", peakPressureMax / 10u,
                   plateauPressureMax / 10u, peepMin / 10u, cyclesPerMinute);

    screen.print(message);
}
