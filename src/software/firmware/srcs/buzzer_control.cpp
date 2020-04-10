/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file buzzer_control.cpp
 * @brief Abstraction to switch buzzer ON or OFF
 *****************************************************************************/

#include "../includes/buzzer_control.h"
#include "../includes/config.h"
#include "../includes/parameters.h"
#include "Arduino.h"

/// Buzzer frequency in Hz
#define BUZZER_FREQ 4000

#define PERIOD_BUZZER_US (1000000 / BUZZER_FREQ)

#if HARDWARE_VERSION == 2
HardwareTimer* Buzzer_Hw_Timer;
uint32_t Buzzer_Timer_Channel;
#endif

void BuzzerControl_Init(void) {
#if HARDWARE_VERSION == 1
    pinMode(PIN_BUZZER, OUTPUT);
#elif HARDWARE_VERSION == 2
    TIM_TypeDef* Buzzer_Timer_Number = reinterpret_cast<TIM_TypeDef*>(
        pinmap_peripheral(digitalPinToPinName(PIN_BUZZER), PinMap_PWM));
    uint32_t Buzzer_Timer_Channel =
        STM_PIN_CHANNEL(pinmap_function(digitalPinToPinName(PIN_BUZZER), PinMap_PWM));

    Buzzer_Hw_Timer = new HardwareTimer(Buzzer_Timer_Number);
#endif
}

void BuzzerControl_On(void) {
#if HARDWARE_VERSION == 1
    // Hardware 1: the buzzer has an internal oscillator. Just switch on the output.
    digitalWrite(PIN_BUZZER, HIGH);
#elif HARDWARE_VERSION == 2
    // Hardware 2: the buzzer has no internal oscillator. uC must generate a 4khz square on
    // PIN_BUZZER (timer 2, channel 1)
    Buzzer_Hw_Timer->setMode(Buzzer_Timer_Channel, TIMER_OUTPUT_COMPARE_PWM1, PIN_BUZZER);
    Buzzer_Hw_Timer->setOverflow(PERIOD_BUZZER_US, MICROSEC_FORMAT);
    Buzzer_Hw_Timer->setCaptureCompare(Buzzer_Timer_Channel, PERIOD_BUZZER_US / 2,
                                       MICROSEC_COMPARE_FORMAT);
    Buzzer_Hw_Timer->resume();
#endif
}

void BuzzerControl_Off(void) {
#if HARDWARE_VERSION == 1
    // Hardware 1: the buzzer has an internal oscillator. Just switch on the output.
    digitalWrite(PIN_BUZZER, LOW);
#elif HARDWARE_VERSION == 2
    Buzzer_Hw_Timer->pause();
#endif
}
