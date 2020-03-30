/*=============================================================================
 * @file parameters.h
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
#include "display.h"

// PARAMETERS =================================================================

// Radial magnitude of the servomotors
// \warning ANGLE_FERMETURE has to be greater than ANGLE_OUVERTURE_MAXI
static const uint16_t ANGLE_OUVERTURE_MINI = 8;
static const uint16_t ANGLE_OUVERTURE_MAXI = 45;
static const uint16_t ANGLE_FERMETURE = 90;

// Multiplier coefficient to tune to reverse angles (if the gear is removed)
static const int16_t ANGLE_MULTIPLICATEUR = 1;

// Lower bound of the pressure sensor
// N.B.: To tune following the calibration
static const uint16_t CAPT_PRESSION_MINI = 0;

// Upper bound of the pressure sensor
// N.B.: -  To tune following the calibration
//       -  Clamp below 1024 due to the AOP saturation
static const uint16_t CAPT_PRESSION_MAXI = 800;

// Inputs & output
#if defined(ARDUINO_AVR_UNO)
/* On the Arduino Uno prototype */
static const int16_t PIN_CAPTEUR_PRESSION = A4;
static const int16_t PIN_SERVO_BLOWER = 4;  // D4
static const int16_t PIN_SERVO_PATIENT = 2; // D2
static const int16_t PIN_SERVO_Y = -1;      // Warning: there is no Y servomotor
static const int16_t BTN_NOMBRE_CYCLE_MINUS = A3;
static const int16_t BTN_NOMBRE_CYCLE_PLUS = A2;

/* LCD screen */
static const int16_t PIN_LCD_RS = 7;
static const int16_t PIN_LCD_EN = 8;
static const int16_t PIN_LCD_D4 = 9;
static const int16_t PIN_LCD_D5 = 10;
static const int16_t PIN_LCD_D6 = 11;
static const int16_t PIN_LCD_D7 = 12;

/* Analogic keyboard */
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

/* LCD screen */
static const int16_t PIN_LCD_RS = D7;
static const int16_t PIN_LCD_EN = D8;
static const int16_t PIN_LCD_D4 = D9;
static const int16_t PIN_LCD_D5 = D10;
static const int16_t PIN_LCD_D6 = D11;
static const int16_t PIN_LCD_D7 = D12;
#else
#error "Electronic board not supported"
#endif

// alarmes
//#define YELLOW_LED_PIN_EXISTS
#ifdef YELLOW_LED_PIN_EXISTS
static const int16_t PIN_YELLOW_LED = 7;
#endif

//#define RED_LED_PIN_EXISTS
#ifdef RED_LED_PIN_EXISTS
static const int16_t PIN_RED_LED = 8;
#endif

//#define BEEPER_PIN_EXISTS
#ifdef BEEPER_PIN_EXISTS
static const int16_t PIN_BEEPER = 9;
#endif

// LCD screen control
static const ScreenSize screenSize{ScreenSize::CHARS_20};
// Period (in hundredth of seconds) to update the LCD screen commands to display
static const int16_t LCD_UPDATE_PERIOD = 20;

// Computation period in milliseconds
static const uint32_t PCONTROLLER_COMPUTE_PERIOD = 10;

// Lower and upper bounds of the tunable parameters during the runtime
static const uint16_t BORNE_SUP_PRESSION_CRETE = 700;   // arbitrary [mmH2O]
static const uint16_t BORNE_INF_PRESSION_CRETE = 100;   // arbitrary [mmH2O]
static const uint16_t BORNE_SUP_PRESSION_PLATEAU = 400; // PP MAX SDRA = 300 [mmH2O]
static const uint16_t BORNE_INF_PRESSION_PLATEAU = 100; // arbitrary [mmH2O]
static const uint16_t BORNE_SUP_PRESSION_PEP = 300;     // PP MAX = 30, or PEP < PP [mmH2O]
static const uint16_t BORNE_INF_PRESSION_PEP = 50;      // arbitrary but has to be > 0 [mmH2O]

static const uint16_t BORNE_SUP_CYCLE = 35; // medical input
static const uint16_t BORNE_INF_CYCLE = 5;  // medical input

// Duration during which a button has to be pushed before the command is taken into account
static const uint16_t MAINTIEN_PARAMETRAGE = 21;

static const uint16_t TENSION_BTN_PRESSION_P_CRETE_PLUS = 915;
static const uint16_t TENSION_BTN_PRESSION_P_CRETE_MINUS = 800;
static const uint16_t TENSION_BTN_PRESSION_PLATEAU_PLUS = 728;
static const uint16_t TENSION_BTN_PRESSION_PLATEAU_MINUS = 598;
static const uint16_t TENSION_BTN_PEP_PLUS = 504;
static const uint16_t TENSION_BTN_PEP_MINUS = 410;
static const uint16_t TENSION_BTN_CYCLE_PLUS = 290;
static const uint16_t TENSION_BTN_CYCLE_MINUS = 215;

const uint16_t TENSION_BTN_ALARME_ON = 650;
const uint16_t TENSION_BTN_ALARME_OFF = 450;
const uint16_t TENSION_BTN_ALARME_SILENCE = 250;

// Pressure sensor parameters
const double RATIO_PONT_DIVISEUR = 0.8192;
const double V_SUPPLY = 5.08;
