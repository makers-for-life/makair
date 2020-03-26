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
 * Fichier définissant les fonctionnalités liées au debug
 */

#pragma once

#include "config.h"

/*
 * Tout d'abord une macro generique permettant d'expanser du code si
 * DEBUG est defini. A utiliser pour un statement seul.
 */
#ifdef DEBUG
#define DBG_DO(statement) statement
#else
#define DBG_DO(statement)
#endif

/*
 * La macro employee au debut de la boucle de controle pour afficher
 * le nombre de centiemes de seconde par cycle et le nombre de centiemes
 * de secondes par inspiration.
 */
#ifdef DEBUG
#define DBG_AFFICHE_CSPCYCLE_CSPINSPI(csparcycle, csparinspi) \
  Serial.println();                                           \
  Serial.println("------ Starting cycle ------");             \
  Serial.print("nbreCentiemeSecParCycle = ");                 \
  Serial.println(csparcycle);                                 \
  Serial.print("nbreCentiemeSecParInspi = ");                 \
  Serial.println(csparinspi);
#else
#define DBG_AFFICHE_CSPCYCLE_CSPINSPI(cspc, cspi)
#endif

/*
 * macro d'affichage des consignes.
 */
#ifdef DEBUG
#define DBG_AFFICHE_CONSIGNES(csgNBcycle, csgOuverture, csgPPEP, csgPPM)  \
  Serial.print("consigneNbCycle = ");                                     \
  Serial.println(csgNBcycle);                                             \
  Serial.print("consigneOuverture = ");                                   \
  Serial.println(csgOuverture);                                           \
  Serial.print("consignePressionPEP = ");                                 \
  Serial.println(csgPPEP);                                                \
  Serial.print("consignePressionPlateauMax = ");                          \
  Serial.println(csgPPM);
#else
#define DBG_AFFICHE_CONSIGNES(csgNBcycle, csgOuverture, csgPPEP, csgPPM)
#endif

/*
 * Macro pour l'alerte de mise en securite pression crete
 */
#ifdef DEBUG
#define DBG_PRESSION_CRETE(centieme, periode)                           \
if (centieme % periode) {                                               \
  Serial.println("Mise en securite : pression crete trop importante");  \
}
#else
#define DBG_PRESSION_CRETE(centieme, periode)
#endif

/*
 * Macro pour l'alerte de mise en securite pression plateau
 */
#ifdef DEBUG
#define DBG_PRESSION_PLATEAU(centieme, periode)                           \
if (centieme % periode) {                                                 \
  Serial.println("Mise en securite : pression plateau trop importante");  \
}
#else
#define DBG_PRESSION_PLATEAU(centieme, periode)
#endif

/*
 * Macro pour l'alerte de mise en securite pression PEP
 */
#ifdef DEBUG
#define DBG_PRESSION_PEP(centieme, periode)                                               \
if (centieme % periode) {                                                                 \
  Serial.println("Mise en securite : pression d'expiration positive (PEP) trop faible");  \
}
#else
#define DBG_PRESSION_PEP(centieme, periode)
#endif

/*
 * Macro pour le message phase + pression
 */
#ifdef DEBUG
#define DBG_PHASE_PRESSION(centieme, periode, phase, pression)  \
if (centieme % periode == 0) {                                  \
  Serial.print("Phase : ");                                     \
  Serial.println(phase);                                        \
  Serial.print("Pression : ");                                  \
  Serial.println(pression);                                     \
}
#else
#define DBG_PHASE_PRESSION(centieme, periode, phase, pression)
#endif
