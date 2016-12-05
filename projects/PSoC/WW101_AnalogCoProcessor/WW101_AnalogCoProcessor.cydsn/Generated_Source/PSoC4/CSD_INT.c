/***************************************************************************//**
* \file CSD_INT.c
* \version 3.10
*
* \brief
*   This file contains the source code for implementation of the CapSense component
*   Interrupt Service Routine (ISR).
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
#include "cyfitter.h"
#include "CSD_Configuration.h"
#include "CSD_Sensing.h"


/*******************************************************************************
* Static Function Prototypes
*******************************************************************************/
/**
* \if SECTION_CAPSENSE_INTERNAL
* \addtogroup group_capsense_internal
* \{
*/

#if (((CSD_ENABLE == CSD_CSD_EN) || (CSD_ENABLE == CSD_CSD_CSX_EN)) && \
     (CSD_ENABLE == CSD_MULTI_FREQ_SCAN_EN))
    static void CSD_SsNextFrequencyScan(void);
#endif /* (((CSD_ENABLE == CSD_CSD_EN) || (CSD_ENABLE == CSD_CSD_CSX_EN)) && \
            (CSD_ENABLE == CSD_MULTI_FREQ_SCAN_EN)) */

#if ((CSD_ENABLE == CSD_CSD_EN) || (CSD_ENABLE == CSD_CSD_CSX_EN))
    CY_INLINE static void CSD_SsCSDPostScan(void);
    CY_INLINE static void CSD_SsCSDInitNextScan(void);
#endif /* ((CSD_ENABLE == CSD_CSD_EN) || (CSD_ENABLE == CSD_CSD_CSX_EN)) */ 

#if (CSD_ENABLE == CSD_CSD2X_EN)
    #if (CSD_ENABLE == CSD_MULTI_FREQ_SCAN_EN)
        CY_INLINE static void CSD_SsCSD2XCheckNextScan(void);
        static void CSD_SsCSD2XNextFrequencyScan(void);
    #endif /* (CSD_ENABLE == CSD_MULTI_FREQ_SCAN_EN) */
    CY_INLINE static void CSD_SsCSD2XInitNextScan(void);
#endif /* (CSD_ENABLE == CSD_CSD2X_EN) */

   
/** \}
* \endif */


/**
* \if SECTION_CAPSENSE_INTERRUPT
* \addtogroup group_capsense_interrupt
* \{
*/


#if ((CSD_ENABLE == CSD_CSD_EN) || (CSD_ENABLE == CSD_CSD_CSX_EN))

