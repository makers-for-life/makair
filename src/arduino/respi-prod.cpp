#include <Arduino.h>
#include <Servo.h>
#include <LiquidCrystal.h>
#include <OneButton.h>
#include <common.h>


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

void setup() {
  #ifdef DEBUG
  Serial.begin(115200);
  Serial.println("demarrage");
  #endif
  patient.attach(PIN_SERVO_PATIENT);
  blower.attach(PIN_SERVO_BLOWER);

  #ifdef DEBUG
  Serial.print("mise en secu initiale");
  #endif
  blower.write(secu_coupureBlower);
  patient.write(secu_ouvertureExpi);

  #ifdef LCD_20_CHARS
  lcd.begin(20, 2);
  #else
  lcd.begin(16, 2);
  #endif

  btn_pression_plateau_minus.attachClick(onPressionPlateauMinus);
  btn_pression_plateau_minus.setClickTicks(MAINTIEN_PARAMETRAGE);

  btn_pression_plateau_plus.attachClick(onPressionPlateauPlus);
  btn_pression_plateau_plus.setClickTicks(MAINTIEN_PARAMETRAGE);

  btn_pression_pep_minus.attachClick(onPressionPepMinus);
  btn_pression_pep_minus.setClickTicks(MAINTIEN_PARAMETRAGE);

  btn_pression_pep_plus.attachClick(onPressionPepPlus);
  btn_pression_pep_plus.setClickTicks(MAINTIEN_PARAMETRAGE);

  btn_cycle_minus.attachClick(onCycleMinus);
  btn_cycle_minus.setClickTicks(MAINTIEN_PARAMETRAGE);

  btn_cycle_plus.attachClick(onCyclePlus);
  btn_cycle_plus.setClickTicks(MAINTIEN_PARAMETRAGE);
}

