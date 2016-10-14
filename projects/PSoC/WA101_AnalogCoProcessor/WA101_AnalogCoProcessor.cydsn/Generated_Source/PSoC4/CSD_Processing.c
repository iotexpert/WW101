/***************************************************************************//**
* \file CSD_Processing.c
* \version 3.10
*
* \brief
* This file provides the source code for the Data Processing module APIs
* of the component. The Data Processing module is responsible for the
* low level raw counts processing provided by the sensing module, maintaining
* baseline and difference values and performing high-level widget processing
* like updating button status or calculating slider position.
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

#include "cytypes.h"
#include "CSD_Configuration.h"
#include "CSD_Processing.h"
#include "CSD_Filter.h"

#if (0u != CSD_CENTROID_EN)
    #include "CSD_Centroid_LL.h"
#endif /* (0 != CSD_CENTROID_EN) */

#if (0 != (CSD_CSD_AUTOTUNE & CSD_CSD_SS_TH_EN))
    #include "CSD_SmartSense_LL.h"
#endif /* (0 != (CSD_CSD_AUTOTUNE & CSD_CSD_SS_TH_EN)) */

#if (CSD_ENABLE == CSD_SELF_TEST_EN)
    #include "CSD_SelfTest.h"
#endif

#include "CyLib.h"

/***********************************************************************************************************************
* Local definition
***********************************************************************************************************************/

/* This definition resolves to TRUE if any of the FW filters for raw counts are enabled */
#define DP_FW_FILTER_EN                                 \
            (CSD_REGULAR_RC_FILTER_EN |  \
             CSD_PROX_RC_FILTER_EN)

/* This definition resolves to TRUE if any of the position filters are enabled */
#define DP_POSITION_FILTER_EN                           \
            (CSD_POS_IIR_FILTER_EN      || \
             CSD_POS_MEDIAN_FILTER_EN   || \
             CSD_POS_AVERAGE_FILTER_EN  || \
             CSD_POS_JITTER_FILTER_EN)

/* This definition resolves to TRUE if any of position filters are enabled and component has sliders or CSD touchpads.
 * The processing function of the CSD touchpad reuses the filtering function of the slider widget. */
#define DP_LINEAR_SLIDER_FILTER_EN                                             \
           (DP_POSITION_FILTER_EN && (CSD_TOTAL_LINEAR_SLIDERS || \
                                      CSD_CSD_TOUCHPAD_WIDGET_EN))

/* UINT32 constant with all odd bits set. It is used by the processing function of the Proximity widget to
*  determine type of the status bit: odd bits report proximity status; even bits report touch status;
 */
#define UINT32_ODD_BITS_CONST       (0x55555555u)

/* Default value for the Off Debounce counter when the Off Debounce is disabled */
#define DEFAULT_OFF_DEBOUNCE_VALUE  (1u)
#define PERCENTAGE_100              (0x00000064Lu)
#define TEMP_BUFFER_MSB_SHIFT       (16)

/***********************************************************************************************************************
* Local variables
***********************************************************************************************************************/
#if (0u != CSD_CSX_MULTIPHASE_TX_EN)
    static uint16 deconvRowData[CSD_CSX_MAX_TX_PHASE_LENGTH];
#endif


/***********************************************************************************************************************
* Static Function Prototypes
***********************************************************************************************************************/
/**
* \if SECTION_CAPSENSE_INTERNAL
* \addtogroup group_capsense_internal
* \{
*/
#if (0u != CSD_TOTAL_LINEAR_SLIDERS)
    static CY_INLINE void DpUpdateLinSlider(
                        const uint16 newPos[], uint32 numTouches,
                        CSD_FLASH_WD_STRUCT const *ptrFlashWdgt,
                        CSD_RAM_WD_SLIDER_STRUCT *ptrRamWdgt);
#endif /* #if (0u != CSD_TOTAL_LINEAR_SLIDERS) */

#if (0u != DP_LINEAR_SLIDER_FILTER_EN)
    static CY_INLINE void DpFilterLinSlider(
                        uint16 newPos, uint32 staticConfig, uint16 *ptrCurrPos,
                        CSD_SLIDER_POS_HISTORY_STRUCT *ptrHistory);
#endif /* #if (0u != DP_LINEAR_SLIDER_FILTER_EN) */

#if (0u != CSD_TOTAL_RADIAL_SLIDERS)
    static CY_INLINE void DpUpdateRadSlider(
                        const uint16 newPos[], uint32 numTouches,
                        CSD_FLASH_WD_STRUCT const * ptrFlashWdgt,
                        CSD_RAM_WD_SLIDER_STRUCT *ptrRamWdgt);
#endif /* #if (0u != CSD_TOTAL_RADIAL_SLIDERS) */

#if (0u != CSD_CSD_TOUCHPAD_WIDGET_EN)
    static CY_INLINE void CSD_DpInitCsdTouchpad(CSD_FLASH_WD_STRUCT const * ptrFlashWdgt);
#endif /* #if (0u != CSD_CSD_TOUCHPAD_WIDGET_EN) */

#if (0u != CSD_CSX_TOUCHPAD_WIDGET_EN)
    static CY_INLINE void CSD_DpInitCsxTouchpad(CSD_FLASH_WD_STRUCT const * ptrFlashWdgt);
#endif /* #if (0u != CSD_CSX_TOUCHPAD_WIDGET_EN) */

#if (0u != CSD_SLIDER_WIDGET_EN)
    static CY_INLINE void CSD_DpInitSlider(CSD_FLASH_WD_STRUCT const * ptrFlashWdgt);
#endif /* #if (0u != CSD_SLIDER_WIDGET_EN) */

#if (0u != CSD_CSD_MATRIX_WIDGET_EN)
    static CY_INLINE void CSD_DpInitCsdMatrix(CSD_FLASH_WD_STRUCT const * ptrFlashWdgt);
#endif /* #if (0u != CSD_CSD_MATRIX_WIDGET_EN) */

/** \}
* \endif */

#if (0u != CSD_CSD_TOUCHPAD_WIDGET_EN)
/*******************************************************************************
* Function Name: CSD_DpInitCsdTouchpad
****************************************************************************//**
*
* \brief
*  Performs initialization of the CSD touchpad widget.
*   - Resets the position data to NONE
*   - Initializes the median filter history if the appropriate position filter is enabled.
*
* \details
*  The position data is always reset to the CSD_TOUCHPAD_POS_NONE value.
*  The same happens to the median filter history data if this type of filter is enabled.
*
*******************************************************************************/
static CY_INLINE void CSD_DpInitCsdTouchpad(CSD_FLASH_WD_STRUCT const * ptrFlashWdgt)
{
    CSD_RAM_WD_CSD_TOUCHPAD_STRUCT *ptrWdTouchpad;

    #if (0u != CSD_POS_MEDIAN_FILTER_EN)
        CSD_TOUCHPAD_POS_HISTORY_STRUCT *ptrHistory;
    #endif  /* #if (0u != CSD_POS_MEDIAN_FILTER_EN) */

    ptrWdTouchpad = ptrFlashWdgt->ptr2WdgtRam;

    /* Reset position data */
    ptrWdTouchpad->posX = CSD_TOUCHPAD_POS_NONE;
    ptrWdTouchpad->posY = CSD_TOUCHPAD_POS_NONE;

    /* Reset median filter */
    #if (0u != CSD_POS_MEDIAN_FILTER_EN)
        ptrHistory = ptrFlashWdgt->ptr2PosHistory;

        if (NULL != ptrHistory)
        {
            ptrHistory->xPos.posMedianZ1 = CSD_TOUCHPAD_POS_NONE;
            ptrHistory->xPos.posMedianZ2 = CSD_TOUCHPAD_POS_NONE;

            ptrHistory->yPos.posMedianZ1 = CSD_TOUCHPAD_POS_NONE;
            ptrHistory->yPos.posMedianZ2 = CSD_TOUCHPAD_POS_NONE;
        }
    #endif /* #if (0u != CSD_POS_MEDIAN_FILTER_EN) */
}
#endif /* #if (0u != CSD_CSD_TOUCHPAD_WIDGET_EN) */

#if (0u != CSD_CSX_TOUCHPAD_WIDGET_EN)
/*******************************************************************************
* Function Name: CSD_DpInitCsxTouchpad
****************************************************************************//**
*
* \brief
*  Performs initialization of the CSX touchpad widget i.e. Resets the position data to NONE.
*
* \details
*  The position data is always reset to the CSD_TOUCHPAD_POS_NONE value.
*
*******************************************************************************/
static CY_INLINE void CSD_DpInitCsxTouchpad(CSD_FLASH_WD_STRUCT const * ptrFlashWdgt)
{
    CSD_RAM_WD_CSX_TOUCHPAD_STRUCT *ptrWdTouchpad;

    ptrWdTouchpad = ptrFlashWdgt->ptr2WdgtRam;

    /* Reset position data for finger */
    ptrWdTouchpad->touch[0u].x = CSD_TOUCHPAD_POS_NONE;
    ptrWdTouchpad->touch[0u].y = CSD_TOUCHPAD_POS_NONE;
    ptrWdTouchpad->touch[0u].z = LO8(CSD_TOUCHPAD_POS_NONE);

    #if (CSD_CSX_MAX_FINGERS > 1u)
        ptrWdTouchpad->touch[1u].x = CSD_TOUCHPAD_POS_NONE;
        ptrWdTouchpad->touch[1u].y = CSD_TOUCHPAD_POS_NONE;
        ptrWdTouchpad->touch[1u].z = LO8(CSD_TOUCHPAD_POS_NONE);
    #endif /* #if (CSD_CSX_MAX_FINGERS > 1u) */

    #if (CSD_CSX_MAX_FINGERS > 2u)
        ptrWdTouchpad->touch[2u].x = CSD_TOUCHPAD_POS_NONE;
        ptrWdTouchpad->touch[2u].y = CSD_TOUCHPAD_POS_NONE;
        ptrWdTouchpad->touch[2u].z = LO8(CSD_TOUCHPAD_POS_NONE);
    #endif /* #if (CSD_CSX_MAX_FINGERS > 2u) */
}
#endif /* #if (0u != CSD_CSX_TOUCHPAD_WIDGET_EN) */

#if (0u != CSD_SLIDER_WIDGET_EN)
/*******************************************************************************
* Function Name: CSD_DpInitSlider
****************************************************************************//**
*
* \brief
*  Performs initialization of the Radial and Linear Slider widget.
*   - Resets the position data to NONE
*   - Initializes the median filter history to NONE if it is enabled.
*
* \details
*  Position data is always reset to the CSD_SLIDER_POS_NONE value.
*  The same happens to the median filter history data.
*
*******************************************************************************/
static CY_INLINE void CSD_DpInitSlider(CSD_FLASH_WD_STRUCT const * ptrFlashWdgt)
{
    uint32 idx;
    CSD_RAM_WD_SLIDER_STRUCT *ptrWdSlider;

    #if (0u != CSD_POS_MEDIAN_FILTER_EN)
        CSD_SLIDER_POS_HISTORY_STRUCT *ptrHistory;
    #endif /* #if (0u != CSD_POS_MEDIAN_FILTER_EN) */

    ptrWdSlider = ptrFlashWdgt->ptr2WdgtRam;

    /* Reset slider position data to NONE */
    for (idx = 0u; idx < CSD_NUM_CENTROIDS; idx++)
    {
        ptrWdSlider->position[idx] = CSD_SLIDER_POS_NONE;

        #if (0u != CSD_POS_MEDIAN_FILTER_EN)
            ptrHistory = ptrFlashWdgt->ptr2PosHistory;
            
            if (NULL != ptrHistory)
            {
                ptrHistory[idx].posMedianZ1 = CSD_SLIDER_POS_NONE;
                ptrHistory[idx].posMedianZ2 = CSD_SLIDER_POS_NONE;
            }
        #endif /* #if (0u != CSD_POS_MEDIAN_FILTER_EN) */
    }
}
#endif /* #if (0u != CSD_SLIDER_WIDGET_EN) */

