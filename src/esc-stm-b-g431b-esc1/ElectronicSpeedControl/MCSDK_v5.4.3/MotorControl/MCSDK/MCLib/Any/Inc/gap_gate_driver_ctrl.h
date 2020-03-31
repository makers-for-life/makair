/**
  ******************************************************************************
  * @file    gap_gate_driver_ctrl.h
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file contains all definitions and functions prototypes for the
  *          gap_gate_driver_ctrl component of the Motor Control SDK.
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
  * @ingroup GAP_GATE_DRIVER_CTRL
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GAP_GATE_DRIVER_CTR_H
#define __GAP_GATE_DRIVER_CTR_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "mc_type.h"

/** @addtogroup MCSDK
  * @{
  */

/** @addtogroup GAP_GATE_DRIVER_CTRL
  * @{
  */

#define GPIO_NoRemap_SPI                            ((uint32_t)(0))
#define MAX_DEVICES_NUMBER                          7

#define CFG1_REG_MASK    (uint8_t)(0xFF)
#define CFG2_REG_MASK    (uint8_t)(0xFF)
#define CFG3_REG_MASK    (uint8_t)(0xFF)
#define CFG4_REG_MASK    (uint8_t)(0x3F)
#define CFG5_REG_MASK    (uint8_t)(0x0F)
#define STATUS1_REG_MASK (uint8_t)(0xFF)
#define STATUS2_REG_MASK (uint8_t)(0x06)
#define STATUS3_REG_MASK (uint8_t)(0x1F) 
#define TEST1_REG_MASK   (uint8_t)(0x1F)
#define DIAG1_REG_MASK   (uint8_t)(0xFF)
#define DIAG2_REG_MASK   (uint8_t)(0xFF)

#define GAP_ERROR_CLEAR                             (uint32_t)(0x00000000)
#define GAP_ERROR_CODE_UVLOD                        (uint32_t)(0x00000001)
#define GAP_ERROR_CODE_OVLOD                        (uint32_t)(0x00000002)
#define GAP_ERROR_CODE_REGERRL                      (uint32_t)(0x00000004)
#define GAP_ERROR_CODE_SPI_ERR                      (uint32_t)(0x00000008)
#define GAP_ERROR_CODE_DT_ERR                       (uint32_t)(0x00000010)
#define GAP_ERROR_CODE_CFG                          (uint32_t)(0x00000020)
#define GAP_ERROR_CODE_GATE                         (uint32_t)(0x00000100)
#define GAP_ERROR_CODE_ASC                          (uint32_t)(0x00000200)
#define GAP_ERROR_CODE_REGERRR                      (uint32_t)(0x00000400)
#define GAP_ERROR_CODE_TWN                          (uint32_t)(0x00010000)
#define GAP_ERROR_CODE_TSD                          (uint32_t)(0x00020000)
#define GAP_ERROR_CODE_UVLOL                        (uint32_t)(0x00040000)
#define GAP_ERROR_CODE_UVLOH                        (uint32_t)(0x00080000)
#define GAP_ERROR_CODE_SENSE                        (uint32_t)(0x00100000)
#define GAP_ERROR_CODE_DESAT                        (uint32_t)(0x00200000)
#define GAP_ERROR_CODE_OVLOL                        (uint32_t)(0x00400000)
#define GAP_ERROR_CODE_OVLOH                        (uint32_t)(0x00800000)
#define GAP_ERROR_CODE_SPI_CRC                      (uint32_t)(0x40000000)
#define GAP_ERROR_CODE_DEVICES_NOT_PROGRAMMABLE     (uint32_t)(0x80000000)

#define GAP_CFG1_CRC_SPI                            (uint8_t)(0x80)
#define GAP_CFG1_UVLOD                              (uint8_t)(0x40)
#define GAP_CFG1_SD_FLAG                            (uint8_t)(0x20)
#define GAP_CFG1_DIAG_EN                            (uint8_t)(0x10)
#define GAP_CFG1_DT_DISABLE                         (uint8_t)(0x00)
#define GAP_CFG1_DT_250NS                           (uint8_t)(0x04)
#define GAP_CFG1_DT_800NS                           (uint8_t)(0x08)
#define GAP_CFG1_DT_1200NS                          (uint8_t)(0x0C)
#define GAP_CFG1_INFILTER_DISABLE                   (uint8_t)(0x00)
#define GAP_CFG1_INFILTER_210NS                     (uint8_t)(0x01)
#define GAP_CFG1_INFILTER_560NS                     (uint8_t)(0x02)
#define GAP_CFG1_INFILTER_70NS                      (uint8_t)(0x03)

