/*
    Copyright (C) 2020 Makers For Life
*/
/******************************************************************************
 * @author Makers For Life
 * @file common.h
 * @brief Various core entities
 *****************************************************************************/

#pragma once

/// Defines the 4 phases of the respiratory cycle
enum CycleSubPhases {
    /**
     * Push inhalation:
     * - Send air until the peak pressure is reached
     * - The blower valve is open so the air stream goes to the patient's lungs
     */
    INSPIRATION,
    /**
     * Hold inhalation:
     * - Once the peak pressure is reached, the pressure is hold to the plateau pressure
     *   The pressure has been decreasing for more than 0.1 s.
     * - The 2 valves are hold closed
     */
    HOLD_INSPIRATION,
    /**
     * Exhalation:
     * - The patient exhales
     */
    EXHALE,
    /**
     * Hold exhalation:
     * - Valves stay closed as the pressure is below the PEEP
     */
    HOLD_EXHALE
};

/// Defines the 2 main phases of the respiratory cycle
enum CyclePhases {
    /// Inspiration and inspiration holding
    INHALATION,
    /// Exhalation and pause
    EXHALATION
};
