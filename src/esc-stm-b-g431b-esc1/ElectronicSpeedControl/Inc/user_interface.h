/**
  ******************************************************************************
  * @file    user_interface.h
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file contains all definitions and functions prototypes for the
  *          user interface component of the Motor Control SDK.
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
  * @ingroup MCUI
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USERINTERFACE_H
#define __USERINTERFACE_H

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "mc_type.h"
#include "mc_interface.h"
#include "mc_tuning.h"
#include "state_machine.h"
#include "stm32g4xx_hal.h"

/** @addtogroup MCSDK
  * @{
  */

/** @addtogroup MCUI
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/**
 * @brief Motor control protocol registers
 *
 * Enumerates the variables and information that can be exchanged across the Motor Control Protocol
 */
typedef enum
{
  MC_PROTOCOL_REG_TARGET_MOTOR,          /* 0   */
  MC_PROTOCOL_REG_FLAGS,                 /* 1   */
  MC_PROTOCOL_REG_STATUS,                /* 2   */
  MC_PROTOCOL_REG_CONTROL_MODE,          /* 3   */
  MC_PROTOCOL_REG_SPEED_REF,             /* 4   */
  MC_PROTOCOL_REG_SPEED_KP,              /* 5   */
  MC_PROTOCOL_REG_SPEED_KI,              /* 6   */
  MC_PROTOCOL_REG_SPEED_KD,              /* 7   */
  MC_PROTOCOL_REG_TORQUE_REF,            /* 8   */
  MC_PROTOCOL_REG_TORQUE_KP,             /* 9   */
  MC_PROTOCOL_REG_TORQUE_KI,             /* 10  */
  MC_PROTOCOL_REG_TORQUE_KD,             /* 11  */
  MC_PROTOCOL_REG_FLUX_REF,              /* 12  */
  MC_PROTOCOL_REG_FLUX_KP,               /* 13  */
  MC_PROTOCOL_REG_FLUX_KI,               /* 14  */
  MC_PROTOCOL_REG_FLUX_KD,               /* 15  */
  MC_PROTOCOL_REG_OBSERVER_C1,           /* 16  */
  MC_PROTOCOL_REG_OBSERVER_C2,           /* 17  */
  MC_PROTOCOL_REG_OBSERVER_CR_C1,        /* 18  */
  MC_PROTOCOL_REG_OBSERVER_CR_C2,        /* 19  */
  MC_PROTOCOL_REG_PLL_KI,                /* 20  */
  MC_PROTOCOL_REG_PLL_KP,                /* 21  */
  MC_PROTOCOL_REG_FLUXWK_KP,             /* 22  */
  MC_PROTOCOL_REG_FLUXWK_KI,             /* 23  */
  MC_PROTOCOL_REG_FLUXWK_BUS,            /* 24  */
  MC_PROTOCOL_REG_BUS_VOLTAGE,           /* 25  */
  MC_PROTOCOL_REG_HEATS_TEMP,            /* 26  */
  MC_PROTOCOL_REG_MOTOR_POWER,           /* 27  */
  MC_PROTOCOL_REG_DAC_OUT1,              /* 28  */
  MC_PROTOCOL_REG_DAC_OUT2,              /* 29  */
  MC_PROTOCOL_REG_SPEED_MEAS,            /* 30  */
  MC_PROTOCOL_REG_TORQUE_MEAS,           /* 31  */
  MC_PROTOCOL_REG_FLUX_MEAS,             /* 32  */
  MC_PROTOCOL_REG_FLUXWK_BUS_MEAS,       /* 33  */
  MC_PROTOCOL_REG_RUC_STAGE_NBR,         /* 34  */
  MC_PROTOCOL_REG_I_A,                   /* 35  */
  MC_PROTOCOL_REG_I_B,                   /* 36  */
  MC_PROTOCOL_REG_I_ALPHA,               /* 37  */
  MC_PROTOCOL_REG_I_BETA,                /* 38  */
  MC_PROTOCOL_REG_I_Q,                   /* 39  */
  MC_PROTOCOL_REG_I_D,                   /* 40  */
  MC_PROTOCOL_REG_I_Q_REF,               /* 41  */
  MC_PROTOCOL_REG_I_D_REF,               /* 42  */
  MC_PROTOCOL_REG_V_Q,                   /* 43  */
  MC_PROTOCOL_REG_V_D,                   /* 44  */
  MC_PROTOCOL_REG_V_ALPHA,               /* 45  */
  MC_PROTOCOL_REG_V_BETA,                /* 46  */
  MC_PROTOCOL_REG_MEAS_EL_ANGLE,         /* 47  */
  MC_PROTOCOL_REG_MEAS_ROT_SPEED,        /* 48  */
  MC_PROTOCOL_REG_OBS_EL_ANGLE,          /* 49  */
  MC_PROTOCOL_REG_OBS_ROT_SPEED,         /* 50  */
  MC_PROTOCOL_REG_OBS_I_ALPHA,           /* 51  */
  MC_PROTOCOL_REG_OBS_I_BETA,            /* 52  */
  MC_PROTOCOL_REG_OBS_BEMF_ALPHA,        /* 53  */
  MC_PROTOCOL_REG_OBS_BEMF_BETA,         /* 54  */
  MC_PROTOCOL_REG_OBS_CR_EL_ANGLE,       /* 55  */
  MC_PROTOCOL_REG_OBS_CR_ROT_SPEED,      /* 56  */
  MC_PROTOCOL_REG_OBS_CR_I_ALPHA,        /* 57  */
  MC_PROTOCOL_REG_OBS_CR_I_BETA,         /* 58  */
  MC_PROTOCOL_REG_OBS_CR_BEMF_ALPHA,     /* 59  */
  MC_PROTOCOL_REG_OBS_CR_BEMF_BETA,      /* 60  */
  MC_PROTOCOL_REG_DAC_USER1,             /* 61  */
  MC_PROTOCOL_REG_DAC_USER2,             /* 62  */
  MC_PROTOCOL_REG_MAX_APP_SPEED,         /* 63  */
  MC_PROTOCOL_REG_MIN_APP_SPEED,         /* 64  */
  MC_PROTOCOL_REG_IQ_SPEEDMODE,          /* 65  */
  MC_PROTOCOL_REG_EST_BEMF_LEVEL,        /* 66  */
  MC_PROTOCOL_REG_OBS_BEMF_LEVEL,        /* 67  */
  MC_PROTOCOL_REG_EST_CR_BEMF_LEVEL,     /* 68  */
  MC_PROTOCOL_REG_OBS_CR_BEMF_LEVEL,     /* 69  */
  MC_PROTOCOL_REG_FF_1Q,                 /* 70  */
  MC_PROTOCOL_REG_FF_1D,                 /* 71  */
  MC_PROTOCOL_REG_FF_2,                  /* 72  */
  MC_PROTOCOL_REG_FF_VQ,                 /* 73  */
  MC_PROTOCOL_REG_FF_VD,                 /* 74  */
  MC_PROTOCOL_REG_FF_VQ_PIOUT,           /* 75  */
  MC_PROTOCOL_REG_FF_VD_PIOUT,           /* 76  */
  MC_PROTOCOL_REG_PFC_STATUS,            /* 77  */
  MC_PROTOCOL_REG_PFC_FAULTS,            /* 78  */
  MC_PROTOCOL_REG_PFC_DCBUS_REF,         /* 79  */
  MC_PROTOCOL_REG_PFC_DCBUS_MEAS,        /* 80  */
  MC_PROTOCOL_REG_PFC_ACBUS_FREQ,        /* 81  */
  MC_PROTOCOL_REG_PFC_ACBUS_RMS,         /* 82  */
  MC_PROTOCOL_REG_PFC_I_KP,              /* 83  */
  MC_PROTOCOL_REG_PFC_I_KI,              /* 84  */
  MC_PROTOCOL_REG_PFC_I_KD,              /* 85  */
  MC_PROTOCOL_REG_PFC_V_KP,              /* 86  */
  MC_PROTOCOL_REG_PFC_V_KI,              /* 87  */
  MC_PROTOCOL_REG_PFC_V_KD,              /* 88  */
  MC_PROTOCOL_REG_PFC_STARTUP_DURATION,  /* 89  */
  MC_PROTOCOL_REG_PFC_ENABLED,           /* 90  */
  MC_PROTOCOL_REG_RAMP_FINAL_SPEED,      /* 91  */
  MC_PROTOCOL_REG_RAMP_DURATION,         /* 92  */
  MC_PROTOCOL_REG_HFI_EL_ANGLE,          /* 93  */
  MC_PROTOCOL_REG_HFI_ROT_SPEED,         /* 94  */
  MC_PROTOCOL_REG_HFI_CURRENT,           /* 95  */
  MC_PROTOCOL_REG_HFI_INIT_ANG_PLL,      /* 96  */
  MC_PROTOCOL_REG_HFI_INIT_ANG_SAT_DIFF, /* 97  */
  MC_PROTOCOL_REG_HFI_PI_PLL_KP,         /* 98  */
  MC_PROTOCOL_REG_HFI_PI_PLL_KI,         /* 99  */
  MC_PROTOCOL_REG_HFI_PI_TRACK_KP,       /* 100 */
  MC_PROTOCOL_REG_HFI_PI_TRACK_KI,       /* 101 */
  MC_PROTOCOL_REG_SC_CHECK,              /* 102 */
  MC_PROTOCOL_REG_SC_STATE,              /* 103 */
  MC_PROTOCOL_REG_SC_RS,                 /* 104 */
  MC_PROTOCOL_REG_SC_LS,                 /* 105 */
  MC_PROTOCOL_REG_SC_KE,                 /* 106 */
  MC_PROTOCOL_REG_SC_VBUS,               /* 107 */
  MC_PROTOCOL_REG_SC_MEAS_NOMINALSPEED,  /* 108 */
  MC_PROTOCOL_REG_SC_STEPS,              /* 109 */
  MC_PROTOCOL_REG_SPEED_KP_DIV,          /* 110 */
  MC_PROTOCOL_REG_SPEED_KI_DIV,          /* 111 */
  MC_PROTOCOL_REG_UID,                   /* 112 */
  MC_PROTOCOL_REG_HWTYPE,                /* 113 */
  MC_PROTOCOL_REG_CTRBDID,               /* 114 */
  MC_PROTOCOL_REG_PWBDID,                /* 115 */
  MC_PROTOCOL_REG_SC_PP,                 /* 116 */
  MC_PROTOCOL_REG_SC_CURRENT,            /* 117 */
  MC_PROTOCOL_REG_SC_SPDBANDWIDTH,       /* 118 */
  MC_PROTOCOL_REG_SC_LDLQRATIO,          /* 119 */
  MC_PROTOCOL_REG_SC_NOMINAL_SPEED,      /* 120 */
  MC_PROTOCOL_REG_SC_CURRBANDWIDTH,      /* 121 */
  MC_PROTOCOL_REG_SC_J,                  /* 122 */
  MC_PROTOCOL_REG_SC_F,                  /* 123 */
  MC_PROTOCOL_REG_SC_MAX_CURRENT,        /* 124 */
  MC_PROTOCOL_REG_SC_STARTUP_SPEED,      /* 125 */
  MC_PROTOCOL_REG_SC_STARTUP_ACC,        /* 126 */
  MC_PROTOCOL_REG_SC_PWM_FREQUENCY,      /* 127 */
  MC_PROTOCOL_REG_SC_FOC_REP_RATE,       /* 128 */
  MC_PROTOCOL_REG_PWBDID2,               /* 129 */
  MC_PROTOCOL_REG_SC_COMPLETED,          /* 130 */
  MC_PROTOCOL_REG_CURRENT_POSITION,      /* 131 */
  MC_PROTOCOL_REG_TARGET_POSITION,       /* 132 */
  MC_PROTOCOL_REG_MOVE_DURATION,         /* 133 */
  MC_PROTOCOL_REG_POSITION_KP,           /* 134 */
  MC_PROTOCOL_REG_POSITION_KI,           /* 135 */
  MC_PROTOCOL_REG_POSITION_KD,           /* 136 */
  MC_PROTOCOL_REG_UNDEFINED
} MC_Protocol_REG_t;

