/*=============================================================================
 * @file respi-prod.h
 *
 * COVID Respirator
 *
 * @section copyright Copyright
 *
 * Makers For Life
 *
 * @section descr File description
 *
 * This file execute the Makair program
 */

// INCLUDES ==================================================================

// Associated header
#include "respi-prod.h"

// External
#include <AnalogButtons.h>
#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Servo.h>

// Internal
#include "affichage.h"
#include "common.h"
#include "parameters.h"
#include "config.h"
#include "debug.h"
#include "pressure_controller.h"
#include "simulation.h"

PressureController pController();

void onFree2() {
  #ifdef DEBUG
  Serial.println("free2");
  #endif
}

void onFree1() {
  #ifdef DEBUG
  Serial.println("free1");
  #endif
}

void onAlarmOff() {
  #ifdef DEBUG
  Serial.println("alarm OFF");
  #endif
}

void onAlarmOn() {
  #ifdef DEBUG
  Serial.println("alarm ON");
  #endif
}

// boutons
AnalogButtons analogButtons(ANALOG_PIN, INPUT);
Button btnFree2(BTN_FREE2, &onFree2);
Button btnFree1(BTN_FREE1, &onFree1);
Button btnAlarmOff(BTN_ALARM_OFF, &onAlarmOff);
Button btnAlarmOn(BTN_ALARM_ON, &onAlarmOn);
Button btnCycleMinus(BTN_CYCLE_MINUS, &pController.onCycleMinus());
Button btnCyclePlus(BTN_CYCLE_PLUS, &pController.onCyclePlus());
Button btnPressionPepMinus(BTN_PRESSION_PEP_MINUS,
                           &pController.onPressionPepMinus());
Button btnPressionPepPlus(BTN_PRESSION_PEP_PLUS,
                          &pController.onPressionPepPlus());
Button btnPressionPlateauMinus(BTN_PRESSION_PLATEAU_MINUS,
                               &pController.onPressionPlateauMinus());
Button btnPressionPlateauPlus(BTN_PRESSION_PLATEAU_PLUS,
                              &pController.onPressionPlateauPlus());

void setup() {

  DBG_DO(Serial.begin(115200);)
  DBG_DO(Serial.println("demarrage");)

  pController.setup();

  startScreen();

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

/*
//Call this in loop() to help find analog values of buttons
void calibrateButtons() {
	uint16_t value = analogRead(ANALOG_PIN);
	Serial.println(value);
	delay(250);
}
*/

void loop()
{
  /********************************************/
  // INITIALIZE THE RESPIRATORY CYCLE
  /********************************************/

  displayEveryRespiratoryCycle(pController.peakPressure(), pController.plateauPressure(),
                    pController.peep());

  pController.initRespiratoryCycle();

  /********************************************/
  // START THE RESPIRATORY CYCLE
  /********************************************/
  for (uint16_t centiSec = 0;
       centiSec < pController.centiSecPerCycle(); centiSec++) {

    // Get the measured pressure for the feedback control
    #ifdef SIMULATION
    if (centiSec < uint16_t(50)) {
      pController.updatePressure(60);
    } else {
      pController.updatePressure(30);
    }
    if (centiSec > pController.centiSecPerInhalation()) {
      pController.updatePressure(5);
    }
    #else
    pController.updatePressure(map(analogRead(PIN_CAPTEUR_PRESSION), 194, 245, 0, 600) / 10);
    #endif

    // Perform the pressure control
    pController.compute(centiSec);


    // Check if some buttons have been pushed
    analogButtons.check();
    //calibrateButtons();

    // Display relevant information during the cycle
    if (centiSec % LCD_UPDATE_PERIOD == 0) {
      displayDuringCycle(pController.cyclesPerMinuteCommand(),
                         pController.maxPlateauPressureCommand(),
                         pController.m_minPeepCommand(),
                         pController.pressure());
    }

    // Wait 1/100 of second
    // Warning: this introduces a time drift, that will be corrected by rtc if needed
    delay(10);
  }
  /********************************************/
  // END OF THE RESPIRATORY CYCLE
  /********************************************/

}
