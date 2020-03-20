#include <Arduino.h>
#include <Servo.h>
#include <LiquidCrystal.h>


#define DEBUG 0 // mettre à "1" pour envoyer les messages de debug en série

// amplitude radiale des servomoteurs
const int ANGLE_OUVERTURE_MINI = 8;
const int ANGLE_OUVERTURE_MAXI = 45;

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
const int PIN_CAPTEUR_PRESSION = A4; // A4
const int PIN_SERVO_BLOWER = 4; // D4
const int PIN_SERVO_PATIENT = 2; // D2
const int BTN_PRESSION_CRETE_MINUS = 1;
const int BTN_PRESSION_CRETE_PLUS = 2;
const int BTN_PRESSION_PLATEAU_MINUS = 3;
const int BTN_PRESSION_PLATEAU_PLUS = 4;
const int BTN_PRESSION_PEP_MINUS = 5;
const int BTN_PRESSION_PEP_PLUS = 6;
const int BTN_NOMBRE_CYCLE_MINUS = A2;
const int BTN_NOMBRE_CYCLE_PLUS = A3;

// contrôle de l'écran LCD
const int rs = 7, en = 8, d4 = 9, d5 = 10, d6 = 11, d7 = 12;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// phases possibles du cycle
const int PHASE_PUSH_INSPI = 1; // pousée d'inspiration : on envoie l'air jusqu'à la pression crête paramétrée : valve blower ouverte à consigne, flux d'air vers le patient
const int PHASE_HOLD_INSPI = 2; // plateau d'inspiration : on a depassé la pression crête, la pression descend depuis plus d'un 1/10sec (arbitraire EFL) : 2 valves fermées
const int PHASE_EXPIRATION = 3; // expiration : flux d'air vers l'extérieur, air patient vers l'extérieur

// minimums et maximums possible des paramètres modifiables à l'exécution
const int BORNE_SUP_CYCLE = 35;
const int BORNE_INF_CYCLE = 8;

// durée en centièmes de secondes entre chaque appui de bouton
const int INTERVALLE_PARAMETRAGE = 200;

// durée d'appui des boutons (en centièmes de secondes) avant prise en compte
const int MAINTIEN_PARAMETRAGE = 21;

// valeurs de sécurité pour les actionneurs
int secu_coupureBlower = 45;
int secu_ouvertureExpi = 45;

// nombre de cycles par minute (cycle = inspi + plateau + expi)
int consigneNbCycle = 20;
int futureConsigneNbCycle = 20;

// degré d'ouverture de la valve blower (quantité d'air du blower qu'on envoie vers le Air Transistor patient)
int consigneOuverture = 30;
int futureConsigneOuverture = 30;

// consigne de pression de crête maximum
int consignePressionCrete = 60;

// consigne de pression plateau maximum
int consignePressionPlateauMax = 30;
int futureConsignePressionPlateauMax = 30;

// consigne de pression PEP
int consignePressionPEP = 5;
int futureConsignePressionPEP = 5;

// données pour affichage (du cycle précédent pour ne pas afficher des valeurs aberrantes)
int previousPressionCrete = -1;
int previousPressionPlateau = -1;
int previousPressionPep = -1;

