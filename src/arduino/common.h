/*=============================================================================
 * @file common.h
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
 * This file defines common stuff
 */

#pragma once

/*! This enum defines the 4 phases of the respiratory cycle
 *  -   Push inhalation:
 *      +   Send air until the peak pressure is reached
 *      +   The blower valve is open so the air stream goes to the patient's lungs
 *  -   Hold inhalation:
 *      +   Once the peak pressure is reached, the pressure is held to the plateau pressure
 *          The pressure has been decreasing for more than 0.1 s.
 *      +   The 2 valves are held closed
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

//! Constant to convert a pressure in [kPa] into a pressure in [mmH2O]
const double KPA_MMH2O = 101.97162129779;
