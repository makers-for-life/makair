/*=============================================================================
 * @file respi-prod.h
 *
 * COVID Respirator
 *
 * @section copyright Copyright
 *
 * Makers For Life
 *
 * @section descr File description
 *
 * This file execute the Makair program
 */

#pragma once

#include "config.h"
#if MODE == MODE_PROD

// INCLUDES ==================================================================

// External
#include <AnalogButtons.h>
#include <Arduino.h>
#include <LiquidCrystal.h>

// Internal
#include "affichage.h"
#include "air_transistor.h"
#include "clavier.h"
#include "common.h"
#include "debug.h"
#include "parameters.h"
#include "pression.h"
#include "pressure_controller.h"

// PROGRAM =====================================================================

AirTransistor servoBlower;
AirTransistor servoPatient;
HardwareTimer* hardwareTimer1;
HardwareTimer* hardwareTimer3;

void waitForInMs(uint16_t ms)
{
    uint16_t start = millis();
    while ((millis() - start) < ms)
        continue;
}

void setup()
{
    DBG_DO(Serial.begin(115200);)
    DBG_DO(Serial.println("demarrage");)
    startScreen();

    pinMode(PIN_CAPTEUR_PRESSION, INPUT);
    pinMode(PIN_ALARM, OUTPUT);

    // Timer for servoBlower
    hardwareTimer1 = new HardwareTimer(TIM1);
    hardwareTimer1->setOverflow(SERVO_VALVE_PERIOD, MICROSEC_FORMAT);

    // Timer for servoPatient and escBlower
    hardwareTimer3 = new HardwareTimer(TIM3);
    hardwareTimer3->setOverflow(SERVO_VALVE_PERIOD, MICROSEC_FORMAT);

    // Servo blower setup
    servoBlower = AirTransistor(VALVE_OUVERT, VALVE_FERME, hardwareTimer1,
                                TIM_CHANNEL_SERVO_VALVE_BLOWER, PIN_SERVO_BLOWER);
    servoBlower.setup();
    hardwareTimer1->resume();

    // Servo patient setup
    servoPatient = AirTransistor(VALVE_OUVERT, VALVE_FERME, hardwareTimer3,
                                 TIM_CHANNEL_SERVO_VALVE_PATIENT, PIN_SERVO_PATIENT);
    servoPatient.setup();

    // Manual escBlower setup
    // Output compare activation on pin PIN_ESC_BLOWER
    hardwareTimer3->setMode(TIM_CHANNEL_ESC_BLOWER, TIMER_OUTPUT_COMPARE_PWM1, PIN_ESC_BLOWER);
    // Set PPM width to 1ms
    hardwareTimer3->setCaptureCompare(TIM_CHANNEL_ESC_BLOWER, Angle2MicroSeconds(0),
                                      MICROSEC_COMPARE_FORMAT);
    hardwareTimer3->resume();

    pController = PressureController(INITIAL_CYCLE_NB, DEFAULT_MIN_PEEP_COMMAND,
                                     BORNE_SUP_PRESSION_PLATEAU, ANGLE_OUVERTURE_MAXI,
                                     BORNE_SUP_PRESSION_CRETE, servoBlower, servoPatient);
    pController.setup();

    initKeyboard();

    // escBlower needs 5s at speed 0 to be properly initalized
    digitalWrite(PIN_ALARM, HIGH);
    waitForInMs(1000);
    digitalWrite(PIN_ALARM, LOW);
    waitForInMs(4000);

    // escBlower start
    hardwareTimer3->setCaptureCompare(TIM_CHANNEL_ESC_BLOWER, Angle2MicroSeconds(130),
                                      MICROSEC_COMPARE_FORMAT);
    DBG_DO(Serial.println("Esc blower is running!");)
}

void loop()
{
    /********************************************/
    // INITIALIZE THE RESPIRATORY CYCLE
    /********************************************/

    pController.initRespiratoryCycle();

    /********************************************/
    // START THE RESPIRATORY CYCLE
    /********************************************/
    uint16_t centiSec = 0;

    while (centiSec < pController.centiSecPerCycle())
    {
        static uint32_t lastpControllerComputeDate = 0ul;
        uint32_t currentDate = millis();
        if (currentDate - lastpControllerComputeDate >= PCONTROLLER_COMPUTE_PERIOD)
        {
            lastpControllerComputeDate = currentDate;

            pController.updatePressure(readPressureSensor(centiSec));

            // Perform the pressure control
            pController.compute(centiSec);

            // Check if some buttons have been pushed
            keyboardLoop();

            // Display relevant information during the cycle
            if (centiSec % LCD_UPDATE_PERIOD == 0)
            {
                displaySubPhase(pController.subPhase());

                displayEveryRespiratoryCycle(pController.peakPressure(),
                                             pController.plateauPressure(), pController.peep(),
                                             pController.pressure());

                displayDuringCycle(
                    pController.maxPeakPressureCommand(), pController.maxPlateauPressureCommand(),
                    pController.minPeepCommand(), pController.cyclesPerMinuteCommand());
            }

            // next tick
            centiSec++;
        }
    }
    /********************************************/
    // END OF THE RESPIRATORY CYCLE
    /********************************************/
}

#endif
