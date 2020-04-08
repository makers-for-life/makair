/*
    Copyright (C) 2020 Makers For Life
*/
/******************************************************************************
 * @author Makers For Life
 * @file respirator.cpp
 * @brief Entry point of ventilator program
 *****************************************************************************/

#pragma once

#include "../includes/config.h"
#if MODE == MODE_PROD

// INCLUDES ==================================================================

// External
#include <AnalogButtons.h>
#include <Arduino.h>
#include <IWatchdog.h>
#include <LiquidCrystal.h>

// Internal
#include "../includes/buzzer.h"
#include "../includes/common.h"
#include "../includes/debug.h"
#include "../includes/keyboard.h"
#include "../includes/parameters.h"
#include "../includes/pression.h"
#include "../includes/pressure_controller.h"
#include "../includes/pressure_valve.h"
#include "../includes/screen.h"

// PROGRAM =====================================================================

PressureValve servoBlower;
PressureValve servoPatient;
HardwareTimer* hardwareTimer1;
HardwareTimer* hardwareTimer3;

/**
 * Block execution for a given duration
 *
 * @param ms  Duration of the blocking in millisecond
 */
void waitForInMs(uint16_t ms) {
    uint16_t start = millis();
    while ((millis() - start) < ms) {
        continue;
    }
}

uint32_t lastpControllerComputeDate;

void setup() {
    /* Catch potential Watchdog reset */
    if (IWatchdog.isReset(true)) {
        /* Code in case of Watchdog detected */
        /* TODO */
        Buzzer_Init();
        Buzzer_High_Prio_Start();
        while (1) {
        }
    }

    DBG_DO(Serial.begin(115200);)
    DBG_DO(Serial.println("Booting the system...");)

    startScreen();

    pinMode(PIN_PRESSURE_SENSOR, INPUT);
    pinMode(PIN_BUZZER, OUTPUT);

    // Timer for servoBlower
    hardwareTimer1 = new HardwareTimer(TIM1);
    hardwareTimer1->setOverflow(SERVO_VALVE_PERIOD, MICROSEC_FORMAT);

    // Timer for servoPatient and escBlower
    hardwareTimer3 = new HardwareTimer(TIM3);
    hardwareTimer3->setOverflow(SERVO_VALVE_PERIOD, MICROSEC_FORMAT);

    // Servo blower setup
    servoBlower = PressureValve(hardwareTimer1, TIM_CHANNEL_SERVO_VALVE_BLOWER, PIN_SERVO_BLOWER,
                                VALVE_OPEN_STATE, VALVE_CLOSED_STATE);

    servoBlower.setup();
    hardwareTimer1->resume();

    // Servo patient setup
    servoPatient = PressureValve(hardwareTimer3, TIM_CHANNEL_SERVO_VALVE_PATIENT, PIN_SERVO_PATIENT,
                                 VALVE_OPEN_STATE, VALVE_CLOSED_STATE);
    servoPatient.setup();

    // Manual escBlower setup
    // Output compare activation on pin PIN_ESC_BLOWER
    hardwareTimer3->setMode(TIM_CHANNEL_ESC_BLOWER, TIMER_OUTPUT_COMPARE_PWM1, PIN_ESC_BLOWER);
    // Set PPM width to 1ms
    hardwareTimer3->setCaptureCompare(TIM_CHANNEL_ESC_BLOWER, BlowerSpeed2MicroSeconds(0),
                                      MICROSEC_COMPARE_FORMAT);
    hardwareTimer3->resume();

    AlarmController alarmController = AlarmController();

    pController = PressureController(INITIAL_CYCLE_NUMBER, DEFAULT_MIN_PEEP_COMMAND,
                                     DEFAULT_MAX_PLATEAU_COMMAND, DEFAULT_MAX_PEAK_PRESSURE_COMMAND,
                                     servoBlower, servoPatient, alarmController);
    pController.setup();

    // Prepare LEDs
    pinMode(PIN_LED_RED, OUTPUT);
    pinMode(PIN_LED_YELLOW, OUTPUT);
    pinMode(PIN_LED_GREEN, OUTPUT);

    initKeyboard();

    Buzzer_Init();

    // escBlower needs 5s at speed 0 to be properly initalized

    // RCM-SW-17 (Christmas tree at startup)
    Buzzer_Boot_Start();
    digitalWrite(PIN_LED_GREEN, HIGH);
    digitalWrite(PIN_LED_RED, HIGH);
    digitalWrite(PIN_LED_YELLOW, HIGH);
    waitForInMs(1000);
    digitalWrite(PIN_LED_GREEN, LOW);
    digitalWrite(PIN_LED_RED, LOW);
    digitalWrite(PIN_LED_YELLOW, LOW);

    waitForInMs(4000);

    // escBlower start
    hardwareTimer3->setCaptureCompare(TIM_CHANNEL_ESC_BLOWER, BlowerSpeed2MicroSeconds(170),
                                      MICROSEC_COMPARE_FORMAT);
    DBG_DO(Serial.println("Blower is running.");)

    // Init the watchdog timer. It must be reloaded frequently otherwise MCU resests
    IWatchdog.begin(WATCHDOG_TIMEOUT);
    IWatchdog.reload();

    lastpControllerComputeDate = millis();
}

// Time of the previous loop iteration
int32_t lastMicro = 0;

// Number of cycles before LCD screen reset
// (because this kind of screen is not reliable, we need to reset it every 5 min or so)
int8_t cyclesBeforeScreenReset = LCD_RESET_PERIOD * CONST_MIN_CYCLE;

void loop() {
    /********************************************/
    // INITIALIZE THE RESPIRATORY CYCLE
    /********************************************/

    pController.initRespiratoryCycle();

    /********************************************/
    // START THE RESPIRATORY CYCLE
    /********************************************/
    uint16_t centiSec = 0;

    while (centiSec < pController.centiSecPerCycle()) {
        pController.updatePressure(readPressureSensor(centiSec));

        uint32_t currentDate = millis();

        uint32_t diff = (currentDate - lastpControllerComputeDate);

        if (diff >= PCONTROLLER_COMPUTE_PERIOD) {
            lastpControllerComputeDate = currentDate;
            Serial.println(diff);
            int32_t currentMicro = micros();

            pController.updateDt(currentMicro - lastMicro);
            lastMicro = currentMicro;

            // Perform the pressure control
            pController.compute(centiSec);

            // Check if some buttons have been pushed
            keyboardLoop();

            // Display relevant information during the cycle
            if ((centiSec % LCD_UPDATE_PERIOD) == 0u) {
                displaySubPhase(pController.subPhase());

                displayCurrentInformation(pController.peakPressure(), pController.plateauPressure(),
                                          pController.peep(), pController.pressure());

                displaySettings(pController.maxPeakPressureCommand(),
                                pController.maxPlateauPressureCommand(),
                                pController.minPeepCommand(), pController.cyclesPerMinuteCommand());
            }

            // next tick
            centiSec++;
        }
        IWatchdog.reload();
    }

    /********************************************/
    // END OF THE RESPIRATORY CYCLE
    /********************************************/

    // Because this kind of LCD screen is not reliable, we need to reset it every 5 min or so
    cyclesBeforeScreenReset--;
    if (cyclesBeforeScreenReset <= 0) {
        DBG_DO(Serial.println("resetting LCD screen");)
        resetScreen();
        cyclesBeforeScreenReset = LCD_RESET_PERIOD * CONST_MIN_CYCLE;
    }
}

#endif
