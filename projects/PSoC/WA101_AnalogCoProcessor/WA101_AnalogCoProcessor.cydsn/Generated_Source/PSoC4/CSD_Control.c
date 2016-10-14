/***************************************************************************//**
* \file CSD_Control.c
* \version 3.10
*
* \brief
* This file provides the source code to the Control module API of the Component.
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
#include "CyLib.h"
#include "cyPm.h"
#include "CSD_Configuration.h"
#include "CSD_Structure.h"
#include "CSD_Control.h"
#include "CSD_Processing.h"
#include "CSD_Filter.h"
#include "CSD_Sensing.h"
#include "CSD_Tuner.h"

#if (CSD_ENABLE == CSD_SELF_TEST_EN)
    #include "CSD_SelfTest.h"
#endif

#if (0u != CSD_ADC_EN)
    #include "CSD_Adc.h"
#endif /* (0u != CSD_ADC_EN) */

/***********************************************************************************************************************
* Local definition
***********************************************************************************************************************/
#define CSD_INIT_DONE   (1u)
#define CSD_INIT_NEEDED (0u)

/***********************************************************************************************************************
* Local variables
***********************************************************************************************************************/
static uint8 CSD_InitVar = CSD_INIT_NEEDED;

/*******************************************************************************
* Function Name: CSD_Start
****************************************************************************//**
*
* \brief
*  Initializes component hardware and firmware modules. This function should be 
*  called by the application program prior to calling any other function of the 
*  component.
*
* \details
*  This function initializes the component hardware and firmware modules and 
*  should be called by the application program prior to calling any other API
*  of the component. When this function is called, the following tasks are 
*  executed as part of the initialization process:
*    1. Initialize the registers of the \ref group_structures variable 
*       CSD_dsRam based on the user selection in component 
*       configuration wizard. 
*    2. Configure the hardware to perform capacitive sensing.
*    3. If SmartSense Auto-tuning is selected for the CSD Tuning mode in the 
*       Basic tab, the auto-tuning algorithm is executed to set the optimal 
*       values for the hardware parameters of the widgets/sensors.
*    4. Calibrate the sensors and find the optimal values for IDACs of each 
*       widget / sensor, if the Enable IDAC auto-calibration is enabled in the
*       CSD Setting or CSX Setting tabs.
*    5. Perform a scan for all sensors and initialize the baseline history. 
*       During the scan, CPU is in the sleep mode to save power.
*    6. If the firmware filters are enabled in the Advanced General tab, the 
*       filter histories are also initialized.

*  Any next call of this API repeats an initialization process except for 
*  data structure initialization. Therefore, it is possible to change the 
*  component configuration from the application program by writing to the 
*  data structure registers and calling this function again. This is also 
*  done inside the CSD_RunTuner() function when a restart command 
*  is received. 
*
*  When the component operation is stopped by the CSD_Stop() 
*  function, the CSD_Start() function repeats an initialization 
*  process including data structure initialization.
*
* \return
*  Returns the status of the initialization process. If CYRET_SUCCESS is not 
*  received, some of the initialization fails and the component may not operate
*  as expected.
*
*******************************************************************************/
cystatus CSD_Start(void)
{
    cystatus result;
    uint8 interruptState;

    /* Initialize CSD modules */
    result = CSD_Initialize();

    #if (CSD_CSD_AUTOTUNE != CSD_CSD_SS_DIS)
        if (CYRET_SUCCESS == result)
        {
            result = CSD_SsAutoTune();
        }
    #endif /* #if (CSD_CSD_AUTOTUNE != CSD_CSD_SS_DIS) */

    #if ((0u != CSD_CSX_IDAC_AUTOCAL_EN) || \
        ((CSD_CSD_AUTOTUNE == CSD_CSD_SS_DIS) && (0u != CSD_CSD_IDAC_AUTOCAL_EN)))
        if (CYRET_SUCCESS == result)
        {
            result = CSD_CalibrateAllWidgets();
        }
    #endif

    if (CYRET_SUCCESS == result)
    {
        result = CSD_ScanAllWidgets();
    }

    if (CYRET_SUCCESS == result)
    {
        /* Put CPU into sleep mode while scanning */
        interruptState = CyEnterCriticalSection();
        while(CSD_NOT_BUSY != CSD_IsBusy())
        {
            CySysPmSleep();

            /* Exit from the critical section to unblock processing pending interrupts. */
            CyExitCriticalSection(interruptState);
            interruptState = CyEnterCriticalSection();
        }
        CyExitCriticalSection(interruptState);

    }
    CSD_FtInitialize();
    CSD_InitializeAllBaselines();

    #if (0u != CSD_ADC_EN)
        CSD_AdcInitialize();
    #endif /* (0u != CSD_ADC_EN) */

    return result;
}

