#include <Arduino.h>
#include <Servo.h>
#include <LiquidCrystal.h>
#include <OneButton.h>
#include <common.h>

const int SECONDS = 1000;

const int TEST_STEP_START                 = 0;
const int TEST_BTN_PRESSION_PLATEAU_MINUS = TEST_STEP_START + 1;
const int TEST_BTN_PRESSION_PLATEAU_PLUS  = TEST_BTN_PRESSION_PLATEAU_MINUS + 1;
const int TEST_BTN_PRESSION_PEP_MINUS     = TEST_BTN_PRESSION_PLATEAU_PLUS + 1;
const int TEST_BTN_PRESSION_PEP_PLUS      = TEST_BTN_PRESSION_PEP_MINUS + 1;
const int TEST_BTN_CYCLE_MINUS            = TEST_BTN_PRESSION_PEP_PLUS + 1;
const int TEST_BTN_CYCLE_PLUS             = TEST_BTN_CYCLE_MINUS + 1;
const int TEST_STEP_STOP                  = TEST_BTN_CYCLE_PLUS + 1;

char *messages [TEST_STEP_STOP + 1] = {
  "Start test in 3s", //16 characters
  "Push Plateau- OK",
  "Push Plateau+ OK",
  "Push PEP- OK",
  "Push PEP+ OK",
  "Push Cycle- OK",
  "Push Cycle+ OK", 
  "Test done."
};

int currentStep = TEST_STEP_START;

void onPressionPlateauMinus() {
  #ifdef DEBUG
  Serial.println("pression plateau --");
  #endif
  if (currentStep == TEST_BTN_PRESSION_PLATEAU_MINUS) {
    currentStep++;
  }
}

void onPressionPlateauPlus() {
  #ifdef DEBUG
  Serial.println("pression plateau ++");
  #endif
  if (currentStep == TEST_BTN_PRESSION_PLATEAU_PLUS) {
    currentStep++;
  }
}

void onPressionPepMinus() {
  #ifdef DEBUG
  Serial.println("pression PEP --");
  #endif
  if (currentStep == TEST_BTN_PRESSION_PEP_MINUS) {
    currentStep++;
  }
}

void onPressionPepPlus() {
  #ifdef DEBUG
  Serial.println("pression PEP ++");
  #endif
  if (currentStep == TEST_BTN_PRESSION_PEP_PLUS) {
    currentStep++;
  }
}

void onCycleMinus() {
  #ifdef DEBUG
  Serial.println("nb cycle --");
  #endif
  if (currentStep ==  TEST_BTN_CYCLE_MINUS) {
    currentStep++;
  }
}

void onCyclePlus() {
  #ifdef DEBUG
  Serial.println("nb cycle ++");
  #endif
  if (currentStep == TEST_BTN_CYCLE_PLUS) {
    currentStep++;
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
  
  /********************************************/
  // Écoute des appuis boutons
  /********************************************/
  btn_pression_plateau_minus.tick();
  btn_pression_plateau_plus.tick();
  btn_pression_pep_minus.tick();
  btn_pression_pep_plus.tick();
  btn_cycle_minus.tick();
  btn_cycle_plus.tick();

  lcd.setCursor(0, 0);
  lcd.print(messages[currentStep]);

  if (currentStep == TEST_STEP_START) {
    delay(3 * SECONDS);
    currentStep++;
  }
}