#if (0u != CSD_CSD_MATRIX_WIDGET_EN)
/*******************************************************************************
* Function Name: CSD_DpInitCsdMatrix
****************************************************************************//**
*
* \brief
*  Performs initialization of the CSD Matrix Buttons widget.
*   - Resets the active button position data to NONE.
*
* \details
*  Position data is always reset to the CSD_MATRIX_POS_NONE value.
*
*******************************************************************************/
static CY_INLINE void CSD_DpInitCsdMatrix(CSD_FLASH_WD_STRUCT const * ptrFlashWdgt)
{
    CSD_RAM_WD_CSD_MATRIX_STRUCT *ptrRamWdgt;

    ptrRamWdgt = ptrFlashWdgt->ptr2WdgtRam;

    /* Reset sensor id, row and col registers in data structure */
    ptrRamWdgt->posRow   = CSD_MATRIX_POS_NONE;
    ptrRamWdgt->posCol   = CSD_MATRIX_POS_NONE;
    ptrRamWdgt->posSnsId = CSD_MATRIX_POS_NONE;
}
#endif /* #if (0u != CSD_CSD_MATRIX_WIDGET_EN) */

/*******************************************************************************
* Function Name: CSD_DpInitialize
****************************************************************************//**
*
* \brief
*  Performs initialization of the data processing FW module:
*   - Resets the de-bounce counters of all widgets
*   - Resets the position filter history for slider and touchpad widgets
*   - Clears widget and sensor statuses
*   - Enables all widgets.
*
* \details
*  The Button and Matrix Button widgets have individual de-bounce counter per
*  sensor for the CSD widgets and per node for the CSX widgets.
*
*  The Slider and Touchpad widgets have a single de-bounce counter per widget.
*
*  The Proximity widget has two de-bounce counters per sensor. One for the
*  proximity event and second for the touch event.
*
*  All debounce counters during initialization are set to the value of the
*  onDebounce widget parameter.
*
*******************************************************************************/
void CSD_DpInitialize(void)
{
    uint32 wdgtId;
    CSD_WD_TYPE_ENUM widgetType;
    CSD_RAM_WD_BASE_STRUCT * ptrRamWdgt;
    CSD_FLASH_WD_STRUCT const * ptrFlashWdgt;

    for (wdgtId = 0u; wdgtId < CSD_WDGT_STATUS_WORDS; wdgtId++)
    {
        /* Clear widget status */
        CSD_dsRam.wdgtStatus[wdgtId] = 0u;

        /* Enable all widgets */
        CSD_dsRam.wdgtEnable[wdgtId] = 0xFFFFFFFFLu;

        /* Set all widgets to working state if the Self-Test is not enabled */
        #if (0u != CSD_SELF_TEST_EN)
            CSD_dsRam.wdgtWorking[wdgtId] = 0xFFFFFFFFLu;
        #endif /* #if (0u == CSD_SELF_TEST_EN) */
    }

    ptrFlashWdgt = CSD_dsFlash.wdgtArray;

    for (wdgtId = CSD_TOTAL_WIDGETS; wdgtId-- > 0u; )
    {
        ptrRamWdgt = ptrFlashWdgt->ptr2WdgtRam;
        widgetType = (CSD_WD_TYPE_ENUM)ptrFlashWdgt->wdgtType;

        /* Clear all sensor statuses */
        CSD_dsRam.snsStatus[wdgtId] = 0u;

        /* Reset de-bounce counters */
        switch (widgetType)
        {
        #if (0u != CSD_MATRIX_WIDGET_EN)
            case CSD_WD_MATRIX_BUTTON_E:
                #if (0u != CSD_CSD_MATRIX_WIDGET_EN)
                    if (CSD_SENSE_METHOD_CSD_E == CSD_GET_SENSE_METHOD(ptrFlashWdgt))
                    {
                        CSD_DpInitCsdMatrix(ptrFlashWdgt);
                    }
                #endif /* #if (0u != CSD_CSD_MATRIX_WIDGET_EN) */
                
                if (NULL != ptrFlashWdgt->ptr2DebounceArr)
                {
                    /* Initialize array of debounce counters */
                    (void)memset(ptrFlashWdgt->ptr2DebounceArr, 
                                 (int32)ptrRamWdgt->onDebounce, 
                                 (size_t)ptrFlashWdgt->totalNumSns);
                }
                break;
        #endif /* #if (0u != CSD_MATRIX_WIDGET_EN) */
        
        #if (0u != CSD_BUTTON_WIDGET_EN)
            case CSD_WD_BUTTON_E:
                if (NULL != ptrFlashWdgt->ptr2DebounceArr)
                {
                    /* Initialize array of debounce counters */
                    (void)memset(ptrFlashWdgt->ptr2DebounceArr, 
                                 (int32)ptrRamWdgt->onDebounce, 
                                 (size_t)ptrFlashWdgt->totalNumSns);
                }
                break;
        #endif /* #if (0u != CSD_BUTTON_WIDGET_EN) */

        #if (0u != CSD_SLIDER_WIDGET_EN)
            case CSD_WD_LINEAR_SLIDER_E:
            case CSD_WD_RADIAL_SLIDER_E:
                if (NULL != ptrFlashWdgt->ptr2DebounceArr)
                {
                    /* Initialize single debounce counter of slider */
                    *(uint8 *)(ptrFlashWdgt->ptr2DebounceArr) = ptrRamWdgt->onDebounce;
                }

                CSD_DpInitSlider(ptrFlashWdgt);

                break;
        #endif /* #if (0u != CSD_SLIDER_WIDGET_EN) */

        #if (0u != CSD_TOUCHPAD_WIDGET_EN)
            case CSD_WD_TOUCHPAD_E:
                if (NULL != ptrFlashWdgt->ptr2DebounceArr)
                {
                    /* Initialize single debounce counter of touchpad */
                    *(uint8 *)(ptrFlashWdgt->ptr2DebounceArr) = ptrRamWdgt->onDebounce;
                }

                #if (0u != CSD_CSD_TOUCHPAD_WIDGET_EN)
                    if (CSD_SENSE_METHOD_CSD_E == CSD_GET_SENSE_METHOD(ptrFlashWdgt))
                    {
                        CSD_DpInitCsdTouchpad(ptrFlashWdgt);
                    }
                #endif /* #if (0u != CSD_CSD_TOUCHPAD_WIDGET_EN) */

                #if (0u != CSD_CSX_TOUCHPAD_WIDGET_EN)
                    if (CSD_SENSE_METHOD_CSX_E == CSD_GET_SENSE_METHOD(ptrFlashWdgt))
                    {
                        CSD_DpInitCsxTouchpad(ptrFlashWdgt);
                    }
                #endif /* #if (0u != CSD_CSX_TOUCHPAD_WIDGET_EN) */

                break;
        #endif /* #if (0u != CSD_TOUCHPAD_WIDGET_EN) */

        #if (0u != CSD_PROXIMITY_WIDGET_EN)
            case CSD_WD_PROXIMITY_E:
                if (NULL != ptrFlashWdgt->ptr2DebounceArr)
                {
                    /* Proximity widgets have 2 de-bounce counters per sensor. 
                     * Therefore the number of sensors is multiplied by 2. 
                     */
                   (void)memset(ptrFlashWdgt->ptr2DebounceArr, 
                                (int32)ptrRamWdgt->onDebounce, 
                                (size_t)ptrFlashWdgt->totalNumSns * 2u);
                }
                break;
        #endif /* #if (0u != CSD_PROXIMITY_WIDGET_EN) */
                
            default:
                break;
        }

        /* Move to next widget */
        ptrFlashWdgt++;
    }
}

#if (0u != CSD_TOTAL_CSX_WIDGETS)
/*******************************************************************************
* Function Name: CSD_DpProcessCsxWidgetRawCounts
****************************************************************************//**
*
* \brief
*  Performs default processing of the CSX raw counts.
*
* \details
*  Performs default processing of the CSX raw counts:
*   - Run Filters
*   - Update Baselines
*   - Update Differences
*   - Update Status.
*  The same process is applied to all sensors of the specified widget.
*
* \param ptrFlashWdgt The pointer to the Flash widget structure.
*
* \return   
*  Returns the status of the specified widget processing operation:
*  - CYRET_SUCCESS if operation was successfully completed;
*  - CSD_PROCESS_BASELINE_FAILED if baseline processing of any 
*    sensor of the specified widget failed. The result is concatenated with the index
*    of failed sensor.
*
*******************************************************************************/
uint32 CSD_DpProcessCsxWidgetRawCounts(CSD_FLASH_WD_STRUCT const *ptrFlashWdgt)
{
    uint32 result = CYRET_SUCCESS;
    uint32 widgetType;
    uint32 snsCount;
    CSD_RAM_SNS_STRUCT *ptrSns;
    CSD_RAM_WD_BASE_STRUCT *ptrRamWdgt;

    #if (0u != DP_FW_FILTER_EN)
        CSD_PTR_FILTER_VARIANT fltrHistV;
        
        fltrHistV.ptr = ptrFlashWdgt->ptr2FltrHistory;
    #endif /* #if (0u != DP_FW_FILTER_EN) */

    ptrSns = ptrFlashWdgt->ptr2SnsRam;
    snsCount = ptrFlashWdgt->totalNumSns;
    ptrRamWdgt = ptrFlashWdgt->ptr2WdgtRam;
    widgetType = ptrFlashWdgt->wdgtType;

    /* Run deconvolution algorithm for the widgets with multiphase TX support */
    #if (0u != CSD_CSX_MULTIPHASE_TX_EN)
        if (0u != (ptrFlashWdgt->staticConfig & CSD_MULTIPHASE_TX_MASK))
        {
            CSD_DpDeconvolution(ptrFlashWdgt);
        }
    #endif  /* #if (0u != CSD_CSX_MULTIPHASE_TX_EN) */   
    
    for (;snsCount-- > 0u;)
    {
        #if (0u != CSD_CSX_SKIP_OVSMPL_SPECIFIC_NODES)
            if (0u != ptrSns->resolution)
        #endif
        {
            #if (0u != DP_FW_FILTER_EN)
                CSD_FtRunEnabledFiltersInternal(fltrHistV, ptrSns, widgetType);
            #endif /* #if (0u != DP_FW_FILTER_EN) */

            result = CSD_FtUpdateBaseline(ptrRamWdgt, ptrSns, widgetType);
            
            #if (CSD_ENABLE == CSD_TST_BSLN_DUPLICATION_EN)
                if (CYRET_SUCCESS != result)
                {
                    result = snsCount | CSD_PROCESS_BASELINE_FAILED;
                }
            #endif /* (CSD_ENABLE == CSD_TST_BSLN_DUPLICATION_EN) */

            CSD_DpUpdateDifferences(ptrRamWdgt, ptrSns);
        }
        /* Move pointers to the next sensor and filter history objects */
        ptrSns++;
            
        #if (0u != DP_FW_FILTER_EN)
            CSD_INC_FLTR_OBJ_VARIANT(0u, fltrHistV);
        #endif /* #if (0u != DP_FW_FILTER_EN) */
    }
    return result;
}

