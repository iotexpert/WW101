/***************************************************************************//**
* \file EZI2C_PM.c
* \version 3.20
*
* \brief
*  This file provides the source code to the Power Management support for
*  the SCB Component.
*
* Note:
*
********************************************************************************
* \copyright
* Copyright 2013-2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "EZI2C.h"
#include "EZI2C_PVT.h"

#if(EZI2C_SCB_MODE_I2C_INC)
    #include "EZI2C_I2C_PVT.h"
#endif /* (EZI2C_SCB_MODE_I2C_INC) */

#if(EZI2C_SCB_MODE_EZI2C_INC)
    #include "EZI2C_EZI2C_PVT.h"
#endif /* (EZI2C_SCB_MODE_EZI2C_INC) */

#if(EZI2C_SCB_MODE_SPI_INC || EZI2C_SCB_MODE_UART_INC)
    #include "EZI2C_SPI_UART_PVT.h"
#endif /* (EZI2C_SCB_MODE_SPI_INC || EZI2C_SCB_MODE_UART_INC) */


/***************************************
*   Backup Structure declaration
***************************************/

#if(EZI2C_SCB_MODE_UNCONFIG_CONST_CFG || \
   (EZI2C_SCB_MODE_I2C_CONST_CFG   && (!EZI2C_I2C_WAKE_ENABLE_CONST))   || \
   (EZI2C_SCB_MODE_EZI2C_CONST_CFG && (!EZI2C_EZI2C_WAKE_ENABLE_CONST)) || \
   (EZI2C_SCB_MODE_SPI_CONST_CFG   && (!EZI2C_SPI_WAKE_ENABLE_CONST))   || \
   (EZI2C_SCB_MODE_UART_CONST_CFG  && (!EZI2C_UART_WAKE_ENABLE_CONST)))

    EZI2C_BACKUP_STRUCT EZI2C_backup =
    {
        0u, /* enableState */
    };
#endif


/*******************************************************************************
* Function Name: EZI2C_Sleep
****************************************************************************//**
*
*  Prepares the EZI2C component to enter Deep Sleep.
*  The “Enable wakeup from Deep Sleep Mode” selection has an influence on this 
*  function implementation:
*  - Checked: configures the component to be wakeup source from Deep Sleep.
*  - Unchecked: stores the current component state (enabled or disabled) and 
*    disables the component. See SCB_Stop() function for details about component 
*    disabling.
*
*  Call the EZI2C_Sleep() function before calling the 
*  CyPmSysDeepSleep() function. 
*  Refer to the PSoC Creator System Reference Guide for more information about 
*  power management functions and Low power section of this document for the 
*  selected mode.
*
*  This function should not be called before entering Sleep.
*
*******************************************************************************/
void EZI2C_Sleep(void)
{
#if(EZI2C_SCB_MODE_UNCONFIG_CONST_CFG)

    if(EZI2C_SCB_WAKE_ENABLE_CHECK)
    {
        if(EZI2C_SCB_MODE_I2C_RUNTM_CFG)
        {
            EZI2C_I2CSaveConfig();
        }
        else if(EZI2C_SCB_MODE_EZI2C_RUNTM_CFG)
        {
            EZI2C_EzI2CSaveConfig();
        }
    #if(!EZI2C_CY_SCBIP_V1)
        else if(EZI2C_SCB_MODE_SPI_RUNTM_CFG)
        {
            EZI2C_SpiSaveConfig();
        }
        else if(EZI2C_SCB_MODE_UART_RUNTM_CFG)
        {
            EZI2C_UartSaveConfig();
        }
    #endif /* (!EZI2C_CY_SCBIP_V1) */
        else
        {
            /* Unknown mode */
        }
    }
    else
    {
        EZI2C_backup.enableState = (uint8) EZI2C_GET_CTRL_ENABLED;

        if(0u != EZI2C_backup.enableState)
        {
            EZI2C_Stop();
        }
    }

#else

    #if (EZI2C_SCB_MODE_I2C_CONST_CFG && EZI2C_I2C_WAKE_ENABLE_CONST)
        EZI2C_I2CSaveConfig();

    #elif (EZI2C_SCB_MODE_EZI2C_CONST_CFG && EZI2C_EZI2C_WAKE_ENABLE_CONST)
        EZI2C_EzI2CSaveConfig();

    #elif (EZI2C_SCB_MODE_SPI_CONST_CFG && EZI2C_SPI_WAKE_ENABLE_CONST)
        EZI2C_SpiSaveConfig();

    #elif (EZI2C_SCB_MODE_UART_CONST_CFG && EZI2C_UART_WAKE_ENABLE_CONST)
        EZI2C_UartSaveConfig();

    #else

        EZI2C_backup.enableState = (uint8) EZI2C_GET_CTRL_ENABLED;

        if(0u != EZI2C_backup.enableState)
        {
            EZI2C_Stop();
        }

    #endif /* defined (EZI2C_SCB_MODE_I2C_CONST_CFG) && (EZI2C_I2C_WAKE_ENABLE_CONST) */

#endif /* (EZI2C_SCB_MODE_UNCONFIG_CONST_CFG) */
}


