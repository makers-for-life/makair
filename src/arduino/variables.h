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
int consigneNbCycle = 20;
int futureConsigneNbCycle = consigneNbCycle;

// degré d'ouverture de la valve blower (quantité d'air du blower qu'on envoie
// vers le Air Transistor patient)
int consigneOuverture = 45;
int futureConsigneOuverture = consigneOuverture;

// consigne de pression de crête maximum
int consignePressionCrete = 60;

// consigne de pression plateau maximum
int consignePressionPlateauMax = 30;
int futureConsignePressionPlateauMax = consignePressionPlateauMax;

// consigne de pression PEP
int consignePressionPEP = 5;
int futureConsignePressionPEP = consignePressionPEP;

// données pour affichage (du cycle précédent pour ne pas afficher des valeurs
// aberrantes)
int previousPressionCrete = -1;
int previousPressionPlateau = -1;
int previousPressionPep = -1;
