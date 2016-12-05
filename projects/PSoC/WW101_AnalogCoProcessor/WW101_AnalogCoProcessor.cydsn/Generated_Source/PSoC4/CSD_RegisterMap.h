/***************************************************************************//**
* \file CSD_RegisterMap.h
* \version 3.10
*
* \brief
*   This file provides the definitions for the component data structure register.
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

#if !defined(CY_CAPSENSE_CSD_REGISTER_MAP_H)
#define CY_CAPSENSE_CSD_REGISTER_MAP_H

#include <cytypes.h>
#include "CSD_Configuration.h"
#include "CSD_Structure.h"

/*****************************************************************************/
/* RAM Data structure register definitions                                   */
/*****************************************************************************/
#define CSD_CONFIG_ID_VALUE                                 (CSD_dsRam.configId)
#define CSD_CONFIG_ID_OFFSET                                (0u)
#define CSD_CONFIG_ID_SIZE                                  (2u)
#define CSD_CONFIG_ID_PARAM_ID                              (0x87000000u)

#define CSD_DEVICE_ID_VALUE                                 (CSD_dsRam.deviceId)
#define CSD_DEVICE_ID_OFFSET                                (2u)
#define CSD_DEVICE_ID_SIZE                                  (2u)
#define CSD_DEVICE_ID_PARAM_ID                              (0x8B000002u)

#define CSD_TUNER_CMD_VALUE                                 (CSD_dsRam.tunerCmd)
#define CSD_TUNER_CMD_OFFSET                                (4u)
#define CSD_TUNER_CMD_SIZE                                  (2u)
#define CSD_TUNER_CMD_PARAM_ID                              (0xAD000004u)

#define CSD_SCAN_COUNTER_VALUE                              (CSD_dsRam.scanCounter)
#define CSD_SCAN_COUNTER_OFFSET                             (6u)
#define CSD_SCAN_COUNTER_SIZE                               (2u)
#define CSD_SCAN_COUNTER_PARAM_ID                           (0x8A000006u)

#define CSD_STATUS_VALUE                                    (CSD_dsRam.status)
#define CSD_STATUS_OFFSET                                   (8u)
#define CSD_STATUS_SIZE                                     (4u)
#define CSD_STATUS_PARAM_ID                                 (0xCA000008u)

#define CSD_WDGT_ENABLE0_VALUE                              (CSD_dsRam.wdgtEnable[0u])
#define CSD_WDGT_ENABLE0_OFFSET                             (12u)
#define CSD_WDGT_ENABLE0_SIZE                               (4u)
#define CSD_WDGT_ENABLE0_PARAM_ID                           (0xE000000Cu)

#define CSD_WDGT_STATUS0_VALUE                              (CSD_dsRam.wdgtStatus[0u])
#define CSD_WDGT_STATUS0_OFFSET                             (16u)
#define CSD_WDGT_STATUS0_SIZE                               (4u)
#define CSD_WDGT_STATUS0_PARAM_ID                           (0xCD000010u)

#define CSD_SNS_STATUS0_VALUE                               (CSD_dsRam.snsStatus[0u])
#define CSD_SNS_STATUS0_OFFSET                              (20u)
#define CSD_SNS_STATUS0_SIZE                                (1u)
#define CSD_SNS_STATUS0_PARAM_ID                            (0x4B000014u)

#define CSD_CSD0_CONFIG_VALUE                               (CSD_dsRam.csd0Config)
#define CSD_CSD0_CONFIG_OFFSET                              (22u)
#define CSD_CSD0_CONFIG_SIZE                                (2u)
#define CSD_CSD0_CONFIG_PARAM_ID                            (0xA9800016u)

#define CSD_MOD_CSD_CLK_VALUE                               (CSD_dsRam.modCsdClk)
#define CSD_MOD_CSD_CLK_OFFSET                              (24u)
#define CSD_MOD_CSD_CLK_SIZE                                (1u)
#define CSD_MOD_CSD_CLK_PARAM_ID                            (0x6E800018u)

