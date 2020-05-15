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
#include <math.h>

// Internal
#include "../includes/buzzer_control.h"
#include "../includes/parameters.h"
#include "../includes/screen.h"

// Linked to Hardware v2
#ifdef MASS_FLOW_METER

// 2 khz => prescaler = 50000 => still OK for a 16 bit timer. it cannnot be slower
// 10 khz => nice
#define MASS_FLOW_TIMER_FREQ 10000

// the timer period in 100us multiple (because 10 khz prescale)

#if MASS_FLOW_METER_SENSOR == MFM_SFM_3300D
#define MASS_FLOW_PERIOD 10
#endif

#if MASS_FLOW_METER_SENSOR == MFM_SDP703_02
#define MASS_FLOW_PERIOD 100
#endif

#if MASS_FLOW_METER_SENSOR == MFM_OMRON_D6F
#define MASS_FLOW_PERIOD 100
#endif

#if MASS_FLOW_METER_SENSOR == MFM_HONEYWELL_HAF
#define MASS_FLOW_PERIOD 100
#endif

#endif

HardwareTimer* massFlowTimer;

int32_t mfmCalibrationOffset = 0;

volatile int32_t mfmAirVolumeSum = 0;
volatile int32_t mfmSensorDetected = 0;

volatile int32_t mfmSampleCount = 0;

bool mfmFaultCondition = false;

volatile int failureCount = 0;

int32_t mfmLastValue = 0;
// Time to reset the sensor after I2C restart, in periods. => 5ms.
#define MFM_WAIT_RESET_PERIODS 5
int32_t mfmResetStateMachine = MFM_WAIT_RESET_PERIODS;

union {
    unsigned short i;
    signed short si;
    unsigned char c[2];
} mfmLastData;

