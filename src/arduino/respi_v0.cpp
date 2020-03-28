/*=============================================================================
 * @file respi_v0.cpp
 *
 * COVID Respirator
 *
 * @section copyright Copyright
 *
 * Makers For Life
 *
 * @section descr File description
 *
 * Initialisation du logiciel et boucle principale.
 */

#if 0

#include <AnalogButtons.h>
#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Servo.h>

#include "affichage.h"
#include "config.h"
#include "debug.h"

// Période en ms de la boucle de traitement
const uint32_t PERIODE_DE_TRAITEMENT = 10ul;

// amplitude radiale des servomoteurs
const int ANGLE_OUVERTURE_MINI = 8;
const int ANGLE_OUVERTURE_MAXI = 45;

// multiplicateur à modifier pour inverser les angles (en cas de suppression de l'engrenage)
const int ANGLE_MULTIPLICATEUR = 1;

// borne pour le capteur de pression
const int CAPT_PRESSION_MINI = 0;   // a adapter lors de la calibration
const int CAPT_PRESSION_MAXI = 800; // on ne va pas jusqu'à 1024 à cause de la saturation de l'AOP
                                    // -> à adapter avec meilleur AOP

// servomoteur blower : connecte le flux d'air vers le Air Transistor patient ou vers l'extérieur
// 90° → tout est fermé
// entre 45° (90 - ANGLE_OUVERTURE_MAXI) et 82° (90 - ANGLE_OUVERTURE_MINI) → envoi du flux vers
// l'extérieur entre 98° (90 + ANGLE_OUVERTURE_MINI) et 135° (90 + ANGLE_OUVERTURE_MAXI) → envoi du
// flux vers le Air Transistor patient
Servo blower;

// servomoteur patient : connecte le patient au flux d'air entrant ou à l'extérieur
// 90° → tout est fermé
// entre 45° (90 - ANGLE_OUVERTURE_MAXI) et 82° (90 - ANGLE_OUVERTURE_MINI) → envoi du flux vers le
// patient entre 98° (90 + ANGLE_OUVERTURE_MINI) et 135° (90 + ANGLE_OUVERTURE_MAXI) → échappe l'air
// du patient vers l'extérieur
Servo patient;

// entrées-sorties
const int PIN_CAPTEUR_PRESSION = A4;
const int PIN_SERVO_BLOWER = 4;  // D4
const int PIN_SERVO_PATIENT = 2; // D2
#define ANALOG_PIN A0
const int BTN_FREE2 = 913;
const int BTN_FREE1 = 821;
const int BTN_ALARM_OFF = 745;
const int BTN_ALARM_ON = 607;
const int BTN_CYCLE_MINUS = 509;
const int BTN_CYCLE_PLUS = 413;
const int BTN_PRESSION_PEP_MINUS = 292;
const int BTN_PRESSION_PEP_PLUS = 215;
const int BTN_PRESSION_PLATEAU_MINUS = 109;
const int BTN_PRESSION_PLATEAU_PLUS = 0;

// contrôle de l'écran LCD
const int rs = 7, en = 8, d4 = 9, d5 = 10, d6 = 11, d7 = 12;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const ScreenSize screenSize{ScreenSize::CHARS_20};
const int LCD_UPDATE_PERIOD =
    20; // période (en centièmes de secondes) de mise à jour du feedback des consignes sur le LCD

// phases possibles du cycle
const int PHASE_PUSH_INSPI =
    1; // pousée d'inspiration : on envoie l'air jusqu'à la pression crête paramétrée : valve blower
       // ouverte à consigne, flux d'air vers le patient
const int PHASE_HOLD_INSPI =
    2; // plateau d'inspiration : on a depassé la pression crête, la pression descend depuis plus
       // d'un 1/10sec (arbitraire EFL) : 2 valves fermées
const int PHASE_EXPIRATION =
    3; // expiration : flux d'air vers l'extérieur, air patient vers l'extérieur
const int PHASE_HOLD_EXPI =
    4; // expiration bloquée : les valves sont fermées car la pression est en dessous de la PEP

// minimums et maximums possible des paramètres modifiables à l'exécution
const int BORNE_SUP_PRESSION_CRETE = 70;   // arbitraire
const int BORNE_INF_PRESSION_CRETE = 10;   // arbitraire
const int BORNE_SUP_PRESSION_PLATEAU = 30; // PP MAX SDRA = 30
const int BORNE_INF_PRESSION_PLATEAU = 10; // arbitraire
const int BORNE_SUP_PRESSION_PEP = 30;     // PP MAX = 30, or PEP < PP
const int BORNE_INF_PRESSION_PEP = 5;      // arbitraire mais > 0

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

