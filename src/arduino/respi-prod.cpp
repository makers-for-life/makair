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
#include "config.h"
#include "debug.h"
#include "simulation.h"
#include "variables.h"


/*-----------------------------------------------------------------------------
 * The following functions allow to modify the parameters of the breathing
 * cycle and are the interface to the keyboard user input.
 */
void incConsigneNbCycle()
{
  futureConsigneNbCycle++;
  if (futureConsigneNbCycle > BORNE_SUP_CYCLE) {
      futureConsigneNbCycle = BORNE_SUP_CYCLE;
  }
}

void decConsigneNbCycle()
{
  futureConsigneNbCycle--;
  if (futureConsigneNbCycle < BORNE_INF_CYCLE) {
      futureConsigneNbCycle = BORNE_INF_CYCLE;
  }
}


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

void onCycleMinus() {
  #ifdef DEBUG
  Serial.println("nb cycle --");
  #endif
  futureConsigneNbCycle--;
  if (futureConsigneNbCycle < BORNE_INF_CYCLE) {
    futureConsigneNbCycle = BORNE_INF_CYCLE;
  }
}

void onCyclePlus() {
  #ifdef DEBUG
  Serial.println("nb cycle ++");
  #endif
  futureConsigneNbCycle++;
  if (futureConsigneNbCycle > BORNE_SUP_CYCLE) {
    futureConsigneNbCycle = BORNE_SUP_CYCLE;
  }
}

void onPressionPepMinus() {
  #ifdef DEBUG
  Serial.println("pression PEP --");
  #endif
  futureConsignePressionPEP--;
  if (futureConsignePressionPEP < BORNE_INF_PRESSION_PEP) {
    futureConsignePressionPEP = BORNE_INF_PRESSION_PEP;
  }
}

void onPressionPepPlus() {
  #ifdef DEBUG
  Serial.println("pression PEP ++");
  #endif
  futureConsignePressionPEP++;
  if (futureConsignePressionPEP > BORNE_SUP_PRESSION_PEP) {
    futureConsignePressionPEP = BORNE_SUP_PRESSION_PEP;
  }
}

void onPressionPlateauMinus() {
  #ifdef DEBUG
  Serial.println("pression plateau --");
  #endif
  futureConsignePressionPlateauMax--;
  if (futureConsignePressionPlateauMax < BORNE_INF_PRESSION_PLATEAU) {
    futureConsignePressionPlateauMax = BORNE_INF_PRESSION_PLATEAU;
  }
}

void onPressionPlateauPlus() {
  #ifdef DEBUG
  Serial.println("pression plateau ++");
  #endif
  futureConsignePressionPlateauMax++;
  if (futureConsignePressionPlateauMax > BORNE_SUP_PRESSION_PLATEAU) {
    futureConsignePressionPlateauMax = BORNE_SUP_PRESSION_PLATEAU;
  }
}

// boutons
AnalogButtons analogButtons(ANALOG_PIN, INPUT);
Button btnFree2(BTN_FREE2, &onFree2);
Button btnFree1(BTN_FREE1, &onFree1);
Button btnAlarmOff(BTN_ALARM_OFF, &onAlarmOff);
Button btnAlarmOn(BTN_ALARM_ON, &onAlarmOn);
Button btnCycleMinus(BTN_CYCLE_MINUS, &onCycleMinus);
Button btnCyclePlus(BTN_CYCLE_PLUS, &onCyclePlus);
Button btnPressionPepMinus(BTN_PRESSION_PEP_MINUS, &onPressionPepMinus);
Button btnPressionPepPlus(BTN_PRESSION_PEP_PLUS, &onPressionPepPlus);
Button btnPressionPlateauMinus(BTN_PRESSION_PLATEAU_MINUS, &onPressionPlateauMinus);
Button btnPressionPlateauPlus(BTN_PRESSION_PLATEAU_PLUS, &onPressionPlateauPlus);