#if (CSD_ENABLE == CSD_CSDV2)
    /*  CSDv2 HW IP block part */

    /*******************************************************************************
    * Function Name: CSD_CSDPostSingleScan
    ****************************************************************************//**
    *
    * \brief
    *  This is an internal ISR function for single-sensor scanning implementation.
    *
    * \details
    *  This ISR handler is triggered when the user calls the
    *  CSD_CSDScanExt() function.
    *
    *  The following tasks are performed for CSDv1 HW IP block:
    *    1. Disable the CSD interrupt
    *    2. Read the Counter register and update the data structure with raw data
    *    3. Connect the Vref buffer to the AMUX bus
    *    4. Update the Scan Counter
    *    5. Reset the BUSY flag
    *    6. Enable the CSD interrupt.
    *
    *  The following tasks are performed for CSDv2 HW IP block:
    *    1. Check if the raw data is not noisy
    *    2. Read the Counter register and update the data structure with raw data
    *    3. Configure and start the scan for the next frequency if the
    *      multi-frequency is enabled
    *    4. Update the Scan Counter
    *    5. Reset the BUSY flag
    *    6. Enable the CSD interrupt.
    *
    *  The ISR handler changes IMO and initializes scanning for the next frequency
    *  channels when multi-frequency scanning is enabled.
    *
	*  This function has two Macro Callbacks that allow calling the user code
    *  from macros specified in a component's generated code. Refer to the 
    *  \ref group_capsense_macrocallbacks section of PSoC Creator User Guide 
    *  for details. 
    *
    *******************************************************************************/
    CY_ISR(CSD_CSDPostSingleScan)
    {
        #ifdef CSD_ENTRY_CALLBACK
            CSD_EntryCallback();
        #endif /* CSD_ENTRY_CALLBACK */

        /* Clear pending interrupt */
        CY_SET_REG32(CSD_INTR_PTR, CSD_INTR_SET_MASK);

    #if (CSD_ENABLE == CSD_CSD_NOISE_METRIC_EN)
        if ((CSD_CSD_NOISE_METRIC_TH < ((CY_GET_REG32(CSD_RESULT_VAL1_PTR) &
                                                    CSD_RESULT_VAL1_BAD_CONVS_MASK) >>
                                                    CSD_RESULT_VAL1_BAD_CONVS_SHIFT)) &&
                                                    (0u < CSD_badConversionsNum))
        {
            /* Decrement bad conversions number */
            CSD_badConversionsNum--;

            /* Start the re-scan */
            CY_SET_REG32(CSD_SEQ_START_PTR, CSD_SEQ_START_AZ0_SKIP_MASK |
                                                         CSD_SEQ_START_AZ1_SKIP_MASK |
                                                         CSD_SEQ_START_START_MASK);
        }
        else
        {
    #endif /* (CSD_ENABLE == CSD_CSD_NOISE_METRIC_EN) */

            CSD_SsCSDPostScan();

            #if (CSD_ENABLE == CSD_MULTI_FREQ_SCAN_EN)
                if (CSD_FREQ_CHANNEL_2 > CSD_scanFreqIndex)
                {
                    /* Scan the next channel */
                    CSD_SsNextFrequencyScan();
                }
                else
                {
                    /* All channels are scanned. Set IMO to zero channel */
                    CSD_scanFreqIndex = CSD_FREQ_CHANNEL_0;
                    CSD_SsChangeImoFreq(CSD_FREQ_CHANNEL_0);

                    #if (CSD_ENABLE == CSD_BLOCK_OFF_AFTER_SCAN_EN)
                        /*  Disable CSDv2 block */
                        CY_SET_REG32(CSD_CONFIG_PTR, CSD_configCsd);
                    #endif /* (CSD_ENABLE == CSD_BLOCK_OFF_AFTER_SCAN_EN) */

                    /* Update Scan Counter */
                    CSD_dsRam.scanCounter++;

                    /* Sensor is totally scanned. Reset BUSY flag */
                    CSD_dsRam.status &= ~(CSD_SW_STS_BUSY | CSD_WDGT_SW_STS_BUSY);
                }
            #else
                {
                    #if (CSD_ENABLE == CSD_BLOCK_OFF_AFTER_SCAN_EN)
                        /*  Disable CSDv2 block */
                        CY_SET_REG32(CSD_CONFIG_PTR, CSD_configCsd);
                    #endif /* (CSD_ENABLE == CSD_BLOCK_OFF_AFTER_SCAN_EN) */

                    /* Update Scan Counter */
                    CSD_dsRam.scanCounter++;

                    /* Sensor is totally scanned. Reset BUSY flag */
                    CSD_dsRam.status &= ~(CSD_SW_STS_BUSY | CSD_WDGT_SW_STS_BUSY);
                }
            #endif /* (CSD_ENABLE == CSD_MULTI_FREQ_SCAN_EN) */
    #if (CSD_ENABLE == CSD_CSD_NOISE_METRIC_EN)
        }
    #endif /* (CSD_ENABLE == CSD_CSD_NOISE_METRIC_EN) */

        #ifdef CSD_EXIT_CALLBACK
            CSD_ExitCallback();
        #endif /* CSD_EXIT_CALLBACK */
    }


    /*******************************************************************************
    * Function Name: CSD_CSDPostMultiScan
    ****************************************************************************//**
    *
    * \brief
    *  This is an internal ISR function for multiple-sensor scanning 
    *  implementation.
    *
    * \details
    *  This ISR handler is triggered when the user calls the
    *  CSD_Scan() or CSD_ScanAllWidgets() APIs.
    *  
    *  The following tasks are performed:
    *    1. Disable the CSD interrupt
    *    2. Read the Counter register and update the data structure with raw data
    *    3. Connect the Vref buffer to the AMUX bus
    *    4. Disable the CSD block (after the widget has been scanned)
    *    5. Update Scan Counter
    *    6. Reset the BUSY flag
    *    7. Enable the CSD interrupt.
    *
    *  The ISR handler initializes scanning for the previous sensor when the 
    *  widget has more than one sensor.
    *  The ISR handler initializes scanning for the next widget when the
    *  CSD_ScanAllWidgets() APIs are called and the project has
    *  more than one widget.
    *  The ISR handler changes the IMO and initializes scanning for the next
    *  frequency channels when multi-frequency scanning is enabled.
    *
    *  This function has two Macro Callbacks that allow calling the user 
    *  code from macros specified in a component's generated code. Refer to the 
    *  \ref group_capsense_macrocallbacks section of PSoC Creator User Guide 
    *  for details.
    *
    *******************************************************************************/
    CY_ISR(CSD_CSDPostMultiScan)
    {
        /*  Declare and initialise ptr to sensor IO structure to appropriate address        */
        CSD_FLASH_IO_STRUCT const *curSnsIOPtr = (CSD_FLASH_IO_STRUCT const *)
                                                          CSD_dsFlash.wdgtArray[CSD_widgetIndex].ptr2SnsFlash
                                                          + CSD_sensorIndex;

        #ifdef CSD_ENTRY_CALLBACK
            CSD_EntryCallback();
        #endif /* CSD_ENTRY_CALLBACK */

        /* Clear pending interrupt */
        CY_SET_REG32(CSD_INTR_PTR, CSD_INTR_SET_MASK);

        #if (CSD_ENABLE == CSD_CSD_NOISE_METRIC_EN)
            if ((CSD_CSD_NOISE_METRIC_TH < ((CY_GET_REG32(CSD_RESULT_VAL1_PTR) &
                                                      CSD_RESULT_VAL1_BAD_CONVS_MASK) >>
                                                      CSD_RESULT_VAL1_BAD_CONVS_SHIFT)) &&
                                                      (0u < CSD_badConversionsNum))
            {
                /* Decrement bad conversions number */
                CSD_badConversionsNum--;

                /* Start the re-scan */
                CY_SET_REG32(CSD_SEQ_START_PTR, CSD_SEQ_START_AZ0_SKIP_MASK |
                                                             CSD_SEQ_START_AZ1_SKIP_MASK |
                                                             CSD_SEQ_START_START_MASK);
            }
            else
            {
        #endif /* (CSD_ENABLE == CSD_CSD_NOISE_METRIC_EN) */

            CSD_SsCSDPostScan();

            #if (CSD_ENABLE == CSD_MULTI_FREQ_SCAN_EN)
                /* Disable sensor when all frequency channels are scanned */
                if (CSD_FREQ_CHANNEL_2 == CSD_scanFreqIndex)
                {
                    /* Disable sensor */
                    CSD_CSDDisconnectSns(curSnsIOPtr);
                }
            #else
                /* Disable sensor */
                CSD_CSDDisconnectSns(curSnsIOPtr);
            #endif /* (CSD_ENABLE == CSD_MULTI_FREQ_SCAN_EN) */

            #if (CSD_ENABLE == CSD_MULTI_FREQ_SCAN_EN)
                if (CSD_FREQ_CHANNEL_2 > CSD_scanFreqIndex)
                {
                     /* Scan the next channel */
                    CSD_SsNextFrequencyScan();
                }
                else
                {
                     /* All channels are scanned. Set IMO to zero channel */
                    CSD_scanFreqIndex = CSD_FREQ_CHANNEL_0;
                    CSD_SsChangeImoFreq(CSD_FREQ_CHANNEL_0);

                     /* Scan the next sensor */
                    CSD_SsCSDInitNextScan();
                }
            #else
                /* Scan the next sensor */
                CSD_SsCSDInitNextScan();
            #endif /* (CSD_ENABLE == CSD_MULTI_FREQ_SCAN_EN) */
        #if (CSD_ENABLE == CSD_CSD_NOISE_METRIC_EN)
            }
        #endif /* (CSD_ENABLE == CSD_CSD_NOISE_METRIC_EN) */

        #ifdef CSD_EXIT_CALLBACK
            CSD_ExitCallback();
        #endif /* CSD_EXIT_CALLBACK */
    }


    #if (CSD_ENABLE == CSD_CSD_GANGED_SNS_EN)
    /*******************************************************************************
    * Function Name: CSD_CSDPostMultiScanGanged
    ****************************************************************************//**
    *
    * \brief
    *  This is an internal ISR function for multiple-sensor scanning 
    *  implementation for ganged sensors.
    *
    * \details
    *  This ISR handler is triggered when the user calls the
    *  CSD_Scan() API for a ganged sensor or the
    *  CSD_ScanAllWidgets() API in the project with ganged sensors.
    *
    *  The following tasks are performed:
    *    1. Disable the CSD interrupt
    *    2. Read the Counter register and update the data structure with raw data
    *    3. Connect the Vref buffer to the AMUX bus
    *    4. Disable the CSD block (after the widget has been scanned)
    *    5. Update Scan Counter
    *    6. Reset the BUSY flag
    *    7. Enable the CSD interrupt.
    *
    *  The ISR handler initializes scanning for the previous sensor when the
    *  widget has more than one sensor.
    *  The ISR handler initializes scanning for the next widget when the
    *  CSD_ScanAllWidgets() APIs are called and the project has 
    *  more than one widget.
    *  The ISR handler changes the IMO and initializes scanning for the next 
    *  frequency channels when multi-frequency scanning is enabled.
    *
    *  This function has two Macro Callbacks that allow calling the user 
    *  code from macros specified in a component's generated code. Refer to the 
    *  \ref group_capsense_macrocallbacks section of PSoC Creator User Guide 
    *  for details.
    *
    *******************************************************************************/
    CY_ISR(CSD_CSDPostMultiScanGanged)
    {
        #ifdef CSD_ENTRY_CALLBACK
            CSD_EntryCallback();
        #endif /* CSD_ENTRY_CALLBACK */

        /* Clear pending interrupt */
        CY_SET_REG32(CSD_INTR_PTR, CSD_INTR_SET_MASK);

        #if (CSD_ENABLE == CSD_CSD_NOISE_METRIC_EN)
            if ((CSD_CSD_NOISE_METRIC_TH < ((CY_GET_REG32(CSD_RESULT_VAL1_PTR) &
                                                      CSD_RESULT_VAL1_BAD_CONVS_MASK) >>
                                                      CSD_RESULT_VAL1_BAD_CONVS_SHIFT)) &&
                                                      (0u < CSD_badConversionsNum))
            {
                /* Decrement bad conversions number */
                CSD_badConversionsNum--;

                /* Start the re-scan */
                CY_SET_REG32(CSD_SEQ_START_PTR, CSD_SEQ_START_AZ0_SKIP_MASK |
                                                             CSD_SEQ_START_AZ1_SKIP_MASK |
                                                             CSD_SEQ_START_START_MASK);
            }
            else
            {
        #endif /* (CSD_ENABLE == CSD_CSD_NOISE_METRIC_EN) */

            CSD_SsCSDPostScan();

            #if (CSD_ENABLE == CSD_MULTI_FREQ_SCAN_EN)
                if (CSD_FREQ_CHANNEL_2 == CSD_scanFreqIndex)
                {
                    CSD_SsCSDDisconnectSnsExt((uint32)CSD_widgetIndex, (uint32)CSD_sensorIndex);
                }
            #else
                CSD_SsCSDDisconnectSnsExt((uint32)CSD_widgetIndex, (uint32)CSD_sensorIndex);
            #endif /* (CSD_ENABLE == CSD_MULTI_FREQ_SCAN_EN) */

            #if (CSD_ENABLE == CSD_MULTI_FREQ_SCAN_EN)
                if (CSD_FREQ_CHANNEL_2 > CSD_scanFreqIndex)
                {
                     /* Scan the next channel */
                    CSD_SsNextFrequencyScan();
                }
                else
                {
                    /* All channels are scanned. Set IMO to zero channel */
                    CSD_scanFreqIndex = CSD_FREQ_CHANNEL_0;
                    CSD_SsChangeImoFreq(CSD_FREQ_CHANNEL_0);

                     /* Scan the next sensor */
                    CSD_SsCSDInitNextScan();
                }
            #else
                 /* Scan the next sensor */
                CSD_SsCSDInitNextScan();
            #endif /* (CSD_ENABLE == CSD_MULTI_FREQ_SCAN_EN) */
        #if (CSD_ENABLE == CSD_CSD_NOISE_METRIC_EN)
            }
        #endif /* (CSD_ENABLE == CSD_CSD_NOISE_METRIC_EN) */

        #ifdef CSD_EXIT_CALLBACK
            CSD_ExitCallback();
        #endif /* CSD_EXIT_CALLBACK */
    }
    #endif /* (CSD_ENABLE == CSD_CSD_GANGED_SNS_EN) */