/** @brief DAC channels */
typedef enum
{
  DAC_CH0,
  DAC_CH1,
  DAC_CH2,
  DAC_CH3,
  DAC_MAX_Channel_nbr
} DAC_Channel_t;

/** @brief DAC user channels */
typedef enum
{
  DAC_USER1,
  DAC_USER2
} DAC_UserChannel_t;

/* @brief To configure the UI use MAIN Sensor (4bit)|AUX Sensor (4 bit) as first byte of CFG.*/
#define UI_SCODE_NONE     0x0u
#define UI_SCODE_HALL     0x1u /*!< This code identifies the Hall sensor.*/
#define UI_SCODE_ENC      0x2u /*!< This code identifies the Encoder sensor.*/
#define UI_SCODE_STO_PLL  0x9u /*!< This code identifies the State observer + PLL sensor.*/
#define UI_SCODE_STO_CR   0xAu /*!< This code identifies the State observer + CORDIC sensor.*/
#define UI_SCODE_HFINJ    0xBu /*!< This code identifies the HF injection sensor.*/

#define UI_CFGOPT_NONE            0x00000000u /*!< Enable this option when no other
                                              option is selected.*/
#define UI_CFGOPT_FW              0x00000001u /*!< Enable this option when the flux
                                              weakening is enabled in the MC
                                              firmware.*/
