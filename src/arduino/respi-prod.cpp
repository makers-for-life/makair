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
#include "common.h"
#include "config.h"
#include "debug.h"
#include "display.h"
#include "keyboard.h"
#include "parameters.h"
#include "pressure_controller.h"
#include "alarm.h"
#include <IWatchdog.h>

// PROGRAM =====================================================================

/* Watchdog timeout in microseconds */
#define WATCHDOG_TIMEOUT 1000000

void setup()
{
    /* Catch potential Watchdog reset */
    if (IWatchdog.isReset(true)) {
      /* Code in case of Watchdog detected */
      /* TODO */
      Alarm_Init();
      Alarm_Red_Start();
      while(1){};
    }

    // Init the watchdog timer. It must be reloaded frequently otherwise MCU resests
    IWatchdog.begin(WATCHDOG_TIMEOUT);

    DBG_DO(Serial.begin(9600);)
    DBG_DO(Serial.println("demarrage");)

    pController.setup();
    startScreen();
    initKeyboard();
    Alarm_Init();
    Alarm_Boot_Start();
    delay(150);
    Alarm_Stop();

    /* Test purpose*/
    delay(2000);
    Alarm_Yellow_Start();

    IWatchdog.reload();
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
            pController.updatePressure(map(analogRead(PIN_CAPTEUR_PRESSION), 194, 245, 0, 600)
                                       / 10);
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
      IWatchdog.reload();
    }
    /********************************************/
    // END OF THE RESPIRATORY CYCLE
    /********************************************/
}