#define CSD_BUTTON0_RESOLUTION_VALUE                        (CSD_dsRam.wdgtList.button0.resolution)
#define CSD_BUTTON0_RESOLUTION_OFFSET                       (26u)
#define CSD_BUTTON0_RESOLUTION_SIZE                         (2u)
#define CSD_BUTTON0_RESOLUTION_PARAM_ID                     (0xAA80001Au)

#define CSD_BUTTON0_FINGER_TH_VALUE                         (CSD_dsRam.wdgtList.button0.fingerTh)
#define CSD_BUTTON0_FINGER_TH_OFFSET                        (28u)
#define CSD_BUTTON0_FINGER_TH_SIZE                          (2u)
#define CSD_BUTTON0_FINGER_TH_PARAM_ID                      (0xA780001Cu)

#define CSD_BUTTON0_NOISE_TH_VALUE                          (CSD_dsRam.wdgtList.button0.noiseTh)
#define CSD_BUTTON0_NOISE_TH_OFFSET                         (30u)
#define CSD_BUTTON0_NOISE_TH_SIZE                           (1u)
#define CSD_BUTTON0_NOISE_TH_PARAM_ID                       (0x6380001Eu)

#define CSD_BUTTON0_NNOISE_TH_VALUE                         (CSD_dsRam.wdgtList.button0.nNoiseTh)
#define CSD_BUTTON0_NNOISE_TH_OFFSET                        (31u)
#define CSD_BUTTON0_NNOISE_TH_SIZE                          (1u)
#define CSD_BUTTON0_NNOISE_TH_PARAM_ID                      (0x6580001Fu)

#define CSD_BUTTON0_HYSTERESIS_VALUE                        (CSD_dsRam.wdgtList.button0.hysteresis)
#define CSD_BUTTON0_HYSTERESIS_OFFSET                       (32u)
#define CSD_BUTTON0_HYSTERESIS_SIZE                         (1u)
#define CSD_BUTTON0_HYSTERESIS_PARAM_ID                     (0x63800020u)

#define CSD_BUTTON0_ON_DEBOUNCE_VALUE                       (CSD_dsRam.wdgtList.button0.onDebounce)
#define CSD_BUTTON0_ON_DEBOUNCE_OFFSET                      (33u)
#define CSD_BUTTON0_ON_DEBOUNCE_SIZE                        (1u)
#define CSD_BUTTON0_ON_DEBOUNCE_PARAM_ID                    (0x65800021u)

#define CSD_BUTTON0_LOW_BSLN_RST_VALUE                      (CSD_dsRam.wdgtList.button0.lowBslnRst)
#define CSD_BUTTON0_LOW_BSLN_RST_OFFSET                     (34u)
#define CSD_BUTTON0_LOW_BSLN_RST_SIZE                       (1u)
#define CSD_BUTTON0_LOW_BSLN_RST_PARAM_ID                   (0x6F800022u)

#define CSD_BUTTON0_BSLN_COEFF_VALUE                        (CSD_dsRam.wdgtList.button0.bslnCoeff)
#define CSD_BUTTON0_BSLN_COEFF_OFFSET                       (35u)
#define CSD_BUTTON0_BSLN_COEFF_SIZE                         (1u)
#define CSD_BUTTON0_BSLN_COEFF_PARAM_ID                     (0x69800023u)

#define CSD_BUTTON0_IDAC_MOD0_VALUE                         (CSD_dsRam.wdgtList.button0.idacMod[0u])
#define CSD_BUTTON0_IDAC_MOD0_OFFSET                        (36u)
#define CSD_BUTTON0_IDAC_MOD0_SIZE                          (1u)
#define CSD_BUTTON0_IDAC_MOD0_PARAM_ID                      (0x44000024u)

#define CSD_BUTTON0_SNS_CLK_VALUE                           (CSD_dsRam.wdgtList.button0.snsClk)
#define CSD_BUTTON0_SNS_CLK_OFFSET                          (37u)
#define CSD_BUTTON0_SNS_CLK_SIZE                            (1u)
#define CSD_BUTTON0_SNS_CLK_PARAM_ID                        (0x64800025u)

