/*******************************************************************************
* File Name: GreenLED.h  
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

#if !defined(CY_PINS_GreenLED_ALIASES_H) /* Pins GreenLED_ALIASES_H */
#define CY_PINS_GreenLED_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"


/***************************************
*              Constants        
***************************************/
#define GreenLED_0			(GreenLED__0__PC)
#define GreenLED_0_PS		(GreenLED__0__PS)
#define GreenLED_0_PC		(GreenLED__0__PC)
#define GreenLED_0_DR		(GreenLED__0__DR)
#define GreenLED_0_SHIFT	(GreenLED__0__SHIFT)
#define GreenLED_0_INTR	((uint16)((uint16)0x0003u << (GreenLED__0__SHIFT*2u)))

#define GreenLED_INTR_ALL	 ((uint16)(GreenLED_0_INTR))


#endif /* End Pins GreenLED_ALIASES_H */


/* [] END OF FILE */