// degré d'ouverture de la valve blower (quantité d'air du blower qu'on envoie vers le Air
// Transistor patient)
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

void onFree2()
{
#ifdef DEBUG
    Serial.println("free2");
#endif
}

void onFree1()
{
#ifdef DEBUG
    Serial.println("free1");
#endif
}

void onAlarmOff()
{
#ifdef DEBUG
    Serial.println("alarm OFF");
#endif
}

void onAlarmOn()
{
#ifdef DEBUG
    Serial.println("alarm ON");
#endif
}

void onCycleMinus()
{
#ifdef DEBUG
    Serial.println("nb cycle --");
#endif
    futureConsigneNbCycle--;
    if (futureConsigneNbCycle < BORNE_INF_CYCLE)
    {
        futureConsigneNbCycle = BORNE_INF_CYCLE;
    }
}

void onCyclePlus()
{
#ifdef DEBUG
    Serial.println("nb cycle ++");
#endif
    futureConsigneNbCycle++;
    if (futureConsigneNbCycle > BORNE_SUP_CYCLE)
    {
        futureConsigneNbCycle = BORNE_SUP_CYCLE;
    }
}

void onPressionPepMinus()
{
#ifdef DEBUG
    Serial.println("pression PEP --");
#endif
    futureConsignePressionPEP--;
    if (futureConsignePressionPEP < BORNE_INF_PRESSION_PEP)
    {
        futureConsignePressionPEP = BORNE_INF_PRESSION_PEP;
    }
}

void onPressionPepPlus()
{
#ifdef DEBUG
    Serial.println("pression PEP ++");
#endif
    futureConsignePressionPEP++;
    if (futureConsignePressionPEP > BORNE_SUP_PRESSION_PEP)
    {
        futureConsignePressionPEP = BORNE_SUP_PRESSION_PEP;
    }
}

void onPressionPlateauMinus()
{
#ifdef DEBUG
    Serial.println("pression plateau --");
#endif
    futureConsignePressionPlateauMax--;
    if (futureConsignePressionPlateauMax < BORNE_INF_PRESSION_PLATEAU)
    {
        futureConsignePressionPlateauMax = BORNE_INF_PRESSION_PLATEAU;
    }
}

