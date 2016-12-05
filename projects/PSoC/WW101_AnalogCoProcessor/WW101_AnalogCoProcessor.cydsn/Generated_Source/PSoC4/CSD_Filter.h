/***************************************************************************//**
* \file     CSD_Filter.h
* \version  3.10
*
* \brief    This file contains the definitions for all firmware filters 
*           implementation.
*
* \see      CapSense P4 v3.10 Datasheet
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

#if !defined(CY_CAPSENSE_CSD_FILTER_H)
#define CY_CAPSENSE_CSD_FILTER_H

#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h"
    
#include "CSD_Structure.h"    
#include "CSD_Configuration.h"
    
/***************************************
* Function Prototypes 
***************************************/ 

/*******************************************************************************
* LOW LEVEL API 
*******************************************************************************/
/**
* \if SECTION_CAPSENSE_LOW_LEVEL
* \addtogroup group_capsense_low_level
* \{
*/
    
cystatus CSD_UpdateAllBaselines(void);
cystatus CSD_UpdateWidgetBaseline(uint32 widgetId);
cystatus CSD_UpdateSensorBaseline(uint32 widgetId, uint32 sensorId);

void CSD_InitializeAllBaselines(void);    
void CSD_InitializeWidgetBaseline(uint32 widgetId);
void CSD_InitializeSensorBaseline(uint32 widgetId, uint32 sensorId);

/** \}
* \endif */


/*******************************************************************************
* Function Prototypes - internal functions
*******************************************************************************/
/**
* \if SECTION_CAPSENSE_INTERNAL
* \addtogroup group_capsense_internal
* \{
*/

void CSD_FtInitialize(void);

#if (CSD_CSD_AUTOTUNE & CSD_CSD_SS_TH_EN)
    void CSD_RunNoiseEnvelope(uint32 widgetId, uint32 sensorId);
    void CSD_InitializeNoiseEnvelope(uint32 widgetId, uint32 sensorId);
#endif /* #if CSD_CSD_AUTOTUNE & CSD_CSD_SS_TH_EN */

#if (CSD_REGULAR_RC_IIR_FILTER_EN || CSD_PROX_RC_IIR_FILTER_EN)
    void CSD_InitializeIIR(uint32 widgetId, uint32 sensorId);
    void CSD_RunIIR(uint32 widgetId, uint32 sensorId);
#endif /* (CSD_REGULAR_RC_IIR_FILTER_EN || CSD_PROX_RC_IIR_FILTER_EN) */

#if (CSD_REGULAR_RC_MEDIAN_FILTER_EN || CSD_PROX_RC_MEDIAN_FILTER_EN)
    void CSD_InitializeMedian(uint32 widgetId, uint32 sensorId);
    void CSD_RunMedian(uint32 widgetId, uint32 sensorId);
#endif /* (CSD_REGULAR_RC_MEDIAN_FILTER_EN || CSD_PROX_RC_MEDIAN_FILTER_EN) */

#if (CSD_REGULAR_RC_AVERAGE_FILTER_EN || CSD_PROX_RC_AVERAGE_FILTER_EN)
    void CSD_InitializeAverage(uint32 widgetId, uint32 sensorId);
    void CSD_RunAverage(uint32 widgetId, uint32 sensorId);
#endif /* (CSD_REGULAR_RC_AVERAGE_FILTER_EN || CSD_PROX_RC_AVERAGE_FILTER_EN) */

void CSD_FtInitializeBaseline(CSD_RAM_SNS_STRUCT *ptrSensor, uint32 wdType);
uint32 CSD_FtUpdateBaseline(
                            CSD_RAM_WD_BASE_STRUCT *ptrWidgetRam,
                            CSD_RAM_SNS_STRUCT *ptrSensor, 
                            uint32 wdType);

#if (CSD_POS_MEDIAN_FILTER_EN || CSD_REGULAR_RC_MEDIAN_FILTER_EN || CSD_PROX_RC_MEDIAN_FILTER_EN)
uint32 CSD_FtMedian(uint32 x1, uint32 x2, uint32 x3);
#endif /*CSD_POS_MEDIAN_FILTER_EN || CSD_REGULAR_RC_MEDIAN_FILTER_EN || CSD_PROX_RC_MEDIAN_FILTER_EN*/

