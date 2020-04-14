/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file sysclock.h
 * @brief Sysclock configuration
 *****************************************************************************/

#pragma once

/**
 * @brief  System Clock Configuration
 * @note   SYSCLK = 100000000 Hz for STM32F411xE, SYSCLK = 84000000 Hz for STM32F401xE
 */
extern "C" void SystemClock_Config(void);
