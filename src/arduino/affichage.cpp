/*=============================================================================
 * @file affichage.cpp
 *
 * COVID Respirator
 *
 * @section copyright Copyright
 *
 * Makers For Life
 *
 * @section descr File description
 *
 * Fichier implémentant les fonctionnalitées liés à l'affichage
 */

// INCLUDES ===================================================================

// Associated header
#include "affichage.h"

// External
#include <LiquidCrystal.h>

// Internal
#include "parameters.h"

// INITIALISATION =============================================================

static LiquidCrystal screen(PIN_RS, PIN_EN, PIN_D4, PIN_D5, PIN_D6, PIN_D7);

// FUNCTIONS ==================================================================

void startScreen()
{
  switch (screenSize)
  {
  case ScreenSize::CHARS_16:
  {
    screen.begin(16, 2);
    break;
  }
  case ScreenSize::CHARS_20:
  {
    screen.begin(20, 2);
    break;
  }
  default:
  {
     screen.begin(16, 2);
  }
  }
}

void displayEveryRespiratoryCycle(int peakPressure, int plateauPressure,
                                  int peep) {
  screen.setCursor(0, 0);

  switch (screenSize)
  {
  case ScreenSize::CHARS_16:
  {
      screen.print("pc");
      screen.print(peakPressure);
      screen.print("/pp");
      screen.print(plateauPressure);
      screen.print("/pep");
      screen.print(peep);
      screen.print("  ");
      break;
  }
  case ScreenSize::CHARS_20:
  {
    screen.print("pc=");
    screen.print(peakPressure);
    screen.print("/pp=");
    screen.print(plateauPressure);
    screen.print("/pep=");
    screen.print(peep);
    screen.print("  ");
    break;
  }
  default:
  {
    screen.print("pc");
    screen.print(peakPressure);
    screen.print("/pp");
    screen.print(plateauPressure);
    screen.print("/pep");
    screen.print(peep);
    screen.print("  ");
  }
  }
}

void displayDuringCycle(int cyclesPerMinute,
                        int maxPlateauPressure,
                        int peep,
                        int currentPressure)
{
  screen.setCursor(0, 1);

  switch (screenSize) {
  case ScreenSize::CHARS_16: {
    screen.print("c");
    screen.print(cyclesPerMinute);
    screen.print("/pl");
    screen.print(maxPlateauPressure);
    screen.print("/pep");
    screen.print(peep);
    break;
  }
  case ScreenSize::CHARS_20: {
    screen.print("c=");
    screen.print(cyclesPerMinute);
    screen.print("/pl=");
    screen.print(maxPlateauPressure);
    screen.print("/pep=");
    screen.print(peep);
    screen.print("|");
    screen.print(currentPressure);
    break;
  }
  default:
  {
    screen.print("c");
    screen.print(cyclesPerMinute);
    screen.print("/pl");
    screen.print(maxPlateauPressure);
    screen.print("/pep");
    screen.print(peep);
  }
  }
}
