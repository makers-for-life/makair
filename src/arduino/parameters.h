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
#include "config.h"

// STRUCTURES =================================================================

/*! This enum lists the different screen sizes
 *  2 LCD versions are supported:
 *  -   LCD with 16 columns and 2 rows (16x2)
 *  -   LCD with 20 columns and 4 rows (20x4)
 */
enum ScreenSize
{
    CHARS_16,
    CHARS_20
};

// MACROS =================================================================

#define Angle2MicroSeconds(value) map(value, 0, 180, 1000, 2000)

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

#define VERSION "v1.1.41"
#define BLOWER_OUVERT 25
#define BLOWER_DEMI_OUVERT 90
#define BLOWER_FERME 150
#define PATIENT_OUVERT 25
#define PATIENT_FERME 150

#define SERVO_VALVE_PERIOD 20000

#define PIN_CAPTEUR_PRESSION A1
#define PIN_ALARM D13
#define PIN_LED_RED PC4
#define PIN_LED_GREEN PB13
#define PIN_LED_YELLOW PB14
#define PIN_ESC_BLOWER D5
#define TIM_CHANNEL_ESC_BLOWER 1
#define PIN_SERVO_BLOWER D2
#define TIM_CHANNEL_SERVO_VALVE_BLOWER 3
#define PIN_SERVO_PATIENT D4
#define TIM_CHANNEL_SERVO_VALVE_PATIENT 2
#define PIN_SERVO_Y D3
#define PIN_CONTROL_BUTTONS A0
#define PIN_BATTERY A2
/* Ecran LCD */
#define PIN_LCD_RS D7
#define PIN_LCD_RW PA13
#define PIN_LCD_EN D8
#define PIN_LCD_D4 D9
#define PIN_LCD_D5 D10
#define PIN_LCD_D6 D11
#define PIN_LCD_D7 D12

// contrôle de l'écran LCD
static const ScreenSize screenSize{ScreenSize::CHARS_20};
// période (en centièmes de secondes) de mise à jour du feedback des
// consignes sur le LCD
static const int16_t LCD_UPDATE_PERIOD = 20;

// Periode de traitement en millisecondes
static const uint32_t PCONTROLLER_COMPUTE_PERIOD = 10;

// minimums et maximums possible des paramètres modifiables à l'exécution
static const uint16_t BORNE_SUP_PRESSION_CRETE = 700;   // arbitraire [mmH2O]
static const uint16_t BORNE_INF_PRESSION_CRETE = 100;   // arbitraire [mmH2O]
static const uint16_t BORNE_SUP_PRESSION_PLATEAU = 400; // PP MAX SDRA = 300 [mmH2O]
static const uint16_t BORNE_INF_PRESSION_PLATEAU = 100; // arbitraire [mmH2O]
static const uint16_t BORNE_SUP_PRESSION_PEP = 300;     // PP MAX = 300, or PEP < PP [mmH2O]
static const uint16_t BORNE_INF_PRESSION_PEP = 50;      // arbitraire mais > 0 [mmH2O]
static const uint16_t INITIAL_ZERO_PRESSURE = 0;        // [mmH2O]

static const uint16_t DEFAULT_MIN_PEEP_COMMAND = 80;
static const uint16_t DEFAULT_MAX_PEAK_PRESSURE_COMMAND = 600;

static const uint16_t INITIAL_CYCLE_NB = 20;
static const uint16_t BORNE_SUP_CYCLE = 35; // demande medical
static const uint16_t BORNE_INF_CYCLE = 5;  // demande medical

// durée d'appui des boutons (en centièmes de secondes) avant prise en compte
static const uint16_t MAINTIEN_PARAMETRAGE = 21;

static const uint16_t TENSION_BTN_PRESSION_P_CRETE_PLUS = 913;
static const uint16_t TENSION_BTN_PRESSION_P_CRETE_MINUS = 820;
static const uint16_t TENSION_BTN_PRESSION_PLATEAU_PLUS = 745;
static const uint16_t TENSION_BTN_PRESSION_PLATEAU_MINUS = 598;
static const uint16_t TENSION_BTN_PEP_PLUS = 512;
static const uint16_t TENSION_BTN_PEP_MINUS = 414;
static const uint16_t TENSION_BTN_CYCLE_PLUS = 292;
static const uint16_t TENSION_BTN_CYCLE_MINUS = 215;

const uint16_t TENSION_BTN_ALARME_ON = 650;
const uint16_t TENSION_BTN_ALARME_OFF = 450;
const uint16_t TENSION_BTN_ALARME_SILENCE = 250;
