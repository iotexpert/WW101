/*******************************************************************************
* \file       CSD_Configuration.h
* \version    3.10
*
* \brief
*   This file provides the customizer parameters definitions.
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

#if !defined(CY_CAPSENSE_CSD_CONFIGURATION_H)
#define CY_CAPSENSE_CSD_CONFIGURATION_H

#include <cytypes.h>

/*******************************************************************************
* Customizer-generated defines
*******************************************************************************/
#define CSD_ENABLE                   (1u)
#define CSD_DISABLE                  (0u)

/*******************************************************************************
* Creator-global defines
*******************************************************************************/
#ifdef CYIPBLOCK_m0s8csdv2_VERSION
    #define CSD_CSDV2                              (1u)
#else
    #define CSD_CSDV2                              (0u)
#endif  /* CYIPBLOCK_m0s8csdv2_VERSION */

#define CSD_2000_MV                            (2000u)

#ifdef CYDEV_VDDA_MV
    #define CSD_CYDEV_VDDA_MV                  (CYDEV_VDDA_MV)
#else
    #define CSD_CYDEV_VDDA_MV                  (CYDEV_VDD_MV)
#endif  /* #ifdef CYDEV_VDDA_MV */

#define CSD_BAD_CONVERSIONS_NUM                (1u)
#define CSD_RESAMPLING_CYCLES_MAX_NUMBER       (1u)

/*******************************************************************************
* Enabled Scan Methods
*******************************************************************************/
#define CSD_CSD_EN                   (1u)
#define CSD_CSX_EN                   (0u)
#define CSD_CSD_CSX_EN               (CSD_CSD_EN & CSD_CSX_EN)
#define CSD_CSD2X_EN                 (0u)
#define CSD_CSX2X_EN                 (0u)

/*******************************************************************************
* Definitions for number of widgets and sensors
*******************************************************************************/
#define CSD_TOTAL_WIDGETS            (1u)
#define CSD_TOTAL_CSD_WIDGETS        (1u)
#define CSD_TOTAL_CSD_SENSORS        (2u)
#define CSD_TOTAL_CSX_WIDGETS        (0u)
#define CSD_TOTAL_CSX_NODES          (0u)

/*******************************************************************************
* Total number of CSD sensors + CSX nodes
*******************************************************************************/
#define CSD_TOTAL_SENSORS            (CSD_TOTAL_CSD_SENSORS + \
                                                   CSD_TOTAL_CSX_NODES)

/*******************************************************************************
* Total number of scan slots (used only when dual-channel scan is enabled)
*******************************************************************************/
#define CSD_TOTAL_SCAN_SLOTS         (2u)

/*******************************************************************************
* Define widget IDs
*******************************************************************************/
#define CSD_BUTTON0_WDGT_ID                     (0u)

/*******************************************************************************
* Define sensor IDs
*******************************************************************************/

/* Button0 */
#define CSD_BUTTON0_SNS0_ID                     (0u)
#define CSD_BUTTON0_SNS1_ID                     (1u)



/*******************************************************************************
* Enabled widget types
*******************************************************************************/
#define CSD_BUTTON_WIDGET_EN         (1u)
#define CSD_SLIDER_WIDGET_EN         (0u)
#define CSD_MATRIX_WIDGET_EN         (0u)
#define CSD_PROXIMITY_WIDGET_EN      (0u)
#define CSD_TOUCHPAD_WIDGET_EN       (0u)

#define CSD_CSD_MATRIX_WIDGET_EN     (0u)
#define CSD_CSD_TOUCHPAD_WIDGET_EN   (0u)

#define CSD_CSX_MATRIX_WIDGET_EN     (0u)
#define CSD_CSX_TOUCHPAD_WIDGET_EN   (0u)

/*******************************************************************************
* Centroid APIs
*******************************************************************************/
#define CSD_CENTROID_EN              (0u)
#define CSD_TOTAL_DIPLEXED_SLIDERS   (0u)
#define CSD_TOTAL_LINEAR_SLIDERS     (0u)
#define CSD_TOTAL_RADIAL_SLIDERS     (0u)
#define CSD_TOTAL_TOUCHPADS          (0u)
#define CSD_MAX_CENTROID_LENGTH      (0u)