void onPressionPlateauPlus()
{
#ifdef DEBUG
    Serial.println("pression plateau ++");
#endif
    futureConsignePressionPlateauMax++;
    if (futureConsignePressionPlateauMax > BORNE_SUP_PRESSION_PLATEAU)
    {
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

void setup()
{

    pinMode(PIN_CAPTEUR_PRESSION, INPUT);

    DBG_DO(Serial.begin(115200);)
    DBG_DO(Serial.println("demarrage");)

    patient.attach(PIN_SERVO_PATIENT);
    blower.attach(PIN_SERVO_BLOWER);

    DBG_DO(Serial.print("mise en secu initiale");)

    blower.write(secu_coupureBlower);
    patient.write(secu_ouvertureExpi);

    switch (screenSize)
    {
    case ScreenSize::CHARS_16:
    {
        lcd.begin(16, 2);
        break;
    }
    case ScreenSize::CHARS_20:
    {
        lcd.begin(20, 2);
        break;
    }
    default:
    {
        lcd.begin(16, 2);
    }
    }

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
        unsigned int value = analogRead(ANALOG_PIN);
        Serial.println(value);
        delay(250);
}
*/

void loop()
{
    int nbreCentiemeSecParCycle = 60 * 100 / consigneNbCycle;
    int nbreCentiemeSecParInspi =
        nbreCentiemeSecParCycle / 3; // inspiration = 1/3 du cycle, expiration = 2/3 du cycle

    DBG_AFFICHE_CSPCYCLE_CSPINSPI(nbreCentiemeSecParCycle, nbreCentiemeSecParInspi)

    int currentPressionCrete = -1;
    int currentPressionPlateau = -1;
    int currentPressionPep = -1;

    // int currentPositionBlower = secu_coupureBlower;

    // int dureeBaissePression = 0; // compteur de centièmes pour la détection du pic de pression
    // (pression crête) (unused so far)

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

    DBG_AFFICHE_CONSIGNES(consigneNbCycle, consigneOuverture, consignePressionPEP,
                          consignePressionPlateauMax)

    /********************************************/
    // Affichage une fois par cycle respiratoire
    /********************************************/
    displayEveryCycle(lcd, screenSize, previousPressionCrete, previousPressionPlateau,
                      previousPressionPep);

    /********************************************/
    // Début d'un cycle
    /********************************************/
    int currentCentieme = 0;

    while (currentCentieme < nbreCentiemeSecParCycle)
    {

        static uint32_t dateDernierTraitement = 0ul;
        uint32_t dateCourante = millis();
        if (dateCourante - dateDernierTraitement >= PERIODE_DE_TRAITEMENT)
        {

            currentCentieme++;

            /********************************************/
            // Le traitement est effectué toutes les
            // PERIODE_DE_TRAITEMENT millisecondes
            // Note sur la gestion du temps. La date en
            // millisecondes de l'Arduino déborde tous
            // les 40 jours environ. Il faut donc comparer
            // des intervalles et non des dates absolues
            // Sinon, au bout de 40 jours, ça plante.
            /********************************************/
            dateDernierTraitement = dateCourante;

/********************************************/
// Mesure pression pour rétro-action
/********************************************/
#ifdef SIMULATION
            int currentPression = 0;
            if (currentCentieme < 50)
            {
                currentPression = 60;
            }
            else
            {
                currentPression = 30;
            }
            if (currentCentieme > nbreCentiemeSecParInspi)
            {
                currentPression = 5;
            }
#else
            int currentPression = map(analogRead(PIN_CAPTEUR_PRESSION), 194, 245, 0, 600) / 10;
#endif

            /********************************************/
            // Calcul des consignes normales
            /********************************************/
            if (currentCentieme <= nbreCentiemeSecParInspi)
            { // on est dans la phase temporelle d'inspiration (poussée puis plateau)
                if (currentPression >= currentPressionCrete)
                {
                    currentPhase = PHASE_PUSH_INSPI;
                    currentPressionCrete = currentPression;

                    consigneBlower = 90
                                     - ANGLE_MULTIPLICATEUR
                                           * consigneOuverture; // on ouvre le blower vers patient à
                                                                // la consigne paramétrée
                    consignePatient = 90
                                      + ANGLE_MULTIPLICATEUR
                                            * ANGLE_OUVERTURE_MAXI; // on ouvre le flux IN patient
                }
                else
                {
                    currentPhase = PHASE_HOLD_INSPI;
                    currentPressionPlateau = currentPression;

                    consigneBlower =
                        90
                        + ANGLE_MULTIPLICATEUR * ANGLE_OUVERTURE_MAXI; // on shunt vers l'extérieur
                    consignePatient = 90;                              // on bloque les flux patient
                }
            }
            else
            { // on gère l'expiration on est phase PHASE_EXPIRATION
                currentPhase = PHASE_EXPIRATION;
                currentPressionPep = currentPression;
                consigneBlower =
                    90 + ANGLE_MULTIPLICATEUR * ANGLE_OUVERTURE_MAXI; // on shunt vers l'extérieur
                consignePatient = secu_ouvertureExpi; // on ouvre le flux OUT patient (expiration
                                                      // vers l'extérieur)
            }

            /********************************************/
            // Calcul des consignes de mise en sécurité
            /********************************************/
            // si pression crête > max, alors fermeture blower de 2°
            if (currentPression > consignePressionCrete)
            {
                DBG_PRESSION_CRETE(currentCentieme, 80)
                consigneBlower = positionBlower - 2;
            }
            // si pression plateau > consigne param, alors ouverture expiration de 1°
            if (currentPhase == PHASE_HOLD_INSPI && currentPression > consignePressionPlateauMax)
            {
                DBG_PRESSION_PLATEAU(currentCentieme, 80)
                consignePatient = positionBlower + 1;
            }
            // si pression PEP < PEP mini, alors fermeture complète valve expiration
            if (currentPression < consignePressionPEP)
            {
                DBG_PRESSION_PEP(currentCentieme, 80)
                consignePatient = 90;
                currentPhase = PHASE_HOLD_EXPI;
            }

            DBG_PHASE_PRESSION(currentCentieme, 50, currentPhase, currentPression)

            /********************************************/
            // Envoi des nouvelles valeurs aux actionneurs
            /********************************************/
            if (consigneBlower != positionBlower)
            {
                blower.write(consigneBlower);
                positionBlower = consigneBlower;
            }

            if (consignePatient != positionPatient)
            {
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
            // calibrateButtons();

            /********************************************/
            // Affichage pendant le cycle
            /********************************************/
            if (currentCentieme % LCD_UPDATE_PERIOD == 0)
            {
                displayDuringCycle(lcd, screenSize, futureConsigneNbCycle,
                                   futureConsignePressionPlateauMax, futureConsignePressionPEP,
                                   currentPression);
            }
        }
    }
    /********************************************/
    // Fin du cycle
    /********************************************/
}

#endif