#else
    /* CSDv1 part */

    /*******************************************************************************
    * Function Name: CSD_CSDPostSingleScan
    ****************************************************************************//**
    *
    * \brief
    *  This is an internal ISR function for single-sensor scanning implementation.
    *
    * \details
    *  This ISR handler is triggered when the user calls the
    *  CSD_CSDScanExt() function.
    *
    *  The following tasks are performed for CSDv1 HW IP block:
    *    1. Disable the CSD interrupt
    *    2. Read the Counter register and update the data structure with raw data
    *    3. Connect the Vref buffer to the AMUX bus
    *    4. Update the Scan Counter
    *    5. Reset the BUSY flag
    *    6. Enable the CSD interrupt.
    *
    *  The following tasks are performed for CSDv2 HW IP block:
    *    1. Check if the raw data is not noisy
    *    2. Read the Counter register and update the data structure with raw data
    *    3. Configure and start the scan for the next frequency if the
    *      multi-frequency is enabled
    *    4. Update the Scan Counter
    *    5. Reset the BUSY flag
    *    6. Enable the CSD interrupt.
    *
    *  The ISR handler changes IMO and initializes scanning for the next frequency
    *  channels when multi-frequency scanning is enabled.
    *
	*  This function has two Macro Callbacks that allow calling the user code
    *  from macros specified in a component's generated code. Refer to the 
    *  \ref group_capsense_macrocallbacks section of PSoC Creator User Guide 
    *  for details. 
    *
    *******************************************************************************/
    CY_ISR(CSD_CSDPostSingleScan)
    {
        #ifdef CSD_ENTRY_CALLBACK
            CSD_EntryCallback();
        #endif /* CSD_ENTRY_CALLBACK */

        /* Clear pending interrupt */
        CY_SET_REG32(CSD_INTR_PTR, CSD_INTR_SET_MASK);

        /* Read Rawdata */
        CSD_SsCSDPostScan();

        #if (CSD_ENABLE == CSD_MULTI_FREQ_SCAN_EN)
            if (CSD_FREQ_CHANNEL_2 > CSD_scanFreqIndex)
            {
                /*  Connect Vref Buffer to AMUX bus. CSDv1 block is enabled */
                CY_SET_REG32(CSD_CONFIG_PTR, CSD_configCsd | CSD_CTANK_PRECHARGE_CONFIG_CSD_EN);

                CSD_SsNextFrequencyScan();
            }
            else
            {
                CSD_scanFreqIndex = CSD_FREQ_CHANNEL_0;
                CSD_SsChangeImoFreq(CSD_FREQ_CHANNEL_0);

                #if (CSD_ENABLE == CSD_BLOCK_OFF_AFTER_SCAN_EN)
                    /*  Disable CSDv1 block. Connect Vref Buffer to AMUX bus */
                    #if ((CSD_CSH_PRECHARGE_IO_BUF == CSD_CSD_CSH_PRECHARGE_SRC) &&\
                         (CSD_ENABLE == CSD_CSD_SHIELD_TANK_EN))
                        CY_SET_REG32(CSD_CONFIG_PTR, CSD_configCsd | CSD_CMOD_PRECHARGE_CONFIG);
                    #else
                        CY_SET_REG32(CSD_CONFIG_PTR, CSD_configCsd | CSD_CTANK_PRECHARGE_CONFIG);
                    #endif /* ((CSD_CSH_PRECHARGE_IO_BUF == CSD_CSD_CSH_PRECHARGE_SRC) &&\
                               (CSD_ENABLE == CSD_CSD_SHIELD_TANK_EN)) */
                #else
                    /*  Connect Vref Buffer to AMUX bus. CSDv1 block is enabled */
                    CY_SET_REG32(CSD_CONFIG_PTR, CSD_configCsd | CSD_CTANK_PRECHARGE_CONFIG_CSD_EN);
                #endif /* (CSD_ENABLE == CSD_BLOCK_OFF_AFTER_SCAN_EN) */

                /* Update Scan Counter */
                CSD_dsRam.scanCounter++;

                /* Sensor is totally scanned. Reset BUSY flag */
                CSD_dsRam.status &= ~(CSD_SW_STS_BUSY | CSD_WDGT_SW_STS_BUSY);
            }
        #else
            {
                #if (CSD_ENABLE == CSD_BLOCK_OFF_AFTER_SCAN_EN)
                    /*  Disable CSDv1 block. Connect Vref Buffer to AMUX bus */
                    #if ((CSD_CSH_PRECHARGE_IO_BUF == CSD_CSD_CSH_PRECHARGE_SRC) &&\
                         (CSD_ENABLE == CSD_CSD_SHIELD_TANK_EN))
                        CY_SET_REG32(CSD_CONFIG_PTR, CSD_configCsd | CSD_CMOD_PRECHARGE_CONFIG);
                    #else
                        CY_SET_REG32(CSD_CONFIG_PTR, CSD_configCsd | CSD_CTANK_PRECHARGE_CONFIG);
                    #endif /* ((CSD_CSH_PRECHARGE_IO_BUF == CSD_CSD_CSH_PRECHARGE_SRC) &&\
                               (CSD_ENABLE == CSD_CSD_SHIELD_TANK_EN)) */
                #else
                    /*  Connect Vref Buffer to AMUX bus. CSDv1 block is enabled */
                    CY_SET_REG32(CSD_CONFIG_PTR, CSD_configCsd | CSD_CTANK_PRECHARGE_CONFIG_CSD_EN);
                #endif /* (CSD_ENABLE == CSD_BLOCK_OFF_AFTER_SCAN_EN) */

                /* Update Scan Counter */
                CSD_dsRam.scanCounter++;

                /* Sensor is totally scanned. Reset BUSY flag */
                CSD_dsRam.status &= ~(CSD_SW_STS_BUSY | CSD_WDGT_SW_STS_BUSY);
            }
        #endif /* (CSD_ENABLE == CSD_MULTI_FREQ_SCAN_EN) */

        #ifdef CSD_EXIT_CALLBACK
            CSD_ExitCallback();
        #endif /* CSD_EXIT_CALLBACK */
    }


    /*******************************************************************************
    * Function Name: CSD_CSDPostMultiScan
    ****************************************************************************//**
    *
    * \brief
    *  This is an internal ISR function for multiple-sensor scanning 
    *  implementation.
    *
    * \details
    *  This ISR handler is triggered when the user calls the
    *  CSD_Scan() or CSD_ScanAllWidgets() APIs.
    *  
    *  The following tasks are performed:
    *    1. Disable the CSD interrupt
    *    2. Read the Counter register and update the data structure with raw data
    *    3. Connect the Vref buffer to the AMUX bus
    *    4. Disable the CSD block (after the widget has been scanned)
    *    5. Update Scan Counter
    *    6. Reset the BUSY flag
    *    7. Enable the CSD interrupt.
    *
    *  The ISR handler initializes scanning for the previous sensor when the 
    *  widget has more than one sensor.
    *  The ISR handler initializes scanning for the next widget when the
    *  CSD_ScanAllWidgets() APIs are called and the project has
    *  more than one widget.
    *  The ISR handler changes the IMO and initializes scanning for the next
    *  frequency channels when multi-frequency scanning is enabled.
    *
    *  This function has two Macro Callbacks that allow calling the user 
    *  code from macros specified in a component's generated code. Refer to the 
    *  \ref group_capsense_macrocallbacks section of PSoC Creator User Guide 
    *  for details.
    *
    *******************************************************************************/
    CY_ISR(CSD_CSDPostMultiScan)
    {
        /*  Declare and initialise ptr to sensor IO structure to appropriate address        */
        CSD_FLASH_IO_STRUCT const *curSnsIOPtr = (CSD_FLASH_IO_STRUCT const *)
                                                          CSD_dsFlash.wdgtArray[CSD_widgetIndex].ptr2SnsFlash
                                                          + CSD_sensorIndex;

        #ifdef CSD_ENTRY_CALLBACK
            CSD_EntryCallback();
        #endif /* CSD_ENTRY_CALLBACK */

        /* Clear pending interrupt */
        CY_SET_REG32(CSD_INTR_PTR, CSD_INTR_SET_MASK);

         /* Read Rawdata */
        CSD_SsCSDPostScan();

        /*  Connect Vref Buffer to AMUX bus */
        CY_SET_REG32(CSD_CONFIG_PTR, CSD_configCsd | CSD_CTANK_PRECHARGE_CONFIG_CSD_EN);

        #if (CSD_ENABLE == CSD_MULTI_FREQ_SCAN_EN)
            /* Disable sensor when all frequency channels are scanned */
            if (CSD_FREQ_CHANNEL_2 == CSD_scanFreqIndex)
            {
                /* Disable sensor */
                CSD_CSDDisconnectSns(curSnsIOPtr);
            }
        #else
            /* Disable sensor */
            CSD_CSDDisconnectSns(curSnsIOPtr);
        #endif /* (CSD_ENABLE == CSD_MULTI_FREQ_SCAN_EN) */

        #if (CSD_ENABLE == CSD_MULTI_FREQ_SCAN_EN)
            if (CSD_FREQ_CHANNEL_2 > CSD_scanFreqIndex)
            {
                 /* Scan the next channel */
                CSD_SsNextFrequencyScan();
            }
            else
            {
                 /* All channels are scanned. Set IMO to zero channel */
                CSD_scanFreqIndex = CSD_FREQ_CHANNEL_0;
                CSD_SsChangeImoFreq(CSD_FREQ_CHANNEL_0);

                 /* Scan the next sensor */
                CSD_SsCSDInitNextScan();
            }
        #else
            /* Scan the next sensor */
            CSD_SsCSDInitNextScan();
        #endif /* (CSD_ENABLE == CSD_MULTI_FREQ_SCAN_EN) */

        #ifdef CSD_EXIT_CALLBACK
            CSD_ExitCallback();
        #endif /* CSD_EXIT_CALLBACK */
    }


    #if (CSD_ENABLE == CSD_CSD_GANGED_SNS_EN)
    /*******************************************************************************
    * Function Name: CSD_CSDPostMultiScanGanged
    ****************************************************************************//**
    *
    * \brief
    *  This is an internal ISR function for multiple-sensor scanning 
    *  implementation for ganged sensors.
    *
    * \details
    *  This ISR handler is triggered when the user calls the
    *  CSD_Scan() API for a ganged sensor or the
    *  CSD_ScanAllWidgets() API in the project with ganged sensors.
    *
    *  The following tasks are performed:
    *    1. Disable the CSD interrupt
    *    2. Read the Counter register and update the data structure with raw data
    *    3. Connect the Vref buffer to the AMUX bus
    *    4. Disable the CSD block (after the widget has been scanned)
    *    5. Update Scan Counter
    *    6. Reset the BUSY flag
    *    7. Enable the CSD interrupt.
    *
    *  The ISR handler initializes scanning for the previous sensor when the
    *  widget has more than one sensor.
    *  The ISR handler initializes scanning for the next widget when the
    *  CSD_ScanAllWidgets() APIs are called and the project has 
    *  more than one widget.
    *  The ISR handler changes the IMO and initializes scanning for the next 
    *  frequency channels when multi-frequency scanning is enabled.
    *
    *  This function has two Macro Callbacks that allow calling the user 
    *  code from macros specified in a component's generated code. Refer to the 
    *  \ref group_capsense_macrocallbacks section of PSoC Creator User Guide 
    *  for details.
    *
    *******************************************************************************/
    CY_ISR(CSD_CSDPostMultiScanGanged)
    {
        #ifdef CSD_ENTRY_CALLBACK
            CSD_EntryCallback();
        #endif /* CSD_ENTRY_CALLBACK */

        /* Clear pending interrupt */
        CY_SET_REG32(CSD_INTR_PTR, CSD_INTR_SET_MASK);

         /* Read Rawdata */
        CSD_SsCSDPostScan();

        /*  Connect Vref Buffer to AMUX bus */
        CY_SET_REG32(CSD_CONFIG_PTR, CSD_configCsd | CSD_CTANK_PRECHARGE_CONFIG_CSD_EN);

        #if (CSD_ENABLE == CSD_MULTI_FREQ_SCAN_EN)
            if (CSD_FREQ_CHANNEL_2 == CSD_scanFreqIndex)
            {
                CSD_SsCSDDisconnectSnsExt((uint32)CSD_widgetIndex, (uint32)CSD_sensorIndex);
            }
        #else
            CSD_SsCSDDisconnectSnsExt((uint32)CSD_widgetIndex, (uint32)CSD_sensorIndex);
        #endif /* (CSD_ENABLE == CSD_MULTI_FREQ_SCAN_EN) */

        #if (CSD_ENABLE == CSD_MULTI_FREQ_SCAN_EN)
            if (CSD_FREQ_CHANNEL_2 > CSD_scanFreqIndex)
            {
                 /* Scan the next channel */
                CSD_SsNextFrequencyScan();
            }
            else
            {
                /* All channels are scanned. Set IMO to zero channel */
                CSD_scanFreqIndex = CSD_FREQ_CHANNEL_0;
                CSD_SsChangeImoFreq(CSD_FREQ_CHANNEL_0);

                 /* Scan the next sensor */
                CSD_SsCSDInitNextScan();
            }
        #else
             /* Scan the next sensor */
            CSD_SsCSDInitNextScan();
        #endif /* (CSD_ENABLE == CSD_MULTI_FREQ_SCAN_EN) */

        #ifdef CSD_EXIT_CALLBACK
            CSD_ExitCallback();
        #endif /* CSD_EXIT_CALLBACK */
    }
    #endif /* (CSD_ENABLE == CSD_CSD_GANGED_SNS_EN) */

