/*******************************************************************************
* File Name: D0.c  
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
#include "D0.h"

static D0_BACKUP_STRUCT  D0_backup = {0u, 0u, 0u};


/*******************************************************************************
* Function Name: D0_Sleep
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
*  \snippet D0_SUT.c usage_D0_Sleep_Wakeup
*******************************************************************************/
void D0_Sleep(void)
{
    #if defined(D0__PC)
        D0_backup.pcState = D0_PC;
    #else
        #if (CY_PSOC4_4200L)
            /* Save the regulator state and put the PHY into suspend mode */
            D0_backup.usbState = D0_CR1_REG;
            D0_USB_POWER_REG |= D0_USBIO_ENTER_SLEEP;
            D0_CR1_REG &= D0_USBIO_CR1_OFF;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(D0__SIO)
        D0_backup.sioState = D0_SIO_REG;
        /* SIO requires unregulated output buffer and single ended input buffer */
        D0_SIO_REG &= (uint32)(~D0_SIO_LPM_MASK);
    #endif  
}


/*******************************************************************************
* Function Name: D0_Wakeup
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
*  Refer to D0_Sleep() for an example usage.
*******************************************************************************/
void D0_Wakeup(void)
{
    #if defined(D0__PC)
        D0_PC = D0_backup.pcState;
    #else
        #if (CY_PSOC4_4200L)
            /* Restore the regulator state and come out of suspend mode */
            D0_USB_POWER_REG &= D0_USBIO_EXIT_SLEEP_PH1;
            D0_CR1_REG = D0_backup.usbState;
            D0_USB_POWER_REG &= D0_USBIO_EXIT_SLEEP_PH2;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(D0__SIO)
        D0_SIO_REG = D0_backup.sioState;
    #endif
}


/* [] END OF FILE */
