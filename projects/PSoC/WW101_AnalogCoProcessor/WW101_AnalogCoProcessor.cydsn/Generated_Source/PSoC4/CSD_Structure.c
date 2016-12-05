/***************************************************************************//**
* \file CSD_Structure.c
* \version 3.10
*
* \brief
*   This file defines the data structure global variables and provides implementation
*   for the high-level and low-level APIs of the Data Structure module.
*
* \see CapSense P4 v3.10 Datasheet
*
*//*****************************************************************************
* Copyright (2016), Cypress Semiconductor Corporation.
********************************************************************************
* This software is owned by Cypress Semiconductor Corporation (Cypress) and is
* protected by and subject to worldwide patent protection (United States and
* foreign), United States copyright laws and international treaty provisions.
* Cypress hereby grants to licensee a personal, non-exclusive, non-transferable
* license to copy, use, modify, create derivative works of, and compile the
* Cypress Source Code and derivative works for the sole purpose of creating
* custom software in support of licensee product to be used only in conjunction
* with a Cypress integrated circuit as specified in the applicable agreement.
* Any reproduction, modification, translation, compilation, or representation of
* this software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH
* REGARD TO THIS MATERIAL, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* Cypress reserves the right to make changes without further notice to the
* materials described herein. Cypress does not assume any liability arising out
* of the application or use of any product or circuit described herein. Cypress
* does not authorize its products for use as critical components in life-support
* systems where a malfunction or failure may reasonably be expected to result in
* significant injury to the user. The inclusion of Cypress' product in a life-
* support systems application implies that the manufacturer assumes all risk of
* such use and in doing so indemnifies Cypress against all charges. Use may be
* limited by and subject to the applicable Cypress software license agreement.
*******************************************************************************/

#include <cytypes.h>
#include "CyLib.h"
#include "CSD_Structure.h"
#include "CSD_Configuration.h"

#if (0u != CSD_ADC_EN)
    #include "CSD_Adc.h"
#endif /* (0u != CSD_ADC_EN) */

#if (CSD_ENABLE == CSD_SELF_TEST_EN)
    #include "CSD_SelfTest.h"
#endif

/*******************************************************************************
* Define the RAM Data Structure variables and their init data in flash
*******************************************************************************/
/**
* \if SECTION_GLOBAL_VARIABLES
* \addtogroup group_global_variables
* \{
*/

/**
* Variable that contains CSD configuration, settings and scanning results.
* CSD_dsRam represents RAM Data Structure.
*/
CSD_RAM_STRUCT CSD_dsRam;
/** \}
* \endif */

/*******************************************************************************
* Declare Widget's De-bounce Counters
*******************************************************************************/
static uint8 CSD_debounceButton0[CSD_BUTTON0_NUM_SENSORS];


/***************************************************************************//**
* Declare Filter History Objects
*******************************************************************************/
static CSD_REGULAR_FLTR_STRUCT CSD_fltrHistoryButton0[CSD_BUTTON0_NUM_SENSORS];

/*******************************************************************************
* Define and initialize the Flash Data Structure
*******************************************************************************/

/**
* \if SECTION_API_CONSTANTS
* \addtogroup group_api_constants
* \{
*/

/**
* Constant for the FLASH Data Structure
*/
const CSD_FLASH_STRUCT CSD_dsFlash =
{
    /* Flash Widget Initialization Data */
    {
        { /* Button0 */
            &CSD_ioList[0u],
            (void *)&CSD_dsRam.wdgtList.button0,
            CSD_dsRam.snsList.button0,
            (void *)CSD_fltrHistoryButton0,
            CSD_debounceButton0,
            CSD_BUTTON0_STATIC_CONFIG,
            CSD_BUTTON0_NUM_SENSORS,
            (uint8)CSD_WD_BUTTON_E,
            CSD_BUTTON0_NUM_SENSORS,
        },
    },
};


/**
* Array of pointers to the electrode specific register
*/
const CSD_FLASH_IO_STRUCT CSD_ioList[CSD_TOTAL_ELECTRODES] =
{
    { /* 0: Button0_Sns0 */
        (reg32 *)CSD_Sns__0__HSIOM,
        (reg32 *)CSD_Sns__0__PC,
        (reg32 *)CSD_Sns__0__DR,
        (reg32 *)CSD_Sns__0__PS,
        CSD_Sns__0__HSIOM_MASK,
        CSD_Sns__0__MASK,
        CSD_Sns__0__HSIOM_SHIFT,
        (uint8)CSD_Sns__0__SHIFT,
        (uint8)CSD_Sns__0__SHIFT * 3u,
    },
    { /* 1: Button0_Sns1 */
        (reg32 *)CSD_Sns__1__HSIOM,
        (reg32 *)CSD_Sns__1__PC,
        (reg32 *)CSD_Sns__1__DR,
        (reg32 *)CSD_Sns__1__PS,
        CSD_Sns__1__HSIOM_MASK,
        CSD_Sns__1__MASK,
        CSD_Sns__1__HSIOM_SHIFT,
        (uint8)CSD_Sns__1__SHIFT,
        (uint8)CSD_Sns__1__SHIFT * 3u,
    },
};



/** \}
* \endif */