/*******************************************************************************
* Function Name: EZI2C_Wakeup
****************************************************************************//**
*
*  Prepares the EZI2C component for Active mode operation after 
*  Deep Sleep.
*  The “Enable wakeup from Deep Sleep Mode” selection has influence on this 
*  function implementation:
*  - Checked: restores the component Active mode configuration.
*  - Unchecked: enables the component if it was enabled before enter Deep Sleep.
*
*  This function should not be called after exiting Sleep.
*
*  \sideeffect
*   Calling the EZI2C_Wakeup() function without first calling the 
*   EZI2C_Sleep() function may produce unexpected behavior.
*
*******************************************************************************/
void EZI2C_Wakeup(void)
{
#if(EZI2C_SCB_MODE_UNCONFIG_CONST_CFG)

    if(EZI2C_SCB_WAKE_ENABLE_CHECK)
    {
        if(EZI2C_SCB_MODE_I2C_RUNTM_CFG)
        {
            EZI2C_I2CRestoreConfig();
        }
        else if(EZI2C_SCB_MODE_EZI2C_RUNTM_CFG)
        {
            EZI2C_EzI2CRestoreConfig();
        }
    #if(!EZI2C_CY_SCBIP_V1)
        else if(EZI2C_SCB_MODE_SPI_RUNTM_CFG)
        {
            EZI2C_SpiRestoreConfig();
        }
        else if(EZI2C_SCB_MODE_UART_RUNTM_CFG)
        {
            EZI2C_UartRestoreConfig();
        }
    #endif /* (!EZI2C_CY_SCBIP_V1) */
        else
        {
            /* Unknown mode */
        }
    }
    else
    {
        if(0u != EZI2C_backup.enableState)
        {
            EZI2C_Enable();
        }
    }

#else

    #if (EZI2C_SCB_MODE_I2C_CONST_CFG  && EZI2C_I2C_WAKE_ENABLE_CONST)
        EZI2C_I2CRestoreConfig();

    #elif (EZI2C_SCB_MODE_EZI2C_CONST_CFG && EZI2C_EZI2C_WAKE_ENABLE_CONST)
        EZI2C_EzI2CRestoreConfig();

    #elif (EZI2C_SCB_MODE_SPI_CONST_CFG && EZI2C_SPI_WAKE_ENABLE_CONST)
        EZI2C_SpiRestoreConfig();

    #elif (EZI2C_SCB_MODE_UART_CONST_CFG && EZI2C_UART_WAKE_ENABLE_CONST)
        EZI2C_UartRestoreConfig();

    #else

        if(0u != EZI2C_backup.enableState)
        {
            EZI2C_Enable();
        }

    #endif /* (EZI2C_I2C_WAKE_ENABLE_CONST) */

#endif /* (EZI2C_SCB_MODE_UNCONFIG_CONST_CFG) */
}


/* [] END OF FILE */
