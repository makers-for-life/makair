/**
  ******************************************************************************
  * @file    user_interface.c
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file provides firmware functions that implement the following features
  *          of the user interface component of the Motor Control SDK.
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

/* Includes ------------------------------------------------------------------*/
/* Pre-compiler coherency check */
#include "mc_type.h"
#include "mc_math.h"
#include "mc_config.h"
#include "user_interface.h"

/** @addtogroup MCSDK
  * @{
  */

/** @defgroup MCUI Motor Control User Interface
  * @brief User Interface Components of the Motor Control SDK
  *
  * These components aim at connecting the Application with the outside. There are two categories
  * of UI Componentes:
  *
  * - Some connect the application with the Motor Conrol Monitor tool via a UART link. The Motor
  *   Control Monitor can control the motor(s) driven by the application and also read and write  
  *   internal variables of the Motor Control subsystem. 
  * - Others UI components allow for using the DAC(s) peripherals in 
  *   order to output internal variables of the Motor Control subsystem for debug purposes.
  *
  * @{
  */

/**
  * @brief  Initialize the user interface component. 
  *
  * Perform the link between the UI, MC interface and MC tuning components.

  * @param  pHandle: Pointer on Handle structure of UI component.
  * @param  bMCNum  Number of MC instance present in the list.
  * @param  pMCI Pointer on the list of MC interface component to inked with UI.
  * @param  pMCT Pointer on the list of MC tuning component to inked with UI.
  * @param  pUICfg Pointer on the user interface configuration list. 
  *         Each element of the list must be a bit field containing one (or more) of
  *         the exported configuration option UI_CFGOPT_xxx (eventually OR-ed).
  * @retval none.
  */
__weak void UI_Init(UI_Handle_t *pHandle, uint8_t bMCNum, MCI_Handle_t ** pMCI, MCT_Handle_t** pMCT, uint32_t* pUICfg)
{
  pHandle->bDriveNum = bMCNum;
  pHandle->pMCI = pMCI;
  pHandle->pMCT = pMCT;
  pHandle->bSelectedDrive = 0u;
  pHandle->pUICfg = pUICfg;
}

/**
  * @brief  Allow to select the MC on which UI operates.
  * @param  pHandle: Pointer on Handle structure of UI component.
  * @param  bSelectMC: The new selected MC, zero based, on which UI operates.
  * @retval Boolean set to true if the bSelectMC is valid oterwise return false.
  */
__weak bool UI_SelectMC(UI_Handle_t *pHandle,uint8_t bSelectMC)
{
  bool retVal = true;
  if (bSelectMC  >= pHandle->bDriveNum)
  {
    retVal = false;
  }
  else
  {
    pHandle->bSelectedDrive = bSelectMC;
  }
  return retVal;
}

/**
  * @brief  Allow to retrieve the MC on which UI currently operates.
  * @param  pHandle: Pointer on Handle structure of UI component.
  * @retval Return the currently selected MC, zero based, on which UI operates.
  */
__weak uint8_t UI_GetSelectedMC(UI_Handle_t *pHandle)
{
  return (pHandle->bSelectedDrive);
}

/**
  * @brief  Retrieve the configuration of the MC on which UI currently operates.
  * @param  pHandle: Pointer on Handle structure of UI component.
  * @retval Return the currently configuration of selected MC on which UI operates.
  *         It represents a bit field containing one (or more) of
  *         the exported configuration option UI_CFGOPT_xxx (eventually OR-ed).
  */
__weak uint32_t UI_GetSelectedMCConfig(UI_Handle_t *pHandle)
{
  return pHandle->pUICfg[pHandle->bSelectedDrive];
}

/**
  * @brief  Allow to execute a SetReg command coming from the user.
  * @param  pHandle: Pointer on Handle structure of UI component.
  * @param  bRegID: Code of register to update.
  *         See MC_PROTOCOL_REG_xxx for code definition.
  * @param  wValue: New value to set.
  * @retval Return the currently selected MC, zero based, on which UI operates.
  */