/* Initialization data for RAM widget list */
static const CSD_RAM_WD_LIST_STRUCT CSD_ramWidgetInit =
{
    { /* Button0 */
        CSD_BUTTON0_RESOLUTION,
        CSD_BUTTON0_FINGER_TH,
        CSD_BUTTON0_NOISE_TH,
        CSD_BUTTON0_NNOISE_TH,
        CSD_BUTTON0_HYSTERESIS,
        CSD_BUTTON0_ON_DEBOUNCE,
        CSD_BUTTON0_LOW_BSLN_RST,
        CSD_BUTTON0_BSLN_COEFF,
        {
            CSD_BUTTON0_IDAC_MOD0,
        },
        CSD_BUTTON0_SNS_CLK,
        CSD_BUTTON0_SNS_CLK_SOURCE,
    },
};


/* IDAC Initialization Data */
static const uint8 CSD_ramIdacInit[CSD_TOTAL_SENSORS] =
{
    /* Button0 */
    CSD_BUTTON0_SNS0_IDAC_COMP0,
    CSD_BUTTON0_SNS1_IDAC_COMP0,
};


/*******************************************************************************
* Define internal data types and statements
*******************************************************************************/
#define PARAM_TYPE_OFFSET       (30u)
#define PARAM_TYPE_MASK         (3Lu << PARAM_TYPE_OFFSET)
#define PARAM_RWBIT_OFFSET      (29u)
#define PARAM_RWBIT_MASK        (1Lu << PARAM_RWBIT_OFFSET)
#define PARAM_FBIT_OFFSET       (28u)
#define PARAM_FBIT_MASK         (1Lu << PARAM_FBIT_OFFSET)
#define PARAM_UBIT_OFFSET       (23u)
#define PARAM_UBIT_MASK         (1Lu << PARAM_UBIT_OFFSET)
#define PARAM_WDID_OFFSET       (16u)
#define PARAM_WDID_MASK         (0x7FLu << PARAM_WDID_OFFSET)
#define PARAM_CRC_OFFSET        (24u)
#define PARAM_CRC_MASK          (0x0FLu << PARAM_CRC_OFFSET)

/* Define align masks for uint16 and uint32 */
#define UINT16_ALIGN_MASK       (1Lu)
#define UINT32_ALIGN_MASK       (3Lu)

/* Define supported parameter types */
#define DS_PARAM_TYPE_UINT8     (0x01u)
#define DS_PARAM_TYPE_UINT16    (0x02u)
#define DS_PARAM_TYPE_UINT32    (0x03u)


/* Define PARAM_ID structure */
typedef struct
{
    uint16 offset;      /* parameter offset                     */
    uint8  widgetId;    /* widget Id parameter belongs to       */
    uint8  affectsCrc;  /* parameter affects widget CRC         */
    uint8  crc;         /* parameter CRC                        */
    uint8  flash;       /* parameter located in FLASH flag      */
    uint8  rw;          /* parameter is read/write flag         */
    uint8  type;        /* parameter type: uint8/uint16/uint32  */
} PARAM_ID_STRUCT;


/*******************************************************************************
* Static Function Prototypes
*******************************************************************************/
/**
* \if SECTION_CAPSENSE_INTERNAL
* \addtogroup group_capsense_internal
* \{
*/

static cystatus DsParseParamId(uint32 paramId, PARAM_ID_STRUCT *pData);

/** \}
* \endif */


/*******************************************************************************
* Function Name: CSD_DsInitialize
****************************************************************************//**
*
* \brief
*   This function initializes the Data Structure storage.
*
* \details
*   This function copies the default widget configuration parameters 
*   from the Flash storage into the RAM Data Structure.
*
*******************************************************************************/
void CSD_DsInitialize(void)
{
    #if (0u != CSD_CSX_EN) || (0 != CSD_CSD_IDAC_COMP_EN)
        uint32 snsId;
        uint32 wdgtId;
    
        CSD_RAM_SNS_STRUCT * ptrSns;
        CSD_FLASH_WD_STRUCT const * ptrFlashWdgt;
        uint8 const * ptrIdacInit = CSD_ramIdacInit;
    #endif        
    
    /* Reset RAM data structure content */
    (void)memset(&CSD_dsRam, 0, sizeof(CSD_dsRam));
    
    /* Initialize configId and deviceId registers */
    CSD_dsRam.configId = CSD_CONFIG_ID;
    CSD_dsRam.deviceId = CSD_DEVICE_ID;
    
    /* Initialize global RAM data */
    CSD_dsRam.csd0Config = CSD_CSD0_CONFIG;

    #if ((0u != CSD_CSD2X_EN) || (0u != CSD_CSX2X_EN))
        CSD_dsRam.csd1Config = CSD_CSD1_CONFIG;
    #endif /* #if ((0u != CSD_CSD2X_EN) || (0u != CSD_CSX2X_EN)) */
    
    #if (0u != CSD_TOTAL_CSD_WIDGETS)
        CSD_dsRam.modCsdClk = CSD_CSD_SCANSPEED_DIVIDER;
        
        #if (0u != CSD_CSD_COMMON_SNS_CLK_EN)
            CSD_dsRam.snsCsdClk = CSD_CSD_SNS_CLK_DIVIDER;
        #endif /* #if (0u != CSD_CSD_COMMON_SNS_CLK_EN) */
    #endif /* #if (0u != CSD_TOTAL_CSD_WIDGETS) */
    
    #if (0u != CSD_CSX_EN)
        CSD_dsRam.modCsxClk = CSD_CSX_SCANSPEED_DIVIDER;
        
        #if (0u != CSD_CSX_COMMON_TX_CLK_EN)
            CSD_dsRam.snsCsxClk = CSD_CSX_TX_CLK_DIVIDER;
        #endif /* #if (0u != CSD_CSX_COMMON_TX_CLK_EN) */
    #endif /* #if (0u != CSD_CSX_EN) */
    
    #if (0u != CSD_SELF_TEST_EN)
        CSD_dsRam.glbCrc = CSD_GLB_CRC;
    #endif /* #if (0u != CSD_SELF_TEST_EN) */
   
    /* Initialize RAM widget data */
    CSD_dsRam.wdgtList = CSD_ramWidgetInit;
    
    #if (0u != CSD_CSX_EN) || (0 != CSD_CSD_IDAC_COMP_EN)
        /* Initialize IDAC data */
        ptrFlashWdgt = CSD_dsFlash.wdgtArray;
        
        for (wdgtId = CSD_TOTAL_WIDGETS; wdgtId-- > 0u; )
        {
            ptrSns = ptrFlashWdgt->ptr2SnsRam;
                
            for (snsId = CSD_GET_SNS_CNT_BY_PTR(ptrFlashWdgt); snsId-- > 0u;)
            {
                ptrSns->idacComp[0u] = *ptrIdacInit;
                ptrIdacInit++;
                
                #if (0u != CSD_MULTI_FREQ_SCAN_EN)
                    ptrSns->idacComp[1u] = *ptrIdacInit;
                    ptrIdacInit++;
                    
                    ptrSns->idacComp[2u] = *ptrIdacInit;
                    ptrIdacInit++;
                #endif /* #if (0u != CSD_MULTI_FREQ_SCAN_EN) */
                
                ptrSns++;
            }
            
            ptrFlashWdgt++; /* Move to next widget */
        }
    #endif /*(0u != CSD_CSX_EN) || (0 != CSD_CSD_IDAC_COMP_EN)*/
    
    #if (0u != CSD_ADC_EN)
        CSD_AdcDsInitialize();
    #endif /* (0u != CSD_ADC_EN) */
}