#define UI_CFGOPT_SPEED_KD        0x00000002u /*!< Enable this option when the speed
                                              controller has derivative action.
                                              */
#define UI_CFGOPT_Iq_KD           0x00000004u /*!< Enable this option when the Iq
                                              controller has derivative action.
                                              */
#define UI_CFGOPT_Id_KD           0x00000008u /*!< Enable this option when the Id
                                              controller has derivative action.
                                              */
#define UI_CFGOPT_DAC             0x00000010u /*!< Enable this option if a DAC object
                                              will be associated with the UI.*/
#define UI_CFGOPT_SETIDINSPDMODE  0x00000020u /*!< Enable this option to allow setting
                                              the Id reference when MC is in
                                              speed mode.*/
#define UI_CFGOPT_PLLTUNING       0x00000040u /*!< Enable this option to allow setting
                                              the PLL KP and KI.*/
#define UI_CFGOPT_PFC             0x00000080u /*!< Enable this option to allow PFC tuning.*/

#define UI_CFGOPT_PFC_I_KD        0x00000100u /*!< Enable this option when PFC current
                                              controller has derivative action.*/
#define UI_CFGOPT_PFC_V_KD        0x00000200u /*!< Enable this option when PFC voltage
                                              controller has derivative action.*/

#define MC_PROTOCOL_CODE_SET_REG          0x01
#define MC_PROTOCOL_CODE_GET_REG          0x02
#define MC_PROTOCOL_CODE_EXECUTE_CMD      0x03
#define MC_PROTOCOL_CODE_STORE_TOADDR     0x04
#define MC_PROTOCOL_CODE_LOAD_FROMADDR    0x05
#define MC_PROTOCOL_CODE_GET_BOARD_INFO   0x06
#define MC_PROTOCOL_CODE_SET_SPEED_RAMP   0x07
#define MC_PROTOCOL_CODE_GET_REVUP_DATA   0x08
#define MC_PROTOCOL_CODE_SET_REVUP_DATA   0x09
#define MC_PROTOCOL_CODE_SET_CURRENT_REF  0x0A
#define MC_PROTOCOL_CODE_GET_MP_INFO      0x0B
#define MC_PROTOCOL_CODE_GET_FW_VERSION   0x0C
#define MC_PROTOCOL_CODE_SET_TORQUE_RAMP  0x0D
#define MC_PROTOCOL_CODE_SET_POSITION_CMD 0x12