__weak bool UI_SetReg(UI_Handle_t *pHandle, MC_Protocol_REG_t bRegID, int32_t wValue)
{
  MCI_Handle_t * pMCI = pHandle->pMCI[pHandle->bSelectedDrive];
  MCT_Handle_t * pMCT = pHandle->pMCT[pHandle->bSelectedDrive];

  bool retVal = true;
  switch (bRegID)
  {
  case MC_PROTOCOL_REG_TARGET_MOTOR:
    {
      retVal = UI_SelectMC(pHandle,(uint8_t)wValue);
    }
    break;

  case MC_PROTOCOL_REG_RAMP_FINAL_SPEED:
    {
      MCI_ExecSpeedRamp(pMCI,(int16_t)((wValue*SPEED_UNIT)/_RPM),0);
    }
    break;

  case MC_PROTOCOL_REG_SPEED_KP:
    {
      PID_SetKP(pMCT->pPIDSpeed,(int16_t)wValue);
    }
    break;

  case MC_PROTOCOL_REG_SPEED_KI:
    {
      PID_SetKI(pMCT->pPIDSpeed,(int16_t)wValue);
    }
    break;

  case MC_PROTOCOL_REG_SPEED_KD:
    {
      PID_SetKD(pMCT->pPIDSpeed,(int16_t)wValue);
    }
    break;

  case MC_PROTOCOL_REG_CONTROL_MODE:
    {
      if ((STC_Modality_t)wValue == STC_TORQUE_MODE)
      {
        MCI_ExecTorqueRamp(pMCI, MCI_GetTeref(pMCI),0);
      }
      if ((STC_Modality_t)wValue == STC_SPEED_MODE)
      {
        MCI_ExecSpeedRamp(pMCI, MCI_GetMecSpeedRefUnit(pMCI),0);
      }
    }
    break;

  case MC_PROTOCOL_REG_TORQUE_REF:
    {
      qd_t currComp;
      currComp = MCI_GetIqdref(pMCI);
      currComp.q = (int16_t)wValue;
      MCI_SetCurrentReferences(pMCI,currComp);
    }
    break;

  case MC_PROTOCOL_REG_TORQUE_KP:
    {
      PID_SetKP(pMCT->pPIDIq,(int16_t)wValue);
    }
    break;

  case MC_PROTOCOL_REG_TORQUE_KI:
    {
      PID_SetKI(pMCT->pPIDIq,(int16_t)wValue);
    }
    break;

  case MC_PROTOCOL_REG_TORQUE_KD:
    {
      PID_SetKD(pMCT->pPIDIq,(int16_t)wValue);
    }
    break;

  case MC_PROTOCOL_REG_FLUX_REF:
    {
      qd_t currComp;
      currComp = MCI_GetIqdref(pMCI);
      currComp.d = (int16_t)wValue;
      MCI_SetCurrentReferences(pMCI,currComp);
    }
    break;

  case MC_PROTOCOL_REG_FLUX_KP:
    {
      PID_SetKP(pMCT->pPIDId,(int16_t)wValue);
    }
    break;

  case MC_PROTOCOL_REG_FLUX_KI:
    {
      PID_SetKI(pMCT->pPIDId,(int16_t)wValue);
    }
    break;

  case MC_PROTOCOL_REG_FLUX_KD:
    {
      PID_SetKD(pMCT->pPIDId,(int16_t)wValue);
    }
    break;

  case MC_PROTOCOL_REG_OBSERVER_C1:
    {
      uint32_t hUICfg = pHandle->pUICfg[pHandle->bSelectedDrive];
      SpeednPosFdbk_Handle_t* pSPD = MC_NULL;
      int16_t hC1,hC2;
      if (MAIN_SCFG_VALUE(hUICfg) == UI_SCODE_STO_PLL)
      {
        pSPD = pMCT->pSpeedSensorMain;
      }
      if (AUX_SCFG_VALUE(hUICfg) == UI_SCODE_STO_PLL)
      {
        pSPD = pMCT->pSpeedSensorAux;
      }
      if (pSPD != MC_NULL)
      {
        STO_PLL_GetObserverGains((STO_PLL_Handle_t*)pSPD,&hC1,&hC2);
        STO_PLL_SetObserverGains((STO_PLL_Handle_t*)pSPD,(int16_t)wValue,hC2);
      }
    }
    break;

  case MC_PROTOCOL_REG_OBSERVER_C2:
    {
      uint32_t hUICfg = pHandle->pUICfg[pHandle->bSelectedDrive];
      SpeednPosFdbk_Handle_t* pSPD = MC_NULL;
      int16_t hC1,hC2;
      if (MAIN_SCFG_VALUE(hUICfg) == UI_SCODE_STO_PLL)
      {
        pSPD = pMCT->pSpeedSensorMain;
      }
      if (AUX_SCFG_VALUE(hUICfg) == UI_SCODE_STO_PLL)
      {
        pSPD = pMCT->pSpeedSensorAux;
      }
      if (pSPD != MC_NULL)
      {
        STO_PLL_GetObserverGains((STO_PLL_Handle_t*)pSPD,&hC1,&hC2);
        STO_PLL_SetObserverGains((STO_PLL_Handle_t*)pSPD,hC1,(int16_t)wValue);
      }
    }
    break;

  case MC_PROTOCOL_REG_PLL_KI:
    {
      uint32_t hUICfg = pHandle->pUICfg[pHandle->bSelectedDrive];
      SpeednPosFdbk_Handle_t* pSPD = MC_NULL;
      int16_t hPgain, hIgain;
      if (MAIN_SCFG_VALUE(hUICfg) == UI_SCODE_STO_PLL)
      {
        pSPD = pMCT->pSpeedSensorMain;
      }
      if (AUX_SCFG_VALUE(hUICfg) == UI_SCODE_STO_PLL)
      {
        pSPD = pMCT->pSpeedSensorAux;
      }
      if (pSPD != MC_NULL)
      {
        STO_GetPLLGains((STO_PLL_Handle_t*)pSPD,&hPgain,&hIgain);
        STO_SetPLLGains((STO_PLL_Handle_t*)pSPD,hPgain,(int16_t)wValue);
      }
    }
    break;

  case MC_PROTOCOL_REG_PLL_KP:
	{
      uint32_t hUICfg = pHandle->pUICfg[pHandle->bSelectedDrive];
      SpeednPosFdbk_Handle_t* pSPD = MC_NULL;
      int16_t hPgain, hIgain;
      if (MAIN_SCFG_VALUE(hUICfg) == UI_SCODE_STO_PLL)
      {
        pSPD = pMCT->pSpeedSensorMain;
      }
      if (AUX_SCFG_VALUE(hUICfg) == UI_SCODE_STO_PLL)
      {
        pSPD = pMCT->pSpeedSensorAux;
      }
      if (pSPD != MC_NULL)
      {
        STO_GetPLLGains((STO_PLL_Handle_t*)pSPD,&hPgain,&hIgain);
        STO_SetPLLGains((STO_PLL_Handle_t*)pSPD,(int16_t)wValue,hIgain);
      }
    }
    break;

  case MC_PROTOCOL_REG_IQ_SPEEDMODE:
    {
      MCI_SetIdref(pMCI,(int16_t)wValue);
    }
    break;

  default:
    retVal = false;
    break;
  }

  return retVal;
}