/*******************************************************************************
* Function Name: CSD_DpProcessCsxSensorRawCountsExt
****************************************************************************//**
*
* \brief
*   Performs customized processing of the CSX sensor raw counts.
*
* \details
*   Performs customized processing of the CSX sensor raw counts.
*
*   If all bits are set at once the default processing order will take place.
*   For the custom order this API can be called multiple times and execute one task at a time.
*
* \param ptrFlashWdgt The pointer to the Flash Widget Object.
* \param ptrSns       The pointer to the specific RAM Sensor Object.
* \param fltrHistV    The pointer to the Filter History Object in RAM associated with specific sensor.
* \param mode         The bit-mask with the data processing tasks to be executed.
*   The mode parameters can take the following values:
*    - CSD_PROCESS_FILTER     (0x01) Run Firmware Filter
*    - CSD_PROCESS_BASELINE   (0x02) Update Baselines
*    - CSD_PROCESS_DIFFCOUNTS (0x04) Update Difference Counts
*    - CSD_PROCESS_ALL               Execute all tasks
*
* \return   
*  Returns the status of the specified sensor processing operation:
*  - CYRET_SUCCESS if operation was successfully completed;
*  - CSD_PROCESS_BASELINE_FAILED if baseline processing of any 
*    sensor of the specified widget failed. The result is concatenated with the index
*    of failed sensor.
*
*******************************************************************************/
uint32 CSD_DpProcessCsxSensorRawCountsExt(
                CSD_FLASH_WD_STRUCT const *ptrFlashWdgt,
                CSD_RAM_SNS_STRUCT *ptrSns,
                CSD_PTR_FILTER_VARIANT fltrHistV,
                uint32 mode)
{
    uint32  result = CYRET_SUCCESS;
    uint32  widgetType;
    CSD_RAM_WD_BASE_STRUCT *ptrRamWdgt;

    widgetType = ptrFlashWdgt->wdgtType;
    ptrRamWdgt = ptrFlashWdgt->ptr2WdgtRam;
    
    #if (0u != DP_FW_FILTER_EN)
        if (0u != (mode & CSD_PROCESS_FILTER))
        {
            CSD_FtRunEnabledFiltersInternal(fltrHistV, ptrSns, widgetType);
        }
    #else
        (void)fltrHistV; /* This parameter is unused in such configurations */
    #endif /* #if (0u != DP_FW_FILTER_EN) */

    if (0u != (mode & CSD_PROCESS_BASELINE))
    {
        result = CSD_FtUpdateBaseline(ptrRamWdgt, ptrSns, widgetType);
    }

    if (0u != (mode & CSD_PROCESS_DIFFCOUNTS))
    {
        CSD_DpUpdateDifferences(ptrRamWdgt, ptrSns);
    }
    return result;
}


/*******************************************************************************
* Function Name: CSD_DpProcessCsxWidgetStatus
****************************************************************************//**
*
* \brief
*   Updates the status of the CSX widget in the Data Structure.
*
* \details
*  This API determines the type of widget and runs the appropriate function
*  that implements the status update algorithm for this type of widget.
*
*  When the widget-specific processing completes this API updates the snsStatus
*  and wdgtStatus registers in the data structure. The wdgtStatus register
*  is a bitmask were each bit corresponds to the specific widget identified by
*  the widget index. When the bit is set the widget is active (touched);
*
*  Depending on the type of widget the snsStatus register may contain either
*  status of each widget node (active/non-active) or in case of the CSX Touchpad
*  is contains the number of touches. In both cases 0 means that widget is not
*  active.
*
* \param wdgtId       The widget index number.
* \param ptrFlashWdgt The pointer to the Flash Widget Object.
*
*******************************************************************************/
void CSD_DpProcessCsxWidgetStatus(uint32 wdgtId, CSD_FLASH_WD_STRUCT const *ptrFlashWdgt)
{
    uint32 sensorStatus;
    uint32 wdStatusIndex;
    uint32 wdStatusMask;
    CSD_WD_TYPE_ENUM widgetType;

    widgetType = (CSD_WD_TYPE_ENUM)ptrFlashWdgt->wdgtType;
    sensorStatus = CSD_dsRam.snsStatus[wdgtId];

    switch (widgetType)
    {
    #if (0u != CSD_BUTTON_WIDGET_EN)
        case CSD_WD_BUTTON_E:
            sensorStatus = CSD_DpProcessButton(sensorStatus, ptrFlashWdgt);
            break;
    #endif /* #if (0u != CSD_BUTTON_WIDGET_EN) */

    #if (0u != CSD_CSX_MATRIX_WIDGET_EN)
        case CSD_WD_MATRIX_BUTTON_E:
            sensorStatus = CSD_DpProcessButton(sensorStatus, ptrFlashWdgt);
            break;
    #endif /* #if (0u != CSD_CSX_MATRIX_WIDGET_EN) */

    #if (0u != CSD_CSX_TOUCHPAD_WIDGET_EN)
        case CSD_WD_TOUCHPAD_E:
            sensorStatus = CSD_DpProcessCsxTouchpad(sensorStatus, ptrFlashWdgt);
            break;
    #endif /* #if (0u != CSD_CSX_TOUCHPAD_WIDGET_EN) */

    default:
        CYASSERT(0u);
        break;
    }

    /* Update sensor status in the data structure */
    CSD_dsRam.snsStatus[wdgtId] = (CSD_SNS_STS_TYPE)sensorStatus;

    /* The next two lines calculate index and bitmask of the widget status bit in the wdgtStatus array member */
    wdStatusIndex = CSD_GET_WDGT_STATUS_INDEX(wdgtId);
    wdStatusMask = CSD_GET_WDGT_STATUS_MASK(wdgtId);

    /* Check if there are active sensors and update widget status accordingly */
    if (0u == sensorStatus)
    {
        CSD_dsRam.wdgtStatus[wdStatusIndex] &= ~wdStatusMask;
    }
    else
    {
        CSD_dsRam.wdgtStatus[wdStatusIndex] |= wdStatusMask;
    }
}
#endif /* #if CSD_TOTAL_CSX_WIDGETS */

#if (0u != CSD_TOTAL_CSD_WIDGETS)
/*******************************************************************************
* Function Name: CSD_DpProcessCsdWidgetRawCounts
****************************************************************************//**
*
* \brief
*   Performs default processing for the CSD widget raw counts.
*
* \details
*   Performs default processing for the CSD widget raw counts:
*    - Run Filters
*    - Update Thresholds (FW Auto-tuning)
*    - Update Baselines
*    - Update Differences.
*   The process is applied to all widget's sensors.
*
* \param ptrFlashWdgt The pointer to the Flash Widget Object.
*
* \return   
*  Returns the status of the specified widget processing operation:
*  - CYRET_SUCCESS if operation was successfully completed;
*  - CSD_PROCESS_BASELINE_FAILED if baseline processing of any 
*    sensor of the specified widget failed. The result is concatenated with the index
*    of failed sensor.
*
*******************************************************************************/
uint32 CSD_DpProcessCsdWidgetRawCounts(CSD_FLASH_WD_STRUCT const *ptrFlashWdgt)
{
    uint32  result = CYRET_SUCCESS;
    
    #if (CSD_ENABLE == CSD_TST_BSLN_DUPLICATION_EN)    
        uint32 bslnTstResult = CYRET_SUCCESS;
    #endif /* (CSD_ENABLE == CSD_TST_BSLN_DUPLICATION_EN) */

    uint32  widgetType;
    uint32  snsId;
    CSD_RAM_SNS_STRUCT *ptrSns;
    CSD_RAM_WD_BASE_STRUCT *ptrRamWdgt;

    #if (0u != (CSD_CSD_AUTOTUNE & CSD_CSD_SS_TH_EN))
        /* The noise envelope data is available only when Full CSD Auto-Tuning is enabled */
        SMARTSENSE_CSD_NOISE_ENVELOPE_STRUCT *ptrNoiseEnvelope;
        #if (CSD_ENABLE == CSD_PROXIMITY_WIDGET_EN)
            uint32  proximityThreshold;
        #endif /* (0 != CSD_PROXIMITY_WIDGET_EN) */
    #endif /* #if (0u != (CSD_CSD_AUTOTUNE & CSD_CSD_SS_TH_EN)) */

    #if (0u != DP_FW_FILTER_EN)
        CSD_PTR_FILTER_VARIANT fltrHistV;
    
        /* Determine if widget type is proximity */
        uint32 isProxWdgt = 
                    (CSD_GET_WIDGET_TYPE(ptrFlashWdgt) == CSD_WD_PROXIMITY_E) ? 1u : 0u;
                    
        /* Get pointer to the filter history object */
        fltrHistV.ptr = ptrFlashWdgt->ptr2FltrHistory;
    #endif /* #if (0u != DP_FW_FILTER_EN) */
    
    ptrSns = ptrFlashWdgt->ptr2SnsRam;
    ptrRamWdgt = ptrFlashWdgt->ptr2WdgtRam;
    widgetType = ptrFlashWdgt->wdgtType;

    #if (0u != (CSD_CSD_AUTOTUNE & CSD_CSD_SS_TH_EN))
        ptrNoiseEnvelope = ptrFlashWdgt->ptr2NoiseEnvlp;
    #endif /* #if (0u != (CSD_CSD_AUTOTUNE & CSD_CSD_SS_TH_EN)) */

    /* Iterate through all widget's sensors */
    for(snsId = 0u; snsId < ptrFlashWdgt->totalNumSns; snsId++)
    {
        /* Run all enabled filter on specified widget's sensor */
        #if (0u != DP_FW_FILTER_EN)
            CSD_FtRunEnabledFiltersInternal(fltrHistV, ptrSns, widgetType);
        #endif /* #if (0u != DP_FW_FILTER_EN) */

        /* Run auto-tuning activities */
        #if (0u != (CSD_CSD_AUTOTUNE & CSD_CSD_SS_TH_EN))
            SmartSense_RunNoiseEnvelope(ptrSns->raw[0u], ptrRamWdgt->sigPFC, ptrNoiseEnvelope);
            CSD_DpUpdateThresholds(ptrRamWdgt, ptrNoiseEnvelope, snsId);

            /* Calculate Proximity Touch Threshold in SmartSense mode based on Finger Threshold and PROX_TOUCH_COEFF */
            #if (CSD_ENABLE == CSD_PROXIMITY_WIDGET_EN)
                if (CSD_WD_PROXIMITY_E == CSD_GET_WIDGET_TYPE(ptrFlashWdgt))
                {
                    proximityThreshold = ((uint32)ptrRamWdgt->fingerTh * CSD_PROX_TOUCH_COEFF) / PERCENTAGE_100;
                    if ((((1Lu) << CSD_THRESHOLD_SIZE) - 1u) < proximityThreshold)
                    {
                        proximityThreshold = ((1Lu) << CSD_THRESHOLD_SIZE) - 1u;
                    }
                    ((CSD_RAM_WD_PROXIMITY_STRUCT *)ptrFlashWdgt->ptr2WdgtRam)->proxTouchTh = 
                        (CSD_THRESHOLD_TYPE)proximityThreshold;
                }
            #endif /* (0 != CSD_PROXIMITY_WIDGET_EN) */
            
            /* Move to next noise-envelope object */
            ptrNoiseEnvelope++;
        #endif /* (0u != (CSD_CSD_AUTOTUNE & CSD_CSD_SS_TH_EN)) */

        #if (CSD_ENABLE == CSD_TST_BSLN_DUPLICATION_EN)
            bslnTstResult = CSD_FtUpdateBaseline(ptrRamWdgt, ptrSns, widgetType);
            if (CYRET_SUCCESS != bslnTstResult)
            {
                if (CYRET_SUCCESS == result)
                {
                    result = snsId | CSD_PROCESS_BASELINE_FAILED;
                }
            }
        #else /* (CSD_ENABLE == CSD_TST_BSLN_DUPLICATION_EN) */
            result = CSD_FtUpdateBaseline(ptrRamWdgt, ptrSns, widgetType);
        #endif /* (CSD_ENABLE == CSD_TST_BSLN_DUPLICATION_EN) */

        CSD_DpUpdateDifferences(ptrRamWdgt, ptrSns);

        /* Move to next sensor object */
        ptrSns++;

        #if (0u != DP_FW_FILTER_EN)
            /* Move to the next filter history object */
            CSD_INC_FLTR_OBJ_VARIANT(isProxWdgt, fltrHistV);
        #endif /* #if (0u != DP_FW_FILTER_EN) */
    }
    return result;
}


