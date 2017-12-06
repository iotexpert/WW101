/*
 * Copyright 2017, Cypress Semiconductor Corporation or a subsidiary of 
 * Cypress Semiconductor Corporation. All Rights Reserved.
 * 
 * This software, associated documentation and materials ("Software"),
 * is owned by Cypress Semiconductor Corporation
 * or one of its subsidiaries ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products. Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */

/** @file
 * Defines peripherals available for use on NEB1DX_01 board
 */
#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

/*
PSoC AFE Shield Resources:
+-----------------------------------------------------------------------------------------------+
| WICED_NAME    | WICED Pin Name | Arduino Header | Shield Connection     | Alternate Functions |
+---------------+----------------+----------------+-----------------------+---------------------+
|WICED_BUTTON1  | WICED_GPIO_40  | D4             | MB1                   | WICED_PWM_5         |
+---------------+----------------+----------------+-----------------------+---------------------+
|WICED_BUTTON2  | WICED_GPIO_23  | D12            | MB2                   | N/A                 |
+---------------+----------------+----------------+-----------------------+---------------------+
|WICED_LED1     | WICED_GPIO_41  | D5             | LED1                  | WICED_PWM_6         |
+---------------+----------------+----------------+-----------------------+---------------------+
|WICED_LED2     | WICED_GPIO_24  | D11            | LED2                  | N/A                 |
+---------------+----------------+----------------+-----------------------+---------------------+
|WICED_ADC_1    | WICED_GPIO_47  | A0             | Ambient Light Sensor  | N/A                 |
+---------------+----------------+----------------+-----------------------+---------------------+
|WICED_ADC_2    | WICED_GPIO_48  | A1             | DAC                   | N/A                 |
+---------------+----------------+----------------+-----------------------+---------------------+
|WICED_ADC_3    | WICED_GPIO_49  | A2             | POT                   | N/A                 |
+---------------+----------------+----------------+-----------------------+---------------------+

I2C Slave Information:
+--------------------------+-------------+-------------------+
| Peripheral               | I2C Port    | I2C Slave Address |
+--------------------------+-------------+-------------------+
| PSoC Analog Co-Processor | WICED_I2C_1 | 0x42              |
+--------------------------+-------------+-------------------+
| U8G OLED Display         | WICED_I2C_1 | 0x3C              |
+--------------------------+-------------+-------------------+

I2C Register Map for PSoC Analog Co-Processor:
+----------------+-------------+----------------------------------------------+
| Offset Address | Size(Bytes) | Function                                     |
+----------------+-------------+----------------------------------------------+
| 0x00           | 4           | Desired DAC Voltage (V)                      |
+----------------+-------------+----------------------------------------------+
| 0x04           | 1           | Desired CapSense LED Values                  |
+----------------+-------------+----------------------------------------------+
| 0x05           | 1           | LED Control                                  |
+----------------+-------------+----------------------------------------------+
| 0x06           | 1           | Button State                                 |
+----------------+-------------+----------------------------------------------+
| 0x07           | 4           | Temperature (C)                              |
+----------------+-------------+----------------------------------------------+
| 0x0B           | 4           | Humidity (%)                                 |
+----------------+-------------+----------------------------------------------+
| 0x0F           | 4           | Ambient Light (LUX)                          |
+----------------+-------------+----------------------------------------------+
| 0x13           | 4           | Potentiometer Voltage (V)                    |
+----------------+-------------+----------------------------------------------+

NEB1DX_01 platform pin definitions ...
+---------------------------------------------------------------------------------------------------------+
| Enum ID       |Pin |   Pin Name on    |    Module     | STM32| Peripheral  |    Board     | Peripheral  |
|               | #  |      Module      |  GPIO Alias   | Port | Available   |  Connection  |   Alias     |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_1  | 34 |                  | WICED_GPIO_1  | A  0 | GPIO        |              | WICED_PWM_8 |
|               |    |                  |               |      | ADC123_IN0  |              |             |
|               |    |                  |               |      | TIM2_CH1    |              |             |
|               |    |                  |               |      | TIM5_CH1    |              |             |
|               |    |                  |               |      | USART2_CTS  |              |             |
|               |    |                  |               |      | USART4_TX   |              |             |
|               |    |                  |               |      | ETH_MII_CRS |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_2  | 35 |                  | WICED_GPIO_2  | A  1 | GPIO        |              | WICED_PWM_9 |
|               |    |                  |               |      | ADC123_IN1  |              |             |
|               |    |                  |               |      | TIM2_CH2    |              |             |
|               |    |                  |               |      | TIM5_CH2    |              |             |
|               |    |                  |               |      | USART2_RTS  |              |             |
|               |    |                  |               |      | USART4_RX   |              |             |
|               |    |                  |               |      |ETH_MII_RX_CLK|             |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_3  | 36 |                  | WICED_GPIO_3  | A  2 | GPIO        | MIKRO_PWM    |             |
|               |    |                  |               |      | ADC123_IN2  |              |             |
|               |    |                  |               |      | TIM2_CH3    |              |             |
|               |    |                  |               |      | TIM5_CH3    |              |             |
|               |    |                  |               |      | TIM9_CH1    |              |             |
|               |    |                  |               |      | USART2_TX   |              |             |
|               |    |                  |               |      | ETH_MDIO    |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_4  | 37 |                  | WICED_GPIO_4  | A  3 | ADC123_IN3  | MIKRO_AD     | WICED_ADC_8 |
|               |    |                  |               |      | GPIO        |              |             |
|               |    |                  |               |      | TIM2_CH4    |              |             |
|               |    |                  |               |      | TIM5_CH4    |              |             |
|               |    |                  |               |      | TIM9_CH2    |              |             |
|               |    |                  |               |      | USART2_RX   |              |             |
|               |    |                  |               |      | ETH_MII_COL |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_5  | 40 |                  | WICED_GPIO_5  | A  4 | ADC12_IN4   |              | WICED_SPI_1 |
|               |    |                  |               |      | DAC1_OUT    |              |             |
|               |    |                  |               |      | GPIO        |              |             |
|               |    |                  |               |      | I2S3_WS     |              |             |
|               |    |                  |               |      | SPI1_NSS    |              |             |
|               |    |                  |               |      | SPI3_NSS    |              |             |
|               |    |                  |               |      | USART2_CK   |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_6  | 41 |                  | WICED_GPIO_6  | A  5 | ADC12_IN5   |              | WICED_SPI_1 |
|               |    |                  |               |      | DAC2_OUT    |              |             |
|               |    |                  |               |      | GPIO        |              |             |
|               |    |                  |               |      | SPI1_SCK    |              |             |
|               |    |                  |               |      | TIM2_CH1_ETR|              |             |
|               |    |                  |               |      | TIM8_CH1N   |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_7  | 42 |                  | WICED_GPIO_7  | A  6 | ADC12_IN6   |              | WICED_SPI_1 |
|               |    |                  |               |      | GPIO        |              |             |
|               |    |                  |               |      | SPI1_MISO   |              |             |
|               |    |                  |               |      | TIM1_BKIN   |              |             |
|               |    |                  |               |      | TIM3_CH1    |              |             |
|               |    |                  |               |      | TIM8_BKIN   |              |             |
|               |    |                  |               |      | TIM13_CH1   |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_8  | 43 |                  | WICED_GPIO_8  | A  7 | ADC12_IN7   |              | WICED_SPI_1 |
|               |    |                  |               |      | GPIO        |              |             |
|               |    |                  |               |      | SPI1_MOSI   |              |             |
|               |    |                  |               |      | TIM1_CH1N   |              |             |
|               |    |                  |               |      | TIM3_CH2    |              |             |
|               |    |                  |               |      | TIM8_CH1N   |              |             |
|               |    |                  |               |      | TIM14_CH1   |              |             |
|               |    |                  |               |      |ETH_MII_RX_DV|              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_9  | 101|                  | WICED_GPIO_9  | A  9 | GPIO        | USB_UART_RX  | WICED_UART_1|
|               |    |                  |               |      | I2C3_SMBA   |              |             |
|               |    |                  |               |      | TIM1_CH2    |              |             |
|               |    |                  |               |      | USART1_TX   |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_10 | 102|                  | WICED_GPIO_10 | A 10 | GPIO        | USB_UART_TX  | WICED_UART_1|
|               |    |                  |               |      | TIM1_CH3    |              |             |
|               |    |                  |               |      | USART1_RX   |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_11 | 103|                  | WICED_GPIO_11 | A 11 | GPIO        |USB_UART_RTS_N| WICED_UART_1|
|               |    |                  |               |      | TIM1_CH4    |              |             |
|               |    |                  |               |      | USART1_CTS  |              |             |
|               |    |                  |               |      | CAN1_RX     |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_12 | 104|                  | WICED_GPIO_12 | A 12 | GPIO        |USB_UART_CTS_N| WICED_UART_1|
|               |    |                  |               |      | TIM1_ETR    |              |             |
|               |    |                  |               |      | USART1_RTS  |              |             |
|               |    |                  |               |      | CAN1_TX     |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_13 | 46 |                  | WICED_GPIO_13 | B  0 | GPIO        | LED2_RED     | WICED_PWM_2 |
|               |    |                  |               |      | ADC12_IN8   |              |             |
|               |    |                  |               |      | TIM1_CH2N   |              |             |
|               |    |                  |               |      | TIM3_CH3    |              |             |
|               |    |                  |               |      | TIM8_CH2N   |              |             |
|               |    |                  |               |      | ETH_MII_RXD2|              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_14 | 47 |                  | WICED_GPIO_14 | B  1 | GPIO        | LED2_GREEN   | WICED_PWM_3 |
|               |    |                  |               |      | ADC12_IN9   |              |             |
|               |    |                  |               |      | TIM1_CH3N   |              |             |
|               |    |                  |               |      | TIM3_CH4    |              |             |
|               |    |                  |               |      | TIM8_CH3N   |              |             |
|               |    |                  |               |      | ETH_MII_RXD3|              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_15 | 48 |                  | WICED_GPIO_15 | B  2 | GPIO        | LED1_RED     |             |
|               |    |                  |               |      |             |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_16 | 136|                  | WICED_GPIO_16 | B  6 | GPIO        | ARD_SCL      | WICED_I2C_1 |
|               |    |                  |               |      | TIM4_CH1    |              |             |
|               |    |                  |               |      | I2C1_SCL    |              |             |
|               |    |                  |               |      | USART1_TX   |              |             |
|               |    |                  |               |      | CAN2_TX     |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_17 | 137|                  | WICED_GPIO_17 | B  7 | GPIO        | ARD_SDA      | WICED_I2C_1 |
|               |    |                  |               |      | TIM4_CH2    |              |             |
|               |    |                  |               |      | I2C1_SDA    |              |             |
|               |    |                  |               |      | USART1_RX   |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_18 | 73 |                  | WICED_GPIO_18 | B 12 | GPIO        | ARD_IO10     | WICED_SPI_2 |
|               |    |                  |               |      | TIM1_BKIN   |              |             |
|               |    |                  |               |      | I2C2_SMBA   |              |             |
|               |    |                  |               |      | SPI2_NSS    |              |             |
|               |    |                  |               |      | I2S2_WS     |              |             |
|               |    |                  |               |      | USART3_CK   |              |             |
|               |    |                  |               |      | CAN2_RX     |              |             |
|               |    |                  |               |      | ETH_MII_TXD0|              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_19 | 74 |                  | WICED_GPIO_19 | B 13 | GPIO        | ARD_IO13     | WICED_SPI_2 |
|               |    |                  |               |      | TIM1_CH1N   |              |             |
|               |    |                  |               |      | SPI2_SCK    |              |             |
|               |    |                  |               |      | I2S2_CK     |              |             |
|               |    |                  |               |      | USART3_CTS  |              |             |
|               |    |                  |               |      | CAN2_TX     |              |             |
|               |    |                  |               |      | ETH_MII_TXD1|              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_20 | 75 |                  | WICED_GPIO_20 | B 14 | GPIO        | USB_MCU_M    |             |
|               |    |                  |               |      | TIM1_CH2N   |              |             |
|               |    |                  |               |      | TIM8_CH2N   |              |             |
|               |    |                  |               |      | TIM12_CH1   |              |             |
|               |    |                  |               |      | SPI2_MISO   |              |             |
|               |    |                  |               |      | I2S2ext_SD  |              |             |
|               |    |                  |               |      | USART3_RTS  |              |             |
|               |    |                  |               |      | OTG_HS_DM   |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_21 | 76 |                  | WICED_GPIO_21 | B 15 | GPIO        | USB_MCU_P    |             |
|               |    |                  |               |      | TIM1_CH3N   |              |             |
|               |    |                  |               |      | TIM8_CH3N   |              |             |
|               |    |                  |               |      | TIM12_CH2   |              |             |
|               |    |                  |               |      | SPI2_MOSI   |              |             |
|               |    |                  |               |      | I2S2_SD     |              |             |
|               |    |                  |               |      | RTC_REFIN   |              |             |
|               |    |                  |               |      | OTG_HS_DP   |              |             |
|---------------+----+------------------+----------------------+-------------+--------------+-------------|
| WICED_GPIO_22 | 26 |                  | WICED_GPIO_22 | C  0 | GPIO        | MIKRO_INT    |             |
|               |    |                  |               |      | ADC123_IN10 |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_23 | 28 |                  | WICED_GPIO_23 | C  2 | GPIO        | ARD_IO12     | WICED_SPI_2 |
|               |    |                  |               |      | SPI2_MISO   |              |             |
|               |    |                  |               |      | I2S2ext_SD  |              |             |
|               |    |                  |               |      | ETH_MII_TXD2|              |             |
|---------------+----+------------------+----------------------+-------------+--------------+-------------|
| WICED_GPIO_24 | 29 |                  | WICED_GPIO_24 | C  3 | GPIO        | ARD_IO11     | WICED_SPI_2 |
|               |    |                  |               |      | SPI2_MOSI   |              |             |
|               |    |                  |               |      | I2S2_SD     |              |             |
|               |    |                  |               |      |ETH_MII_TX_CLK|             |             |
|---------------+----+------------------+----------------------+-------------+--------------+-------------|
| WICED_GPIO_25 | 114|                  | WICED_GPIO_25 | D  0 | GPIO        |              |             |
|               |    |                  |               |      | CAN1_RX     |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_26 | 115|                  | WICED_GPIO_26 | D  1 | GPIO        |              |             |
|               |    |                  |               |      | CAN1_TX     |              |             |
|---------------+----+------------------+----------------------+-------------+--------------+-------------|
| WICED_GPIO_27 | 116|                  | WICED_GPIO_27 | D  7 | GPIO        | ARD_RESET    |             |
|               |    |                  |               |      | USART2_CK   |              |             |
|---------------+----+------------------+----------------------+-------------+--------------+-------------+
| WICED_GPIO_28 | 77 |                  | WICED_GPIO_28 | D  8 | GPIO        | ARD_IO1      | WICED_UART_2|
|               |    |                  |               |      | USART3_TX   |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_29 | 78 |                  | WICED_GPIO_29 | D  9 | GPIO        | ARD_IO0      | WICED_UART_2|
|               |    |                  |               |      | USART3_RX   |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_30 | 80 |                  | WICED_GPIO_30 | D 11 | GPIO        | ARD_IO2      |             |
|               |    |                  |               |      | USART3_CTS  |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_31 | 81 |                  | WICED_GPIO_31 | D 12 | GPIO        | ARD_IO3      | WICED_PWM_4 |
|               |    |                  |               |      | TIM4_CH2    |              |             |
|               |    |                  |               |      | USART3_RTS  |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_32 | 141|                  | WICED_GPIO_32 | E  0 | GPIO        | ARD_IO9      |             |
|               |    |                  |               |      | TIM4_ETR    |              |             |
|               |    |                  |               |      | UART8_RX    |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_33 | 142|                  | WICED_GPIO_33 | E  1 | GPIO        | ARD_IO8      |             |
|               |    |                  |               |      | UART8_TX    |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_34 |  1 |                  | WICED_GPIO_34 | E  2 | GPIO        | MIKRO_SCK    | WICED_SPI_3 |
|               |    |                  |               |      | TRACECLK    |              |             |
|               |    |                  |               |      | SPI4_SCK    |              |             |
|               |    |                  |               |      | SAI1_MCLK_A |              |             |
|               |    |                  |               |      | ETH_MII_TXD3|              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_35 |  3 |                  | WICED_GPIO_35 | E  4 | GPIO        | MIKRO_CS     | WICED_SPI_3 |
|               |    |                  |               |      | TRACED1     |              |             |
|               |    |                  |               |      | SAI1_FS_A   |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_36 |  4 |                  | WICED_GPIO_36 | E  5 | GPIO        | MIKRO_MISO   | WICED_SPI_3 |
|               |    |                  |               |      | TRACED2     |              |             |
|               |    |                  |               |      | TIM9_CH1    |              |             |
|               |    |                  |               |      | SPI4_MISO   |              |             |
|               |    |                  |               |      | SAI1_CLK_A  |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_37 |  5 |                  | WICED_GPIO_37 | E  6 | GPIO        | MIKRO_MOSI   | WICED_SPI_3 |
|               |    |                  |               |      | TRACED3     |              |             |
|               |    |                  |               |      | TIM9_CH2    |              |             |
|               |    |                  |               |      | SPI4_MOSI   |              |             |
|               |    |                  |               |      | SAI1_SCK_A  |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_38 | 58 |                  | WICED_GPIO_38 | E  7 | GPIO        | MIKRO_TX     | WICED_UART_3|
|               |    |                  |               |      | TIM1_ETR    |              |             |
|               |    |                  |               |      | UART7_RX    |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_39 | 59 |                  | WICED_GPIO_39 | E  8 | GPIO        | MIKRO_RX     | WICED_UART_3|
|               |    |                  |               |      | TIM1_CH1N   |              |             |
|               |    |                  |               |      | UART7_TX    |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_40 | 63 |                  | WICED_GPIO_40 | E 10 | GPIO        | ARD_IO4      | WICED_PWM_5 |
|               |    |                  |               |      | TIM1_CH2N   |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_41 | 64 |                  | WICED_GPIO_41 | E 11 | GPIO        | ARD_IO5      | WICED_PWM_6 |
|               |    |                  |               |      | TIM1_CH2    |              |             |
|               |    |                  |               |      | SPI4_NSS    |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_42 | 65 |                  | WICED_GPIO_42 | E 12 | GPIO        | ARD_IO6      | WICED_PWM_7 |
|               |    |                  |               |      | TIM1_CH3N   |              |             |
|               |    |                  |               |      | SPI4_SCK    |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_43 | 75 |                  | WICED_GPIO_43 | E 13 | GPIO        | ARD_IO7      |             |
|               |    |                  |               |      | TIM1_CH3    |              |             |
|               |    |                  |               |      | SPI4_MISO   |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_44 | 10 |                  | WICED_GPIO_44 | F  0 | GPIO        | MIKRO_SDA    | WICED_I2C_2 |
|               |    |                  |               |      | I2C2_SDA    |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_45 | 11 |                  | WICED_GPIO_45 | F  1 | GPIO        | MIKRO_SCL    | WICED_I2C_2 |
|               |    |                  |               |      | I2C2_SCL    |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_46 | 12 |                  | WICED_GPIO_46 | F  2 | GPIO        | MIKRO_RST    |             |
|               |    |                  |               |      | I2C2_SMBA   |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_47 | 13 |                  | WICED_GPIO_47 | F  3 | GPIO        | AD_CH0       | WICED_ADC_1 |
|               |    |                  |               |      | ADC3_IN9    |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_48 | 14 |                  | WICED_GPIO_48 | F  4 | GPIO        | AD_CH1       | WICED_ADC_2 |
|               |    |                  |               |      | ADC3_IN14   |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_49 | 15 |                  | WICED_GPIO_49 | F  5 | GPIO        | AD_CH2       | WICED_ADC_3 |
|               |    |                  |               |      | ADC3_IN15   |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_50 | 19 |                  | WICED_GPIO_50 | F  7 | GPIO        | AD_CH3       | WICED_ADC_4 |
|               |    |                  |               |      | ADC3_IN5    |              |             |
|               |    |                  |               |      | TIM11_CH1   |              |             |
|               |    |                  |               |      | SPI5_SCK    |              |             |
|               |    |                  |               |      | SAI1_MCLK_B |              |             |
|               |    |                  |               |      | UART7_TX    |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_51 | 20 |                  | WICED_GPIO_51 | F  8 | GPIO        | AD_CH4       | WICED_ADC_5 |
|               |    |                  |               |      | ADC3_IN6    |              |             |
|               |    |                  |               |      | TIM13_CH1   |              |             |
|               |    |                  |               |      | SPI5_MISO   |              |             |
|               |    |                  |               |      | SAI1_SCK_B  |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_52 | 21 |                  | WICED_GPIO_52 | F  9 | GPIO        | AD_CH5       | WICED_ADC_6 |
|               |    |                  |               |      | ADC3_IN7    |              |             |
|               |    |                  |               |      | TIM14_CH1   |              |             |
|               |    |                  |               |      | SPI5_MOSI   |              |             |
|               |    |                  |               |      | SAI1_FS_B   |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_53 | 22 |                  | WICED_GPIO_53 | F 10 | GPIO        | TEMP_ADC     | WICED_ADC_7 |
|               |    |                  |               |      | ADC3_IN8    |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_54 | 49 |                  | WICED_GPIO_54 | F 11 | GPIO        | LED1_GREEN   |             |
|               |    |                  |               |      | SPI5_MOSI   |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_55 | 91 |                  | WICED_GPIO_55 | G  6 | GPIO        | USER_BUTTON1 |             |
|               |    |                  |               |      |             |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_56 | 92 |                  | WICED_GPIO_56 | G  7 | GPIO        | USER_BUTTON1 |             |
|               |    |                  |               |      | USART6_CK   |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_57 | 93 |                  | WICED_GPIO_57 | G  8 | GPIO        |              |             |
|               |    |                  |               |      | SPI6_NSS    |              |             |
|               |    |                  |               |      | USART6_RTS  |              |             |
|               |    |                  |               |      | ETH_PPS_OUT |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_58 | 127|                  | WICED_GPIO_58 | G 12 | GPIO        |              | WICED_SPI_4 |
|               |    |                  |               |      | SPI6_MISO   |              |             |
|               |    |                  |               |      | USART6_RTS  |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_59 | 128|                  | WICED_GPIO_59 | G 13 | GPIO        |              | WICED_SPI_4 |
|               |    |                  |               |      | SPI6_SCK    |              |             |
|               |    |                  |               |      | USART6_CTS  |              |             |
|               |    |                  |               |      | ETH_MII_TXD0|              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_60 | 129|                  | WICED_GPIO_60 | G 14 | GPIO        |              | WICED_SPI_4 |
|               |    |                  |               |      | SPI6_MOSI   |              |             |
|               |    |                  |               |      | USART6_TX   |              |             |
|               |    |                  |               |      | ETH_MII_TXD1|              |             |
+---------------+----+------------------+---------------+------+-------------+--------------+-------------+

*/

