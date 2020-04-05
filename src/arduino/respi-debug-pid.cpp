/*=============================================================================
 * @file respi-tests.h
 *
 * COVID Respirator
 *
 * @section copyright Copyright
 *
 * Makers For Life
 *
 * @section descr File description
 *
 * This file execute the MakAir test program
 */

#pragma once

#include "config.h"
#if MODE == MODE_DEBUG_PID

// INCLUDES ===================================================================

// External
#include <Arduino.h>

// Include
#include "air_transistor.h"
#include "debug.h"
#include "parameters.h"
#include "pression.h"

#define PCONTROLLER_COMPUTE_PERIOD 10000ul
static uint32_t lastpControllerComputeDateUs = 0ul;

uint32_t lastpControllerComputeDateUs2 = 0ul;

AirTransistor servoBlower;
AirTransistor servoPatient;
HardwareTimer* hardwareTimer1;
HardwareTimer* hardwareTimer3;

void setup()
{
    DBG_DO(Serial.begin(115200);)
    DBG_DO(Serial.println("demarrage");)

    // Timer for servoBlower
    hardwareTimer1 = new HardwareTimer(TIM1);
    hardwareTimer1->setOverflow(SERVO_VALVE_PERIOD, MICROSEC_FORMAT);

    // Timer for servoPatient and escBlower
    hardwareTimer3 = new HardwareTimer(TIM3);
    hardwareTimer3->setOverflow(SERVO_VALVE_PERIOD, MICROSEC_FORMAT);

    // Servo blower setup
    servoBlower = AirTransistor(VALVE_OUVERT, 145, hardwareTimer1, TIM_CHANNEL_SERVO_VALVE_BLOWER,
                                PIN_SERVO_BLOWER);
    servoBlower.setup();
    hardwareTimer1->resume();

    // Servo patient setup
    servoPatient = AirTransistor(VALVE_OUVERT, 145, hardwareTimer3, TIM_CHANNEL_SERVO_VALVE_PATIENT,
                                 PIN_SERVO_PATIENT);
    servoPatient.setup();

    // Manual escBlower setup
    // Output compare activation on pin PIN_ESC_BLOWER
    hardwareTimer3->setMode(TIM_CHANNEL_ESC_BLOWER, TIMER_OUTPUT_COMPARE_PWM1, PIN_ESC_BLOWER);
    // Set PPM width to 1ms
    hardwareTimer3->setCaptureCompare(TIM_CHANNEL_ESC_BLOWER, Angle2MicroSeconds(0),
                                      MICROSEC_COMPARE_FORMAT);
    hardwareTimer3->resume();

    servoBlower.ouvrirImmediatement(25);
    delay(5000);

    // escBlower start
    hardwareTimer3->setCaptureCompare(TIM_CHANNEL_ESC_BLOWER, Angle2MicroSeconds(160),
                                      MICROSEC_COMPARE_FORMAT);

    servoBlower.ouvrirImmediatement(145);
}

int32_t MAX = 145;
int32_t MIN = 25;
int32_t _erreur = 0;
int32_t _sommeErreur = 0;
int32_t _derivee = 0;
int32_t _cmd = 0;
int32_t _kp = 1 * 2 * 2 * 2 / 2;
int32_t _ki = 1 * 2 * 2 * 2;
int32_t _kd = 5 * 2;
int32_t BORNE_MAX = 1000;
int32_t BORNE_MIN = -1 * BORNE_MAX;

int32_t calculConsigneMoteur(int32_t consigne, int32_t reel, int32_t dt)
{

    _erreur = consigne - reel; // erreur entre la consigne et la realite
    _sommeErreur += _ki * _erreur * dt / 1000000.0;

    _sommeErreur = max(BORNE_MIN, min(BORNE_MAX, _sommeErreur));

    _derivee = 1000000.0 * _erreur / dt;

    _cmd = _kp * _erreur + _sommeErreur + _kd * _derivee / 1000; // calcul de la commande

    uint32_t consigneBlower = max(MIN, min(MAX, MAX - (_cmd + 1000) * (MAX - MIN) / 2000));
    return consigneBlower;
}

bool ouvrir = false;
static uint16_t consignePression = 400; // mmH20
int32_t consigneServo = 0;
void loop()
{
    uint16_t pressionMesure = readPressureSensor(0); // mmH20
    uint32_t currentDateInUs = micros();
    uint32_t diff = currentDateInUs - lastpControllerComputeDateUs;
    uint32_t diff2 = currentDateInUs - lastpControllerComputeDateUs2;

    if (diff >= PCONTROLLER_COMPUTE_PERIOD)
    {
        consigneServo = calculConsigneMoteur(consignePression, pressionMesure, diff);
        servoBlower.ouvrirImmediatement(consigneServo);
        lastpControllerComputeDateUs = currentDateInUs;
    }

    if (diff2 >= PCONTROLLER_COMPUTE_PERIOD * 10)
    {
        // ouvrir = !ouvrir;
        // if (ouvrir)
        // {
        //     servoBlower.ouvrirImmediatement(145);
        // }
        // else
        // {
        //     servoBlower.ouvrirImmediatement(25);
        // }
        DBG_PID_PLOTTER(consignePression, pressionMesure, consigneServo)
        lastpControllerComputeDateUs2 = currentDateInUs;
    }
}

#endif
