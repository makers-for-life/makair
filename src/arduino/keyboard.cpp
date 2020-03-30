/*=============================================================================
 * @file keyboard.cpp
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
 * This file is the implementation for input keyboard
 */

// INCLUDES ===================================================================

// Associated header
#include "keyboard.h"

// External
/* For analog keyboard */
#include <AnalogButtons.h>

// Internal
#include "debug.h"
#include "parameters.h"
#include "pressure_controller.h"

// INITIALISATION =============================================================

static AnalogButtons analogButtons(PIN_CONTROL_BUTTONS, INPUT);

/*-----------------------------------------------------------------------------
 * Button handlers
 */

/* Handler of the button to increment the Crete pressure */
void increasePeakPressure()
{ /* TODO */
}

/* Handler of the button to decrement the Crete pressure */
void decreasePeakPressure()
{ /* TODO */
}

/* Handler of the button to decrement the plateau pressure */
void decreasePlateauPressure() { pController.decreasePlateauPressure(); }

/* Handler of the button to increment the plateau pressure */
void increasePlateauPressure() { pController.increasePlateauPressure(); }

/* Handler of the button to increment the PEP pressure */
void increasePeep() { pController.increasePeep(); }

/* Handler of the button to decrement the PEP pressure */
void decreasePeep() { pController.decreasePeep(); }

/* Handler of the button to increment the number of breathing cycles */
void increaseCyclesPerMinute() { pController.increaseCyclesPerMinute(); }

/* Handler of the button to decrement the number of breathing cycles */
void decreaseCyclesPerMinute() { pController.decreaseCyclesPerMinute(); }

/*
 * Buttons associations with analog levels and handlers
 */

Button btnPressionCretePlus(TENSION_BTN_PRESSION_P_CRETE_PLUS, &increasePeakPressure);
Button btnPressionCreteMinus(TENSION_BTN_PRESSION_P_CRETE_MINUS, &decreasePeakPressure);
Button btnPressionPlateauPlus(TENSION_BTN_PRESSION_PLATEAU_PLUS, &increasePlateauPressure);
Button btnPressionPlateauMinus(TENSION_BTN_PRESSION_PLATEAU_MINUS, &decreasePlateauPressure);
Button btnPressionPepPlus(TENSION_BTN_PEP_PLUS, &increasePeep);
Button btnPressionPepMinus(TENSION_BTN_PEP_MINUS, &decreasePeep);
Button btnCyclePlus(TENSION_BTN_CYCLE_PLUS, &increaseCyclesPerMinute);
Button btnCycleMinus(TENSION_BTN_CYCLE_MINUS, &decreaseCyclesPerMinute);

/*-----------------------------------------------------------------------------
 * initKeyboard does the initializations to use a keyboard
 */
void initKeyboard()
{
    analogButtons.add(btnPressionCretePlus);
    analogButtons.add(btnPressionCretePlus);
    analogButtons.add(btnPressionPlateauPlus);
    analogButtons.add(btnPressionPlateauMinus);
    analogButtons.add(btnPressionPepPlus);
    analogButtons.add(btnPressionPepMinus);
    analogButtons.add(btnCyclePlus);
    analogButtons.add(btnCycleMinus);
}

/*-----------------------------------------------------------------------------
 * keyboardLoop triggers the reading of the buttons and call the handlers
 * Has to be called within the Pressure controller loop
 */
void keyboardLoop() { analogButtons.check(); }

void calibrateButtons()
{
    uint16_t value = analogRead(PIN_CONTROL_BUTTONS);
    Serial.println(value);
    delay(250);
}
