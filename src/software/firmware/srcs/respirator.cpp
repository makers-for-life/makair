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
#include "Arduino.h"
#include <IWatchdog.h>
#include <LiquidCrystal.h>
#if HARDWARE_VERSION == 2
#include <HardwareSerial.h>
#endif

// Internal
#include "../includes/activation.h"
#include "../includes/battery.h"
#include "../includes/blower.h"
#include "../includes/buzzer.h"
#include "../includes/buzzer_control.h"
#include "../includes/debug.h"
#include "../includes/keyboard.h"
#include "../includes/parameters.h"
#include "../includes/pressure.h"
#include "../includes/pressure_controller.h"
#include "../includes/pressure_valve.h"
#include "../includes/screen.h"
#if HARDWARE_VERSION == 2
#include "../includes/telemetry.h"
#endif

// PROGRAM =====================================================================

PressureValve servoBlower;
PressureValve servoPatient;
HardwareTimer* hardwareTimer1;
HardwareTimer* hardwareTimer3;
Blower* blower_pointer;
Blower blower;

int16_t pressureOffset;
int32_t pressureOffsetSum;
uint32_t pressureOffsetCount;
int16_t minOffsetValue = 0;
int16_t maxOffsetValue = 0;

#if HARDWARE_VERSION == 2
HardwareSerial Serial6(PIN_TELEMETRY_SERIAL_RX, PIN_TELEMETRY_SERIAL_TX);
#endif

/**
 * Block execution for a given duration
 *
 * @param ms  Duration of the blocking in millisecond
 */
void waitForInMs(uint16_t ms) {
    uint16_t start = millis();
    minOffsetValue = readPressureSensor(0, 0);
    maxOffsetValue = readPressureSensor(0, 0);
    pressureOffsetSum = 0;
    pressureOffsetCount = 0;

    while ((millis() - start) < ms) {
        // Measure 1 pressure per ms we wait
        if ((millis() - start) > pressureOffsetCount) {
            int16_t pressureValue = readPressureSensor(0, 0);
            pressureOffsetSum += pressureValue;
            minOffsetValue = min(pressureValue, minOffsetValue);
            maxOffsetValue = max(pressureValue, maxOffsetValue);
            pressureOffsetCount++;
        }
        continue;
    }
}

uint32_t lastpControllerComputeDate;

