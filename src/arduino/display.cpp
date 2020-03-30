/*=============================================================================
 * @file display.cpp
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
