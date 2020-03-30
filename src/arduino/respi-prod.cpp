/*=============================================================================
 * @file respi-prod.cpp
 *
 * COVID Respirator
 *
 * @section copyright Copyright
 *
 * Makers For Life
 *
 * This is free and unencumbered software released into the public domain.
 * Anyone is free to copy, modify, publish, use, compile, sell, or distribute
 * this software, either in source code form or as a compiled binary,
 * for any purpose, commercial or non-commercial, and by any means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the software
 * to the public domain. We make this dedication for the benefit of the public
 * at large and to the detriment of our heirs and successors. We intend this
 * dedication to be an overt act of relinquishment in perpetuity of all present
 * and future rights to this software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to [http://unlicense.org]
 *
 * @section descr File description
 *
 * This file execute the MakAir program
 */

// INCLUDES ==================================================================

// External
#include <AnalogButtons.h>
#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Servo.h>

// Internal
#include "common.h"
#include "config.h"
#include "debug.h"
#include "display.h"
#include "keyboard.h"
#include "parameters.h"
#include "pressure_controller.h"
#include "pressure_sensor.h"

// PROGRAM =====================================================================

void setup()
{
    pinMode(PIN_CAPTEUR_PRESSION, INPUT);

    DBG_DO(Serial.begin(9600);)
    DBG_DO(Serial.println("Start");)

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
            pController.updatePressure(pSensor.read());
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