#define MC_PROTOCOL_CMD_START_MOTOR       0x01
#define MC_PROTOCOL_CMD_STOP_MOTOR        0x02
#define MC_PROTOCOL_CMD_STOP_RAMP         0x03
#define MC_PROTOCOL_CMD_RESET             0x04
#define MC_PROTOCOL_CMD_PING              0x05
#define MC_PROTOCOL_CMD_START_STOP        0x06
#define MC_PROTOCOL_CMD_FAULT_ACK         0x07
#define MC_PROTOCOL_CMD_ENCODER_ALIGN     0x08
#define MC_PROTOCOL_CMD_IQDREF_CLEAR      0x09
#define MC_PROTOCOL_CMD_PFC_ENABLE        0x0A
#define MC_PROTOCOL_CMD_PFC_DISABLE       0x0B
#define MC_PROTOCOL_CMD_PFC_FAULT_ACK     0x0C
#define MC_PROTOCOL_CMD_SC_START          0x0D
#define MC_PROTOCOL_CMD_SC_STOP           0x0E

#define CTRBDID 10015
#define PWBDID 10015
#define MC_UID 883328122

/**
  * @brief  UserInterface class parameters definition
  */
typedef const void UserInterfaceParams_t, *pUserInterfaceParams_t;

/**
  * @brief This structure is used to handle an instance of the UI component
  *
  */