/******************************************************
 *                   Enumerations
 ******************************************************/

typedef enum
{
    WICED_GPIO_1,
    WICED_GPIO_2,
    WICED_GPIO_3,
    WICED_GPIO_4,
    WICED_GPIO_5,
    WICED_GPIO_6,
    WICED_GPIO_7,
    WICED_GPIO_8,
    WICED_GPIO_9,
    WICED_GPIO_10,
    WICED_GPIO_11,
    WICED_GPIO_12,
    WICED_GPIO_13,
    WICED_GPIO_14,
    WICED_GPIO_15,
    WICED_GPIO_16,
    WICED_GPIO_17,
    WICED_GPIO_18,
    WICED_GPIO_19,
    WICED_GPIO_20,
    WICED_GPIO_21,
    WICED_GPIO_22,
    WICED_GPIO_23,
    WICED_GPIO_24,
    WICED_GPIO_25,
    WICED_GPIO_26,
    WICED_GPIO_27,
    WICED_GPIO_28,
    WICED_GPIO_29,
    WICED_GPIO_30,
    WICED_GPIO_31,
    WICED_GPIO_32,
    WICED_GPIO_33,
    WICED_GPIO_34,
    WICED_GPIO_35,
    WICED_GPIO_36,
    WICED_GPIO_37,
    WICED_GPIO_38,
    WICED_GPIO_39,
    WICED_GPIO_40,
    WICED_GPIO_41,
    WICED_GPIO_42,
    WICED_GPIO_43,
    WICED_GPIO_44,
    WICED_GPIO_45,
    WICED_GPIO_46,
    WICED_GPIO_47,
    WICED_GPIO_48,
    WICED_GPIO_49,
    WICED_GPIO_50,
    WICED_GPIO_51,
    WICED_GPIO_52,
    WICED_GPIO_53,
    WICED_GPIO_54,
    WICED_GPIO_55,
    WICED_GPIO_56,
    WICED_GPIO_57,
    WICED_GPIO_58,
    WICED_GPIO_59,
    WICED_GPIO_60,
    WICED_GPIO_MAX, /* Denotes the total number of GPIO port aliases. Not a valid GPIO alias */
    WICED_GPIO_32BIT = 0x7FFFFFFF,
} wiced_gpio_t;