#if (0u != CSD_ADC_EN)
    /*******************************************************************************
    * Function Name: CSD_AdcDsInitialize
    ****************************************************************************//**
    *
    * \brief
    *   Configures the initial Adc datastructure members.
    *
    *******************************************************************************/
    void CSD_AdcDsInitialize(void)
    {
        CSD_dsRam.adcResolution = CSD_ADC_RESOLUTION;
        CSD_dsRam.adcIdac = (uint8)(CSD_ADC_IDACVAL);
    }
#endif /* (0u != CSD_ADC_EN) */


/*******************************************************************************
* Function Name: DsParseParamId
****************************************************************************//**
*
* \brief
*   This function parses the parameter ID and checks its correctness.
*
* \details
*   This function checks the parameter CRC if the self-test is enabled, 
*   validates the offset value and parameter type.
*
* \param paramId The parameter to parse.
* \param pData   The pointer to the structure that will hold parsed data.
*
* \return CYRET_SUCCESS     If the parameter is valid
*         CYRET_BAD_PARAM   If  the parameter is invalid
*
*******************************************************************************/
static cystatus DsParseParamId(uint32 paramId, PARAM_ID_STRUCT *pData)
{
    cystatus result = CYRET_SUCCESS;
    
    #if (0u != CSD_SELF_TEST_EN)
        const uint8 crcTable[] =
        {
            0x00u, 0x09u, 0x0Bu, 0x02u, 0x0Fu, 0x06u, 0x04u, 0x0Du,
            0x07u, 0x0Eu, 0x0Cu, 0x05u, 0x08u, 0x01u, 0x03u, 0x0Au
        };
        
        uint32 i;
        uint32 actualCrc = 0u;
        uint32 crcIndex;
    #endif /* #if (0u != CSD_SELF_TEST_EN) */
            
    /* Extract parameter data */
    pData->offset     = LO16(paramId);
    pData->affectsCrc = LO8((paramId & PARAM_UBIT_MASK)  >> PARAM_UBIT_OFFSET);
    pData->widgetId   = LO8((paramId & PARAM_WDID_MASK)  >> PARAM_WDID_OFFSET);
    pData->type       = LO8((paramId & PARAM_TYPE_MASK)  >> PARAM_TYPE_OFFSET);
    pData->flash      = LO8((paramId & PARAM_FBIT_MASK)  >> PARAM_FBIT_OFFSET);
    pData->rw         = LO8((paramId & PARAM_RWBIT_MASK) >> PARAM_RWBIT_OFFSET);
    pData->crc        = LO8((paramId & PARAM_CRC_MASK)   >> PARAM_CRC_OFFSET);
    
    /* Check parameter CRC if self-test is enabled */
    #if (0u != CSD_SELF_TEST_EN)
 
        /* Calculate CRC for bits 0..24.
         * The CRC is calculated using nibbles (4-bits). 
         * So for 0..24 bits there are in total 6 nibbles. */
        for (i = 6u; i > 0u; i--)
        {
            crcIndex = actualCrc ^ (paramId & 0x0Fu);
            actualCrc = crcTable[crcIndex];
            paramId >>= 4u;
        }

        /* Add bits 28..32 */
        crcIndex = actualCrc ^ ((paramId >> 4u) & 0x0Fu);
        actualCrc = crcTable[crcIndex];

        /* Add bits 24..28 (CRC) */
        crcIndex = actualCrc ^ (paramId & 0x0Fu);
        actualCrc = crcTable[crcIndex];
        
        if (0u != actualCrc)
        {
            /* CRC mismatch */
            result = CYRET_BAD_PARAM;
        }
        
        if ((CYRET_SUCCESS == result) && 
            (0u != pData->affectsCrc) && (pData->widgetId >= CSD_TOTAL_WIDGETS ))
        {
            /* Wrong widgetId for protected parameter */
            result = CYRET_BAD_PARAM;
        }
        
        /* Check offset value range */
        if (CYRET_SUCCESS == result)
        {
            if (0u == pData->flash)
            {
                /* Check offset for RAM Data Structure range */
                if (pData->offset >= sizeof(CSD_dsRam))
                {
                    result = CYRET_BAD_PARAM;
                }
            }
            else
            {
                /* Check offset for Flash Data Structure range */
                if (pData->offset >= sizeof(CSD_dsFlash))
                {
                    result = CYRET_BAD_PARAM;
                }
                
                /* Check if it is marked as Read Only */
                if (0u != pData->rw)
                {
                    result = CYRET_BAD_PARAM;
                }
            }
        }
    #else /* #if (0u != CSD_SELF_TEST_EN) */
        /* Check offset value range */
        if (0u == pData->flash)
        {
            /* Check offset for RAM Data Structure range */
            if (pData->offset >= sizeof(CSD_dsRam))
            {
                result = CYRET_BAD_PARAM;
            }
        }
        else
        {
            /* Check offset for Flash Data Structure range */
            if (pData->offset >= sizeof(CSD_dsFlash))
            {
                result = CYRET_BAD_PARAM;
            }
            
            /* Check if it is marked as Read Only */
            if (0u != pData->rw)
            {
                result = CYRET_BAD_PARAM;
            }
        }
    #endif /* #if (0u != CSD_SELF_TEST_EN) */

    if (CYRET_SUCCESS == result)
    {
        /* Check parameter type, offset alignment */
        switch (pData->type)
        {
        case DS_PARAM_TYPE_UINT16:
            if (0u != (pData->offset & UINT16_ALIGN_MASK))
            {
                result = CYRET_BAD_PARAM;
            }
            break;
            
        case DS_PARAM_TYPE_UINT32:
            if (0u != (pData->offset & UINT32_ALIGN_MASK))
            {
                result = CYRET_BAD_PARAM;
            }
            break;
            
        case DS_PARAM_TYPE_UINT8:
            break;
            
        default:
            result = CYRET_BAD_PARAM;
            break;
        }
    }

    return result;
}