#define GAP_CFG2_SENSETH_100MV                      (uint8_t)(0x00)
#define GAP_CFG2_SENSETH_125MV                      (uint8_t)(0x20)
#define GAP_CFG2_SENSETH_150MV                      (uint8_t)(0x40)
#define GAP_CFG2_SENSETH_175MV                      (uint8_t)(0x60)
#define GAP_CFG2_SENSETH_200MV                      (uint8_t)(0x80)
#define GAP_CFG2_SENSETH_250MV                      (uint8_t)(0xA0)
#define GAP_CFG2_SENSETH_300MV                      (uint8_t)(0xC0)
#define GAP_CFG2_SENSETH_400MV                      (uint8_t)(0xE0)
#define GAP_CFG2_DESATCURR_250UA                    (uint8_t)(0x00)
#define GAP_CFG2_DESATCURR_500UA                    (uint8_t)(0x08)
#define GAP_CFG2_DESATCURR_750UA                    (uint8_t)(0x10)
#define GAP_CFG2_DESATCURR_1000UA                   (uint8_t)(0x18)
#define GAP_CFG2_DESATTH_3V                         (uint8_t)(0x00)
#define GAP_CFG2_DESATTH_4V                         (uint8_t)(0x01)
#define GAP_CFG2_DESATTH_5V                         (uint8_t)(0x02)
#define GAP_CFG2_DESATTH_6V                         (uint8_t)(0x03)
#define GAP_CFG2_DESATTH_7V                         (uint8_t)(0x04)
#define GAP_CFG2_DESATTH_8V                         (uint8_t)(0x05)
#define GAP_CFG2_DESATTH_9V                         (uint8_t)(0x06)
#define GAP_CFG2_DESATTH_10V                        (uint8_t)(0x07)

#define GAP_CFG3_2LTOTH_7_0V                        (uint8_t)(0x00)
#define GAP_CFG3_2LTOTH_7_5V                        (uint8_t)(0x10)
#define GAP_CFG3_2LTOTH_8_0V                        (uint8_t)(0x20)
#define GAP_CFG3_2LTOTH_8_5V                        (uint8_t)(0x30)
#define GAP_CFG3_2LTOTH_9_0V                        (uint8_t)(0x40)
#define GAP_CFG3_2LTOTH_9_5V                        (uint8_t)(0x50)
#define GAP_CFG3_2LTOTH_10_0V                       (uint8_t)(0x60)
#define GAP_CFG3_2LTOTH_10_5V                       (uint8_t)(0x70)
#define GAP_CFG3_2LTOTH_11_0V                       (uint8_t)(0x80)
#define GAP_CFG3_2LTOTH_11_5V                       (uint8_t)(0x90)
#define GAP_CFG3_2LTOTH_12_0V                       (uint8_t)(0xA0)
#define GAP_CFG3_2LTOTH_12_5V                       (uint8_t)(0xB0)
#define GAP_CFG3_2LTOTH_13_0V                       (uint8_t)(0xC0)
#define GAP_CFG3_2LTOTH_13_5V                       (uint8_t)(0xD0)
#define GAP_CFG3_2LTOTH_14_0V                       (uint8_t)(0xE0)
#define GAP_CFG3_2LTOTH_14_5V                       (uint8_t)(0xF0)
#define GAP_CFG3_2LTOTIME_DISABLE                   (uint8_t)(0x00)
#define GAP_CFG3_2LTOTIME_0_75_US                   (uint8_t)(0x01)
#define GAP_CFG3_2LTOTIME_1_00_US                   (uint8_t)(0x02)
#define GAP_CFG3_2LTOTIME_1_50_US                   (uint8_t)(0x03)
#define GAP_CFG3_2LTOTIME_2_00_US                   (uint8_t)(0x04)
#define GAP_CFG3_2LTOTIME_2_50_US                   (uint8_t)(0x05)
#define GAP_CFG3_2LTOTIME_3_00_US                   (uint8_t)(0x06)
#define GAP_CFG3_2LTOTIME_3_50_US                   (uint8_t)(0x07)
#define GAP_CFG3_2LTOTIME_3_75_US                   (uint8_t)(0x08)
#define GAP_CFG3_2LTOTIME_4_00_US                   (uint8_t)(0x09)
#define GAP_CFG3_2LTOTIME_4_25_US                   (uint8_t)(0x0A)
#define GAP_CFG3_2LTOTIME_4_50_US                   (uint8_t)(0x0B)
#define GAP_CFG3_2LTOTIME_4_75_US                   (uint8_t)(0x0C)
#define GAP_CFG3_2LTOTIME_5_00_US                   (uint8_t)(0x0D)
#define GAP_CFG3_2LTOTIME_5_25_US                   (uint8_t)(0x0E)
#define GAP_CFG3_2LTOTIME_5_50_US                   (uint8_t)(0x0F)

