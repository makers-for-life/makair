/**
  ******************************************************************************
  * @file    mc_config.c 
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   Motor Control Subsystem components configuration and handler structures.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */ 
#include "main.h"
#include "mc_type.h"
#include "parameters_conversion.h"
#include "mc_parameters.h"
#include "mc_config.h"

/* USER CODE BEGIN Additional include */
#include "esc.h"
/* USER CODE END Additional include */ 

#define FREQ_RATIO 1                /* Dummy value for single drive */
#define FREQ_RELATION HIGHEST_FREQ  /* Dummy value for single drive */

#define OFFCALIBRWAIT_MS     0
#define OFFCALIBRWAIT_MS2    0     
#include "pqd_motor_power_measurement.h"
/* USER CODE BEGIN Additional define */

/* USER CODE END Additional define */ 

PQD_MotorPowMeas_Handle_t PQD_MotorPowMeasM1 =
{
  .wConvFact = PQD_CONVERSION_FACTOR
};
PQD_MotorPowMeas_Handle_t *pPQD_MotorPowMeasM1 = &PQD_MotorPowMeasM1; 

/**
  * @brief  PI / PID Speed loop parameters Motor 1
  */
PID_Handle_t PIDSpeedHandle_M1 =
{
  .hDefKpGain          = (int16_t)PID_SPEED_KP_DEFAULT,
  .hDefKiGain          = (int16_t)PID_SPEED_KI_DEFAULT, 
  .wUpperIntegralLimit = (int32_t)IQMAX * (int32_t)SP_KIDIV,
  .wLowerIntegralLimit = -(int32_t)IQMAX * (int32_t)SP_KIDIV,
  .hUpperOutputLimit       = (int16_t)IQMAX, 
  .hLowerOutputLimit       = -(int16_t)IQMAX,
  .hKpDivisor          = (uint16_t)SP_KPDIV,
  .hKiDivisor          = (uint16_t)SP_KIDIV,
  .hKpDivisorPOW2      = (uint16_t)SP_KPDIV_LOG,
  .hKiDivisorPOW2      = (uint16_t)SP_KIDIV_LOG,
  .hDefKdGain           = 0x0000U,
  .hKdDivisor           = 0x0000U,
  .hKdDivisorPOW2       = 0x0000U,
};

/**
  * @brief  PI / PID Iq loop parameters Motor 1
  */
PID_Handle_t PIDIqHandle_M1 =
{
  .hDefKpGain          = (int16_t)PID_TORQUE_KP_DEFAULT,
  .hDefKiGain          = (int16_t)PID_TORQUE_KI_DEFAULT,
  .wUpperIntegralLimit = (int32_t)INT16_MAX * TF_KIDIV,
  .wLowerIntegralLimit = (int32_t)-INT16_MAX * TF_KIDIV,   
  .hUpperOutputLimit       = INT16_MAX,     
  .hLowerOutputLimit       = -INT16_MAX,           
  .hKpDivisor          = (uint16_t)TF_KPDIV,       
  .hKiDivisor          = (uint16_t)TF_KIDIV,       
  .hKpDivisorPOW2      = (uint16_t)TF_KPDIV_LOG,       
  .hKiDivisorPOW2      = (uint16_t)TF_KIDIV_LOG,        
  .hDefKdGain           = 0x0000U,
  .hKdDivisor           = 0x0000U,
  .hKdDivisorPOW2       = 0x0000U,
};

/**
  * @brief  PI / PID Id loop parameters Motor 1
  */
PID_Handle_t PIDIdHandle_M1 =
{
  .hDefKpGain          = (int16_t)PID_FLUX_KP_DEFAULT, 
  .hDefKiGain          = (int16_t)PID_FLUX_KI_DEFAULT, 
  .wUpperIntegralLimit = (int32_t)INT16_MAX * TF_KIDIV, 
  .wLowerIntegralLimit = (int32_t)-INT16_MAX * TF_KIDIV,
  .hUpperOutputLimit       = INT16_MAX,                 
  .hLowerOutputLimit       = -INT16_MAX,                
  .hKpDivisor          = (uint16_t)TF_KPDIV,          
  .hKiDivisor          = (uint16_t)TF_KIDIV,          
  .hKpDivisorPOW2      = (uint16_t)TF_KPDIV_LOG,       
  .hKiDivisorPOW2      = (uint16_t)TF_KIDIV_LOG,       
  .hDefKdGain           = 0x0000U,
  .hKdDivisor           = 0x0000U,
  .hKdDivisorPOW2       = 0x0000U,
};

/**
  * @brief  SpeednTorque Controller parameters Motor 1
  */
