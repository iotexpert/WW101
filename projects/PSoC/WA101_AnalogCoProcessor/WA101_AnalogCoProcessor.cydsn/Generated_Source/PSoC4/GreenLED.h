/*******************************************************************************
* File Name: GreenLED.h  
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

#if !defined(CY_PINS_GreenLED_H) /* Pins GreenLED_H */
#define CY_PINS_GreenLED_H

#include "cytypes.h"
#include "cyfitter.h"
#include "GreenLED_aliases.h"


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
} GreenLED_BACKUP_STRUCT;

/** @} structures */


/***************************************
*        Function Prototypes             
***************************************/
/**
* \addtogroup group_general
* @{
*/
uint8   GreenLED_Read(void);
void    GreenLED_Write(uint8 value);
uint8   GreenLED_ReadDataReg(void);
#if defined(GreenLED__PC) || (CY_PSOC4_4200L) 
    void    GreenLED_SetDriveMode(uint8 mode);
#endif
void    GreenLED_SetInterruptMode(uint16 position, uint16 mode);
uint8   GreenLED_ClearInterrupt(void);
/** @} general */

/**
* \addtogroup group_power
* @{
*/
void GreenLED_Sleep(void); 
void GreenLED_Wakeup(void);
/** @} power */


/***************************************
*           API Constants        
***************************************/
#if defined(GreenLED__PC) || (CY_PSOC4_4200L) 
    /* Drive Modes */
    #define GreenLED_DRIVE_MODE_BITS        (3)
    #define GreenLED_DRIVE_MODE_IND_MASK    (0xFFFFFFFFu >> (32 - GreenLED_DRIVE_MODE_BITS))

    /**
    * \addtogroup group_constants
    * @{
    */
        /** \addtogroup driveMode Drive mode constants
         * \brief Constants to be passed as "mode" parameter in the GreenLED_SetDriveMode() function.
         *  @{
         */
        #define GreenLED_DM_ALG_HIZ         (0x00u) /**< \brief High Impedance Analog   */
        #define GreenLED_DM_DIG_HIZ         (0x01u) /**< \brief High Impedance Digital  */
        #define GreenLED_DM_RES_UP          (0x02u) /**< \brief Resistive Pull Up       */
        #define GreenLED_DM_RES_DWN         (0x03u) /**< \brief Resistive Pull Down     */
        #define GreenLED_DM_OD_LO           (0x04u) /**< \brief Open Drain, Drives Low  */
        #define GreenLED_DM_OD_HI           (0x05u) /**< \brief Open Drain, Drives High */
        #define GreenLED_DM_STRONG          (0x06u) /**< \brief Strong Drive            */
        #define GreenLED_DM_RES_UPDWN       (0x07u) /**< \brief Resistive Pull Up/Down  */
        /** @} driveMode */
    /** @} group_constants */
#endif

/* Digital Port Constants */
#define GreenLED_MASK               GreenLED__MASK
#define GreenLED_SHIFT              GreenLED__SHIFT
#define GreenLED_WIDTH              1u

/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in GreenLED_SetInterruptMode() function.
     *  @{
     */
        #define GreenLED_INTR_NONE      ((uint16)(0x0000u)) /**< \brief Disabled             */
        #define GreenLED_INTR_RISING    ((uint16)(0x5555u)) /**< \brief Rising edge trigger  */
        #define GreenLED_INTR_FALLING   ((uint16)(0xaaaau)) /**< \brief Falling edge trigger */
        #define GreenLED_INTR_BOTH      ((uint16)(0xffffu)) /**< \brief Both edge trigger    */
    /** @} intrMode */
/** @} group_constants */

/* SIO LPM definition */
#if defined(GreenLED__SIO)
    #define GreenLED_SIO_LPM_MASK       (0x03u)
#endif

/* USBIO definitions */
#if !defined(GreenLED__PC) && (CY_PSOC4_4200L)
    #define GreenLED_USBIO_ENABLE               ((uint32)0x80000000u)
    #define GreenLED_USBIO_DISABLE              ((uint32)(~GreenLED_USBIO_ENABLE))
    #define GreenLED_USBIO_SUSPEND_SHIFT        CYFLD_USBDEVv2_USB_SUSPEND__OFFSET
    #define GreenLED_USBIO_SUSPEND_DEL_SHIFT    CYFLD_USBDEVv2_USB_SUSPEND_DEL__OFFSET
    #define GreenLED_USBIO_ENTER_SLEEP          ((uint32)((1u << GreenLED_USBIO_SUSPEND_SHIFT) \
                                                        | (1u << GreenLED_USBIO_SUSPEND_DEL_SHIFT)))
    #define GreenLED_USBIO_EXIT_SLEEP_PH1       ((uint32)~((uint32)(1u << GreenLED_USBIO_SUSPEND_SHIFT)))
    #define GreenLED_USBIO_EXIT_SLEEP_PH2       ((uint32)~((uint32)(1u << GreenLED_USBIO_SUSPEND_DEL_SHIFT)))
    #define GreenLED_USBIO_CR1_OFF              ((uint32)0xfffffffeu)
#endif


/***************************************
*             Registers        
***************************************/
/* Main Port Registers */
#if defined(GreenLED__PC)
    /* Port Configuration */
    #define GreenLED_PC                 (* (reg32 *) GreenLED__PC)
#endif
/* Pin State */
#define GreenLED_PS                     (* (reg32 *) GreenLED__PS)
/* Data Register */
#define GreenLED_DR                     (* (reg32 *) GreenLED__DR)
/* Input Buffer Disable Override */
#define GreenLED_INP_DIS                (* (reg32 *) GreenLED__PC2)

/* Interrupt configuration Registers */
#define GreenLED_INTCFG                 (* (reg32 *) GreenLED__INTCFG)
#define GreenLED_INTSTAT                (* (reg32 *) GreenLED__INTSTAT)

/* "Interrupt cause" register for Combined Port Interrupt (AllPortInt) in GSRef component */
#if defined (CYREG_GPIO_INTR_CAUSE)
    #define GreenLED_INTR_CAUSE         (* (reg32 *) CYREG_GPIO_INTR_CAUSE)
#endif

/* SIO register */
#if defined(GreenLED__SIO)
    #define GreenLED_SIO_REG            (* (reg32 *) GreenLED__SIO)
#endif /* (GreenLED__SIO_CFG) */

/* USBIO registers */
#if !defined(GreenLED__PC) && (CY_PSOC4_4200L)
    #define GreenLED_USB_POWER_REG       (* (reg32 *) CYREG_USBDEVv2_USB_POWER_CTRL)
    #define GreenLED_CR1_REG             (* (reg32 *) CYREG_USBDEVv2_CR1)
    #define GreenLED_USBIO_CTRL_REG      (* (reg32 *) CYREG_USBDEVv2_USB_USBIO_CTRL)
#endif    
    
    
/***************************************
* The following code is DEPRECATED and 
* must not be used in new designs.
***************************************/
/**
* \addtogroup group_deprecated
* @{
*/
#define GreenLED_DRIVE_MODE_SHIFT       (0x00u)
#define GreenLED_DRIVE_MODE_MASK        (0x07u << GreenLED_DRIVE_MODE_SHIFT)
/** @} deprecated */

#endif /* End Pins GreenLED_H */


/* [] END OF FILE */