/*******************************************************************************
* Function Name: CSD_Initialize
****************************************************************************//**
*
* \brief
*  This function initializes the CSD component.
*
* \details
*  This API initializes all sub-modules of the CSD component:
*   - Data Structure - set the default component parameters defined in the Customizer.
*   - Data Processing - resets all widget statuses.
*   - Tuner - resets tuning state.
*   - Sensing - prepares CSD HW for operation.
*
*  Note that Data Structure module is initialized only once after the reset or
*  CSD_Stop() API is called. The repeated calls of Initialize API
*  will not re-initialize Data Structure. This is done to preserve component
*  parameters that user may set in runtime.
*
*  If default component parameters are needed the CSD_DsInitialize()
*  API must be called directly from the application code.
*
* \return
*  Return CYRET_SUCCESS if the initialization was successful.
*
*******************************************************************************/
cystatus CSD_Initialize(void)
{
    cystatus result;

    /* The Data Structure and Tuner are initialized only once */
    if (CSD_INIT_NEEDED == CSD_InitVar)
    {
        CSD_DsInitialize();
        CSD_TuInitialize();
        CSD_InitVar = CSD_INIT_DONE;
    }

    CSD_DpInitialize();

    result = CSD_SsInitialize();

    return result;
}

/*******************************************************************************
* Function Name: CSD_Stop
****************************************************************************//**
*
* \brief
*  Stops the component operation.
*
* \details
*  This function stops the component operation, no sensor scanning can be 
*  executed when the component is stopped. Once stopped, the hardware block may 
*  be reconfigured by the application program for any other special usage. The 
*  component operation can be resumed by calling the CSD_Resume() 
*  function or the component can be reset by calling the 
*  CSD_Start() function.
*  
*  This function should be called when no scanning is in progress. 
*  I.e. CSD_IsBusy() returns a non-busy status.
*
* \return
*  Returns the status of the stop process. If CYRET_SUCCESS is not received, 
*  the stop process fails and retries may be required.
*
*******************************************************************************/
cystatus CSD_Stop(void)
{
    cystatus result = CYRET_SUCCESS;

    #if (CSD_ENABLE == CSD_ADC_EN)
        /* Release CSD resources */
        result = CSD_SsReleaseResources();

        /* Release ADC resources */
        CSD_AdcStop();
    #endif /* (CSD_ENABLE == CSD_ADC_EN) */

    CSD_InitVar = CSD_INIT_NEEDED;

    return result;
}

/*******************************************************************************
* Function Name: CSD_Resume
****************************************************************************//**
*
* \brief
*  Resumes the component operation if CSD_Stop() function was 
*  called previously.
*
* \details
*  This function resumes the component operation if the operation is stopped
*  previously by the CSD_Stop() function. The following tasks are 
*  executed as part of the operation resume process:
*    1. Reset all Widgets/Sensors status.
*    2. Configure the hardware to perform capacitive sensing.
*
* \return
*  Returns the status of the resume process. If CYRET_SUCCESS is not received,
*  the resume process fails and retries may be required.
*
*******************************************************************************/
cystatus CSD_Resume(void)
{
    cystatus result;

    /* The Tuner are initialized only once to do not break communication */
    if (CSD_INIT_NEEDED == CSD_InitVar)
    {
        CSD_TuInitialize();
        CSD_InitVar = CSD_INIT_DONE;
    }
    CSD_DpInitialize();

    result = CSD_SsInitialize();

    return result;
}