/* Used to execute a GetReg command coming from the user. */
__weak int32_t UI_GetReg(UI_Handle_t *pHandle, MC_Protocol_REG_t bRegID, bool * success)
{
  MCT_Handle_t* pMCT = pHandle->pMCT[pHandle->bSelectedDrive];
  MCI_Handle_t * pMCI = pHandle->pMCI[pHandle->bSelectedDrive];

  int32_t bRetVal = 0;

  if ( success != (bool *) 0 ) 
  {
    *success = true;
  }

  switch (bRegID)
  {
    case MC_PROTOCOL_REG_TARGET_MOTOR:
    {
      bRetVal = (int32_t)UI_GetSelectedMC(pHandle);
    }
    break;

    case MC_PROTOCOL_REG_FLAGS:
    {
      bRetVal = (int32_t)STM_GetFaultState(pMCT->pStateMachine);
    }
	break;

    case MC_PROTOCOL_REG_STATUS:
    {
      bRetVal = (int32_t)STM_GetState(pMCT->pStateMachine);
    }
	break;

    case MC_PROTOCOL_REG_SPEED_REF:
    {
      bRetVal = (int32_t)((MCI_GetMecSpeedRefUnit(pMCI)*_RPM)/SPEED_UNIT);
    }
    break;

    case MC_PROTOCOL_REG_SPEED_KP:
    {
      bRetVal = (int32_t)PID_GetKP(pMCT->pPIDSpeed);
    }
    break;

    case MC_PROTOCOL_REG_SPEED_KI:
    {
      bRetVal = (int32_t)PID_GetKI(pMCT->pPIDSpeed);
    }
    break;

    case MC_PROTOCOL_REG_SPEED_KD:
    {
      bRetVal = (int32_t)PID_GetKD(pMCT->pPIDSpeed);
    }
    break;

    case MC_PROTOCOL_REG_BUS_VOLTAGE:
    {
      bRetVal = (int32_t)VBS_GetAvBusVoltage_V(pMCT->pBusVoltageSensor);
    }
    break;

    case MC_PROTOCOL_REG_HEATS_TEMP:
    {
      bRetVal = (int32_t)NTC_GetAvTemp_C(pMCT->pTemperatureSensor);
    }
    break;

    case MC_PROTOCOL_REG_SPEED_MEAS:
    {
      bRetVal = (int32_t)((MCI_GetAvrgMecSpeedUnit(pMCI) * _RPM)/SPEED_UNIT);
    }
    break;

    case MC_PROTOCOL_REG_UID:
    {
      bRetVal = (int32_t)(MC_UID);
    }
    break;

    case MC_PROTOCOL_REG_CTRBDID:
    {
      bRetVal = CTRBDID;
    }
    break;

    case MC_PROTOCOL_REG_PWBDID:
    {
      bRetVal = PWBDID;
    }
    break;

    case MC_PROTOCOL_REG_PWBDID2:
    {
      bRetVal = (uint32_t) 0;
    }
    break;

    case MC_PROTOCOL_REG_TORQUE_REF:
    {
      qd_t currComp;
      currComp = MCI_GetIqdref(pMCI);
      bRetVal = (int32_t)currComp.q;
    }
    break;

    case MC_PROTOCOL_REG_FLUX_REF:
    {
      qd_t currComp;
      currComp = MCI_GetIqdref(pMCI);
      bRetVal = (int32_t)currComp.d;
    }
    break;

    case MC_PROTOCOL_REG_CONTROL_MODE:
    {
      bRetVal = (int32_t)MCI_GetControlMode(pMCI);
    }
    break;

    case MC_PROTOCOL_REG_RAMP_FINAL_SPEED:
    {
      if (MCI_GetControlMode(pMCI) == STC_SPEED_MODE)
      {
      bRetVal = (int32_t)((MCI_GetLastRampFinalSpeed(pMCI) * _RPM)/SPEED_UNIT) ;
      }
      else
      {
      bRetVal = (int32_t)((MCI_GetMecSpeedRefUnit(pMCI) * _RPM)/SPEED_UNIT) ;
      }
    }
    break;

    case MC_PROTOCOL_REG_SPEED_KP_DIV:
    {
      bRetVal = (int32_t)PID_GetKPDivisor(pMCT->pPIDSpeed);
    }
    break;

    case MC_PROTOCOL_REG_SPEED_KI_DIV:
    {
      bRetVal = (int32_t)PID_GetKIDivisor(pMCT->pPIDSpeed);
    }
    break;

    case MC_PROTOCOL_REG_TORQUE_KP:
    {
      bRetVal = (int32_t)PID_GetKP(pMCT->pPIDIq);
    }
    break;

    case MC_PROTOCOL_REG_TORQUE_KI:
    {
      bRetVal = (int32_t)PID_GetKI(pMCT->pPIDIq);
    }
    break;

    case MC_PROTOCOL_REG_TORQUE_KD:
    {
      bRetVal = (int32_t)PID_GetKD(pMCT->pPIDIq);
    }
    break;

    case MC_PROTOCOL_REG_IQ_SPEEDMODE:
    {
      qd_t currComp;
      currComp = MCI_GetIqdref(pMCI);
      bRetVal = (int32_t)currComp.d;
    }
    break;

    case MC_PROTOCOL_REG_FLUX_KP:
    {
      bRetVal = (int32_t)PID_GetKP(pMCT->pPIDId);
    }
    break;

    case MC_PROTOCOL_REG_FLUX_KI:
    {
      bRetVal = (int32_t)PID_GetKI(pMCT->pPIDId);
    }
    break;

    case MC_PROTOCOL_REG_FLUX_KD:
    {
      bRetVal = (int32_t)PID_GetKD(pMCT->pPIDId);
    }
    break;

    case MC_PROTOCOL_REG_OBSERVER_C1:
    {
      uint32_t hUICfg = pHandle->pUICfg[pHandle->bSelectedDrive];
      SpeednPosFdbk_Handle_t* pSPD = MC_NULL;
      int16_t hC1,hC2;
      if (MAIN_SCFG_VALUE(hUICfg) == UI_SCODE_STO_PLL)
      {
        pSPD = pMCT->pSpeedSensorMain;
      }
      if (AUX_SCFG_VALUE(hUICfg) == UI_SCODE_STO_PLL)
      {
        pSPD = pMCT->pSpeedSensorAux;
      }
      if (pSPD != MC_NULL)
      {
        STO_PLL_GetObserverGains((STO_PLL_Handle_t*)pSPD,&hC1,&hC2);
      }
      bRetVal = (int32_t)hC1;
    }
    break;

    case MC_PROTOCOL_REG_OBSERVER_C2:
    {
      uint32_t hUICfg = pHandle->pUICfg[pHandle->bSelectedDrive];
      SpeednPosFdbk_Handle_t* pSPD = MC_NULL;
      int16_t hC1,hC2;
      if (MAIN_SCFG_VALUE(hUICfg) == UI_SCODE_STO_PLL)
      {
        pSPD = pMCT->pSpeedSensorMain;
      }
      if (AUX_SCFG_VALUE(hUICfg) == UI_SCODE_STO_PLL)
      {
        pSPD = pMCT->pSpeedSensorAux;
      }
      if (pSPD != MC_NULL)
      {
        STO_PLL_GetObserverGains((STO_PLL_Handle_t*)pSPD,&hC1,&hC2);
      }
      bRetVal = (int32_t)hC2;
    }
    break;

    case MC_PROTOCOL_REG_OBS_EL_ANGLE:
    {
      uint32_t hUICfg = pHandle->pUICfg[pHandle->bSelectedDrive];
      SpeednPosFdbk_Handle_t* pSPD = MC_NULL;
      if (MAIN_SCFG_VALUE(hUICfg) == UI_SCODE_STO_PLL)
      {
        pSPD = pMCT->pSpeedSensorMain;
      }
      if (AUX_SCFG_VALUE(hUICfg) == UI_SCODE_STO_PLL)
      {
        pSPD = pMCT->pSpeedSensorAux;
      }
      if (pSPD != MC_NULL)
      {
        bRetVal = SPD_GetElAngle(pSPD);
      }
    }
    break;

    case MC_PROTOCOL_REG_PLL_KP:
    {
      uint32_t hUICfg = pHandle->pUICfg[pHandle->bSelectedDrive];
      SpeednPosFdbk_Handle_t* pSPD = MC_NULL;
      int16_t hPgain, hIgain;
      if (MAIN_SCFG_VALUE(hUICfg) == UI_SCODE_STO_PLL)
      {
        pSPD = pMCT->pSpeedSensorMain;
      }
      if (AUX_SCFG_VALUE(hUICfg) == UI_SCODE_STO_PLL)
      {
        pSPD = pMCT->pSpeedSensorAux;
      }
      if (pSPD != MC_NULL)
      {
        STO_GetPLLGains((STO_PLL_Handle_t*)pSPD,&hPgain,&hIgain);
      }
      bRetVal = (int32_t)hPgain;
    }
    break;

    case MC_PROTOCOL_REG_PLL_KI:
    {
      uint32_t hUICfg = pHandle->pUICfg[pHandle->bSelectedDrive];
      SpeednPosFdbk_Handle_t* pSPD = MC_NULL;
      int16_t hPgain, hIgain;
      if (MAIN_SCFG_VALUE(hUICfg) == UI_SCODE_STO_PLL)
      {
        pSPD = pMCT->pSpeedSensorMain;
      }
      if (AUX_SCFG_VALUE(hUICfg) == UI_SCODE_STO_PLL)
      {
        pSPD = pMCT->pSpeedSensorAux;
      }
      if (pSPD != MC_NULL)
      {
        STO_GetPLLGains((STO_PLL_Handle_t*)pSPD,&hPgain,&hIgain);
      }
      bRetVal = (int32_t)hIgain;
    }
    break;

    case MC_PROTOCOL_REG_OBS_ROT_SPEED:
    {
      uint32_t hUICfg = pHandle->pUICfg[pHandle->bSelectedDrive];
      SpeednPosFdbk_Handle_t* pSPD = MC_NULL;
      if (MAIN_SCFG_VALUE(hUICfg) == UI_SCODE_STO_PLL)
      {
        pSPD = pMCT->pSpeedSensorMain;
      }
      if (AUX_SCFG_VALUE(hUICfg) == UI_SCODE_STO_PLL)
      {
        pSPD = pMCT->pSpeedSensorAux;
      }
      if (pSPD != MC_NULL)
      {
        bRetVal = SPD_GetS16Speed(pSPD);
      }
    }
    break;

    case MC_PROTOCOL_REG_OBS_I_ALPHA:
    {
      uint32_t hUICfg = pHandle->pUICfg[pHandle->bSelectedDrive];
      SpeednPosFdbk_Handle_t* pSPD = MC_NULL;
      if (MAIN_SCFG_VALUE(hUICfg) == UI_SCODE_STO_PLL)
      {
        pSPD = pMCT->pSpeedSensorMain;
      }
      if (AUX_SCFG_VALUE(hUICfg) == UI_SCODE_STO_PLL)
      {
        pSPD = pMCT->pSpeedSensorAux;
      }
      if (pSPD != MC_NULL)
      {
        bRetVal = STO_PLL_GetEstimatedCurrent((STO_PLL_Handle_t*)pSPD).alpha;
      }
    }
    break;

    case MC_PROTOCOL_REG_OBS_I_BETA:
    {
      uint32_t hUICfg = pHandle->pUICfg[pHandle->bSelectedDrive];
      SpeednPosFdbk_Handle_t* pSPD = MC_NULL;
      if (MAIN_SCFG_VALUE(hUICfg) == UI_SCODE_STO_PLL)
      {
        pSPD = pMCT->pSpeedSensorMain;
      }
      if (AUX_SCFG_VALUE(hUICfg) == UI_SCODE_STO_PLL)
      {
        pSPD = pMCT->pSpeedSensorAux;
      }
      if (pSPD != MC_NULL)
      {
        bRetVal = STO_PLL_GetEstimatedCurrent((STO_PLL_Handle_t*)pSPD).beta;
      }
    }
    break;

    case MC_PROTOCOL_REG_OBS_BEMF_ALPHA:
    {
      uint32_t hUICfg = pHandle->pUICfg[pHandle->bSelectedDrive];
      SpeednPosFdbk_Handle_t* pSPD = MC_NULL;
      if (MAIN_SCFG_VALUE(hUICfg) == UI_SCODE_STO_PLL)
      {
        pSPD = pMCT->pSpeedSensorMain;
      }
      if (AUX_SCFG_VALUE(hUICfg) == UI_SCODE_STO_PLL)
      {
        pSPD =  pMCT->pSpeedSensorAux;
      }
      if (pSPD != MC_NULL)
      {
        bRetVal = STO_PLL_GetEstimatedBemf((STO_PLL_Handle_t*)pSPD).alpha;
      }
    }
    break;

    case MC_PROTOCOL_REG_OBS_BEMF_BETA:
    {
      uint32_t hUICfg = pHandle->pUICfg[pHandle->bSelectedDrive];
      SpeednPosFdbk_Handle_t* pSPD = MC_NULL;
      if (MAIN_SCFG_VALUE(hUICfg) == UI_SCODE_STO_PLL)
      {
       pSPD = pMCT->pSpeedSensorMain;
      }
      if (AUX_SCFG_VALUE(hUICfg) == UI_SCODE_STO_PLL)
      {
        pSPD = pMCT->pSpeedSensorAux;
      }
      if (pSPD != MC_NULL)
      {
        bRetVal = STO_PLL_GetEstimatedBemf((STO_PLL_Handle_t*)pSPD).beta;
      }
    }
    break;

    case MC_PROTOCOL_REG_EST_BEMF_LEVEL:
    {
      uint32_t hUICfg = pHandle->pUICfg[pHandle->bSelectedDrive];
      SpeednPosFdbk_Handle_t* pSPD = MC_NULL;
      if (MAIN_SCFG_VALUE(hUICfg) == UI_SCODE_STO_PLL)
      {
        pSPD = pMCT->pSpeedSensorMain;
      }
      if (AUX_SCFG_VALUE(hUICfg) == UI_SCODE_STO_PLL)
      {
        pSPD = pMCT->pSpeedSensorAux;
      }
      if (pSPD != MC_NULL)
      {
        bRetVal = STO_PLL_GetEstimatedBemfLevel((STO_PLL_Handle_t*)pSPD) >> 16;
      }
    }
    break;

    case MC_PROTOCOL_REG_OBS_BEMF_LEVEL:
    {
      uint32_t hUICfg = pHandle->pUICfg[pHandle->bSelectedDrive];
      SpeednPosFdbk_Handle_t* pSPD = MC_NULL;
      if (MAIN_SCFG_VALUE(hUICfg) == UI_SCODE_STO_PLL)
      {
        pSPD = pMCT->pSpeedSensorMain;
      }
      if (AUX_SCFG_VALUE(hUICfg) == UI_SCODE_STO_PLL)
      {
        pSPD = pMCT->pSpeedSensorAux;
      }
      if (pSPD != MC_NULL)
      {
        bRetVal = STO_PLL_GetObservedBemfLevel((STO_PLL_Handle_t*)pSPD) >> 16;
      }
    }
    break;

    case MC_PROTOCOL_REG_MOTOR_POWER:
    {
      bRetVal = MPM_GetAvrgElMotorPowerW(pMCT->pMPM);
    }
    break;

    case MC_PROTOCOL_REG_DAC_OUT1:
    {
      MC_Protocol_REG_t value = UI_GetDAC(pHandle, DAC_CH0);
      bRetVal = value;
    }
    break;

    case MC_PROTOCOL_REG_DAC_OUT2:
    {
      MC_Protocol_REG_t value = UI_GetDAC(pHandle, DAC_CH1);
      bRetVal = value;
    }
    break;

    case MC_PROTOCOL_REG_DAC_USER1:
    {
      if (pHandle->pFctDACGetUserChannelValue)
      {
        bRetVal = (int32_t) pHandle->pFctDACGetUserChannelValue(pHandle, 0);
      }
      else
      {
        bRetVal = (uint32_t) 0;
      }
    }
    break;

    case MC_PROTOCOL_REG_DAC_USER2:
    {
      if (pHandle->pFctDACGetUserChannelValue)
      {
        bRetVal = (int32_t) pHandle->pFctDACGetUserChannelValue(pHandle, 1);
      }
      else
      {
        bRetVal = (uint32_t) 0;
      }
    }
    break;

    case MC_PROTOCOL_REG_MAX_APP_SPEED:
    {
      bRetVal = (STC_GetMaxAppPositiveMecSpeedUnit(pMCT->pSpeednTorqueCtrl) * _RPM)/SPEED_UNIT ;
    }
    break;

    case MC_PROTOCOL_REG_MIN_APP_SPEED:
    {
      bRetVal = (STC_GetMinAppNegativeMecSpeedUnit(pMCT->pSpeednTorqueCtrl)  * _RPM)/SPEED_UNIT ;
    }
    break;

    case MC_PROTOCOL_REG_TORQUE_MEAS:
    case MC_PROTOCOL_REG_I_Q:
    {
      bRetVal = MCI_GetIqd(pMCI).q;
    }
    break;

    case MC_PROTOCOL_REG_FLUX_MEAS:
    case MC_PROTOCOL_REG_I_D:
    {
      bRetVal = MCI_GetIqd(pMCI).d;
    }
    break;

    case MC_PROTOCOL_REG_RUC_STAGE_NBR:
    {
      if (pMCT->pRevupCtrl)
      {
        bRetVal = (int32_t)RUC_GetNumberOfPhases(pMCT->pRevupCtrl);
      }
      else
      {
        bRetVal = (uint32_t) 0;
      }
    }
    break;

    case MC_PROTOCOL_REG_I_A:
    {
      bRetVal = MCI_GetIab(pMCI).a;
    }
    break;

    case MC_PROTOCOL_REG_I_B:
    {
      bRetVal = MCI_GetIab(pMCI).b;
    }
    break;

    case MC_PROTOCOL_REG_I_ALPHA:
    {
      bRetVal = MCI_GetIalphabeta(pMCI).alpha;
    }
    break;

    case MC_PROTOCOL_REG_I_BETA:
    {
      bRetVal = MCI_GetIalphabeta(pMCI).beta;
    }
    break;

    case MC_PROTOCOL_REG_I_Q_REF:
    {
      bRetVal = MCI_GetIqdref(pMCI).q;
    }
    break;

    case MC_PROTOCOL_REG_I_D_REF:
    {
      bRetVal = MCI_GetIqdref(pMCI).d;
    }
    break;

    case MC_PROTOCOL_REG_V_Q:
    {
      bRetVal = MCI_GetVqd(pMCI).q;
    }
    break;

    case MC_PROTOCOL_REG_V_D:
    {
      bRetVal = MCI_GetVqd(pMCI).d;
    }
    break;

   case MC_PROTOCOL_REG_V_ALPHA:
    {
      bRetVal = MCI_GetValphabeta(pMCI).alpha;
    }
    break;

    case MC_PROTOCOL_REG_V_BETA:
    {
      bRetVal = MCI_GetValphabeta(pMCI).beta;
    }
    break;

    default:
	{
      if ( success != (bool *) 0 ) 
      {
        *success = false;
      }
	}
    break;
  }
  return bRetVal;
}

