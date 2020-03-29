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

// INCLUDES ==================================================================

// External
#include <AnalogButtons.h>
#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Servo.h>

// Internal
#include "affichage.h"
#include "clavier.h"
#include "common.h"
#include "config.h"
#include "debug.h"
#include "parameters.h"
#include "pressure_controller.h"
#include "simulation.h"

// PROGRAM =====================================================================

double filteredVout = 0;
const double RATIO_PONT_DIVISEUR = 0.8192;
const double V_SUPPLY = 5.08;
const double KPA_MMH2O = 101.97162129779;

int readPressureSensor() 
{
    double rawVout = analogRead(PIN_CAPTEUR_PRESSION) * 3.3 / 1024.0;
    filteredVout = filteredVout + (rawVout - filteredVout) * 0.2;

    // Ratio a cause du pont diviseur
    double vOut = filteredVout / RATIO_PONT_DIVISEUR;

    // Pression en kPA
    double pressure  = (vOut / V_SUPPLY - 0.04) / 0.09;
    Serial.println(pressure * KPA_MMH2O);
    return pressure * KPA_MMH2O;
}

void setup()
{
    pinMode(PIN_CAPTEUR_PRESSION, INPUT);

    DBG_DO(Serial.begin(9600);)
    DBG_DO(Serial.println("demarrage");)

    pController.setup();
    startScreen();
    initKeyboard();
    Serial.begin(9600);
}

void loop()
{
    /********************************************/
    // INITIALIZE THE RESPIRATORY CYCLE
    /********************************************/

    displayEveryRespiratoryCycle(pController.peakPressure(), pController.plateauPressure(),
                                 pController.peep());

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

// Get the measured pressure for the feedback control
#ifdef SIMULATION
            if (centiSec < uint16_t(50))
            {
                pController.updatePressure(60);
            }
            else
            {
                pController.updatePressure(30);
            }
            if (centiSec > pController.centiSecPerInhalation())
            {
                pController.updatePressure(5);
            }
#else
            pController.updatePressure(readPressureSensor());
#endif

            // Perform the pressure control
            pController.compute(centiSec);

            // Check if some buttons have been pushed
            keyboardLoop();

            // Display relevant information during the cycle
            if (centiSec % LCD_UPDATE_PERIOD == 0)
            {
                displayDuringCycle(pController.cyclesPerMinuteCommand(),
                                   pController.maxPlateauPressureCommand(),
                                   pController.minPeepCommand(), pController.pressure());
            }

            // next tick
            centiSec++;
        }
    }
    /********************************************/
    // END OF THE RESPIRATORY CYCLE
    /********************************************/
}