/*******************************************************************************
* Function Name: CSD_ProcessAllWidgets
****************************************************************************//**
*
* \brief
*  Performs full data processing of all enabled widgets.
*
* \details
*  This function performs all data processes for all enabled widgets in the 
*  component. The following tasks are executed as part of processing all the 
*  widgets:
*    1. Apply raw-count filters to raw counts, if they are enabled in the 
*       customizer.
*    2. Update thresholds if the SmartSense Full Auto-Tuning is enabled in
*       the customizer.
*    3. Update the Baselines and Difference counts for all sensors.
*    4. Update the sensor and widget status (on/off), update centroid for 
*       sliders and X/Y position for touchpads.
*  
*  Disabled widgets are not processed. To disable/enable a widget, set 
*  appropriate values in the 
*  CSD_WDGT_ENABLE<RegisterNumber>_PARAM_ID register using the
*  CSD_SetParam() function.
*  This function should be called only after all the sensors in the component
*  are scanned. Calling this function multiple times without sensor scanning
*  causes unexpected behavior.
*
*  If Self-test library is enabled this function executes baseline duplication
*  test. Refer to CSD_CheckBaselineDuplication() for details.
*
* \return
*  Returns the status of the processing operation. If CYRET_SUCCESS is not received, 
*  the processing fails and retries may be required.
*
*******************************************************************************/
cystatus CSD_ProcessAllWidgets(void)
{
    uint32 wdgtId;
    cystatus result = CYRET_SUCCESS;
    CSD_FLASH_WD_STRUCT const *ptrFlashWdgt;

    ptrFlashWdgt = CSD_dsFlash.wdgtArray;

    for (wdgtId = 0u; wdgtId < CSD_TOTAL_WIDGETS; wdgtId++)
    {
        if (0uL != CSD_GET_WIDGET_EN_STATUS(wdgtId))
        {
            switch(CSD_GET_SENSE_METHOD(ptrFlashWdgt))
            {
            #if (0u != CSD_TOTAL_CSD_WIDGETS)
                case CSD_SENSE_METHOD_CSD_E:
                    result = CSD_DpProcessCsdWidgetRawCounts(ptrFlashWdgt);
                    CSD_DpProcessCsdWidgetStatus(wdgtId, ptrFlashWdgt);
                    break;
            #endif /* #if (0u != CSD_TOTAL_CSD_WIDGETS) */

            #if (0u != CSD_TOTAL_CSX_WIDGETS)
                case CSD_SENSE_METHOD_CSX_E:
                    result = CSD_DpProcessCsxWidgetRawCounts(ptrFlashWdgt);
                    CSD_DpProcessCsxWidgetStatus(wdgtId, ptrFlashWdgt);
                    break;
            #endif /* #if (0u != CSD_TOTAL_CSX_WIDGETS) */

            default:
                CYASSERT(0u);
                break;
            }
                    
            #if (CSD_ENABLE == CSD_TST_BSLN_DUPLICATION_EN)
                if (CYRET_SUCCESS != result)
                {
                    CSD_UpdateTestResultBaselineDuplication(wdgtId, result);
                    result = CYRET_BAD_DATA;
                }
            #endif /* (CSD_ENABLE == CSD_TST_BSLN_DUPLICATION_EN) */
                    
        }
        ptrFlashWdgt++;
    }
    return result;
}