/*******************************************************************************
* Function Name: CSD_DpProcessCsdSensorRawCountsExt
****************************************************************************//**
*
* \brief
*   Performs customized processing of the CSD sensor raw counts.
*
* \details
*   Performs customized processing of the CSD sensor raw counts.
*
*   Note that this API updates data associated only with the sensor. It cannot be used to update
*   widget status.
*
* \param ptrFlashWdgt The pointer to the Flash widget structure.
* \param ptrSns       The pointer to the sensor object in RAM.
* \param fltrHistV    The pointer to the Filter History Object in RAM.
* \param mode         The bit-mask with the data processing tasks to be executed.
*   The mode parameters can take the following values:
*    - CSD_PROCESS_FILTER     (0x01) Run Firmware Filter
*    - CSD_PROCESS_BASELINE   (0x02) Update Baselines
*    - CSD_PROCESS_DIFFCOUNTS (0x04) Update Difference Counts
*    - CSD_PROCESS_CALC_NOISE (0x08) Calculate Noise (only if FW Tuning is enabled)
*    - CSD_PROCESS_THRESHOLDS (0x10) Update Thresholds (only if FW Tuning is enabled)
*    - CSD_PROCESS_ALL               Execute all tasks
*
* \return   
*  Returns the status of the specified sensor processing operation:
*  - CYRET_SUCCESS if operation was successfully completed;
*  - CSD_PROCESS_BASELINE_FAILED if baseline processing of any 
*    sensor of the specified widget failed. The result is concatenated with the index
*    of failed sensor.
*
*******************************************************************************/
uint32 CSD_DpProcessCsdSensorRawCountsExt(
                CSD_FLASH_WD_STRUCT const *ptrFlashWdgt,
                CSD_RAM_SNS_STRUCT *ptrSns,
                CSD_PTR_FILTER_VARIANT fltrHistV,
                 uint32 mode)
{
    uint32 result = CYRET_SUCCESS;
    uint32  widgetType;
    CSD_RAM_WD_BASE_STRUCT *ptrRamWdgt;

    #if (0u != (CSD_CSD_AUTOTUNE & CSD_CSD_SS_TH_EN))
        SMARTSENSE_CSD_NOISE_ENVELOPE_STRUCT *ptrNoiseEnvelope;
        #if (CSD_ENABLE == CSD_PROXIMITY_WIDGET_EN)
            uint32  proximityThreshold;
        #endif /* (0 != CSD_PROXIMITY_WIDGET_EN) */

        /* Need to specify pointer to corresponding sensor's noise-envelope object.*/
        ptrNoiseEnvelope = ptrFlashWdgt->ptr2NoiseEnvlp;
    #endif /* (0u != (CSD_CSD_AUTOTUNE & CSD_CSD_SS_TH_EN)) */

    widgetType = ptrFlashWdgt->wdgtType;
    ptrRamWdgt = ptrFlashWdgt->ptr2WdgtRam;

    #if (0u != DP_FW_FILTER_EN)
        if (0u != (mode & CSD_PROCESS_FILTER))
        {
            CSD_FtRunEnabledFiltersInternal(fltrHistV, ptrSns, widgetType);
        }
    #else
        (void)fltrHistV; /* This parameter is unused in such configurations */
    #endif /* #if (0u != DP_FW_FILTER_EN) */

    #if (0u != (CSD_CSD_AUTOTUNE & CSD_CSD_SS_TH_EN))
        if (0u != (mode & CSD_PROCESS_CALC_NOISE))
        {
            SmartSense_RunNoiseEnvelope(ptrSns->raw[0u], ptrRamWdgt->sigPFC, ptrNoiseEnvelope);
        }

        if (0u != (mode & CSD_PROCESS_THRESHOLDS))
        {            
            CSD_DpUpdateThresholds(ptrRamWdgt, ptrNoiseEnvelope, 0u);
            
            /* Calculate Proximity Touch Threshold in SmartSense mode based on Finger Threshold and PROX_TOUCH_COEFF */
            #if (CSD_ENABLE == CSD_PROXIMITY_WIDGET_EN)
                if (CSD_WD_PROXIMITY_E == CSD_GET_WIDGET_TYPE(ptrFlashWdgt))
                {
                    proximityThreshold = ((uint32)ptrRamWdgt->fingerTh * CSD_PROX_TOUCH_COEFF) / PERCENTAGE_100;
                    if ((((1Lu) << CSD_THRESHOLD_SIZE) - 1u) < proximityThreshold)
                    {
                        proximityThreshold = ((1Lu) << CSD_THRESHOLD_SIZE) - 1u;
                    }
                    ((CSD_RAM_WD_PROXIMITY_STRUCT *)ptrFlashWdgt->ptr2WdgtRam)->proxTouchTh = 
                        (CSD_THRESHOLD_TYPE)proximityThreshold;
                }
            #endif /* (0 != CSD_PROXIMITY_WIDGET_EN) */
        }
    #endif /* #if (0u != (CSD_CSD_AUTOTUNE & CSD_CSD_SS_TH_EN)) */

    if (0u != (mode & CSD_PROCESS_BASELINE))
    {
        result = CSD_FtUpdateBaseline(ptrRamWdgt, ptrSns, widgetType);
    }

    if (0u != (mode & CSD_PROCESS_DIFFCOUNTS))
    {
        CSD_DpUpdateDifferences(ptrRamWdgt, ptrSns);
    }
    return result;
}

/*******************************************************************************
* Function Name: CSD_DpProcessCsdWidgetStatus
****************************************************************************//**
*
* \brief
*  Runs the status processing of the CSD widget and updates it in the Data Structure.
*
* \details
*  This API determines the widget type and calls the appropriate low-level processing
*  function. Based on the returned results it updates the widget and sensor
*  status in the data structure. Any widget specific registers like the centroid
*  position are updated directly in the low-level processing function.
*
*  When the widget-specific processing completes this API updates the snsStatus
*  and wdgtStatus registers in the data structure. The wdgtStatus register
*  is a bitmask were each bit corresponds to the specific widget identified by
*  the widget index. When the bit is set the widget is active (touched);
*
*  The snsStatus register for CSD widgets contains status of each widget sensor
*  (active/non-active). Each bit corresponds to one sensor. In case of the
*  Proximity widget two bits are used per sensor: one for proximity status and
*  second for the touch status.
*
* \param wdgtId        The widget index number.
* \param ptrFlashWdgt  The pointer to the Flash widget structure.
*
*******************************************************************************/
void CSD_DpProcessCsdWidgetStatus(uint32 wdgtId, CSD_FLASH_WD_STRUCT const *ptrFlashWdgt)
{
    uint32 sensorStatus;
    uint32 wdStatusIndex;
    uint32 wdStatusMask;
    CSD_WD_TYPE_ENUM widgetType;

    widgetType = (CSD_WD_TYPE_ENUM)ptrFlashWdgt->wdgtType;
    sensorStatus = CSD_dsRam.snsStatus[wdgtId];

    switch (widgetType)
    {
    #if (0u != CSD_BUTTON_WIDGET_EN)
        case CSD_WD_BUTTON_E:
            sensorStatus = CSD_DpProcessButton(sensorStatus, ptrFlashWdgt);
            break;
    #endif /* #if (0u != CSD_BUTTON_WIDGET_EN) */

    #if (0u != CSD_SLIDER_WIDGET_EN)
        case CSD_WD_LINEAR_SLIDER_E:
        case CSD_WD_RADIAL_SLIDER_E:
            sensorStatus = CSD_DpProcessSlider(sensorStatus, ptrFlashWdgt);
            break;
    #endif /* #if (0u != CSD_SLIDER_WIDGET_EN) */

    #if (0u != CSD_CSD_MATRIX_WIDGET_EN)
        case CSD_WD_MATRIX_BUTTON_E:
            sensorStatus = CSD_DpProcessCsdMatrix(sensorStatus, ptrFlashWdgt);
            break;
    #endif /* #if (0u != CSD_CSD_MATRIX_WIDGET_EN) */

    #if (0u != CSD_CSD_TOUCHPAD_WIDGET_EN)
        case CSD_WD_TOUCHPAD_E:
            sensorStatus = CSD_DpProcessCsdTouchpad(sensorStatus, ptrFlashWdgt);
            break;
    #endif /* #if (0u != CSD_CSD_TOUCHPAD_WIDGET_EN) */

    #if (0u != CSD_PROXIMITY_WIDGET_EN)
        case CSD_WD_PROXIMITY_E:
            sensorStatus = CSD_DpProcessProximity(sensorStatus, ptrFlashWdgt);
            break;
    #endif /* #if (0u != CSD_PROXIMITY_WIDGET_EN) */

    default:
        CYASSERT(0u);
        break;
    }

    CSD_dsRam.snsStatus[wdgtId] = (CSD_SNS_STS_TYPE)sensorStatus;

    /* The next two lines calculate index and bitmask of the widget status bit in the wdgtStatus array member */
    wdStatusIndex = CSD_GET_WDGT_STATUS_INDEX(wdgtId);
    wdStatusMask = CSD_GET_WDGT_STATUS_MASK(wdgtId);

    /* Check if there are active sensors and update widget status accordingly */
    if (0u == sensorStatus)
    {
        CSD_dsRam.wdgtStatus[wdStatusIndex] &= ~wdStatusMask;
    }
    else
    {
        CSD_dsRam.wdgtStatus[wdStatusIndex] |= wdStatusMask;
    }
}
#endif /* #if (0u != CSD_TOTAL_CSD_WIDGETS) */


#if (0 != (CSD_CSD_AUTOTUNE & CSD_CSD_SS_TH_EN))
/*******************************************************************************
* Function Name: CSD_DpUpdateThresholds
****************************************************************************//**
*
* \brief
*   Updates noise and finger thresholds for a specified widget.
*
* \details
*   Updates noise and finger thresholds for a specified widget.
*   This algorithm is a part of Auto-tuning feature.
*
* \param ptrRamWdgt       The pointer to the RAM widget object.
* \param ptrNoiseEnvelope The pointer to the noise-envelope object in RAM.
* \param startFlag        The flag indicates when new widget is processed.
*
*******************************************************************************/
void CSD_DpUpdateThresholds(CSD_RAM_WD_BASE_STRUCT *ptrRamWdgt,
                                         SMARTSENSE_CSD_NOISE_ENVELOPE_STRUCT *ptrNoiseEnvelope,
                                         uint32 startFlag)
{
    SMARTSENSE_UPDATE_THRESHOLDS_STRUCT thresholds;    
    SMARTSENSE_UPDATE_THRESHOLDS_STRUCT *ptrThresholds = &thresholds;
    ptrThresholds->fingerTh = ptrRamWdgt->fingerTh;

    SmartSense_UpdateThresholds(ptrNoiseEnvelope, ptrThresholds, ptrRamWdgt->sigPFC, startFlag);

    ptrRamWdgt->fingerTh = ptrThresholds->fingerTh;
    ptrRamWdgt->noiseTh = ptrThresholds->noiseTh;
    ptrRamWdgt->nNoiseTh = ptrThresholds->nNoiseTh;
    ptrRamWdgt->hysteresis = ptrThresholds->hysteresis;    
}
#endif /* #if (0u != (CSD_CSD_AUTOTUNE & CSD_CSD_SS_TH_EN)) */


/*******************************************************************************
* Function Name: CSD_DpUpdateDifferences
****************************************************************************//**
*
* \brief
*   Calculates new difference values.
*   Also, executes the frequency hopping algorithm if it is enabled.
*
* \details
*   This API calculates the difference between the baseline and raw counts.
*   If the difference is positive (raw > baseline), and higher than the widget
*   noise threshold, it is saved into the data structure for further processing.
*   Otherwise the difference is set to zero. The final difference value is saved
*   with the subtracted noise threshold value.
*
*   If the frequency hopping algorithm is enabled, the difference is calculated
*   for the three sets of data available for each sensor. Then
*   the median filter selects the final difference value.
*
* \param ptrRamWdgt The pointer to the RAR Widget Object.
* \param ptrSns     The pointer to the sensor object in RAM.
*
*******************************************************************************/
void CSD_DpUpdateDifferences(
                        CSD_RAM_WD_BASE_STRUCT *ptrRamWdgt,
                        CSD_RAM_SNS_STRUCT *ptrSns)
{
    register int32 diffValue = 0;

    #if (0u != CSD_MULTI_FREQ_SCAN_EN)
        register int32 diffValue_0 = 0;
        register int32 diffValue_2 = 0;
    #endif /* #if (0u != CSD_MULTI_FREQ_SCAN_EN) */

    /* Calculate difference */
    diffValue = (int32)ptrSns->raw[0u] - (int32)ptrSns->bsln[0u];

    #if (0u != CSD_MULTI_FREQ_SCAN_EN)
        diffValue_0 = (int32)ptrSns->raw[1u] - (int32)ptrSns->bsln[1u];
        diffValue_2 = (int32)ptrSns->raw[2u] - (int32)ptrSns->bsln[2u];

        /* Find median and put it into [0u] element */
        if (diffValue_0 > diffValue)
        {
            register int32 swapv = diffValue_0;
            diffValue_0 = diffValue;
            diffValue = swapv;
        }

        if (diffValue > diffValue_2)
        {
            register int32 swapv = diffValue;
            diffValue = diffValue_2;
            diffValue_2 = swapv;
        }

        if (diffValue_0 > diffValue)
        {
            register int32 swapv = diffValue_0;
            diffValue_0 = diffValue;
            diffValue = swapv;
        }
    #endif /* #if (0u != CSD_MULTI_FREQ_SCAN_EN) */

    ptrSns->diff = 0u;

    if (diffValue > (int32)ptrRamWdgt->noiseTh)
    {
        ptrSns->diff = (CSD_THRESHOLD_TYPE) diffValue;
    }
}


