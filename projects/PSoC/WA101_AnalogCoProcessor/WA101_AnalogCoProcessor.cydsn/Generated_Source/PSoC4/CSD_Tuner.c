/***************************************************************************//**
* \file CSD_Tuner.c
* \version 3.10
*
* \brief
* This file provides the source code for the Tuner module API of the Component.
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
#include "CSD_Structure.h"
#include "CSD_Tuner.h"
#include "CSD_Control.h"

/* Tuner states definition */
#define TU_FSM_RUNNING      (0u)
#define TU_FSM_SUSPENDED    (1u)
#define TU_FSM_SNR_TEST     (2u)

/* Tuner state variable */
static uint32 tunerFsm = TU_FSM_RUNNING;


/*******************************************************************************
* Function Name: CSD_TuInitialize
****************************************************************************//**
*
* \brief
*   This function initializes the communication interface with the Tuner GUI.
*
* \details
*   This API sets the tuner state machine to the default state. It is called by
*   the CSD_Start() API.
*
*******************************************************************************/
void CSD_TuInitialize(void)
{
    volatile CSD_RAM_STRUCT *ptrDsRam = &CSD_dsRam;
    
    ptrDsRam->tunerCmd = (uint16)CSD_TU_CMD_NONE_E;
    tunerFsm = TU_FSM_RUNNING;
}

/*******************************************************************************
* Function Name: CSD_RunTuner
****************************************************************************//**
*
* \brief
*  Establishes synchronized communication with the Tuner application.
*
* \details
*  This function is used to establish synchronized communication between the 
*  CSD component and Tuner application (or other host controllers).
*  This function should be called periodically in the application program loop 
*  to serve the Tuner application (or host controller) requests and commands. 
*  In most cases, the best place to call this function is after processing and 
*  before next scanning. 
*  
*  If this function is absent in the application program, then communication is 
*  asynchronous and the following disadvantages are applicable:
*    - The raw counts displayed in the tuner may be filtered and/or unfiltered 
*      firmware and as a result, noise and SNR measurements will not be 
*      accurate.
*    - The Tuner may read the sensor data such as raw counts from a scan 
*      multiple times, as a result, noise and SNR measurement will not be 
*      accurate.
*    - The Tuner and host controller should not change the component parameters 
*      via the tuner interface. Changing the component parameters via the tuner 
*      interface in the async mode will result in abnormal behavior from the 
*      component.
*  
*  Note that calling this function is not mandatory for the application, but 
*  required only to synchronize the communication with the host controller or 
*  tuner application.
*
* \return
*  In some cases, the application program may need to know if the component was
*  re-initialized. The return indicates if a restart command was executed or not: 
*    - CSD_STATUS_RESTART_DONE based on a received command, the 
*      component was restarted.
*    -  CSD_STATUS_RESTART_NONE, no restart was executed by this 
*      function.
*
*******************************************************************************/
uint32 CSD_RunTuner(void)
{
    uint8  critSect;
    uint32 widgetId;
    uint32 sensorId;
    uint32 updateFlag = 0Lu;
    CSD_TU_CMD_ENUM command;
    uint32 tunerStatus = CSD_STATUS_RESTART_NONE;
    
    volatile CSD_RAM_STRUCT *ptrDsRam;
    volatile CSD_RAM_SNS_STRUCT *ptrSns;
    
    ptrDsRam = &CSD_dsRam;
    
    do 
    {
        command = (CSD_TU_CMD_ENUM)ptrDsRam->tunerCmd;
        
        /* Check command register */
        switch (command)
        {
        case CSD_TU_CMD_SUSPEND_E:
            tunerFsm = TU_FSM_SUSPENDED;
            updateFlag = 1Lu;
            break;
            
        case CSD_TU_CMD_RESUME_E:    
            tunerFsm = TU_FSM_RUNNING;
            updateFlag = 1Lu;
            break;
            
        case CSD_TU_CMD_RESTART_E:
            (void)CSD_Start();
            tunerStatus = CSD_STATUS_RESTART_DONE;
            tunerFsm = TU_FSM_RUNNING;
            updateFlag = 1Lu;
            break;
            
        case CSD_TU_CMD_RUN_SNR_TEST_E:
            tunerFsm = TU_FSM_SNR_TEST;
            updateFlag = 1Lu;
            break;
            
        case CSD_TU_CMD_PING_E:
            updateFlag = 1Lu;
            break;
            
        default:
            break;
        }
        
        /* Set Complete flag in command register if needed */
        if (0Lu != updateFlag)
        {
            critSect = CyEnterCriticalSection();
            
            /* Check if command wasn't overwritten with new command */
            if (command == (CSD_TU_CMD_ENUM)ptrDsRam->tunerCmd)
            {
                ptrDsRam->tunerCmd |= CSD_TU_CMD_COMPLETE_BIT;
            }
            CyExitCriticalSection(critSect);
            
            updateFlag = 0Lu;
        }
    } while (TU_FSM_SUSPENDED == tunerFsm);

    /* Check for SNR Test mode and do appropriate actions */
    if (TU_FSM_SNR_TEST == tunerFsm)
    {
        widgetId = ptrDsRam->snrTestWidgetId;
        sensorId = ptrDsRam->snrTestSensorId;
        
        /* Check if widgetId and sensorId are in range, 
         * and data was updated in RAM (scan counter changed) 
         */
        if ((widgetId < CSD_TOTAL_WIDGETS)              && 
            (sensorId < CSD_GET_SENSOR_COUNT(widgetId)) &&
            (ptrDsRam->scanCounter != ptrDsRam->snrTestScanCounter))
        {
            /* Get access to the Sensor Object */
            ptrSns = CSD_dsFlash.wdgtArray[widgetId].ptr2SnsRam;
            ptrSns += sensorId;
            
            /* Update data in SNR Test Object */
            critSect = CyEnterCriticalSection();
            ptrDsRam->snrTestScanCounter = ptrDsRam->scanCounter;
            ptrDsRam->snrTestRawCount[0u] = ptrSns->raw[0u];
            
            #if (0u != CSD_MULTI_FREQ_SCAN_EN)
                ptrDsRam->snrTestRawCount[1u] = ptrSns->raw[1u];
                ptrDsRam->snrTestRawCount[2u] = ptrSns->raw[2u];
            #endif
            
            CyExitCriticalSection(critSect);
        } 
    }
    return tunerStatus;
}


/* [] END OF FILE */