/*******************************************************************************
* Function Name: CSD_ProcessWidget
****************************************************************************//**
*
* \brief
*  Performs full data processing of the specified widget if it is enabled.
*
* \details
*  This function performs exactly the same tasks as 
*  CSD_ProcessAllWidgets(), but only for a specified widget. This 
*  function can be used along with the CSD_SetupWidget() and 
*  CSD_Scan() functions to scan and process data for a specific
*  widget. This function should be called only after all the sensors in the 
*  widgets are scanned. A disabled widget is not processed by this function.
*  
*  The pipeline scan method (i.e. during scanning of a widget perform processing
*  of the previously scanned widget) can be implemented using this function and
*  it may reduce the total scan/process time, increase the refresh rate and 
*  decrease the power consumption.
*
*  If Self-test library is enabled this function executes baseline duplication
*  test. Refer to CSD_CheckBaselineDuplication() for details.
*
* \param widgetId 
*  Specify the ID number of the widget to be processed. 
*  A macro for the widget ID can be found in the 
*  CSD Configuration header file defined as
*  CSD_<WidgetName>_WDGT_ID
*
* \return
*  Returns the status of the widget processing:
*  - CYRET_SUCCESS if operation is successfully completed
*  - CYRET_BAD_PARAM if the input parameter is invalid
*  - CYRET_INVALID_STATE if the specified widget is disabled
*  - CYRET_BAD_DATA if processing was failed
*
*******************************************************************************/
cystatus CSD_ProcessWidget(uint32 widgetId)
{
    cystatus result = CYRET_SUCCESS;
    CSD_FLASH_WD_STRUCT const *ptrFlashWdgt;

    if (widgetId >= CSD_TOTAL_WIDGETS)
    {
        result = CYRET_BAD_PARAM;
    }

    if ((CYRET_SUCCESS == result) && (0uL == CSD_GET_WIDGET_EN_STATUS(widgetId)))
    {
        result = CYRET_INVALID_STATE;
    }

    if (CYRET_SUCCESS == result)
    {
        ptrFlashWdgt = &CSD_dsFlash.wdgtArray[widgetId];

        switch(CSD_GET_SENSE_METHOD(ptrFlashWdgt))
        {
        #if (0u != CSD_TOTAL_CSD_WIDGETS)
            case CSD_SENSE_METHOD_CSD_E:
                result = CSD_DpProcessCsdWidgetRawCounts(ptrFlashWdgt);
                CSD_DpProcessCsdWidgetStatus(widgetId, ptrFlashWdgt);
                break;
        #endif /* #if (0u != CSD_TOTAL_CSD_WIDGETS) */

        #if (0u != CSD_TOTAL_CSX_WIDGETS)
            case CSD_SENSE_METHOD_CSX_E:
                result = CSD_DpProcessCsxWidgetRawCounts(ptrFlashWdgt);
                CSD_DpProcessCsxWidgetStatus(widgetId, ptrFlashWdgt);
                break;
        #endif /* #if (0u != CSD_TOTAL_CSX_WIDGETS) */

        default:
            CYASSERT(0u);
            break;
        }
                
        #if (CSD_ENABLE == CSD_TST_BSLN_DUPLICATION_EN)
            if (CYRET_SUCCESS != result)
            {
                CSD_UpdateTestResultBaselineDuplication(widgetId, result);
                result = CYRET_BAD_DATA;
            }
        #endif /* (CSD_ENABLE == CSD_TST_BSLN_DUPLICATION_EN) */
    }
    return result;
}