#if ((0u != CSD_BUTTON_WIDGET_EN) || (0u != CSD_CSX_MATRIX_WIDGET_EN))
/*******************************************************************************
* Function Name: CSD_DpProcessButton
****************************************************************************//**
*
* \brief
*   Processes the status of the Button widget.
*
* \details
*   This API processes the status of the CSD/CSX Button widget and
*   CSX Matrix Button widget. It applies the hysteresis and de-bounce algorithm
*   to each sensor difference value. This API is expected to be called
*   after each new widget scan. If it is called multiple times for the same
*   scan data, the de-bounce algorithm will not work properly.
*
*   This API does not update any Data Structure registers
*   except the de-bounce counters.
*
* \param currStatus   The current status of the widget sensors.
* \param ptrFlashWdgt The pointer to the Flash Widget Object.
*
* \return Returns the status of the widget's sensors. Bit #0 - corresponds to Sensor #0.
*
*******************************************************************************/
uint32 CSD_DpProcessButton(uint32 currStatus, CSD_FLASH_WD_STRUCT const *ptrFlashWdgt)
{
    uint32 snsCnt;
    uint32 snsMask;
    uint32 newStatus;
    uint32 touchTh;
    CSD_RAM_SNS_STRUCT *ptrSns;
    CSD_RAM_WD_BASE_STRUCT *ptrRamWdgt;
    uint8 * ptrDebounceCnt;

    snsCnt = ptrFlashWdgt->totalNumSns;
    ptrSns = ptrFlashWdgt->ptr2SnsRam;
    ptrRamWdgt = ptrFlashWdgt->ptr2WdgtRam;
    ptrDebounceCnt = ptrFlashWdgt->ptr2DebounceArr;

    /* Go through all widget's sensors */
    snsMask = 1Lu;
    for (; snsCnt-- > 0u; )
    {
        /* Calculate touch threshold taking into account hysteresis
           and current sensor state. */
        touchTh = (0u == (currStatus & snsMask)) ?
                  ((uint32)ptrRamWdgt->fingerTh + ptrRamWdgt->hysteresis) :
                  ((uint32)ptrRamWdgt->fingerTh - ptrRamWdgt->hysteresis);

        /* Determine new potential sensor state */
        newStatus = (ptrSns->diff > touchTh) ? snsMask : 0u;

        /* Sensor state changed ? */
        if (newStatus != (currStatus & snsMask))
        {
            /* Decrement de-bounce counter */
            (*ptrDebounceCnt)--;
        }

        /* This piece of code has two objectives:
           1. Reset de-bounce counter if the sensor state did not change.
              (It is possible that during previous processing cycles sensor
               was active and de-bounce counter was decremented.)
           2. Set new sensor state after successful de-bounce.
         */
        if ((newStatus == (currStatus & snsMask)) || (0u == (*ptrDebounceCnt)))
        {
            /* Set new sensor status */
            currStatus &= ~snsMask;
            currStatus |= newStatus;

            /* Reset de-bounce counter */
            #if (0u != CSD_OFF_DEBOUNCE_EN)
                *ptrDebounceCnt = (0Lu == newStatus) ? ptrRamWdgt->onDebounce : ptrRamWdgt->offDebounce;
            #else
                *ptrDebounceCnt = (0Lu == newStatus) ? ptrRamWdgt->onDebounce : DEFAULT_OFF_DEBOUNCE_VALUE;
            #endif /* #if (0u != CSD_OFF_DEBOUNCE_EN) */
        }
        
        /* Move mask to the next sensor */
        snsMask <<= 1u;
        
        ptrSns++;
        ptrDebounceCnt++;
    }

    return currStatus;
}
#endif /* #if ((0u != CSD_BUTTON_WIDGET_EN) || (0u != CSD_CSX_MATRIX_WIDGET_EN)) */


#if (0u != CSD_PROXIMITY_WIDGET_EN)
/*******************************************************************************
* Function Name: CSD_DpProcessProximity
****************************************************************************//**
*
* \brief
*   Processes the status of the Proximity widget.
*
* \details
*   This API processes the status of the CSD Proximity widget. It applies the
*   hysteresis and debounce algorithm to each sensor difference value.
*   The proximity and touch events are considered independently so debounce and
*   hysteresis are also applied independently.
*
*   This API is expected to be called after each new widget scan. If it is
*   called multiple times for the same scan data the debounce algorithm
*   will not work properly.
*
*   This API does not update any Data Structure registers
*   except the debounce counters.
*
* \param currStatus   The value with the current widget status.
* \param ptrFlashWdgt The pointer to the Flash Widget Object.
*
* \return
*  Returns the status of the proximity sensors. Each sensor is represented by two bits
*  that can take the following values:
*   - 00b not active
*   - 01b proximity is detected (signal is above finger threshold)
*   - 11b finger touch is detected (signal is above the touch threshold)
*
*******************************************************************************/
uint32 CSD_DpProcessProximity(uint32 currStatus, CSD_FLASH_WD_STRUCT const *ptrFlashWdgt)
{
    uint32 currTh;
    uint32 snsCnt;
    uint32 snsMask;
    uint32 newStatus;
    uint32 difference;

    uint8 *ptrDebounceCnt;
    CSD_RAM_SNS_STRUCT *ptrSns;
    CSD_RAM_WD_PROXIMITY_STRUCT *ptrRamWdgt;

    /* Mask for proximity status bits. (All odd bits.) */
    const uint32 proxMask = UINT32_ODD_BITS_CONST;

    ptrSns = ptrFlashWdgt->ptr2SnsRam;
    ptrRamWdgt = ptrFlashWdgt->ptr2WdgtRam;
    ptrDebounceCnt = ptrFlashWdgt->ptr2DebounceArr;

    /* Get number of sensors and multiply it by 2 - proximity and touch statuses per sensor. */
    snsCnt = (uint32)ptrFlashWdgt->totalNumSns << 1u;

    /* Go through all sensor's status bits */
    snsMask = 1Lu;
    for (; snsCnt-- > 0u;)
    {
        difference = ptrSns->diff;

        /* Determine current event (proximity - odd; touch - even) and get appropriate threshold. */
        currTh = ptrRamWdgt->fingerTh;

        if (0u == (snsMask & proxMask))
        {
            currTh = ptrRamWdgt->proxTouchTh;
            ptrSns++;
        }

        /* Calculate finger threshold applying hysteresis based on current sensor state. */
        currTh = (0u == (currStatus & snsMask)) ? (currTh + ptrRamWdgt->hysteresis) : (currTh - ptrRamWdgt->hysteresis);

        /* Determine new potential sensor state before de-bounce */
        newStatus = (difference > currTh) ? snsMask : 0Lu;

        /* Sensor state changed ? */
        if (newStatus != (currStatus & snsMask))
        {
            /* Decrement de-bounce counter */
            (*ptrDebounceCnt)--;
        }

        /* This piece of code has two objectives:
           1. Reset de-bounce counter if new sensor state has not changed.
              (It is possible that during previous processing cycles the sensor
               state was different which caused de-bounce counter change.)
           2. Set new sensor state after successful de-bounce.
         */
        if ((newStatus == (currStatus & snsMask)) || (0u == (*ptrDebounceCnt)))
        {
            /* Set new sensor status */
            currStatus &= ~snsMask;
            currStatus |= newStatus;

            /* Reset de-bounce counter */
            #if (0u != CSD_OFF_DEBOUNCE_EN)
                *ptrDebounceCnt = (0Lu == newStatus) ? ptrRamWdgt->onDebounce : ptrRamWdgt->offDebounce;
            #else
                *ptrDebounceCnt = (0Lu == newStatus) ? ptrRamWdgt->onDebounce : DEFAULT_OFF_DEBOUNCE_VALUE;
            #endif /* #if (0u != CSD_OFF_DEBOUNCE_EN) */
        }

        ptrDebounceCnt++;
        
        /* Move sensor mask to the next sensor */
        snsMask <<= 1u;
    }

    return currStatus;
}
#endif /* #if (0u != CSD_PROXIMITY_WIDGET_EN) */


