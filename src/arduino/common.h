/*=============================================================================
 * @file common.h
 *
 * COVID Respirator
 *
 * @section copyright Copyright
 *
 * Makers For Life
 *
 * @section descr File description
 *
 * This file defines common stuff
 */

#pragma once

/*! This enum defines the 4 phases of the respiratory cycle
 *  -   Push inhalation:
 *      +   Send air until the peak pressure is reached
 *      +   The blower valve is open so the air stream goes to the patient's lungs
 *  -   Hold inhalation:
 *      +   Once the peak pressure is reached, the pressure is hold to the plateau pressure
 *          The pressure has been decreasing for more than 0.1 s.
 *      +   The 2 valves are hold closed
 *  -   Exhalation:
 *      +   The patient exhales
 *  -   Hold exhalation:
 *      +   Valves stay closed as the pressure is below the PEEP
 */
enum CyclePhases
{
  INHALATION,
  PLATEAU,
  EXHALATION,
  HOLD_EXHALATION
};