/*******************************************************************************
* Function Name: CSD_ProcessWidgetExt
****************************************************************************//**
*
* \brief
*  Performs customized data processing on the selected widget.
*
* \details
*  This function performs data processes for the specified widget specified by 
*  the mode parameter. The execution order of the requested operations is from 
*  LSB to MSB of the mode parameter. If a different order is needed, this API 
*  can be called multiple times with the required mode parameter. 
*  
*  This function can be used with any of the available scan functions. This 
*  function should be called only after all sensors in the specified widget are
*  scanned. Calling this function multiple times with the same mode without 
*  sensor scanning causes unexpected behavior. This function ignores the value 
*  of the wdgtEnable register. 
*  
*  The CSD_PROCESS_CALC_NOISE and 
*  CSD_PROCESS_THRESHOLDS flags are supported by the CSD sensing
*  method only when the auto-tuning mode is enabled.
*  
*  The pipeline scan method (i.e. during scanning of a widget perform processing
*  of a previously scanned widget) can be implemented using this function and it
*  may reduce the total scan/process time, increase the refresh rate and 
*  decrease the power consumption.
*
*  If Self-test library is enabled this function executes baseline duplication
*  test. Refer to CSD_CheckBaselineDuplication() for details.
*
* \param widgetId 
*  Specify the ID number of the widget to be processed. 
*  A macro for the widget ID can be found in the CSD Configuration header 
*  file defined as CSD_<WidgetName>_WDGT_ID.

* \param mode 
*  Specify the type of widget processing that needs to be executed for the 
*  specified widget:
*    1.	Bits [31..6] - Reserved
*    2.	Bits [5..0] - CSD_PROCESS_ALL - Execute all tasks
*    3.	Bit [5] - CSD_PROCESS_STATUS - Update Status (on/off, 
*       centroid position)
*    4.	Bit [4] - CSD_PROCESS_THRESHOLDS - Update Thresholds 
*       (only in CSD auto-tuning mode)
*    5.	Bit [3] - CSD_PROCESS_CALC_NOISE - Calculate Noise (only in
*       CSD auto-tuning mode)
*    6.	Bit [2] - CSD_PROCESS_DIFFCOUNTS - Update Difference Counts
*    7.	Bit [1] - CSD_PROCESS_BASELINE - Update Baselines
*    8.	Bit [0] - CSD_PROCESS_FILTER - Run Firmware Filter
*
* \return
*  Returns the status of the widget processing operation:
*  - CYRET_SUCCESS if processing is successfully performed
*  - CYRET_BAD_PARAM if the input parameter is invalid
*  - CYRET_BAD_DATA if processing was failed
*
*******************************************************************************/
cystatus CSD_ProcessWidgetExt(uint32 widgetId, uint32 mode)
{
    uint32 snsCount;
    cystatus result = CYRET_BAD_PARAM;
    CSD_FLASH_WD_STRUCT const *ptrFlashWdgt;
    CSD_PTR_FILTER_VARIANT fltrHistV;
    CSD_RAM_SNS_STRUCT *ptrSns;

    #if (0u != CSD_TOTAL_CSD_WIDGETS)
        uint32 isProxWdgt;
    #endif

    if (widgetId < CSD_TOTAL_WIDGETS)
    {
        ptrFlashWdgt = &CSD_dsFlash.wdgtArray[widgetId];
        snsCount = CSD_GET_SNS_CNT_BY_PTR(ptrFlashWdgt);
        ptrSns = ptrFlashWdgt->ptr2SnsRam;
        fltrHistV.ptr = ptrFlashWdgt->ptr2FltrHistory;

        switch(CSD_GET_SENSE_METHOD(ptrFlashWdgt))
        {
        #if (0u != CSD_TOTAL_CSD_WIDGETS)
            case CSD_SENSE_METHOD_CSD_E:
            {
                /* Determine if widget is type of proximity.
                 * The Proximity widgets use different filters and
                 * therefore have different filter history object structure */
                isProxWdgt =
                    (CSD_GET_WIDGET_TYPE(ptrFlashWdgt) == CSD_WD_PROXIMITY_E) ? 1Lu : 0Lu;

                /* Run the desired processing for the all CSD widget sensors */
                for (;snsCount-- > 0u;)
                {
                    result = CSD_DpProcessCsdSensorRawCountsExt(ptrFlashWdgt, ptrSns, fltrHistV, mode);
                    
                    #if (CSD_ENABLE == CSD_TST_BSLN_DUPLICATION_EN)
                        if (CYRET_SUCCESS != result)
                        {
                            result = (result | CSD_TST_LSBYTE) & snsCount;
                        }
                    #endif /* (CSD_ENABLE == CSD_TST_BSLN_DUPLICATION_EN) */

                    /* Move to the next sensor and filter history objects */
                    ptrSns++;
                    CSD_INC_FLTR_OBJ_VARIANT(isProxWdgt, fltrHistV);
                }

                if (0u != (mode & CSD_PROCESS_STATUS))
                {
                    CSD_DpProcessCsdWidgetStatus(widgetId, ptrFlashWdgt);
                }
                break;
            }
        #endif /* #if (0u != CSD_TOTAL_CSD_WIDGETS) */

        #if (0u != CSD_TOTAL_CSX_WIDGETS)
            case CSD_SENSE_METHOD_CSX_E:

                #if (0u != CSD_CSX_MULTIPHASE_TX_EN)
                    if ((0u != (mode & CSD_PROCESS_DECONVOLUTION)) &&
                        (0u != (ptrFlashWdgt->staticConfig & CSD_MULTIPHASE_TX_MASK)))
                    {
                        CSD_DpDeconvolution(ptrFlashWdgt);
                    }
                #endif /* #if (0u != CSD_CSX_MULTIPHASE_TX_EN) */

                /* Run the desired processing for the all CSX widget sensors */
                for (;snsCount-- > 0u;)
                {
                    result = CSD_DpProcessCsxSensorRawCountsExt(ptrFlashWdgt, ptrSns, fltrHistV, mode);
                    
                    #if (CSD_ENABLE == CSD_TST_BSLN_DUPLICATION_EN)
                        if (CYRET_SUCCESS != result)
                        {
                            result = (result | CSD_TST_LSBYTE) & snsCount;
                        }
                    #endif /* (CSD_ENABLE == CSD_TST_BSLN_DUPLICATION_EN) */
                    
                    /* Move to the next sensor and filter history objects */
                    ptrSns++;
                    CSD_INC_FLTR_OBJ_VARIANT(0u, fltrHistV);
                }

                if (0u != (mode & CSD_PROCESS_STATUS))
                {
                    CSD_DpProcessCsxWidgetStatus(widgetId, ptrFlashWdgt);
                }
                break;
        #endif /* #if (0u != CSD_TOTAL_CSX_WIDGETS) */

        default:
            CYASSERT(0u);
            break;
        }

        #if (CSD_ENABLE == CSD_TST_BSLN_DUPLICATION_EN)
            if (CYRET_SUCCESS != result)
            {
                CSD_UpdateTestResultBaselineDuplication(widgetId, snsCount);
                result = CYRET_BAD_DATA;
            }
        #endif /* (CSD_ENABLE == CSD_TST_BSLN_DUPLICATION_EN) */
        
    }
    return result;
}

