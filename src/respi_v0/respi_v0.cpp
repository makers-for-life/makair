#include <Arduino.h>
#include <Servo.h>

Servo patient;
Servo blower;

const int PIN_CAPTEUR_PRESSION = 4; // A4
const int PIN_SERVO_BLOWER = 4; // D4
const int PIN_SERVO_PATIENT = 2; // D2

const int PHASE_PUSH_INSPI = 1; // inspiration : on envoie l'air jusqu'à la pression crête paramétrée : valve blower ouverte à consigne, valve patient fermée (pas d'expiration)
const int PHASE_HOLD_INSPI = 2; // plateau : on a depassé la pression crête, la pression descend depuis plus d'un 1/10sec (arbitraire EFL) : 2 valves fermées
const int PHASE_EXPIRATION = 3; // expiration : valve blower fermée, valve patient ouverte

const int ANGLE_OUVERTURE_MINI = 8;
const int ANGLE_OUVERTURE_MAXI = 45;

int secu_coupureBlower = 45;
int secu_ouvertureExpi = 45;

void setup() {
  //Serial.begin(115200);
  //Serial.println("demarrage");
  patient.attach(PIN_SERVO_PATIENT);
  blower.attach(PIN_SERVO_BLOWER);

  //Serial.print("mise en secu initiale");
  blower.write(secu_coupureBlower);
  patient.write(secu_ouvertureExpi);
}

// nombre de cycles par minute (cycle = inspi + plateau + expi)
int consigneNbCycle = 20;

// degré d'ouverture de la valve blower (quantité d'air du blower qu'on envoie vers la boite 2)
int consigneOuverture = 30;

// consigne de pression plateau maxi
int consignePressionPlateauMax = 30;

// consigne de pression PEP
int consignePressionPEP = 5;

// données pour affichage (du cycle précédent pour ne pas afficher des valeurs aberrantes)
int previousPressionCrete = -1;
int previousPressionPlateau = -1;

void loop() {
  int nbreCentiemeSecParCycle = 60 * 100 / consigneNbCycle;
  int nbreCentiemeSecParInspi = nbreCentiemeSecParCycle / 3; // inspiration = 1/3 du cycle, expiration = 2/3 du cycle

  //Serial.println();
  //Serial.println("------ Starting cycle ------");
  //Serial.print("nbreCentiemeSecParCycle = ");
  //Serial.println(nbreCentiemeSecParCycle);
  //Serial.print("nbreCentiemeSecParInspi = ");
  //Serial.println(nbreCentiemeSecParInspi);

  int currentPressionCrete = -1;
  int currentPressionPlateau = -1;

  //int currentPositionBlower = secu_coupureBlower;

  int dureeBaissePression = 0; // compteur de centièmes pour la détection du pic de pression (pression crête)

  int currentPhase = PHASE_PUSH_INSPI;

  int positionBlower = 90;
  int positionPatient = 90;

  int consigneBlower = 90;
  int consignePatient = 90;

  /********************************************/
  // Début d'un cycle
  /********************************************/
  for (int currentCentieme = 0; currentCentieme < nbreCentiemeSecParCycle; currentCentieme++) {

    /********************************************/
    // Mesure pression pour rétro-action
    /********************************************/
    int currentPression = 0 ; //analogRead(PIN_CAPTEUR_PRESSION);
    if (currentCentieme < 50) {
      currentPression = 100; //analogRead(PIN_CAPTEUR_PRESSION);
    } else {
      currentPression = 90;
    }

    /********************************************/
    // Calcul des consignes normales
    /********************************************/
    if (currentCentieme <= nbreCentiemeSecParInspi) {
      if (currentPression >= currentPressionCrete) {
        currentPhase = PHASE_PUSH_INSPI;
        currentPressionCrete = currentPression;

        consigneBlower = 90 - consigneOuverture;  // on ouvre le blower vers patient à la consigne paramétrée
        consignePatient = 90 + ANGLE_OUVERTURE_MAXI; // on ouvre le flux IN patient
      } else {
        currentPhase = PHASE_HOLD_INSPI;
        currentPressionPlateau = currentPression;

        consigneBlower = 90 + ANGLE_OUVERTURE_MAXI; // on shunt vers l'extérieur
        consignePatient = 90; // on bloque les flux patient
      }
    } else { // on gère l'expiration on est phase PHASE_EXPIRATION
      currentPhase = PHASE_EXPIRATION;
      consigneBlower = 90 + ANGLE_OUVERTURE_MAXI;  // on shunt vers l'extérieur
      consignePatient = secu_ouvertureExpi; // on ouvre le flux OUT patient (expiration vers l'extérieur)
    }

    /********************************************/
    // Calcul des consignes de mise en sécurité
    /********************************************/
    /*
    * -> si pression crête > max, alors fermeture ouverture blower de 2°
    * -> si pression plateau > consigne param, alors ouverture expiration de 1°
    * -> si pression PEP < PEP mini, alors fermeture complète valve expiration
    */

    //if (currentCentieme % 10 == 0) {
    //  Serial.print("Phase : ");
    //  Serial.println(currentPhase);
    //}

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

    delay(10); // on attend 1 centième de seconde (on aura de la dérive en temps, sera corrigé par rtc au besoin)

  }
  /********************************************/
  // Fin du cycle
  /********************************************/

}
