/***************************************************************************//**
* \file     ADC_INIT.c
* \version  1.20
*
* \brief
* Provides the initialization data structure for the ADC Component.
*
********************************************************************************
* \copyright
* (c) 2015-2016, Cypress Semiconductor Corporation. All rights reserved.
* This software, including source code, documentation and related
* materials ("Software"), is owned by Cypress Semiconductor
* Corporation ("Cypress") and is protected by and subject to worldwide
* patent protection (United States and foreign), United States copyright
* laws and international treaty provisions. Therefore, you may use this
* Software only as provided in the license agreement accompanying the
* software package from which you obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, nonexclusive,
* non-transferable license to copy, modify, and compile the
* Software source code solely for use in connection with Cypress's
* integrated circuit products. Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO
* WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING,
* BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
* PARTICULAR PURPOSE. Cypress reserves the right to make
* changes to the Software without notice. Cypress does not assume any
* liability arising out of the application or use of the Software or any
* product or circuit described in the Software. Cypress does not
* authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*******************************************************************************/
#include "ADC.h"

/* Array of configuration structures used to set configurations */
const ADC_CONFIG_STRUCT ADC_allConfigs[ADC_TOTAL_CONFIGS] = {
	/*Configuration 0*/
	{
		ADC_CFG0_CHANNEL_BASE,
		ADC_CFG0_CHANNEL_COUNT,
		ADC_CFG0_CTRL_INIT,
		ADC_CFG0_SAMPLE_CTRL_INIT,
		ADC_CFG0_SAMPLE_TIME01_INIT,
		ADC_CFG0_SAMPLE_TIME23_INIT,
		ADC_CFG0_RANGE_THRES_INIT,
		ADC_CFG0_RANGE_COND_INIT,
        ADC_CFG0_CHANNEL_EN,
		ADC_CFG0_RANGE_INTR_MASK,
		ADC_CFG0_SATURATE_INTR_MASK,
		ADC_CFG0_VREF_MV_VALUE,
		ADC_CFG0_MISC_CONFIG_INIT,
#if (ADC_CLOCK_INTERNAL)
		ADC_CFG0_CLOCKDIVIDER,
#endif /* ADC_CLOCK_INTERNAL */
#if(ADC_CFG0_FILTER_PRESENT == 1)	
		ADC_CFG0_FILTERCLOCKDIVIDER,
#endif
	}
	#if(ADC_TOTAL_CONFIGS > 1)
	/*Configuration 1*/
	,{
		ADC_CFG1_CHANNEL_BASE,
		ADC_CFG1_CHANNEL_COUNT,
		ADC_CFG1_CTRL_INIT,
		ADC_CFG1_SAMPLE_CTRL_INIT,
		ADC_CFG1_SAMPLE_TIME01_INIT,
		ADC_CFG1_SAMPLE_TIME23_INIT,
		ADC_CFG1_RANGE_THRES_INIT,
		ADC_CFG1_RANGE_COND_INIT,
   		ADC_CFG1_CHANNEL_EN,
		ADC_CFG1_RANGE_INTR_MASK,
		ADC_CFG1_SATURATE_INTR_MASK,
		ADC_CFG1_VREF_MV_VALUE,
		ADC_CFG1_MISC_CONFIG_INIT,
#if (ADC_CLOCK_INTERNAL)
		ADC_CFG1_CLOCKDIVIDER,
#endif /* ADC_CLOCK_INTERNAL */
#if(ADC_CFG1_FILTER_PRESENT == 1)	
		ADC_CFG1_FILTERCLOCKDIVIDER,
#endif
	}
	#endif
	#if(ADC_TOTAL_CONFIGS > 2)
	/*Configuration 2*/
	,{
		ADC_CFG2_CHANNEL_BASE,
		ADC_CFG2_CHANNEL_COUNT,
		ADC_CFG2_CTRL_INIT,
		ADC_CFG2_SAMPLE_CTRL_INIT,
		ADC_CFG2_SAMPLE_TIME01_INIT,
		ADC_CFG2_SAMPLE_TIME23_INIT,
		ADC_CFG2_RANGE_THRES_INIT,
		ADC_CFG2_RANGE_COND_INIT,
   		ADC_CFG2_CHANNEL_EN,
		ADC_CFG2_RANGE_INTR_MASK,
		ADC_CFG2_SATURATE_INTR_MASK,
		ADC_CFG2_VREF_MV_VALUE,
		ADC_CFG2_MISC_CONFIG_INIT,
#if (ADC_CLOCK_INTERNAL)
		ADC_CFG2_CLOCKDIVIDER,
#endif /* ADC_CLOCK_INTERNAL */
#if(ADC_CFG2_FILTER_PRESENT == 1)	
		ADC_CFG2_FILTERCLOCKDIVIDER,
#endif
	}
	#endif
	/*Configuration 3*/
	#if(ADC_TOTAL_CONFIGS > 3)
	,{
		ADC_CFG3_CHANNEL_BASE,
		ADC_CFG3_CHANNEL_COUNT,
        ADC_CFG3_CTRL_INIT,
		ADC_CFG3_SAMPLE_CTRL_INIT,
		ADC_CFG3_SAMPLE_TIME01_INIT,
		ADC_CFG3_SAMPLE_TIME23_INIT,
		ADC_CFG3_RANGE_THRES_INIT,
		ADC_CFG3_RANGE_COND_INIT,
        ADC_CFG3_CHANNEL_EN,
		ADC_CFG3_RANGE_INTR_MASK,
		ADC_CFG3_SATURATE_INTR_MASK,
		ADC_CFG3_VREF_MV_VALUE,
		ADC_CFG3_MISC_CONFIG_INIT,
#if (ADC_CLOCK_INTERNAL)
		ADC_CFG3_CLOCKDIVIDER,
#endif /* ADC_CLOCK_INTERNAL */
#if(ADC_CFG3_FILTER_PRESENT == 1)	
		ADC_CFG3_FILTERCLOCKDIVIDER,
#endif
	}
	#endif	
};

