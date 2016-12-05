/*******************************************************************************
* File Name: BlueLED.c  
* Version 2.20
*
* Description:
*  This file contains APIs to set up the Pins component for low power modes.
*
* Note:
*
********************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "cytypes.h"
#include "BlueLED.h"

static BlueLED_BACKUP_STRUCT  BlueLED_backup = {0u, 0u, 0u};


/*******************************************************************************
* Function Name: BlueLED_Sleep
****************************************************************************//**
*
* \brief Stores the pin configuration and prepares the pin for entering chip 
*  deep-sleep/hibernate modes. This function must be called for SIO and USBIO
*  pins. It is not essential if using GPIO or GPIO_OVT pins.
*
* <b>Note</b> This function is available in PSoC 4 only.
*
* \return 
*  None 
*  
* \sideeffect
*  For SIO pins, this function configures the pin input threshold to CMOS and
*  drive level to Vddio. This is needed for SIO pins when in device 
*  deep-sleep/hibernate modes.
*
* \funcusage
*  \snippet BlueLED_SUT.c usage_BlueLED_Sleep_Wakeup
*******************************************************************************/
void BlueLED_Sleep(void)
{
    #if defined(BlueLED__PC)
        BlueLED_backup.pcState = BlueLED_PC;
    #else
        #if (CY_PSOC4_4200L)
            /* Save the regulator state and put the PHY into suspend mode */
            BlueLED_backup.usbState = BlueLED_CR1_REG;
            BlueLED_USB_POWER_REG |= BlueLED_USBIO_ENTER_SLEEP;
            BlueLED_CR1_REG &= BlueLED_USBIO_CR1_OFF;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(BlueLED__SIO)
        BlueLED_backup.sioState = BlueLED_SIO_REG;
        /* SIO requires unregulated output buffer and single ended input buffer */
        BlueLED_SIO_REG &= (uint32)(~BlueLED_SIO_LPM_MASK);
    #endif  
}


/*******************************************************************************
* Function Name: BlueLED_Wakeup
****************************************************************************//**
*
* \brief Restores the pin configuration that was saved during Pin_Sleep().
*
* For USBIO pins, the wakeup is only triggered for falling edge interrupts.
*
* <b>Note</b> This function is available in PSoC 4 only.
*
* \return 
*  None
*  
* \funcusage
*  Refer to BlueLED_Sleep() for an example usage.
*******************************************************************************/
void BlueLED_Wakeup(void)
{
    #if defined(BlueLED__PC)
        BlueLED_PC = BlueLED_backup.pcState;
    #else
        #if (CY_PSOC4_4200L)
            /* Restore the regulator state and come out of suspend mode */
            BlueLED_USB_POWER_REG &= BlueLED_USBIO_EXIT_SLEEP_PH1;
            BlueLED_CR1_REG = BlueLED_backup.usbState;
            BlueLED_USB_POWER_REG &= BlueLED_USBIO_EXIT_SLEEP_PH2;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(BlueLED__SIO)
        BlueLED_SIO_REG = BlueLED_backup.sioState;
    #endif
}


/* [] END OF FILE */
