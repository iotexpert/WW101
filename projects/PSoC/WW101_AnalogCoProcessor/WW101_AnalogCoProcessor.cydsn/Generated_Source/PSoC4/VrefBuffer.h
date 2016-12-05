/*******************************************************************************
* File Name: VrefBuffer.h
* Version 1.20
*
* Description:
*  This file contains the function prototypes and constants used in
*  the Opamp (Analog Buffer) Component.
*
*
********************************************************************************
* Copyright 2013-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_OPAMP_VrefBuffer_H)
#define CY_OPAMP_VrefBuffer_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*       Type Definitions
***************************************/

/* Structure to save state before go to sleep */
typedef struct
{
    uint8  enableState;
} VrefBuffer_BACKUP_STRUCT;


/**************************************
*        Function Prototypes
**************************************/
void VrefBuffer_Init(void);
void VrefBuffer_Enable(void);
void VrefBuffer_Start(void);
void VrefBuffer_Stop(void);
void VrefBuffer_SetPower(uint32 power);
void VrefBuffer_PumpControl(uint32 onOff);
void VrefBuffer_Sleep(void);
void VrefBuffer_Wakeup(void);
void VrefBuffer_SaveConfig(void);
void VrefBuffer_RestoreConfig(void);


/**************************************
*           API Constants
**************************************/

/* Parameters for SetPower() function */
#define VrefBuffer_LOW_POWER      (1u)
#define VrefBuffer_MED_POWER      (2u)
#define VrefBuffer_HIGH_POWER     (3u)


/* Parameters for PumpControl() function */
#define VrefBuffer_PUMP_ON        (1u)
#define VrefBuffer_PUMP_OFF       (0u)


/***************************************
*   Initial Parameter Constants
****************************************/

#define VrefBuffer_OUTPUT_CURRENT         (1u)
#define VrefBuffer_POWER                  (3u)
#define VrefBuffer_MODE                   (1u)
#define VrefBuffer_OA_COMP_TRIM_VALUE     (3u)
#define VrefBuffer_DEEPSLEEP_SUPPORT      (0u)


/***************************************
*    Variables with External Linkage
***************************************/

extern uint8  VrefBuffer_initVar;


/**************************************
*             Registers
**************************************/

#ifdef CYIPBLOCK_m0s8pass4b_VERSION
    #define VrefBuffer_CTB_CTRL_REG       (*(reg32 *) VrefBuffer_cy_psoc4_abuf__CTB_CTB_CTRL)
    #define VrefBuffer_CTB_CTRL_PTR       ( (reg32 *) VrefBuffer_cy_psoc4_abuf__CTB_CTB_CTRL)
#else
    #define VrefBuffer_CTB_CTRL_REG       (*(reg32 *) VrefBuffer_cy_psoc4_abuf__CTBM_CTB_CTRL)
    #define VrefBuffer_CTB_CTRL_PTR       ( (reg32 *) VrefBuffer_cy_psoc4_abuf__CTBM_CTB_CTRL)
#endif /* CYIPBLOCK_m0s8pass4b_VERSION */

#define VrefBuffer_OA_RES_CTRL_REG    (*(reg32 *) VrefBuffer_cy_psoc4_abuf__OA_RES_CTRL)
#define VrefBuffer_OA_RES_CTRL_PTR    ( (reg32 *) VrefBuffer_cy_psoc4_abuf__OA_RES_CTRL)
#define VrefBuffer_OA_COMP_TRIM_REG   (*(reg32 *) VrefBuffer_cy_psoc4_abuf__OA_COMP_TRIM)
#define VrefBuffer_OA_COMP_TRIM_PTR   ( (reg32 *) VrefBuffer_cy_psoc4_abuf__OA_COMP_TRIM)


/***************************************
*        Registers Constants
***************************************/

/* VrefBuffer_CTB_CTRL_REG */
#define VrefBuffer_CTB_CTRL_DEEPSLEEP_ON_SHIFT    (30u)   /* [30] Selects behavior CTB IP in the DeepSleep power mode */
#define VrefBuffer_CTB_CTRL_ENABLED_SHIFT         (31u)   /* [31] Enable of the CTB IP */


#define VrefBuffer_CTB_CTRL_DEEPSLEEP_ON          ((uint32) 0x01u << VrefBuffer_CTB_CTRL_DEEPSLEEP_ON_SHIFT)
#define VrefBuffer_CTB_CTRL_ENABLED               ((uint32) 0x01u << VrefBuffer_CTB_CTRL_ENABLED_SHIFT)


