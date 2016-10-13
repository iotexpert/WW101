/*******************************************************************************
* File Name: VrefBuffer.c
* Version 1.20
*
* Description:
*  This file provides the source code to the API for the Opamp (Analog Buffer)
*  Component.
*
*
********************************************************************************
* Copyright 2013-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "VrefBuffer.h"

uint8 VrefBuffer_initVar = 0u; /* Defines if component was initialized */
static uint32 VrefBuffer_internalPower = 0u; /* Defines component Power value */


/*******************************************************************************
* Function Name: VrefBuffer_Init
********************************************************************************
*
* Summary:
*  Initializes or restores the component according to the customizer Configure 
*  dialog settings. It is not necessary to call Init() because the Start() API 
*  calls this function and is the preferred method to begin the component operation.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void VrefBuffer_Init(void)
{
    VrefBuffer_internalPower = VrefBuffer_POWER;
    VrefBuffer_CTB_CTRL_REG = VrefBuffer_DEFAULT_CTB_CTRL;
    VrefBuffer_OA_RES_CTRL_REG = VrefBuffer_DEFAULT_OA_RES_CTRL;
    VrefBuffer_OA_COMP_TRIM_REG = VrefBuffer_DEFAULT_OA_COMP_TRIM_REG;
}


/*******************************************************************************
* Function Name: VrefBuffer_Enable
********************************************************************************
*
* Summary:
*  Activates the hardware and begins the component operation. It is not necessary to 
*  call Enable() because the Start() API calls this function, which is the 
*  preferred method to begin the component operation.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void VrefBuffer_Enable(void)
{
    VrefBuffer_OA_RES_CTRL_REG |= VrefBuffer_internalPower | \
                                        VrefBuffer_OA_PUMP_EN;
}


/*******************************************************************************
* Function Name: VrefBuffer_Start
********************************************************************************
*
* Summary:
*  Performs all of the required initialization for the component and enables power 
*  to the block. The first time the routine is executed, the Power level, Mode, 
*  and Output mode are set. When called to restart the Opamp following a Stop() call, 
*  the current component parameter settings are retained.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  VrefBuffer_initVar: Used to check the initial configuration, modified
*  when this function is called for the first time.
*
*******************************************************************************/
void VrefBuffer_Start(void)
{
    if( 0u == VrefBuffer_initVar)
    {
        VrefBuffer_Init();
        VrefBuffer_initVar = 1u;
    }
    VrefBuffer_Enable();
}


/*******************************************************************************
* Function Name: VrefBuffer_Stop
********************************************************************************
*
* Summary:
*  Turn off the Opamp block.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void VrefBuffer_Stop(void)
{
    VrefBuffer_OA_RES_CTRL_REG &= ((uint32)~(VrefBuffer_OA_PWR_MODE_MASK | \
                                                   VrefBuffer_OA_PUMP_EN));
}


/*******************************************************************************
* Function Name: VrefBuffer_SetPower
********************************************************************************
*
* Summary:
*  Sets the Opamp to one of the three power levels.
*
* Parameters:
*  power: power levels.
*   VrefBuffer_LOW_POWER - Lowest active power
*   VrefBuffer_MED_POWER - Medium power
*   VrefBuffer_HIGH_POWER - Highest active power
*
* Return:
*  None
*
**********************************************************************************/
void VrefBuffer_SetPower(uint32 power)
{
    uint32 tmp;
    
    VrefBuffer_internalPower = VrefBuffer_GET_OA_PWR_MODE(power);
    tmp = VrefBuffer_OA_RES_CTRL_REG & \
           (uint32)~VrefBuffer_OA_PWR_MODE_MASK;
    VrefBuffer_OA_RES_CTRL_REG = tmp | VrefBuffer_internalPower;
}


/*******************************************************************************
* Function Name: VrefBuffer_PumpControl
********************************************************************************
*
* Summary:
*  Allows the user to turn the Opamp's boost pump on or off. By Default the Start() 
*  function turns on the pump. Use this API to turn it off. The boost must be 
*  turned on when the supply is less than 2.7 volts and off if the supply is more 
*  than 4 volts.
*
* Parameters:
*  onOff: Control the pump.
*   VrefBuffer_PUMP_OFF - Turn off the pump
*   VrefBuffer_PUMP_ON - Turn on the pump
*
* Return:
*  None
*
**********************************************************************************/
void VrefBuffer_PumpControl(uint32 onOff)
{
    
    if(0u != onOff)
    {
        VrefBuffer_OA_RES_CTRL |= VrefBuffer_OA_PUMP_EN;    
    }
    else
    {
        VrefBuffer_OA_RES_CTRL &= (uint32)~VrefBuffer_OA_PUMP_EN;
    }
}


/* [] END OF FILE */