/*******************************************************************************
* Enabled sensor types
*******************************************************************************/
#define CSD_REGULAR_SENSOR_EN        (1u)
#define CSD_PROXIMITY_SENSOR_EN      (0u)

/*******************************************************************************
* Sensor ganging
*******************************************************************************/
#define CSD_GANGED_SNS_EN            (0u)
#define CSD_CSD_GANGED_SNS_EN        (0u)
#define CSD_CSX_GANGED_SNS_EN        (0u)

/*******************************************************************************
* Max number of sensors used among all widgets
*******************************************************************************/
#define CSD_MAX_SENSORS_PER_WIDGET   (2u)

/*******************************************************************************
* Total number of all used electrodes (NOT unique)
*******************************************************************************/
#define CSD_TOTAL_ELECTRODES         (2u)
/* Obsolete */
#define CSD_TOTAL_SENSOR_IOS         CSD_TOTAL_ELECTRODES

/*******************************************************************************
* Total number of used physical IOs (unique)
*******************************************************************************/
#define CSD_TOTAL_IO_CNT             (2u)

/*******************************************************************************
* Array length for widget status registers
*******************************************************************************/
#define CSD_WDGT_STATUS_WORDS        \
                        (((uint8)((CSD_TOTAL_WIDGETS - 1u) / 32u)) + 1u)


/*******************************************************************************
* Auto-tuning mode selection
*******************************************************************************/
#define CSD_CSD_SS_DIS         (0x00ul)
#define CSD_CSD_SS_HW_EN       (0x01ul)
#define CSD_CSD_SS_TH_EN       (0x02ul)
#define CSD_CSD_SS_HWTH_EN     (CSD_CSD_SS_HW_EN | \
                                             CSD_CSD_SS_TH_EN)

#define CSD_CSD_AUTOTUNE       CSD_CSD_SS_DIS


/*******************************************************************************
* General settings
*******************************************************************************/

#define CSD_AUTO_RESET_METHOD_LEGACY (0u)
#define CSD_AUTO_RESET_METHOD_SAMPLE (1u)

#define CSD_MULTI_FREQ_SCAN_EN       (0u)
#define CSD_SENSOR_AUTO_RESET_EN     (0u)
#define CSD_SENSOR_AUTO_RESET_METHOD (0u)
#define CSD_NUM_CENTROIDS            (1u)
#define CSD_OFF_DEBOUNCE_EN          (0u)

/* Define power status of HW IP block after scanning */
#define CSD_BLOCK_OFF_AFTER_SCAN_EN  (0u)

/* Define number of scan frequencies */
#if (CSD_DISABLE != CSD_MULTI_FREQ_SCAN_EN)
    #define CSD_NUM_SCAN_FREQS       (3u)
#else
    #define CSD_NUM_SCAN_FREQS       (1u)
#endif /* #if (CSD_DISABLE != CSD_MULTI_FREQ_SCAN_EN) */

/* Data size for thresholds / low baseline reset */
#define CSD_SIZE_8BITS               (8u)
#define CSD_SIZE_16BITS              (16u)

#define CSD_THRESHOLD_SIZE           CSD_SIZE_16BITS
typedef uint16 CSD_THRESHOLD_TYPE;

#if (CSD_AUTO_RESET_METHOD_LEGACY == CSD_SENSOR_AUTO_RESET_METHOD)
    #define CSD_LOW_BSLN_RST_SIZE        CSD_SIZE_8BITS
    typedef uint8 CSD_LOW_BSLN_RST_TYPE;
#else
    #define CSD_LOW_BSLN_RST_SIZE    (16u)
    typedef uint16 CSD_LOW_BSLN_RST_TYPE;
#endif /* #if (CSD_AUTO_RESET_METHOD_LEGACY == CSD_SENSOR_AUTO_RESET_METHOD) */

/* Coefficient to define touch threshold for proximity sensors */
#define CSD_PROX_TOUCH_COEFF         (300u)

/*******************************************************************************
* General Filter Constants
*******************************************************************************/

/* Baseline algorithm options */
#define CSD_IIR_BASELINE                 (0u)
#define CSD_BUCKET_BASELINE              (1u)

#define CSD_BASELINE_TYPE                CSD_IIR_BASELINE

