/***************************************************************************//**
* \file PVref.c
* \version 1.0
*
* \brief
*  This file provides the source code to the API for the Programmable
*  Voltage Reference component.
*
* Note:
*  None
*
********************************************************************************
* \copyright
* Copyright 2016, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "PVref.h"

uint8 PVref_initVar = 0u;


/*******************************************************************************
* Function Name: PVref_Init
****************************************************************************//**
*
*  Initialize/Restore the default Voltage Reference configuration as defined
*  in the customizer. If the reference is set to Vdda, it will enable all
*  Vdda-based references.
*
* \sideeffect
*  If the reference is Vdda-based, the function will enable all Vdda-based
*  references because they share the same resistor tree.
*
*******************************************************************************/
void PVref_Init(void)
{
    PVref_PRB_CTRL_REG |= PVref_PRB_CTRL_REG_INIT;
    PVref_PRB_REF_REG = PVref_PRB_REF_REG_INIT;
}


/*******************************************************************************
* Function Name: PVref_Enable
****************************************************************************//**
*
*  Enables the Voltage Reference.
*
*******************************************************************************/
void PVref_Enable(void)
{
    PVref_PRB_REF_REG |= PVref_VREF_ENABLE;
}


/*******************************************************************************
* Function Name: PVref_Start
****************************************************************************//**
*
*  Initializes all parameters required to setup the component as defined in the
*  customizer. If the reference is set to Vdda, it will enable all Vdda based
*  references.
*
* \globalvars
*  \ref PVref_initVar - This variable is used to indicate the initial
*  configuration of this component. The variable is initialized to zero and
*  set to 1 the first time PVref_Start() is called. This allows
*  the component initialization without re-initialization in all subsequent
*  calls to the PVref_Start() routine.
*
* \sideeffect
*  If the reference is Vdda-based, the function will enable all Vdda-based
*  references because they share the same resistor tree.
*
*******************************************************************************/
void PVref_Start(void)
{
    /* If not Initialized, then initialize all required hardware and software. */
    if (PVref_initVar == 0u)
    {
        PVref_initVar = 1u;
        PVref_Init();
    }

    PVref_Enable();
}


/*******************************************************************************
* Function Name: PVref_Stop
****************************************************************************//**
*
*  Disables the Voltage Reference.
*
*******************************************************************************/
void PVref_Stop(void)
{
    PVref_PRB_REF_REG &= ~PVref_VREF_ENABLE;
}


/*******************************************************************************
* Function Name: PVref_SetTap
****************************************************************************//**
*
*  Selects which of the equally spaced resistor divider taps will be used for
*  the reference. The output will range in 1/16th steps between Vref/16 to Vref.
*
*  \param
*  dividerTap:    One of the 16-tap voltage dividers to select a voltage
*                 between 1/16 of the reference source to the full reference
*                 source in steps of 1/16.
* * 1   :    1/16 Vref
* * ... :    ...
* * 16  :    Vref
*
*******************************************************************************/
void PVref_SetTap(uint32 dividerTap)
{
    PVref_PRB_REF_REG = (PVref_PRB_REF_REG & ~PVref_VREF_LEVEL_SEL_MASK) |
        (((dividerTap - 1u) << PVref_VREF_LEVEL_SEL_MASK_POS) & PVref_VREF_LEVEL_SEL_MASK);
}


/*******************************************************************************
* Function Name: PVref_SetVRefSrc
****************************************************************************//**
*
*  Selects whether the reference at the top of the resistor divider is based
*  on the bandgap or Vdda.
*
*  \param
*  reference:    The reference source.
* * PVref_BANDGAP_REFERENCE :    Bandgap reference
* * PVref_VDDA_REFERENCE    :    Vdda reference
*
* \sideeffect
*  If the reference is based on Vdda, it will affect all Vdda-based references
*  because they share the same resistor tree.
*
*******************************************************************************/
void PVref_SetVRefSrc(uint32 reference)
{
    if (PVref_BANDGAP_REFERENCE == reference)
    {
        PVref_PRB_REF_REG &= ~PVref_VREF_SUPPLY_SEL;
    }
    else /* Vdda reference source */
    {
        PVref_PRB_CTRL_REG |= PVref_VDDA_ENABLE;
        PVref_PRB_REF_REG |= PVref_VREF_SUPPLY_SEL;
    }
}


/* [] END OF FILE */