typedef enum
{
    WICED_SPI_1,
    WICED_SPI_2,
    WICED_SPI_3,
    WICED_SPI_4,
    WICED_SPI_MAX, /* Denotes the total number of SPI port aliases. Not a valid SPI alias */
    WICED_SPI_32BIT = 0x7FFFFFFF,
} wiced_spi_t;

typedef enum
{
    WICED_I2C_1,
    WICED_I2C_2,
    WICED_I2C_MAX,
    WICED_I2C_32BIT = 0x7FFFFFFF,
} wiced_i2c_t;

typedef enum
{
    WICED_I2S_1,
    WICED_I2S_MAX, /* Denotes the total number of I2S port aliases.  Not a valid I2S alias */
    WICED_I2S_32BIT = 0x7FFFFFFF
} wiced_i2s_t;

typedef enum
{
    WICED_PWM_1,
    WICED_PWM_2,
    WICED_PWM_3,
    WICED_PWM_4,
    WICED_PWM_5,
    WICED_PWM_6,
    WICED_PWM_7,
    WICED_PWM_8,
    WICED_PWM_9,
    WICED_PWM_MAX, /* Denotes the total number of PWM port aliases. Not a valid PWM alias */
    WICED_PWM_32BIT = 0x7FFFFFFF,
} wiced_pwm_t;