/* IIR baseline filter algorithm for regular sensors*/
#define CSD_REGULAR_IIR_BL_TYPE          CSD_IIR_FILTER_PERFORMANCE

/* IIR baseline coefficients for regular sensors */
#define CSD_REGULAR_IIR_BL_N             (1u)
#define CSD_REGULAR_IIR_BL_SHIFT         (8u)

/* IIR baseline filter algorithm for proximity sensors*/
#define CSD_PROX_IIR_BL_TYPE             CSD_IIR_FILTER_PERFORMANCE

/* IIR baseline coefficients for proximity sensors */
#define CSD_PROX_IIR_BL_N                (1u)
#define CSD_PROX_IIR_BL_SHIFT            (8u)


/* IIR filter constants */
#define CSD_IIR_COEFFICIENT_K            (256u)

/* IIR filter type */
#define CSD_IIR_FILTER_STANDARD          (1u)
#define CSD_IIR_FILTER_PERFORMANCE       (2u)
#define CSD_IIR_FILTER_MEMORY            (3u)

/* Regular sensor raw-count filters */
#define CSD_REGULAR_RC_FILTER_EN         (1u)
#define CSD_REGULAR_RC_IIR_FILTER_EN     (1u)
#define CSD_REGULAR_RC_MEDIAN_FILTER_EN  (0u)
#define CSD_REGULAR_RC_AVERAGE_FILTER_EN (0u)
#define CSD_REGULAR_RC_CUSTOM_FILTER_EN  (0u)

/* Proximity sensor raw-count filters */
#define CSD_PROX_RC_FILTER_EN            (0u)
#define CSD_PROX_RC_IIR_FILTER_EN        (0u)
#define CSD_PROX_RC_MEDIAN_FILTER_EN     (0u)
#define CSD_PROX_RC_AVERAGE_FILTER_EN    (0u)
#define CSD_PROX_RC_CUSTOM_FILTER_EN     (0u)

/* IIR raw-count filter algorithm for regular sensors */
#define CSD_REGULAR_IIR_RC_TYPE          (CSD_IIR_FILTER_STANDARD)

/* IIR raw-count filter coefficients for regular sensors */
#define CSD_REGULAR_IIR_RC_N             (128u)
#define CSD_REGULAR_IIR_RC_SHIFT         (0u)

/* IIR raw-count filter algorithm for proximity sensors*/
#define CSD_PROX_IIR_RC_TYPE             (CSD_IIR_FILTER_STANDARD)

/* IIR raw-count filter coefficients for proximity sensors */
#define CSD_PROX_IIR_RC_N                (128u)
#define CSD_PROX_IIR_RC_SHIFT            (0u)

/* Median filter constants */

/* Order of regular sensor median filter */
#define CSD_REGULAR_MEDIAN_LEN           (2u)

/* Order of proximity sensor median filter */
#define CSD_PROX_MEDIAN_LEN              (2u)

/* Average filter constants*/
#define CSD_AVERAGE_FILTER_LEN_2         (2u)
#define CSD_AVERAGE_FILTER_LEN_4         (4u)

/* Order of regular sensor average filter */
#define CSD_REGULAR_AVERAGE_LEN          (CSD_AVERAGE_FILTER_LEN_4)

/* Order of proximity sensor average filter */
#define CSD_PROX_AVERAGE_LEN             (CSD_AVERAGE_FILTER_LEN_4)


/* Centroid position filters */
#define CSD_POS_IIR_FILTER_EN            (0x00u)
#define CSD_POS_MEDIAN_FILTER_EN         (0x00u)
#define CSD_POS_AVERAGE_FILTER_EN        (0x00u)
#define CSD_POS_JITTER_FILTER_EN         (0x00u)
#define CSD_CSX_TOUCHPAD_POS_MEDIAN_FILTER_EN (0x00u)

#define CSD_POS_IIR_COEFF                (128u)
#define CSD_CSX_TOUCHPAD_UNDEFINED       (40u)


/* IDAC options */

/* CSDv1 IDAC gain */
#define CSD_IDAC_GAIN_4X                 (4u)
#define CSD_IDAC_GAIN_8X                 (8u)

/* CSDv2 IDAC gain */
#define CSD_IDAC_GAIN_LOW                (0uL)
#define CSD_IDAC_GAIN_MEDIUM             (1uL)
#define CSD_IDAC_GAIN_HIGH               (2uL)

