/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file mass_flow_meter.cpp
 * @brief Mass Flow meter management
 *****************************************************************************/

/**
 * SFM3300-D sensirion mass flow meter is connected on I2C bus.
 * To perform the integral of the mass flow, i2c polling must be done in a high priority timer
 */

// Associated header
#include "../includes/mass_flow_meter.h"

// External
#include "../includes/config.h"
#include <Arduino.h>
#include <IWatchdog.h>
#include <OneButton.h>
#include <Wire.h>

// Internal
#include "../includes/parameters.h"

// Linked to Hardware v2
#ifdef MASS_FLOW_METER

// 2 khz => prescaler = 50000 => OK for a 16 bit timer. it cannnot be slower
// 10 khz => nice
#define MASS_FLOW_TIMER_FREQ 10000

// the timer period in microsecond, 100us precision (10 khz)
#define MASS_FLOW_PERIOD_US 1000
HardwareTimer* massFlowTimer;

volatile int32_t mfmAirVolumeSum = 0;
volatile int32_t mfmPreviousValue = 0;
volatile int32_t mfmSensorDetected = 0;

bool mfmFaultCondition = false;
int32_t mfmResetStateMachine = 5;
// volatile int32_t mfmTimerCounter = 0;

union {
    unsigned short i;
    unsigned char c[2];
} mfmLastData;

void MFM_Timer_Callback(HardwareTimer*) {

    int32_t rawValue;

    if (!mfmFaultCondition) {
        Wire.beginTransmission(0x40);

        digitalWrite(PIN_LED_START, true);
        Wire.requestFrom(0x40, 2);
        mfmLastData.c[1] = Wire.read();
        mfmLastData.c[0] = Wire.read();

        // mfmTimerCounter++;

        if (Wire.endTransmission() != 0)  // If transmission failed
        {
            mfmFaultCondition = true;
            mfmResetStateMachine = 5;
        }
        digitalWrite(PIN_LED_START, false);

        // newSum = ( (mfmLastData.i - 0x8000) / 120 * 60);

        // mfmAirVolumeSum += (((mfmLastData.i - 0x8000) / 7200) + mfmPreviousValue) / 2;  //
        // l.min-1

        // mfmPreviousValue = ((mfmLastData.i - 0x8000) / 7200);
        rawValue = mfmLastData.i;  //- 0x8000;

        mfmAirVolumeSum += rawValue - 0x8000;

        // Correction factor is 120. Divide by 60 to convert ml.min-1 to ml.ms-1, hence the 7200 =
        // 120 * 60
        // TODO : Adapt calculation formula based on the timer period

        // mfmAirVolumeSum += ( (mfmLastData.i - 0x8000) / 120 * 60 * MASS_FLOW_PERIOD_US * 100000);

    } else {

        if (mfmResetStateMachine == 5) {
            // reset attempt
            Wire.flush();
            Wire.end();
            // force reset of pin in a high state.
            // pinMode(PIN_I2C_SDA, OUTPUT);
            // pinMode(PIN_I2C_SCL, OUTPUT);
            // digitalWrite(PIN_I2C_SCL, HIGH);
            // digitalWrite(PIN_I2C_SDA, LOW);
            // delayMicroseconds(20);
            // digitalWrite(PIN_I2C_SDA, HIGH);
            // Wire.setSDA(PIN_I2C_SDA);
            // Wire.setSCL(PIN_I2C_SCL);
        }
        mfmResetStateMachine--;

        if (mfmResetStateMachine == 0) {
            Wire.begin(true);
            Wire.beginTransmission(0x40);
            Wire.write(0x10);
            Wire.write(0x00);
            mfmFaultCondition = (Wire.endTransmission() != 0);
            if (mfmFaultCondition) {
                mfmResetStateMachine = 5;
            }
        }
    }
}

/**
 *  Returns true if there is a Mass Flow Meter connected
 *  If not detected, you will always read volume = 0 mL
 */
boolean MFM_init(void) {

    mfmAirVolumeSum = 0;

    // set the timer
    massFlowTimer = new HardwareTimer(MASS_FLOW_TIMER);

    // prescaler. stm32f411 clock is 100mhz
    massFlowTimer->setPrescaleFactor((massFlowTimer->getTimerClkFreq() / MASS_FLOW_TIMER_FREQ) - 1);

    // set the period
    massFlowTimer->setOverflow(MASS_FLOW_TIMER_FREQ / MASS_FLOW_PERIOD_US);
    massFlowTimer->setMode(MASS_FLOW_CHANNEL, TIMER_OUTPUT_COMPARE, NC);
    massFlowTimer->attachInterrupt(MFM_Timer_Callback);

    // interrupt priority is documented here:
    // https://stm32f4-discovery.net/2014/05/stm32f4-stm32f429-nvic-or-nested-vector-interrupt-controller/
    massFlowTimer->setInterruptPriority(2, 0);

    // detect if the sensor is connected
    Wire.setSDA(PIN_I2C_SDA);
    Wire.setSCL(PIN_I2C_SCL);

    Wire.begin();  // join i2c bus (address optional for master)
    // Wire.endTransmission();
    Wire.beginTransmission(0x40);

    Wire.write(0x10);
    Wire.write(0x00);

    // mfmTimerCounter = 0;

    mfmFaultCondition = (Wire.endTransmission() != 0);

    massFlowTimer->resume();

    return mfmFaultCondition;
}

/**
 * Reset the volume counter
 */
void MFM_reset(void) {
    mfmAirVolumeSum = 0;
    mfmPreviousValue = 0;
    // mfmTimerCounter = 0;
}

/**
 * return the number of milliliters since last reset
 * Can also perform the volume reset in the same atomic operation
 */
int32_t MFM_read_liters(boolean reset_after_read) {

    int32_t result;
    // int32_t timerCounter;

    // timerCounter = mfmTimerCounter;
    // this should be an atomic operation (32 bits aligned data)
    result = mfmFaultCondition ? 999999 : mfmAirVolumeSum;

    if (reset_after_read) {
        MFM_reset();
    }

    return result;
}

void setup(void) {
    Serial.begin(115200);
    Serial.println("coucou, tu veux voir ma ... ?");
    Serial.println("init mass flow meter");
    MFM_init();

    pinMode(PIN_SERIAL_TX, OUTPUT);
    pinMode(PIN_LED_START, OUTPUT);

    Serial.println("init done");
    Serial.println("A poiiiiiiiiiiiiiiiiiiiiiiiiiiiiiils !");
}

void loop(void) {

    delay(1000);
    Serial.print("volume = ");
    Serial.println(MFM_read_liters(true));
}

#endif