#endif /* (CSD_ENABLE == CSD_CSDV2) */

#endif /* ((CSD_ENABLE == CSD_CSD_EN) || (CSD_ENABLE == CSD_CSD_CSX_EN)) */

/** \}
 * \endif */


#if ((CSD_ENABLE == CSD_CSD_EN) || (CSD_ENABLE == CSD_CSD_CSX_EN))

/*******************************************************************************
* Function Name: CSD_SsCSDPostScan
****************************************************************************//**
*
* \brief
*   This function reads rawdata and releases required HW resources after scan.
*
* \details
*   This function performs following tasks after scan:
*   - Reads SlotResult from Raw Counter;
*   - Inits bad Conversions number;
*   - Disconnects Vrefhi from AMUBUF positive input;
*   - Disconnects AMUBUF output from CSDBUSB with sych PHI2+HSCMP;
*   - Opens HCBV and HCBG switches.
*
*******************************************************************************/
CY_INLINE static void CSD_SsCSDPostScan(void)
{
    #if (CSD_ENABLE == CSD_CSDV2)

        uint32 tmpRawData;
        uint32 tmpMaxCount;
        CSD_RAM_WD_BASE_STRUCT const *ptrWdgt = (CSD_RAM_WD_BASE_STRUCT *)
                                            CSD_dsFlash.wdgtArray[CSD_widgetIndex].ptr2WdgtRam;

        /* Read SlotResult from Raw Counter */
        tmpRawData = CSD_RESULT_VAL1_VALUE_MASK & CY_GET_REG32(CSD_COUNTER_PTR);

        tmpMaxCount = ((1uL << ptrWdgt->resolution) - 1uL);
        if(tmpRawData < tmpMaxCount)
        {
            CSD_curRamSnsPtr->raw[CSD_scanFreqIndex] = LO16(tmpRawData);
        }
        else
        {
            CSD_curRamSnsPtr->raw[CSD_scanFreqIndex] = LO16(tmpMaxCount);
        }

        #if (CSD_ENABLE == CSD_CSD_NOISE_METRIC_EN)
            /*  Init bad Conversions number */
            CSD_badConversionsNum = CSD_BAD_CONVERSIONS_NUM;
        #endif /* (CSD_ENABLE == CSD_CSD_NOISE_METRIC_EN) */

        #if ((CSD_ENABLE == CSD_CSD_SHIELD_EN) && \
             (CSD_2000_MV <= CSD_CYDEV_VDDA_MV))
            /*  Disconnect Vrefhi from AMUBUF positive input. Disconnect AMUBUF output from CSDBUSB with sych PHI2+HSCMP  */
            CY_SET_REG32(CSD_SW_AMUXBUF_SEL_PTR, CSD_SW_AMUXBUF_SEL_SW_DEFAULT);
        #endif /* ((CSD_ENABLE == CSD_CSD_SHIELD_EN) && \
                   (CSD_2000_MV <= CSD_CYDEV_VDDA_MV)) */

        #if (CSD_ENABLE == CSD_CSD_SHIELD_EN)
            /* Open HCBV and HCBG switches */
            CY_SET_REG32(CSD_SW_SHIELD_SEL_PTR, CSD_SW_SHIELD_SEL_SW_HCBV_STATIC_OPEN |
                                                             CSD_SW_SHIELD_SEL_SW_HCBG_STATIC_OPEN);
        #endif /* (CSD_ENABLE == CSD_CSD_SHIELD_EN) */

    #else

        /* Read SlotResult from Raw Counter */
       CSD_curRamSnsPtr->raw[CSD_scanFreqIndex] = (uint16)CY_GET_REG32(CSD_COUNTER_PTR);

    #endif /* (CSD_ENABLE == CSD_CSDV2) */
}


