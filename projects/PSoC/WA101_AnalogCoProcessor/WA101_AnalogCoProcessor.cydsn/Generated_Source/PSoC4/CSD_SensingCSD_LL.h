/***************************************************************************//**
* \file CSD_SensingCSD_LL.h
* \version 3.10
*
* \brief
*   This file provides the headers of APIs specific to CSD sensing implementation.
*
* \see CapSense P4 v3.10 Datasheet
*
*//*****************************************************************************
* Copyright (2016), Cypress Semiconductor Corporation.
********************************************************************************
* This software is owned by Cypress Semiconductor Corporation (Cypress) and is
* protected by and subject to worldwide patent protection (United States and
* foreign), United States copyright laws and international treaty provisions.
* Cypress hereby grants to licensee a personal, non-exclusive, non-transferable
* license to copy, use, modify, create derivative works of, and compile the
* Cypress Source Code and derivative works for the sole purpose of creating
* custom software in support of licensee product to be used only in conjunction
* with a Cypress integrated circuit as specified in the applicable agreement.
* Any reproduction, modification, translation, compilation, or representation of
* this software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH
* REGARD TO THIS MATERIAL, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* Cypress reserves the right to make changes without further notice to the
* materials described herein. Cypress does not assume any liability arising out
* of the application or use of any product or circuit described herein. Cypress
* does not authorize its products for use as critical components in life-support
* systems where a malfunction or failure may reasonably be expected to result in
* significant injury to the user. The inclusion of Cypress' product in a life-
* support systems application implies that the manufacturer assumes all risk of
* such use and in doing so indemnifies Cypress against all charges. Use may be
* limited by and subject to the applicable Cypress software license agreement.
*******************************************************************************/

#if !defined(CY_CAPSENSE_CSD_SENSINGCSD_LL_H)
#define CY_CAPSENSE_CSD_SENSINGCSD_LL_H

#include <CyLib.h>
#include <cyfitter.h>
#include "cytypes.h"
#include "CSD_Structure.h"
#include "CSD_Configuration.h"

/***************************************
* Function Prototypes
**************************************/

/**
* \if SECTION_CAPSENSE_LOW_LEVEL
* \addtogroup group_capsense_low_level
* \{
*/

void CSD_CSDSetupWidget(uint32 wdgtIndex);
void CSD_CSDSetupWidgetExt(uint32 wdgtIndex, uint32 snsIndex);
void CSD_CSDScan(void);
void CSD_CSDScanExt(void);
#if ((CSD_CSD_SS_DIS != CSD_CSD_AUTOTUNE) || \
     (CSD_ENABLE == CSD_CSD_IDAC_AUTOCAL_EN))
    cystatus CSD_CSDCalibrateWidget(uint32 wdgtIndex, uint32 target);
#endif /* ((CSD_CSD_SS_DIS != CSD_CSD_AUTOTUNE) || \
           (CSD_ENABLE == CSD_CSD_IDAC_AUTOCAL_EN))  */
void CSD_CSDConnectSns(CSD_FLASH_IO_STRUCT const *snsAddrPtr);
void CSD_CSDDisconnectSns(CSD_FLASH_IO_STRUCT const *snsAddrPtr);

/** \}
* \endif */

/*****************************************************
* Function Prototypes - internal Low Level functions
*****************************************************/
/**
* \if SECTION_CAPSENSE_INTERNAL
* \addtogroup group_capsense_internal
* \{
*/

void CSD_SsCSDInitialize(void);
void CSD_SsCSDStartSample(void);
void CSD_SsCSDSetUpIdacs(CSD_RAM_WD_BASE_STRUCT const *ptrWdgt);
void CSD_SsCSDConfigClock(void);
void CSD_SsCSDElectrodeCheck(void);
#if ((CSD_ENABLE == CSD_CSD_SHIELD_EN) && \
     (0u != CSD_CSD_TOTAL_SHIELD_COUNT))
    void CSD_SsCSDDisableShieldElectrodes(void);
#endif /* ((CSD_ENABLE == CSD_CSD_SHIELD_EN) && \
           (0u != CSD_CSD_TOTAL_SHIELD_COUNT)) */
#if (CSD_ENABLE == CSD_CSDV2)
    uint32 CSD_SsCSDGetNumberOfConversions(uint32 snsClkDivider, uint32 resolution, uint32 snsClkSrc);
#endif /* (CSD_ENABLE == CSD_CSDV2) */
void CSD_SsCSDCalculateScanDuration(CSD_RAM_WD_BASE_STRUCT const *ptrWdgt);
void CSD_SsCSDConnectSensorExt(uint32 wdgtIndex, uint32 sensorIndex);
void CSD_SsCSDDisconnectSnsExt(uint32 wdgtIndex, uint32 sensorIndex);
void CSD_SsCSDSetModeSnsClockDivider(uint32 snsClkSource, uint32 snsClkDivider);

#if ((CSD_CSD_SS_DIS != CSD_CSD_AUTOTUNE) || \
     (CSD_ENABLE == CSD_SELF_TEST_EN) || \
     (CSD_ENABLE == CSD_CSD_IDAC_AUTOCAL_EN))
    #if (CSD_ENABLE == CSD_CSD_IDAC_COMP_EN)
        void CSD_SsCSDSetSingleIdacMode(void);
    #endif /* (CSD_ENABLE == CSD_CSD_IDAC_COMP_EN) */
#endif /* ((CSD_CSD_SS_DIS != CSD_CSD_AUTOTUNE) || \
           (CSD_ENABLE == CSD_SELF_TEST_EN) || \
           (CSD_ENABLE == CSD_CSD_IDAC_AUTOCAL_EN)) */

/** \}
* \endif */

/***************************************
* Global software variables
***************************************/
extern uint32 CSD_configCsd;
/* Interrupt handler */
extern CY_ISR_PROTO(CSD_CSDPostSingleScan);
extern CY_ISR_PROTO(CSD_CSDPostMultiScan);
#if (CSD_ENABLE == CSD_CSD_GANGED_SNS_EN)
extern CY_ISR_PROTO(CSD_CSDPostMultiScanGanged);
#endif /* (CSD_ENABLE == CSD_CSD_GANGED_SNS_EN) */
#if (CSD_ENABLE == CSD_CSD_NOISE_METRIC_EN)
    extern uint8 CSD_badConversionsNum;
#endif /* (CSD_ENABLE == CSD_CSD_NOISE_METRIC_EN) */

#endif /* End CY_CAPSENSE_CSD_SENSINGCSD_LL_H */


/* [] END OF FILE */
