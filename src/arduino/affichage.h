/*=============================================================================
 * @file affichage.h
 *
 * COVID Respirator
 *
 * @section copyright Copyright
 *
 * Makers For Life
 *
 * @section descr File description
 *
 * Fichier définissant les fonctionnalitées liés à l'affichage
 */

#pragma once

// FORWARD DECLARATIONS =======================================================
class LiquidCrystal;

// STRUCTURES =================================================================

//! This enum lists the different screen sizes
enum ScreenSize
{
    CHARS_16,
    CHARS_20
};

// FUNCTIONS ==================================================================

/*! This function displays the relevant pressures on the HMI screen once every
 *  cycle
 *  N.B.: by default, the screen size is 16 chars
 *
 *  \param screen           The HMI screen
 *  \param ScreenSize       The screen size
 *  \param peakPressure     The peak pressure
 *  \param plateauPressure  The plateau pressure
 *  \param peep             The positive End Expiratory Pressure (PEEP)
 */
void displayEveryCycle(LiquidCrystal& screen,
                       ScreenSize size,
                       int peakPressure,
                       int plateauPressure,
                       int peep);

/*! This function displays the relevant pressures on the HMI screen during the
 *  cycle.
 *  N.B.: by default, the screen size is 16 chars
 *
 *  \param screen               The HMI screen
 *  \param ScreenSize           The screen size
 *  \param cyclesPerMinute      Next desired number of cycles per minute
 *  \param maxPlateauPressure   Next maximal plateau pressure
 *  \param peep                 Next desired Positive End Expiratory Pressure (PEEP)
 *  \param currentPressure      Current pressure
 */
void displayDuringCycle(LiquidCrystal& screen,
                        ScreenSize size,
                        int cyclesPerMinute,
                        int maxPlateauPressure,
                        int peep,
                        int currentPressure);