/*******************************************************************************
* Function Name: CSD_SsCSDInitNextScan
****************************************************************************//**
*
* \brief
*   This function initializes the next sensor scan.
*
* \details
*   The function decrements the sensor index, updates sense clock for matrix
*   or touchpad widgets only, sets up Compensation IDAC, enables the sensor and
*   scans it. When all sensors are scanned it continues to set up the next widget
*   until all widgets are scanned. The CSD block is disabled when all widgets are
*   scanned.
*
*******************************************************************************/
CY_INLINE static void CSD_SsCSDInitNextScan(void)
{
    /*  Declare and initialise ptr to widget and sensor structures to appropriate address */
    #if (((CSD_ENABLE == CSD_CSD_IDAC_COMP_EN) || \
             (CSD_ENABLE == (CSD_CSD_MATRIX_WIDGET_EN | CSD_CSD_TOUCHPAD_WIDGET_EN))) || \
         (((CSD_DISABLE == CSD_CSD_COMMON_SNS_CLK_EN) && \
           (CSD_ENABLE == CSD_CSDV2) && \
          (CSD_ENABLE == (CSD_CSD_MATRIX_WIDGET_EN | CSD_CSD_TOUCHPAD_WIDGET_EN)))))
        CSD_RAM_WD_BASE_STRUCT *ptrWdgt = (CSD_RAM_WD_BASE_STRUCT *)
                                                        CSD_dsFlash.wdgtArray[CSD_widgetIndex].ptr2WdgtRam;
    #endif /* ((((CSD_ENABLE == CSD_CSD_IDAC_COMP_EN) && \
               (CSD_ENABLE == (CSD_CSD_MATRIX_WIDGET_EN | CSD_CSD_TOUCHPAD_WIDGET_EN)))) || \
               (((CSD_DISABLE == CSD_CSD_COMMON_SNS_CLK_EN) && \
               (CSD_ENABLE == (CSD_CSD_MATRIX_WIDGET_EN | CSD_CSD_TOUCHPAD_WIDGET_EN))) && \
               (CSD_CLK_SOURCE_DIRECT == CSD_CSD_SNS_CLK_SOURCE))) */

    /* Check if all sensors are scanned in widget */
    if (((uint8)CSD_dsFlash.wdgtArray[(CSD_widgetIndex)].totalNumSns - 1u) > CSD_sensorIndex)
    {
        /*  Decrement snsIndex to configure next sensor in widget */
        CSD_sensorIndex++;

        /*  Update global pointer to CSD_RAM_SNS_STRUCT to current sensor  */
        CSD_curRamSnsPtr = (CSD_RAM_SNS_STRUCT *)
                                                  CSD_dsFlash.wdgtArray[CSD_widgetIndex].ptr2SnsRam
                                                  + CSD_sensorIndex;

        /* Configure clock divider to row or column */
        #if ((CSD_DISABLE == CSD_CSD_COMMON_SNS_CLK_EN) && \
             (CSD_ENABLE == (CSD_CSD_MATRIX_WIDGET_EN | CSD_CSD_TOUCHPAD_WIDGET_EN)))
            if ((CSD_WD_TOUCHPAD_E == (CSD_WD_TYPE_ENUM)CSD_dsFlash.wdgtArray[(CSD_widgetIndex)].wdgtType) ||
                (CSD_WD_MATRIX_BUTTON_E == (CSD_WD_TYPE_ENUM)CSD_dsFlash.wdgtArray[(CSD_widgetIndex)].wdgtType))
            {
                CSD_SsCSDConfigClock();

                #if (CSD_ENABLE == CSD_CSDV2)
                     /* Set up scanning resolution */
                    CSD_SsCSDCalculateScanDuration(ptrWdgt);
                #endif /* (CSD_ENABLE == CSD_CSDV2) */
            }
        #endif /* ((CSD_DISABLE == CSD_CSD_COMMON_SNS_CLK_EN) && \
                   (CSD_ENABLE == (CSD_CSD_MATRIX_WIDGET_EN | CSD_CSD_TOUCHPAD_WIDGET_EN)))) */

        /* Setup Compensation IDAC for next sensor in widget */
        #if ((CSD_ENABLE == CSD_CSD_IDAC_COMP_EN) || \
             (CSD_ENABLE == (CSD_CSD_MATRIX_WIDGET_EN | CSD_CSD_TOUCHPAD_WIDGET_EN)))
            CSD_SsCSDSetUpIdacs(ptrWdgt);
        #endif /* ((CSD_ENABLE == CSD_CSD_IDAC_COMP_EN) || \
             (CSD_ENABLE == (CSD_CSD_MATRIX_WIDGET_EN | CSD_CSD_TOUCHPAD_WIDGET_EN)))*/

        /* Enable sensor */
        CSD_SsCSDConnectSensorExt((uint32)CSD_widgetIndex, (uint32)CSD_sensorIndex);

        /* Proceed scanning */
        CSD_SsCSDStartSample();
    }
    /*    Call scan next widget API if requested, if not, complete the scan  */
    else
    {
        CSD_sensorIndex = 0u;

        /* Current widget is totally scanned. Reset WIDGET BUSY flag */
        CSD_dsRam.status &= ~CSD_WDGT_SW_STS_BUSY;

        /* Check if all widgets have been scanned */
        if (CSD_ENABLE == CSD_requestScanAllWidget)
        {
            /* Configure and begin scanning next widget */
            CSD_SsPostAllWidgetsScan();
        }
        else
        {
            #if (CSD_ENABLE == CSD_BLOCK_OFF_AFTER_SCAN_EN)
                #if (CSD_ENABLE == CSD_CSDV2)
                    /*  Disable the CSD block */
                    CY_SET_REG32(CSD_CONFIG_PTR, CSD_configCsd);
                #else
                    /*  Disable the CSD block. Connect Vref Buffer to AMUX bus */
                    #if ((CSD_CSH_PRECHARGE_IO_BUF == CSD_CSD_CSH_PRECHARGE_SRC) &&\
                         (CSD_ENABLE == CSD_CSD_SHIELD_TANK_EN))
                        CY_SET_REG32(CSD_CONFIG_PTR, CSD_configCsd | CSD_CMOD_PRECHARGE_CONFIG);
                    #else
                        CY_SET_REG32(CSD_CONFIG_PTR, CSD_configCsd | CSD_CTANK_PRECHARGE_CONFIG);
                    #endif /* ((CSD_CSH_PRECHARGE_IO_BUF == CSD_CSD_CSH_PRECHARGE_SRC) &&\
                               (CSD_ENABLE == CSD_CSD_SHIELD_TANK_EN)) */
                #endif /* (CSD_ENABLE == CSD_CSDV2) */
            #endif /* (CSD_ENABLE == CSD_BLOCK_OFF_AFTER_SCAN_EN) */

            /* All widgets are totally scanned. Reset BUSY flag */
            CSD_dsRam.status &= ~CSD_SW_STS_BUSY;

            /* Update scan Counter */
            CSD_dsRam.scanCounter++;
        }
    }
}

