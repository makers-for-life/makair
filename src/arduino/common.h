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

// External
#include <AnalogButtons.h>
#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Servo.h>

// Internal
#include "affichage.h"

// PARAMETERS =================================================================

// amplitude radiale des servomoteurs
const int ANGLE_OUVERTURE_MINI = 8;
const int ANGLE_OUVERTURE_MAXI = 45;

// multiplicateur à modifier pour inverser les angles (en cas de suppression de l'engrenage)
const int ANGLE_MULTIPLICATEUR = 1;

// borne pour le capteur de pression
const int CAPT_PRESSION_MINI = 0; // a adapter lors de la calibration
const int CAPT_PRESSION_MAXI = 800; // on ne va pas jusqu'à 1024 à cause de la saturation de l'AOP -> à adapter avec meilleur AOP

// entrées-sorties
const int PIN_CAPTEUR_PRESSION = A4;
const int PIN_SERVO_BLOWER = 4; // D4
const int PIN_SERVO_PATIENT = 2; // D2
const int BTN_NOMBRE_CYCLE_MINUS = A3;
const int BTN_NOMBRE_CYCLE_PLUS = A2;

// contrôle de l'écran LCD
const int rs = 7, en = 8, d4 = 9, d5 = 10, d6 = 11, d7 = 12;
const ScreenSize screenSize{ScreenSize::CHARS_20};
const int LCD_UPDATE_PERIOD = 20; // période (en centièmes de secondes) de mise à jour du feedback des consignes sur le LCD

// phases possibles du cycle
const int PHASE_PUSH_INSPI = 1; // pousée d'inspiration : on envoie l'air jusqu'à la pression crête paramétrée : valve blower ouverte à consigne, flux d'air vers le patient
const int PHASE_HOLD_INSPI = 2; // plateau d'inspiration : on a depassé la pression crête, la pression descend depuis plus d'un 1/10sec (arbitraire EFL) : 2 valves fermées
const int PHASE_EXPIRATION = 3; // expiration : flux d'air vers l'extérieur, air patient vers l'extérieur
const int PHASE_HOLD_EXPI  = 4; // expiration bloquée : les valves sont fermées car la pression est en dessous de la PEP

// minimums et maximums possible des paramètres modifiables à l'exécution
const int BORNE_SUP_PRESSION_CRETE = 70; // arbitraire
const int BORNE_INF_PRESSION_CRETE = 10; // arbitraire
const int BORNE_SUP_PRESSION_PLATEAU = 30; // PP MAX SDRA = 30
const int BORNE_INF_PRESSION_PLATEAU = 10; // arbitraire
const int BORNE_SUP_PRESSION_PEP = 30; // PP MAX = 30, or PEP < PP
const int BORNE_INF_PRESSION_PEP = 5; // arbitraire mais > 0

const int BORNE_SUP_CYCLE = 35; // demande medical
const int BORNE_INF_CYCLE = 5;  // demande medical

// durée d'appui des boutons (en centièmes de secondes) avant prise en compte
const int MAINTIEN_PARAMETRAGE = 21;

// valeurs de sécurité pour les actionneurs
const int secu_coupureBlower = 90 - ANGLE_OUVERTURE_MAXI;
const int secu_ouvertureExpi = 90 - ANGLE_OUVERTURE_MAXI;

// servomoteur blower : connecte le flux d'air vers le Air Transistor patient ou vers l'extérieur
// 90° → tout est fermé
// entre 45° (90 - ANGLE_OUVERTURE_MAXI) et 82° (90 - ANGLE_OUVERTURE_MINI) → envoi du flux vers l'extérieur
// entre 98° (90 + ANGLE_OUVERTURE_MINI) et 135° (90 + ANGLE_OUVERTURE_MAXI) → envoi du flux vers le Air Transistor patient
Servo blower;

// servomoteur patient : connecte le patient au flux d'air entrant ou à l'extérieur
// 90° → tout est fermé
// entre 45° (90 - ANGLE_OUVERTURE_MAXI) et 82° (90 - ANGLE_OUVERTURE_MINI) → envoi du flux vers le patient
// entre 98° (90 + ANGLE_OUVERTURE_MINI) et 135° (90 + ANGLE_OUVERTURE_MAXI) → échappe l'air du patient vers l'extérieur
Servo patient;


// contrôle de l'écran LCD
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

AnalogButtons buttons = AnalogButtons(A0);

#define ANALOG_PIN A0
const int BTN_FREE2                       = 913;
const int BTN_FREE1                       = 821;
const int BTN_ALARM_OFF                   = 745;
const int BTN_ALARM_ON                    = 607;
const int BTN_CYCLE_MINUS                 = 509;
const int BTN_CYCLE_PLUS                  = 413;
const int BTN_PRESSION_PEP_MINUS          = 292;
const int BTN_PRESSION_PEP_PLUS           = 215;
const int BTN_PRESSION_PLATEAU_MINUS      = 109;
const int BTN_PRESSION_PLATEAU_PLUS       =   0;

// TODO revoir les tensions ici
const uint16_t TENSION_BTN_PRESSION_PLATEAU_PLUS   = BTN_PRESSION_PLATEAU_PLUS;
const uint16_t TENSION_BTN_PRESSION_PLATEAU_MINUS  = BTN_PRESSION_PLATEAU_MINUS;
const uint16_t TENSION_BTN_PEP_PLUS                = BTN_PRESSION_PEP_PLUS;
const uint16_t TENSION_BTN_PEP_MINUS               = BTN_PRESSION_PEP_MINUS;
const uint16_t TENSION_BTN_RB_PLUS                 = BTN_CYCLE_PLUS;
const uint16_t TENSION_BTN_RB_MINUS                = BTN_CYCLE_MINUS;
const uint16_t TENSION_BTN_ALARME_ON               = BTN_ALARM_ON;
const uint16_t TENSION_BTN_ALARME_OFF              = BTN_ALARM_OFF;
const uint16_t TENSION_BTN_VALVE_BLOWER_PLUS       = BTN_FREE1;
const uint16_t TENSION_BTN_VALVE_BLOWER_MINUS      = BTN_FREE2;
const uint16_t TENSION_BTN_VALVE_PATIENT_PLUS      = 971;
const uint16_t TENSION_BTN_VALVE_PATIENT_MINUS     = 1012;