#define GAP_CFG4_OVLO                               (uint8_t)(0x20)
#define GAP_CFG4_UVLOLATCH                          (uint8_t)(0x10)
#define GAP_CFG4_UVLOTH_VH_DISABLE                  (uint8_t)(0x00)
#define GAP_CFG4_UVLOTH_VH_10V                      (uint8_t)(0x01)
#define GAP_CFG4_UVLOTH_VH_12V                      (uint8_t)(0x02)
#define GAP_CFG4_UVLOTH_VH_14V                      (uint8_t)(0x03)
#define GAP_CFG4_UVLOTH_VL_DISABLE                  (uint8_t)(0x00)
#define GAP_CFG4_UVLOTH_VL_N3V                      (uint8_t)(0x04)
#define GAP_CFG4_UVLOTH_VL_N5V                      (uint8_t)(0x08)
#define GAP_CFG4_UVLOTH_VL_N7V                      (uint8_t)(0x0C)

#define GAP_CFG5_2LTO_ON_FAULT                      (uint8_t)(0x08)
#define GAP_CFG5_CLAMP_EN                           (uint8_t)(0x04)
#define GAP_CFG5_DESAT_EN                           (uint8_t)(0x02)
#define GAP_CFG5_SENSE_EN                           (uint8_t)(0x01)

#define GAP_STATUS1_OVLOH                           (uint8_t)(0x80)
#define GAP_STATUS1_OVLOL                           (uint8_t)(0x40)
#define GAP_STATUS1_DESAT                           (uint8_t)(0x20)
#define GAP_STATUS1_SENSE                           (uint8_t)(0x10)
#define GAP_STATUS1_UVLOH                           (uint8_t)(0x08)
#define GAP_STATUS1_UVLOL                           (uint8_t)(0x04)
#define GAP_STATUS1_TSD                             (uint8_t)(0x02)
#define GAP_STATUS1_TWN                             (uint8_t)(0x01)

#define GAP_STATUS2_REGERRR                         (uint8_t)(0x04)
#define GAP_STATUS2_ASC                             (uint8_t)(0x02)
#define GAP_STATUS2_GATE                            (uint8_t)(0x01)

#define GAP_STATUS3_CFG                             (uint8_t)(0x20)
#define GAP_STATUS3_DT_ERR                          (uint8_t)(0x10)
#define GAP_STATUS3_SPI_ERR                         (uint8_t)(0x08)
#define GAP_STATUS3_REGERRL                         (uint8_t)(0x04)
#define GAP_STATUS3_OVLOD                           (uint8_t)(0x02)
#define GAP_STATUS3_UVLOD                           (uint8_t)(0x01)

#define GAP_TEST1_GOFFCHK                           (uint8_t)(0x10)
#define GAP_TEST1_GONCHK                            (uint8_t)(0x08)
#define GAP_TEST1_DESCHK                            (uint8_t)(0x04)
#define GAP_TEST1_SNSCHK                            (uint8_t)(0x02)
#define GAP_TEST1_RCHK                              (uint8_t)(0x01)

#define GAP_DIAG_SPI_REGERR                         (uint8_t)(0x80)
#define GAP_DIAG_UVLOD_OVLOD                        (uint8_t)(0x40)
#define GAP_DIAG_UVLOH_UVLOL                        (uint8_t)(0x20)
#define GAP_DIAG_OVLOH_OVLOL                        (uint8_t)(0x10)
#define GAP_DIAG_DESAT_SENSE                        (uint8_t)(0x08)
#define GAP_DIAG_ASC_DT_ERR                         (uint8_t)(0x04)
#define GAP_DIAG_TSD                                (uint8_t)(0x02)
#define GAP_DIAG_TWN                                (uint8_t)(0x01)
#define GAP_DIAG_NONE                               (uint8_t)(0x00)

