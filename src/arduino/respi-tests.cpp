#include <Arduino.h>
#include <Servo.h>
#include <LiquidCrystal.h>
#include <OneButton.h>
#include <AnalogButtons.h>
#include <common.h>

// TODO extraire ces 2 lignes dans de la configuration
const int LCD_LINE_LENGTH = 16;
const int LCD_LINE_NUMBER = 2;

const int SECONDS = 1000;

/**
 * Liste de toutes les étapes de test du montage.
 * 
 * - TEST_STEP_START                  Affichage d'un pattern sur l'écran LCD
 * - TEST_BTN_PRESSION_PLATEAU_MINUS  Test du bouton pression plateau minus
 * - TEST_BTN_PRESSION_PLATEAU_PLUS   Test du bouton pression plateau plus
 * - TEST_BTN_PRESSION_PEP_MINUS      Test du bouton pression pression PEP minus
 * - TEST_BTN_PRESSION_PEP_PLUS       Test du bouton pression pression PEP plus
 * - TEST_BTN_CYCLE_MINUS             Test du bouton pression cyle minus
 * - TEST_BTN_CYCLE_PLUS              Test du bouton pression cycle plus
 * - TEST_STEP_STOP                   Fin du test
 */
const int TEST_STEP_START                 = 0;
const int TEST_BTN_PRESSION_PLATEAU_PLUS  = TEST_STEP_START                  + 1;
const int TEST_BTN_PRESSION_PLATEAU_MINUS = TEST_BTN_PRESSION_PLATEAU_PLUS   + 1;
const int TEST_BTN_PEP_PLUS               = TEST_BTN_PRESSION_PLATEAU_MINUS  + 1;
const int TEST_BTN_PEP_MINUS              = TEST_BTN_PEP_PLUS                + 1;
const int TEST_BTN_RB_PLUS                = TEST_BTN_PEP_MINUS               + 1;
const int TEST_BTN_RB_MINUS               = TEST_BTN_RB_PLUS                 + 1;
const int TEST_BTN_ALARME_ON              = TEST_BTN_RB_MINUS                + 1;
const int TEST_BTN_ALARME_OFF             = TEST_BTN_ALARME_ON               + 1;
const int TEST_BTN_VALVE_BLOWER_PLUS      = TEST_BTN_ALARME_OFF              + 1;
const int TEST_BTN_VALVE_BLOWER_MINUS     = TEST_BTN_VALVE_BLOWER_PLUS       + 1;
const int TEST_BTN_VALVE_PATIENT_PLUS     = TEST_BTN_VALVE_BLOWER_MINUS      + 1;
const int TEST_BTN_VALVE_PATIENT_MINUS    = TEST_BTN_VALVE_PATIENT_PLUS      + 1;
const int TEST_STEP_STOP                  = TEST_BTN_VALVE_PATIENT_MINUS     + 1;

int validatedStep = TEST_STEP_START;

/**
 * Affiche un pattern de la forme suivante sur l'écran LCD.
 * La taille du pattern s'adapte en fonction du nombre de ligne et 
 * de caractères de l'écran LCD.
 * 
 * 01234567890123456789
 * 12345678901234567890
 * 23456789012345678901
 * 34567890123456789012
 */
void printPatternOnLcd() {
  lcd.setCursor(0, 0);
  
  for (int line = 0; line < LCD_LINE_NUMBER; line++) {
    for (int character = 0; character < LCD_LINE_LENGTH; character++) {
      lcd.print((character + line) % 10);
    }
    lcd.println();
  }
}

void onPressionPlateauPlusClick() {
  #ifdef DEBUG
  Serial.println("pression plateau ++");
  #endif
  if (validatedStep == TEST_STEP_START) {
    lcd.setCursor(0, 0);
    lcd.println("Btn Plateau + OK");
    lcd.print("Push Plateau -");
    validatedStep = TEST_BTN_PRESSION_PLATEAU_PLUS;
  }
}

void onPressionPlateauMinusClick() {
  #ifdef DEBUG
  Serial.println("pression plateau --");
  #endif
  if (validatedStep == TEST_BTN_PRESSION_PLATEAU_PLUS) {
    lcd.setCursor(0, 0);
    lcd.println("Btn Plateau - OK");
    lcd.print("Push PEP +");
    validatedStep = TEST_BTN_PRESSION_PLATEAU_MINUS;
  }
}

void onPepPlusClick() {
  #ifdef DEBUG
  Serial.println("pression pep ++");
  #endif
  if (validatedStep == TEST_BTN_PRESSION_PLATEAU_MINUS) {
    lcd.setCursor(0, 0);
    lcd.println("Btn PEP + OK");
    lcd.print("Push PEP -");
    validatedStep = TEST_BTN_PEP_PLUS;
  }
}

void onPepMinusClick() {
  #ifdef DEBUG
  Serial.println("pression pep --");
  #endif
  if (validatedStep == TEST_BTN_PEP_PLUS) {
    lcd.setCursor(0, 0);
    lcd.println("Btn PEP - OK");
    lcd.print("Push RB +");
    validatedStep = TEST_BTN_PEP_MINUS;
  }
}

void onRbPlusClick() {
  #ifdef DEBUG
  Serial.println("pression RB ++ ");
  #endif
  if (validatedStep == TEST_BTN_PEP_MINUS) {
    lcd.setCursor(0, 0);
    lcd.println("Btn RB + OK");
    lcd.print("Push RB -");
    validatedStep = TEST_BTN_RB_PLUS;
  }
}