SpeednTorqCtrl_Handle_t SpeednTorqCtrlM1 =
{
  .STCFrequencyHz =           		MEDIUM_FREQUENCY_TASK_RATE, 	 
  .MaxAppPositiveMecSpeedUnit =	(uint16_t)(MAX_APPLICATION_SPEED_UNIT), 
  .MinAppPositiveMecSpeedUnit =	(uint16_t)(MIN_APPLICATION_SPEED_UNIT), 
  .MaxAppNegativeMecSpeedUnit =	(int16_t)(-MIN_APPLICATION_SPEED_UNIT), 
  .MinAppNegativeMecSpeedUnit =	(int16_t)(-MAX_APPLICATION_SPEED_UNIT),
  .MaxPositiveTorque =				(int16_t)NOMINAL_CURRENT,		 
  .MinNegativeTorque =				-(int16_t)NOMINAL_CURRENT,       
  .ModeDefault =					DEFAULT_CONTROL_MODE,            
  .MecSpeedRefUnitDefault =		(int16_t)(DEFAULT_TARGET_SPEED_UNIT),
  .TorqueRefDefault =				(int16_t)DEFAULT_TORQUE_COMPONENT,
  .IdrefDefault =					(int16_t)DEFAULT_FLUX_COMPONENT,                                                                     
};
RevUpCtrl_Handle_t RevUpControlM1 =
{
  .hRUCFrequencyHz         = MEDIUM_FREQUENCY_TASK_RATE,   
  .hStartingMecAngle       = (int16_t)((int32_t)(STARTING_ANGLE_DEG)* 65536/360),
  .bFirstAccelerationStage = (ENABLE_SL_ALGO_FROM_PHASE-1u),   
  .hMinStartUpValidSpeed   = OBS_MINIMUM_SPEED_UNIT, 
  .hMinStartUpFlySpeed     = (int16_t)(OBS_MINIMUM_SPEED_UNIT/2),  
  .OTFStartupEnabled       = false,  
  .OTFPhaseParams         = {(uint16_t)500,                 
                                         0,                 
                             (int16_t)PHASE5_FINAL_CURRENT,
                             (void*)MC_NULL},
  .ParamsData             = {{(uint16_t)PHASE1_DURATION,(int16_t)(PHASE1_FINAL_SPEED_UNIT),(int16_t)PHASE1_FINAL_CURRENT,&RevUpControlM1.ParamsData[1]},
                             {(uint16_t)PHASE2_DURATION,(int16_t)(PHASE2_FINAL_SPEED_UNIT),(int16_t)PHASE2_FINAL_CURRENT,&RevUpControlM1.ParamsData[2]},
                             {(uint16_t)PHASE3_DURATION,(int16_t)(PHASE3_FINAL_SPEED_UNIT),(int16_t)PHASE3_FINAL_CURRENT,&RevUpControlM1.ParamsData[3]},
                             {(uint16_t)PHASE4_DURATION,(int16_t)(PHASE4_FINAL_SPEED_UNIT),(int16_t)PHASE4_FINAL_CURRENT,&RevUpControlM1.ParamsData[4]},
                             {(uint16_t)PHASE5_DURATION,(int16_t)(PHASE5_FINAL_SPEED_UNIT),(int16_t)PHASE5_FINAL_CURRENT,(void*)MC_NULL},
                            },
};
PWMC_R3_2_Handle_t PWM_Handle_M1 =
{
  {
    .pFctGetPhaseCurrents              = &R3_2_GetPhaseCurrents,    
    .pFctSwitchOffPwm                  = &R3_2_SwitchOffPWM,             
    .pFctSwitchOnPwm                   = &R3_2_SwitchOnPWM,              
    .pFctCurrReadingCalib              = &R3_2_CurrentReadingPolarization,
    .pFctTurnOnLowSides                = &R3_2_TurnOnLowSides,         
    .pFctIsOverCurrentOccurred         = &R3_2_IsOverCurrentOccurred,    
    .pFctOCPSetReferenceVoltage        = MC_NULL,
    .pFctRLDetectionModeEnable         = &R3_2_RLDetectionModeEnable,    
    .pFctRLDetectionModeDisable        = &R3_2_RLDetectionModeDisable,   
    .pFctRLDetectionModeSetDuty        = &R3_2_RLDetectionModeSetDuty,    
    
    .hT_Sqrt3 = (PWM_PERIOD_CYCLES*SQRT3FACTOR)/16384u,   
    .Sector = 0,    
    .CntPhA = 0,
    .CntPhB = 0,
    .CntPhC = 0,
    .SWerror = 0,
    .TurnOnLowSidesAction = false, 
    .OffCalibrWaitTimeCounter = 0, 
    .Motor = M1,     
    .RLDetectionMode = false, 
    .Ia = 0, 
    .Ib = 0, 
    .Ic = 0, 
    .DTTest = 0,   
    .DTCompCnt = 0, 
    .PWMperiod          = PWM_PERIOD_CYCLES,
    .OffCalibrWaitTicks = (uint16_t)((SYS_TICK_FREQUENCY * OFFCALIBRWAIT_MS)/ 1000),
    .DTCompCnt          = DTCOMPCNT,
    .Ton                 = TON,                              
    .Toff                = TOFF
  },
  .PhaseAOffset = 0,   
  .PhaseBOffset = 0,   
  .PhaseCOffset = 0,                               
  .Half_PWMPeriod = PWM_PERIOD_CYCLES/2u, 
  .OverCurrentFlag = false,    
  .OverVoltageFlag = false,    
  .BrakeActionLock = false,                               
  .pParams_str = &R3_2_ParamsM1
};
  
  
  
