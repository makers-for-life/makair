/*=============================================================================
 * @file pression.h
 *
 * COVID Respirator
 *
 * @section copyright Copyright
 *
 * Makers For Life
 *
 * @section descr File description
 *
 * Fichier définissant les fonctionnalités liées à l'acquisition ou la simulation
 * du capteur de pression
 */

#pragma once

// Get the measured or simulated pressure for the feedback control
int readPressureSensor(uint16_t centiSec);
