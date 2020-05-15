/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file mass_flow_meter.h
 * @brief Mass flow meter management
 *****************************************************************************/

#pragma once

#define MASS_FLOW_ERROR_VALUE 999999

#if MASS_FLOW_METER_SENSOR == MFM_SFM_3300D
#define MFM_SENSOR_I2C_ADDRESS 0x40
#endif

#if MASS_FLOW_METER_SENSOR == MFM_SDP703_02
#define MFM_SENSOR_I2C_ADDRESS 0x40
#endif

#if MASS_FLOW_METER_SENSOR == MFM_HONEYWELL_HAF
#define MFM_SENSOR_I2C_ADDRESS 0x49
#endif