typedef struct UI_Handle UI_Handle_t;

/**
  * @brief Polymorphic function. The function called can change in run-time and
  *        is assigned by the library to one of the callback pointers
  *        pFctSwitchOffPwm
  *        pFctSwitchOnPwm
  *        pFctCurrReadingCalib
  *        pFctTurnOnLowSides
  *        pFctRLDetectionModeEnable
  *        pFctRLDetectionModeDisable
  *
  *
  */
typedef void (*UI_Generic_Cb_t)( UI_Handle_t *pHandle);

/**
  * @brief Polymorphic function. The function called can change in run-time and
  *        is assigned by the library to the callback pointer pFctIrqHandler
  *
  */
typedef void* (*UI_IrqHandler_Cb_t)( void *pHandle, unsigned char flag, unsigned short rx_data);

/**
  * @brief Polymorphic function. The function called can change in run-time and
  *        is assigned by the library to the callback pointer pFctDACSetChannelConfig
  *
  */
typedef void (*UI_DACSetChannelConfig_Cb_t)( UI_Handle_t *pHandle, DAC_Channel_t bChannel, MC_Protocol_REG_t bVariable);

/**
  * @brief Polymorphic function. The function called can change in run-time and
  *        is assigned by the library to the callback pointer pFctDACGetChannelConfig
  *
  */
typedef MC_Protocol_REG_t (*UI_DACGetChannelConfig_Cb_t)( UI_Handle_t *pHandle, DAC_Channel_t bChannel);

/**
  * @brief Polymorphic function. The function called can change in run-time and
  *        is assigned by the library to the callback pointer pFctDACSetUserChannelValue
  *
  */
typedef void (*UI_DACSetUserChannelValue_Cb_t)( UI_Handle_t *pHandle, uint8_t bUserChNumber, int16_t hValue);

/**
  * @brief Polymorphic function. The function called can change in run-time and
  *        is assigned by the library to the callback pointer pFctDACGetUserChannelValue
  *
  */
typedef int16_t (*UI_DACGetUserChannelValue_Cb_t)( UI_Handle_t *pHandle, uint8_t bUserChNumber);

/**
  * @brief UI_Handle structure used for User Interface
  *
  */
struct UI_Handle
{

  UI_IrqHandler_Cb_t pFctIrqHandler;

  /* DAC related functions */
  UI_DACSetChannelConfig_Cb_t pFctDACSetChannelConfig;
  UI_DACGetChannelConfig_Cb_t pFctDACGetChannelConfig;
  UI_DACSetUserChannelValue_Cb_t pFctDACSetUserChannelValue;
  UI_DACGetUserChannelValue_Cb_t pFctDACGetUserChannelValue;

  UI_Generic_Cb_t pFct_DACInit;
  UI_Generic_Cb_t pFct_DACExec;
  uint8_t bDriveNum;      /*!< Total number of MC objects.*/
  MCI_Handle_t** pMCI;             /*!< Pointer of MC interface list.*/
  MCT_Handle_t** pMCT;             /*!< Pointer of MC tuning list.*/
  uint32_t* pUICfg;       /*!< Pointer of UI configuration list.*/
  uint8_t bSelectedDrive; /*!< Current selected MC object in the list.*/
};

