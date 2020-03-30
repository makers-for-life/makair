/*=============================================================================
 * @file alarme.h
 *
 * COVID Respirator
 *
 * @section copyright Copyright
 *
 * Makers For Life
 *
 * This is free and unencumbered software released into the public domain.
 * Anyone is free to copy, modify, publish, use, compile, sell, or distribute
 * this software, either in source code form or as a compiled binary,
 * for any purpose, commercial or non-commercial, and by any means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the software
 * to the public domain. We make this dedication for the benefit of the public
 * at large and to the detriment of our heirs and successors. We intend this
 * dedication to be an overt act of relinquishment in perpetuity of all present
 * and future rights to this software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to [http://unlicense.org]
 *
 * @section descr File description
 *
 * Fichier declarant les fonctionnalitees liees aux alarmes
 */

#pragma once

#include "parameters.h"
#include <stdint.h>

typedef enum AlarmeType
{
    AUCUNE_ALARME,
    ALARME_BASSE,
    ALARME_MOYENNE,
    ALARME_ELEVEE
} AlarmeType;

static const uint32_t PERIODE_ALARME_BASSE = 20000;   // 20s
static const uint32_t PERIODE_ALARME_MOYENNE = 20000; // 20s
static const uint32_t PERIODE_ALARME_ELEVEE = 11000;  // 11s

static const uint32_t DUREE_BIP = 100;
static const uint32_t DUREE_BEEP = 250;

static const uint16_t FREQUENCE_BIP = 440;  // La
static const uint16_t FREQUENCE_BEEP = 494; // Si

static const uint32_t DELAI_UNMUTE_ALARME = 120000; // 120s

/*
 * Fonction d'initialisation des alarmes. À appeler dans setup
 */
void setupAlarmes();

/*
 * Fonction de notification d'une alarme
 */
void notifyAlarme(AlarmeType alarme);

/*
 * Coupe le son d'une alarme
 */
void muteAlarme();

/*
 * Remet le son d'une alarme
 */
void unmuteAlarme();

/*
 * retourne true si les alarmes sont muted
 */
bool isAlarmeMuted();

/*
 * Fonction de raffraichissement de l'alarme. Doit être appelee dans loop
 */
void loopAlarmes();