#define CSD_BUTTON0_SNS_CLK_SOURCE_VALUE                    (CSD_dsRam.wdgtList.button0.snsClkSource)
#define CSD_BUTTON0_SNS_CLK_SOURCE_OFFSET                   (38u)
#define CSD_BUTTON0_SNS_CLK_SOURCE_SIZE                     (1u)
#define CSD_BUTTON0_SNS_CLK_SOURCE_PARAM_ID                 (0x45800026u)

#define CSD_BUTTON0_SNS0_RAW0_VALUE                         (CSD_dsRam.snsList.button0[0u].raw[0u])
#define CSD_BUTTON0_SNS0_RAW0_OFFSET                        (40u)
#define CSD_BUTTON0_SNS0_RAW0_SIZE                          (2u)
#define CSD_BUTTON0_SNS0_RAW0_PARAM_ID                      (0x8F000028u)

#define CSD_BUTTON0_SNS0_BSLN0_VALUE                        (CSD_dsRam.snsList.button0[0u].bsln[0u])
#define CSD_BUTTON0_SNS0_BSLN0_OFFSET                       (42u)
#define CSD_BUTTON0_SNS0_BSLN0_SIZE                         (2u)
#define CSD_BUTTON0_SNS0_BSLN0_PARAM_ID                     (0x8300002Au)

#define CSD_BUTTON0_SNS0_BSLN_EXT0_VALUE                    (CSD_dsRam.snsList.button0[0u].bslnExt[0u])
#define CSD_BUTTON0_SNS0_BSLN_EXT0_OFFSET                   (44u)
#define CSD_BUTTON0_SNS0_BSLN_EXT0_SIZE                     (1u)
#define CSD_BUTTON0_SNS0_BSLN_EXT0_PARAM_ID                 (0x4600002Cu)

#define CSD_BUTTON0_SNS0_DIFF_VALUE                         (CSD_dsRam.snsList.button0[0u].diff)
#define CSD_BUTTON0_SNS0_DIFF_OFFSET                        (46u)
#define CSD_BUTTON0_SNS0_DIFF_SIZE                          (2u)
#define CSD_BUTTON0_SNS0_DIFF_PARAM_ID                      (0x8200002Eu)

#define CSD_BUTTON0_SNS0_NEG_BSLN_RST_CNT0_VALUE            (CSD_dsRam.snsList.button0[0u].negBslnRstCnt[0u])
#define CSD_BUTTON0_SNS0_NEG_BSLN_RST_CNT0_OFFSET           (48u)
#define CSD_BUTTON0_SNS0_NEG_BSLN_RST_CNT0_SIZE             (1u)
#define CSD_BUTTON0_SNS0_NEG_BSLN_RST_CNT0_PARAM_ID         (0x40000030u)

#define CSD_BUTTON0_SNS0_IDAC_COMP0_VALUE                   (CSD_dsRam.snsList.button0[0u].idacComp[0u])
#define CSD_BUTTON0_SNS0_IDAC_COMP0_OFFSET                  (49u)
#define CSD_BUTTON0_SNS0_IDAC_COMP0_SIZE                    (1u)
#define CSD_BUTTON0_SNS0_IDAC_COMP0_PARAM_ID                (0x46000031u)

#define CSD_BUTTON0_SNS1_RAW0_VALUE                         (CSD_dsRam.snsList.button0[1u].raw[0u])
#define CSD_BUTTON0_SNS1_RAW0_OFFSET                        (50u)
#define CSD_BUTTON0_SNS1_RAW0_SIZE                          (2u)
#define CSD_BUTTON0_SNS1_RAW0_PARAM_ID                      (0x84000032u)

