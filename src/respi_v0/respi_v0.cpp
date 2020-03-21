#include <Arduino.h>
#include <Servo.h>
#include <LiquidCrystal.h>
#include <OneButton.h>


//#define DEBUG // décommenter pour envoyer les messages de debug en série
// #define SIMULATION

// amplitude radiale des servomoteurs
const int ANGLE_OUVERTURE_MINI = 8;
const int ANGLE_OUVERTURE_MAXI = 45;

// multiplicateur à modifier pour inverser les angles (en cas de suppression de l'engrenage)
const int ANGLE_MULTIPLICATEUR = 1; 

// borne pour le capteur de pression
const int CAPT_PRESSION_MINI = 0; // a adapter lors de la calibration
const int CAPT_PRESSION_MAXI = 800; // on ne va pas jusqu'à 1024 à cause de la saturation de l'AOP -> à adapter avec meilleur AOP

// servomoteur blower : connecte le flux d'air vers le Air Transistor patient ou vers l'extérieur
// 90° → tout est fermé
// entre 45° (90 - ANGLE_OUVERTURE_MAXI) et 82° (90 - ANGLE_OUVERTURE_MINI) → envoi du flux vers l'extérieur
// entre 98° (90 + ANGLE_OUVERTURE_MINI) et 135° (90 + ANGLE_OUVERTURE_MAXI) → envoi du flux vers le Air Transistor patient
Servo blower;

// servomoteur patient : connecte le patient au flux d'air entrant ou à l'extérieur
// 90° → tout est fermé
// entre 45° (90 - ANGLE_OUVERTURE_MAXI) et 82° (90 - ANGLE_OUVERTURE_MINI) → envoi du flux vers le patient
// entre 98° (90 + ANGLE_OUVERTURE_MINI) et 135° (90 + ANGLE_OUVERTURE_MAXI) → échappe l'air du patient vers l'extérieur
Servo patient;

// entrées-sorties
const int PIN_CAPTEUR_PRESSION = A4;
const int PIN_SERVO_BLOWER = 4; // D4
const int PIN_SERVO_PATIENT = 2; // D2
const int BTN_PRESSION_PLATEAU_MINUS = A1;
const int BTN_PRESSION_PLATEAU_PLUS = A0;
const int BTN_PRESSION_PEP_MINUS = 5; // D5
const int BTN_PRESSION_PEP_PLUS = 6; // D6
const int BTN_NOMBRE_CYCLE_MINUS = A3;
const int BTN_NOMBRE_CYCLE_PLUS = A2;

OneButton btn_pression_plateau_minus(BTN_PRESSION_PLATEAU_MINUS, true, true);
OneButton btn_pression_plateau_plus(BTN_PRESSION_PLATEAU_PLUS, true, true);
OneButton btn_pression_pep_minus(BTN_PRESSION_PEP_MINUS, true, true);
OneButton btn_pression_pep_plus(BTN_PRESSION_PEP_PLUS, true, true);
OneButton btn_cycle_minus(BTN_NOMBRE_CYCLE_MINUS, true, true);
OneButton btn_cycle_plus(BTN_NOMBRE_CYCLE_PLUS, true, true);

// contrôle de l'écran LCD
const int rs = 7, en = 8, d4 = 9, d5 = 10, d6 = 11, d7 = 12;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const int LCD_UPDATE_PERIOD = 20; // période (en centièmes de secondes) de mise à jour du feedback des consignes sur le LCD

// phases possibles du cycle
const int PHASE_PUSH_INSPI = 1; // pousée d'inspiration : on envoie l'air jusqu'à la pression crête paramétrée : valve blower ouverte à consigne, flux d'air vers le patient
const int PHASE_HOLD_INSPI = 2; // plateau d'inspiration : on a depassé la pression crête, la pression descend depuis plus d'un 1/10sec (arbitraire EFL) : 2 valves fermées
const int PHASE_EXPIRATION = 3; // expiration : flux d'air vers l'extérieur, air patient vers l'extérieur
const int PHASE_HOLD_EXPI  = 4; // expiration bloquée : les valves sont fermées car la pression est en dessous de la PEP

