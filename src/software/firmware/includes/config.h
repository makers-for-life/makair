/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file config.h
 * @brief Main configuration
 *****************************************************************************/

#pragma once

// Available modes
#define MODE_PROD 1           ///< Ventilator mode
#define MODE_QUALIFICATION 2  ///< Test electrical wiring mode

/// Defines the current mode
#define MODE MODE_PROD

/**
 * Activates debug traces
 *
 * When DEBUG = 1, additional code is added to send debug traces using serial
 */
#define DEBUG 1

/**
 * Activates pressure sensor simulation
 *
 * When SIMULATION = 1, the current pressure value will not be read from the sensor but computed by
 * a basic and deterministic model
 */
#define SIMULATION 0

// Available pneumatic versions
#define PHW_PIGGY 0
#define PHW_CHU 1

/// Defines which preset to use for controlling pressure
#define PNEUMATIC_HARDWARE_VERSION PHW_CHU

/**
 * Defines which hardware preset to use
 *
 * This changes which hardware is used, to which pins it is connected, as well as various parameters
 */
#define HARDWARE_VERSION 2
