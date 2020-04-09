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
#define VERSION "v1.1.45"

/**
 * @name Core parameters
 */
///@{

// Période de traitement en millisecondes
#define PCONTROLLER_COMPUTE_PERIOD 10u

// Minimum & maximum execution parameters
#define CONST_MAX_PEAK_PRESSURE 700u     // arbitrary [mmH2O]
#define CONST_MIN_PEAK_PRESSURE 100u     // arbitrary [mmH2O]
#define CONST_MAX_PLATEAU_PRESSURE 400u  // PP MAX ARDS = 300 [mmH2O]
#define CONST_MIN_PLATEAU_PRESSURE 100u  // arbitrary [mmH2O]
#define CONST_MAX_PEEP_PRESSURE 300u     // PP MAX = 300, or PEEP < PP [mmH2O]
#define CONST_MIN_PEEP_PRESSURE 50u      // arbitrary but > 0 [mmH2O]
#define CONST_INITIAL_ZERO_PRESSURE 0    // [mmH2O]

#define DEFAULT_MIN_PEEP_COMMAND 80
#define DEFAULT_MAX_PLATEAU_COMMAND 300
#define DEFAULT_MAX_PEAK_PRESSURE_COMMAND 320

#define INITIAL_CYCLE_NUMBER 20
#define CONST_MAX_CYCLE 35u
#define CONST_MIN_CYCLE 5u

///@}

/**
 * @name PID gains & settings
 */
///@{
static const int32_t PID_BLOWER_KP = 4;
static const int32_t PID_BLOWER_KI = 32;
static const int32_t PID_BLOWER_KD = 4;

static const int32_t PID_BLOWER_INTEGRAL_MAX = 1000;
static const int32_t PID_BLOWER_INTEGRAL_MIN = -1000;

static const int32_t PID_PATIENT_KP = 4;
static const int32_t PID_PATIENT_KI = 32;
static const int32_t PID_PATIENT_KD = 8;

static const int32_t PID_PATIENT_INTEGRAL_MAX = 500;
static const int32_t PID_PATIENT_INTEGRAL_MIN = -500;

///@}

/**
 * @name Valves
 */
///@{

/// Angle when opened
#define VALVE_OPEN_STATE 0

/// Angle when closed
#define VALVE_CLOSED_STATE 125

#define SERVO_VALVE_PERIOD 20000
#define PIN_SERVO_BLOWER D2  // PA10 / TIM1_CH3
#define TIM_CHANNEL_SERVO_VALVE_BLOWER 3
#define PIN_SERVO_PATIENT D4  // PB5 / TIM3_CH2
#define TIM_CHANNEL_SERVO_VALVE_PATIENT 2

///@}

/**
 * @name Blower
 */
///@{
#define MIN_BLOWER_SPEED 95
#define MAX_BLOWER_SPEED 180
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
#define LCD_UPDATE_PERIOD 30u

/// Period between screen resets in minutes
#define LCD_RESET_PERIOD 5

///@}

/**
 * @name Buttons
 */
///@{

#define PIN_CONTROL_BUTTONS A0
#define PIN_BTN_ALARM_OFF PB2
#define PIN_BTN_START PB1
#define PIN_BTN_STOP PB15
#define VOLTAGE_BUTTON_PEAK_PRESSURE_INCREASE 913
#define VOLTAGE_BUTTON_PEAK_PRESSURE_DECREASE 819
#define VOLTAGE_BUTTON_PLATEAU_PRESSURE_INCREASE 745
#define VOLTAGE_BUTTON_PLATEAU_PRESSURE_DECREASE 608
#define VOLTAGE_BUTTON_PEEP_PRESSURE_INCREASE 512
#define VOLTAGE_BUTTON_PEEP_PRESSURE_DECREASE 415
#define VOLTAGE_BUTTON_CYCLE_INCREASE 294
#define VOLTAGE_BUTTON_CYCLE_DECREASE 216

///@}

/**
 * @name Other I/O
 */
///@{

#define PIN_PRESSURE_SENSOR A1
#define PIN_BUZZER D13
#define PIN_LED_RED PC4
#define PIN_LED_GREEN PB13
#define PIN_LED_YELLOW PB14
#define PIN_ESC_BLOWER D5  // PB4 / TIM3_CH1
#define TIM_CHANNEL_ESC_BLOWER 1
#define PIN_BATTERY A2

///@}

/**
 * @name Alarm thresholds
 */
///@{

#define ALARM_2_CMH2O 20                                           // RCM-SW-2
#define ALARM_35_CMH2O 350                                         // RCM-SW-1
#define ALARM_THRESHOLD_PEEP_ABOVE_OR_UNDER_2_CMH2O ALARM_2_CMH2O  // RCM-SW-3 / RCM-SW-15
#define ALARM_THRESHOLD_PLATEAU_UNDER_2_CMH2O ALARM_2_CMH2O        // RCM-SW-19
#define ALARM_THRESHOLD_PLATEAU_ABOVE_80_CMH2O 800                 // RCM-SW-18

///@}