/*******************************************************************************
* Function Name: CSD_ProcessSensorExt
****************************************************************************//**
*
* \brief
*  Performs customized data processing on the selected widget's sensor.
*
* \details
*  This function performs data processes for the specified sensor specified by
*  the mode parameter. The execution order of the requested operations is from 
*  LSB to MSB of the mode parameter. If a different order is needed, this 
*  function can be called multiple times with the required mode parameter.
*  
*  This function can be used with any of the available scan functions. This 
*  function should be called only after a specified sensor in the widget is 
*  scanned. Calling this function multiple times with the same mode without 
*  sensor scanning causes unexpected behavior. This function ignores the value 
*  of the wdgtEnable register. 
*  
*  The CSD_PROCESS_CALC_NOISE and 
*  CSD_PROCESS_THRESHOLDS flags are supported by the CSD sensing 
*  method only when the auto-tuning mode is enabled.
*  
*  The pipeline scan method (i.e. during scanning of a sensor perform processing
*  of a previously scanned sensor) can be implemented using this function and it
*  may reduce the total scan/process time, increase the refresh rate and 
*  decrease the power consumption.
*
*  If Self-test library is enabled this function executes baseline duplication
*  test. Refer to CSD_CheckBaselineDuplication() for details.
*
* \param widgetId  
*  Specify the ID number of the widget to process one of its sensors. 
*  A macro for the widget ID can be found in the 
*  CSD Configuration header file defined as 
*  CSD_<WidgetName>_WDGT_ID
*  
* \param sensorId  
*  Specify the ID number of the sensor within the widget to process it.
*  A macro for the sensor ID within a specified widget can be found in the
*  CSD Configuration header file defined as 
*  CSD_<WidgetName>_SNS<SensorNumber>_ID
*  
* \param mode      
*  Specify the type of the sensor processing that needs to be executed for the
*  specified sensor:
*    1. Bits [31..5] - Reserved
*    2. Bits [4..0] - CSD_PROCESS_ALL - Executes all tasks
*    3. Bit [4] - CSD_PROCESS_THRESHOLDS - Updates Thresholds (only
*       in auto-tuning mode)
*    4. Bit [3] - CSD_PROCESS_CALC_NOISE - Calculates Noise (only 
*       in auto-tuning mode)
*    5. Bit [2] - CSD_PROCESS_DIFFCOUNTS - Updates Difference Count
*    6. Bit [1] - CSD_PROCESS_BASELINE - Updates Baseline
*    7. Bit [0] - CSD_PROCESS_FILTER - Runs Firmware Filter
*
* \return
*  Returns the status of the sensor process operation:
*  - CYRET_SUCCESS if processing is successfully performed
*  - CYRET_BAD_PARAM if the input parameter is invalid
*  - CYRET_BAD_DATA if processing was failed
*
*******************************************************************************/
cystatus CSD_ProcessSensorExt(uint32 widgetId, uint32 sensorId, uint32 mode)
{
    cystatus result = CYRET_BAD_PARAM;
    CSD_FLASH_WD_STRUCT const *ptrFlashWdgt;
    CSD_PTR_FILTER_VARIANT fltrHistV;
    CSD_RAM_SNS_STRUCT *ptrSns;

    if ((widgetId < CSD_TOTAL_WIDGETS) && (sensorId < CSD_GET_SENSOR_COUNT(widgetId)))
    {
        result = CYRET_SUCCESS;

        ptrFlashWdgt = &CSD_dsFlash.wdgtArray[widgetId];
        fltrHistV.ptr = ptrFlashWdgt->ptr2FltrHistory;
        ptrSns = ptrFlashWdgt->ptr2SnsRam;
        ptrSns += sensorId;

        switch(CSD_GET_SENSE_METHOD(ptrFlashWdgt))
        {
        #if (0u != CSD_TOTAL_CSD_WIDGETS)
            case CSD_SENSE_METHOD_CSD_E:
                if (CSD_WD_PROXIMITY_E == (CSD_WD_TYPE_ENUM)ptrFlashWdgt->wdgtType)
                {
                    #if (0u != CSD_PROX_RC_FILTER_EN)
                        fltrHistV.ptr = &fltrHistV.ptrProx[sensorId];
                    #endif /* #if (0u != CSD_PROX_RC_FILTER_EN) */
                }
                else
                {
                    #if (0u != CSD_REGULAR_RC_FILTER_EN)
                        fltrHistV.ptr = &fltrHistV.ptrRegular[sensorId];
                    #endif /* #if (0u != CSD_REGULAR_RC_FILTER_EN) */
                }

                result = CSD_DpProcessCsdSensorRawCountsExt(ptrFlashWdgt, ptrSns, fltrHistV, mode);
                break;
        #endif /* #if (0u != CSD_TOTAL_CSD_WIDGETS) */

        #if (0u != CSD_TOTAL_CSX_WIDGETS)
            case CSD_SENSE_METHOD_CSX_E:
                #if (0u != CSD_REGULAR_RC_FILTER_EN)
                    fltrHistV.ptr = &fltrHistV.ptrRegular[sensorId];
                #endif /* #if (0u != CSD_REGULAR_RC_FILTER_EN) */

                result = CSD_DpProcessCsxSensorRawCountsExt(ptrFlashWdgt, ptrSns, fltrHistV, mode);
                break;
        #endif /* #if (0u != CSD_TOTAL_CSX_WIDGETS) */

        default:
            CYASSERT(0u);
            break;
        }
                
        #if (CSD_ENABLE == CSD_TST_BSLN_DUPLICATION_EN)
            if (CYRET_SUCCESS != result)
            {
                CSD_UpdateTestResultBaselineDuplication(widgetId, sensorId);
                result = CYRET_BAD_DATA;
            }
        #endif /* (CSD_ENABLE == CSD_TST_BSLN_DUPLICATION_EN) */
        
    }
    return result;
}

/*******************************************************************************
* Function Name: CSD_Sleep
****************************************************************************//**
*
* \brief
*  Prepares the component to deep sleep.
*
* \details
*  Currently this function is empty and exists as a place for future updates, 
*  this function shall be used to prepare the component to enter deep sleep.
*
*******************************************************************************/
void CSD_Sleep(void)
{
}


/*******************************************************************************
* Function Name: CSD_Wakeup
****************************************************************************//**
*
* \brief
*  Currently this function is empty and exists as a place for future updates,
*  this function shall be used to resume the component after exiting deep sleep.
*
* \details
*  This function resumes the component after sleep.
*
*******************************************************************************/
void CSD_Wakeup(void)
{
}


/* [] END OF FILE */