void loop() {
  int nbreCentiemeSecParCycle = 60 * 100 / consigneNbCycle;
  int nbreCentiemeSecParInspi = nbreCentiemeSecParCycle / 3; // inspiration = 1/3 du cycle, expiration = 2/3 du cycle

  #ifdef DEBUG
  Serial.println();
  Serial.println("------ Starting cycle ------");
  Serial.print("nbreCentiemeSecParCycle = ");
  Serial.println(nbreCentiemeSecParCycle);
  Serial.print("nbreCentiemeSecParInspi = ");
  Serial.println(nbreCentiemeSecParInspi);
  #endif

  int currentPressionCrete = -1;
  int currentPressionPlateau = -1;
  int currentPressionPep = -1;

  //int currentPositionBlower = secu_coupureBlower;

  int dureeBaissePression = 0; // compteur de centièmes pour la détection du pic de pression (pression crête)

  // phase courante du cycle
  int currentPhase = PHASE_PUSH_INSPI;

  // état des actionneurs au tick précédent
  int positionBlower = 90;
  int positionPatient = 90;

  // nouvelles consignes pour les actionneurs
  int consigneBlower = 90;
  int consignePatient = 90;

  consigneNbCycle = futureConsigneNbCycle;
  consigneOuverture = futureConsigneOuverture;
  consignePressionPEP = futureConsignePressionPEP;
  consignePressionPlateauMax = futureConsignePressionPlateauMax;
  #ifdef DEBUG
  Serial.print("consigneNbCycle = ");
  Serial.println(consigneNbCycle);
  Serial.print("consigneOuverture = ");
  Serial.println(consigneOuverture);
  Serial.print("consignePressionPEP = ");
  Serial.println(consignePressionPEP);
  Serial.print("consignePressionPlateauMax = ");
  Serial.println(consignePressionPlateauMax);
  #endif


  /********************************************/
  // Affichage une fois par cycle respiratoire
  /********************************************/
  lcd.setCursor(0, 0);
  #ifdef LCD_20_CHARS
  lcd.print("pc=");
  lcd.print(previousPressionCrete);
  lcd.print("/pp=");
  lcd.print(previousPressionPlateau);
  lcd.print("/pep=");
  lcd.print(previousPressionPep);
  lcd.print("  ");
  #else
  lcd.print("pc");
  lcd.print(previousPressionCrete);
  lcd.print("/pp");
  lcd.print(previousPressionPlateau);
  lcd.print("/pep");
  lcd.print(previousPressionPep);
  lcd.print("  ");
  #endif


  /********************************************/
  // Début d'un cycle
  /********************************************/
  for (int currentCentieme = 0; currentCentieme < nbreCentiemeSecParCycle; currentCentieme++) {

    /********************************************/
    // Mesure pression pour rétro-action
    /********************************************/
    #ifdef SIMULATION
    int currentPression = 0
    if (currentCentieme < 50) {
      currentPression = 60;
    } else {
      currentPression = 30;
    }
    if (currentCentieme > nbreCentiemeSecParInspi) {
      currentPression = 5;
    }
    #else
    int currentPression = map(analogRead(PIN_CAPTEUR_PRESSION), 194, 245, 0, 600) / 10;
    #endif

    /********************************************/
    // Calcul des consignes normales
    /********************************************/
    if (currentCentieme <= nbreCentiemeSecParInspi) { // on est dans la phase temporelle d'inspiration (poussée puis plateau)
      if (currentPression >= currentPressionCrete) {
        currentPhase = PHASE_PUSH_INSPI;
        currentPressionCrete = currentPression;

        consigneBlower = 90 - ANGLE_MULTIPLICATEUR * consigneOuverture; // on ouvre le blower vers patient à la consigne paramétrée
        consignePatient = 90 + ANGLE_MULTIPLICATEUR * ANGLE_OUVERTURE_MAXI; // on ouvre le flux IN patient
      } else {
        currentPhase = PHASE_HOLD_INSPI;
        currentPressionPlateau = currentPression;

        consigneBlower = 90 + ANGLE_MULTIPLICATEUR * ANGLE_OUVERTURE_MAXI; // on shunt vers l'extérieur
        consignePatient = 90; // on bloque les flux patient
      }
    } else { // on gère l'expiration on est phase PHASE_EXPIRATION
      currentPhase = PHASE_EXPIRATION;
      currentPressionPep = currentPression;
      consigneBlower = 90 + ANGLE_MULTIPLICATEUR * ANGLE_OUVERTURE_MAXI; // on shunt vers l'extérieur
      consignePatient = secu_ouvertureExpi; // on ouvre le flux OUT patient (expiration vers l'extérieur)
    }

    /********************************************/
    // Calcul des consignes de mise en sécurité
    /********************************************/
    // si pression crête > max, alors fermeture blower de 2°
    if (currentPression > consignePressionCrete) {
      #ifdef DEBUG
      if (currentCentieme % 80) {
        Serial.println("Mise en securite : pression crete trop importante");
      }
      #endif
      consigneBlower = positionBlower - 2;
    }
    // si pression plateau > consigne param, alors ouverture expiration de 1°
    if (currentPhase == PHASE_HOLD_INSPI && currentPression > consignePressionPlateauMax) {
      #ifdef DEBUG
      if (currentCentieme % 80) {
        Serial.println("Mise en securite : pression plateau trop importante");
      }
      #endif
      consignePatient = positionBlower + 1;
    }
    // si pression PEP < PEP mini, alors fermeture complète valve expiration
    if (currentPression < consignePressionPEP) {
      #ifdef DEBUG
      if (currentCentieme % 80) {
        Serial.println("Mise en securite : pression d'expiration positive (PEP) trop faible");
      }
      #endif
      consignePatient = 90;
      currentPhase = PHASE_HOLD_EXPI;
    }

    #ifdef DEBUG
    if (currentCentieme % 50 == 0) {
      Serial.print("Phase : ");
      Serial.println(currentPhase);
      Serial.print("Pression : ");
      Serial.println(currentPression);
    }
    #endif

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
    btn_pression_plateau_minus.tick();
    btn_pression_plateau_plus.tick();
    btn_pression_pep_minus.tick();
    btn_pression_pep_plus.tick();
    btn_cycle_minus.tick();
    btn_cycle_plus.tick();

    /********************************************/
    // Affichage pendant le cycle
    /********************************************/
    if (currentCentieme % LCD_UPDATE_PERIOD == 0) {
      lcd.setCursor(0, 1);
      #ifdef LCD_20_CHARS
      lcd.print("c=");
      lcd.print(futureConsigneNbCycle);
      lcd.print("/pl=");
      lcd.print(futureConsignePressionPlateauMax);
      lcd.print("/pep=");
      lcd.print(futureConsignePressionPEP);
      lcd.print("|");
      lcd.print(currentPression);
      #else
      lcd.print("c");
      lcd.print(futureConsigneNbCycle);
      lcd.print("/pl");
      lcd.print(futureConsignePressionPlateauMax);
      lcd.print("/pep");
      lcd.print(futureConsignePressionPEP);
      #endif
    }

    delay(10); // on attend 1 centième de seconde (on aura de la dérive en temps, sera corrigé par rtc au besoin)

  }
  /********************************************/
  // Fin du cycle
  /********************************************/

}
