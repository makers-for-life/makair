/*
    Copyright (C) 2020 Makers For Life
*/
/******************************************************************************
 * @author Makers For Life
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
