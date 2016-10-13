/*******************************************************************************
* File Name: ADC_intSarClock.h
* Version 2.20
*
*  Description:
*   Provides the function and constant definitions for the clock component.
*
*  Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_CLOCK_ADC_intSarClock_H)
#define CY_CLOCK_ADC_intSarClock_H

#include <cytypes.h>
#include <cyfitter.h>


/***************************************
*        Function Prototypes
***************************************/
#if defined CYREG_PERI_DIV_CMD

void ADC_intSarClock_StartEx(uint32 alignClkDiv);
#define ADC_intSarClock_Start() \
    ADC_intSarClock_StartEx(ADC_intSarClock__PA_DIV_ID)

#else

void ADC_intSarClock_Start(void);

#endif/* CYREG_PERI_DIV_CMD */

void ADC_intSarClock_Stop(void);

void ADC_intSarClock_SetFractionalDividerRegister(uint16 clkDivider, uint8 clkFractional);

uint16 ADC_intSarClock_GetDividerRegister(void);
uint8  ADC_intSarClock_GetFractionalDividerRegister(void);

#define ADC_intSarClock_Enable()                         ADC_intSarClock_Start()
#define ADC_intSarClock_Disable()                        ADC_intSarClock_Stop()
#define ADC_intSarClock_SetDividerRegister(clkDivider, reset)  \
    ADC_intSarClock_SetFractionalDividerRegister((clkDivider), 0u)
#define ADC_intSarClock_SetDivider(clkDivider)           ADC_intSarClock_SetDividerRegister((clkDivider), 1u)
#define ADC_intSarClock_SetDividerValue(clkDivider)      ADC_intSarClock_SetDividerRegister((clkDivider) - 1u, 1u)


/***************************************
*             Registers
***************************************/
#if defined CYREG_PERI_DIV_CMD

#define ADC_intSarClock_DIV_ID     ADC_intSarClock__DIV_ID

#define ADC_intSarClock_CMD_REG    (*(reg32 *)CYREG_PERI_DIV_CMD)
#define ADC_intSarClock_CTRL_REG   (*(reg32 *)ADC_intSarClock__CTRL_REGISTER)
#define ADC_intSarClock_DIV_REG    (*(reg32 *)ADC_intSarClock__DIV_REGISTER)

#define ADC_intSarClock_CMD_DIV_SHIFT          (0u)
#define ADC_intSarClock_CMD_PA_DIV_SHIFT       (8u)
#define ADC_intSarClock_CMD_DISABLE_SHIFT      (30u)
#define ADC_intSarClock_CMD_ENABLE_SHIFT       (31u)

#define ADC_intSarClock_CMD_DISABLE_MASK       ((uint32)((uint32)1u << ADC_intSarClock_CMD_DISABLE_SHIFT))
#define ADC_intSarClock_CMD_ENABLE_MASK        ((uint32)((uint32)1u << ADC_intSarClock_CMD_ENABLE_SHIFT))

#define ADC_intSarClock_DIV_FRAC_MASK  (0x000000F8u)
#define ADC_intSarClock_DIV_FRAC_SHIFT (3u)
#define ADC_intSarClock_DIV_INT_MASK   (0xFFFFFF00u)
#define ADC_intSarClock_DIV_INT_SHIFT  (8u)

#else 

#define ADC_intSarClock_DIV_REG        (*(reg32 *)ADC_intSarClock__REGISTER)
#define ADC_intSarClock_ENABLE_REG     ADC_intSarClock_DIV_REG
#define ADC_intSarClock_DIV_FRAC_MASK  ADC_intSarClock__FRAC_MASK
#define ADC_intSarClock_DIV_FRAC_SHIFT (16u)
#define ADC_intSarClock_DIV_INT_MASK   ADC_intSarClock__DIVIDER_MASK
#define ADC_intSarClock_DIV_INT_SHIFT  (0u)

#endif/* CYREG_PERI_DIV_CMD */

#endif /* !defined(CY_CLOCK_ADC_intSarClock_H) */

/* [] END OF FILE */
