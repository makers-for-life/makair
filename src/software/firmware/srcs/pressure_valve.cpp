/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file pressure_valve.cpp
 * @brief Tools to control an Pressure Valve's servomotor
 *****************************************************************************/

#pragma once

// INCLUDES ===================================================================

// Associated header
#include "../includes/pressure_valve.h"

// External libraries
#include <algorithm>

// Internal libraries
#include "../includes/parameters.h"

// FUNCTIONS ==================================================================
PressureValve::PressureValve() {}

PressureValve::PressureValve(HardwareTimer* p_hardwareTimer,
	uint16_t p_timerChannel,
	uint16_t p_servoPin,
	uint16_t p_openApertureAngle,
	uint16_t p_closeApertureAngle) {
	actuator = p_hardwareTimer;
	timerChannel = p_timerChannel;
	servoPin = p_servoPin;
	openApertureAngle = p_openApertureAngle;
	closeApertureAngle = p_closeApertureAngle;
	minApertureAngle = min(p_closeApertureAngle, p_openApertureAngle);
	maxApertureAngle = max(p_closeApertureAngle, p_openApertureAngle);
	command = p_closeApertureAngle;
	position = -1;
}

void PressureValve::setup() {
	actuator->setMode(timerChannel, TIMER_OUTPUT_COMPARE_PWM1, servoPin);
	actuator->setCaptureCompare(timerChannel, 0, PERCENT_COMPARE_FORMAT);
}

void PressureValve::open() { command = openApertureAngle; }

void PressureValve::close() { command = closeApertureAngle; }

void PressureValve::open(uint16_t p_command) { command = p_command; }

uint16_t PWM_LUT(uint16_t input)
{
	float inputf = input;
	float out;
	
	//Entre 0 % et 5 % LUT(x) = 0
    //Entre 5 % et 95 % LUT(x) = 3.9229E-05*x ^ 3 + 0.06*x + 48
    //Entre 95 % et 100 % LUT(x) = 100
	if(inputf >= 0 && inputf <= 5.0)
	{
		out = 0.0;
	}
	else if(inputf > 5.0 && inputf < 95.0)
	{
		out = 3.9229E-05*inputf*inputf*inputf + 0.06*inputf + 48.0;
	}
	else
	{
		out = 100.0;
	}	
	
	out *= 20; // map(out, 0, 100.0, 0, 2000);	
	return out;
}

uint16_t valveAngle2MicroSeconds(uint16_t value) {
#if VALVE_TYPE == VT_SERVO_V1
	// map 0 - 125 ° to 0.8 - 1.2 ms (standard PPM command is 1ms - 2ms, these servomotors handle a
	// greater range)
	return map(value, 0, 125, 800, 2200);
#elif VALVE_TYPE == VT_EMERSON_ASCO
	// default value
	// compare units are a bit weird in this micro controller
	// TODO: avoid 500ns spikes
	uint16_t result = 1;

	// map 0 - 125 ° to 100% pwm - EMERSON_MIN_PWM (0% when fully closed)
//	if(VALVE_CLOSED_STATE != value) {
//		result = map(value, 0, 125, SERVO_VALVE_PERIOD, EMERSON_MIN_PWM);
//	}
	
	//	result = (uint16_t)map(value, 0, 125, 0, 100);
		return PWM_LUT((uint16_t)map(value, 0, 125, 100, 0)); //CG: Inverted 100 & 0
		//	return result;
#endif
}