typedef enum
{
    WICED_ADC_1,
    WICED_ADC_2,
    WICED_ADC_3,
    WICED_ADC_4,
    WICED_ADC_5,
    WICED_ADC_6,
    WICED_ADC_7,
    WICED_ADC_8,
    WICED_ADC_MAX, /* Denotes the total number of ADC port aliases. Not a valid ADC alias */
    WICED_ADC_32BIT = 0x7FFFFFFF,
} wiced_adc_t;

typedef enum
{
    WICED_UART_1,
    WICED_UART_2,
    WICED_UART_3,
    WICED_UART_4,
    WICED_UART_MAX, /* Denotes the total number of UART port aliases. Not a valid UART alias */
    WICED_UART_32BIT = 0x7FFFFFFF,
} wiced_uart_t;

/* Logical Button-ids which map to physical buttons on the board */
typedef enum
{
    PLATFORM_BUTTON_1,
    PLATFORM_BUTTON_2,
    PLATFORM_BUTTON_MAX, /* Denotes the total number of Buttons on the board. Not a valid Button Alias */
} platform_button_t;

/* Logical LED-ids which map to physical GPIO LEDs on the board (see platform_gpio_leds[] in platform.c)*/
typedef enum
{
    PLATFORM_LED_1,
    PLATFORM_LED_2,
    PLATFORM_LED_3,
    PLATFORM_LED_4,
    PLATFORM_LED_MAX, /* Denotes the total number of LEDs on the board. Not a valid alias */
} platform_led_t;
/******************************************************
 *                    Constants
 ******************************************************/