/*******************************************************************************
* Function Name: CSD_GetParam
****************************************************************************//**
*
* \brief
*  Gets the specified parameter value from the \ref group_structures.
*
* \details
*  This function gets the value of the specified parameter by the paramId 
*  argument. The paramId for each register is available in the 
*  CSD RegisterMap header file as 
*  CSD_<ParameterName>_PARAM_ID. The paramId is a special 
*  enumerated value generated by the customizer. The format of paramId is as 
*  follows:
*    1. [ byte 3 byte 2 byte 1 byte 0 ] 
*    2. [ TTWFCCCC UIIIIIII MMMMMMMM LLLLLLLL ]
*    3. T - encodes the parameter type:
*      - 01b: uint8
*      - 10b: uint16
*      - 11b: uint32
*    4. W - indicates whether the parameter is writable:
*      - 0: ReadOnly
*      - 1: Read/Write
*    5. C - 4 bit CRC (X^3 + 1) of the whole paramId word, the C bits are 
*       filled with 0s when the CRC is calculated.
*    6. U - indicates if the parameter affects the RAM Widget Object CRC.
*    7. I - specifies that the widgetID parameter belongs to
*    8. M,L - the parameter offset MSB and LSB accordingly in:
*      - Flash Data Structure if W bit is 0.
*      - RAM Data Structure if W bit is 1.
*
*  Refer to the \ref group_structures section for details of the data structure 
*  organization and examples of its register access.
*
* \param  paramId 
*  Specify the ID of parameter to get its value.
*  A macro for the parameter ID can be found in the CSD RegisterMap header 
*  file defined as CSD_<ParameterName>_PARAM_ID.
*
* \param  value   
*  The pointer to a variable that should be updated with the got value.
*
* \return
*  Returns the status of operation:
*    - CYRET_SUCCESS if the operation is successfully completed.
*    - CYRET_BAD_PARAM if the input parameter is invalid. 
*
*******************************************************************************/
cystatus CSD_GetParam(uint32 paramId, uint32 *value)
{
    PARAM_ID_STRUCT pData;
    cystatus result;
    
    union
    {
        volatile void   const * ptr;
        volatile uint8  const * ptrUint8;
        volatile uint16 const * ptrUint16;
        volatile uint32 const * ptrUint32;
    } ptrData;
    
    result = DsParseParamId(paramId, &pData);
    
    /* Parse and validate paramId */
    if (CYRET_SUCCESS == result)
    {
        /* Get base address of parameter */
        if (0u == pData.flash)
        {
            ptrData.ptr = &CSD_dsRam;
        }
        else
        {
            ptrData.ptr = &CSD_dsFlash;
        }
        
        /* Add offset to base address */
        ptrData.ptrUint8 += pData.offset;
        
        /* Read data */
        if (DS_PARAM_TYPE_UINT8 == pData.type)
        {
            *value = (uint32) (*ptrData.ptrUint8);
        }
        else if (DS_PARAM_TYPE_UINT16 == pData.type)
        {
            *value = (uint32) (*ptrData.ptrUint16);
        }
        else if (DS_PARAM_TYPE_UINT32 == pData.type)
        {
            *value = *ptrData.ptrUint32;
        }
        else
        {
            /* Parameter comes here already validated. */
            CYASSERT(0u);
        }
    }
    
    return result;
}


