/**
  ******************************************************************************
  * @file    hall_speed_pos_fdbk.h
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file contains all definitions and functions prototypes for the
  *          Hall Speed & Position Feedback component of the Motor Control SDK.
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
  * @ingroup hall_speed_pos_fdbk
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HALL_SPEEDNPOSFDBK_H
#define __HALL_SPEEDNPOSFDBK_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "speed_pos_fdbk.h"

/** @addtogroup MCSDK
  * @{
  */

/** @addtogroup SpeednPosFdbk
  * @{
  */

/** @addtogroup hall_speed_pos_fdbk
 * @{
 */

#define HALL_SPEED_FIFO_SIZE  ((uint8_t)18)

/* HALL SENSORS PLACEMENT ----------------------------------------------------*/
#define DEGREES_120 0u
#define DEGREES_60 1u


/* Exported types ------------------------------------------------------------*/

/**
  * @brief HALL component parameters definition
  *
  *  <Type @p type represents a thing that needs to be detailed more. Additional details
  * are provided in the detailed section of the doxygen comment block.
  *
  * The brief line should be brief and light. It should avoid useless repetitions and expression such as
  * "the CCC_Type_t type...". Expressions like "This type ..." are tolerated especially for key types
  * (usually structures) where we may want ot be more formal.
  *
  * In general: be direct, avoid the obvious, tell the hidden.>
  */

