/*=============================================================================
 * @file parameters.h
 *
 * COVID Respirator
 *
 * @section copyright Copyright
 *
 * Makers For Life
 *
 * @section descr File description
 *
 * This file defines the Makair parameters
 */

#pragma once

// INCLUDES ===================================================================

// External
#include <AnalogButtons.h>
#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Servo.h>

// Internal
#include "affichage.h"
#include "config.h"

// PARAMETERS =================================================================

// amplitude radiale des servomoteurs
// \warning ANGLE_FERMETURE has to be greater than ANGLE_OUVERTURE_MAXI
static const uint16_t ANGLE_OUVERTURE_MINI = 8;
static const uint16_t ANGLE_OUVERTURE_MAXI = 45;
static const uint16_t ANGLE_FERMETURE = 90;

// multiplicateur à modifier pour inverser les angles (en cas de suppression de
// l'engrenage)
static const int16_t ANGLE_MULTIPLICATEUR = 1;

// borne pour le capteur de pression
// a adapter lors de la calibration
static const uint16_t CAPT_PRESSION_MINI = 0;

// on ne va pas jusqu'à 1024 à cause de la saturation de l'AOP
// -> à adapter avec meilleur AOP
static const uint16_t CAPT_PRESSION_MAXI = 800;

// entrées-sorties
#if defined(ARDUINO_AVR_UNO)
/* Sur le prototype Arduino Uno */
static const int16_t PIN_CAPTEUR_PRESSION = A4;
static const int16_t PIN_SERVO_BLOWER = 4;  // D4
static const int16_t PIN_SERVO_PATIENT = 2; // D2
static const int16_t BTN_NOMBRE_CYCLE_MINUS = A3;
static const int16_t BTN_NOMBRE_CYCLE_PLUS = A2;
/* Ecran LCD */
static const int16_t PIN_LCD_RS = 7;
static const int16_t PIN_LCD_EN = 8;
static const int16_t PIN_LCD_D4 = 9;
static const int16_t PIN_LCD_D5 = 10;
static const int16_t PIN_LCD_D6 = 11;
static const int16_t PIN_LCD_D7 = 12;
/* clavier analogique */
static const int16_t PIN_CONTROL_BUTTONS = A0;
#elif defined(ARDUINO_NUCLEO_F411RE)
static const int16_t PIN_CAPTEUR_PRESSION = A1;
static const int16_t PIN_ALARM = D13;
static const int16_t PIN_LED_RED = PC4;
static const int16_t PIN_LED_GREEN = PB13;
static const int16_t PIN_MOTEUR_VENTILATEUR = D5;
static const int16_t PIN_SERVO_BLOWER = D2;
static const int16_t PIN_SERVO_PATIENT = D4;
static const int16_t PIN_SERVO_Y = D3;
static const int16_t PIN_CONTROL_BUTTONS = A0;
static const int16_t PIN_BATTERY = A2;
/* Ecran LCD */
static const int16_t PIN_LCD_RS = D7;
static const int16_t PIN_LCD_EN = D8;
static const int16_t PIN_LCD_D4 = D9;
static const int16_t PIN_LCD_D5 = D10;
static const int16_t PIN_LCD_D6 = D11;
static const int16_t PIN_LCD_D7 = D12;
#else
#error "Carte non supportee"
#endif

// contrôle de l'écran LCD
static const ScreenSize screenSize{ScreenSize::CHARS_20};
// période (en centièmes de secondes) de mise à jour du feedback des
// consignes sur le LCD
static const int16_t LCD_UPDATE_PERIOD = 20;

// Periode de traitement en millisecondes
static const uint32_t PCONTROLLER_COMPUTE_PERIOD = 10;

// minimums et maximums possible des paramètres modifiables à l'exécution
static const uint16_t BORNE_SUP_PRESSION_CRETE = 700;   // arbitraire mmH2O
static const uint16_t BORNE_INF_PRESSION_CRETE = 100;   // arbitraire mmH2O
static const uint16_t BORNE_SUP_PRESSION_PLATEAU = 400; // PP MAX SDRA = 300 mmH20
static const uint16_t BORNE_INF_PRESSION_PLATEAU = 100; // arbitraire mmH2O
static const uint16_t BORNE_SUP_PRESSION_PEP = 300;     // PP MAX = 30, or PEP < PP mmH2O
static const uint16_t BORNE_INF_PRESSION_PEP = 50;      // arbitraire mais > 0 mmH2O

static const uint16_t BORNE_SUP_CYCLE = 35; // demande medical
static const uint16_t BORNE_INF_CYCLE = 5;  // demande medical

// durée d'appui des boutons (en centièmes de secondes) avant prise en compte
static const uint16_t MAINTIEN_PARAMETRAGE = 21;

static const uint16_t TENSION_BTN_PRESSION_P_CRETE_PLUS   = 915;
static const uint16_t TENSION_BTN_PRESSION_P_CRETE_MINUS  = 800;
static const uint16_t TENSION_BTN_PRESSION_PLATEAU_PLUS   = 728;
static const uint16_t TENSION_BTN_PRESSION_PLATEAU_MINUS  = 598;
static const uint16_t TENSION_BTN_PEP_PLUS                = 504;
static const uint16_t TENSION_BTN_PEP_MINUS               = 410;
static const uint16_t TENSION_BTN_CYCLE_PLUS              = 290;
static const uint16_t TENSION_BTN_CYCLE_MINUS             = 215;

const uint16_t TENSION_BTN_ALARME_ON        = 650;
const uint16_t TENSION_BTN_ALARME_OFF       = 450;
const uint16_t TENSION_BTN_ALARME_SILENCE   = 250; 