/*******************************************************************************
* Function Name: CSD_SetParam
****************************************************************************//**
*
* \brief
*  Sets a new value for the specified parameter in the \ref group_structures.
*
* \details
*  This function sets the value of the specified parameter by the paramId 
*  argument. The paramId for each register is available in the 
*  CSD RegisterMap header file as 
*  CSD_<ParameterName>_PARAM_ID. The paramId is a special 
*  enumerated value generated by the customizer. The format of paramId is as 
*  follows:
*    1. [ byte 3 byte 2 byte 1 byte 0 ] 
*    2. [ TTWFCCCC UIIIIIII MMMMMMMM LLLLLLLL ]
*    3. T - encodes the parameter type:
*      - 01b: uint8
*      - 10b: uint16
*      - 11b: uint32
*    4. W - indicates whether the parameter is writable:
*      - 0: ReadOnly
*      - 1: Read/Write
*    5. C - 4 bit CRC (X^3 + 1) of the whole paramId word, the C bits are 
*       filled with 0s when the CRC is calculated.
*    6. U - indicates if the parameter affects the RAM Widget Object CRC.
*    7. I - specifies that the widgetID parameter belongs to
*    8. M,L - the parameter offset MSB and LSB accordingly in:
*      - Flash Data Structure if W bit is 0.
*      - RAM Data Structure if W bit is 1.
*
*  Refer to the \ref group_structures section for details of the data structure 
*  organization and examples of its register access.
*
* \param paramId  
*  Specify the ID of parameter to set its value.
*  A macro for the parameter ID can be found in the CSD RegisterMap header
*  file defined as CSD_<ParameterName>_PARAM_ID.

* \param value    
*  Specify the new parameter's value.
*
* \return
*  Returns the status of operation:
*    - CYRET_SUCCESS if the operation is successfully completed.
*    - CYRET_BAD_PARAM if the input parameter is invalid. 
*
*******************************************************************************/
cystatus CSD_SetParam(uint32 paramId, uint32 value)
{
    cystatus result;
    PARAM_ID_STRUCT pData;
    
    union
    {
        volatile void   * ptr;
        volatile uint8  * ptrUint8;
        volatile uint16 * ptrUint16;
        volatile uint32 * ptrUint32;
    } ptrData;
    
    result = DsParseParamId(paramId, &pData);
    
    /* verify if parameter is writable */
    if (CYRET_SUCCESS == result)
    {
        if (0u == pData.rw)
        {
            result = CYRET_BAD_PARAM;
        }
    }
    
    /* Parse and validate paramId */
    if (CYRET_SUCCESS == result)
    {
        /* Get base address of parameter */
        ptrData.ptr = &CSD_dsRam;
        
        /* Add offset to base address */
        ptrData.ptrUint8 += pData.offset;
        
        /* Write data */
        if (DS_PARAM_TYPE_UINT8 == pData.type)
        {
            *ptrData.ptrUint8 = LO8(value);
        }
        else if (DS_PARAM_TYPE_UINT16 == pData.type)
        {
            *ptrData.ptrUint16 = LO16(value);
        }
        else if (DS_PARAM_TYPE_UINT32 == pData.type)
        {
            *ptrData.ptrUint32 = value;
        }
        else
        {
            /* Parameter comes here already validated. */
            CYASSERT(0u);
        }
        
        #if (CSD_ENABLE == (CSD_TST_WDGT_CRC_EN || CSD_TST_GLOBAL_CRC_EN))
            /* Update widget or global CRC if self-test is enabled and parameter affects it */
            if (0u != pData.affectsCrc)
            {
                if ((ptrData.ptrUint16 >= &CSD_dsRam.csd0Config) &&
                    (ptrData.ptrUint16 <  &CSD_dsRam.glbCrc))
                {
                    
                    #if (CSD_ENABLE == CSD_TST_GLOBAL_CRC_EN)
                        CSD_DsUpdateGlobalCrc();
                    #endif /* (CSD_ENABLE == CSD_TST_GLOBAL_CRC_EN) */

                }
                else
                {
                    #if (CSD_ENABLE == CSD_TST_WDGT_CRC_EN)
                        CSD_DsUpdateWidgetCrc((uint32)pData.widgetId);
                    #endif /* (CSD_ENABLE == CSD_TST_WDGT_CRC_EN) */
                }
            }
        #endif /* (CSD_ENABLE == (CSD_TST_WDGT_CRC_EN || CSD_TST_GLOBAL_CRC_EN)) */
    }
    
    return result;
}

/*******************************************************************************
* Function Name: CSD_IsAnyWidgetActive
****************************************************************************//**
*
* \brief
*  Reports if any widget has detected a touch.
*
* \details
*  This function reports if any widget has detected a touch or not by extracting 
*  the information from the wdgtStatus registers 
*  (CSD_WDGT_STATUS<X>_VALUE). This function does not process any 
*  widget but extracts the processed results from the \ref group_structures.
*
* \return 
*   Returns the touch detection status of all widgets:
*     - Zero if no touch is detected in all widgets or sensors.
*     - Non-zero if at least one widget or sensor detected a touch.
*
*******************************************************************************/
uint32 CSD_IsAnyWidgetActive(void)
{
    uint32 result = 0Lu;
    uint32 wdWord;
    
    for (wdWord = CSD_WDGT_STATUS_WORDS; wdWord-- > 0u;)
    {
        result |= CSD_dsRam.wdgtStatus[wdWord];
    }
    
    return result;
}