#define CSD_BUTTON0_SNS1_BSLN0_VALUE                        (CSD_dsRam.snsList.button0[1u].bsln[0u])
#define CSD_BUTTON0_SNS1_BSLN0_OFFSET                       (52u)
#define CSD_BUTTON0_SNS1_BSLN0_SIZE                         (2u)
#define CSD_BUTTON0_SNS1_BSLN0_PARAM_ID                     (0x89000034u)

#define CSD_BUTTON0_SNS1_BSLN_EXT0_VALUE                    (CSD_dsRam.snsList.button0[1u].bslnExt[0u])
#define CSD_BUTTON0_SNS1_BSLN_EXT0_OFFSET                   (54u)
#define CSD_BUTTON0_SNS1_BSLN_EXT0_SIZE                     (1u)
#define CSD_BUTTON0_SNS1_BSLN_EXT0_PARAM_ID                 (0x4D000036u)

#define CSD_BUTTON0_SNS1_DIFF_VALUE                         (CSD_dsRam.snsList.button0[1u].diff)
#define CSD_BUTTON0_SNS1_DIFF_OFFSET                        (56u)
#define CSD_BUTTON0_SNS1_DIFF_SIZE                          (2u)
#define CSD_BUTTON0_SNS1_DIFF_PARAM_ID                      (0x8A000038u)

#define CSD_BUTTON0_SNS1_NEG_BSLN_RST_CNT0_VALUE            (CSD_dsRam.snsList.button0[1u].negBslnRstCnt[0u])
#define CSD_BUTTON0_SNS1_NEG_BSLN_RST_CNT0_OFFSET           (58u)
#define CSD_BUTTON0_SNS1_NEG_BSLN_RST_CNT0_SIZE             (1u)
#define CSD_BUTTON0_SNS1_NEG_BSLN_RST_CNT0_PARAM_ID         (0x4E00003Au)

#define CSD_BUTTON0_SNS1_IDAC_COMP0_VALUE                   (CSD_dsRam.snsList.button0[1u].idacComp[0u])
#define CSD_BUTTON0_SNS1_IDAC_COMP0_OFFSET                  (59u)
#define CSD_BUTTON0_SNS1_IDAC_COMP0_SIZE                    (1u)
#define CSD_BUTTON0_SNS1_IDAC_COMP0_PARAM_ID                (0x4800003Bu)

#define CSD_SNR_TEST_WIDGET_ID_VALUE                        (CSD_dsRam.snrTestWidgetId)
#define CSD_SNR_TEST_WIDGET_ID_OFFSET                       (60u)
#define CSD_SNR_TEST_WIDGET_ID_SIZE                         (1u)
#define CSD_SNR_TEST_WIDGET_ID_PARAM_ID                     (0x6800003Cu)

#define CSD_SNR_TEST_SENSOR_ID_VALUE                        (CSD_dsRam.snrTestSensorId)
#define CSD_SNR_TEST_SENSOR_ID_OFFSET                       (61u)
#define CSD_SNR_TEST_SENSOR_ID_SIZE                         (1u)
#define CSD_SNR_TEST_SENSOR_ID_PARAM_ID                     (0x6E00003Du)

#define CSD_SNR_TEST_SCAN_COUNTER_VALUE                     (CSD_dsRam.snrTestScanCounter)
#define CSD_SNR_TEST_SCAN_COUNTER_OFFSET                    (62u)
#define CSD_SNR_TEST_SCAN_COUNTER_SIZE                      (2u)
#define CSD_SNR_TEST_SCAN_COUNTER_PARAM_ID                  (0x8700003Eu)

#define CSD_SNR_TEST_RAW_COUNT0_VALUE                       (CSD_dsRam.snrTestRawCount[0u])
#define CSD_SNR_TEST_RAW_COUNT0_OFFSET                      (64u)
#define CSD_SNR_TEST_RAW_COUNT0_SIZE                        (2u)
#define CSD_SNR_TEST_RAW_COUNT0_PARAM_ID                    (0x8A000040u)


