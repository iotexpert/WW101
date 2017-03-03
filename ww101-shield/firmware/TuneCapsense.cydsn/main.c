/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    CapSense_Start();
    CapSense_ScanAllWidgets();
    EZI2C_Start();
    EZI2C_EzI2CSetBuffer1(sizeof(CapSense_dsRam), sizeof(CapSense_dsRam),(uint8 *)&CapSense_dsRam);
    
    
    for(;;)
    {
        if(MB0_Read() == 0)
            Bootloadable_Load();
            
        if(!CapSense_IsBusy())
        {
            CapSense_ProcessAllWidgets();
            
            if(CapSense_IsWidgetActive(CapSense_BUTTON0_WDGT_ID))
            {
                CSLED0_Write(0);
            } else {
                CSLED0_Write(1);
            }
            if(CapSense_IsWidgetActive(CapSense_BUTTON1_WDGT_ID))
            {
                CSLED1_Write(0);
            } else {
                CSLED1_Write(1);
            }
            if(CapSense_IsWidgetActive(CapSense_BUTTON2_WDGT_ID))
            {
                CSLED2_Write(0);
            } else {
                CSLED2_Write(1);
            }
            if(CapSense_IsWidgetActive(CapSense_BUTTON3_WDGT_ID))
            {
                CSLED3_Write(0);
            } else {
                CSLED3_Write(1);
            }
            if(CapSense_IsWidgetActive(CapSense_PROXIMITY0_WDGT_ID))
            {
                ProxLED_Write(0);
            } else {
                ProxLED_Write(1);
            }
            
            CapSense_RunTuner();
            CapSense_ScanAllWidgets();
        }
    }
}

/* [] END OF FILE */