/*******************************************************************************
* Function Name: CSD_IsWidgetActive
****************************************************************************//**
*
* \brief
*  Reports if the specified widget detects a touch on any of its sensors.
*
* \details
*  This function reports if the specified widget has detected a touch or not by 
*  extracting the information from the wdgtStatus registers 
*  (CSD_WDGT_STATUS<X>_VALUE). This function does not process the 
*  widget, but extracts the processed results from the \ref group_structures.
*
* \param widgetId  
*  Specify the ID number of the widget to get its status.
*  A macro for the widget ID can be found in the CSD Configuration header 
*  file defined as CSD_<WidgetName>_WDGT_ID.
*
* \return 
*  Returns the touch detection status of the specified widgets:
*    - Zero if no touch is detected in the specified widget or a wrong widgetId
*      is specified
*    - Non-zero if at least one sensor of the specified widget is active i.e. 
*      touch is detected
*
*******************************************************************************/
uint32 CSD_IsWidgetActive(uint32 widgetId)
{
    uint32 result = 0Lu;
   
    if (widgetId < CSD_TOTAL_WIDGETS)
    {
        result = CSD_GET_WIDGET_ACTIVE_STATUS(widgetId);
    }
    return result;
}


/*******************************************************************************
* Function Name: CSD_IsSensorActive
****************************************************************************//**
*
* \brief
*  Reports if the specified sensor in the widget detects a touch.
*
* \details 
*  This function reports if the specified sensor in the widget has detected a 
*  touch or not by extracting the information from wdgtStatus registers 
*  (CSD_WDGT_STATUS<X>_VALUE). This function does not process the
*  widget or sensor, but extracts the processed results from the \ref group_structures.
*   
*  For Proximity sensors this function returns proximity detection status. To 
*  get finger touch status of proximity sensors, use the 
*  CSD_IsProximitySensorActive() function. 
*
* \param widgetId  
*  Specify the ID number of the widget.
*  A macro for the widget ID can be found in the CSD Configuration header 
*  file defined as CSD_<WidgetName>_WDGT_ID 

* \param sensorId  
*  Specify the ID number of the sensor within the widget to get its touch 
*  detection status.
*  A macro for the sensor ID within specified widget can be found in the 
*  CSD Configuration header file defined as 
*  CSD_<WidgetName>_SNS<SensorNumber>_ID 
*
* \return 
*  Returns the touch detection status of the specified sensor / widget:
*    - Zero if no touch is detected in the specified sensor / widget or a wrong
*      widget ID / sensor ID is specified
*    - Non-zero if the specified sensor is active i.e. touch is detected. If the
*      specific sensor belongs to a proximity widget, the proximity detection 
*      status is returned.
*
*******************************************************************************/
uint32 CSD_IsSensorActive(uint32 widgetId, uint32 sensorId)
{
    uint32 result = 0Lu;
    
    if ((widgetId < CSD_TOTAL_WIDGETS) &&
        (sensorId < CSD_GET_SENSOR_COUNT(widgetId)))
    {
        #if (CSD_ENABLE == CSD_PROXIMITY_WIDGET_EN)
            if (CSD_WD_PROXIMITY_E == 
				(CSD_WD_TYPE_ENUM)CSD_dsFlash.wdgtArray[widgetId].wdgtType)
            {
                sensorId = CSD_PROX_STS_OFFSET(sensorId);
            }
        #endif /* (CSD_ENABLE != CSD_PROXIMITY_WIDGET_EN) */
        result = CSD_dsRam.snsStatus[widgetId] & (1Lu << sensorId);
    }
    
    return result;
}

