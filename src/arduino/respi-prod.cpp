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

// Associated header
#include "respi-prod.h"

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

void setup()
{

    DBG_DO(Serial.begin(115200);)
    DBG_DO(Serial.println("demarrage");)

    pController.setup();
    startScreen();
    initKeyboard();
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
    for (uint16_t centiSec = 0; centiSec < pController.centiSecPerCycle(); centiSec++)
    {

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
        pController.updatePressure(map(analogRead(PIN_CAPTEUR_PRESSION), 194, 245, 0, 600) / 10);
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

        // Wait 1/100 of second
        // Warning: this introduces a time drift, that will be corrected by rtc if needed
        delay(10);
    }
    /********************************************/
    // END OF THE RESPIRATORY CYCLE
    /********************************************/
}
