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
#include "LL/stm32yyxx_ll_utils.h"
#endif

/// Internals

// FUNCTIONS ==================================================================

#define FIRST_BYTE (uint8_t)0xFF

void initTelemetry(void) {
#if HARDWARE_VERSION == 2
    Serial6.begin(115200);
#endif
}

void sendDataSnapshot() {
#if HARDWARE_VERSION == 2
    Serial6.write("D:");
    Serial6.write((uint8_t)1);

    byte deviceId1[4];  // 32 bits
    deviceId1[0] = (LL_GetUID_Word0() >> 24) & FIRST_BYTE;
    deviceId1[1] = (LL_GetUID_Word0() >> 16) & FIRST_BYTE;
    deviceId1[2] = (LL_GetUID_Word0() >> 8) & FIRST_BYTE;
    deviceId1[3] = LL_GetUID_Word0() & FIRST_BYTE;
    Serial6.write(deviceId1, 4);

    byte deviceId2[4];  // 32 bits
    deviceId2[0] = (LL_GetUID_Word1() >> 24) & FIRST_BYTE;
    deviceId2[1] = (LL_GetUID_Word1() >> 16) & FIRST_BYTE;
    deviceId2[2] = (LL_GetUID_Word1() >> 8) & FIRST_BYTE;
    deviceId2[3] = LL_GetUID_Word1() & FIRST_BYTE;
    Serial6.write(deviceId2, 4);

    byte deviceId3[4];  // 32 bits
    deviceId3[0] = (LL_GetUID_Word2() >> 24) & FIRST_BYTE;
    deviceId3[1] = (LL_GetUID_Word2() >> 16) & FIRST_BYTE;
    deviceId3[2] = (LL_GetUID_Word2() >> 8) & FIRST_BYTE;
    deviceId3[3] = LL_GetUID_Word2() & FIRST_BYTE;
    Serial6.write(deviceId3, 4);

    Serial6.print("\t");

    byte systick[8];  // 64 bits
    uint64_t systickValue = millis();
    systick[0] = (systickValue >> 56) & FIRST_BYTE;
    systick[1] = (systickValue >> 48) & FIRST_BYTE;
    systick[2] = (systickValue >> 40) & FIRST_BYTE;
    systick[3] = (systickValue >> 32) & FIRST_BYTE;
    systick[4] = (systickValue >> 24) & FIRST_BYTE;
    systick[5] = (systickValue >> 16) & FIRST_BYTE;
    systick[6] = (systickValue >> 8) & FIRST_BYTE;
    systick[7] = systickValue & FIRST_BYTE;
    Serial6.write(systick, 8);

    Serial6.print("\t");

    byte centile[2];  // 16 bits
    uint16_t centileValue = 42;
    centile[0] = (centileValue >> 8) & FIRST_BYTE;
    centile[1] = centileValue & FIRST_BYTE;
    Serial6.write(centile, 2);

    Serial6.print("\t");

    byte pressure[2];  // 16 bits
    uint16_t pressureValue = 80;
    pressure[0] = (pressureValue >> 8) & FIRST_BYTE;
    pressure[1] = pressureValue & FIRST_BYTE;
    Serial6.write(pressure, 2);
#endif
}

#endif