#if (0u != CSD_MATRIX_WIDGET_EN)
/*******************************************************************************
* Function Name: CSD_IsMatrixButtonsActive
****************************************************************************//**
*
* \brief
*  Reports the status of the specified matrix button widget.
*
* \details 
*  This function reports if the specified matrix widget has detected a touch or
*  not by extracting the information from the wdgtStatus registers 
*  (CSD_WDGT_STATUS<X>_VALUE for the CSD widgets and 
*  CSD_SNS_STATUS<WidgetId>_VALUE for CSX widget). In addition, the
*  function provides details of the active sensor including active rows/columns 
*  for the CSD widgets. This function should be used only with the matrix button
*  widgets. This function does not process the widget, but extracts the 
*  processed results from the \ref group_structures.
*
* \param widgetId 
*  Specify the ID number of the matrix button widget to check status of its 
*  sensors. 
*  A macro for widget ID can be found in the CSD Configuration header file 
*  defined as CSD_<WidgetName>_WDGT_ID
*
* \return 
*  Returns the touch detection status of sensors in the specified matrix 
*  buttons widget. Zero indicates that no touch is detected in the specified 
*  widget or a wrong widgetId is specified.
*    1. For the matrix buttons widgets with the CSD sensing mode:
*      - Bit [31] if set, indicates that one or more sensors in the widget 
*        detected a touch.
*      - Bits [30..24] is reserved
*      - Bits [23..16] indicates the logical sensor number of the sensor that 
*        detected a touch. If more than one sensor detected a touch for the CSD
*        widget, no status is reported as more than one touch is invalid for the
*        CSD matrix buttons widgets.
*      - Bits [15..8] indicates the active row number.
*      - Bits [7..0] indicates the active column number.
*    2. For the matrix buttons widgets with the CSX widgets, each bit (31..0)
*      corresponds to the TX/RX intersection.
*
*******************************************************************************/
uint32 CSD_IsMatrixButtonsActive(uint32 widgetId)
{
    uint32 result = 0Lu;
    cystatus state = CYRET_SUCCESS;
    CSD_FLASH_WD_STRUCT const *ptrFlashWdgt = 0u;
    
    #if (0u != CSD_CSD_MATRIX_WIDGET_EN)
        CSD_RAM_WD_CSD_MATRIX_STRUCT *wdCsdMatrix;
    #endif
  
    if (widgetId >= CSD_TOTAL_WIDGETS)
    {
        state = CYRET_BAD_PARAM;
    }
    else
    {
        ptrFlashWdgt = &CSD_dsFlash.wdgtArray[widgetId];
        
        if ((CSD_WD_TYPE_ENUM)ptrFlashWdgt->wdgtType != CSD_WD_MATRIX_BUTTON_E)
        {
            state = CYRET_BAD_PARAM;
        }
        else if (0u == CSD_GET_WIDGET_ACTIVE_STATUS(widgetId))
        {
            state = CYRET_BAD_PARAM;
        }
        else
        {
            /* input parameters are OK */
        }
    }
       
    if (CYRET_SUCCESS == state)
    {
        switch(CSD_GET_SENSE_METHOD(ptrFlashWdgt))
        {
        #if (0u != CSD_CSD_MATRIX_WIDGET_EN)
            case CSD_SENSE_METHOD_CSD_E:
                wdCsdMatrix = ptrFlashWdgt->ptr2WdgtRam;
                
                result = CSD_MATRIX_BUTTONS_TOUCHED | 
                         ((uint32)wdCsdMatrix->posSnsId << 16u)  |
                         ((uint32)wdCsdMatrix->posRow   << 8u)   |
                         (uint32)wdCsdMatrix->posCol;
                break;
        #endif /* #if (0u != CSD_CSD_MATRIX_WIDGET_EN) */
        
        #if (0u != CSD_CSX_MATRIX_WIDGET_EN)
            case CSD_SENSE_METHOD_CSX_E:
                result = CSD_dsRam.snsStatus[widgetId];
                break;
        #endif /* #if (0u != CSD_CSX_MATRIX_WIDGET_EN) */
        
        default:
            CYASSERT(0u);
            break;
        }
    }
    
    return result;
}
#endif /* #if (0u != CSD_MATRIX_WIDGET_EN) */

#if (0u != CSD_PROXIMITY_WIDGET_EN)
/*******************************************************************************
* Function Name: CSD_IsProximitySensorActive
****************************************************************************//**
*
* \brief
*  Reports the finger detection status of the specified proximity widget/sensor.
*
* \details 
*  This function reports if the specified proximity sensor has detected a touch
*  or not by extracting the information from the wdgtStatus registers 
*  (CSD_SNS_STATUS<WidgetId>_VALUE). This function should be used 
*  only with the proximity sensor widgets. This function does not process the 
*  widget but extracts the processed results from the \ref group_structures.
*
* \param widgetId  
*  Specify the ID number of the proximity widget.
*  A macro for the widget ID can be found in the CSD Configuration header 
*  file defined as CSD_<WidgetName>_WDGT_ID
*  
* \param proxId    
*  Specify the ID number of the proximity sensor within the proximity widget to 
*  get its touch detection status.
*  A macro for the proximity ID within a specified widget can be found in the
*  CSD Configuration header file defined as 
*  CSD_<WidgetName>_SNS<SensorNumber>_ID
*
* \return 
*  Returns the status of the specified sensor of proximity widget. Zero 
*  indicates that no touch is detected in the specified sensor / widget or a 
*  wrong widgetId / proxId is specified.
*    - Bits [31..2] is reserved.
*    - Bit [1] indicates that a touch is detected.
*    - Bit [0] indicates that a proximity is detected.
*
*******************************************************************************/
uint32 CSD_IsProximitySensorActive(uint32 widgetId, uint32 proxId)
{
    uint32 result = 0Lu;
    
    if ((widgetId < CSD_TOTAL_WIDGETS) && (proxId < CSD_GET_SENSOR_COUNT(widgetId)) &&
        (CSD_WD_PROXIMITY_E == 
            (CSD_WD_TYPE_ENUM)CSD_dsFlash.wdgtArray[widgetId].wdgtType))
    {
        result = CSD_dsRam.snsStatus[widgetId];
        result >>= CSD_PROX_STS_OFFSET(proxId);
        result &=  CSD_PROX_STS_MASK;
    }
    
    return result;
}
#endif /* #if (0u != CSD_PROXIMITY_WIDGET_EN) */


