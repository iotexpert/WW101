/*******************************************************************************
* File Name: Pin_Vhi.c  
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
#include "Pin_Vhi.h"

static Pin_Vhi_BACKUP_STRUCT  Pin_Vhi_backup = {0u, 0u, 0u};


/*******************************************************************************
* Function Name: Pin_Vhi_Sleep
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
*  \snippet Pin_Vhi_SUT.c usage_Pin_Vhi_Sleep_Wakeup
*******************************************************************************/
void Pin_Vhi_Sleep(void)
{
    #if defined(Pin_Vhi__PC)
        Pin_Vhi_backup.pcState = Pin_Vhi_PC;
    #else
        #if (CY_PSOC4_4200L)
            /* Save the regulator state and put the PHY into suspend mode */
            Pin_Vhi_backup.usbState = Pin_Vhi_CR1_REG;
            Pin_Vhi_USB_POWER_REG |= Pin_Vhi_USBIO_ENTER_SLEEP;
            Pin_Vhi_CR1_REG &= Pin_Vhi_USBIO_CR1_OFF;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(Pin_Vhi__SIO)
        Pin_Vhi_backup.sioState = Pin_Vhi_SIO_REG;
        /* SIO requires unregulated output buffer and single ended input buffer */
        Pin_Vhi_SIO_REG &= (uint32)(~Pin_Vhi_SIO_LPM_MASK);
    #endif  
}


/*******************************************************************************
* Function Name: Pin_Vhi_Wakeup
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
*  Refer to Pin_Vhi_Sleep() for an example usage.
*******************************************************************************/
void Pin_Vhi_Wakeup(void)
{
    #if defined(Pin_Vhi__PC)
        Pin_Vhi_PC = Pin_Vhi_backup.pcState;
    #else
        #if (CY_PSOC4_4200L)
            /* Restore the regulator state and come out of suspend mode */
            Pin_Vhi_USB_POWER_REG &= Pin_Vhi_USBIO_EXIT_SLEEP_PH1;
            Pin_Vhi_CR1_REG = Pin_Vhi_backup.usbState;
            Pin_Vhi_USB_POWER_REG &= Pin_Vhi_USBIO_EXIT_SLEEP_PH2;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(Pin_Vhi__SIO)
        Pin_Vhi_SIO_REG = Pin_Vhi_backup.sioState;
    #endif
}


/* [] END OF FILE */