#if (0u != CSD_SLIDER_WIDGET_EN)
/*******************************************************************************
* Function Name: CSD_DpProcessSlider
****************************************************************************//**
*
* \brief
*   Processes the status of the Linear/Radial Slider widget.
*   The process involves running the Linear/Radial Centroid algorithm.
*
* \details
*   This API processes the status of the CSD Slider widget (Linear and Radial).
*   It applies the hysteresis and de-bounce algorithm to the widget ignoring
*   the individual states of the sensors.
*
*   This API is expected to be called after each new widget scan. If it is
*   called multiple times for the same scan data, the debounce algorithm
*   will not work properly.
*
*   Slider widgets do not support the ON to OFF debounce.
*
*   This API does not update any Data Structure registers
*   except the debounce counters.
*
* \param currStatus   The current sensor status of the Slider widget.
* \param ptrFlashWdgt The pointer to the Flash Widget Object.
*
* \return Returns the status of the widget's sensors. Bit #0 - corresponds to Sensor #0.
*
*******************************************************************************/
uint32 CSD_DpProcessSlider(uint32 currStatus, CSD_FLASH_WD_STRUCT const *ptrFlashWdgt)
{
    uint32 snsCnt;
    uint32 snsMask;
    uint32 touchTh;
    uint32 newStatus = 0Lu;
    uint8 * ptrDebounceCnt;
    CSD_RAM_SNS_STRUCT *ptrSns;
    CSD_RAM_WD_SLIDER_STRUCT *ptrRamWdgt;

    #if (0u != CSD_POS_MEDIAN_FILTER_EN)
        CSD_SLIDER_POS_HISTORY_STRUCT *ptrHistory;
    #endif /* #if (0u != CSD_POS_MEDIAN_FILTER_EN) */

    uint32 touchIdx;
    uint32 touchCnt = 0Lu;
    uint16 newPos[CSD_NUM_CENTROIDS];

    snsCnt = ptrFlashWdgt->totalNumSns;
    ptrSns = ptrFlashWdgt->ptr2SnsRam;
    ptrRamWdgt = ptrFlashWdgt->ptr2WdgtRam;
    ptrDebounceCnt = ptrFlashWdgt->ptr2DebounceArr;
    
    #if (0u != CSD_POS_MEDIAN_FILTER_EN)
        ptrHistory = ptrFlashWdgt->ptr2PosHistory;
    #endif /* #if (0u != CSD_POS_MEDIAN_FILTER_EN) */

    /* Calculate touch threshold based on current slider state */
    touchTh = (0Lu == currStatus ) ?
              ((uint32)ptrRamWdgt->fingerTh + ptrRamWdgt->hysteresis) :
              ((uint32)ptrRamWdgt->fingerTh - ptrRamWdgt->hysteresis);

    /* Go through all widget's sensors */
    snsMask = 1Lu;
    for (; snsCnt-- > 0Lu; )
    {
        /* Determine new potential sensor state */
        newStatus |= (ptrSns->diff > touchTh) ? snsMask : 0Lu;
        ptrSns++;

        snsMask <<= 1u;
    }

    /* If widget is active, check whether debounce is needed */
    if (0Lu != newStatus)
    {
        if (0u < (*ptrDebounceCnt))
        {
            /* Decrement debounce counter */
            (*ptrDebounceCnt)--;
        }

        if (0u == (*ptrDebounceCnt))
        {
            /* Update slider sensor state when debounce completes */
            currStatus = newStatus;
        }
    }
    else
    {
        /* Reset debounce counter */
        *ptrDebounceCnt = ptrRamWdgt->onDebounce;

        /* Clear slider sensor state */
        currStatus = 0Lu;
    }

    /* Centroid processing */

    /* If there are active sensors after debounce run the centroid algorithm */
    if (0u != currStatus)
    {
        /* Centroid always uses the lowest finger threshold (minus hysteresis)
         * to produce same results for first and all consecutive invocations */
        touchTh = (uint32)ptrRamWdgt->fingerTh - ptrRamWdgt->hysteresis;
        snsCnt = ptrFlashWdgt->totalNumSns;

        #if (0u != CSD_TOTAL_DIPLEXED_SLIDERS)
            if ((0u != (ptrFlashWdgt->staticConfig & CSD_DIPLEXING_MASK)))
            {
                /* Run local maximum search for duplexed slider */
                touchCnt = CSD_DpFindLocalMaxDiplex(
                                ptrFlashWdgt->ptr2SnsRam, snsCnt, ptrFlashWdgt->ptr2DiplexTable, touchTh);

                /* Centroid calculation API requires double sensor count for the diplexed sliders. */
                snsCnt *= 2u;
            }
            else
            {
                /* Run local maximum search for NON-duplexed slider */
                touchCnt = CSD_DpFindLocalMaxSd(ptrFlashWdgt->ptr2SnsRam, snsCnt, touchTh);
            }
        #else
            /* Run local maximum search for NON-duplexed slider */
            touchCnt = CSD_DpFindLocalMaxSd(ptrFlashWdgt->ptr2SnsRam, snsCnt, touchTh);
        #endif /* #if (0u != CSD_TOTAL_DIPLEXED_SLIDERS) */
        
        /* If at least one touch is detected */
        if (0u != touchCnt)
        {
            /* Determine slider type and calculate new position(s) */
            #if (0u != CSD_TOTAL_LINEAR_SLIDERS)
                if ((CSD_WD_TYPE_ENUM)ptrFlashWdgt->wdgtType == CSD_WD_LINEAR_SLIDER_E)
                {
                    touchCnt = CSD_DpCalcLinearCentroid(
                                    newPos, (uint32)ptrFlashWdgt->xCentroidMultiplier, snsCnt);

                    DpUpdateLinSlider(newPos, touchCnt, ptrFlashWdgt, ptrRamWdgt);
                }
            #endif /* #if (0u != CSD_TOTAL_LINEAR_SLIDERS) */

            #if (0u != CSD_TOTAL_RADIAL_SLIDERS)
                if ((CSD_WD_TYPE_ENUM)ptrFlashWdgt->wdgtType == CSD_WD_RADIAL_SLIDER_E)
                {
                    touchCnt = CSD_DpCalcRadialCentroid(
                                    newPos, (uint32)ptrFlashWdgt->xCentroidMultiplier, snsCnt);

                    DpUpdateRadSlider(newPos, touchCnt, ptrFlashWdgt, ptrRamWdgt);
                }
            #endif /* #if (0u != CSD_TOTAL_RADIAL_SLIDERS) */
        }
    }
    
    /* Clear not active position data slots */
    for (touchIdx = touchCnt; touchIdx < CSD_NUM_CENTROIDS; touchIdx++)
    {
        ptrRamWdgt->position[touchIdx] = CSD_SLIDER_POS_NONE;

        #if (0u != CSD_POS_MEDIAN_FILTER_EN)
            if (NULL != ptrHistory)
            {
                ptrHistory[touchIdx].posMedianZ1 = CSD_SLIDER_POS_NONE;
                ptrHistory[touchIdx].posMedianZ2 = CSD_SLIDER_POS_NONE;
            }
        #endif /* #if (0u != CSD_POS_MEDIAN_FILTER_EN) */
    }

    return currStatus;
}
#endif /* #if (0u != CSD_SLIDER_WIDGET_EN) */

#if (0u != CSD_TOTAL_LINEAR_SLIDERS)
/*******************************************************************************
* Function Name: DpUpdateLinSlider
****************************************************************************//**
*
* \brief
*   Updates the touch status of the Linear Slider widget.
*
* \details
*   This function updates the position data of the Linear Slider
*   in the Data Structure. Also, this function runs position filters for the
*   position data if any enabled.
*
* \param newPos       The pointer to the array with the new finger position coordinates.
* \param numTouches   The number of the coordinates in the newPos array.
* \param ptrFlashWdgt The pointer to the Flash Widget Object.
* \param ptrRamWdgt   The pointer to the RAM Widget Object.
*
*******************************************************************************/
static CY_INLINE void DpUpdateLinSlider(
                        const uint16 newPos[],
                        uint32 numTouches,
                        CSD_FLASH_WD_STRUCT const *ptrFlashWdgt,
                        CSD_RAM_WD_SLIDER_STRUCT *ptrRamWdgt)
{
    uint32 touchIdx;

    #if (0u == DP_LINEAR_SLIDER_FILTER_EN)
        /* This parameter is unused in such configurations */
        (void)ptrFlashWdgt;
    #else
        /* Get bitmask with the enabled filters */
        uint32 filterConfig = (uint32)ptrFlashWdgt->staticConfig;
        
        #if (0u != CSD_POS_MEDIAN_FILTER_EN)
            /* Get median filter history */
            CSD_SLIDER_POS_HISTORY_STRUCT *ptrHistory = ptrFlashWdgt->ptr2PosHistory;
        #endif /* #if (0u != CSD_POS_MEDIAN_FILTER_EN) */
    #endif /* #if (0u == DP_LINEAR_SLIDER_FILTER_EN) */
    
    /* Run Filter and update the position registers with the filtered data */
    for (touchIdx = 0u; touchIdx < numTouches; touchIdx++)
    {
        #if (0u == DP_LINEAR_SLIDER_FILTER_EN)
            /* If filters are not enabled just copy the position data */
            ptrRamWdgt->position[touchIdx] = newPos[touchIdx];
        #else
            #if (0u != CSD_POS_MEDIAN_FILTER_EN)
                /* Median filter requires position history with the last two values */
                DpFilterLinSlider(newPos[touchIdx], filterConfig, &(ptrRamWdgt->position[touchIdx]), ptrHistory);

                ptrHistory = (NULL != ptrHistory) ? (ptrHistory + 1) : NULL;
            #else
                /* The rest filters work just with previous value hold in  */
                DpFilterLinSlider(newPos[touchIdx], filterConfig, &(ptrRamWdgt->position[touchIdx]), NULL);
            #endif /* #if (0u != CSD_POS_MEDIAN_FILTER_EN) */
        #endif /* #if (0u == DP_LINEAR_SLIDER_FILTER_EN) */
    }
}
#endif /* #if (0u != CSD_TOTAL_LINEAR_SLIDERS) */


#if (0u != DP_LINEAR_SLIDER_FILTER_EN)
/*******************************************************************************
* Function Name: DpFilterLinSlider
****************************************************************************//**
*
* \brief
*   Filters the Linear Slider widget position.
*
* \details
*   Runs the position filter for the Linear Slider or CSD touchpad axis data.
*   The filtered position data is written by the address pointed by the ptrCurrPos.
*
* \param newPos       A new coordinate value.
* \param staticConfig The static configuration of the widget.
                          Includes information on the enabled filters.
* \param ptrCurrPos   The pointer to the position register in the data structure.
* \param ptrHistory   The pointer to the structure that holds previous historical position value.
*
*******************************************************************************/
static CY_INLINE void DpFilterLinSlider(
                        uint16 newPos, uint32 staticConfig, uint16 *ptrCurrPos,
                        CSD_SLIDER_POS_HISTORY_STRUCT *ptrHistory)
{
    uint32 newPosition;
    uint32 currPosition;

    #if (0u != CSD_POS_MEDIAN_FILTER_EN)
        uint32 medianPosition;
    #else
        (void)ptrHistory; /* This parameter is unused in such configurations */
    #endif /* #if (0u != CSD_POS_MEDIAN_FILTER_EN) */

    /* extend new and current position data to UINT32 */
    newPosition  = newPos;
    currPosition = *ptrCurrPos;

    /* If there were no touch initialize the current position with new position data  */
    if (currPosition == CSD_SLIDER_POS_NONE)
    {
        currPosition = LO16(newPosition);
    }

    /* Filter new slider position using the configured filter types */
    #if (0u != CSD_POS_IIR_FILTER_EN)
        if (0u != (staticConfig & CSD_POS_IIR_FILTER_MASK))
        {
            newPosition = CSD_FtIIR1stOrder(newPosition, currPosition, CSD_POS_IIR_COEFF, 0u);
        }
    #endif /* #if (0u != CSD_POS_IIR_FILTER_EN) */

    #if (0u != CSD_POS_MEDIAN_FILTER_EN)
        if (0u != (staticConfig & CSD_POS_MEDIAN_FILTER_MASK))
        {
            CYASSERT(ptrHistory); /* ptrHistory cannot be NULL if median filter is enabled*/

            /* Initialize previousPosition if it is NONE */
            if (ptrHistory->posMedianZ1 == CSD_SLIDER_POS_NONE)
            {
                ptrHistory->posMedianZ1 = LO16(newPosition);
                ptrHistory->posMedianZ2 = LO16(newPosition);
            }

            /* Run median filter */
            medianPosition = CSD_FtMedian(
                                    (uint32)ptrHistory->posMedianZ2, (uint32)ptrHistory->posMedianZ1, newPosition);

            /* Update history */
            ptrHistory->posMedianZ2 = ptrHistory->posMedianZ1;
            ptrHistory->posMedianZ1 = LO16(newPosition);

            newPosition = medianPosition;
        }
    #endif /* #if (0u != CSD_POS_MEDIAN_FILTER_EN) */

    #if (0u != CSD_POS_AVERAGE_FILTER_EN)
        if (0u != (staticConfig & CSD_POS_AVERAGE_FILTER_MASK))
        {
            /* Run average filter */
            newPosition = (currPosition + newPosition) >> 1u;
        }
    #endif /* #if (0u != CSD_POS_AVERAGE_FILTER_EN) */

    #if (0u != CSD_POS_JITTER_FILTER_EN)
        if (0u != (staticConfig & CSD_POS_JITTER_FILTER_MASK))
        {
            newPosition = CSD_FtJitter(newPosition, currPosition);
        }
    #endif /* #if (0u != CSD_POS_JITTER_FILTER_EN)  */

    /* Update position data */
    *ptrCurrPos = LO16(newPosition);
}
#endif /* #if (0u != DP_LINEAR_SLIDER_FILTER_EN) */