// minimums et maximums possible des paramètres modifiables à l'exécution
const int BORNE_SUP_PRESSION_CRETE = ANGLE_OUVERTURE_MAXI;
const int BORNE_INF_PRESSION_CRETE = ANGLE_OUVERTURE_MINI;
const int BORNE_SUP_PRESSION_PLATEAU = ANGLE_OUVERTURE_MAXI;
const int BORNE_INF_PRESSION_PLATEAU = ANGLE_OUVERTURE_MINI;
const int BORNE_SUP_PRESSION_PEP = ANGLE_OUVERTURE_MAXI;
const int BORNE_INF_PRESSION_PEP = ANGLE_OUVERTURE_MINI;
const int BORNE_SUP_CYCLE = 35;
const int BORNE_INF_CYCLE = 8;

// durée d'appui des boutons (en centièmes de secondes) avant prise en compte
const int MAINTIEN_PARAMETRAGE = 21;

// valeurs de sécurité pour les actionneurs
int secu_coupureBlower = 45;
int secu_ouvertureExpi = 45;

// nombre de cycles par minute (cycle = inspi + plateau + expi)
int consigneNbCycle = 20;
int futureConsigneNbCycle = consigneNbCycle;

// degré d'ouverture de la valve blower (quantité d'air du blower qu'on envoie vers le Air Transistor patient)
int consigneOuverture = 30;
int futureConsigneOuverture = consigneOuverture;

// consigne de pression de crête maximum
int consignePressionCrete = 60;

// consigne de pression plateau maximum
int consignePressionPlateauMax = 30;
int futureConsignePressionPlateauMax = consignePressionPlateauMax;

// consigne de pression PEP
int consignePressionPEP = 5;
int futureConsignePressionPEP = consignePressionPEP;

// données pour affichage (du cycle précédent pour ne pas afficher des valeurs aberrantes)
int previousPressionCrete = -1;
int previousPressionPlateau = -1;
int previousPressionPep = -1;

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

  lcd.begin(16, 2);

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
  lcd.print("pc");
  lcd.print(previousPressionCrete);
  lcd.print("/pp");
  lcd.print(previousPressionPlateau);
  lcd.print("/pep");
  lcd.print(previousPressionPep);
  lcd.print("  ");


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
      currentPression = 60; //analogRead(PIN_CAPTEUR_PRESSION);
    } else {
      currentPression = 30;
    }
    if (currentCentieme > nbreCentiemeSecParInspi) {
      currentPression = 5;
    }
    #else
    int currentPression = analogRead(PIN_CAPTEUR_PRESSION);    
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
      Serial.println("Mise en securite : pression crete trop importante");
      #endif
      consigneBlower = positionBlower - 2;
    }
    // si pression plateau > consigne param, alors ouverture expiration de 1°
    if (currentPhase == PHASE_HOLD_INSPI && currentPression > consignePressionPlateauMax) {
      #ifdef DEBUG
      Serial.println("Mise en securite : pression plateau trop importante");
      #endif
      consignePatient = positionBlower + 1;
    }
    // si pression PEP < PEP mini, alors fermeture complète valve expiration
    if (currentPression < consignePressionPEP) {
      #ifdef DEBUG
      Serial.println("Mise en securite : pression d'expiration positive (PEP) trop faible");
      #endif
      consignePatient = 90;
      currentPhase = PHASE_HOLD_EXPI;
    }

    #ifdef DEBUG
    if (currentCentieme % 10 == 0) {
      Serial.print("Phase : ");
      Serial.println(currentPhase);
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
      lcd.print("c");
      lcd.print(futureConsigneNbCycle);
      lcd.print("/pl");
      lcd.print(futureConsignePressionPlateauMax);
      lcd.print("/pep");
      lcd.print(futureConsignePressionPEP);
    }

    delay(10); // on attend 1 centième de seconde (on aura de la dérive en temps, sera corrigé par rtc au besoin)

  }
  /********************************************/
  // Fin du cycle
  /********************************************/

}
