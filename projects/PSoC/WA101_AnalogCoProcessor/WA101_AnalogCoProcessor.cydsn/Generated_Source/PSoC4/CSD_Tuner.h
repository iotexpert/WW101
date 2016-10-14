/***************************************************************************//**
* \file CSD_Tuner.h
* \version 3.10
*
* \brief
*   This file provides Tuner FW module function prototypes.
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

#if !defined(CY_CAPSENSE_CSD_TUNER_H)
#define CY_CAPSENSE_CSD_TUNER_H

#include "cytypes.h"

/* Defines the Tuner command codes */
typedef enum
{
    CSD_TU_CMD_NONE_E          = 0x0000u,
    CSD_TU_CMD_SUSPEND_E       = 0x0001u,
    CSD_TU_CMD_RESUME_E        = 0x0002u,
    CSD_TU_CMD_RESTART_E       = 0x0003u,
    CSD_TU_CMD_RUN_SNR_TEST_E  = 0x0004u,
    CSD_TU_CMD_PING_E          = 0x0005u
} CSD_TU_CMD_ENUM;

/* Defines the command complete bit */
#define CSD_TU_CMD_COMPLETE_BIT        (0x8000u)

/* Defines the status of restart command operation */
#define CSD_STATUS_RESTART_NONE        (0x00000000Lu)
#define CSD_STATUS_RESTART_DONE        (0x00000001Lu)

/*******************************************************************************
* Function Prototypes 
*******************************************************************************/

/**
* \if SECTION_CAPSENSE_HIGH_LEVEL
* \addtogroup group_capsense_high_level
* \{
*/

uint32 CSD_RunTuner(void);

/** \} 
* \endif */

/**
* \if SECTION_CAPSENSE_INTERNAL
* \addtogroup group_capsense_internal
* \{
*/

void CSD_TuInitialize(void);

/** \}
* \endif */

#endif /* End CY_CAPSENSE_CSD_TUNER_H */


/* [] END OF FILE */