#if (0u != CSD_TOTAL_RADIAL_SLIDERS)
/*******************************************************************************
* Function Name: DpUpdateRadSlider
****************************************************************************//**
*
* \brief
*   Filters the Radial Slider widget position.
*
* \details
*   This function updates the position data of the radial slider
*   in the Data Structure. Also, this function runs position filters for the
*   position data if any enabled.
*
*   The filtering algorithm takes into account that it is possible that new value
*   will cross the zero position so it adjusts the position data appropriately to
*   get the correct filtered value.
*
* \param newPos[]     The pointer to the array with new centroid position data.
* \param numTouches   The number of elements in the newPos array.
* \param ptrFlashWdgt The pointer to the Flash Widget Object.
* \param ptrRamWdgt   The pointer to the RAM Widget Object .
*
*******************************************************************************/
static CY_INLINE void DpUpdateRadSlider(
                        const uint16 newPos[], uint32 numTouches,
                        CSD_FLASH_WD_STRUCT const *ptrFlashWdgt,
                        CSD_RAM_WD_SLIDER_STRUCT *ptrRamWdgt)
{
    uint32 posNum;
    uint32 currPosition;
    uint32 newPosition;

    #if (0u != CSD_POS_MEDIAN_FILTER_EN)
        uint32 medianPosition;
        CSD_SLIDER_POS_HISTORY_STRUCT *ptrHistory;

        ptrHistory = ptrFlashWdgt->ptr2PosHistory;
    #endif /* #if (0u != CSD_POS_MEDIAN_FILTER_EN) */

    for (posNum = 0u; posNum < numTouches; posNum++)
    {
        /* Get the current and new position data */
        newPosition  = (uint32)newPos[posNum];
        currPosition = (uint32)ptrRamWdgt->position[posNum];

        /* The current position data is used as previous value for the filters so if it is NONE set it to new value */
        if (currPosition == CSD_SLIDER_POS_NONE)
        {
            currPosition = newPosition;
        }

        /* If new position crosses zero point in one or another direction the position variable with smaller value
         * is increased by the slider resolution. It is done for the proper filtering. For example,
         * yResolution = 100, currPosition = 95, newPosition = 5. If no actions are taken than the average filter
         * will give value 50 - which is wrong. But if the position values are adusted as mentioned here we will get
         * newPosition eqaul 105 and the average will be 100. Later this filtered value will be adusted further to not
         * cross the yResolution and it will end up with 0u - which is correct average result for the provided example.
         */
        if (currPosition > newPosition)
        {
            if (((uint32)ptrFlashWdgt->xResolution >> 1u) < (currPosition - newPosition))
            {
                newPosition += (uint32)ptrFlashWdgt->xResolution;
            }
        }
        else
        {
            if (((uint32)ptrFlashWdgt->xResolution >> 1u) < (newPosition - currPosition))
            {
                currPosition += (uint32)ptrFlashWdgt->xResolution;
            }
        }

        /* Filter new slider position */
        #if (0u != CSD_POS_IIR_FILTER_EN)
            if (0u != (ptrFlashWdgt->staticConfig & CSD_POS_IIR_FILTER_MASK))
            {
                newPosition = CSD_FtIIR1stOrder(
                                    newPosition, currPosition, CSD_POS_IIR_COEFF, 0u);
            }
        #endif /* #if (0u != CSD_POS_IIR_FILTER_EN) */

        #if (0u != CSD_POS_MEDIAN_FILTER_EN)
            if (0u != (ptrFlashWdgt->staticConfig & CSD_POS_MEDIAN_FILTER_MASK))
            {
                /* Adjust the median filter history values if needed */
                if (CSD_SLIDER_POS_NONE == ptrHistory[posNum].posMedianZ1)
                {
                    ptrHistory[posNum].posMedianZ1 = LO16(newPosition);
                    ptrHistory[posNum].posMedianZ2 = LO16(newPosition);
                }
                else if (ptrHistory[posNum].posMedianZ1 < newPosition)
                {
                    if ((newPosition - ptrHistory[posNum].posMedianZ1) > ((uint32)ptrFlashWdgt->xResolution >> 1u))
                    {
                        ptrHistory[posNum].posMedianZ1 += ptrFlashWdgt->xResolution;
                        ptrHistory[posNum].posMedianZ2 += ptrFlashWdgt->xResolution;
                    }
                }
                else
                {
                    if ((ptrHistory[posNum].posMedianZ1 - newPosition) > ((uint32)ptrFlashWdgt->xResolution >> 1u))
                    {
                        newPosition += (uint32)ptrFlashWdgt->xResolution;
                    }
                }

                medianPosition = CSD_FtMedian((uint32)ptrHistory[posNum].posMedianZ2, 
                                                           (uint32)ptrHistory[posNum].posMedianZ1, 
                                                            newPosition);

                /* Update the median filter history */
                ptrHistory[posNum].posMedianZ2 = ptrHistory[posNum].posMedianZ1;
                ptrHistory[posNum].posMedianZ1 = LO16(newPosition);

                newPosition = medianPosition;
            }
        #endif /* #if (0u != CSD_POS_MEDIAN_FILTER_EN) */

        #if (0u != CSD_POS_AVERAGE_FILTER_EN)
            if (0u != (ptrFlashWdgt->staticConfig & CSD_POS_AVERAGE_FILTER_MASK))
            {
                /* Run the average filter */
                newPosition = (currPosition + newPosition) >> 1u;
            }
        #endif /* #if (0u != CSD_POS_AVERAGE_FILTER_EN) */

        #if (0u != CSD_POS_JITTER_FILTER_EN)
            if (0u != (ptrFlashWdgt->staticConfig & CSD_POS_JITTER_FILTER_MASK))
            {
                newPosition = CSD_FtJitter(newPosition, currPosition);
            }
        #endif /* #if (0u != CSD_POS_JITTER_FILTER_EN) */

        /* Update position value in Data Structure */
        if (newPosition >= (uint32)ptrFlashWdgt->xResolution)
        {
            newPosition -= (uint32)ptrFlashWdgt->xResolution;
        }

        ptrRamWdgt->position[posNum] = LO16(newPosition);
    }
}
#endif /* #if (0u != CSD_TOTAL_RADIAL_SLIDERS) */


#if (0u != CSD_CSD_MATRIX_WIDGET_EN)
/*******************************************************************************
* Function Name: CSD_DpProcessCsdMatrix
****************************************************************************//**
*
* \brief
*   Processes the status of the CSD Matrix Button widget.
*
* \details
*   This API processes the status of the CSD Matrix Button widget.
*   It applies the hysteresis and debounce algorithm to each sensor value.
*
*   Then the API analyzes how many row and column sensors are active. If only
*   one per row and one per column, the API considers this as a valid touch and
*   updates the corresponding Data Structure registers with the button id and active
*   row and column sensors.
*
*   If multiple sensors are active in row or column sensors, this API sets
*   the corresponding Data Structure registers to the CSD_MATRIX_POS_MULTI
*   value which indicates that it is not possible to detect the touched button id.
*
*   This API is expected to be called after each new widget scan. If it is
*   called multiple times for the same scan data, the debounce algorithm
*   will not work properly.
*
* \param currStatus   The current status of the widget sensors.
* \param ptrFlashWdgt The pointer to the Flash Widget Object.
*
* \return Returns the status of the widget's sensors. The row sensors occupy the
*         least significant bits and column sensors - the most significant bits.
*         Bit #0 - corresponds to Row Sensor #0.
*
*******************************************************************************/
uint32 CSD_DpProcessCsdMatrix(uint32 currStatus, CSD_FLASH_WD_STRUCT const *ptrFlashWdgt)
{
    uint32 snsId;
    uint32 snsMask;
    uint32 newStatus;
    uint32 touchTh;
    uint8 * ptrDebounceCnt;
    CSD_RAM_SNS_STRUCT *ptrSns;
    CSD_RAM_WD_CSD_MATRIX_STRUCT *ptrRamWdgt;

    uint32 numActiveRows = 0u;
    uint32 numActiveCols = 0u;

    uint32 activeRow = CSD_MATRIX_POS_NONE;
    uint32 activeCol = CSD_MATRIX_POS_NONE;

    ptrSns = ptrFlashWdgt->ptr2SnsRam;
    ptrRamWdgt = ptrFlashWdgt->ptr2WdgtRam;
    ptrDebounceCnt = ptrFlashWdgt->ptr2DebounceArr;

    /* Go through all widget's sensors */
    for (snsId = 0u; snsId < ptrFlashWdgt->totalNumSns; snsId++)
    {
        /* calculate current sensor mask */
        snsMask = 1Lu << snsId;

        /* Calculate touch threshold taking into account hysteresis and current sensor state. */
        touchTh = (0u == (currStatus & snsMask)) ?
                  ((uint32)ptrRamWdgt->fingerTh + ptrRamWdgt->hysteresis) :
                  ((uint32)ptrRamWdgt->fingerTh - ptrRamWdgt->hysteresis);

        /* Determine new potential sensor state */
        newStatus = (ptrSns->diff > touchTh) ? snsMask : 0u;

        /* Sensor state changed ? */
        if (newStatus != (currStatus & snsMask))
        {
            /* Decrement debounce counter */
            (*ptrDebounceCnt)--;
        }

        /* This piece of code has two objectives:
         * 1. Reset debounce counter if new sensor state has not changed.
         *    (It is possible that during previous processing cycles sensor state
         *     was changing which caused decrement of debounce counter.)
         * 2. Set new sensor state after successful debounce.
         */
        if ((newStatus == (currStatus & snsMask)) || (0u == (*ptrDebounceCnt)))
        {
            /* Set new sensor status */
            currStatus &= ~snsMask;
            currStatus |= newStatus;

            if (0u == newStatus)
            {
                /* Reset OFF->ON debounce counter */
                *ptrDebounceCnt = ptrRamWdgt->onDebounce;
            }
            else
            {
                /* Reset ON->OFF debounce counter */
                #if (0u != CSD_OFF_DEBOUNCE_EN)
                    *ptrDebounceCnt = ptrRamWdgt->offDebounce;
                #else
                    *ptrDebounceCnt = DEFAULT_OFF_DEBOUNCE_VALUE;
                #endif /* #if (0u != CSD_OFF_DEBOUNCE_EN) */
            }
        }

        /* Update information about active row/col sensors */
        if (0u != (currStatus & snsMask))
        {
            if (snsId < ptrFlashWdgt->numCols)
            {
                numActiveCols++;
                activeCol = snsId;
            }
            else
            {
                numActiveRows++;
                activeRow = snsId - ptrFlashWdgt->numCols;
            }
        }

        ptrSns++;
        ptrDebounceCnt++;
    }

    /* Update row and col registers in data structure */
    ptrRamWdgt->posCol = (numActiveCols > 1u) ? CSD_MATRIX_POS_MULTI : (uint8)activeCol;
    ptrRamWdgt->posRow = (numActiveRows > 1u) ? CSD_MATRIX_POS_MULTI : (uint8)activeRow;

    /* If only one row and one column sensors are active update the sensor ID register */
    if ((1u == numActiveRows) && (1u == numActiveCols))
    {
        ptrRamWdgt->posSnsId = LO8((activeRow * ptrFlashWdgt->numCols) + activeCol);
    }
    else
    {
        ptrRamWdgt->posSnsId = CSD_MATRIX_POS_NONE;
    }

    return currStatus;
}
#endif /* #if (0u != CSD_CSD_MATRIX_WIDGET_EN) */