/*****************************************************************************/
/* Flash Data structure register definitions                                 */
/*****************************************************************************/
#define CSD_BUTTON0_PTR2SNS_FLASH_VALUE                     (CSD_dsFlash.wdgtArray[0].ptr2SnsFlash)
#define CSD_BUTTON0_PTR2SNS_FLASH_OFFSET                    (0u)
#define CSD_BUTTON0_PTR2SNS_FLASH_SIZE                      (4u)
#define CSD_BUTTON0_PTR2SNS_FLASH_PARAM_ID                  (0xD1000000u)

#define CSD_BUTTON0_PTR2WD_RAM_VALUE                        (CSD_dsFlash.wdgtArray[0].ptr2WdgtRam)
#define CSD_BUTTON0_PTR2WD_RAM_OFFSET                       (4u)
#define CSD_BUTTON0_PTR2WD_RAM_SIZE                         (4u)
#define CSD_BUTTON0_PTR2WD_RAM_PARAM_ID                     (0xD0000004u)

#define CSD_BUTTON0_PTR2SNS_RAM_VALUE                       (CSD_dsFlash.wdgtArray[0].ptr2SnsRam)
#define CSD_BUTTON0_PTR2SNS_RAM_OFFSET                      (8u)
#define CSD_BUTTON0_PTR2SNS_RAM_SIZE                        (4u)
#define CSD_BUTTON0_PTR2SNS_RAM_PARAM_ID                    (0xD3000008u)

#define CSD_BUTTON0_PTR2FLTR_HISTORY_VALUE                  (CSD_dsFlash.wdgtArray[0].ptr2FltrHistory)
#define CSD_BUTTON0_PTR2FLTR_HISTORY_OFFSET                 (12u)
#define CSD_BUTTON0_PTR2FLTR_HISTORY_SIZE                   (4u)
#define CSD_BUTTON0_PTR2FLTR_HISTORY_PARAM_ID               (0xD200000Cu)

#define CSD_BUTTON0_PTR2DEBOUNCE_VALUE                      (CSD_dsFlash.wdgtArray[0].ptr2DebounceArr)
#define CSD_BUTTON0_PTR2DEBOUNCE_OFFSET                     (16u)
#define CSD_BUTTON0_PTR2DEBOUNCE_SIZE                       (4u)
#define CSD_BUTTON0_PTR2DEBOUNCE_PARAM_ID                   (0xD4000010u)

#define CSD_BUTTON0_STATIC_CONFIG_VALUE                     (CSD_dsFlash.wdgtArray[0].staticConfig)
#define CSD_BUTTON0_STATIC_CONFIG_OFFSET                    (20u)
#define CSD_BUTTON0_STATIC_CONFIG_SIZE                      (2u)
#define CSD_BUTTON0_STATIC_CONFIG_PARAM_ID                  (0x9A000014u)

#define CSD_BUTTON0_TOTAL_NUM_SNS_VALUE                     (CSD_dsFlash.wdgtArray[0].totalNumSns)
#define CSD_BUTTON0_TOTAL_NUM_SNS_OFFSET                    (22u)
#define CSD_BUTTON0_TOTAL_NUM_SNS_SIZE                      (2u)
#define CSD_BUTTON0_TOTAL_NUM_SNS_PARAM_ID                  (0x96000016u)

#define CSD_BUTTON0_TYPE_VALUE                              (CSD_dsFlash.wdgtArray[0].wdgtType)
#define CSD_BUTTON0_TYPE_OFFSET                             (24u)
#define CSD_BUTTON0_TYPE_SIZE                               (1u)
#define CSD_BUTTON0_TYPE_PARAM_ID                           (0x51000018u)

#define CSD_BUTTON0_NUM_COLS_VALUE                          (CSD_dsFlash.wdgtArray[0].numCols)
#define CSD_BUTTON0_NUM_COLS_OFFSET                         (25u)
#define CSD_BUTTON0_NUM_COLS_SIZE                           (1u)
#define CSD_BUTTON0_NUM_COLS_PARAM_ID                       (0x57000019u)


#endif /* End CY_CAPSENSE_CSD_REGISTER_MAP_H */

/* [] END OF FILE */