void setup() {

  DBG_DO(Serial.begin(115200);)
  DBG_DO(Serial.println("demarrage");)

  patient.attach(PIN_SERVO_PATIENT);
  blower.attach(PIN_SERVO_BLOWER);

  DBG_DO(Serial.print("mise en secu initiale");)

  blower.write(secu_coupureBlower);
  patient.write(secu_ouvertureExpi);

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

void loop() {
  int16_t nbreCentiemeSecParCycle = 60 * 100 / consigneNbCycle;
  int16_t nbreCentiemeSecParInspi = nbreCentiemeSecParCycle / 3; // inspiration = 1/3 du cycle, expiration = 2/3 du cycle

  DBG_AFFICHE_CSPCYCLE_CSPINSPI(nbreCentiemeSecParCycle,
                                nbreCentiemeSecParInspi)

  int16_t currentPressionCrete = -1;
  int16_t currentPressionPlateau = -1;
  int16_t currentPressionPep = -1;

  //int16_t currentPositionBlower = secu_coupureBlower;

  // int16_t dureeBaissePression = 0; // compteur de centièmes pour la détection du pic de pression (pression crête) (Warning: unused)

  // phase courante du cycle
  int16_t currentPhase = CyclePhases::INHALATION;

  // état des actionneurs au tick précédent
  int16_t positionBlower = 90;
  int16_t positionPatient = 90;

  // nouvelles consignes pour les actionneurs
  int16_t consigneBlower = 90;
  int16_t consignePatient = 90;

  consigneNbCycle = futureConsigneNbCycle;
  consigneOuverture = futureConsigneOuverture;
  consignePressionPEP = futureConsignePressionPEP;
  consignePressionPlateauMax = futureConsignePressionPlateauMax;

  DBG_AFFICHE_CONSIGNES(consigneNbCycle, consigneOuverture, consignePressionPEP,
                        consignePressionPlateauMax)

  /********************************************/
  // Affichage une fois par cycle respiratoire
  /********************************************/
  displayEveryCycle(previousPressionCrete,
                    previousPressionPlateau, previousPressionPep);

  /********************************************/
  // Début d'un cycle
  /********************************************/
  for (uint16_t currentCentieme = 0; currentCentieme < nbreCentiemeSecParCycle; currentCentieme++) {

    /********************************************/
    // Mesure pression pour rétro-action
    /********************************************/
    #ifdef SIMULATION
    int16_t currentPression = 0;
    if (currentCentieme < 50) {
      currentPression = 60;
    } else {
      currentPression = 30;
    }
    if (currentCentieme > nbreCentiemeSecParInspi) {
      currentPression = 5;
    }
    #else
    int16_t currentPression = map(analogRead(PIN_CAPTEUR_PRESSION), 194, 245, 0, 600) / 10;
    #endif

    /********************************************/
    // Calcul des consignes normales
    /********************************************/
    if (currentCentieme <= nbreCentiemeSecParInspi) { // on est dans la phase temporelle d'inspiration (poussée puis plateau)
      if (currentPression >= currentPressionCrete) {
        currentPhase = CyclePhases::INHALATION;
        currentPressionCrete = currentPression;

        consigneBlower = 90 - ANGLE_MULTIPLICATEUR * consigneOuverture; // on ouvre le blower vers patient à la consigne paramétrée
        consignePatient = 90 + ANGLE_MULTIPLICATEUR * ANGLE_OUVERTURE_MAXI; // on ouvre le flux IN patient
      } else {
        currentPhase = CyclePhases::PLATEAU;
        currentPressionPlateau = currentPression;

        consigneBlower = 90 + ANGLE_MULTIPLICATEUR * ANGLE_OUVERTURE_MAXI; // on shunt vers l'extérieur
        consignePatient = 90; // on bloque les flux patient
      }
    } else { // on gère l'expiration on est phase CyclePhases::EXHALATION
      currentPhase = CyclePhases::EXHALATION;
      currentPressionPep = currentPression;
      consigneBlower = 90 + ANGLE_MULTIPLICATEUR * ANGLE_OUVERTURE_MAXI; // on shunt vers l'extérieur
      consignePatient = secu_ouvertureExpi; // on ouvre le flux OUT patient (expiration vers l'extérieur)
    }

    /********************************************/
    // Calcul des consignes de mise en sécurité
    /********************************************/
    // si pression crête > max, alors fermeture blower de 2°
    if (currentPression > consignePressionCrete) {
      DBG_PRESSION_CRETE(currentCentieme, 80)
      consigneBlower = positionBlower - 2;
    }
    // si pression plateau > consigne param, alors ouverture expiration de 1°
    if (currentPhase == CyclePhases::PLATEAU && currentPression > consignePressionPlateauMax) {
      DBG_PRESSION_PLATEAU(currentCentieme, 80)
      consignePatient = positionBlower + 1;
    }
    // si pression PEP < PEP mini, alors fermeture complète valve expiration
    if (currentPression < consignePressionPEP) {
      DBG_PRESSION_PEP(currentCentieme, 80)
      consignePatient = 90;
      currentPhase = CyclePhases::HOLD_EXHALATION;
    }

    DBG_PHASE_PRESSION(currentCentieme, 50, currentPhase, currentPression)

    /********************************************/
    // Envoi des nouvelles valeurs aux actionneurs
    /********************************************/
    if (consigneBlower != positionBlower) {
      blower.write(consigneBlower);
      positionBlower = consigneBlower;
    }

    if (consignePatient != positionPatient) {
      patient.write(consignePatient);
      positionPatient = consignePatient;
    }

    /********************************************/
    // Préparation des valeurs pour affichage
    /********************************************/
    previousPressionCrete = currentPressionCrete;
    previousPressionPlateau = currentPressionPlateau;
    previousPressionPep = currentPressionPep;

    /********************************************/
    // Écoute des appuis boutons
    /********************************************/
    analogButtons.check();
    //calibrateButtons();

    /********************************************/
    // Affichage pendant le cycle
    /********************************************/
    if (currentCentieme % LCD_UPDATE_PERIOD == 0) {
      displayDuringCycle(futureConsigneNbCycle,
                         futureConsignePressionPlateauMax,
                         futureConsignePressionPEP, currentPression);
    }

    delay(10); // on attend 1 centième de seconde (on aura de la dérive en temps, sera corrigé par rtc au besoin)

  }
  /********************************************/
  // Fin du cycle
  /********************************************/

}