#define WICED_PLATFORM_BUTTON_COUNT  ( 2 )

/** Arduino connector I2C */
#define PLATFORM_ARDUINO_I2C    ( WICED_I2C_1 )
/** Arduino connector SPI */
#define PLATFORM_ARDUINO_SPI    ( WICED_SPI_2 )
/** Arduino connector SPI CS */
#define PLATFORM_ARDUINO_SPI_CS ( WICED_GPIO_18 )
/** Arduino connector UART */
#define PLATFORM_ARDUINO_UART   ( WICED_UART_2 )
/** Arduino connector RST */
#define PLATFORM_ARDUINO_RST    ( WICED_GPIO_27 )

/** mikroBus connector I2C */
#define PLATFORM_MIKRO_I2C    ( WICED_I2C_2 )
/** mikroBus connector SPI */
#define PLATFORM_MIKRO_SPI    ( WICED_SPI_3 )
/** mikroBus connector SPI CS */
#define PLATFORM_MIKRO_SPI_CS ( WICED_GPIO_35 )
/** mikroBus connector UART */
#define PLATFORM_MIKRO_UART   ( WICED_UART_3 )
/** mikroBus connector ADC */
#define PLATFORM_MIKRO_ADC    ( WICED_ADC_8 )
/** mikroBus connector INT */
#define PLATFORM_MIKRO_INT    ( WICED_GPIO_22 )
/** mikroBus connector RST */
#define PLATFORM_MIKRO_RST    ( WICED_GPIO_46 )
/** mikroBus connector PWM */
#define PLATFORM_MIKRO_PWM    ( WICED_PWM_1 )

