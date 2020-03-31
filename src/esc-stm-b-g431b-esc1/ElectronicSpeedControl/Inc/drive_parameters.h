
/**
  ******************************************************************************
  * @file    drive_parameters.h
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file contains the parameters needed for the Motor Control SDK
  *          in order to configure a motor drive.
  *
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DRIVE_PARAMETERS_H
#define __DRIVE_PARAMETERS_H

/************************
 *** Motor Parameters ***
 ************************/

/******** MAIN AND AUXILIARY SPEED/POSITION SENSOR(S) SETTINGS SECTION ********/

/*** Speed measurement settings ***/
#define MAX_APPLICATION_SPEED_RPM       35000 /*!< rpm, mechanical */
#define MIN_APPLICATION_SPEED_RPM       0 /*!< rpm, mechanical,  
                                                           absolute value */
#define MEAS_ERRORS_BEFORE_FAULTS       3 /*!< Number of speed  
                                                             measurement errors before 
                                                             main sensor goes in fault */
/****** State Observer + PLL ****/
#define VARIANCE_THRESHOLD              0.1 /*!<Maximum accepted 
                                                            variance on speed 
                                                            estimates (percentage) */
/* State observer scaling factors F1 */                    
#define F1                               16384
#define F2                               4096
#define F1_LOG                           LOG2(16384)
#define F2_LOG                           LOG2(4096)

/* State observer constants */
#define GAIN1                            -19944
#define GAIN2                            23868
/*Only in case PLL is used, PLL gains */
#define PLL_KP_GAIN                      1862
#define PLL_KI_GAIN                      66
#define PLL_KPDIV     16384
#define PLL_KPDIV_LOG LOG2(PLL_KPDIV)
#define PLL_KIDIV     65535
#define PLL_KIDIV_LOG LOG2(PLL_KIDIV)

#define OBS_MEAS_ERRORS_BEFORE_FAULTS    3  /*!< Number of consecutive errors   
                                                           on variance test before a speed 
                                                           feedback error is reported */
#define STO_FIFO_DEPTH_DPP               64  /*!< Depth of the FIFO used  
                                                            to average mechanical speed  
                                                            in dpp format */
#define STO_FIFO_DEPTH_DPP_LOG           LOG2(64)
                                                            
#define STO_FIFO_DEPTH_UNIT              64  /*!< Depth of the FIFO used  
                                                            to average mechanical speed 
                                                            in the unit defined by #SPEED_UNIT */
#define BEMF_CONSISTENCY_TOL             64   /* Parameter for B-emf 
                                                            amplitude-speed consistency */
#define BEMF_CONSISTENCY_GAIN            64   /* Parameter for B-emf 
                                                           amplitude-speed consistency */
                                                                                

/* USER CODE BEGIN angle reconstruction M1 */
#define PARK_ANGLE_COMPENSATION_FACTOR 0
#define REV_PARK_ANGLE_COMPENSATION_FACTOR 0
/* USER CODE END angle reconstruction M1 */

/**************************    DRIVE SETTINGS SECTION   **********************/
/* PWM generation and current reading */

#define PWM_FREQUENCY   20000
#define PWM_FREQ_SCALING 1

#define LOW_SIDE_SIGNALS_ENABLING        LS_PWM_TIMER
#define SW_DEADTIME_NS                   800 /*!< Dead-time to be inserted  
                                                           by FW, only if low side 
                                                           signals are enabled */
                                                                                         
/* Torque and flux regulation loops */
#define REGULATION_EXECUTION_RATE     1    /*!< FOC execution rate in 
                                                           number of PWM cycles */     
/* Gains values for torque and flux control loops */
#define PID_TORQUE_KP_DEFAULT         3560       
#define PID_TORQUE_KI_DEFAULT         2684
#define PID_TORQUE_KD_DEFAULT         100
#define PID_FLUX_KP_DEFAULT           3560
#define PID_FLUX_KI_DEFAULT           2684
#define PID_FLUX_KD_DEFAULT           100

/* Torque/Flux control loop gains dividers*/
#define TF_KPDIV                      1024
#define TF_KIDIV                      2048
#define TF_KDDIV                      8192
#define TF_KPDIV_LOG                  LOG2(1024)
#define TF_KIDIV_LOG                  LOG2(2048)
#define TF_KDDIV_LOG                  LOG2(8192)
#define TFDIFFERENTIAL_TERM_ENABLING  DISABLE

/* Speed control loop */ 
#define SPEED_LOOP_FREQUENCY_HZ       1000 /*!<Execution rate of speed   
                                                      regulation loop (Hz) */
                                        
#define PID_SPEED_KP_DEFAULT          2852/(SPEED_UNIT/10) /* Workbench compute the gain for 01Hz unit*/
#define PID_SPEED_KI_DEFAULT          78/(SPEED_UNIT/10) /* Workbench compute the gain for 01Hz unit*/
#define PID_SPEED_KD_DEFAULT          0/(SPEED_UNIT/10) /* Workbench compute the gain for 01Hz unit*/
/* Speed PID parameter dividers */
#define SP_KPDIV                      1024
#define SP_KIDIV                      16384
#define SP_KDDIV                      16
#define SP_KPDIV_LOG                  LOG2(1024)
#define SP_KIDIV_LOG                  LOG2(16384)
#define SP_KDDIV_LOG                  LOG2(16)