#define CSD_IDAC_SOURCING                (0u)
#define CSD_IDAC_SINKING                 (1u)

/* Shield tank capacitor precharge source */
#define CSD_CSH_PRECHARGE_VREF           (0u)
#define CSD_CSH_PRECHARGE_IO_BUF         (1u)

/* Shield electrode delay */
#define CSD_NO_DELAY                     (0u)
/* CSDv1 */
#define CSD_SH_DELAY_50NS                (1u)
#define CSD_SH_DELAY_100NS               (2u)
/* CSDv2 */
#define CSD_SH_DELAY_5NS                 (1u)
#define CSD_SH_DELAY_10NS                (2u)
#define CSD_SH_DELAY_20NS                (3u)

/* Inactive sensor connection options */
#define CSD_SNS_CONNECTION_GROUND        (0x00000006Lu)
#define CSD_SNS_CONNECTION_HIGHZ         (0x00000000Lu)
#define CSD_SNS_CONNECTION_SHIELD        (0x00000002Lu)

/* Sense clock selection options */
#if defined(CSD_TAPEOUT_STAR_USED)
    #define CSD_CSDV2_REF9P6UA_EN            (0u)
#else
    #define CSD_CSDV2_REF9P6UA_EN            (0u)
#endif /* defined(CSD_TAPEOUT_A_USED) */

#define CSD_CLK_SOURCE_DIRECT            (0x00000000Lu)

#define CSD_CLK_SOURCE_SSC1              (0x01u)
#define CSD_CLK_SOURCE_SSC2              (0x02u)
#define CSD_CLK_SOURCE_SSC3              (0x03u)
#define CSD_CLK_SOURCE_SSC4              (0x04u)

/* CSDv1 and CSDv2 */
#define CSD_CLK_SOURCE_PRS8              (0x05u)
#define CSD_CLK_SOURCE_PRS12             (0x06u)
#define CSD_CLK_SOURCE_PRSAUTO           (0xFFu)

/* Defines scan resolutions */
#define CSD_RES6BIT                      (6u)
#define CSD_RES7BIT                      (7u)
#define CSD_RES8BIT                      (8u)
#define CSD_RES9BIT                      (9u)
#define CSD_RES10BIT                     (10u)
#define CSD_RES11BIT                     (11u)
#define CSD_RES12BIT                     (12u)
#define CSD_RES13BIT                     (13u)
#define CSD_RES14BIT                     (14u)
#define CSD_RES15BIT                     (15u)
#define CSD_RES16BIT                     (16u)

/* CSDv2: Initialization switch resistance */
#define CSD_INIT_SW_RES_LOW              (0x00000000Lu)
#define CSD_INIT_SW_RES_MEDIUM           (0x00000001Lu)
#define CSD_INIT_SW_RES_HIGH             (0x00000002Lu)

/* CSDv2: Initialization switch resistance */
#define CSD_SCAN_SW_RES_LOW              (0x00000000Lu)
#define CSD_SCAN_SW_RES_MEDIUM           (0x00000001Lu)
#define CSD_SCAN_SW_RES_HIGH             (0x00000002Lu)

/* CSDv2: CSD shield switch resistance */
#define CSD_SHIELD_SW_RES_LOW            (0x00000000Lu)
#define CSD_SHIELD_SW_RES_MEDIUM         (0x00000001Lu)
#define CSD_SHIELD_SW_RES_HIGH           (0x00000002Lu)
#define CSD_SHIELD_SW_RES_LOW_EMI        (0x00000003Lu)

/* CSDv2: CSD shield switch resistance */
#define CSD_INIT_SHIELD_SW_RES_LOW       (0x00000000Lu)
#define CSD_INIT_SHIELD_SW_RES_MEDIUM    (0x00000001Lu)
#define CSD_INIT_SHIELD_SW_RES_HIGH      (0x00000002Lu)
#define CSD_INIT_SHIELD_SW_RES_LOW_EMI   (0x00000003Lu)