/**
  * @brief  Initialization of UI object. It perform the link between the UI
  *         object and the MC interface and MC tuning objects. It must be called
  *         before the derived class initialization.
  */
void UI_Init(UI_Handle_t *pHandle, uint8_t bMCNum, MCI_Handle_t** pMCI, MCT_Handle_t** pMCT, uint32_t* pUICfg);

/**
  * @brief  It is used to select the MC on which UI operates.
  */
bool UI_SelectMC(UI_Handle_t *pHandle,uint8_t bSelectMC);

/**
  * @brief  It is used to retrive the MC on which UI currently operates.
  * @param  pHandle pointer on the target component handle.
  * @retval uint8_t It returns the currently selected MC, zero based, on which
  *         UI operates.
  */
uint8_t UI_GetSelectedMC(UI_Handle_t *pHandle);

/**
  * @brief  It is used to retrive the configuration of the MC on which UI
  *         currently operates.
  * @param  pHandle pointer on the target component handle.
  * @retval uint32_t It returns the currently configuration of selected MC on
  *         which UI operates.
  *         It represents a bit field containing one (or more) of
  *         the exported configuration option UI_CFGOPT_xxx (eventually OR-ed).
  */
uint32_t UI_GetSelectedMCConfig(UI_Handle_t *pHandle);

/**
  * @brief  It is used to execute a SetReg command coming from the user.
  * @param  pHandle pointer on the target component handle.
  * @param  bRegID Code of register to be updated. Valid code is one of the
  *         MC_PROTOCOL_REG_xxx values exported by UserInterfaceClass.
  * @param  wValue is the new value to be set.
  * @retval bool It returns true if the SetReg command has been performed
  *         succesfully otherwise returns false.
  */
bool UI_SetReg(UI_Handle_t *pHandle, MC_Protocol_REG_t bRegID, int32_t wValue);

/**
  * @brief  It is used to execute a GetReg command coming from the user.
  * @param  pHandle pointer on the target component handle.
  * @param  bRegID Code of register to be updated. Valid code is one of the
  *         MC_PROTOCOL_REG_xxx values exported by UserInterfaceClass.
  * @param  success if not NULL, the function sets success to true if tregister 
  *         @p bRegID is known bby the application and false otherwise. 
  *           
  * @retval int32_t is the current value of register bRegID.
  *
  * If @p success is set to false by the function, then the return value is meaningless.
  */
int32_t UI_GetReg(UI_Handle_t *pHandle, MC_Protocol_REG_t bRegID, bool * success );

/**
  * @brief  It is used to execute a command coming from the user.
  * @param  pHandle pointer on the target component handle.
  * @param  bCmdID Code of register to be updated. Valid code is one of the
  *         MC_PROTOCOL_CMD_xxx define exported by UserInterfaceClass.
  * @retval bool It returns true if the command has been performed
  *         succesfully otherwise returns false.
  */
bool UI_ExecCmd(UI_Handle_t *pHandle, uint8_t bCmdID);

/* Executes a speed ramp command coming from the user.*/
bool UI_ExecSpeedRamp(UI_Handle_t *pHandle, int32_t wFinalMecSpeedUnit, uint16_t hDurationms);

/**
  * @brief  It is used to execute a torque ramp command coming from the user.
  * @param  pHandle pointer on the target component handle.
  * @param  hTargetFinal final torque value. See MCI interface for more
            details.
  * @param  hDurationms the duration of the ramp expressed in milliseconds. It
  *         is possible to set 0 to perform an instantaneous change in the value.
  * @retval bool It returns true if the command has been performed
  *         succesfully otherwise returns false.
  */
bool UI_ExecTorqueRamp(UI_Handle_t *pHandle, int16_t hTargetFinal, uint16_t hDurationms);

/* Executes a get Revup data command coming from the user */
bool UI_GetRevupData(UI_Handle_t *pHandle, uint8_t bStage, uint16_t* pDurationms,
                     int16_t* pFinalMecSpeedUnit, int16_t* pFinalTorque );