uint32 CSD_FtIIR1stOrder(uint32 input, uint32 prevOutput, uint32 n, uint32 shift);

#if (CSD_POS_JITTER_FILTER_EN)
    uint32 CSD_FtJitter(uint32 input, uint32 prevOutput);
#endif /* CSD_POS_JITTER_FILTER_EN */

void CSD_FtInitializeBaselineChannel(CSD_RAM_SNS_STRUCT *ptrSensor, uint32 wdType, uint32 channel); 

#if (CSD_REGULAR_RC_FILTER_EN || CSD_PROX_RC_FILTER_EN)
    void CSD_FtRunEnabledFilters(uint32 widgetId, uint32 sensorId);
    void CSD_FtRunEnabledFiltersInternal(CSD_PTR_FILTER_VARIANT ptrFilterHistObj, 
                                                      CSD_RAM_SNS_STRUCT *ptrSensorObj, uint32 wdType);
#endif /* (CSD_REGULAR_RC_FILTER_EN || CSD_PROX_RC_FILTER_EN) */


#if (CSD_REGULAR_RC_IIR_FILTER_EN || CSD_PROX_RC_IIR_FILTER_EN)
    void CSD_InitializeIIRInternal(CSD_PTR_FILTER_VARIANT ptrFilterHistObj,
                                                  CSD_RAM_SNS_STRUCT *ptrSensorObj, uint32 wdType);
    void CSD_RunIIRInternal(CSD_PTR_FILTER_VARIANT ptrFilterHistObj,
                                           CSD_RAM_SNS_STRUCT *ptrSensorObj, uint32 wdType);
#endif /* (CSD_REGULAR_RC_IIR_FILTER_EN || CSD_PROX_RC_IIR_FILTER_EN) */

#if (CSD_REGULAR_RC_MEDIAN_FILTER_EN || CSD_PROX_RC_MEDIAN_FILTER_EN)
    void CSD_InitializeMedianInternal(CSD_PTR_FILTER_VARIANT ptrFilterHistObj,
                                                     CSD_RAM_SNS_STRUCT *ptrSensorObj, uint32 wdType);    
    void CSD_RunMedianInternal(CSD_PTR_FILTER_VARIANT ptrFilterHistObj,
                                              CSD_RAM_SNS_STRUCT *ptrSensorObj, uint32 wdType);    
#endif /* (CSD_REGULAR_RC_MEDIAN_FILTER_EN || CSD_PROX_RC_MEDIAN_FILTER_EN) */

#if (CSD_REGULAR_RC_AVERAGE_FILTER_EN || CSD_PROX_RC_AVERAGE_FILTER_EN)
    void CSD_InitializeAverageInternal(CSD_PTR_FILTER_VARIANT ptrFilterHistObj,
                                                      CSD_RAM_SNS_STRUCT *ptrSensorObj, uint32 wdType);
    void CSD_RunAverageInternal(CSD_PTR_FILTER_VARIANT ptrFilterHistObj,
                                               CSD_RAM_SNS_STRUCT *ptrSensorObj, uint32 wdType);
#endif /* (CSD_REGULAR_RC_AVERAGE_FILTER_EN || CSD_PROX_RC_AVERAGE_FILTER_EN) */

/** \}
* \endif */

/***************************************
* Initial Parameter Constants
***************************************/
#define NOISE_ENVELOPE_SHIFT                        (0x02u)
#define NOISE_ENVELOPE_RUN_DIFF_SHIFT               (0x03u)
#define NOISE_ENVELOPE_SIGN_REG                     (0x0Fu)
#define NOISE_ENVELOPE_SIGN_REG_SHIFT               (0x01u)
#define NOISE_ENVELOPE_RESET_COUNTER                (0x0Au)
#define NOISE_ENVELOPE_4_TIMES                      (0x02u)

#endif /* End CY_CAPSENSE_CSD_FILTER_H */


/* [] END OF FILE */