/* CSDv2: CSD shield switch resistance */
#define CSD_SCAN_SHIELD_SW_RES_LOW            (0x00000000Lu)
#define CSD_SCAN_SHIELD_SW_RES_MEDIUM         (0x00000001Lu)
#define CSD_SCAN_SHIELD_SW_RES_HIGH           (0x00000002Lu)
#define CSD_SCAN_SHIELD_SW_RES_LOW_EMI        (0x00000003Lu)

/*******************************************************************************
* CSD Specific settings
*******************************************************************************/

/* CSD scan method settings */
#define CSD_CSD_IDAC_AUTOCAL_EN          (1u)
#define CSD_CSD_IDAC_GAIN                (CSD_IDAC_GAIN_HIGH)
#define CSD_CSD_SHIELD_EN                (0u)
#define CSD_CSD_SHIELD_TANK_EN           (0u)
#define CSD_CSD_CSH_PRECHARGE_SRC        (CSD_CSH_PRECHARGE_VREF)
#define CSD_CSD_SHIELD_DELAY             (CSD_NO_DELAY)
#define CSD_CSD_TOTAL_SHIELD_COUNT       (0u)
#define CSD_CSD_SCANSPEED_DIVIDER        (1u)
#define CSD_CSD_COMMON_SNS_CLK_EN        (0u)
#define CSD_CSD_SNS_CLK_SOURCE           (CSD_CLK_SOURCE_DIRECT)
#define CSD_CSD_SNS_CLK_DIVIDER          (8u)
#define CSD_CSD_INACTIVE_SNS_CONNECTION  (CSD_SNS_CONNECTION_GROUND)
#define CSD_CSD_IDAC_COMP_EN             (1u)
#define CSD_CSD_IDAC_CONFIG              (CSD_IDAC_SOURCING)
#define CSD_CSD_RAWCOUNT_CAL_LEVEL       (85u)
#define CSD_CSD_DUALIDAC_LEVEL           (50u)
#define CSD_CSD_PRESCAN_SETTLING_TIME    (5u)
#define CSD_CSD_SNSCLK_R_CONST           (1000u)
#define CSD_CSD_VREF_MV                  (2021u)

/* CSD settings - CSDv2 */
#define CSD_CSD_DEDICATED_IDAC_COMP_EN   (1u)
#define CSD_CSD_AUTO_ZERO_EN             (0u)
#define CSD_CSD_AUTO_ZERO_TIME           (1Lu)
#define CSD_CSD_NOISE_METRIC_EN          (0u)
#define CSD_CSD_NOISE_METRIC_TH          (1Lu)
#define CSD_CSD_INIT_SWITCH_RES          (CSD_INIT_SW_RES_MEDIUM)
#define CSD_CSD_SHIELD_SWITCH_RES        (CSD_SHIELD_SW_RES_MEDIUM)
#define CSD_CSD_GAIN                     (18Lu)


/*******************************************************************************
* CSX Specific settings
*******************************************************************************/

/* CSX scan method settings */
#define CSD_CSX_SCANSPEED_DIVIDER        (1u)
#define CSD_CSX_COMMON_TX_CLK_EN         (0u)
#define CSD_CSX_TX_CLK_SOURCE            (CSD_CLK_SOURCE_PRSAUTO)
#define CSD_CSX_TX_CLK_DIVIDER           (80u)
#define CSD_CSX_MAX_FINGERS              (1u)
#define CSD_CSX_MAX_LOCAL_PEAKS          (5u)
#define CSD_CSX_IDAC_AUTOCAL_EN          (0u)
#define CSD_CSX_IDAC_BITS_USED           (7u)
#define CSD_CSX_RAWCOUNT_CAL_LEVEL       (40u)
#define CSD_CSX_IDAC_GAIN                (CSD_IDAC_GAIN_MEDIUM)
#define CSD_CSX_SKIP_OVSMPL_SPECIFIC_NODES (0u)
#define CSD_CSX_MULTIPHASE_TX_EN         (0u)
#define CSD_CSX_MAX_TX_PHASE_LENGTH      (0u)

