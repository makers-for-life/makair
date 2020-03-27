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
 * This file is the implementation for input keyboard
 */

#include "clavier.h"
#include "debug.h"
#include "respi-prod.h"
#include "common.h"

/* For analog keyboard */
#include <AnalogButtons.h>

static AnalogButtons analogButtons(ANALOG_PIN, INPUT);

/*-----------------------------------------------------------------------------
 * Button handlers
 */

/* Handler of the second free button */
void onFree2() {
  DBG_DO(Serial.println("free2");)
}

/* Handler of the first free button */
void onFree1() {
  DBG_DO(Serial.println("free1");)
}

/* Handler of the button to switch the alarm off */
void onAlarmOff() {
  DBG_DO(Serial.println("alarm OFF");)
}

/* Handler of the button to switch the alarm on */
void onAlarmOn() {
  DBG_DO(Serial.println("alarm ON");)
}

/* Handler of the button to decrement the number of breathing cycles */
void onCycleMinus() {
  pController.onCycleMinus();
}

/* Handler of the button to increment the number of breathing cycles */
void onCyclePlus() {
  pController.onCyclePlus();
}

/* Handler of the button to decrement the PEP pressure */
void onPressionPepMinus() {
  pController.onPressionPepMinus();
}

/* Handler of the button to increment the PEP pressure */
void onPressionPepPlus() {
  pController.onPressionPepPlus();
}

/* Handler of the button to decrement the plateau pressure */
void onPressionPlateauMinus() {
  pController.onPressionPlateauMinus();
}

/* Handler of the button to increment the plateau pressure */
void onPressionPlateauPlus() {
  pController.onPressionPlateauPlus();
}

/*
 * Buttons association with analog level and handlers
 */
static Button btnFree2(BTN_FREE2, onFree2);
static Button btnFree1(BTN_FREE1, onFree1);
static Button btnAlarmOff(BTN_ALARM_OFF, onAlarmOff);
static Button btnAlarmOn(BTN_ALARM_ON, onAlarmOn);
static Button btnCycleMinus(BTN_CYCLE_MINUS, onCycleMinus);
static Button btnCyclePlus(BTN_CYCLE_PLUS, onCyclePlus);
static Button btnPressionPepMinus(BTN_PRESSION_PEP_MINUS, onPressionPepMinus);
static Button btnPressionPepPlus(BTN_PRESSION_PEP_PLUS, onPressionPepPlus);
static Button btnPressionPlateauMinus(BTN_PRESSION_PLATEAU_MINUS, onPressionPlateauMinus);
static Button btnPressionPlateauPlus(BTN_PRESSION_PLATEAU_PLUS, onPressionPlateauPlus);

/*-----------------------------------------------------------------------------
 * initKeyboard does the initializations to use a keyboard
 */
void initKeyboard()
{
  analogButtons.add(btnFree2);
  analogButtons.add(btnFree1);
  analogButtons.add(btnAlarmOff);
  analogButtons.add(btnAlarmOn);
  analogButtons.add(btnCycleMinus);
  analogButtons.add(btnCyclePlus);
  analogButtons.add(btnPressionPepMinus);
  analogButtons.add(btnPressionPepPlus);
  analogButtons.add(btnPressionPlateauMinus);
  analogButtons.add(btnPressionPlateauPlus);
}

/*-----------------------------------------------------------------------------
 * keyboardLoop triggers the reading of the buttons and call the handlers
 * Has to be called within the Pressure controller loop
 */
void keyboardLoop()
{
  analogButtons.check();
}
