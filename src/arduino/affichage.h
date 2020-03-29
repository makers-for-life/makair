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

void displayConsigneServo(int blowerCommand, int yCommand, int patientCommand);
