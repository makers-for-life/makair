/*=============================================================================
 * @file respi-prod.h
 *
 * COVID Respirator
 *
 * @section copyright Copyright
 *
 * Makers For Life
 *
 * @section descr File description
 *
 * This file is the header for the respi-prod
 */

#pragma once

#include <stdint.h>

/*
 * respiLoop has to be called in the loop function.
 */
void respiLoop();

/*
 * inc/decConsigneNbCycle increment and decrement the number of cycles
 * for the next breathing cycle
 */
 void incConsigneNbCycle();
 void decConsigneNbCycle();

/*
 * inc/decConsignePressionPEP increment and decrement the pressure of PEP
 * for the next breathing cycle
 */
void incConsignePressionPEP();
void decConsignePressionPEP();

/*
 * inc/decConsignePressionPlateauMax set the maximum plateau pressure
 * for the next breathing cycle.
 */
void incConsignePressionPlateauMax();
void decConsignePressionPlateauMax();
