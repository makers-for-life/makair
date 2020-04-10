/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file respirator.cpp
 * @brief Entry point of ventilator program
 *****************************************************************************/

#pragma once

#include "../includes/config.h"
#if MODE == MODE_PROD

// INCLUDES ==================================================================

// External
#if HARDWARE_VERSION == 1
#include <AnalogButtons.h>
#endif
#include <Arduino.h>
#include <IWatchdog.h>
#include <LiquidCrystal.h>

// Internal
#include "../includes/activation.h"
#include "../includes/battery.h"
#include "../includes/blower.h"
#include "../includes/buzzer.h"
#include "../includes/buzzer_control.h"
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
Blower* blower_pointer;
Blower blower;

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

void setup(void) {
    DBG_DO(Serial.begin(115200);)
    DBG_DO(Serial.println("Booting the system...");)

    startScreen();

    pinMode(PIN_PRESSURE_SENSOR, INPUT);
    pinMode(PIN_BATTERY, INPUT);

#if HARDWARE_VERSION == 1
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

    blower = Blower(hardwareTimer3, TIM_CHANNEL_ESC_BLOWER, PIN_ESC_BLOWER);
    blower.setup();
    blower_pointer = &blower;
#elif HARDWARE_VERSION == 2
    // Timer for servos
    hardwareTimer3 = new HardwareTimer(TIM3);
    hardwareTimer3->setOverflow(SERVO_VALVE_PERIOD, MICROSEC_FORMAT);

    // Servo blower setup
    servoBlower = PressureValve(hardwareTimer3, TIM_CHANNEL_SERVO_VALVE_BLOWER, PIN_SERVO_BLOWER,
                                VALVE_OPEN_STATE, VALVE_CLOSED_STATE);
    servoBlower.setup();
    hardwareTimer3->resume();

    // Servo patient setup
    servoPatient = PressureValve(hardwareTimer3, TIM_CHANNEL_SERVO_VALVE_PATIENT, PIN_SERVO_PATIENT,
                                 VALVE_OPEN_STATE, VALVE_CLOSED_STATE);
    servoPatient.setup();
    hardwareTimer3->resume();

    hardwareTimer1 = new HardwareTimer(TIM1);
    blower = Blower(hardwareTimer1, TIM_CHANNEL_ESC_BLOWER, PIN_ESC_BLOWER);
    blower.setup();
    blower_pointer = &blower;
#endif

    alarmController = AlarmController();

    pController = PressureController(INITIAL_CYCLE_NUMBER, DEFAULT_MIN_PEEP_COMMAND,
                                     DEFAULT_MAX_PLATEAU_COMMAND, DEFAULT_MAX_PEAK_PRESSURE_COMMAND,
                                     servoBlower, servoPatient, &alarmController, blower_pointer);
    pController.setup();

    // Prepare LEDs
    pinMode(PIN_LED_RED, OUTPUT);
    pinMode(PIN_LED_YELLOW, OUTPUT);
    pinMode(PIN_LED_GREEN, OUTPUT);

    initKeyboard();
    initBattery();

    BuzzerControl_Init();
    Buzzer_Init();

    // escBlower needs 5s at speed 0 to be properly initalized

    // RCM-SW-17 (Christmas tree at startup)
    Buzzer_Boot_Start();
    digitalWrite(PIN_LED_GREEN, LED_GREEN_ACTIVE);
    digitalWrite(PIN_LED_RED, LED_RED_ACTIVE);
    digitalWrite(PIN_LED_YELLOW, LED_YELLOW_ACTIVE);
    waitForInMs(1000);
    digitalWrite(PIN_LED_GREEN, LED_GREEN_INACTIVE);
    digitalWrite(PIN_LED_RED, LED_RED_INACTIVE);
    digitalWrite(PIN_LED_YELLOW, LED_YELLOW_INACTIVE);

    waitForInMs(4000);

    lastpControllerComputeDate = millis();

    // Catch potential Watchdog reset
    if (IWatchdog.isReset(true)) {
        // TODO holdExhale
        // Display something ?
        // Alarm code ?

        Buzzer_Init();
        Buzzer_High_Prio_Start();
        while (1) {
        }
    }

    // Init the watchdog timer. It must be reloaded frequently otherwise MCU resests
    IWatchdog.begin(WATCHDOG_TIMEOUT);
    IWatchdog.reload();
}

// Time of the previous loop iteration
int32_t lastMicro = 0;

// Number of cycles before LCD screen reset
// (because this kind of screen is not reliable, we need to reset it every 5 min or so)
int8_t cyclesBeforeScreenReset = LCD_RESET_PERIOD * CONST_MIN_CYCLE;

void loop(void) {
    /********************************************/
    // INITIALIZE THE RESPIRATORY CYCLE
    /********************************************/
    bool shouldRun = activationController.isRunning();
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

            if (shouldRun) {
                int32_t currentMicro = micros();

                pController.updateDt(currentMicro - lastMicro);
                lastMicro = currentMicro;

                // Perform the pressure control
                pController.compute(centiSec);
            } else {
                blower.stop();
            }

            // Check if some buttons have been pushed
            keyboardLoop();

            // Check if battery state has changed
            batteryLoop(pController.cycleNumber());

            // Display relevant information during the cycle
            if ((centiSec % LCD_UPDATE_PERIOD) == 0u) {
                displayCurrentPressure(pController.pressure(),
                                       pController.cyclesPerMinuteCommand());

                displayCurrentSettings(pController.maxPeakPressureCommand(),
                                       pController.maxPlateauPressureCommand(),
                                       pController.minPeepCommand());
                if (activationController.isRunning()) {
                    displayCurrentInformation(pController.peakPressure(),
                                              pController.plateauPressure(), pController.peep());
                } else {
                    displayMachineStopped();
                }
            }

            alarmController.runAlarmEffects(centiSec);

            // next tick
            centiSec++;
            IWatchdog.reload();
        }
    }

    /********************************************/
    // END OF THE RESPIRATORY CYCLE
    /********************************************/

    // Because this kind of LCD screen is not reliable, we need to reset it every 5 min or so
    cyclesBeforeScreenReset--;
    if (cyclesBeforeScreenReset <= 0) {
        DBG_DO(Serial.println("resetting LCD screen");)
        resetScreen();
        clearAlarmDisplayCache();
        cyclesBeforeScreenReset = LCD_RESET_PERIOD * CONST_MIN_CYCLE;
    }
}

#endif
