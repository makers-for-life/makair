/*=============================================================================
 * @file display.cpp
 *
 * COVID Respirator
 *
 * @section copyright Copyright
 *
 * Makers For Life
 *
 * @section descr File description
 *
 * This file implements the display features
 */

// INCLUDES ===================================================================

// Associated header
#include "display.h"

// External
#include <LiquidCrystal.h>

// Internal
#include "parameters.h"

// INITIALISATION =============================================================

static LiquidCrystal screen(PIN_LCD_RS, PIN_LCD_EN, PIN_LCD_D4, PIN_LCD_D5, PIN_LCD_D6, PIN_LCD_D7);

// FUNCTIONS ==================================================================

void startScreen()
{
    switch (screenSize)
    {
    case ScreenSize::CHARS_20:
    {
        screen.begin(20, 2);
        break;
    }
    default:
    {
        // Default case is ScreenSize::CHARS_16
        screen.begin(16, 2);
    }
    }
}

void displayEveryRespiratoryCycle(int peakPressure, int plateauPressure, int peep)
{
    screen.setCursor(0, 0);

    switch (screenSize)
    {
    case ScreenSize::CHARS_20:
    {
        screen.print("pc=");
        screen.print(peakPressure);
        screen.print("/pp=");
        screen.print(plateauPressure);
        screen.print("/pep=");
        screen.print(peep);
        screen.print("  ");
        break;
    }
    default:
    {
        // Default case is ScreenSize::CHARS_16
        screen.print("pc");
        screen.print(peakPressure);
        screen.print("/pp");
        screen.print(plateauPressure);
        screen.print("/pep");
        screen.print(peep);
        screen.print("  ");
    }
    }
}

void displayDuringCycle(int cyclesPerMinute, int maxPlateauPressure, int peep, int currentPressure)
{
    screen.setCursor(0, 1);

    switch (screenSize)
    {
    case ScreenSize::CHARS_20:
    {
        screen.print("c=");
        screen.print(cyclesPerMinute);
        screen.print("/pl=");
        screen.print(maxPlateauPressure);
        screen.print("/pep=");
        screen.print(peep);
        screen.print("|");
        screen.print(currentPressure);
        break;
    }
    default:
    {
        // Default case is ScreenSize::CHARS_16
        screen.print("c");
        screen.print(cyclesPerMinute);
        screen.print("/pl");
        screen.print(maxPlateauPressure);
        screen.print("/pep");
        screen.print(peep);
    }
    }
}