/* CSX settings - CSDv2 */
#define CSD_CSX_AUTO_ZERO_EN             (0u)
#define CSD_CSX_AUTO_ZERO_TIME           (1u)
#define CSD_CSX_FINE_INIT_TIME           (4u)
#define CSD_CSX_NOISE_METRIC_EN          (0u)
#define CSD_CSX_NOISE_METRIC_TH          (1u)
#define CSD_CSX_INIT_SWITCH_RES          (CSD_INIT_SW_RES_MEDIUM)
#define CSD_CSX_SCAN_SWITCH_RES          (CSD_SCAN_SW_RES_LOW)
#define CSD_CSX_INIT_SHIELD_SWITCH_RES   (CSD_INIT_SHIELD_SW_RES_HIGH)
#define CSD_CSX_SCAN_SHIELD_SWITCH_RES   (CSD_SCAN_SHIELD_SW_RES_LOW)


/*******************************************************************************
* Global Parameter Definitions
*******************************************************************************/

/* RAM Global Parameters Definitions */
#define CSD_CONFIG_ID                           (0x7C30u)
#define CSD_DEVICE_ID                           (0x0160u)
#define CSD_CSD0_CONFIG                         (0x0008u)

/***************************************************************************//**
* Button0 initialization values for FLASH data structure
*******************************************************************************/
#define CSD_BUTTON0_STATIC_CONFIG               (1u)
#define CSD_BUTTON0_NUM_SENSORS                 (2u)

/***************************************************************************//**
* Button0 initialization values for RAM data structure
*******************************************************************************/
#define CSD_BUTTON0_RESOLUTION                  (CSD_RES16BIT)
#define CSD_BUTTON0_FINGER_TH                   (100u)
#define CSD_BUTTON0_NOISE_TH                    (40u)
#define CSD_BUTTON0_NNOISE_TH                   (40u)
#define CSD_BUTTON0_HYSTERESIS                  (10u)
#define CSD_BUTTON0_ON_DEBOUNCE                 (3u)
#define CSD_BUTTON0_LOW_BSLN_RST                (30u)
#define CSD_BUTTON0_BSLN_COEFF                  (100u)
#define CSD_BUTTON0_IDAC_MOD0                   (32u)
#define CSD_BUTTON0_SNS_CLK                     (128u)
#define CSD_BUTTON0_SNS_CLK_SOURCE              (0u)

/* RAM Sensor Parameters Definitions */
#define CSD_BUTTON0_SNS0_IDAC_COMP0             (32u)
#define CSD_BUTTON0_SNS1_IDAC_COMP0             (32u)


/*******************************************************************************
* ADC Specific Macros
*******************************************************************************/
#define CSD_ADC_RES8BIT                  (8u)
#define CSD_ADC_RES10BIT                 (10u)

#define CSD_ADC_EN                       (0u)
#define CSD_ADC_STANDALONE_EN            (0u)
#define CSD_ADC_TOTAL_CHANNELS           (1u)
#define CSD_ADC_RESOLUTION               (CSD_ADC_RES10BIT)
#define CSD_ADC_AMUXB_INPUT_EN           (0u)
#define CSD_ADC_SELECT_AMUXB_CH          (0u)
#define CSD_ADC_AZ_EN                    (0Lu)
#define CSD_ADC_AZ_TIME                  (10u)
#define CSD_ADC_VREF_MV                  (2133u)
#define CSD_ADC_GAIN                     (17Lu)
#define CSD_ADC_IDACVAL                  (22u)
#define CSD_ADC_MEASURE_MODE             (0u)

/*******************************************************************************
* Built-In Self-Test Configuration
*******************************************************************************/
#define CSD_SELF_TEST_EN                   (0Lu)
#define CSD_TST_GLOBAL_CRC_EN              (0Lu)
#define CSD_TST_WDGT_CRC_EN                (0Lu)
#define CSD_TST_BSLN_DUPLICATION_EN        (0Lu)
#define CSD_TST_BSLN_RAW_OUT_RANGE_EN      (0Lu)
#define CSD_TST_SNS_SHORT_EN               (0Lu)
#define CSD_TST_SNS2SNS_SHORT_EN           (0Lu)
#define CSD_TST_SNS_CAP_EN                 (0Lu)
#define CSD_TST_SH_CAP_EN                  (0Lu)
#define CSD_TST_EXTERNAL_CAP_EN            (0Lu)
#define CSD_TST_INTERNAL_CAP_EN            (0Lu)
#define CSD_TST_VDDA_EN                    (0Lu)


#endif /* CY_CAPSENSE_CSD_CONFIGURATION_H */


/* [] END OF FILE */
