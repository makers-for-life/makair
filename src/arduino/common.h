//#define DEBUG // décommenter pour envoyer les messages de debug en série
//#define SIMULATION // décommenter pour simuler des valeurs de capteur de pression au lieu de lire les vraies

// amplitude radiale des servomoteurs
const int ANGLE_OUVERTURE_MINI = 8;
const int ANGLE_OUVERTURE_MAXI = 45;

// multiplicateur à modifier pour inverser les angles (en cas de suppression de l'engrenage)
const int ANGLE_MULTIPLICATEUR = 1;

// borne pour le capteur de pression
const int CAPT_PRESSION_MINI = 0; // a adapter lors de la calibration
const int CAPT_PRESSION_MAXI = 800; // on ne va pas jusqu'à 1024 à cause de la saturation de l'AOP -> à adapter avec meilleur AOP

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

// contrôle de l'écran LCD
const int rs = 7, en = 8, d4 = 9, d5 = 10, d6 = 11, d7 = 12;
#define LCD_20_CHARS // commenter pour utiliser un écran LCD 16 caratères
const int LCD_UPDATE_PERIOD = 20; // période (en centièmes de secondes) de mise à jour du feedback des consignes sur le LCD

// phases possibles du cycle
const int PHASE_PUSH_INSPI = 1; // pousée d'inspiration : on envoie l'air jusqu'à la pression crête paramétrée : valve blower ouverte à consigne, flux d'air vers le patient
const int PHASE_HOLD_INSPI = 2; // plateau d'inspiration : on a depassé la pression crête, la pression descend depuis plus d'un 1/10sec (arbitraire EFL) : 2 valves fermées
const int PHASE_EXPIRATION = 3; // expiration : flux d'air vers l'extérieur, air patient vers l'extérieur
const int PHASE_HOLD_EXPI  = 4; // expiration bloquée : les valves sont fermées car la pression est en dessous de la PEP

// minimums et maximums possible des paramètres modifiables à l'exécution
const int BORNE_SUP_PRESSION_CRETE = 70; // arbitraire
const int BORNE_INF_PRESSION_CRETE = 10; // arbitraire
const int BORNE_SUP_PRESSION_PLATEAU = 30; // PP MAX SDRA = 30
const int BORNE_INF_PRESSION_PLATEAU = 10; // arbitraire
const int BORNE_SUP_PRESSION_PEP = 30; // PP MAX = 30, or PEP < PP
const int BORNE_INF_PRESSION_PEP = 5; // arbitraire mais > 0

const int BORNE_SUP_CYCLE = 35; // demande medical
const int BORNE_INF_CYCLE = 5;  // demande medical

// durée d'appui des boutons (en centièmes de secondes) avant prise en compte
const int MAINTIEN_PARAMETRAGE = 21;

// valeurs de sécurité pour les actionneurs
int secu_coupureBlower = 45;
int secu_ouvertureExpi = 45;

// nombre de cycles par minute (cycle = inspi + plateau + expi)
int consigneNbCycle = 20;
int futureConsigneNbCycle = consigneNbCycle;

// degré d'ouverture de la valve blower (quantité d'air du blower qu'on envoie vers le Air Transistor patient)
int consigneOuverture = 45;
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

OneButton btn_pression_plateau_minus(BTN_PRESSION_PLATEAU_MINUS, true, true);
OneButton btn_pression_plateau_plus(BTN_PRESSION_PLATEAU_PLUS, true, true);
OneButton btn_pression_pep_minus(BTN_PRESSION_PEP_MINUS, true, true);
OneButton btn_pression_pep_plus(BTN_PRESSION_PEP_PLUS, true, true);
OneButton btn_cycle_minus(BTN_NOMBRE_CYCLE_MINUS, true, true);
OneButton btn_cycle_plus(BTN_NOMBRE_CYCLE_PLUS, true, true);

// contrôle de l'écran LCD
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);