/* VrefBuffer_OA_RES_CTRL_REG */
#define VrefBuffer_OA_PWR_MODE_SHIFT          (0u)    /* [1:0]    Power level */
#define VrefBuffer_OA_DRIVE_STR_SEL_SHIFT     (2u)    /* [2]      Opamp output strenght select: 0 - 1x, 1 - 10x */
#define VrefBuffer_OA_COMP_EN_SHIFT           (4u)    /* [4]      CTB IP mode: 0 - Opamp, 1 - Comparator  */
#define VrefBuffer_OA_PUMP_EN_SHIFT           (11u)   /* [11]     Pump enable */


#define VrefBuffer_OA_PWR_MODE                ((uint32) 0x02u << VrefBuffer_OA_PWR_MODE_SHIFT)
#define VrefBuffer_OA_PWR_MODE_MASK           ((uint32) 0x03u << VrefBuffer_OA_PWR_MODE_SHIFT)
#define VrefBuffer_OA_DRIVE_STR_SEL_1X        ((uint32) 0x00u << VrefBuffer_OA_DRIVE_STR_SEL_SHIFT)
#define VrefBuffer_OA_DRIVE_STR_SEL_10X       ((uint32) 0x01u << VrefBuffer_OA_DRIVE_STR_SEL_SHIFT)
#define VrefBuffer_OA_DRIVE_STR_SEL_MASK      ((uint32) 0x01u << VrefBuffer_OA_DRIVE_STR_SEL_SHIFT)
#define VrefBuffer_OA_COMP_EN                 ((uint32) 0x00u << VrefBuffer_OA_COMP_EN_SHIFT)
#define VrefBuffer_OA_PUMP_EN                 ((uint32) 0x01u << VrefBuffer_OA_PUMP_EN_SHIFT)


/***************************************
*       Init Macros Definitions
***************************************/

#define VrefBuffer_GET_DEEPSLEEP_ON(deepSleep)    ((0u != (deepSleep)) ? (VrefBuffer_CTB_CTRL_DEEPSLEEP_ON) : (0u))
#define VrefBuffer_GET_OA_DRIVE_STR(current)      ((0u != (current)) ? (VrefBuffer_OA_DRIVE_STR_SEL_10X) : \
                                                                             (VrefBuffer_OA_DRIVE_STR_SEL_1X))
#define VrefBuffer_GET_OA_PWR_MODE(mode)          ((mode) & VrefBuffer_OA_PWR_MODE_MASK)
#define VrefBuffer_CHECK_PWR_MODE_OFF             (0u != (VrefBuffer_OA_RES_CTRL_REG & \
                                                                VrefBuffer_OA_PWR_MODE_MASK))

/* Returns true if component available in Deep Sleep power mode*/ 
#define VrefBuffer_CHECK_DEEPSLEEP_SUPPORT        (0u != VrefBuffer_DEEPSLEEP_SUPPORT) 

#define VrefBuffer_DEFAULT_CTB_CTRL (VrefBuffer_GET_DEEPSLEEP_ON(VrefBuffer_DEEPSLEEP_SUPPORT) | \
                                           VrefBuffer_CTB_CTRL_ENABLED)

#define VrefBuffer_DEFAULT_OA_RES_CTRL (VrefBuffer_OA_COMP_EN | \
                                              VrefBuffer_GET_OA_DRIVE_STR(VrefBuffer_OUTPUT_CURRENT))

#define VrefBuffer_DEFAULT_OA_COMP_TRIM_REG (VrefBuffer_OA_COMP_TRIM_VALUE)


/***************************************
* The following code is DEPRECATED and 
* should not be used in new projects.
***************************************/

#define VrefBuffer_LOWPOWER                   (VrefBuffer_LOW_POWER)
#define VrefBuffer_MEDPOWER                   (VrefBuffer_MED_POWER)
#define VrefBuffer_HIGHPOWER                  (VrefBuffer_HIGH_POWER)

/* PUMP ON/OFF defines */
#define VrefBuffer_PUMPON                     (VrefBuffer_PUMP_ON)
#define VrefBuffer_PUMPOFF                    (VrefBuffer_PUMP_OFF)

#define VrefBuffer_OA_CTRL                    (VrefBuffer_CTB_CTRL_REG)
#define VrefBuffer_OA_RES_CTRL                (VrefBuffer_OA_RES_CTRL_REG)

/* Bit Field  OA_CTRL */
#define VrefBuffer_OA_CTB_EN_SHIFT            (VrefBuffer_CTB_CTRL_ENABLED_SHIFT)
#define VrefBuffer_OA_PUMP_CTRL_SHIFT         (VrefBuffer_OA_PUMP_EN_SHIFT)
#define VrefBuffer_OA_PUMP_EN_MASK            (0x800u)
#define VrefBuffer_PUMP_PROTECT_MASK          (1u)


#endif    /* CY_OPAMP_VrefBuffer_H */


/* [] END OF FILE */
