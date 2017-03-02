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
            CapSense_RunTuner();
            CapSense_ScanAllWidgets();
        }
    }
}

/* [] END OF FILE */
