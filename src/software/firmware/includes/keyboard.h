/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file keyboard.h
 * @brief Analog buttons related functions
 *
 * This relies on the Analog Buttons library (https://github.com/rlogiacco/AnalogButtons).
 *****************************************************************************/

#pragma once

/**
 * Initialize keyboard abstraction
 *
 * @warning It must be called once to be able to use the buttons
 */
void initKeyboard();

/**
 * Handle button events
 *
 * @warning It must be called in the program loop
 */
void keyboardLoop();

/// Helper to calibrate analog values of buttons
void calibrateButtons();