/** PMOD connector SPI */
#define PLATFORM_PMOD_SPI    ( WICED_SPI_4 )
/** PMOD connector SPI CS */
#define PLATFORM_PMOD_SPI_CS ( WICED_GPIO_57 )
/** PMOD connector INT */
#define PLATFORM_PMOD_INT    ( WICED_GPIO_1 )
/** PMOD connector RST */
#define PLATFORM_PMOD_RST    ( WICED_GPIO_2 )

/** UART port used for standard I/O */
#define STDIO_UART ( WICED_UART_1 )

/* SPI flash is present */
#define WICED_PLATFORM_INCLUDES_SPI_FLASH
#define WICED_SPI_FLASH_CS ( WICED_GPIO_5 )

/* Components connected to external I/Os */
/* BUTTON1, BUTTON2, LED1 and LED2 are mappedto the shield board instead of the base board */
#define PLATFORM_LED_COUNT  ( 4 )
#define WICED_LED1          ( WICED_GPIO_41 )
#define WICED_LED2          ( WICED_GPIO_24 )
#define WICED_LED3          ( WICED_GPIO_13 )
#define WICED_LED4          ( WICED_GPIO_14 )
#define WICED_LED1_ON_STATE ( WICED_ACTIVE_HIGH )
#define WICED_LED2_ON_STATE ( WICED_ACTIVE_HIGH )
#define WICED_LED3_ON_STATE ( WICED_ACTIVE_HIGH )
#define WICED_LED4_ON_STATE ( WICED_ACTIVE_HIGH )
#define WICED_BUTTON1       ( WICED_GPIO_40 )
#define WICED_BUTTON2       ( WICED_GPIO_23 )
#define WICED_THERMISTOR    ( WICED_GPIO_53 )