typedef struct
{
  SpeednPosFdbk_Handle_t _Super;
  /* SW Settings */
  uint8_t  SensorPlacement; /*!< Define here the mechanical position of the sensors
                             with reference to an electrical cycle.
                             Allowed values are: DEGREES_120 or DEGREES_60.*/

  int16_t  PhaseShift;  /*!< Define here in s16degree the electrical phase shift
                             between the low to high transition of signal H1 and
                             the maximum of the Bemf induced on phase A.*/

  uint16_t SpeedSamplingFreqHz; /*!< Frequency (Hz) at which motor speed is to
                             be computed. It must be equal to the frequency
                             at which function SPD_CalcAvrgMecSpeedUnit
                             is called.*/

  uint8_t  SpeedBufferSize; /*!< Size of the buffer used to calculate the average
                             speed. It must be less than 18.*/


  /* HW Settings */
  uint32_t TIMClockFreq; /*!< Timer clock frequency express in Hz.*/

  TIM_TypeDef * TIMx;   /*!< Timer used for HALL sensor management.*/


  GPIO_TypeDef * H1Port;
  /*!< HALL sensor H1 channel GPIO input port (if used,
       after re-mapping). It must be GPIOx x= A, B, ...*/

  uint32_t  H1Pin;      /*!< HALL sensor H1 channel GPIO output pin (if used,
                             after re-mapping). It must be GPIO_Pin_x x= 0, 1,
                             ...*/

  GPIO_TypeDef * H2Port;
  /*!< HALL sensor H2 channel GPIO input port (if used,
       after re-mapping). It must be GPIOx x= A, B, ...*/

  uint32_t  H2Pin;      /*!< HALL sensor H2 channel GPIO output pin (if used,
                             after re-mapping). It must be GPIO_Pin_x x= 0, 1,
                             ...*/

  GPIO_TypeDef * H3Port;
  /*!< HALL sensor H3 channel GPIO input port (if used,
       after re-mapping). It must be GPIOx x= A, B, ...*/

  uint32_t H3Pin;      /*!< HALL sensor H3 channel GPIO output pin (if used,
                             after re-mapping). It must be GPIO_Pin_x x= 0, 1,
                             ...*/

  uint8_t ICx_Filter;               /*!< Input Capture filter selection */

  bool SensorIsReliable;            /*!< Flag to indicate a wrong configuration
                                         of the Hall sensor signanls.*/

  volatile bool RatioDec;           /*!< Flag to avoid consecutive prescaler
                                         decrement.*/
  volatile bool RatioInc;           /*!< Flag to avoid consecutive prescaler
                                         increment.*/
  volatile uint8_t FirstCapt;      /*!< Flag used to discard first capture for
                                         the speed measurement*/
  volatile uint8_t BufferFilled;   /*!< Indicate the number of speed measuremt
                                         present in the buffer from the start.
                                         It will be max bSpeedBufferSize and it
                                         is used to validate the start of speed
                                         averaging. If bBufferFilled is below
                                         bSpeedBufferSize the instantaneous
                                         measured speed is returned as average
                                         speed.*/
  volatile uint8_t OVFCounter;     /*!< Count overflows if prescaler is too low
                                         */

  int32_t SensorPeriod[HALL_SPEED_FIFO_SIZE];/*!< Holding the last
                                         period captures */                                         
                                         
  uint8_t SpeedFIFOIdx;/*!< Pointer of next element to be stored in
                                         the speed sensor buffer*/
  int16_t  CurrentSpeed; /*!< Latest speed computed in HALL_IRQ_HANDLER*/

  int32_t  ElPeriodSum; /* Period accumulator used to speed up the average speed computation*/

  int16_t PrevRotorFreq; /*!< Used to store the last valid rotor electrical
                               speed in dpp used when HALL_MAX_PSEUDO_SPEED
                               is detected */
  int8_t Direction;          /*!< Instantaneous direction of rotor between two
                               captures*/
  int8_t  NewSpeedAcquisition; /*!< Indacate that new speed information has
                                     been stored in the buffer.*/

  int16_t AvrElSpeedDpp; /*!< It is the averaged rotor electrical speed express
                               in s16degree per current control period.*/

  uint8_t HallState;     /*!< Current HALL state configuration */

  int16_t DeltaAngle;    /*!< Delta angle at the Hall sensor signal edge between
                               current electrical rotor angle of synchronism.
                               It is in s16degrees.*/
  int16_t MeasuredElAngle;/*!< This is the electrical angle  measured at each
                               Hall sensor signal edge. It is considered the
                               best measurement of electrical rotor angle.*/
  int16_t TargetElAngle; /*!< This is the electrical angle target computed at
                               speed control frequency based on hMeasuredElAngle.*/
  int16_t CompSpeed;     /*!< Speed compensation factor used to syncronize
                               the current electrical angle with the target
                               electrical angle. */

  uint16_t HALLMaxRatio; /*!< Max TIM prescaler ratio defining the lowest
                             expected speed feedback.*/
  uint16_t SatSpeed;     /*!< Returned value if the measured speed is above the
                             maximum realistic.*/
  uint32_t PseudoFreqConv;/*!< Conversion factor between time interval Delta T
                             between HALL sensors captures, express in timer
                             counts, and electrical rotor speed express in dpp.
                             Ex. Rotor speed (dpp) = wPseudoFreqConv / Delta T
                             It will be ((CKTIM / 6) / (SAMPLING_FREQ)) * 65536.*/

  uint32_t MaxPeriod;  /*!< Time delay between two sensor edges when the speed
                             of the rotor is the minimum realistic in the
                             application: this allows to discriminate too low
                             freq for instance.
                             This period shoud be expressed in timer counts and
                             it will be:
                             wMaxPeriod = ((10 * CKTIM) / 6) / MinElFreq(0.1Hz).*/

  uint32_t MinPeriod;
  /*!< Time delay between two sensor edges when the speed
       of the rotor is the maximum realistic in the
       application: this allows discriminating glitches
       for instance.
       This period shoud be expressed in timer counts and
       it will be:
       wSpeedOverflow = ((10 * CKTIM) / 6) / MaxElFreq(0.1Hz).*/

  uint16_t HallTimeout;/*!< Max delay between two Hall sensor signal to assert
                             zero speed express in milliseconds.*/

  uint16_t OvfFreq;   /*!< Frequency of timer overflow (from 0 to 0x10000)
                             it will be: hOvfFreq = CKTIM /65536.*/
  uint16_t PWMNbrPSamplingFreq; /*!< Number of current control periods inside
                             each speed control periods it will be:
                             (hMeasurementFrequency / hSpeedSamplingFreqHz) - 1.*/
  uint8_t PWMFreqScaling; /*!< Scaling factor to allow to store a PWMFrequency greater than 16 bits */
  
  bool HallMtpa; /* if true at each sensor toggling, the true angle is set without ramp*/

} HALL_Handle_t;
/**
  * @}
  */


void * HALL_TIMx_UP_IRQHandler( void * pHandleVoid );
void * HALL_TIMx_CC_IRQHandler( void * pHandleVoid );
void HALL_Init( HALL_Handle_t * pHandle );
void HALL_Clear( HALL_Handle_t * pHandle );
int16_t HALL_CalcElAngle( HALL_Handle_t * pHandle );
bool HALL_CalcAvrgMecSpeedUnit( HALL_Handle_t * pHandle, int16_t * hMecSpeedUnit );
void HALL_SetMecAngle( HALL_Handle_t * pHandle, int16_t hMecAngle );

/**
  * @}
  */

/**
  * @}
  */

/** @} */

#endif /*__HALL_SPEEDNPOSFDBK_H*/

/******************* (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
