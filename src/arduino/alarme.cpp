/*=============================================================================
 * @file alarme.cpp
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
 * Fichier definissant les fonctionnalitees liees aux alarmes
 */

#include "alarme.h"
#include "debug.h"
#include <LightDimmer.h>
#include <ScheduleTable.h>

/*
 * Schedule table pour l'alarme ALARME_BASSE
 */
static SchedTable<1> stAlarmeBasse(PERIODE_ALARME_BASSE);

/*
 * Schedule table pour l'alarme ALARME_MOYENNE
 */
static SchedTable<2> stAlarmeMoyenne(PERIODE_ALARME_MOYENNE);

/*
 * Schedule table pour l'alarme ALARME_ELEVEE
 */
static SchedTable<3> stAlarmeElevee(PERIODE_ALARME_ELEVEE);

/*
 * Schedule table pour unmute une alarm en mute preventif
 */
static SchedTable<1> stUnmute(DELAI_UNMUTE_ALARME + 1);

/*
 * LightDimmer pour les deux LEDs
 */
#ifdef YELLOW_LED_PIN_EXISTS
LightDimmerSoft yellowLED;
#endif

#ifdef RED_LED_PIN_EXISTS
LightDimmerSoft redLED;
#endif

/*
 * Mute boolean to stop sound
 */
static bool muted = false;

/*
 * Actions des alarmes
 */
void lowAlarmLEDAndSoundStartEP()
{
    DBG_DO(Serial.println("lowAlarmLEDAndSoundStartEP");)
#ifdef YELLOW_LED_PIN_EXISTS
    yellowLED.on();
#endif
#ifdef BEEPER_PIN_EXISTS()
    if (!muted)
    {
        tone(PIN_BEEPER, FREQUENCE_BEEP, DUREE_BEEP);
    }
#endif
}

void mediumAlarmBLinkAndFirstSoundEP()
{
    DBG_DO(Serial.println("mediumAlarmBLinkAndFirstSoundEP");)
#ifdef YELLOW_LED_PIN_EXISTS
    yellowLED.startBlink();
#endif
#ifdef BEEPER_PIN_EXISTS()
    if (!muted)
    {
        tone(PIN_BEEPER, FREQUENCE_BEEP, DUREE_BEEP);
    }
#endif
}

void mediumAlarmSecondSoundEP()
{
    DBG_DO(Serial.println("mediumAlarmSecondSoundEP");)
#ifdef BEEPER_PIN_EXISTS()
    if (!muted)
    {
        tone(PIN_BEEPER, FREQUENCE_BEEP, DUREE_BEEP);
    }
#endif
}

void highAlarmLEDAndFirstSoundEP()
{
    DBG_DO(Serial.println("HighAlarmLEDAndFirstSoundEP");)
#ifdef RED_LED_PIN_EXISTS
    redLED.on();
#endif
#ifdef BEEPER_PIN_EXISTS()
    if (!muted)
    {
        tone(PIN_BEEPER, FREQUENCE_BIP, DUREE_BIP);
    }
#endif
}

void highAlarmSecondSoundEP()
{
    DBG_DO(Serial.println("HighAlarmSecondSoundEP");)
#ifdef BEEPER_PIN_EXISTS()
    if (!muted)
    {
        tone(PIN_BEEPER, FREQUENCE_BIP, DUREE_BIP);
    }
#endif
}

void highAlarmThirdSoundEP()
{
    DBG_DO(Serial.println("highAlarmThirdSoundEP");)
#ifdef BEEPER_PIN_EXISTS()
    if (!muted)
    {
        tone(PIN_BEEPER, FREQUENCE_BEEP, DUREE_BEEP);
    }
#endif
}

/*
 * Coupe le son d'une alarme
 */
void muteAlarme()
{
    muted = true;
    if (!stAlarmeBasse.isRunning() && !stAlarmeMoyenne.isRunning() && !stAlarmeElevee.isRunning())
    {
        Serial.println("Mute Pending");
        stUnmute.start(1);
    }
}

/*
 * Remet le son d'une alarme
 */
void unmuteAlarme() { muted = false; }

/*
 * retourne true si les alarmes sont muted
 */
bool isAlarmeMuted() { return muted; }

/*
 * Remet le son si pas de mute anticipe en cours
 */
void unmuteIfNotPending()
{
    if (!stUnmute.isRunning())
    {
        unmuteAlarme();
    }
}

void unmuteAlarmEP()
{
    DBG_DO(Serial.println("unmuteAlarmEP");)
    unmuteAlarme();
}

/*
 * Fonction d'initialisation des alarmes. À appeler dans setup
 */
void setupAlarmes()
{
#ifdef YELLOW_LED_PIN_EXISTS
    yellowLED.begin(PIN_YELLOW_LED, HIGH);
#endif
#ifdef RED_LED_PIN_EXISTS
    redLED.begin(PIN_RED_LED, HIGH);
#endif
    /* expiry point de la schedule table de l'alarme basse */
    stAlarmeBasse.at(0, lowAlarmLEDAndSoundStartEP);
    /* expiry points de la schedule table de l'alarme moyenne */
    stAlarmeMoyenne.at(0, mediumAlarmBLinkAndFirstSoundEP);
    stAlarmeMoyenne.at(1000, mediumAlarmSecondSoundEP);
    /* expiry points de la schedule table de l'alarme elevee */
    stAlarmeElevee.at(0, highAlarmLEDAndFirstSoundEP);
    stAlarmeElevee.at(1000, highAlarmSecondSoundEP);
    stAlarmeElevee.at(2000, highAlarmThirdSoundEP);
    /* expiry point du desenclenchement du mute */
    stUnmute.at(DELAI_UNMUTE_ALARME, unmuteAlarmEP);
}

/*
 * Fonction de notification d'une alarme
 */
void notifyAlarme(AlarmeType alarme)
{
    switch (alarme)
    {
    case ALARME_BASSE:
        if (!stAlarmeMoyenne.isRunning() && !stAlarmeElevee.isRunning())
        {
            stAlarmeBasse.start();
            unmuteIfNotPending();
        }
        break;
    case ALARME_MOYENNE:
        if (!stAlarmeElevee.isRunning())
        {
            stAlarmeBasse.stop();
#ifdef YELLOW_LED_PIN_EXISTS
            yellowLED.off();
#endif
            Serial.println("demarre alarme moyenne");
            stAlarmeMoyenne.start();
            unmuteIfNotPending();
        }
        break;
    case ALARME_ELEVEE:
        stAlarmeMoyenne.stop();
        stAlarmeBasse.stop();
#ifdef YELLOW_LED_PIN_EXISTS
        yellowLED.off();
#endif
        stAlarmeElevee.start();
        unmuteIfNotPending();
        break;
    case AUCUNE_ALARME:
        stAlarmeBasse.stop();
        stAlarmeMoyenne.stop();
        stAlarmeElevee.stop();
#ifdef YELLOW_LED_PIN_EXISTS
        yellowLED.off();
#endif
#ifdef RED_LED_PIN_EXISTS
        redLED.off();
#endif
        unmuteIfNotPending();
        break;
    default:
        /* It's a bad habit of passing off arguments that are out of bounds! */
        break;
    }
}

/*
 * Fonction de raffraichissement de l'alarme. Doit être appelee dans loop
 */
void loopAlarmes()
{
    ScheduleTable::update();
    LightDimmer::update();
}