/* I/O connection <-> Peripheral Connections */
#define WICED_LED3_JOINS_PWM        ( WICED_PWM_2 )
#define WICED_LED4_JOINS_PWM        ( WICED_PWM_3 )
/* Redirect WICED_LED1_JOINS_PWM to WICED_LED3_JOINS_PWM and WICED_LED2_JOINS_PWM to WICED_LED4_JOINS_PWM
 * to keep with WICED convention.
 */
#define WICED_LED1_JOINS_PWM        ( WICED_LED3_JOINS_PWM )
#define WICED_LED2_JOINS_PWM        ( WICED_LED4_JOINS_PWM )
#define WICED_THERMISTOR_JOINS_ADC  ( WICED_ADC_7 )

/* Bootloader OTA and OTA2 factory reset during settings */
#define PLATFORM_FACTORY_RESET_BUTTON_INDEX     ( PLATFORM_BUTTON_1 )
#define PLATFORM_FACTORY_RESET_TIMEOUT          ( 10000 )

/* Generic button checking defines */
#define PLATFORM_BUTTON_PRESS_CHECK_PERIOD      ( 100 )
#define PLATFORM_BUTTON_PRESSED_STATE           (   0 )

#define PLATFORM_GREEN_LED_INDEX                ( WICED_LED_INDEX_2 )
#define PLATFORM_RED_LED_INDEX                  ( WICED_LED_INDEX_1 )


#ifdef __cplusplus
} /*extern "C" */
#endif