void setup(void) {
    DBG_DO(Serial.begin(115200);)
    DBG_DO(Serial.println("Booting the system...");)

#if HARDWARE_VERSION == 2
    initTelemetry();
    sendBootMessage();
#endif

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
    hardwareTimer1->setOverflow(ESC_PPM_PERIOD, MICROSEC_FORMAT);
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
    pinMode(PIN_LED_START, OUTPUT);
    pinMode(PIN_LED_RED, OUTPUT);
    pinMode(PIN_LED_YELLOW, OUTPUT);
    pinMode(PIN_LED_GREEN, OUTPUT);

    initKeyboard();

    // Initialize battery level estimation
    initBattery();

    BuzzerControl_Init();
    Buzzer_Init();

    // escBlower needs 5s at speed 0 to be properly initalized

    // RCM-SW-17 (Christmas tree at startup)
    Buzzer_Boot_Start();
    digitalWrite(PIN_LED_START, LED_START_ACTIVE);
    digitalWrite(PIN_LED_GREEN, LED_GREEN_ACTIVE);
    digitalWrite(PIN_LED_RED, LED_RED_ACTIVE);
    digitalWrite(PIN_LED_YELLOW, LED_YELLOW_ACTIVE);
    waitForInMs(1000);
    digitalWrite(PIN_LED_START, LED_START_INACTIVE);
    digitalWrite(PIN_LED_GREEN, LED_GREEN_INACTIVE);
    digitalWrite(PIN_LED_RED, LED_RED_INACTIVE);
    digitalWrite(PIN_LED_YELLOW, LED_YELLOW_INACTIVE);
    waitForInMs(3000);

    screen.setCursor(0, 0);
    screen.print("Calibrating P offset");
    screen.setCursor(0, 2);
    screen.print("Patient must be");
    screen.setCursor(0, 3);
    screen.print("unplugged");
    waitForInMs(3000);

    resetScreen();
    pressureOffset = pressureOffsetSum / static_cast<int32_t>(pressureOffsetCount);
    DBG_DO({
        Serial.print("pressure offset = ");
        Serial.print(pressureOffsetSum);
        Serial.print(" / ");
        Serial.print(pressureOffsetCount);
        Serial.print(" = ");
        Serial.print(pressureOffset);
        Serial.println();
    })

    // Happens when patient is plugged at starting
    if ((maxOffsetValue - minOffsetValue) >= 10) {
        resetScreen();
        screen.setCursor(0, 0);
        char line1[SCREEN_LINE_LENGTH + 1];
        (void)snprintf(line1, SCREEN_LINE_LENGTH + 1, "P offset is unstable");
        screen.print(line1);
        screen.setCursor(0, 1);
        char line2[SCREEN_LINE_LENGTH + 1];
        (void)snprintf(line2, SCREEN_LINE_LENGTH + 1, "Max-Min: %3d mmH2O",
                       maxOffsetValue - minOffsetValue);
        screen.print(line2);
        screen.setCursor(0, 2);
        screen.print("Unplug patient and");
        screen.setCursor(0, 3);
        screen.print("reboot");
        Buzzer_High_Prio_Start();
        while (true) {
        }
    }

    if (pressureOffset >= MAX_PRESSURE_OFFSET) {
        resetScreen();
        screen.setCursor(0, 0);
        char line1[SCREEN_LINE_LENGTH + 1];
        (void)snprintf(line1, SCREEN_LINE_LENGTH + 1, "P offset: %3d mmH2O", pressureOffset);
        screen.print(line1);
        screen.setCursor(0, 1);
        char line2[SCREEN_LINE_LENGTH + 1];
        (void)snprintf(line2, SCREEN_LINE_LENGTH + 1, "P offset is > %-3d", MAX_PRESSURE_OFFSET);
        screen.print(line2);
        screen.setCursor(0, 2);
        screen.print("Unplug patient and");
        screen.setCursor(0, 3);
        screen.print("reboot");
        Buzzer_High_Prio_Start();
        while (true) {
        }
    }

    screen.setCursor(0, 3);
    char message[SCREEN_LINE_LENGTH + 1];
    (void)snprintf(message, SCREEN_LINE_LENGTH + 1, "P offset: %3d mmH2O", pressureOffset);
    screen.print(message);
    waitForInMs(1000);

    lastpControllerComputeDate = millis();

    // Catch potential Watchdog reset
    // cppcheck-suppress misra-c2012-14.4 ; unknown external signature
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
int8_t cyclesBeforeScreenReset = LCD_RESET_PERIOD * (int8_t)CONST_MIN_CYCLE;

// cppcheck-suppress unusedFunction
void loop(void) {
    /********************************************/
    // INITIALIZE THE RESPIRATORY CYCLE
    /********************************************/
    activationController.refreshState();
    bool shouldRun = activationController.isRunning();

    if (shouldRun) {
        pController.initRespiratoryCycle();
    }

    /********************************************/
    // START THE RESPIRATORY CYCLE
    /********************************************/
    uint16_t centiSec = 0;

    while (centiSec < pController.centiSecPerCycle()) {
        uint32_t pressure = readPressureSensor(centiSec, pressureOffset);

        uint32_t currentDate = millis();

        uint32_t diff = (currentDate - lastpControllerComputeDate);

        if (diff >= PCONTROLLER_COMPUTE_PERIOD) {
            lastpControllerComputeDate = currentDate;

            if (shouldRun) {
                digitalWrite(PIN_LED_START, LED_START_ACTIVE);
                pController.updatePressure(pressure);
                int32_t currentMicro = micros();

                pController.updateDt(currentMicro - lastMicro);
                lastMicro = currentMicro;

                // Perform the pressure control
                pController.compute(centiSec);
            } else {
                digitalWrite(PIN_LED_START, LED_START_INACTIVE);
                blower.stop();

                // Stop alarms related to breathing cycle
                alarmController.notDetectedAlarm(RCM_SW_1);
                alarmController.notDetectedAlarm(RCM_SW_2);
                alarmController.notDetectedAlarm(RCM_SW_3);
                alarmController.notDetectedAlarm(RCM_SW_14);
                alarmController.notDetectedAlarm(RCM_SW_15);
                alarmController.notDetectedAlarm(RCM_SW_18);
                alarmController.notDetectedAlarm(RCM_SW_19);

#if HARDWARE_VERSION == 2
                if ((centiSec % 10u) == 0u) {
                    sendStoppedMessage();
                }
#endif
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
            }

            alarmController.runAlarmEffects(centiSec);

            // next tick
            centiSec++;
            IWatchdog.reload();
        }
    }

    if (shouldRun) {
        pController.endRespiratoryCycle();
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
        cyclesBeforeScreenReset = LCD_RESET_PERIOD * (int8_t)CONST_MIN_CYCLE;
    }

    if (shouldRun) {
        displayCurrentInformation(pController.peakPressure(), pController.plateauPressure(),
                                  pController.peep());
    } else {
        displayMachineStopped();
    }
}

#endif
