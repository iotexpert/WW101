/*******************************************************************************
* File Name: BlueLED.h  
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

#if !defined(CY_PINS_BlueLED_H) /* Pins BlueLED_H */
#define CY_PINS_BlueLED_H

#include "cytypes.h"
#include "cyfitter.h"
#include "BlueLED_aliases.h"


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
} BlueLED_BACKUP_STRUCT;

/** @} structures */


/***************************************
*        Function Prototypes             
***************************************/
/**
* \addtogroup group_general
* @{
*/
uint8   BlueLED_Read(void);
void    BlueLED_Write(uint8 value);
uint8   BlueLED_ReadDataReg(void);
#if defined(BlueLED__PC) || (CY_PSOC4_4200L) 
    void    BlueLED_SetDriveMode(uint8 mode);
#endif
void    BlueLED_SetInterruptMode(uint16 position, uint16 mode);
uint8   BlueLED_ClearInterrupt(void);
/** @} general */

/**
* \addtogroup group_power
* @{
*/
void BlueLED_Sleep(void); 
void BlueLED_Wakeup(void);
/** @} power */


/***************************************
*           API Constants        
***************************************/
#if defined(BlueLED__PC) || (CY_PSOC4_4200L) 
    /* Drive Modes */
    #define BlueLED_DRIVE_MODE_BITS        (3)
    #define BlueLED_DRIVE_MODE_IND_MASK    (0xFFFFFFFFu >> (32 - BlueLED_DRIVE_MODE_BITS))

    /**
    * \addtogroup group_constants
    * @{
    */
        /** \addtogroup driveMode Drive mode constants
         * \brief Constants to be passed as "mode" parameter in the BlueLED_SetDriveMode() function.
         *  @{
         */
        #define BlueLED_DM_ALG_HIZ         (0x00u) /**< \brief High Impedance Analog   */
        #define BlueLED_DM_DIG_HIZ         (0x01u) /**< \brief High Impedance Digital  */
        #define BlueLED_DM_RES_UP          (0x02u) /**< \brief Resistive Pull Up       */
        #define BlueLED_DM_RES_DWN         (0x03u) /**< \brief Resistive Pull Down     */
        #define BlueLED_DM_OD_LO           (0x04u) /**< \brief Open Drain, Drives Low  */
        #define BlueLED_DM_OD_HI           (0x05u) /**< \brief Open Drain, Drives High */
        #define BlueLED_DM_STRONG          (0x06u) /**< \brief Strong Drive            */
        #define BlueLED_DM_RES_UPDWN       (0x07u) /**< \brief Resistive Pull Up/Down  */
        /** @} driveMode */
    /** @} group_constants */
#endif

/* Digital Port Constants */
#define BlueLED_MASK               BlueLED__MASK
#define BlueLED_SHIFT              BlueLED__SHIFT
#define BlueLED_WIDTH              1u

/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in BlueLED_SetInterruptMode() function.
     *  @{
     */
        #define BlueLED_INTR_NONE      ((uint16)(0x0000u)) /**< \brief Disabled             */
        #define BlueLED_INTR_RISING    ((uint16)(0x5555u)) /**< \brief Rising edge trigger  */
        #define BlueLED_INTR_FALLING   ((uint16)(0xaaaau)) /**< \brief Falling edge trigger */
        #define BlueLED_INTR_BOTH      ((uint16)(0xffffu)) /**< \brief Both edge trigger    */
    /** @} intrMode */
/** @} group_constants */

/* SIO LPM definition */
#if defined(BlueLED__SIO)
    #define BlueLED_SIO_LPM_MASK       (0x03u)
#endif

/* USBIO definitions */
#if !defined(BlueLED__PC) && (CY_PSOC4_4200L)
    #define BlueLED_USBIO_ENABLE               ((uint32)0x80000000u)
    #define BlueLED_USBIO_DISABLE              ((uint32)(~BlueLED_USBIO_ENABLE))
    #define BlueLED_USBIO_SUSPEND_SHIFT        CYFLD_USBDEVv2_USB_SUSPEND__OFFSET
    #define BlueLED_USBIO_SUSPEND_DEL_SHIFT    CYFLD_USBDEVv2_USB_SUSPEND_DEL__OFFSET
    #define BlueLED_USBIO_ENTER_SLEEP          ((uint32)((1u << BlueLED_USBIO_SUSPEND_SHIFT) \
                                                        | (1u << BlueLED_USBIO_SUSPEND_DEL_SHIFT)))
    #define BlueLED_USBIO_EXIT_SLEEP_PH1       ((uint32)~((uint32)(1u << BlueLED_USBIO_SUSPEND_SHIFT)))
    #define BlueLED_USBIO_EXIT_SLEEP_PH2       ((uint32)~((uint32)(1u << BlueLED_USBIO_SUSPEND_DEL_SHIFT)))
    #define BlueLED_USBIO_CR1_OFF              ((uint32)0xfffffffeu)
#endif


/***************************************
*             Registers        
***************************************/
/* Main Port Registers */
#if defined(BlueLED__PC)
    /* Port Configuration */
    #define BlueLED_PC                 (* (reg32 *) BlueLED__PC)
#endif
/* Pin State */
#define BlueLED_PS                     (* (reg32 *) BlueLED__PS)
/* Data Register */
#define BlueLED_DR                     (* (reg32 *) BlueLED__DR)
/* Input Buffer Disable Override */
#define BlueLED_INP_DIS                (* (reg32 *) BlueLED__PC2)

/* Interrupt configuration Registers */
#define BlueLED_INTCFG                 (* (reg32 *) BlueLED__INTCFG)
#define BlueLED_INTSTAT                (* (reg32 *) BlueLED__INTSTAT)

/* "Interrupt cause" register for Combined Port Interrupt (AllPortInt) in GSRef component */
#if defined (CYREG_GPIO_INTR_CAUSE)
    #define BlueLED_INTR_CAUSE         (* (reg32 *) CYREG_GPIO_INTR_CAUSE)
#endif

/* SIO register */
#if defined(BlueLED__SIO)
    #define BlueLED_SIO_REG            (* (reg32 *) BlueLED__SIO)
#endif /* (BlueLED__SIO_CFG) */

/* USBIO registers */
#if !defined(BlueLED__PC) && (CY_PSOC4_4200L)
    #define BlueLED_USB_POWER_REG       (* (reg32 *) CYREG_USBDEVv2_USB_POWER_CTRL)
    #define BlueLED_CR1_REG             (* (reg32 *) CYREG_USBDEVv2_CR1)
    #define BlueLED_USBIO_CTRL_REG      (* (reg32 *) CYREG_USBDEVv2_USB_USBIO_CTRL)
#endif    
    
    
/***************************************
* The following code is DEPRECATED and 
* must not be used in new designs.
***************************************/
/**
* \addtogroup group_deprecated
* @{
*/
#define BlueLED_DRIVE_MODE_SHIFT       (0x00u)
#define BlueLED_DRIVE_MODE_MASK        (0x07u << BlueLED_DRIVE_MODE_SHIFT)
/** @} deprecated */

#endif /* End Pins BlueLED_H */


/* [] END OF FILE */