/**
  * @brief  Allow to execute a command coming from the user.
  * @param  pHandle: Pointer on Handle structure of UI component.
  * @param  bCmdID: Code of command to execute.
  *         See MC_PROTOCOL_CMD_xxx for code definition.
  *  @retval Return true if the command executed succesfully, otherwise false.
*/
__weak bool UI_ExecCmd(UI_Handle_t *pHandle, uint8_t bCmdID)
{
  bool retVal = true;

  MCI_Handle_t * pMCI = pHandle->pMCI[pHandle->bSelectedDrive];

  switch (bCmdID)
  {
  case MC_PROTOCOL_CMD_START_MOTOR:
    {
      /* Call MCI Start motor; */
      MCI_StartMotor(pMCI);
    }
    break;

  case MC_PROTOCOL_CMD_STOP_MOTOR:
  case MC_PROTOCOL_CMD_SC_STOP:
    {
      /* Call MCI Stop motor; */
      MCI_StopMotor(pMCI);
    }
    break;

  case MC_PROTOCOL_CMD_STOP_RAMP:
    {
      if (MCI_GetSTMState(pMCI) == RUN)
      {
        MCI_StopRamp(pMCI);
      }
    }
    break;

  case MC_PROTOCOL_CMD_PING:
    {
	  /* Do nothing at the moment */
    }
    break;

  case MC_PROTOCOL_CMD_START_STOP:
    {
      /* Queries the STM and a command start or stop depending on the state. */
      if (MCI_GetSTMState(pMCI) == IDLE)
      {
        MCI_StartMotor(pMCI);
      }
      else
      {
        MCI_StopMotor(pMCI);
      }
    }
    break;

  case MC_PROTOCOL_CMD_RESET:
    {
	  /* Do nothing at the moment */
    }
    break;

  case MC_PROTOCOL_CMD_FAULT_ACK:
    {
      MCI_FaultAcknowledged(pMCI);
    }
    break;

  case MC_PROTOCOL_CMD_ENCODER_ALIGN:
    {
      MCI_EncoderAlign(pMCI);
    }
    break;

  case MC_PROTOCOL_CMD_IQDREF_CLEAR:
    {
      MCI_Clear_Iqdref(pMCI);
    }
    break;

  default:
    {
    retVal = false;
	}
    break;
  }
  return retVal;
}

