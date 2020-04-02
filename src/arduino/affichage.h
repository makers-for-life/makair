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

// INCLUDES ===================================================================

// External
#include <LiquidCrystal.h>

// Internal
#include "common.h"
#include "parameters.h"

// INITIALISATION =============================================================

extern LiquidCrystal screen;

// FUNCTIONS ==================================================================

//! This function starts the screen
void startScreen();

//! This function resets the screen
void resetScreen();

void displaySubPhase(CycleSubPhases phase);

/*! This function displays the relevant pressures on the HMI screen once every
 *  cycle
 *  N.B.: by default, the screen size is 16 chars
 *
 *  \param peakPressure     The peak pressure [mmH2O]
 *  \param plateauPressure  The plateau pressure [mmH2O]
 *  \param peep             The Positive End Expiratory Pressure (PEEP) [mmH2O]
 *  \param pressure         Current pressure [mmH2O]
 */
void displayEveryRespiratoryCycle(int peakPressure, int plateauPressure, int peep, int pressure);

/*! This function displays the relevant pressures on the HMI screen during the
 *  cycle.
 *  N.B.: by default, the screen size is 16 chars
 *
 *  \param peakPressureMax      PeakPressureMax [mmH2O]
 *  \param maxPlateauPressure   Next maximal plateau pressure [mmH2O]
 *  \param peep                 Next desired Positive End Expiratory Pressure (PEEP) [mmH2O]
 *  \param cyclesPerMinute      Next desired number of cycles per minute
 */
void displayDuringCycle(int peakPressureMax, int plateauPressureMax, int peepMin, int cyclesPerMinute);