/**
  * @brief  SpeedNPosition sensor parameters Motor 1 - Base Class
  */
VirtualSpeedSensor_Handle_t VirtualSpeedSensorM1 =
{
  
  ._Super = {
    .bElToMecRatio                     =	POLE_PAIR_NUM, 
    .hMaxReliableMecSpeedUnit          =	(uint16_t)(1.15*MAX_APPLICATION_SPEED_UNIT),
    .hMinReliableMecSpeedUnit          =	(uint16_t)(MIN_APPLICATION_SPEED_UNIT),
    .bMaximumSpeedErrorsNumber         =	MEAS_ERRORS_BEFORE_FAULTS,      
    .hMaxReliableMecAccelUnitP         =	65535,                             
    .hMeasurementFrequency             =	TF_REGULATION_RATE_SCALED,
    .DPPConvFactor                     =  DPP_CONV_FACTOR,       
    },
  .hSpeedSamplingFreqHz =	MEDIUM_FREQUENCY_TASK_RATE, 
  .hTransitionSteps     =	(int16_t)(TF_REGULATION_RATE * TRANSITION_DURATION/ 1000.0),
                           
};

/**
  * @brief  SpeedNPosition sensor parameters Motor 1 - State Observer + PLL
  */
STO_PLL_Handle_t STO_PLL_M1 =
{
  ._Super = {
	.bElToMecRatio                     =	POLE_PAIR_NUM,
    .SpeedUnit                         = SPEED_UNIT,
    .hMaxReliableMecSpeedUnit          =	(uint16_t)(1.15*MAX_APPLICATION_SPEED_UNIT),
    .hMinReliableMecSpeedUnit          =	(uint16_t)(MIN_APPLICATION_SPEED_UNIT),
    .bMaximumSpeedErrorsNumber         =	MEAS_ERRORS_BEFORE_FAULTS,           
    .hMaxReliableMecAccelUnitP         =	65535,                               
    .hMeasurementFrequency             =	TF_REGULATION_RATE_SCALED,
    .DPPConvFactor                     =  DPP_CONV_FACTOR,    
  },
 .hC1                         =	C1,                             
 .hC2                         =	C2,                             
 .hC3                         =	C3,                             
 .hC4                         =	C4,                             
 .hC5                         =	C5,                             
 .hF1                         =	F1,                             
 .hF2                         =	F2,                             
 .PIRegulator = {
     .hDefKpGain = PLL_KP_GAIN, 
     .hDefKiGain = PLL_KI_GAIN, 
	 .hDefKdGain = 0x0000U,     
     .hKpDivisor = PLL_KPDIV,   
     .hKiDivisor = PLL_KIDIV,   
	 .hKdDivisor = 0x0000U,			 
     .wUpperIntegralLimit = INT32_MAX, 
     .wLowerIntegralLimit = -INT32_MAX,
     .hUpperOutputLimit = INT16_MAX, 
     .hLowerOutputLimit = -INT16_MAX, 
     .hKpDivisorPOW2 = PLL_KPDIV_LOG,  
     .hKiDivisorPOW2 = PLL_KIDIV_LOG, 
     .hKdDivisorPOW2       = 0x0000U, 
   },      			
 .SpeedBufferSizeUnit                =	STO_FIFO_DEPTH_UNIT,           
 .SpeedBufferSizeDpp                 =	STO_FIFO_DEPTH_DPP,            
 .VariancePercentage                 =	PERCENTAGE_FACTOR,             
 .SpeedValidationBand_H              =	SPEED_BAND_UPPER_LIMIT,        
 .SpeedValidationBand_L              =	SPEED_BAND_LOWER_LIMIT,        
 .MinStartUpValidSpeed               =	OBS_MINIMUM_SPEED_UNIT,             
 .StartUpConsistThreshold            =	NB_CONSECUTIVE_TESTS,  	       
 .Reliability_hysteresys             =	OBS_MEAS_ERRORS_BEFORE_FAULTS, 
 .BemfConsistencyCheck               =	BEMF_CONSISTENCY_TOL,          
 .BemfConsistencyGain                =	BEMF_CONSISTENCY_GAIN,         
 .MaxAppPositiveMecSpeedUnit         =	(uint16_t)(MAX_APPLICATION_SPEED_UNIT*1.15), 
 .F1LOG                              =	F1_LOG,                            
 .F2LOG                              =	F2_LOG,                            
 .SpeedBufferSizeDppLOG              =	STO_FIFO_DEPTH_DPP_LOG             
};
STO_PLL_Handle_t *pSTO_PLL_M1 = &STO_PLL_M1; 