void onRbMinusClick() {
  #ifdef DEBUG
  Serial.println("pression RB -- ");
  #endif
  if (validatedStep == TEST_BTN_RB_PLUS) {
    lcd.setCursor(0, 0);
    lcd.println("Btn RB - OK");
    lcd.print("Push Alarme ON");
    validatedStep = TEST_BTN_RB_MINUS;
  }
}

void onAlarmeOnClick() {
  #ifdef DEBUG
  Serial.println("Alarme ON ");
  #endif
  if (validatedStep == TEST_BTN_RB_MINUS) {
    lcd.setCursor(0, 0);
    lcd.println("Alarme ON OK");
    lcd.print("Push Alarme OFF");
    validatedStep = TEST_BTN_ALARME_ON;
  }
}

void onAlarmeOffClick() {
  #ifdef DEBUG
  Serial.println("Alarme OFF ");
  #endif
  if (validatedStep == TEST_BTN_ALARME_ON) {
    lcd.setCursor(0, 0);
    lcd.println("Alarme OFF OK");
    lcd.print("Push Blower +");
    validatedStep = TEST_BTN_ALARME_OFF;
  }
}

void onValveBlowerPlusClick() {
  #ifdef DEBUG
  Serial.println("Valve Blower ++ ");
  #endif
  if (validatedStep == TEST_BTN_ALARME_OFF) {
    lcd.setCursor(0, 0);
    lcd.println("Btn Blower + OK");
    lcd.print("Push Blower -");
    validatedStep = TEST_BTN_VALVE_BLOWER_PLUS;
  }
}

void onValveBlowerMinusClick() {
  #ifdef DEBUG
  Serial.println("Valve Blower -- ");
  #endif
  if (validatedStep == TEST_BTN_VALVE_BLOWER_PLUS) {
    lcd.setCursor(0, 0);
    lcd.println("Btn Blower - OK");
    lcd.print("Push Patient +");
    validatedStep = TEST_BTN_VALVE_BLOWER_MINUS;
  }
}

void onValvePatientPlusClick() {
  #ifdef DEBUG
  Serial.println("Valve Patient ++ ");
  #endif
  if (validatedStep == TEST_BTN_VALVE_BLOWER_MINUS) {
    lcd.setCursor(0, 0);
    lcd.println("Btn Patient + OK");
    lcd.print("Push Patient -");
    validatedStep = TEST_BTN_VALVE_PATIENT_PLUS;
  }
}

void onValvePatientMinusClick() {
  #ifdef DEBUG
  Serial.println("Valve Patient -- ");
  #endif
  if (validatedStep == TEST_BTN_VALVE_PATIENT_PLUS) {
    lcd.setCursor(0, 0);
    lcd.println("Btn Patient - OK");
    lcd.print("Test end.");
    validatedStep = TEST_BTN_VALVE_PATIENT_MINUS;
  }
}

Button btn_pression_plateau_plus  = Button(TENSION_BTN_PRESSION_PLATEAU_PLUS , &onPressionPlateauPlusClick);
Button btn_pression_plateau_minus = Button(TENSION_BTN_PRESSION_PLATEAU_MINUS, &onPressionPlateauMinusClick);
Button btn_pep_plus               = Button(TENSION_BTN_PEP_PLUS              , &onPepPlusClick);
Button btn_pep_minus              = Button(TENSION_BTN_PEP_MINUS             , &onPepMinusClick);
Button btn_rb_plus                = Button(TENSION_BTN_RB_PLUS               , &onRbPlusClick);
Button btn_rb_minus               = Button(TENSION_BTN_RB_MINUS              , &onRbMinusClick);
Button btn_alarme_on              = Button(TENSION_BTN_ALARME_ON             , &onAlarmeOnClick);
Button btn_alarme_off             = Button(TENSION_BTN_ALARME_OFF            , &onAlarmeOffClick);
Button btn_valve_blower_plus      = Button(TENSION_BTN_VALVE_BLOWER_PLUS     , &onValveBlowerPlusClick);
Button btn_valve_blower_minus     = Button(TENSION_BTN_VALVE_BLOWER_MINUS    , &onValveBlowerMinusClick);
Button btn_valve_patient_plus     = Button(TENSION_BTN_VALVE_PATIENT_PLUS    , &onValvePatientPlusClick);
Button btn_valve_patient_minus    = Button(TENSION_BTN_VALVE_PATIENT_MINUS   , &onValvePatientMinusClick);

void setup() {
  #ifdef DEBUG
  Serial.begin(115200);
  Serial.println("demarrage");
  #endif

  patient.attach(PIN_SERVO_PATIENT);
  blower.attach(PIN_SERVO_BLOWER);

  buttons.add(btn_pression_plateau_plus);
  buttons.add(btn_pression_plateau_minus);
  buttons.add(btn_pep_plus);
  buttons.add(btn_pep_minus);
  buttons.add(btn_rb_plus);
  buttons.add(btn_rb_minus);
  buttons.add(btn_alarme_on);
  buttons.add(btn_alarme_off);
  buttons.add(btn_valve_blower_plus);
  buttons.add(btn_valve_blower_minus);

  lcd.begin(LCD_LINE_LENGTH, LCD_LINE_NUMBER);

  printPatternOnLcd();
}

void loop() {
  buttons.check();
}
