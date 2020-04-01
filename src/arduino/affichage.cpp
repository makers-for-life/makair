/*=============================================================================
 * @file affichage.cpp
 *
 * COVID Respirator
 *
 * @section copyright Copyright
 *
 * Makers For Life
 *
 * @section descr File description
 *
 * Fichier implémentant les fonctionnalitées liés à l'affichage
 */

// INCLUDES ===================================================================

// Associated header
#include "affichage.h"

// External
#include <LiquidCrystal.h>

// Internal
#include "common.h"
#include "parameters.h"

// INITIALISATION =============================================================

static LiquidCrystal screen(PIN_LCD_RS, PIN_LCD_RW, PIN_LCD_EN, PIN_LCD_D4, PIN_LCD_D5, PIN_LCD_D6, PIN_LCD_D7);

// FUNCTIONS ==================================================================

void startScreen()
{
    switch (screenSize)
    {
    case ScreenSize::CHARS_16:
    {
        screen.begin(16, 2);
        break;
    }
    case ScreenSize::CHARS_20:
    {
        screen.begin(20, 4);
        screen.print(VERSION);
        break;
    }
    default:
    {
        screen.begin(16, 2);
    }
    }
}

void displaySubPhase(CycleSubPhases subPhase) {
    screen.setCursor(0, 0);
    switch(subPhase)
    {
    case CycleSubPhases::INSPI:
    {
        screen.print("Inhalation          ");
        break;
    }
    case CycleSubPhases::HOLD_INSPI :
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

void displayEveryRespiratoryCycle(int peakPressure, int plateauPressure, int peep, int pressure)
{

    switch (screenSize)
    {
    case ScreenSize::CHARS_16:
    {
        screen.setCursor(0, 1);
        screen.print(peakPressure / 10);
        screen.print("  ");
        screen.print(plateauPressure / 10);
        screen.print("  ");
        screen.print(peep / 10);
        screen.print("  ");
        screen.print(pressure / 10);

        break;
    }
    case ScreenSize::CHARS_20:
    {
        screen.setCursor(0, 1);
        char msg[20];
        sprintf(msg, "%-4u %-4u %-4u %-4u", peakPressure / 10, plateauPressure / 10, peep / 10, pressure / 10);
        screen.print(msg);
        break;
    }
    default:
    {
        screen.print(peakPressure / 10);
        screen.print("  ");
        screen.print(plateauPressure / 10);
        screen.print("  ");
        screen.print(peep / 10);
        screen.print("  ");
        screen.print(pressure / 10);
    }
    }
}

void displayDuringCycle(int peakPressureMax, int plateauPressureMax, int peepMin, int cyclesPerMinute)
{
    screen.setCursor(0, 3);

    switch (screenSize)
    {
    case ScreenSize::CHARS_16:
    {
        // screen.print("c");
        // screen.print(cyclesPerMinute);
        // screen.print("/pl");
        // screen.print(maxPlateauPressure);
        // screen.print("/pep");
        // screen.print(peep);
        break;
    }
    case ScreenSize::CHARS_20:
    {

        char msg[20];
        sprintf(msg, "%-4u %-4u %-4u %-4u", peakPressureMax / 10, plateauPressureMax / 10, peepMin / 10, cyclesPerMinute);
        screen.print(msg);
        break;
    }
    default:
    {
        // screen.print("c");
        // screen.print(cyclesPerMinute);
        // screen.print("/pl");
        // screen.print(maxPlateauPressure);
        // screen.print("/pep");
        // screen.print(peep);
    }
    }
}
