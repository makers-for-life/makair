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

// 2 khz => prescaler = 50000 => still OK for a 16 bit timer. it cannnot be slower
// 10 khz => nice
#define MASS_FLOW_TIMER_FREQ 10000

// the timer period in microsecond, 100us precision (because 10 khz prescale)
#define MASS_FLOW_PERIOD_US 1000
HardwareTimer* massFlowTimer;

volatile int32_t mfmAirVolumeSum = 0;
volatile int32_t mfmPreviousValue = 0;
volatile int32_t mfmSensorDetected = 0;

bool mfmFaultCondition = false;

// Time to reset the sensor after I2C restart, in periods. => 5ms.
#define MFM_WAIT_RESET_PERIODS 5
int32_t mfmResetStateMachine = MFM_WAIT_RESET_PERIODS;


union {
    unsigned short i;
    unsigned char c[2];
} mfmLastData;

void MFM_Timer_Callback(HardwareTimer*) {

    int32_t rawValue;
    int32_t newSum;

    if (!mfmFaultCondition) {
        #if MODE == MODE_MFM_TESTS
        digitalWrite(PIN_LED_START, true);
        #endif

        Wire.beginTransmission(MFM_SENSOR_I2C_ADDRESS);
        Wire.requestFrom(MFM_SENSOR_I2C_ADDRESS, 2);
        mfmLastData.c[1] = Wire.read();
        mfmLastData.c[0] = Wire.read();
        if (Wire.endTransmission() != 0) {  // If transmission failed
            mfmFaultCondition = true;
            mfmResetStateMachine = 5;
        }
        mfmAirVolumeSum += (int32_t)mfmLastData.i - 0x8000;

        #if MODE == MODE_MFM_TESTS
        digitalWrite(PIN_LED_START, false);
        #endif
    } else {

        if (mfmResetStateMachine == MFM_WAIT_RESET_PERIODS) {
            // reset attempt
            Wire.flush();
            Wire.end();
        }
        mfmResetStateMachine--;

        if (mfmResetStateMachine == 0) {
            // MFM_WAIT_RESET_PERIODS cycles later, try again
            Wire.begin(true);
            Wire.beginTransmission(MFM_SENSOR_I2C_ADDRESS);
            Wire.write(0x10);
            Wire.write(0x00);
            mfmFaultCondition = (Wire.endTransmission() != 0);
            if (mfmFaultCondition) {
                mfmResetStateMachine = MFM_WAIT_RESET_PERIODS;
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
    Wire.beginTransmission(MFM_SENSOR_I2C_ADDRESS);

    Wire.write(0x10);
    Wire.write(0x00);

    // mfmTimerCounter = 0;

    mfmFaultCondition = (Wire.endTransmission() != 0);

    massFlowTimer->resume();

    return mfmFaultCondition;
}

/*
 * Reset the volume counter
 */
void MFM_reset(void) {
    mfmAirVolumeSum = 0;
    mfmPreviousValue = 0;
}

/**
 * return the number of milliliters since last reset
 * Can also perform the volume reset in the same atomic operation
 */
int32_t MFM_read_liters(boolean reset_after_read) {

    int32_t result;

    // this should be an atomic operation (32 bits aligned data)
    result = mfmFaultCondition ? 999999 : mfmAirVolumeSum / (60 * 120);

    // Correction factor is 120. Divide by 60 to convert ml.min-1 to ml.ms-1, hence the 7200 =
    // 120 * 60
    // TODO : Adapt calculation formula based on the timer period

    if (reset_after_read) {
        MFM_reset();
    }

    return result;
}


#if MODE == MODE_MFM_TESTS

void setup(void) {
    
    Serial.begin(115200);
    Serial.println("init mass flow meter");
    MFM_init();

    pinMode(PIN_SERIAL_TX, OUTPUT);
    pinMode(PIN_LED_START, OUTPUT);

    Serial.println("init done");
}

void loop(void) {

    delay(10000);
    Serial.print("volume = ");
    Serial.println(MFM_read_liters(true));
}
#endif

#endif