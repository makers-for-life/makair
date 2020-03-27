/*=============================================================================
 * @file variables.h
 *
 * COVID Respirator
 *
 * @section copyright Copyright
 *
 * Makers For Life
 *
 * @section descr File description
 *
 * This file sets the variables
 */

#pragma once

// nombre de cycles par minute (cycle = inspi + plateau + expi)
int16_t consigneNbCycle = 20;
int16_t futureConsigneNbCycle = consigneNbCycle;

// degré d'ouverture de la valve blower (quantité d'air du blower qu'on envoie
// vers le Air Transistor patient)
int16_t consigneOuverture = 45;
int16_t futureConsigneOuverture = consigneOuverture;

// consigne de pression de crête maximum
int16_t consignePressionCrete = 60;

// consigne de pression plateau maximum
int16_t consignePressionPlateauMax = 30;
int16_t futureConsignePressionPlateauMax = consignePressionPlateauMax;

// consigne de pression PEP
int16_t consignePressionPEP = 5;
int16_t futureConsignePressionPEP = consignePressionPEP;

// données pour affichage (du cycle précédent pour ne pas afficher des valeurs
// aberrantes)
int16_t previousPressionCrete = -1;
int16_t previousPressionPlateau = -1;
int16_t previousPressionPep = -1;