/**
  * @brief  Allow to execute a speed ramp command coming from the user.
  * @param  pHandle: Pointer on Handle structure of UI component.
  * @param  wFinalMecSpeedUnit: Final speed value expressed in the unit defined by #SPEED_UNIT.
  * @param  hDurationms: Duration of the ramp expressed in milliseconds. 
  *         It is possible to set 0 to perform an instantaneous change in the value.
  *  @retval Return true if the command executed succesfully, otherwise false.
  */
__weak bool UI_ExecSpeedRamp(UI_Handle_t *pHandle, int32_t wFinalMecSpeedUnit, uint16_t hDurationms)
{
  MCI_Handle_t * pMCI = pHandle->pMCI[pHandle->bSelectedDrive];

  /* Call MCI Exec Ramp */
  MCI_ExecSpeedRamp(pMCI,(int16_t)((wFinalMecSpeedUnit*SPEED_UNIT)/_RPM),hDurationms);
  return true;
}
/**
  * @brief  It is used to execute a torque ramp command coming from the user.
  * @param  pHandle: Pointer on Handle structure of UI component.
  * @param  hTargetFinal: final torque value. See MCI interface for more
            details.
  * @param  hDurationms: the duration of the ramp expressed in milliseconds. It
  *         is possible to set 0 to perform an instantaneous change in the value.
  *  @retval Return true if the command executed succesfully, otherwise false.
  */
