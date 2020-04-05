/*
    Copyright (C) 2020 Makers For Life
*/
/******************************************************************************
 * @author Makers For Life
 * @file parameters.h
 * @brief Various settings
 *****************************************************************************/
#pragma once

// INCLUDES ===================================================================

// External
#include <Arduino.h>

// Internal
#include "config.h"

// PARAMETERS =================================================================

/// Current version of the software
#define VERSION "v1.1.43"

/**
 * @name Core parameters
 */
///@{

// Période de traitement en millisecondes
#define PCONTROLLER_COMPUTE_PERIOD 10

// Minimums et maximums possible des paramètres modifiables à l'exécution
#define BORNE_SUP_PRESSION_CRETE 700    // arbitraire [mmH2O]
#define BORNE_INF_PRESSION_CRETE 100    // arbitraire [mmH2O]
#define BORNE_SUP_PRESSION_PLATEAU 400  // PP MAX SDRA = 300 [mmH2O]
#define BORNE_INF_PRESSION_PLATEAU 100  // arbitraire [mmH2O]
#define BORNE_SUP_PRESSION_PEP 300      // PP MAX = 300, or PEP < PP [mmH2O]
#define BORNE_INF_PRESSION_PEP 50       // arbitraire mais > 0 [mmH2O]
#define INITIAL_ZERO_PRESSURE 0         // [mmH2O]

#define DEFAULT_MIN_PEEP_COMMAND 80
#define DEFAULT_MAX_PLATEAU_COMMAND 300
#define DEFAULT_MAX_PEAK_PRESSURE_COMMAND 320

#define INITIAL_CYCLE_NB 20
#define BORNE_SUP_CYCLE 35  // demande medical
#define BORNE_INF_CYCLE 5   // demande medical

///@}

/**
 * @name Valves
 */
///@{

/// Angle when open
#define VALVE_OUVERT 25

/// Angle when half-open
#define VALVE_DEMI_OUVERT 90

/// Angle when closed
#define VALVE_FERME 145

#define SERVO_VALVE_PERIOD 20000
#define PIN_SERVO_BLOWER D2  // PA10 / TIM1_CH3
#define TIM_CHANNEL_SERVO_VALVE_BLOWER 3
#define PIN_SERVO_PATIENT D4  // PB5 / TIM3_CH2
#define TIM_CHANNEL_SERVO_VALVE_PATIENT 2

///@}

/**
 * @name LCD screen
 */
///@{

#define PIN_LCD_RS D7
#define PIN_LCD_RW PA13
#define PIN_LCD_EN D8
#define PIN_LCD_D4 D9
#define PIN_LCD_D5 D10
#define PIN_LCD_D6 D11
#define PIN_LCD_D7 D12

/// Number of lines
#define SCREEN_LINE_NUMBER 4

/// Number of characters per line
#define SCREEN_LINE_LENGTH 20

/// Period between screen updates in hundredth of second
#define LCD_UPDATE_PERIOD 20

///@}

/**
 * @name Buttons
 */
///@{

#define PIN_CONTROL_BUTTONS A0
#define PIN_BTN_ALARM_OFF PB2
#define PIN_BTN_START PB1
#define PIN_BTN_STOP PB15
#define TENSION_BTN_PRESSION_P_CRETE_PLUS 913
#define TENSION_BTN_PRESSION_P_CRETE_MINUS 819
#define TENSION_BTN_PRESSION_PLATEAU_PLUS 745
#define TENSION_BTN_PRESSION_PLATEAU_MINUS 608
#define TENSION_BTN_PEP_PLUS 512
#define TENSION_BTN_PEP_MINUS 415
#define TENSION_BTN_CYCLE_PLUS 294
#define TENSION_BTN_CYCLE_MINUS 216

///@}

/**
 * @name Other I/O
 */
///@{

#define PIN_CAPTEUR_PRESSION A1
#define PIN_ALARM D13
#define PIN_LED_RED PC4
#define PIN_LED_GREEN PB13
#define PIN_LED_YELLOW PB14
#define PIN_ESC_BLOWER D5  // PB4 / TIM3_CH1
#define TIM_CHANNEL_ESC_BLOWER 1
#define PIN_BATTERY A2

///@}

// USELESS?

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
