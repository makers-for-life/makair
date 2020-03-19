#include <Servo.h>

Servo patient;
Servo blower;

const int PIN_CAPTEUR_PRESSION = 4; // A4
const int  PIN_SERVO_BLOWER = 4; // D4
const int  PIN_SERVO_PATIENT = 2; // D2

const int  PHASE_PUSH_INSPI = 1; // première phase où on envoie l'air jusqu'à pression crete param : valve blower ouverte à consigne, valve patient fermée (pas d'expi) 
const int  PHASE_HOLD_INSPI = 2; // plateau : on a depassé la pression crete, la pression descend depuis plus d'un 1/10sec (arbitraire EFL) : 2 valve fermée
const int  PHASE_EXPIRATION = 3; // Expiration : valve blower 

const int  ANGLE_OUVERTURE_MINI = 8;
const int  ANGLE_OUVERTURE_MAXI = 45;

int secu_coupureBrower = 45;
int secu_ouvertureExpi = 45;

void setup() {
//  Serial.begin(115200); 
//  Serial.println("demarrage");
patient.attach(PIN_SERVO_PATIENT);
blower.attach(PIN_SERVO_BLOWER);

//Serial.print("mise en secu initiale");
blower.write(secu_coupureBrower);
patient.write(secu_ouvertureExpi);

}
// nombre de cycle par minute (cycle = inspi + expi)
int consigneNbCycle = 20;

// degre d'ouverture de la valve brower (quantité d'air du blower qu'on push vers la boite 2)
int consigneOuverture = 30; 

// consigne de pression plateau maxi
int consignePressionPlateauMax = 30; 

// consigne de pression pep
int consignePressionPEP = 5; 

// data pour affichage : donnée du cycle precedent pour ne pas afficher des datas aberrantes
int previousPressionCrete = -1;
int previousPressionPlateau = -1;

void loop() {
  int nbreCentiemeSecParCycle = 60 * 100 / consigneNbCycle;
  int nbreCentiemeSecParInspi = nbreCentiemeSecParCycle / 3; // Inspiration est 1/3 du cycle, expi est 2/3

//  Serial.println();
//  Serial.println("------ Starting cycle ------");
//  Serial.print("nbreCentiemeSecParCycle = "); 
//  Serial.println(nbreCentiemeSecParCycle);
//  Serial.print("nbreCentiemeSecParInspi = "); 
//  Serial.println(nbreCentiemeSecParInspi);

  int currentPressionCrete = -1;
  int currentPressionPlateau = -1;
  
  //int currentPositionBlower = secu_coupureBrower;
  
  int dureeBaissePression = 0; // compteur de centieme pour la détection du pic de pression (pression crete)
  
  int currentPhase = PHASE_PUSH_INSPI;

  int positionBlower = 90;
  int positionPatient = 90;

  int consigneBlower = 90;
  int consignePatient = 90;
  
  for(int currentCentieme = 0; currentCentieme < nbreCentiemeSecParCycle; currentCentieme++){
    
    // mesure pression pour retro action
    int currentPression = 0 ; // analogRead(4);
    if(currentCentieme < 50){
      currentPression = 100 ; //analogRead(PIN_CAPTEUR_PRESSION);
    } else{
      currentPression = 90;
    }

    // calcul des consignes normales
    if(currentCentieme <= nbreCentiemeSecParInspi){
      if(currentPression >= currentPressionCrete){
        currentPhase = PHASE_PUSH_INSPI;
        currentPressionCrete = currentPression;
        
        consigneBlower = 90 - consigneOuverture;  // on ouvre le blower vers patient à la consigne paramétrée
        consignePatient = 90 + ANGLE_OUVERTURE_MAXI; // on ouvre le flux IN patient
      } else{
        currentPhase = PHASE_HOLD_INSPI; 
        currentPressionPlateau = currentPression;
 
        consigneBlower = 90 + ANGLE_OUVERTURE_MAXI; // on shunt vers l'extérieur
        consignePatient = 90; // on bloque les flux patient
      }
    } else{ // on gère l'expiration on est phase PHASE_EXPIRATION
      currentPhase = PHASE_EXPIRATION;
      consigneBlower = 90 + ANGLE_OUVERTURE_MAXI;  // on shunt vers l'extérieur
      consignePatient = secu_ouvertureExpi; // on ouvre le flux OUT patient (expi -> extérieur)
    }

  /* 
   * calcul des consignes de mise en sécurité du patient 
   * -> si pression crete > max => fermeture ouverture blower de 2°
   * -> si pression plateau > consigne param => ouverture expi de 1°
   * -> si pression pep < pep mini => fermeture valve expiration complètement
   */
  
//    if(currentCentieme % 10 == 0){
//      Serial.print("Phase : ");
//      Serial.println(currentPhase);
//    }

    if(consigneBlower != positionBlower){
      blower.write(consigneBlower);
      positionBlower = consigneBlower;
    }

    if(consignePatient != positionPatient){
      patient.write(consignePatient);
      positionPatient = consignePatient;
    }
    
    previousPressionCrete = currentPressionCrete;
    previousPressionPlateau = currentPressionPlateau;
    
    delay(10); // on attends 1 centieme de seconde (on aura de la dérive en temps, sera corrigé par rtc au besoin)
  }

}