__weak bool UI_ExecTorqueRamp(UI_Handle_t *pHandle, int16_t hTargetFinal, uint16_t hDurationms)
{

  MCI_Handle_t * pMCI = pHandle->pMCI[pHandle->bSelectedDrive];

  /* Call MCI Exec Ramp */
  MCI_ExecTorqueRamp(pMCI,hTargetFinal,hDurationms);
  return true;
}

/**
  * @brief  Executes a get Revup data command coming from the user
  *
  * @param  pHandle Pointer on Handle structure of UI component.
  * @param  bStage Rev up phase to be read (zero based).
  * @param  pDurationms Pointer to an uint16_t variable used to retrieve
  *         the duration of the Revup stage.
  * @param  pFinalMecSpeedUnit Pointer to an int16_t variable used to
  *         retrieve the mechanical speed at the end of that stage. Expressed in
  *         the unit defined by #SPEED_UNIT.
  * @param  pFinalTorque Pointer to an int16_t variable used to
  *         retrieve the value of motor torque at the end of that stage. 
  *         This value represents actually the Iq current expressed in digit.
  *         
  *  @retval Returns true if the command executed successfully, false otherwise.
  */
__weak bool UI_GetRevupData(UI_Handle_t *pHandle, uint8_t bStage, uint16_t* pDurationms,
                     int16_t* pFinalMecSpeedUnit, int16_t* pFinalTorque )
{
  bool hRetVal = true;

  RevUpCtrl_Handle_t *pRevupCtrl = pHandle->pMCT[pHandle->bSelectedDrive]->pRevupCtrl;
  if (pRevupCtrl)
  {
    *pDurationms = RUC_GetPhaseDurationms(pRevupCtrl, bStage);
    *pFinalMecSpeedUnit = RUC_GetPhaseFinalMecSpeedUnit(pRevupCtrl, bStage);
    *pFinalTorque = RUC_GetPhaseFinalTorque(pRevupCtrl, bStage);
  }
  else
  {
    hRetVal = false;
  }
  return hRetVal;
}

