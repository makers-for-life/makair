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

#ifndef __CONFIG_H__
#define __CONFIG_H__

/*
 * Lorsque DEBUG est défini, du code additionnel envoyant des infos sur
 * la ligne serie est compile.
 */
//#define DEBUG

/*
 * Deux versions de LCD sont supportees, les LCD 16 colonnes et 2 lignes (16x2)
 * et les LCD 20 colonnes et 4 lignes (20x4).
 * Lorsque LCD_20_CHARS est defini, un LCD 20x4 est utilise. Sinon il
 * s'agit d'un LCD 16x2
 */
//#define LCD_20_CHARS

/*
 * Lorsque SIMULATION est defini, l'information n'est pas lue du capteur de
 * pression mais generee logiciellement
 */
//#define SIMULATION

#endif /* __CONFIG_H_ */