/* USER CODE BEGIN PID_SPEED_INTEGRAL_INIT_DIV */
#define PID_SPEED_INTEGRAL_INIT_DIV 1 /*  */
/* USER CODE END PID_SPEED_INTEGRAL_INIT_DIV */

#define SPD_DIFFERENTIAL_TERM_ENABLING DISABLE
#define IQMAX                          980

/* Default settings */
#define DEFAULT_CONTROL_MODE           STC_SPEED_MODE /*!< STC_TORQUE_MODE or 
                                                        STC_SPEED_MODE */
#define DEFAULT_TARGET_SPEED_RPM      5384
#define DEFAULT_TARGET_SPEED_UNIT      (DEFAULT_TARGET_SPEED_RPM*SPEED_UNIT/_RPM)
#define DEFAULT_TORQUE_COMPONENT       0
#define DEFAULT_FLUX_COMPONENT         0

/**************************    FIRMWARE PROTECTIONS SECTION   *****************/
#define OV_VOLTAGE_PROT_ENABLING        ENABLE
#define UV_VOLTAGE_PROT_ENABLING        ENABLE
#define OV_VOLTAGE_THRESHOLD_V          28 /*!< Over-voltage 
                                                         threshold */
#define UD_VOLTAGE_THRESHOLD_V          8 /*!< Under-voltage 
                                                          threshold */
#if 0
#define ON_OVER_VOLTAGE                 TURN_OFF_PWM /*!< TURN_OFF_PWM, 
                                                         TURN_ON_R_BRAKE or 
                                                         TURN_ON_LOW_SIDES */
#endif /* 0 */
#define R_BRAKE_SWITCH_OFF_THRES_V      22

#define OV_TEMPERATURE_THRESHOLD_C      110 /*!< Celsius degrees */
#define OV_TEMPERATURE_HYSTERESIS_C     10 /*!< Celsius degrees */

#define HW_OV_CURRENT_PROT_BYPASS       DISABLE /*!< In case ON_OVER_VOLTAGE  
                                                          is set to TURN_ON_LOW_SIDES
                                                          this feature may be used to
                                                          bypass HW over-current
                                                          protection (if supported by 
                                                          power stage) */
/******************************   START-UP PARAMETERS   **********************/

/* Phase 1 */
#define PHASE1_DURATION                1000 /*milliseconds */
#define PHASE1_FINAL_SPEED_UNIT         (0*SPEED_UNIT/_RPM) 
#define PHASE1_FINAL_CURRENT           245
/* Phase 2 */
#define PHASE2_DURATION                10000 /*milliseconds */
#define PHASE2_FINAL_SPEED_UNIT         (5982*SPEED_UNIT/_RPM)
#define PHASE2_FINAL_CURRENT           490
/* Phase 3 */
#define PHASE3_DURATION                0 /*milliseconds */
#define PHASE3_FINAL_SPEED_UNIT         (5982*SPEED_UNIT/_RPM)
#define PHASE3_FINAL_CURRENT           490
/* Phase 4 */
#define PHASE4_DURATION                0 /*milliseconds */
#define PHASE4_FINAL_SPEED_UNIT         (5982*SPEED_UNIT/_RPM)
#define PHASE4_FINAL_CURRENT           490
/* Phase 5 */
#define PHASE5_DURATION                0 /* milliseconds */
#define PHASE5_FINAL_SPEED_UNIT         (5982*SPEED_UNIT/_RPM)
#define PHASE5_FINAL_CURRENT           490

#define ENABLE_SL_ALGO_FROM_PHASE      2
/* Sensor-less rev-up sequence */
#define STARTING_ANGLE_DEG             0  /*!< degrees [0...359] */                                                             
/* Observer start-up output conditions  */
#define OBS_MINIMUM_SPEED_RPM          5384

#define NB_CONSECUTIVE_TESTS           2 /* corresponding to 
                                                         former NB_CONSECUTIVE_TESTS/
                                                         (TF_REGULATION_RATE/
                                                         MEDIUM_FREQUENCY_TASK_RATE) */
#define SPEED_BAND_UPPER_LIMIT         17 /*!< It expresses how much 
                                                            estimated speed can exceed 
                                                            forced stator electrical 
                                                            without being considered wrong. 
                                                            In 1/16 of forced speed */
#define SPEED_BAND_LOWER_LIMIT         15  /*!< It expresses how much 
                                                             estimated speed can be below 
                                                             forced stator electrical 
                                                             without being considered wrong.
                                                             In 1/16 of forced speed */ 
                                                                                    
#define TRANSITION_DURATION            25  /* Switch over duration, ms */ 
/******************************   BUS VOLTAGE Motor 1  **********************/
#define  M1_VBUS_SAMPLING_TIME  LL_ADC_SAMPLING_CYCLE(47)
/******************************   Temperature sensing Motor 1  **********************/
#define  M1_TEMP_SAMPLING_TIME  LL_ADC_SAMPLING_CYCLE(47)
/******************************   Current sensing Motor 1   **********************/
#define ADC_SAMPLING_CYCLES (12 + SAMPLING_CYCLE_CORRECTION)

/******************************   ADDITIONAL FEATURES   **********************/

/*** On the fly start-up ***/

/**************************
 *** Control Parameters ***
 **************************/

/* ##@@_USER_CODE_START_##@@ */
/* ##@@_USER_CODE_END_##@@ */

#endif /*__DRIVE_PARAMETERS_H*/
/******************* (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
