#include "../includes/buzzer_control.h"
#include "../includes/config.h"
#include "../includes/parameters.h"
#include "Arduino.h"

// frequency = 4khz
#define Buzzer_Freq 4000  

#define Period_Buzzer_us ( 1000000 / Buzzer_Freq ) 


TIM_TypeDef* Buzzer_Timer_Number = reinterpret_cast<TIM_TypeDef*>(
        pinmap_peripheral(digitalPinToPinName(PIN_BUZZER), PinMap_PWM));
uint32_t Buzzer_Timer_Channel =
        STM_PIN_CHANNEL(pinmap_function(digitalPinToPinName(PIN_BUZZER), PinMap_PWM));

HardwareTimer* Buzzer_Hw_Timer = new HardwareTimer(Buzzer_Timer_Number);

void Buzzer_On(void) {


#if HARDWARE_VERSION == 2
  // Hardware 2: the buzzer has no internal oscillator. uC must generate a 4khz square on PIN_BUZZER (timer 2, channel 1)
  Buzzer_Hw_Timer->setMode(Buzzer_Timer_Channel, TIMER_OUTPUT_COMPARE_PWM1, PIN_BUZZER);
  Buzzer_Hw_Timer->setOverflow(Period_Buzzer_us, MICROSEC_FORMAT);
  Buzzer_Hw_Timer->setCaptureCompare(Buzzer_Timer_Channel, Period_Buzzer_us / 2 , MICROSEC_COMPARE_FORMAT);
  Buzzer_Hw_Timer->resume();



#else
  // Hardware 1: the buzzer has an internal oscillator. Just switch on the output.
  digitalWrite(PIN_BUZZER, HIGH);
#endif

}

void Buzzer_Off(void){

#if HARDWARE_VERSION == 2
  Buzzer_Hw_Timer->pause();
#else
  // Hardware 1: the buzzer has an internal oscillator. Just switch on the output.
  digitalWrite(PIN_BUZZER, LOW);
#endif

}
