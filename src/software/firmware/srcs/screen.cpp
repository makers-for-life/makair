/*
    Copyright (C) 2020 Makers For Life
*/
/******************************************************************************
 * @author Makers For Life
 * @file screen.cpp
 * @brief Display and LCD screen related functions
 *
 * This relies on the LiquidCrystal library (https://github.com/arduino-libraries/LiquidCrystal).
 * LCD screen must have 4 lines of 20 characters.
 *****************************************************************************/

#pragma once

// INCLUDES ===================================================================

// Associated header
#include "../includes/screen.h"

// Internal
#include "../includes/parameters.h"

// INITIALISATION =============================================================

/// Number of alarm codes to display on screen at most
static const uint8_t MAX_ALARMS_DISPLAYED = 4;

/// Text to display at the third line of the screen when no alarm is triggered
static const char* NO_ALARM_LINE = "PEAK  PLAT  PEEP    ";

/// Static label to display at the begining of the third line of the screen when at least one alarm
/// is triggered
static const char* ALARM_LINE = "Alarm:              ";

/// Position of the first alarm code in the third line of the screen
static const int ALARMS_CODE_POS = 6;

/// Instance of the screen controller
LiquidCrystal
    screen(PIN_LCD_RS, PIN_LCD_RW, PIN_LCD_EN, PIN_LCD_D4, PIN_LCD_D5, PIN_LCD_D6, PIN_LCD_D7);

// FUNCTIONS ==================================================================

void startScreen() {
    screen.begin(SCREEN_LINE_LENGTH, SCREEN_LINE_NUMBER);
    screen.setCursor(0, 0);
    screen.print("Initialization      ");
    screen.setCursor(0, 1);
    screen.print(VERSION);
}

void resetScreen() { screen.clear(); }

void displayCurrentPressure(uint16_t pressure, uint16_t cyclesPerMinute) {
    screen.setCursor(0, 0);

    char message[SCREEN_LINE_LENGTH + 1];

    (void)snprintf(message, SCREEN_LINE_LENGTH + 1, "Pressure:%2u    %2ucpm", pressure / 10u,
                   cyclesPerMinute);

    screen.print(message);
}

void displayCurrentSettings(uint16_t peakPressureMax,
                            uint16_t plateauPressureMax,
                            uint16_t peepMin) {
    screen.setCursor(0, 1);

    char message[SCREEN_LINE_LENGTH + 1];

    (void)snprintf(message, SCREEN_LINE_LENGTH + 1, "%2u    %2u    %2u  set ",
                   peakPressureMax / 10u, plateauPressureMax / 10u, peepMin / 10u);

    screen.print(message);
}

void displayCurrentInformation(uint16_t peakPressure, uint16_t plateauPressure, uint16_t peep) {
    screen.setCursor(0, 3);
    char message[SCREEN_LINE_LENGTH + 1];

    (void)snprintf(message, SCREEN_LINE_LENGTH + 1, "%2u    %2u    %2u  meas", peakPressure / 10u,
                   plateauPressure / 10u, peep / 10u);

    screen.print(message);
}

static uint8_t prevNbAlarmToPrint = 255;
static uint8_t prevAlarmCodes[MAX_ALARMS_DISPLAYED] = {0};
static bool clearCache = false;

/// Check whether triggered alarms are already displayed on screen or not
static bool hasAlarmInformationChanged(uint8_t p_alarmCodes[], uint8_t p_nbTriggeredAlarms) {
    if (clearCache == true) {
        clearCache = false;
        return true;
    }

    uint8_t nbAlarmToPrint = min(static_cast<uint8_t>(MAX_ALARMS_DISPLAYED), p_nbTriggeredAlarms);

    bool hasChanged = false;
    if (nbAlarmToPrint != prevNbAlarmToPrint) {
        hasChanged = true;
    } else {
        for (uint8_t i = 0; i < nbAlarmToPrint; ++i) {
            if (p_alarmCodes[i] != prevAlarmCodes[i]) {
                hasChanged = true;
                break;
            }
        }
    }

    if (hasChanged) {
        prevNbAlarmToPrint = nbAlarmToPrint;
        for (uint8_t i = 0; i < nbAlarmToPrint; ++i) {
            prevAlarmCodes[i] = p_alarmCodes[i];
        }
    }

    return hasChanged;
}

void clearAlarmDisplayCache() {
    clearCache = true;
}

void displayAlarmInformation(uint8_t p_alarmCodes[], uint8_t p_nbTriggeredAlarms) {
    // WARNING There is a risk of data not being displayed as expected
    // if the line is overwritten somewhere else in the code.
    if (!hasAlarmInformationChanged(p_alarmCodes, p_nbTriggeredAlarms)) {
        return;
    }

    if (p_nbTriggeredAlarms == 0) {
        screen.setCursor(0, 2);
        screen.print(NO_ALARM_LINE);
        return;
    }

    uint8_t nbAlarmToPrint = min(static_cast<uint8_t>(MAX_ALARMS_DISPLAYED), p_nbTriggeredAlarms);

    // +1 for trailing NULL char
    char buf[SCREEN_LINE_LENGTH + 1];

    // Write beginning of line
    strncpy(buf, ALARM_LINE, ALARMS_CODE_POS);

    // Write alarm codes
    char* dst = buf + ALARMS_CODE_POS;
    int spaceLeft = SCREEN_LINE_LENGTH - ALARMS_CODE_POS;
    for (uint8_t i = 0; i < nbAlarmToPrint; i++) {
        // + 1 for the trailing NULL char
        int n = snprintf(dst, spaceLeft + 1, " %u", p_alarmCodes[i]);
        if (n < 0 || n > spaceLeft) {
            break;  // Error or no space left in buffer
        }
        spaceLeft -= n;
        dst += n;
    }

    // Fill the end of the line with spaces
    strncpy(dst, &ALARM_LINE[SCREEN_LINE_LENGTH - spaceLeft], spaceLeft);

    // Make sure string is NULL terminated
    buf[SCREEN_LINE_LENGTH] = '\0';

    screen.setCursor(0, 2);
    screen.print(buf);
}