// indicateur paramétrage en cours
// faux par défaut, sur détection d'un changement d'état d'un bouton,
// on lui met un nombre de centièmes à attendre afin de confirmer la détection
int parametrageEnCours = 0;
int boutonDetecte = 0;
int previousBoutonDetecte = 0;
int centiemesDepuisReglage = INTERVALLE_PARAMETRAGE;

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
  pinMode(BTN_NOMBRE_CYCLE_MINUS, INPUT);
  pinMode(BTN_NOMBRE_CYCLE_PLUS, INPUT);
  /* pinMode(BTN_PRESSION_CRETE_MINUS, INPUT);
  pinMode(BTN_PRESSION_CRETE_PLUS, INPUT);
  pinMode(BTN_PRESSION_PLATEAU_MINUS, INPUT);
  pinMode(BTN_PRESSION_PLATEAU_PLUS, INPUT);
  pinMode(BTN_PRESSION_PEP_MINUS, INPUT);
  pinMode(BTN_PRESSION_PEP_PLUS, INPUT);
  */
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
  lcd.setCursor(0, 1);
  lcd.print("c:");
  lcd.print(consigneNbCycle);


  /********************************************/
  // Début d'un cycle
  /********************************************/
  for (int currentCentieme = 0; currentCentieme < nbreCentiemeSecParCycle; currentCentieme++) {

    /********************************************/
    // Mesure pression pour rétro-action
    /********************************************/
    int currentPression = 0 ; //analogRead(PIN_CAPTEUR_PRESSION);
    if (currentCentieme < 50) {
      currentPression = 60; //analogRead(PIN_CAPTEUR_PRESSION);
    } else {
      currentPression = 30;
    }
    if (currentCentieme > nbreCentiemeSecParInspi) {
      currentPression = 5;
    }

    /********************************************/
    // Calcul des consignes normales
    /********************************************/
    if (currentCentieme <= nbreCentiemeSecParInspi) { // on est dans la phase temporelle d'inspiration (poussée puis plateau)
      if (currentPression >= currentPressionCrete) {
        currentPhase = PHASE_PUSH_INSPI;
        currentPressionCrete = currentPression;

        consigneBlower = 90 - consigneOuverture; // on ouvre le blower vers patient à la consigne paramétrée
        consignePatient = 90 + ANGLE_OUVERTURE_MAXI; // on ouvre le flux IN patient
      } else {
        currentPhase = PHASE_HOLD_INSPI;
        currentPressionPlateau = currentPression;

        consigneBlower = 90 + ANGLE_OUVERTURE_MAXI; // on shunt vers l'extérieur
        consignePatient = 90; // on bloque les flux patient
      }
    } else { // on gère l'expiration on est phase PHASE_EXPIRATION
      currentPhase = PHASE_EXPIRATION;
      currentPressionPep = currentPression;
      consigneBlower = 90 + ANGLE_OUVERTURE_MAXI; // on shunt vers l'extérieur
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
    // Gestion paramétrage
    /********************************************/
    boutonDetecte = 0;
    if (digitalRead(BTN_NOMBRE_CYCLE_MINUS) == HIGH) {
      boutonDetecte = BTN_NOMBRE_CYCLE_MINUS;
    }
    if (digitalRead(BTN_NOMBRE_CYCLE_PLUS) == HIGH) {
      boutonDetecte = BTN_NOMBRE_CYCLE_PLUS;
    }
    if (digitalRead(BTN_PRESSION_CRETE_MINUS) == HIGH) {
      boutonDetecte = BTN_PRESSION_CRETE_MINUS;
    }
    if (digitalRead(BTN_PRESSION_CRETE_PLUS) == HIGH) {
      boutonDetecte = BTN_PRESSION_CRETE_PLUS;
    }
    if (digitalRead(BTN_PRESSION_PEP_MINUS) == HIGH) {
      boutonDetecte = BTN_PRESSION_PEP_MINUS;
    }
    if (digitalRead(BTN_PRESSION_PEP_PLUS) == HIGH) {
      boutonDetecte = BTN_PRESSION_PEP_PLUS;
    }
    if (digitalRead(BTN_PRESSION_PLATEAU_MINUS) == HIGH) {
      boutonDetecte = BTN_PRESSION_PLATEAU_MINUS;
    }
    if (digitalRead(BTN_PRESSION_PLATEAU_PLUS) == HIGH) {
      boutonDetecte = BTN_PRESSION_PLATEAU_PLUS;
    }

    // première détection
    if (parametrageEnCours == 0 && boutonDetecte != 0) {
      parametrageEnCours = MAINTIEN_PARAMETRAGE; // on attend x ms avant de re-tester pour éviter les rebonds
      previousBoutonDetecte = boutonDetecte;
    }

    // à la fin de la durée de maintien, si le signal est toujours là et que cela fait plus de x secondes que l'on a fait un paramétrage
    if (parametrageEnCours == 1
      && boutonDetecte == previousBoutonDetecte
      && centiemesDepuisReglage > INTERVALLE_PARAMETRAGE) {

      // on réinitialise l'intervalle
      centiemesDepuisReglage = 0;

      // on définit la nouvelle consigne
      if (boutonDetecte == BTN_NOMBRE_CYCLE_MINUS) {
        futureConsigneNbCycle++;
        if (futureConsigneNbCycle > BORNE_SUP_CYCLE) {
          futureConsigneNbCycle = BORNE_SUP_CYCLE;
        }
      }
      if (boutonDetecte == BTN_NOMBRE_CYCLE_PLUS) {
        futureConsigneNbCycle--;
        if (futureConsigneNbCycle < BORNE_INF_CYCLE) {
          futureConsigneNbCycle = BORNE_INF_CYCLE;
        }
      }
      // TODO: gérer les autres boutons

    }

    // à chaque tick de 10 ms, on réduit la durée restante avant prise en compte de l'appui de bouton
    if (parametrageEnCours > 0) {
      parametrageEnCours--;
    }

    // on a lâché le bouton, après paramétrage, on permet un ré-appui
    if (centiemesDepuisReglage < INTERVALLE_PARAMETRAGE && boutonDetecte == 0) {
      centiemesDepuisReglage = INTERVALLE_PARAMETRAGE;
    }

    centiemesDepuisReglage++;

    delay(10); // on attend 1 centième de seconde (on aura de la dérive en temps, sera corrigé par rtc au besoin)

  }
  /********************************************/
  // Fin du cycle
  /********************************************/

}