void MFM_Timer_Callback(HardwareTimer*) {

    int32_t rawValue;
    int32_t newSum;

    if (!mfmFaultCondition) {

#if MODE == MODE_MFM_TESTS
        digitalWrite(PIN_LED_START, true);
#endif

#if MASS_FLOW_METER_SENSOR == MFM_SFM_3300D
        Wire.beginTransmission(MFM_SENSOR_I2C_ADDRESS);
        Wire.requestFrom(MFM_SENSOR_I2C_ADDRESS, 2);
        mfmLastData.c[1] = Wire.read();
        mfmLastData.c[0] = Wire.read();
        if (Wire.endTransmission() != 0) {  // If transmission failed
            mfmFaultCondition = true;
            mfmResetStateMachine = MFM_WAIT_RESET_PERIODS;
        }

        mfmLastValue = (int32_t)mfmLastData.i - 0x8000;

        if (mfmLastValue > 28) {
            mfmAirVolumeSum += mfmLastValue;
        }
#endif

#if MASS_FLOW_METER_SENSOR == MFM_SDP703_02
        Wire.requestFrom(MFM_SENSOR_I2C_ADDRESS, 2);
        mfmLastData.c[1] = Wire.read();
        mfmLastData.c[0] = Wire.read();

        if (Wire.endTransmission() != 0) {  // If transmission failed
            // mfmFaultCondition = true;
            // mfmResetStateMachine = MFM_WAIT_RESET_PERIODS;
        }
        mfmLastValue = abs(mfmLastData.si);
        if (mfmLastValue > 40) {
            mfmAirVolumeSum += sqrt(mfmLastValue);
        }

        mfmSampleCount++;
#endif

#if MASS_FLOW_METER_SENSOR == MFM_OMRON_D6F
        mfmLastValue = analogRead(MFM_ANALOG_INPUT);
        if (mfmLastValue > mfmCalibrationOffset + 10) {
            mfmAirVolumeSum += analogRead(MFM_ANALOG_INPUT);
        }
        Serial.println(mfmLastValue);
#endif

#if MASS_FLOW_METER_SENSOR == MFM_HONEYWELL_HAF
        Wire.beginTransmission(MFM_SENSOR_I2C_ADDRESS);
        
        Wire.requestFrom(MFM_SENSOR_I2C_ADDRESS, 2);

        mfmLastData.c[0] = Wire.read();
        mfmLastData.c[1] = Wire.read();

        if (Wire.endTransmission() != 0) { 
            //The sensor tends to NACK the address quite often. So let's say there is actually something wrong when the sensor does that five times in a row, don't care otherwise.
            failureCount++;

            if(failureCount == 5)
            {
                mfmFaultCondition = true;
                mfmResetStateMachine = MFM_WAIT_RESET_PERIODS;
            }
        }

        else
        {
            failureCount = 0;
        }

        mfmLastValue = (uint32_t)mfmLastData.c[1] & 0xFF;
        mfmLastValue |= ( ( (uint32_t)mfmLastData.c[0] ) << 8 ) & 0xFF00;

        mfmLastValue = MFM_HONEYWELL_HAF_RANGE * ( ( (uint32_t)mfmLastValue/16384.0) - 0.1) / 0.8; //Output value in SLPM
        
        //The sensor (100SLM version anyway) tends to output spurrious values located at around 500 SLM, which are obviously not correct. Let's filter them out based on the range of the sensor + 10%.
        if (mfmLastValue < MFM_HONEYWELL_HAF_RANGE * 1.1) {
            mfmAirVolumeSum += mfmLastValue;
        }

#endif

#if MODE == MODE_MFM_TESTS
        digitalWrite(PIN_LED_START, false);
#endif
    } else {

        if (mfmResetStateMachine == MFM_WAIT_RESET_PERIODS) {
            // reset attempt
// I2C sensors
    
#if MASS_FLOW_METER_SENSOR == MFM_SFM_3300D || MASS_FLOW_METER_SENSOR == MFM_SDP703_02 || MASS_FLOW_METER_SENSOR == MFM_HONEYWELL_HAF
            Wire.flush();
            Wire.end();
#endif

#if MASS_FLOW_METER_SENSOR == MFM_SDP703_02
            Wire.begin();
            Wire.beginTransmission(MFM_SENSOR_I2C_ADDRESS);
            Wire.write(0xFE);
            Wire.endTransmission();
#endif

#if MASS_FLOW_METER_SENSOR == MFM_HONYWELL_HAF
            /*Wire.begin();
            Wire.beginTransmission(MFM_SENSOR_I2C_ADDRESS);
            Wire.write(0x02);
            Wire.endTransmission();*/
#endif
        }
        mfmResetStateMachine--;

        if (mfmResetStateMachine == 0) {
// MFM_WAIT_RESET_PERIODS cycles later, try again to init the sensor
#if MASS_FLOW_METER_SENSOR == MFM_SFM_3300D
            Wire.begin(true);
            Wire.beginTransmission(MFM_SENSOR_I2C_ADDRESS);
            Wire.write(0x10);
            Wire.write(0x00);
            mfmFaultCondition = (Wire.endTransmission() != 0);
#endif

#if MASS_FLOW_METER_SENSOR == MFM_SDP703_02

            Wire.beginTransmission(MFM_SENSOR_I2C_ADDRESS);
            Wire.write(0xE2);
            Wire.write(0x02);
            Wire.write(0x08);
            Wire.endTransmission();

            Wire.beginTransmission(MFM_SENSOR_I2C_ADDRESS);
            Wire.write(0xE4);
            Wire.write(0x76);
            Wire.write(0xA2);
            Wire.endTransmission();

            Wire.beginTransmission(MFM_SENSOR_I2C_ADDRESS);
            Wire.write(0xF1);
            mfmFaultCondition = (Wire.endTransmission() != 0);
#endif

#if MASS_FLOW_METER_SENSOR == MFM_HONEYWELL_HAF

            Wire.begin();
            Wire.beginTransmission(MFM_SENSOR_I2C_ADDRESS);
            Wire.write(0x02); //Force reset
            Wire.endTransmission();

            Wire.beginTransmission(MFM_SENSOR_I2C_ADDRESS);

            Wire.requestFrom(MFM_SENSOR_I2C_ADDRESS, 2);
            mfmLastData.c[1] = Wire.read();
            mfmLastData.c[0] = Wire.read();

            mfmFaultCondition = (Wire.endTransmission() != 0);
#endif

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
    massFlowTimer->setOverflow(MASS_FLOW_PERIOD);
    massFlowTimer->setMode(MASS_FLOW_CHANNEL, TIMER_OUTPUT_COMPARE, NC);
    massFlowTimer->attachInterrupt(MFM_Timer_Callback);

    // interrupt priority is documented here:
    // https://stm32f4-discovery.net/2014/05/stm32f4-stm32f429-nvic-or-nested-vector-interrupt-controller/
    massFlowTimer->setInterruptPriority(2, 0);

    /* mass flow needs to be corrected with pressure ?
     * flow (kg/s) = rho (kg/m3) x section (m²) x speed (m/s)
     * Rho = (Pressure * M) / (R * Temperature)
     * sea level pressure : P0 = 101 325 Pa = 1 013,25 mbar = 1 013,25 hPa = 1032 cmH2O
     * respirator reach 50cmH2O... it means 5% error.
     */
#if MASS_FLOW_METER_SENSOR == MFM_OMRON_D6F
    pinMode(MFM_ANALOG_INPUT, INPUT);
#endif

// I2C sensors
#if MASS_FLOW_METER_SENSOR == MFM_SFM_3300D || MASS_FLOW_METER_SENSOR == MFM_SDP703_02 || MASS_FLOW_METER_SENSOR == MFM_HONEYWELL_HAF
    // detect if the sensor is connected
    Wire.setSDA(PIN_I2C_SDA);
    Wire.setSCL(PIN_I2C_SCL);
#endif
    // init the sensor, test communication
#if MASS_FLOW_METER_SENSOR == MFM_SFM_3300D
    Wire.begin();  // join i2c bus (address optional for master)
    Wire.beginTransmission(MFM_SENSOR_I2C_ADDRESS);

    Wire.write(0x10);
    Wire.write(0x00);

    // mfmTimerCounter = 0;

    mfmFaultCondition = (Wire.endTransmission() != 0);
    delay(100);
#endif

#if MASS_FLOW_METER_SENSOR == MFM_HONEYWELL_HAF
    
/*
Init sequence for Honeywell Zephyr mass flow sensor :
1st read operation: the sensor will send 0x0000
2nd read operation: the sensor will send the first part of the serial number
3rd read operation: the sensor will send the second part of the serial number
Subsequent read operations: the sensor will send calibrated mass air flow values with two leading 0
*/
    Wire.begin();
    Wire.beginTransmission(MFM_SENSOR_I2C_ADDRESS);
    Wire.write(0x02); //Force reset
    Wire.endTransmission();

    Wire.beginTransmission(MFM_SENSOR_I2C_ADDRESS);

    Wire.requestFrom(MFM_SENSOR_I2C_ADDRESS, 2);
    mfmLastData.c[1] = Wire.read();
    mfmLastData.c[0] = Wire.read();

    if (Wire.endTransmission() != 0) {  // If transmission failed
            mfmFaultCondition = true;
            mfmResetStateMachine = MFM_WAIT_RESET_PERIODS;
    }
    Serial.println("Read 1");
    Serial.println(mfmLastData.i);
    Serial.println("fault condition:");
    Serial.println(mfmFaultCondition);
/*
    //Wire.beginTransmission(MFM_SENSOR_I2C_ADDRESS);
    Wire.requestFrom(MFM_SENSOR_I2C_ADDRESS, 2);
    mfmLastData.c[1] = Wire.read();
    mfmLastData.c[0] = Wire.read();
    if (Wire.endTransmission() != 0) {  // If transmission failed
            mfmFaultCondition = true;
            mfmResetStateMachine = 5;
    }
    Serial.println("Read 2");
    Serial.println(mfmLastData.i);
    Serial.println("fault condition:");
    Serial.println(mfmFaultCondition);
    
    //Wire.beginTransmission(MFM_SENSOR_I2C_ADDRESS);
    Wire.requestFrom(MFM_SENSOR_I2C_ADDRESS, 2);
    mfmLastData.c[1] = Wire.read();
    mfmLastData.c[0] = Wire.read();
    if (Wire.endTransmission() != 0) {  // If transmission failed
            mfmFaultCondition = true;
            mfmResetStateMachine = 5;
    }
    Serial.println("Read 3");
    Serial.println(mfmLastData.i);
    

    Serial.println("fault condition:");
    Serial.println(mfmFaultCondition);
*/
    //mfmFaultCondition = (Wire.endTransmission() != 0);
    delay(100);
#endif

#if MASS_FLOW_METER_SENSOR == MFM_SDP703_02
    Wire.endTransmission();
    Wire.begin();

    delay(10);
    Wire.beginTransmission(MFM_SENSOR_I2C_ADDRESS);
    Wire.write(0xFE);
    Wire.endTransmission();
    delay(1);

    Wire.beginTransmission(MFM_SENSOR_I2C_ADDRESS);
    Wire.write(0xE2);
    Wire.write(0x02);
    Wire.write(0x08);
    Wire.endTransmission();

    Wire.beginTransmission(MFM_SENSOR_I2C_ADDRESS);
    Wire.write(0xE4);
    Wire.write(0x76);
    Wire.write(0xA2);
    Wire.endTransmission();

    Wire.beginTransmission(MFM_SENSOR_I2C_ADDRESS);
    Wire.write(0xF1);
    mfmFaultCondition = (Wire.endTransmission() != 0);

    delay(10);

#endif

    massFlowTimer->resume();
    return !mfmFaultCondition;
}

/*
 * Reset the volume counter
 */
void MFM_reset(void) {
    mfmAirVolumeSum = 0;
    mfmSampleCount = 0;
}

/**
 * Calibrate the zero of the sensor :
 * mean of 10 samples.
 */
void MFM_calibrateZero(void) {
#if MASS_FLOW_METER_SENSOR == MFM_OMRON_D6F
    int32_t sum = 0;
    for (int i = 0; i < 10; i++) {
        sum += analogRead(MFM_ANALOG_INPUT);
        delayMicroseconds(5000);
    }
    mfmCalibrationOffset = sum / 10;
#endif
}

/**
 * return the number of milliliters since last reset
 * Can also perform the volume reset in the same atomic operation
 */
int32_t MFM_read_liters(boolean reset_after_read) {

    int32_t result;

#if MASS_FLOW_METER_SENSOR == MFM_SFM_3300D
    // this should be an atomic operation (32 bits aligned data)
    result = mfmFaultCondition ? 999999 : mfmAirVolumeSum / (60 * 120);

    // Correction factor is 120. Divide by 60 to convert ml.min-1 to ml.ms-1, hence the 7200 =
    // 120 * 60
#endif

#if MASS_FLOW_METER_SENSOR == MFM_SDP703_02
    // this should be an atomic operation (32 bits aligned data)
    result = mfmFaultCondition ? 999999 : (mfmAirVolumeSum / 6.5);
#endif

#if MASS_FLOW_METER_SENSOR == MFM_OMRON_D6F
    result = mfmFaultCondition ? 999999 : (mfmAirVolumeSum / 130);
#endif

#if MASS_FLOW_METER_SENSOR == MFM_HONEYWELL_HAF
    result = mfmFaultCondition ? 999999 : ( (mfmAirVolumeSum * MASS_FLOW_PERIOD) / (60 * 10) );
#endif

    if (reset_after_read) {
        MFM_reset();
    }

    return result;
}

#if MODE == MODE_MFM_TESTS

void onStartClick() {
    MFM_reset();
    Serial.println("dtc");
}

OneButton btn_stop(PIN_BTN_ALARM_OFF, false, false);

void setup(void) {

    Serial.begin(115200);
    Serial.println("init mass flow meter");
    boolean ok = MFM_init();

    pinMode(PIN_SERIAL_TX, OUTPUT);
    
    pinMode(PIN_LED_START, OUTPUT);

    pinMode(PIN_LED_GREEN, OUTPUT);

    startScreen();
    resetScreen();
    screen.setCursor(0, 0);
    screen.print("debug prog");
    screen.setCursor(0, 1);
    screen.print("mass flow sensor");
    screen.setCursor(0, 2);
    screen.print(ok ? "sensor OK" : "sensor not OK");

    btn_stop.attachClick(onStartClick);
    btn_stop.setDebounceTicks(0);
    MFM_calibrateZero();
    mfmAirVolumeSum = 0;
    Serial.println("init done");
}

int loopcounter = 0;

void loop(void) {

    delay(10);
    loopcounter++;
    if (loopcounter == 50) {
        loopcounter = 0;

        char buffer[30];

        int32_t volume = MFM_read_liters(false);

        resetScreen();
        screen.setCursor(0, 0);
        screen.print("debug prog");
        screen.setCursor(0, 1);
        screen.print("mass flow sensor");
        screen.setCursor(0, 2);

        if (volume == MASS_FLOW_ERROR_VALUE) {
            screen.print("sensor not OK");
        } else {
            screen.print("sensor OK");
            // screen.print(mfmLastValue);
            screen.setCursor(0, 3);
            sprintf(buffer, "volume=%dmL", volume);
            screen.print(buffer);
        }

        // Serial.print("volume = ");
        // Serial.print(volume);
        // Serial.println("mL");
    }
    btn_stop.tick();
}
#endif