/** Define the GAP register enum.
  *
  */
typedef enum
{
  CFG1    = 0x0C,
  CFG2    = 0x1D,
  CFG3    = 0x1E,
  CFG4    = 0x1F,
  CFG5    = 0x19,
  STATUS1 = 0x02,
  STATUS2 = 0x01,
  STATUS3 = 0x0A,
  TEST1   = 0x11,
  DIAG1   = 0x05,
  DIAG2   = 0x06
} GAP_Registers_Handle_t;

/** Define test modes
  *
  */
typedef enum
{
  SENSE_RESISTOR_CHK,
  SENSE_COMPARATOR_CHK,
  GON_CHK,
  GOFF_CHK,
  DESAT_CHK
} GAP_TestMode_t;

/**
  * @brief  GAP class parameters definition
  */
typedef struct
{
  uint8_t CFG1;                 /*!< Configuration value for CFG1 register. */
  uint8_t CFG2;                 /*!< Configuration value for CFG2 register. */
  uint8_t CFG3;                 /*!< Configuration value for CFG3 register. */
  uint8_t CFG4;                 /*!< Configuration value for CFG4 register. */
  uint8_t CFG5;                 /*!< Configuration value for CFG5 register. */
  uint8_t DIAG1;                /*!< Configuration value for DIAG1 register. */
  uint8_t DIAG2;                /*!< Configuration value for DIAG2 register. */
} GAP_DeviceParams_Handle_t;

typedef struct
{
  uint8_t       DeviceNum;      /*!< Number of GAP used in the daisy chain */
  GAP_DeviceParams_Handle_t DeviceParams[MAX_DEVICES_NUMBER];
  /*!< Device parameters pointers */
  /* SPI related parameters.
  */
  SPI_TypeDef *  SPIx;
  GPIO_TypeDef * NCSPort;        /*!< GPIO port used by NCS. It must
                                     be equal to GPIOx x= A, B, ...*/
  uint16_t      NCSPin;         /*!< GPIO pin used by NCS. It must be
                                     equal to GPIO_Pin_x x= 0, 1, ...*/
  GPIO_TypeDef * NSDPort;        /*!< GPIO port used by NSD. It must
                                     be equal to GPIOx x= A, B, ...*/
  uint16_t      NSDPin;         /*!< GPIO pin used by NSD. It must be
                                     equal to GPIO_Pin_x x= 0, 1, ...*/
  uint32_t      GAP_ErrorsNow[MAX_DEVICES_NUMBER];
  /*!< Bitfield with extra error codes that is currently
       active. The error code avalilable
       are listed here (TBF).*/
  uint32_t      GAP_ErrorsOccurred[MAX_DEVICES_NUMBER];
  /*!< Bitfield with extra error codes that occurs and is
       over. The error code avalilable
       are listed here (TBF).*/
} GAP_Handle_t;


/* Exported functions ------------------------------------------------------- */

bool GAP_CheckErrors( GAP_Handle_t * pHandle, uint32_t * error_now, uint32_t * error_occurred );
void GAP_FaultAck( GAP_Handle_t * pHandle );
bool GAP_Configuration( GAP_Handle_t * pHandle );
bool GAP_IsDevicesProgrammed( GAP_Handle_t * pHandle );
bool GAP_DevicesConfiguration( GAP_Handle_t * pHandle );
uint16_t GAP_CRCCalculate( uint8_t data, uint8_t crc_initial_value );
bool GAP_CRCCheck( uint8_t * out, uint16_t data_in );
void wait( uint16_t count );
uint8_t GAP_RegMask( GAP_Registers_Handle_t reg );
bool GAP_ReadRegs( GAP_Handle_t * pHandle, uint8_t * pDataRead, GAP_Registers_Handle_t reg );
void GAP_StartConfig( GAP_Handle_t * pHandle );
void GAP_StopConfig( GAP_Handle_t * pHandle );
bool GAP_WriteRegs( GAP_Handle_t * pHandle, uint8_t * pDataWrite, GAP_Registers_Handle_t reg );
void GAP_GlobalReset( GAP_Handle_t * pHandle );
bool GAP_ResetStatus( GAP_Handle_t * pHandle, GAP_Registers_Handle_t reg );
bool GAP_Test( GAP_Handle_t * pHandle, GAP_TestMode_t testMode );

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif /* __cpluplus */

#endif /* __GAP_GATE_DRIVER_CTR_H */

/************************ (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/

