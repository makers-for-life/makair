/*=============================================================================
 * @file common.h
 *
 * COVID Respirator
 *
 * @section copyright Copyright
 *
 * Makers For Life
 *
 * @section descr File description
 *
 * This file sets the parameters
 */

// INCLUDES ===================================================================

#pragma once

// External
#include <AnalogButtons.h>
#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Servo.h>

// Internal
#include "affichage.h"

// PARAMETERS =================================================================

// amplitude radiale des servomoteurs
static const int ANGLE_OUVERTURE_MINI = 8;
static const int ANGLE_OUVERTURE_MAXI = 45;

// multiplicateur à modifier pour inverser les angles (en cas de suppression de l'engrenage)
static const int ANGLE_MULTIPLICATEUR = 1;

// borne pour le capteur de pression
static const int CAPT_PRESSION_MINI = 0; // a adapter lors de la calibration
static const int CAPT_PRESSION_MAXI = 800; // on ne va pas jusqu'à 1024 à cause de la saturation de l'AOP -> à
         // adapter avec meilleur AOP

// entrées-sorties
static const int PIN_CAPTEUR_PRESSION = A4;
static const int PIN_SERVO_BLOWER = 4; // D4
static const int PIN_SERVO_PATIENT = 2; // D2
static const int BTN_NOMBRE_CYCLE_MINUS = A3;
static const int BTN_NOMBRE_CYCLE_PLUS = A2;

// contrôle de l'écran LCD
static const int rs = 7, en = 8, d4 = 9, d5 = 10, d6 = 11, d7 = 12;
static const ScreenSize screenSize{ScreenSize::CHARS_20};
static const int LCD_UPDATE_PERIOD = 20; // période (en centièmes de secondes) de mise à jour du feedback des consignes sur le LCD

// phases possibles du cycle
static const int PHASE_PUSH_INSPI =
    1; // pousée d'inspiration : on envoie l'air jusqu'à la pression crête
       // paramétrée : valve blower ouverte à consigne, flux d'air vers le
       // patient
static const int PHASE_HOLD_INSPI =
    2; // plateau d'inspiration : on a depassé la pression crête, la pression
       // descend depuis plus d'un 1/10sec (arbitraire EFL) : 2 valves fermées
static const int PHASE_EXPIRATION =
    3; // expiration : flux d'air vers l'extérieur, air patient vers l'extérieur
static const int PHASE_HOLD_EXPI =
    4; // expiration bloquée : les valves sont fermées car la pression est en
       // dessous de la PEP

// minimums et maximums possible des paramètres modifiables à l'exécution
static const int BORNE_SUP_PRESSION_CRETE = 70; // arbitraire
static const int BORNE_INF_PRESSION_CRETE = 10; // arbitraire
static const int BORNE_SUP_PRESSION_PLATEAU = 30; // PP MAX SDRA = 30
static const int BORNE_INF_PRESSION_PLATEAU = 10; // arbitraire
static const int BORNE_SUP_PRESSION_PEP = 30; // PP MAX = 30, or PEP < PP
static const int BORNE_INF_PRESSION_PEP = 5; // arbitraire mais > 0

static const int BORNE_SUP_CYCLE = 35; // demande medical
static const int BORNE_INF_CYCLE = 5;  // demande medical

// durée d'appui des boutons (en centièmes de secondes) avant prise en compte
static const int MAINTIEN_PARAMETRAGE = 21;

// valeurs de sécurité pour les actionneurs
static const int secu_coupureBlower = 90 - ANGLE_OUVERTURE_MAXI;
static const int secu_ouvertureExpi = 90 - ANGLE_OUVERTURE_MAXI;

// servomoteur blower : connecte le flux d'air vers le Air Transistor patient ou vers l'extérieur
// 90° → tout est fermé
// entre 45° (90 - ANGLE_OUVERTURE_MAXI) et 82° (90 - ANGLE_OUVERTURE_MINI) → envoi du flux vers l'extérieur
// entre 98° (90 + ANGLE_OUVERTURE_MINI) et 135° (90 + ANGLE_OUVERTURE_MAXI) → envoi du flux vers le Air Transistor patient
static Servo blower;

// servomoteur patient : connecte le patient au flux d'air entrant ou à l'extérieur
// 90° → tout est fermé
// entre 45° (90 - ANGLE_OUVERTURE_MAXI) et 82° (90 - ANGLE_OUVERTURE_MINI) → envoi du flux vers le patient
// entre 98° (90 + ANGLE_OUVERTURE_MINI) et 135° (90 + ANGLE_OUVERTURE_MAXI) → échappe l'air du patient vers l'extérieur
static Servo patient;

static AnalogButtons buttons = AnalogButtons(A0);

#define ANALOG_PIN A0
static const int BTN_FREE2                       = 913;
static const int BTN_FREE1                       = 821;
static const int BTN_ALARM_OFF                   = 745;
static const int BTN_ALARM_ON                    = 607;
static const int BTN_CYCLE_MINUS                 = 509;
static const int BTN_CYCLE_PLUS                  = 413;
static const int BTN_PRESSION_PEP_MINUS          = 292;
static const int BTN_PRESSION_PEP_PLUS           = 215;
static const int BTN_PRESSION_PLATEAU_MINUS      = 109;
static const int BTN_PRESSION_PLATEAU_PLUS       =   0;

// TODO revoir les tensions ici
static const uint16_t TENSION_BTN_PRESSION_PLATEAU_PLUS   = BTN_PRESSION_PLATEAU_PLUS;
static const uint16_t TENSION_BTN_PRESSION_PLATEAU_MINUS  = BTN_PRESSION_PLATEAU_MINUS;
static const uint16_t TENSION_BTN_PEP_PLUS                = BTN_PRESSION_PEP_PLUS;
static const uint16_t TENSION_BTN_PEP_MINUS               = BTN_PRESSION_PEP_MINUS;
static const uint16_t TENSION_BTN_RB_PLUS                 = BTN_CYCLE_PLUS;
static const uint16_t TENSION_BTN_RB_MINUS                = BTN_CYCLE_MINUS;
static const uint16_t TENSION_BTN_ALARME_ON               = BTN_ALARM_ON;
static const uint16_t TENSION_BTN_ALARME_OFF              = BTN_ALARM_OFF;
static const uint16_t TENSION_BTN_VALVE_BLOWER_PLUS       = BTN_FREE1;
static const uint16_t TENSION_BTN_VALVE_BLOWER_MINUS      = BTN_FREE2;
static const uint16_t TENSION_BTN_VALVE_PATIENT_PLUS      = 971;
static const uint16_t TENSION_BTN_VALVE_PATIENT_MINUS     = 1012;