#if (0u != CSD_SLIDER_WIDGET_EN)
/*******************************************************************************
* Function Name: CSD_GetCentroidPos
****************************************************************************//**
*
* \brief
*  Reports the centroid position for the specified slider widget.
*
* \details 
*  This function reports the centrode value of a specified radial or linear 
*  slider widget by extracting the information from the wdgtStatus registers 
*  (CSD_<WidgetName>_POSITION<X>_VALUE). This function should be 
*  used only with the radial or linear slider widgets. This function does not 
*  process the widget but extracts the processed results from the \ref group_structures.
*
* \param widgetId 
*  Specify the ID number of the slider widget to get the centroid of the 
*  detected touch.
*  A macro for the widget ID can be found in the 
*  CSD Configuration header file defined as 
*  CSD_<WidgetName>_WDGT_ID
*
* \return 
*  Returns the centroid position of a specified slider widget:
*    - The centroid position if a touch is detected.
*    - CSD_SLIDER_NO_TOUCH if no touch is detected or a wrong 
*      widgetId is specified.
*
*******************************************************************************/
uint32 CSD_GetCentroidPos(uint32 widgetId)
{
    uint32 result = CSD_SLIDER_NO_TOUCH;
    CSD_RAM_WD_SLIDER_STRUCT *wdSlider;
    
    if ((widgetId < CSD_TOTAL_WIDGETS) && 
        ((CSD_WD_LINEAR_SLIDER_E == 
            (CSD_WD_TYPE_ENUM)CSD_dsFlash.wdgtArray[widgetId].wdgtType) ||
         (CSD_WD_RADIAL_SLIDER_E == 
            (CSD_WD_TYPE_ENUM)CSD_dsFlash.wdgtArray[widgetId].wdgtType)))
    {
        wdSlider = CSD_dsFlash.wdgtArray[widgetId].ptr2WdgtRam;
        result = (uint32)wdSlider->position[0u];
    }
    
    return result;
}
#endif /* #if (0u != CSD_SLIDER_WIDGET_EN) */


#if (0u != CSD_TOUCHPAD_WIDGET_EN)
/*******************************************************************************
* Function Name: CSD_GetXYCoordinates
****************************************************************************//**
*
* \brief
*  Reports the X/Y position detected for the specified touchpad widget.
*
* \details 
*  This function reports a touch position (X and Y coordinates) value of a 
*  specified touchpad widget by extracting the information from the wdgtStatus 
*  registers (CSD_<WidgetName>_POS_Y_VALUE). This function should 
*  be used only with the touchpad widgets. This function does not process the 
*  widget, but extracts the processed results from the \ref group_structures.
*
* \param widgetId 
*  Specify the ID number of the touchpad widget to get the X/Y position of a 
*  detected touch.
*  A macro for the widget ID can be found in the 
*  CSD Configuration header file defined as 
*  CSD_<WidgetName>_WDGT_ID.
*
* \return 
*  Returns the touch position of a specified touchpad widget:
*    1. If a touch is detected:
*      - Bits [31..16] indicates the Y coordinate.
*      - Bits [15..0] indicates the X coordinate.
*    2. If no touch is detected or a wrong widgetId is specified:
*      - CSD_TOUCHPAD_NO_TOUCH.
*
*******************************************************************************/
uint32 CSD_GetXYCoordinates(uint32 widgetId)
{
    CSD_FLASH_WD_STRUCT const *ptrFlashWdgt = NULL;
    
    #if (0u != CSD_CSD_TOUCHPAD_WIDGET_EN)
        CSD_RAM_WD_CSD_TOUCHPAD_STRUCT *wdCsdTouchpad;
    #endif /* #if (0u != CSD_CSD_TOUCHPAD_WIDGET_EN) */
    
    #if (0u != CSD_CSX_TOUCHPAD_WIDGET_EN)
        CSD_RAM_WD_CSX_TOUCHPAD_STRUCT *wdCsxTouchpad;
    #endif /* #if (0u != CSD_CSX_TOUCHPAD_WIDGET_EN) */
    
    cystatus state = CYRET_SUCCESS;
    uint32 result = CSD_TOUCHPAD_NO_TOUCH;

    if (widgetId >= CSD_TOTAL_WIDGETS)
    {
        state = CYRET_BAD_PARAM;
    }
    else
    {
        ptrFlashWdgt = &CSD_dsFlash.wdgtArray[widgetId];
         
        if ((CSD_WD_TYPE_ENUM)ptrFlashWdgt->wdgtType != CSD_WD_TOUCHPAD_E)
        {
            state = CYRET_BAD_PARAM;
        }
    }
      
    if (CYRET_SUCCESS == state)
    {
        switch(CSD_GET_SENSE_METHOD(ptrFlashWdgt))
        {
        #if (0u != CSD_CSD_TOUCHPAD_WIDGET_EN)
            case CSD_SENSE_METHOD_CSD_E:
                wdCsdTouchpad = ptrFlashWdgt->ptr2WdgtRam;
                
                result = ((uint32)wdCsdTouchpad->posX) |
                         ((uint32)wdCsdTouchpad->posY << 16u);
                break;
        #endif /* #if (0u != CSD_CSD_TOUCHPAD_WIDGET_EN) */
        
        #if (0u != CSD_CSX_TOUCHPAD_WIDGET_EN)
            case CSD_SENSE_METHOD_CSX_E:
                wdCsxTouchpad = ptrFlashWdgt->ptr2WdgtRam;
                
                result = ((uint32)wdCsxTouchpad->touch[0u].x) |
                         ((uint32)wdCsxTouchpad->touch[0u].y << 16u);
                break;
        #endif /* #if (0u != CSD_CSX_TOUCHPAD_WIDGET_EN) */
        
        default:
            CYASSERT(0u);
            break;
        }
    }
    
    return result;
}
#endif /* #if (0u != CSD_TOUCHPAD_WIDGET_EN) */


/* [] END OF FILE */
