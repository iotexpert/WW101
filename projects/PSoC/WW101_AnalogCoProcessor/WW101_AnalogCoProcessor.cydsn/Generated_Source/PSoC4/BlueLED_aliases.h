/*******************************************************************************
* File Name: BlueLED.h  
* Version 2.20
*
* Description:
*  This file contains the Alias definitions for Per-Pin APIs in cypins.h. 
*  Information on using these APIs can be found in the System Reference Guide.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_BlueLED_ALIASES_H) /* Pins BlueLED_ALIASES_H */
#define CY_PINS_BlueLED_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"


/***************************************
*              Constants        
***************************************/
#define BlueLED_0			(BlueLED__0__PC)
#define BlueLED_0_PS		(BlueLED__0__PS)
#define BlueLED_0_PC		(BlueLED__0__PC)
#define BlueLED_0_DR		(BlueLED__0__DR)
#define BlueLED_0_SHIFT	(BlueLED__0__SHIFT)
#define BlueLED_0_INTR	((uint16)((uint16)0x0003u << (BlueLED__0__SHIFT*2u)))

#define BlueLED_INTR_ALL	 ((uint16)(BlueLED_0_INTR))


#endif /* End Pins BlueLED_ALIASES_H */


/* [] END OF FILE */
