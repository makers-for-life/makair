/*
    Copyright (C) 2020 Makers For Life
*/
/******************************************************************************
 * @author Makers For Life
 * @file affichage.cpp
 * @brief Display and LCD screen related functions
 *
 * This relies on the LiquidCrytal library (https://github.com/arduino-libraries/LiquidCrystal).
 * LCD screen must have 4 lines of 20 characters.
 *****************************************************************************/

#pragma once

// INCLUDES ===================================================================

// Associated header
#include "affichage.h"

// Internal
#include "common.h"
#include "parameters.h"

// INITIALISATION =============================================================

/// Instance of the screen controller
LiquidCrystal
    screen(PIN_LCD_RS, PIN_LCD_RW, PIN_LCD_EN, PIN_LCD_D4, PIN_LCD_D5, PIN_LCD_D6, PIN_LCD_D7);

// FUNCTIONS ==================================================================

void startScreen()
{
    screen.begin(SCREEN_LINE_LENGTH, SCREEN_LINE_NUMBER);
    screen.setCursor(0, 0);
    screen.print("Initialisation      ");
    screen.setCursor(0, 1);
    screen.print(VERSION);
}

void resetScreen() { screen.clear(); }

void displaySubPhase(CycleSubPhases subPhase)
{
    screen.setCursor(0, 0);
    switch (subPhase)
    {
    case CycleSubPhases::INSPI:
    {
        screen.print("Inhalation          ");
        break;
    }
    case CycleSubPhases::HOLD_INSPI:
    {
        screen.print("Plateau             ");
        break;
    }
    case CycleSubPhases::EXHALE:
    {
        screen.print("Exhalation          ");
        break;
    }
    case CycleSubPhases::HOLD_EXHALE:
    {
        screen.print("Hold exhalation     ");
        break;
    }
    default:
    {
        break;
    }
    }
}

void displayInstantInfo(int peakPressure, int plateauPressure, int peep, int pressure)
{
    screen.setCursor(0, 1);
    char msg[SCREEN_LINE_LENGTH];
    sprintf(msg, "%-4u %-4u %-4u %-4u", peakPressure / 10, plateauPressure / 10, peep / 10,
            pressure / 10);
    screen.print(msg);
}

void displaySettings(int peakPressureMax,
                     int plateauPressureMax,
                     int peepMin,
                     int cyclesPerMinute)
{
    screen.setCursor(0, 3);
    char msg[SCREEN_LINE_LENGTH];
    sprintf(msg, "%-4u %-4u %-4u %-4u", peakPressureMax / 10, plateauPressureMax / 10,
            peepMin / 10, cyclesPerMinute);
    screen.print(msg);
}