#if (0u != CSD_CSD_TOUCHPAD_WIDGET_EN)
/*******************************************************************************
* Function Name: CSD_DpProcessCsdTouchpad
****************************************************************************//**
*
* \brief
*   Processes status of the CSD Touchpad widget. This includes running
*   Centroid algorithm and updating status in the Data Structure registers.
*
* \details
*   Processes status of the CSD Touchpad widget. This includes running
*   Centroid algorithm and updating status in the Data Structure registers.
*   The status is updated in the RAM data structure.
*
* \param currStatus   The current status of the widget sensors.
* \param ptrFlashWdgt The pointer to the Flash Widget Object.
*
* \return Returns the status of the widget's sensors. The row sensors occupy the
*         least significant bits and column sensors - the most significant bits.
*         Bit #0 - corresponds to Row Sensor #0.
*
*******************************************************************************/
uint32 CSD_DpProcessCsdTouchpad(uint32 currStatus, CSD_FLASH_WD_STRUCT const *ptrFlashWdgt)
{
    uint32 snsCnt;
    uint32 snsMask;
    uint32 touchTh;
    uint32 newStatus = 0Lu;
    uint8 * ptrDebounceCnt;
    CSD_RAM_SNS_STRUCT *ptrSns;
    CSD_RAM_WD_CSD_TOUCHPAD_STRUCT *ptrRamWdgt;

    uint32 numLocalMax;
    uint16 newPosX;
    uint16 newPosY;

    uint32 rowSnsMask;
    uint32 colSnsMask;
    
    #if (0u != DP_POSITION_FILTER_EN)
        uint32 filterConfig;
    #endif /* #if (0u != DP_POSITION_FILTER_EN) */

    #if (0u != CSD_POS_MEDIAN_FILTER_EN)
        CSD_TOUCHPAD_POS_HISTORY_STRUCT *ptrHistory;

        ptrHistory = ptrFlashWdgt->ptr2PosHistory;
    #endif /* #if (0u != CSD_POS_MEDIAN_FILTER_EN) */

    snsCnt = ptrFlashWdgt->totalNumSns;
    ptrSns = ptrFlashWdgt->ptr2SnsRam;
    ptrRamWdgt = ptrFlashWdgt->ptr2WdgtRam;
    ptrDebounceCnt = ptrFlashWdgt->ptr2DebounceArr;

    /* Build masks for status word of row and column sensors */
    colSnsMask = (1Lu << ptrFlashWdgt->numCols) - 1Lu;
    rowSnsMask = (1Lu << ptrFlashWdgt->numRows) - 1Lu;
    rowSnsMask <<= ptrFlashWdgt->numCols;

    /* Calculate touch threshold based on current touchpad state */
    touchTh = (0Lu == currStatus) ?
              ((uint32)ptrRamWdgt->fingerTh + ptrRamWdgt->hysteresis) :
              ((uint32)ptrRamWdgt->fingerTh - ptrRamWdgt->hysteresis);

    /* Go through all widget's sensors */
    snsMask = 1Lu;
    for (; snsCnt-- > 0Lu;)
    {
        /* Determine new potential sensor state */
        newStatus |= (ptrSns->diff > touchTh) ? snsMask : 0Lu;
        ptrSns++;
        
        snsMask <<= 1u;
    }

    /* If widget is active perform the debounce.
     * Widget is considered as active if at least one sensor is active on both axes.
     */
    if (((newStatus & rowSnsMask) > 0u) && ((newStatus & colSnsMask) > 0u))
    {
        if (0u < (*ptrDebounceCnt))
        {
            /* Decrement debounce counter */
            (*ptrDebounceCnt)--;
        }

        if (0u == (*ptrDebounceCnt))
        {
            /* Update slider sensor state when debounce completes */
            currStatus = newStatus;
        }
    }
    else
    {
        /* If widget is not active: */

        /* Reset debounce counter */
        *ptrDebounceCnt = ptrRamWdgt->onDebounce;

        /* Reset current sensor state */
        currStatus = 0Lu;

        /* Clear centroid position data */
        ptrRamWdgt->posX = CSD_TOUCHPAD_POS_NONE;
        ptrRamWdgt->posY = CSD_TOUCHPAD_POS_NONE;

        /* Clear history data if available */
        #if (0u != CSD_POS_MEDIAN_FILTER_EN)
            if (NULL != ptrHistory)
            {
                ptrHistory->xPos.posMedianZ1 = CSD_TOUCHPAD_POS_NONE;
                ptrHistory->xPos.posMedianZ2 = CSD_TOUCHPAD_POS_NONE;
                ptrHistory->yPos.posMedianZ1 = CSD_TOUCHPAD_POS_NONE;
                ptrHistory->yPos.posMedianZ2 = CSD_TOUCHPAD_POS_NONE;
            }
        #endif /* #if (0u != CSD_POS_MEDIAN_FILTER_EN) */
    }

    /* If widget is still active after debounce run the centroid algorithm */
    if (0Lu != currStatus)
    {
        /* Centroid processing */
        ptrSns = ptrFlashWdgt->ptr2SnsRam;
        snsCnt = ptrFlashWdgt->totalNumSns;
        touchTh = (uint32)ptrRamWdgt->fingerTh - ptrRamWdgt->hysteresis;

        /* Run local maximum search for Column sensors of touchpad */
        numLocalMax = CSD_DpFindLocalMaxSd(ptrSns, (uint32)ptrFlashWdgt->numCols, touchTh);

        CYASSERT(numLocalMax); /* There must be at least one local maximum found */

        /* Verify how many local maximums are found on X axis.
         * The CSD Touchpads support only one finger touch so we should have only one local maximum per axis.
         * If more are found then there are multiple finger touches and it is not possible to recover their position.
         */
        if (1u == numLocalMax)
        {
            /* Run centroid algorithm for the X axis */
            (void)CSD_DpCalcLinearCentroid(
                        &newPosX, (uint32)ptrFlashWdgt->xCentroidMultiplier, (uint32)ptrFlashWdgt->numCols);

            /* Get pointer to Row RAM sensor structure array */
            ptrSns += ptrFlashWdgt->numCols;

            /* Run local maximum search for Row sensors of touchpad*/
            numLocalMax = CSD_DpFindLocalMaxSd(ptrSns, (uint32)ptrFlashWdgt->numRows, touchTh);

            CYASSERT(numLocalMax); /* There must be at least one local maximum found */

            /* Verify how many local maximums are found on Y axis.
             * The CSD Touchpads support only one finger touch so we should have only one local maximum per axis.
             * If more are found then it is not possible to recover finger position.
             */
            if (1u == numLocalMax)
            {
                /* Run centroid algorithm for the Y axis */
                (void)CSD_DpCalcLinearCentroid(
                            &newPosY, (uint32)ptrFlashWdgt->yCentroidMultiplier, (uint32)ptrFlashWdgt->numRows);

                /* Now filter the X and Y position data if needed */
                #if (0u != DP_POSITION_FILTER_EN)
                    /* Position filter configuration is stored in the widget static config register */
                    filterConfig = (uint32)ptrFlashWdgt->staticConfig;

                    #if (0u != CSD_POS_MEDIAN_FILTER_EN)
                        /* Run filter for X position */
                        DpFilterLinSlider(newPosX, filterConfig, &ptrRamWdgt->posX, 
                            (NULL != ptrHistory) ? &(ptrHistory->xPos) : NULL);

                        /* Run filter for Y position */
                        DpFilterLinSlider(newPosY, filterConfig, &ptrRamWdgt->posY, 
                            (NULL != ptrHistory) ? &(ptrHistory->yPos) : NULL);
                    #else
                        /* Run filter for X position */
                        DpFilterLinSlider(newPosX, filterConfig, &ptrRamWdgt->posX, NULL);

                        /* Run filter for Y position */
                        DpFilterLinSlider(newPosY, filterConfig, &ptrRamWdgt->posY, NULL);
                    #endif /* #if (0u != CSD_POS_MEDIAN_FILTER_EN) */
                #else
                    ptrRamWdgt->posX = newPosX;
                    ptrRamWdgt->posY = newPosY;
                #endif /* #if (0u != DP_POSITION_FILTER_EN) */
            }
        }

        if (numLocalMax > 1u)
        {
            /* Clear centroid position data */
            ptrRamWdgt->posX = CSD_TOUCHPAD_POS_NONE;
            ptrRamWdgt->posY = CSD_TOUCHPAD_POS_NONE;

            /* Clear history data if available */
            #if (0u != CSD_POS_MEDIAN_FILTER_EN)
                if (NULL != ptrHistory)
                {
                    ptrHistory->xPos.posMedianZ1 = CSD_TOUCHPAD_POS_NONE;
                    ptrHistory->xPos.posMedianZ2 = CSD_TOUCHPAD_POS_NONE;
                    ptrHistory->yPos.posMedianZ1 = CSD_TOUCHPAD_POS_NONE;
                    ptrHistory->yPos.posMedianZ2 = CSD_TOUCHPAD_POS_NONE;
                }
            #endif /* #if (0u != CSD_POS_MEDIAN_FILTER_EN) */
        }
    }

    return currStatus;
}
#endif /* #if (0u != CSD_CSD_TOUCHPAD_WIDGET_EN) */


#if (0u != CSD_CSX_TOUCHPAD_WIDGET_EN)
/*******************************************************************************
* Function Name: CSD_DpProcessCsxTouchpad
****************************************************************************//**
*
* \brief
*   Processes the status of the CSX Touchpad widget.
*   The process involves running the 3x3 centroid algorithm with the finger id tracker.
*
* \details
*   Processes the status of the CSX Touchpad widget.
*   The process involves running the 3x3 centroid algorithm with the finger id tracker.
*   The X/Y coordinates are updated in the RAM data structure.
*
* \param currStatus The current status of the widget sensors.
* \param ptrFlashWdgt The pointer to the Flash Widget Object.
*
* \return The number of detected touches.
*
*******************************************************************************/
uint32 CSD_DpProcessCsxTouchpad(uint32 currStatus, CSD_FLASH_WD_STRUCT const *ptrFlashWdgt)
{
    uint32 touchCount;
    
    (void)currStatus; /* Unused parameter */
    
    /* Check if sensors are active and located all local maxima */
    CSD_DpFindLocalMaxDd(ptrFlashWdgt);
    
    /* Calculate centroid position for all found local maxima */
    CSD_DpCalcTouchPadCentroid(ptrFlashWdgt);
    
    /* Identify all touches and assign them correct ID based on historical data */
    CSD_DpTouchTracking(ptrFlashWdgt);

    /* Filter the position data and write it into data structure.
     * The returned value tells how many touch positions were written to the Data Structure */
    touchCount = CSD_DpFilterTouchRecord(ptrFlashWdgt);
    
    return touchCount;
}
#endif /* #if (0u != CSD_CSX_TOUCHPAD_WIDGET_EN) */

#if (0u != CSD_CSX_MULTIPHASE_TX_EN)
/*******************************************************************************
* Function Name: CSD_DpDeconvolution
****************************************************************************//**
*
* \brief
*   Runs deconvolution algorithm for the CSX Touchpad widgets with the enabled
*   TX Multiphase scanning.
*
* \details
*   Runs deconvolution algorithm for the CSX Touchpad widgets with the enabled
*   TX Multiphase scanning.
*
*   The deconvolution coefficients are taken from the Flash Widget Object. 
*   The deconvolution results are written over the scan data stored in the
*   RAM Sensor objects.
*
* \param ptrFlashWdgt The pointer to the Flash Widget Object.
*
*******************************************************************************/
void CSD_DpDeconvolution(CSD_FLASH_WD_STRUCT const *ptrFlashWdgt)
{
    uint32 stNum;
    uint32 stInd;
    uint32 txNum;
    uint32 rxNum;
    uint32 freqNum;
    int32 accRaw;
    
    uint32 txCount = ptrFlashWdgt->numRows;
    uint32 rxCount = ptrFlashWdgt->numCols;
    const int16 *ptrSolveTable = ptrFlashWdgt->ptr2TxMultiphaseCoeff;
    CSD_RAM_SNS_STRUCT *ptrSns = (CSD_RAM_SNS_STRUCT *)ptrFlashWdgt->ptr2SnsRam;
    
    CYASSERT(ptrSolveTable);/* This table must be available for widgets with multiphase TX */
    
    /* Run deconvolution algorithm for each touchpad column */
    for (rxNum = 0u; rxNum < rxCount; rxNum++)
    {
        /* for each scan frequency */
        for (freqNum = 0u; freqNum < CSD_NUM_SCAN_FREQS; freqNum++)
        {
            /* for each RX/TX intersection */
            for (txNum = 0u; txNum < txCount; txNum++)
            {
                accRaw = 0u;
                
                for (stNum = 0u; stNum < txCount; stNum++)
                {
                    stInd = txCount - txNum + stNum;
                    
                    if (stInd > (txCount - 1u))
                    {
                        stInd = stNum - txNum;
                    }
                    
                    accRaw += (int32)ptrSns[rxNum * txCount + stNum].raw[freqNum] * (int32)ptrSolveTable[stInd];
                }
                
                /* Save calculated raw value to the temporary buffer. It is not possible at 
                 * this stage to save it into the sensor object by overwriting the scan data
                 * because the scan data is used to calculate raw values of other sensor objects 
                 */
                deconvRowData[txNum] = (accRaw >> TEMP_BUFFER_MSB_SHIFT);
            }
            
            for (txNum = 0u; txNum < txCount; txNum++)
            {
                ptrSns[rxNum * txCount + txNum].raw[freqNum] = deconvRowData[txNum];
            }
        }
    }
}

#endif /* #if (0u != CSD_CSX_MULTIPHASE_TX_EN) */


/* [] END OF FILE */
