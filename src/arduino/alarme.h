/*=============================================================================
 * @file alarme.h
 *
 * COVID Respirator
 *
 * @section copyright Copyright
 *
 * Makers For Life
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