#if(ADC_TOTAL_CHANNELS_NUM > 1)
	/* Input Placement Array */
    const uint32 CYCODE ADC_InputsPlacement[ADC_TOTAL_CHANNELS_NUM] = 
    {
        (uint32)(ADC_cy_psoc4_sarmux_1__CH_0_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_0_PIN
        #ifdef ADC_cy_psoc4_sarmux_1__CH_0_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_0_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_0_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_1_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_1_PIN
        #ifdef ADC_cy_psoc4_sarmux_1__CH_1_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_1_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_1_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 2)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_2_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_2_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_2_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_2_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_2_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 3)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_3_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_3_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_3_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_3_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_3_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 4)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_4_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_4_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_4_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_4_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_4_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 5)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_5_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_5_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_5_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_5_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_5_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 6)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_6_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_6_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_6_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_6_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_6_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 7)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_7_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_7_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_7_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_7_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_7_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 8)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_8_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_8_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_8_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_8_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_8_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 9)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_9_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_9_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_9_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_9_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_9_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 10)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_10_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_10_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_10_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_10_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_10_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 11)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_11_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_11_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_11_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_11_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_11_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 12)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_12_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_12_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_12_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_12_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_12_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 13)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_13_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_13_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_13_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_13_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_13_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 14)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_14_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_14_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_14_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_14_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_14_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 15)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_15_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_15_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_15_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_15_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_15_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 16)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_16_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_16_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_16_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_16_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_16_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 17)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_17_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_17_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_17_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_17_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_17_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 18)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_18_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_18_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_18_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_18_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_18_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 19)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_19_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_19_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_19_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_19_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_19_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 20)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_20_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_20_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_20_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_20_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_20_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 21)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_21_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_21_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_21_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_21_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_21_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 22)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_22_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_22_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_22_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_22_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_22_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 23)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_23_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_23_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_23_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_23_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_23_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 24)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_24_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_24_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_24_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_24_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_24_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 25)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_25_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_25_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_25_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_25_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_25_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 26)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_26_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_26_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_26_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_26_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_26_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 27)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_27_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_27_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_27_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_27_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_27_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 28)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_28_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_28_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_28_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_28_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_28_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 29)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_29_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_29_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_29_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_29_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_29_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 30)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_30_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_30_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_30_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_30_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_30_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 31)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_31_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_31_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_31_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_31_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_31_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 32)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_32_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_32_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_32_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_32_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_32_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 33)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_33_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_33_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_33_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_33_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_33_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 34)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_34_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_34_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_34_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_34_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_34_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 35)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_35_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_35_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_35_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_35_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_35_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 36)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_36_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_36_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_36_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_36_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_36_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 37)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_37_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_37_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_37_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_37_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_37_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 38)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_38_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_38_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_38_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_38_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_38_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 39)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_39_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_39_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_39_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_39_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_39_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 40)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_40_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_40_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_40_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_40_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_40_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 41)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_41_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_41_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_41_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_41_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_41_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 42)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_42_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_42_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_42_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_42_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_42_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 43)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_43_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_43_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_43_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_43_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_43_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 44)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_44_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_44_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_44_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_44_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_44_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 45)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_45_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_45_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_45_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_45_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_45_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 46)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_46_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_46_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_46_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_46_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_46_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 47)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_47_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_47_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_47_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_47_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_47_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 48)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_48_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_48_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_48_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_48_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_48_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 49)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_49_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_49_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_49_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_49_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_49_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 50)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_50_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_50_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_50_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_50_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_50_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 51)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_51_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_51_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_51_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_51_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_51_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 52)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_52_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_52_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_52_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_52_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_52_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 53)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_53_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_53_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_53_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_53_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_53_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 54)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_54_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_54_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_54_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_54_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_54_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 55)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_55_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_55_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_55_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_55_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_55_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 56)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_56_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_56_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_56_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_56_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_56_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 57)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_57_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_57_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_57_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_57_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_57_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 58)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_58_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_58_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_58_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_58_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_58_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 59)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_59_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_59_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_59_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_59_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_59_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 60)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_60_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_60_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_60_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_60_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_60_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 61)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_61_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_61_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_61_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_61_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_61_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 62)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_62_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_62_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_62_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_62_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_62_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
        #if(ADC_TOTAL_CHANNELS_NUM > 63)
        ,(uint32)(ADC_cy_psoc4_sarmux_1__CH_63_PORT << 4u) |
        (uint32) ADC_cy_psoc4_sarmux_1__CH_63_PIN 
        #ifdef ADC_cy_psoc4_sarmux_1__CH_63_MINUS_PIN
            | (uint32) (ADC_cy_psoc4_sarmux_1__CH_63_MINUS_PIN << 16u) |
            (uint32)(ADC_cy_psoc4_sarmux_1__CH_63_MINUS_PORT << 20u) |
            ADC_CHANNEL_CONFIG_NEG_ADDR_EN_MSK
        #else
            | (uint32)(ADC_NEG_PORT_ADDR_CTB3 << 20u)
        #endif
        #endif
    };
#endif
	/* End of Input Placement Array */
	
/* [] END OF FILE */
