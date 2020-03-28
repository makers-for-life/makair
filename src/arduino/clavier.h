/*=============================================================================
 * @file clavier.h
 *
 * COVID Respirator
 *
 * @section copyright Copyright
 *
 * Makers For Life
 *
 * @section descr File description
 *
 * This file is the header for input keyboard
 */

#pragma once

/*
 * initKeyboard does the initializations to use a keyboard
 */
void initKeyboard();

/*
 * keyboardLoop does the polling of the keyboard. It has to be called
 * in loop
 */
void keyboardLoop();


//Call this in loop() to help find analog values of buttons
void calibrateButtons();