/**
  * @brief  It is used to execute a set Revup data command coming from the user.
  * @param  pHandle: Pointer on Handle structure of UI component.
  * @param  bStage: is the rev up phase, zero based, to be modified.
  * @param  hDurationms: is the new duration of the Revup stage.
  * @param  hFinalMecSpeedUnit: is the new mechanical speed at the end of that
  *         stage expressed in the unit defined by SPEED_UNIT.
  * @param  hFinalTorque: is the new value of motor torque at the end of that
  *         stage. This value represents actually the Iq current expressed in
  *         digit.
  *  @retval Return true if the command executed successfully, otherwise false.
  */
__weak bool UI_SetRevupData(UI_Handle_t *pHandle, uint8_t bStage, uint16_t hDurationms,
                     int16_t hFinalMecSpeedUnit, int16_t hFinalTorque )
{
  RevUpCtrl_Handle_t *pRevupCtrl = pHandle->pMCT[pHandle->bSelectedDrive]->pRevupCtrl;
  RUC_SetPhaseDurationms(pRevupCtrl, bStage, hDurationms);
  RUC_SetPhaseFinalMecSpeedUnit(pRevupCtrl, bStage, hFinalMecSpeedUnit);
  RUC_SetPhaseFinalTorque(pRevupCtrl, bStage, hFinalTorque);
  return true;
}