STO_Handle_t STO_M1 = 
{
  ._Super                        = (SpeednPosFdbk_Handle_t*)&STO_PLL_M1,
  .pFctForceConvergency1         = &STO_PLL_ForceConvergency1,
  .pFctForceConvergency2         = &STO_PLL_ForceConvergency2,
  .pFctStoOtfResetPLL            = &STO_OTF_ResetPLL,
  .pFctSTO_SpeedReliabilityCheck = &STO_PLL_IsVarianceTight                              
};

/**
  * temperature sensor parameters Motor 1
  */
NTC_Handle_t TempSensorParamsM1 =
{
  .bSensorType = REAL_SENSOR,
  .TempRegConv =
  {
    .regADC = ADC1,
    .channel = MC_ADC_CHANNEL_5,
    .samplingTime = M1_TEMP_SAMPLING_TIME,   
  },  
  .hLowPassFilterBW        = M1_TEMP_SW_FILTER_BW_FACTOR,
  .hOverTempThreshold      = (uint16_t)(OV_TEMPERATURE_THRESHOLD_d),
  .hOverTempDeactThreshold = (uint16_t)(OV_TEMPERATURE_THRESHOLD_d - OV_TEMPERATURE_HYSTERESIS_d),
  .hSensitivity            = (uint16_t)(ADC_REFERENCE_VOLTAGE/dV_dT),
  .wV0                     = (uint16_t)(V0_V *65536/ ADC_REFERENCE_VOLTAGE),
  .hT0                     = T0_C,											 
};

/* Bus voltage sensor value filter buffer */
uint16_t RealBusVoltageSensorFilterBufferM1[M1_VBUS_SW_FILTER_BW_FACTOR];

/**
  * Bus voltage sensor parameters Motor 1
  */
RDivider_Handle_t RealBusVoltageSensorParamsM1 =
{
  ._Super                =
  {
    .SensorType          = REAL_SENSOR,                 
    .ConversionFactor    = (uint16_t)(ADC_REFERENCE_VOLTAGE / VBUS_PARTITIONING_FACTOR),                                                   
  },
  
  .VbusRegConv =
  {
    .regADC = ADC1,
    .channel = MC_ADC_CHANNEL_1,
    .samplingTime = M1_VBUS_SAMPLING_TIME,   
  },
  .LowPassFilterBW       =  M1_VBUS_SW_FILTER_BW_FACTOR,  
  .OverVoltageThreshold  = OVERVOLTAGE_THRESHOLD_d,   
  .UnderVoltageThreshold =  UNDERVOLTAGE_THRESHOLD_d,  
  .aBuffer = RealBusVoltageSensorFilterBufferM1,
};

UI_Handle_t UI_Params =
{
  .bDriveNum = 0,
  .pFct_DACInit = &DAC_Init,               
  .pFct_DACExec = &DAC_Exec,
  .pFctDACSetChannelConfig    = &DAC_SetChannelConfig,
  .pFctDACGetChannelConfig    = &DAC_GetChannelConfig,
  .pFctDACSetUserChannelValue = &DAC_SetUserChannelValue,
  .pFctDACGetUserChannelValue = &DAC_GetUserChannelValue,
 
};

DAC_UI_Handle_t DAC_UI_Params = 
{
  .hDAC_CH1_ENABLED = ENABLE,  
  .hDAC_CH2_ENABLED = DISABLE
};

/** RAMP for Motor1.
  *
  */
RampExtMngr_Handle_t RampExtMngrHFParamsM1 =
{
  .FrequencyHz = TF_REGULATION_RATE 
};

/**
  * @brief  CircleLimitation Component parameters Motor 1 - Base Component
  */
CircleLimitation_Handle_t CircleLimitationM1 =
{
  .MaxModule          = MAX_MODULE,
  .MaxVd          	  = (uint16_t)(MAX_MODULE * 950 / 1000),
  .Circle_limit_table = MMITABLE,        	
  .Start_index        = START_INDEX, 		
};

UFCP_Handle_t pUSART =
{
  ._Super.RxTimeout = 0,
  .USARTx = USART2,
       
};

/* USER CODE BEGIN Additional configuration */
ESC_Handle_t ESC_M1 =
{
  .pESC_params = &ESC_ParamsM1,
} ;
/* USER CODE END Additional configuration */ 

/******************* (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/