#if (CSD_ENABLE == CSD_MULTI_FREQ_SCAN_EN)
    /*******************************************************************************
    * Function Name: CSD_SsNextFrequencyScan
    ****************************************************************************//**
    *
    * \brief
    *   This function scans the sensor on the next frequency channel.
    *
    * \details
    *   The function increments the frequency channel, changes IMO and initializes
    *   the scanning process of the same sensor.
    *
    *******************************************************************************/
    static void CSD_SsNextFrequencyScan(void)
    {
        CSD_RAM_WD_BASE_STRUCT const *ptrWdgt = (CSD_RAM_WD_BASE_STRUCT *)
                                                        CSD_dsFlash.wdgtArray[CSD_widgetIndex].ptr2WdgtRam;

        CSD_scanFreqIndex++;

        /* Set Immunity */
        CSD_SsChangeImoFreq((uint32)CSD_scanFreqIndex);

        /* Update IDAC registers */
        CSD_SsCSDSetUpIdacs(ptrWdgt);

        /* Proceed scanning */
        CSD_SsCSDStartSample();
    }
#endif /* (CSD_ENABLE == CSD_MULTI_FREQ_SCAN_EN) */

#endif /* ((CSD_ENABLE == CSD_CSD_EN) || (CSD_ENABLE == CSD_CSD_CSX_EN)) */