/** Executes a set Revup data command coming from the user. */
bool UI_SetRevupData(UI_Handle_t *pHandle, uint8_t bStage, uint16_t hDurationms,
                     int16_t hFinalMecSpeedUnit, int16_t hFinalTorque );

/**
  * @brief  It is used to execute a set current reference command coming from
  *         the user.
  * @param  pHandle pointer on the target component handle.
  * @param  hIqRef is the current Iq reference on qd reference frame. This value
  *         is expressed in digit. To convert current expressed in digit to
  *         current expressed in Amps is possible to use the formula:
  *         Current(Amp) = [Current(digit) * Vdd micro] / [65536 * Rshunt * Aop]
  * @param  hIdRef is the current Id reference on qd reference frame. This value
  *         is expressed in digit. See hIqRef param description.
  * @retval none.
  */
void UI_SetCurrentReferences(UI_Handle_t *pHandle, int16_t hIqRef, int16_t hIdRef);

/**
  * @brief  Function to get information about MP registers available for each
  *         step. PC send to the FW the list of steps to get the available
  *         registers. The FW returs the list of available registers for that
  *         steps.
  * @param  stepList List of requested steps.
  * @param  pMPInfo The returned list of register.
  *         It is populated by this function.
  * @retval true if MP is enabled, false otherwise.
  */
bool UI_GetMPInfo(pMPInfo_t stepList, pMPInfo_t MPInfo);

/**
  * @brief  Hardware and software initialization of the DAC object. This is a
  *         virtual function and is implemented by related object.
  * @param  pHandle pointer on the target component handle. It must be a DACx_UI object casted
  *         to CUI otherwise the DACInit method will have no effect.
  * @retval none.
  */
void UI_DACInit(UI_Handle_t *pHandle);

/**
  * @brief  This method is used to update the DAC outputs. The selected
  *         variables will be provided in the related output channels. This is a
  *         virtual function and is implemented by related object.
  * @param  pHandle pointer on the target component handle. It must be a DACx_UI object casted
  *         to CUI otherwise the DACInit method will have no effect.
  * @retval none.
  */
void UI_DACExec(UI_Handle_t *pHandle);

/**
  * @brief  This method is used to set up the DAC outputs. The selected
  *         variables will be provided in the related output channels after next
  *         DACExec. This is a virtual function and is implemented by related
  *         object.
  * @param  pHandle pointer on the target component handle. It must be a DACx_UI object casted
  *         to CUI otherwise the DACInit method will have no effect.
  * @param  bChannel the DAC channel to be programmed. It must be one of the
  *         exported channels Ex. DAC_CH0.
  * @param  bVariable the variables to be provided in out through the selected
  *         channel. It must be one of the exported UI register Ex.
  *         MC_PROTOCOL_REG_I_A.
  * @retval none.
  */
void UI_SetDAC(UI_Handle_t *pHandle, DAC_Channel_t bChannel,
                         MC_Protocol_REG_t bVariable);

/**
  * @brief  This method is used to get the current DAC channel selected output.
  * @param  pHandle pointer on the target component handle. It must be a DACx_UI object casted
  *         to CUI otherwise the method will have no effect.
  * @param  bChannel the inspected DAC channel. It must be one of the
  *         exported channels (Ex. DAC_CH0).
  * @retval MC_Protocol_REG_t The variables provided in out through the inspected
  *         channel. It will be one of the exported UI register (Ex.
  *         MC_PROTOCOL_REG_I_A).
  */
MC_Protocol_REG_t UI_GetDAC(UI_Handle_t *pHandle, DAC_Channel_t bChannel);

/**
  * @brief  This method is used to set the value of the "User DAC channel".
  * @param  pHandle pointer on the target component handle. It must be a DACx_UI object casted
  *         to CUI otherwise the DACInit method will have no effect.
  * @param  bUserChNumber the "User DAC channel" to be programmed.
  * @param  hValue the value to be put in output.
  * @retval none.
  */
void UI_SetUserDAC(UI_Handle_t *pHandle, DAC_UserChannel_t bUserChNumber, int16_t hValue);

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif /* __cpluplus */

#endif /*__USERINTERFACE_H*/

/************************ (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
