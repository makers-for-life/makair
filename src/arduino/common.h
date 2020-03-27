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
static const int16_t ANGLE_OUVERTURE_MINI = 8;
static const int16_t ANGLE_OUVERTURE_MAXI = 45;
static const int16_t ANGLE_FERMETURE = 90;

// multiplicateur à modifier pour inverser les angles (en cas de suppression de l'engrenage)
static const int16_t ANGLE_MULTIPLICATEUR = 1;

// borne pour le capteur de pression
static const int16_t CAPT_PRESSION_MINI = 0; // a adapter lors de la calibration
static const int16_t CAPT_PRESSION_MAXI = 800; // on ne va pas jusqu'à 1024 à cause de la saturation de l'AOP -> à
         // adapter avec meilleur AOP

// entrées-sorties
#if defined(__AVR_ATmega328P__)
/* Sur le prototype Arduino Uno */
static const int16_t PIN_CAPTEUR_PRESSION   = A4;
static const int16_t PIN_SERVO_BLOWER       = 4; // D4
static const int16_t PIN_SERVO_PATIENT      = 2; // D2
static const int16_t BTN_NOMBRE_CYCLE_MINUS = A3;
static const int16_t BTN_NOMBRE_CYCLE_PLUS  = A2;
static const int16_t rs = 7, en = 8, d4 = 9, d5 = 10, d6 = 11, d7 = 12;
static const int16_t PIN_ANALOG_KEYBOARD    = A0;
#else
#error "Carte non supportee"
#endif

// contrôle de l'écran LCD
static const ScreenSize screenSize{ScreenSize::CHARS_20};
static const int16_t LCD_UPDATE_PERIOD = 20; // période (en centièmes de secondes) de mise à jour du feedback des consignes sur le LCD
static LiquidCrystal screen(rs, en, d4, d5, d6, d7);

/*! This enum defines the 4 phases of the respiratory cycle
 *  -   Push inhalation:
 *      +   Send air until the peak pressure is reached
 *      +   The blower valve is open so the air stream goes to the patient's lungs
 *  -   Hold inhalation:
 *      +   Once the peak pressure is reached, the pressure is hold to the plateau pressure
 *          The pressure has been decreasing for more than 0.1 s.
 *      +   The 2 valves are hold closed
 *  -   Exhalation:
 *      +   The patient exhales
 *  -   Hold exhalation:
 *      +   Valves stay closed as the pressure is below the PEEP
 */
enum CyclePhases
{
  INHALATION,
  PLATEAU,
  EXHALATION,
  HOLD_EXHALATION
};

// minimums et maximums possible des paramètres modifiables à l'exécution
static const int16_t BORNE_SUP_PRESSION_CRETE = 70; // arbitraire
static const int16_t BORNE_INF_PRESSION_CRETE = 10; // arbitraire
static const int16_t BORNE_SUP_PRESSION_PLATEAU = 30; // PP MAX SDRA = 30
static const int16_t BORNE_INF_PRESSION_PLATEAU = 10; // arbitraire
static const int16_t BORNE_SUP_PRESSION_PEP = 30; // PP MAX = 30, or PEP < PP
static const int16_t BORNE_INF_PRESSION_PEP = 5; // arbitraire mais > 0

static const int16_t BORNE_SUP_CYCLE = 35; // demande medical
static const int16_t BORNE_INF_CYCLE = 5;  // demande medical

// durée d'appui des boutons (en centièmes de secondes) avant prise en compte
static const int16_t MAINTIEN_PARAMETRAGE = 21;

// valeurs de sécurité pour les actionneurs
static const int16_t secu_coupureBlower =
    ANGLE_FERMETURE - ANGLE_OUVERTURE_MAXI;
static const int16_t secu_ouvertureExpi =
    ANGLE_FERMETURE - ANGLE_OUVERTURE_MAXI;

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

static AnalogButtons buttons = AnalogButtons(PIN_ANALOG_KEYBOARD);

#define ANALOG_PIN PIN_ANALOG_KEYBOARD

static const int16_t BTN_FREE2                       = 913;
static const int16_t BTN_FREE1                       = 821;
static const int16_t BTN_ALARM_OFF                   = 745;
static const int16_t BTN_ALARM_ON                    = 607;
static const int16_t BTN_CYCLE_MINUS                 = 509;
static const int16_t BTN_CYCLE_PLUS                  = 413;
static const int16_t BTN_PRESSION_PEP_MINUS          = 292;
static const int16_t BTN_PRESSION_PEP_PLUS           = 215;
static const int16_t BTN_PRESSION_PLATEAU_MINUS      = 109;
static const int16_t BTN_PRESSION_PLATEAU_PLUS       =   0;

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