#if (CSD_ENABLE == CSD_CSD2X_EN)

    /*******************************************************************************
    * Function Name: CSD_CSD0PostMultiScan
    ****************************************************************************//**
    *
    * \brief
    *   This is an internal ISR function for multiple-sensor scanning implementation
    *
    * \details
    *   This ISR handler is triggered when the user calls the
    *   CSD_Scan() or CSD_ScanAllWidgets() APIs.
    *   The following tasks are performed:
    *   - Clear pending interrupt
    *   - Read the Counter register and update the data structure with raw data
    *   - Connect the Vref buffer to the AMUX bus
    *   - Disable the CSD block (after the widget has been scanned)
    *   - Update Scan Counter
    *   - Reset the BUSY flag
    *
    *   The ISR handler changes the IMO and initializes scanning for the next frequency
    *   channels when multi-frequency scanning is enabled.
    *
    *   This function has two Macro Callbacks that allow to call user code from macros
    *   specified in a component's generated code. Refer to Macro Callbacks section
    *   of PSoC Creator User Guide for details.
    *
    *******************************************************************************/
    CY_ISR(CSD_CSD0PostMultiScan)
    {
        #ifdef CSD_ENTRY_CALLBACK
            CSD_EntryCallback();
        #endif /* CSD_ENTRY_CALLBACK */

        /* Clear pending interrupt */
        CY_SET_REG32(CSD_CSD0_INTR_PTR, CSD_INTR_SET_MASK);
        CyIntClearPending(CSD_ISR0_NUMBER);

        /* Read Rawdata */
        CSD_curRamSnsPtr0->raw[CSD_scanFreqIndex] = (uint16)CY_GET_REG32(CSD_CSD0_COUNTER_PTR);

        /*  Connect Vref Buffer to AMUX bus */
        CY_SET_REG32(CSD_CSD0_CONFIG_PTR, CSD_configCsd0 | CSD_CTANK0_PRECHARGE_CONFIG_CSD_EN);

        /* The sensor0 of current scan slot is scanned. Reset CSD0 BUSY flag */
        CSD_dsRam.status &= ~CSD_STATUS_CSD0_MASK;
        
        #if (CSD_ENABLE == CSD_MULTI_FREQ_SCAN_EN)
            /* Scan the next channel or slot */
            CSD_SsCSD2XCheckNextScan();
        #else
            /* Scan the next slot */
            CSD_SsCSD2XInitNextScan();
        #endif /* (CSD_ENABLE == CSD_MULTI_FREQ_SCAN_EN) */

        #ifdef CSD_EXIT_CALLBACK
            CSD_ExitCallback();
        #endif /* CSD_EXIT_CALLBACK */
    }


    /*******************************************************************************
    * Function Name: CSD_CSD1PostMultiScan
    ****************************************************************************//**
    *
    * \brief
    *   This is an internal ISR function for multiple-sensor scanning implementation
    *
    * \details
    *   This ISR handler is triggered when the user calls the
    *   CSD_Scan() or CSD_ScanAllWidgets() APIs.
    *   The following tasks are performed:
    *   - Clear pending interrupt
    *   - Read the Counter register and update the data structure with raw data
    *   - Connect the Vref buffer to the AMUX bus
    *   - Disable the CSD block (after the widget has been scanned)
    *   - Update Scan Counter
    *   - Reset the BUSY flag
    *
    *   The ISR handler changes the IMO and initializes scanning for the next frequency
    *   channels when multi-frequency scanning is enabled.
    *
    *   This function has two Macro Callbacks that allow to call user code from macros
    *   specified in a component's generated code. Refer to Macro Callbacks section
    *   of PSoC Creator User Guide for details.
    *
    *******************************************************************************/
    CY_ISR(CSD_CSD1PostMultiScan)
    {
        #ifdef CSD_ENTRY_CALLBACK
            CSD_EntryCallback();
        #endif /* CSD_ENTRY_CALLBACK */

        /* Clear pending interrupt */
        CY_SET_REG32(CSD_CSD1_INTR_PTR, CSD_INTR_SET_MASK);
        CyIntClearPending(CSD_ISR1_NUMBER);

        /* Read Rawdata */
        CSD_curRamSnsPtr1->raw[CSD_scanFreqIndex] = (uint16)CY_GET_REG32(CSD_CSD1_COUNTER_PTR);

        /*  Connect Vref Buffer to AMUX bus */
        CY_SET_REG32(CSD_CSD1_CONFIG_PTR, CSD_configCsd1 | CSD_CTANK1_PRECHARGE_CONFIG_CSD_EN);

        /* The sensor0 of current scan slot is scanned. Reset CSD1 BUSY flag */
        CSD_dsRam.status &= ~CSD_STATUS_CSD1_MASK;
        
        #if (CSD_ENABLE == CSD_MULTI_FREQ_SCAN_EN)
            /* Scan the next channel or slot */
            CSD_SsCSD2XCheckNextScan();
        #else
            /* Scan the next slot */
            CSD_SsCSD2XInitNextScan();
        #endif /* (CSD_ENABLE == CSD_MULTI_FREQ_SCAN_EN) */

        #ifdef CSD_EXIT_CALLBACK
            CSD_ExitCallback();
        #endif /* CSD_EXIT_CALLBACK */
    }


    #if (CSD_ENABLE == CSD_CSD_GANGED_SNS_EN)
        /*******************************************************************************
        * Function Name: CSD_CSD0PostMultiScanGanged
        ****************************************************************************//**
        *
        * \brief
        *   This is an internal ISR function for multiple-sensor scanning implementation
        *
        * \details
        *   This ISR handler is triggered when the user calls the
        *   CSD_Scan() or CSD_ScanAllWidgets() APIs.
        *   The following tasks are performed:
        *   - Clear pending interrupt
        *   - Read the Counter register and update the data structure with raw data
        *   - Connect the Vref buffer to the AMUX bus
        *   - Disable the CSD block (after the widget has been scanned)
        *   - Update Scan Counter
        *   - Reset the BUSY flag
        *
        *   The ISR handler changes the IMO and initializes scanning for the next frequency
        *   channels when multi-frequency scanning is enabled.
        *
        *   This function has two Macro Callbacks that allow to call user code from macros
        *   specified in a component's generated code. Refer to Macro Callbacks section
        *   of PSoC Creator User Guide for details.
        *
        *******************************************************************************/
        CY_ISR(CSD_CSD0PostMultiScanGanged)
        {
            #ifdef CSD_ENTRY_CALLBACK
                CSD_EntryCallback();
            #endif /* CSD_ENTRY_CALLBACK */

            /* Clear pending interrupt */
            CY_SET_REG32(CSD_CSD0_INTR_PTR, CSD_INTR_SET_MASK);
            CyIntClearPending(CSD_ISR0_NUMBER);

            /* Read Rawdata */
            CSD_curRamSnsPtr0->raw[CSD_scanFreqIndex] = (uint16)CY_GET_REG32(CSD_CSD0_COUNTER_PTR);

            /*  Connect Vref Buffer to AMUX bus */
            CY_SET_REG32(CSD_CSD0_CONFIG_PTR, CSD_configCsd0 | CSD_CTANK0_PRECHARGE_CONFIG_CSD_EN);

            /* The sensor0 of current scan slot is scanned. Reset CSD0 BUSY flag */
            CSD_dsRam.status &= ~CSD_STATUS_CSD0_MASK;
            
            #if (CSD_ENABLE == CSD_MULTI_FREQ_SCAN_EN)
                /* Scan the next channel or slot */
                CSD_SsCSD2XCheckNextScan();
            #else
                /* Scan the next slot */
                CSD_SsCSD2XInitNextScan();
            #endif /* (CSD_ENABLE == CSD_MULTI_FREQ_SCAN_EN) */

            #ifdef CSD_EXIT_CALLBACK
                CSD_ExitCallback();
            #endif /* CSD_EXIT_CALLBACK */
        }


        /*******************************************************************************
        * Function Name: CSD_CSD1PostMultiScanGanged
        ****************************************************************************//**
        *
        * \brief
        *   This is an internal ISR function for multiple-sensor scanning implementation
        *
        * \details
        *   This ISR handler is triggered when the user calls the
        *   CSD_Scan() or CSD_ScanAllWidgets() APIs.
        *   The following tasks are performed:
        *   - Clear pending interrupt
        *   - Read the Counter register and update the data structure with raw data
        *   - Connect the Vref buffer to the AMUX bus
        *   - Disable the CSD block (after the widget has been scanned)
        *   - Update Scan Counter
        *   - Reset the BUSY flag
        *
        *   The ISR handler changes the IMO and initializes scanning for the next frequency
        *   channels when multi-frequency scanning is enabled.
        *
        *   This function has two Macro Callbacks that allow to call user code from macros
        *   specified in a component's generated code. Refer to Macro Callbacks section
        *   of PSoC Creator User Guide for details.
        *
        *******************************************************************************/
        CY_ISR(CSD_CSD1PostMultiScanGanged)
        {
            #ifdef CSD_ENTRY_CALLBACK
                CSD_EntryCallback();
            #endif /* CSD_ENTRY_CALLBACK */

            /* Clear pending interrupt */
            CY_SET_REG32(CSD_CSD1_INTR_PTR, CSD_INTR_SET_MASK);
            CyIntClearPending(CSD_ISR1_NUMBER);

            /* Read Rawdata */
            CSD_curRamSnsPtr1->raw[CSD_scanFreqIndex] = (uint16)CY_GET_REG32(CSD_CSD1_COUNTER_PTR);

            /*  Connect Vref Buffer to AMUX bus */
            CY_SET_REG32(CSD_CSD1_CONFIG_PTR, CSD_configCsd1 | CSD_CTANK1_PRECHARGE_CONFIG_CSD_EN);

            /* The sensor0 of current scan slot is scanned. Reset CSD1 BUSY flag */
            CSD_dsRam.status &= ~CSD_STATUS_CSD1_MASK;
            
            #if (CSD_ENABLE == CSD_MULTI_FREQ_SCAN_EN)
                /* Scan the next channel or slot */
                CSD_SsCSD2XCheckNextScan();
            #else
                /* Scan the next slot */
                CSD_SsCSD2XInitNextScan();
            #endif /* (CSD_ENABLE == CSD_MULTI_FREQ_SCAN_EN) */

            #ifdef CSD_EXIT_CALLBACK
                CSD_ExitCallback();
            #endif /* CSD_EXIT_CALLBACK */
        }
    #endif /* (CSD_ENABLE == CSD_CSD_GANGED_SNS_EN) */


    /*******************************************************************************
    * Function Name: CSD_SsCSD2XInitNextScan
    ****************************************************************************//**
    *
    * \brief
    *   This function initializes the next slot scan.
    *
    * \details
    *   The function checks if all slots scan is requested and configures the 
    *   next slot. If the all slots are scanned the function disables the CSD block 
    *   updates the scan Counter and resets the BUSY flag.
    * 
    *******************************************************************************/
    CY_INLINE static void CSD_SsCSD2XInitNextScan(void)
    {
        if (0u == (CSD_dsRam.status & (CSD_STATUS_CSD0_MASK | CSD_STATUS_CSD1_MASK)))
        {
            #if (CSD_ENABLE == CSD_MULTI_FREQ_SCAN_EN)
                 /* All channels are scanned. Set IMO to zero channel */
                CSD_scanFreqIndex = CSD_FREQ_CHANNEL_0;
                CSD_SsChangeImoFreq(CSD_FREQ_CHANNEL_0);
            #endif /* (CSD_ENABLE == CSD_MULTI_FREQ_SCAN_EN) */

            /* Check if all slots have been scanned */
            if (CSD_ENABLE == CSD_requestScanAllWidget)
            {
                /* Current slot is scanned. Reset WIDGET BUSY flag */
                CSD_dsRam.status &= ~CSD_WDGT_SW_STS_BUSY;
                
                /* Configure and begin scanning next slot */
                CSD_SsPostAllWidgetsScan();  
            }
            else
            {
                #if (CSD_ENABLE == CSD_BLOCK_OFF_AFTER_SCAN_EN)
                    /*  Disable the CSD blocks. Connect Vref Buffer to AMUX bus */
                    CY_SET_REG32(CSD_CSD0_CONFIG_PTR, CSD_configCsd0 | CSD_CTANK0_PRECHARGE_CONFIG);
                    CY_SET_REG32(CSD_CSD1_CONFIG_PTR, CSD_configCsd1 | CSD_CTANK1_PRECHARGE_CONFIG);
                #endif /* (CSD_ENABLE == CSD_BLOCK_OFF_AFTER_SCAN_EN) */

                /* All widgets are totally scanned. Reset BUSY flag */
                CSD_dsRam.status &= ~(CSD_SW_STS_BUSY | CSD_WDGT_SW_STS_BUSY);
                
                /* Update scan Counter */
                CSD_dsRam.scanCounter++;
            }
        }
    }


    #if (CSD_ENABLE == CSD_MULTI_FREQ_SCAN_EN)
        /*******************************************************************************
        * Function Name: CSD_SsCSD2XCheckNextScan
        ****************************************************************************//**
        *
        * \brief
        *   This function initializes the next channel or slot scan.
        *
        * \details
        *   The function checks if all channels are scanned and initializes the next 
        *   slot scan.
        * 
        *******************************************************************************/
        CY_INLINE static void CSD_SsCSD2XCheckNextScan(void)
        {
            if (CSD_FREQ_CHANNEL_2 > CSD_scanFreqIndex)
            {
                 /* Scan the next channel */
                CSD_SsCSD2XNextFrequencyScan();
            }
            else
            {
                 /* Scan the slot sensor */
                CSD_SsCSD2XInitNextScan();   
            } 
        }


        /*******************************************************************************
        * Function Name: CSD_SsCSD2XNextFrequencyScan
        ****************************************************************************//**
        *
        * \brief
        *   This function scans the sensor on the next frequency channel.
        *
        * \details
        *   The function increments the frequency channel, changes IMO and initializes 
        *   the scanning process of the same sensor.
        * 
        *******************************************************************************/
        static void CSD_SsCSD2XNextFrequencyScan(void)
        {
            if (0u == (CSD_dsRam.status & (CSD_STATUS_CSD0_MASK | CSD_STATUS_CSD1_MASK)))
            {        
                CSD_scanFreqIndex++;
                
                /* Set Immunity */
                CSD_SsChangeImoFreq((uint32)CSD_scanFreqIndex);
                
                /* Proceed scanning */
                CSD_SsCSD2XStartSample();
            }  
        }
    #endif /* (CSD_ENABLE == CSD_MULTI_FREQ_SCAN_EN) */    
#endif /* (CSD_ENABLE == CSD_CSD2X_EN) */


/* [] END OF FILE */