/**
  * @brief  Allow to execute a set current reference command coming from the user.
  * @param  pHandle: Pointer on Handle structure of UI component.
  * @param  hIqRef: Current Iq reference on qd reference frame. 
  *         This value is expressed in digit. 
  * @note   current convertion formula (from digit to Amps):
  *               Current(Amps) = [Current(digit) * Vdd micro] / [65536 * Rshunt * Aop]
  * @param  hIdRef: Current Id reference on qd reference frame. 
  *         This value is expressed in digit. See hIqRef param description.
  * @retval none.
  */
__weak void UI_SetCurrentReferences(UI_Handle_t *pHandle, int16_t hIqRef, int16_t hIdRef)
{

  MCI_Handle_t * pMCI = pHandle->pMCI[pHandle->bSelectedDrive];
  qd_t currComp;
  currComp.q = hIqRef;
  currComp.d = hIdRef;
  MCI_SetCurrentReferences(pMCI,currComp);
}

/**
  * @brief  Allow to get information about MP registers available for each step.
  *         PC send to the FW the list of steps to get the available registers. 
  *         The FW returs the list of available registers for that steps.
  * @param  stepList: List of requested steps.
  * @param  pMPInfo: The returned list of register.
  *         It is populated by this function.
  * @retval true if MP is enabled, false otherwise.
  */
__weak bool UI_GetMPInfo(pMPInfo_t stepList, pMPInfo_t pMPInfo)
{
    return false;
}

/**
  * @brief  Hardware and software DAC initialization.
  * @param  pHandle: Pointer on Handle structure of DACx UI component.
  * @retval none.
  */
__weak void UI_DACInit(UI_Handle_t *pHandle)
{
  if (pHandle->pFct_DACInit)
  {
	  pHandle->pFct_DACInit(pHandle);
  }
}

/**
  * @brief  Allow to update the DAC outputs. 
  * @param  pHandle: Pointer on Handle structure of DACx UI component.
  * @retval none.
  */
void UI_DACExec(UI_Handle_t *pHandle)
{
  if (pHandle->pFct_DACExec)
  {
    pHandle->pFct_DACExec(pHandle);
  }
}

/**
  * @brief  Allow to set up the DAC outputs. 
  *         Selected variables will be provided in the related output channels after next DACExec.
  * @param  pHandle: Pointer on Handle structure of DACx UI component.
  * @param  bChannel: DAC channel to program. 
  *         It must be one of the exported channels (Example: DAC_CH0).
  * @param  bVariable: Value to be provided in out through the selected channel.
  *         It must be one of the exported UI register (Example: MC_PROTOCOL_REG_I_A).
  * @retval none.
  */
void UI_SetDAC(UI_Handle_t *pHandle, DAC_Channel_t bChannel,
                         MC_Protocol_REG_t bVariable)
{
  if (pHandle->pFctDACSetChannelConfig)
  {
	  pHandle->pFctDACSetChannelConfig(pHandle, bChannel, bVariable);
  }
}

/**
  * @brief  Allow to get the current DAC channel selected output.
  * @param  pHandle: Pointer on Handle structure of DACx UI component.
  * @param  bChannel: Inspected DAC channel. 
  *         It must be one of the exported channels (Example: DAC_CH0).
  * @retval MC_Protocol_REG_t: Variables provided in out through the inspected channel. 
  *         It must be one of the exported UI register (Example: MC_PROTOCOL_REG_I_A).
  */
__weak MC_Protocol_REG_t UI_GetDAC(UI_Handle_t *pHandle, DAC_Channel_t bChannel)
{
  MC_Protocol_REG_t retVal = MC_PROTOCOL_REG_UNDEFINED;
  if (pHandle->pFctDACGetChannelConfig)
  {
    retVal = pHandle->pFctDACGetChannelConfig(pHandle, bChannel);
  }
  return retVal;
}

/**
  * @brief  Allow to set the value of the user DAC channel.
  * @param  pHandle: Pointer on Handle structure of DACx UI component.
  * @param  bUserChNumber: user DAC channel to program.
  * @param  hValue: Value to be put in output.
  * @retval none.
  */
__weak void UI_SetUserDAC(UI_Handle_t *pHandle, DAC_UserChannel_t bUserChNumber, int16_t hValue)
{
  if (pHandle->pFctDACSetUserChannelValue)
  {
	  pHandle->pFctDACSetUserChannelValue(pHandle, bUserChNumber, hValue);
  }
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
