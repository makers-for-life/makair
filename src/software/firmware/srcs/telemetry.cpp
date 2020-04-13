/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file telemetry.h
 * @brief Implementation of the telemetry protocol
 *****************************************************************************/

#pragma once

#include "../includes/config.h"
#if MODE == MODE_PROD

// INCLUDES ===================================================================

// Associated header
#include "../includes/telemetry.h"

// Externals
#if HARDWARE_VERSION == 2
#include "Arduino.h"
#include "stm32yyxx_ll_utils.h"
#endif

/// Internals

// FUNCTIONS ==================================================================

void initTelemetry(void) {
#if HARDWARE_VERSION == 2
    Serial6.begin(115200);
#endif
}

void sendDataSnapshot() {
#if HARDWARE_VERSION == 2
    Serial6.write("D:");
    //Serial6.printf("%x%x%x", LL_GetUID_Word0(), LL_GetUID_Word1(), LL_GetUID_Word2());
    Serial6.write(LL_GetUID_Word0());
    Serial6.write(LL_GetUID_Word1());
    Serial6.write(LL_GetUID_Word2());
    Serial6.write("\t");
    Serial6.write(millis());
    Serial6.write("\n");
#endif
}

#endif
