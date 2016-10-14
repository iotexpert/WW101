/*******************************************************************************
* File Name: RedLED.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_RedLED_H) /* Pins RedLED_H */
#define CY_PINS_RedLED_H

#include "cytypes.h"
#include "cyfitter.h"
#include "RedLED_aliases.h"


/***************************************
*     Data Struct Definitions
***************************************/

/**
* \addtogroup group_structures
* @{
*/
    
/* Structure for sleep mode support */
typedef struct
{
    uint32 pcState; /**< State of the port control register */
    uint32 sioState; /**< State of the SIO configuration */
    uint32 usbState; /**< State of the USBIO regulator */
} RedLED_BACKUP_STRUCT;

/** @} structures */


/***************************************
*        Function Prototypes             
***************************************/
/**
* \addtogroup group_general
* @{
*/
uint8   RedLED_Read(void);
void    RedLED_Write(uint8 value);
uint8   RedLED_ReadDataReg(void);
#if defined(RedLED__PC) || (CY_PSOC4_4200L) 
    void    RedLED_SetDriveMode(uint8 mode);
#endif
void    RedLED_SetInterruptMode(uint16 position, uint16 mode);
uint8   RedLED_ClearInterrupt(void);
/** @} general */

/**
* \addtogroup group_power
* @{
*/
void RedLED_Sleep(void); 
void RedLED_Wakeup(void);
/** @} power */


/***************************************
*           API Constants        
***************************************/
#if defined(RedLED__PC) || (CY_PSOC4_4200L) 
    /* Drive Modes */
    #define RedLED_DRIVE_MODE_BITS        (3)
    #define RedLED_DRIVE_MODE_IND_MASK    (0xFFFFFFFFu >> (32 - RedLED_DRIVE_MODE_BITS))

    /**
    * \addtogroup group_constants
    * @{
    */
        /** \addtogroup driveMode Drive mode constants
         * \brief Constants to be passed as "mode" parameter in the RedLED_SetDriveMode() function.
         *  @{
         */
        #define RedLED_DM_ALG_HIZ         (0x00u) /**< \brief High Impedance Analog   */
        #define RedLED_DM_DIG_HIZ         (0x01u) /**< \brief High Impedance Digital  */
        #define RedLED_DM_RES_UP          (0x02u) /**< \brief Resistive Pull Up       */
        #define RedLED_DM_RES_DWN         (0x03u) /**< \brief Resistive Pull Down     */
        #define RedLED_DM_OD_LO           (0x04u) /**< \brief Open Drain, Drives Low  */
        #define RedLED_DM_OD_HI           (0x05u) /**< \brief Open Drain, Drives High */
        #define RedLED_DM_STRONG          (0x06u) /**< \brief Strong Drive            */
        #define RedLED_DM_RES_UPDWN       (0x07u) /**< \brief Resistive Pull Up/Down  */
        /** @} driveMode */
    /** @} group_constants */
#endif

/* Digital Port Constants */
#define RedLED_MASK               RedLED__MASK
#define RedLED_SHIFT              RedLED__SHIFT
#define RedLED_WIDTH              1u

/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in RedLED_SetInterruptMode() function.
     *  @{
     */
        #define RedLED_INTR_NONE      ((uint16)(0x0000u)) /**< \brief Disabled             */
        #define RedLED_INTR_RISING    ((uint16)(0x5555u)) /**< \brief Rising edge trigger  */
        #define RedLED_INTR_FALLING   ((uint16)(0xaaaau)) /**< \brief Falling edge trigger */
        #define RedLED_INTR_BOTH      ((uint16)(0xffffu)) /**< \brief Both edge trigger    */
    /** @} intrMode */
/** @} group_constants */

/* SIO LPM definition */
#if defined(RedLED__SIO)
    #define RedLED_SIO_LPM_MASK       (0x03u)
#endif

/* USBIO definitions */
#if !defined(RedLED__PC) && (CY_PSOC4_4200L)
    #define RedLED_USBIO_ENABLE               ((uint32)0x80000000u)
    #define RedLED_USBIO_DISABLE              ((uint32)(~RedLED_USBIO_ENABLE))
    #define RedLED_USBIO_SUSPEND_SHIFT        CYFLD_USBDEVv2_USB_SUSPEND__OFFSET
    #define RedLED_USBIO_SUSPEND_DEL_SHIFT    CYFLD_USBDEVv2_USB_SUSPEND_DEL__OFFSET
    #define RedLED_USBIO_ENTER_SLEEP          ((uint32)((1u << RedLED_USBIO_SUSPEND_SHIFT) \
                                                        | (1u << RedLED_USBIO_SUSPEND_DEL_SHIFT)))
    #define RedLED_USBIO_EXIT_SLEEP_PH1       ((uint32)~((uint32)(1u << RedLED_USBIO_SUSPEND_SHIFT)))
    #define RedLED_USBIO_EXIT_SLEEP_PH2       ((uint32)~((uint32)(1u << RedLED_USBIO_SUSPEND_DEL_SHIFT)))
    #define RedLED_USBIO_CR1_OFF              ((uint32)0xfffffffeu)
#endif


/***************************************
*             Registers        
***************************************/
/* Main Port Registers */
#if defined(RedLED__PC)
    /* Port Configuration */
    #define RedLED_PC                 (* (reg32 *) RedLED__PC)
#endif
/* Pin State */
#define RedLED_PS                     (* (reg32 *) RedLED__PS)
/* Data Register */
#define RedLED_DR                     (* (reg32 *) RedLED__DR)
/* Input Buffer Disable Override */
#define RedLED_INP_DIS                (* (reg32 *) RedLED__PC2)

/* Interrupt configuration Registers */
#define RedLED_INTCFG                 (* (reg32 *) RedLED__INTCFG)
#define RedLED_INTSTAT                (* (reg32 *) RedLED__INTSTAT)

/* "Interrupt cause" register for Combined Port Interrupt (AllPortInt) in GSRef component */
#if defined (CYREG_GPIO_INTR_CAUSE)
    #define RedLED_INTR_CAUSE         (* (reg32 *) CYREG_GPIO_INTR_CAUSE)
#endif

/* SIO register */
#if defined(RedLED__SIO)
    #define RedLED_SIO_REG            (* (reg32 *) RedLED__SIO)
#endif /* (RedLED__SIO_CFG) */

/* USBIO registers */
#if !defined(RedLED__PC) && (CY_PSOC4_4200L)
    #define RedLED_USB_POWER_REG       (* (reg32 *) CYREG_USBDEVv2_USB_POWER_CTRL)
    #define RedLED_CR1_REG             (* (reg32 *) CYREG_USBDEVv2_CR1)
    #define RedLED_USBIO_CTRL_REG      (* (reg32 *) CYREG_USBDEVv2_USB_USBIO_CTRL)
#endif    
    
    
/***************************************
* The following code is DEPRECATED and 
* must not be used in new designs.
***************************************/
/**
* \addtogroup group_deprecated
* @{
*/
#define RedLED_DRIVE_MODE_SHIFT       (0x00u)
#define RedLED_DRIVE_MODE_MASK        (0x07u << RedLED_DRIVE_MODE_SHIFT)
/** @} deprecated */

#endif /* End Pins RedLED_H */


/* [] END OF FILE */
