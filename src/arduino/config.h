/*=============================================================================
 * @file config.h
 *
 * COVID Respirator
 *
 * @section copyright Copyright
 *
 * Makers For Life
 *
 * @section descr File description
 *
 * Fichier de configuration pour les differentes compilations conditionelles
 * possibles.
 */

#pragma once

// Available modes
#define MODE_PROD 1          // Ventilator
#define MODE_QUALIFICATION 2 // Test electrical wiring

// Define current mode
#define MODE MODE_PROD

/*
 * Lorsque DEBUG est defini, du code additionnel envoyant des infos sur
 * la ligne serie est compile.
 */
//#define DEBUG

/*
 * Lorsque SIMULATION est defini, l'information n'est pas lue du capteur de
 * pression mais generee logiciellement
 */
//  #define SIMULATION

//#define __AVR_ATmega328P__
