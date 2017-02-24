/* ========================================
 *
 * Copyright Cypress Semiconductor, 2017
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF Cypress Semiconductor.
 *
 * ========================================
*/
#include "project.h"
#include <stdbool.h>

#define PRESSED (0)
#define LEDON   (0)
#define LEDOFF  (1)
#define RW (4)

/* Global variables */
CY_PACKED struct {
    float  dacVal;
    uint8  ledVal;
    uint8  ledControl;
    uint8  buttonVal;
    float  temperature;
    float  humidity;
    float  alsVal;
    float  potVal;
} CY_PACKED_ATTR I2Cbuf;
    

CY_ISR(BL_ISR)
{
    /* If we get here it is time to launch the bootloader */
    BootloadTimer_ClearInterrupt(BootloadTimer_INTR_MASK_TC);
    Bootloadable_Load();
}

void processCapSense(void)
{
    static uint8 state = 0;
    
    if(!CapSense_IsBusy())
    {
        switch(state) {
            case 0: /* Scan Button 0 */
                CapSense_SetupWidget(CapSense_BUTTON0_WDGT_ID);
                CapSense_Scan();
                state++;
                break;
            case 1: /* Process Button 0, Scan Button 1 */
                CapSense_ProcessWidget(CapSense_BUTTON0_WDGT_ID);
                if(CapSense_IsWidgetActive(CapSense_BUTTON0_WDGT_ID))
                {
                    CBLED0_Write(LEDON);
                    I2Cbuf.buttonVal |= (0x01);
                }
                else
                {
                    CBLED0_Write(LEDOFF);
                    I2Cbuf.buttonVal &= (~0x01);
                }
                CapSense_SetupWidget(CapSense_BUTTON1_WDGT_ID);
                CapSense_Scan();
                state++;
                break;
            case 2: /* Process Button 1, Scan Button 2 */
                CapSense_ProcessWidget(CapSense_BUTTON1_WDGT_ID);
                if(CapSense_IsWidgetActive(CapSense_BUTTON1_WDGT_ID))
                {
                    CBLED1_Write(LEDON);
                    I2Cbuf.buttonVal |= (0x02);
                }
                else
                {
                    CBLED1_Write(LEDOFF);
                    I2Cbuf.buttonVal &= (~0x02);
                }
                CapSense_SetupWidget(CapSense_BUTTON2_WDGT_ID);
                CapSense_Scan();
                state++;
                break;
            case 3: /* Process Button 2, Scan Button 3 */
                CapSense_ProcessWidget(CapSense_BUTTON2_WDGT_ID);
                if(CapSense_IsWidgetActive(CapSense_BUTTON2_WDGT_ID))
                {
                    CBLED2_Write(LEDON);
                    I2Cbuf.buttonVal |= (0x04);
                }
                else
                {
                    CBLED2_Write(LEDOFF);
                    I2Cbuf.buttonVal &= (~0x04);
                }
                CapSense_SetupWidget(CapSense_BUTTON3_WDGT_ID);
                CapSense_Scan();
                state++;
                break;
            case 4: /* Process Button 3, Setup Proximity */
                CapSense_ProcessWidget(CapSense_BUTTON3_WDGT_ID);
                if(CapSense_IsWidgetActive(CapSense_BUTTON3_WDGT_ID))
                {
                    CBLED3_Write(LEDON);
                    I2Cbuf.buttonVal |= (0x08);
                }
                else
                {
                    CBLED3_Write(LEDOFF);
                    I2Cbuf.buttonVal &= (~0x08);
                }
                CapSense_SetupWidget(CapSense_PROXIMITY0_WDGT_ID);
                CapSense_Scan();
                state++;
                break;      
            case 5: /* Process Proximity, Setup Humidity */
                CapSense_ProcessWidget(CapSense_PROXIMITY0_WDGT_ID);
                if(CapSense_IsWidgetActive(CapSense_PROXIMITY0_WDGT_ID))
                {
                    I2Cbuf.buttonVal |= (0x10);
                }
                else
                {
                    I2Cbuf.buttonVal &= (~0x10);
                }
                CapSense_SetupWidget(CapSense_BUTTON0_WDGT_ID);
                CapSense_Scan();
                state=0;
                break;
        }
    }
}

int main(void)
{
    /* Local variables */
    bool BootloadCountFlag = false;
    int32 dacValPrev = 0;
    int32 dacVal;
    int32 dacCode;
    
    BL_INT_StartEx(BL_ISR);
    
    CyGlobalIntEnable; /* Enable global interrupts. */

    CapSense_Start();   
    EZI2C_Start();
    EZI2C_EzI2CSetBuffer1(sizeof(I2Cbuf), RW, (void *) &I2Cbuf);
    
    for(;;)
    {
        /* Look for bootloader entry - both mechanical buttons held down for 2 seconds */
        if((MB0_Read() == PRESSED) && (MB1_Read() == PRESSED))
        {
            BootloadCountFlag = true;
            BootloadTimer_Start();
        }
        else if(BootloadCountFlag == true)
        {
            BootloadCountFlag = false;
            BootloadTimer_Stop();
            BootloadTimer_WriteCounter(0);
        }
        
        /* CapSense Scanning */
        processCapSense();
        
        /* Set VDAC value*/
        dacVal = I2Cbuf.dacVal;
        if(dacValPrev != dacVal)
        {
            //Todo - convert float to int32
            // DAC range is 2.4V, Valid inputs are -4096 to 4094
            dacCode = (int32)(((dacVal * 8190)/2.4) - 4095);
            VDAC_SetValue(dacCode);
        }
        
        /* */
        
    }
}

/* [] END OF FILE */
