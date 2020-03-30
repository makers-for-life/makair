/*=============================================================================
 * @file display.h
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
 * This file defines the display features
 */

#pragma once

// FORWARD DECLARATIONS =======================================================
class LiquidCrystal;

// STRUCTURES =================================================================

/*! This enum lists the different screen sizes
 *  2 LCD versions are supported:
 *  -   LCD with 16 columns and 2 rows (16x2)
 *  -   LCD with 20 columns and 4 rows (20x4)
 */
enum ScreenSize
{
    CHARS_16,
    CHARS_20
};

// FUNCTIONS ==================================================================

//! This function starts the screen
void startScreen();

/*! This function displays the relevant pressures on the HMI screen once every
 *  cycle
 *  N.B.: by default, the screen size is 16 chars
 *
 *  \param peakPressure     The peak pressure
 *  \param plateauPressure  The plateau pressure
 *  \param peep             The Positive End Expiratory Pressure (PEEP)
 */
void displayEveryRespiratoryCycle(int peakPressure, int plateauPressure, int peep);

/*! This function displays the relevant pressures on the HMI screen during the
 *  cycle.
 *  N.B.: by default, the screen size is 16 chars
 *
 *  \param cyclesPerMinute      Next desired number of cycles per minute
 *  \param maxPlateauPressure   Next maximal plateau pressure
 *  \param peep                 Next desired Positive End Expiratory Pressure (PEEP)
 *  \param currentPressure      Current pressure
 */
void displayDuringCycle(int cyclesPerMinute, int maxPlateauPressure, int peep, int currentPressure);
