#include "project.h"
#include "regmap.h"
#include "u8g2.h"
#include <stdio.h>

dataSet_t pafeDataSet;
#define PSOC_AFE_I2C (0x42)
// How often in MS to update the screen
#define UPDATE_INTERVAL 200
int updateData=0; // this flag is set by the systick timer ISR
u8x8_t u8x8;
int16 A0,A1,A2; // hold the ADC values for the 3 ADC channels
char buff[64];  // a global scratch buffer to hold sprintfs
int bootloaderMode = 0; // the baseboard button toggles this flag

// A HAL function for the u8x8 library
uint8_t u8x8_byte_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    uint8_t *data;
    switch(msg)
    {
        case U8X8_MSG_BYTE_SEND:
            data = (uint8_t *)arg_ptr;
            while( arg_int > 0 )
            {
                (void)I2C_I2CMasterWriteByte(*data); // No Error Checking..
  	            data++;
	            arg_int--;
            }
        break;
      
        case U8X8_MSG_BYTE_INIT: // Using the HW block you dont need to set 1/1
        break;
        case U8X8_MSG_BYTE_SET_DC:
        break;
        case U8X8_MSG_BYTE_START_TRANSFER:
            (void)I2C_I2CMasterSendStart(u8x8_GetI2CAddress(u8x8)>>1,I2C_I2C_WRITE_XFER_MODE); // no Error Checking
         break;
        
        case U8X8_MSG_BYTE_END_TRANSFER:
            (void)I2C_I2CMasterSendStop(); // no error checking
        break;
    
        default:
            return 0;
    }
    
    return 1;
}

/* 
 * A PSoC implementation of the delay and gpio callback for the u8x8 HAL
 * 
*/
uint8_t psoc_gpio_and_delay_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    (void)u8x8;
    (void)arg_ptr;
    switch(msg)
    {
        case U8X8_MSG_GPIO_AND_DELAY_INIT: // for now who cares?
        break;
        case U8X8_MSG_DELAY_MILLI:
            CyDelay(arg_int);
        break;
        case U8X8_MSG_DELAY_10MICRO:
            CyDelayUs(10);
        break;
        case U8X8_MSG_DELAY_100NANO:
            CyDelayCycles((CYDEV_BCLK__SYSCLK__HZ/1000000) * 100 - 16); //PSoC system reference guide says ~16 extra cycles 
        break;
        /*    - My Display has only I2C... which I have implemented in HW
         * If you want to use a software interface or have these pins then you 
         * need to read and write them
        case U8X8_MSG_GPIO_SPI_CLOCK:
        case U8X8_MSG_GPIO_SPI_DATA:
        case U8X8_MSG_GPIO_CS:
        case U8X8_MSG_GPIO_DC:
        case U8X8_MSG_GPIO_RESET:
        case U8X8_MSG_GPIO_SPI_CLOCK:
        case U8X8_MSG_GPIO_SPI_DATA:		
        case U8X8_MSG_GPIO_D0:    
        case U8X8_MSG_GPIO_D1:			
        case U8X8_MSG_GPIO_D2:		
        case U8X8_MSG_GPIO_D3:		
        case U8X8_MSG_GPIO_D4:	
        case U8X8_MSG_GPIO_D5:	
        case U8X8_MSG_GPIO_D6:	
        case U8X8_MSG_GPIO_D7:
        case U8X8_MSG_GPIO_E: 	
        case U8X8_MSG_GPIO_I2C_CLOCK:
        case U8X8_MSG_GPIO_I2C_DATA:
        break;  */
    }
    return 1;
}


// The function is called by the systick timer... and is used to trigger
// the updateDisplay screen every UPDATE_INTERVAL
void sysTickISR()
{
    static int counter=0;
    if(counter++ > UPDATE_INTERVAL)
    {
        updateData = 1;
        counter = 0;
    }
}


// This function displays the voltage in mV from the 3 Arduino header pins (A0,A1,A2)
void displayDataScreen0()
{
    sprintf(buff,"A0=%d mV   ",ADC_CountsTo_mVolts(0,A0));
    u8x8_DrawString(&u8x8,0,3,buff);
    
    sprintf(buff,"A1=%d mV   ",ADC_CountsTo_mVolts(1,A1));
    u8x8_DrawString(&u8x8,0,4,buff);
    
    sprintf(buff,"A2=%d mV  ",ADC_CountsTo_mVolts(2,A2));
    u8x8_DrawString(&u8x8,0,5,buff);
   
}

// This function displays the last read values from the I2C Register Map
// In addition it displays the current values it is reading from A1(DAC) ,A2 (Pot)
void displayDataScreen1()
{
    
    sprintf(buff,"Temp=%.1f C",pafeDataSet.temperature);
    u8x8_DrawString(&u8x8,0,2,buff);
    
    sprintf(buff,"Humidity=%.1f%%",pafeDataSet.humidity);
    u8x8_DrawString(&u8x8,0,3,buff);

    sprintf(buff,"Illum=%.1f lux",pafeDataSet.illuminance);
    u8x8_DrawString(&u8x8,0,4,buff);
    
    
    sprintf(buff,"Pot=%.2fV",pafeDataSet.potVal);
    u8x8_DrawString(&u8x8,0,5,buff);

    sprintf(buff,"A2=%.2fV",(float)ADC_CountsTo_mVolts(2,A2) / 1000.0);
    u8x8_DrawString(&u8x8,0,6,buff);
    
    sprintf(buff,"A1=%.2fV",(float)ADC_CountsTo_mVolts(1,A1) /1000.0);
    u8x8_DrawString(&u8x8,0,7,buff);
    

    
}

// This function displays the I2C Register map variable from the LED and Buttons
void displayDataScreen2()
{
    sprintf(buff,"Buttons=%x",pafeDataSet.buttonVal);
    u8x8_DrawString(&u8x8,0,3,buff);
    
    sprintf(buff,"LEDs=%x",pafeDataSet.ledVal);
    u8x8_DrawString(&u8x8,0,4,buff);
    
    
    sprintf(buff,"LED Cntrl=%x",pafeDataSet.ledControl);
    u8x8_DrawString(&u8x8,0,5,buff);
    
}


// This typedef is used to hold a function pointer for the display function
// and a subtitle for the screen
typedef struct DisplayFunction {
    void (*fcn)(void);
    char *title;
} DisplayFunction;

// One entry per screen
DisplayFunction displayFunctions[] = { 
    {displayDataScreen1,"Test"},
    {displayDataScreen0,"Base ADC"},
    {displayDataScreen2,"Buttons"}
};
#define NUM_SCREENS (sizeof(displayFunctions)/sizeof(DisplayFunction))
int currentScreen=0; // a counter of the current displayed screen


// This function displays the main title, subtitle (cetered)
// the calls the current display function
void switchScreen()
{
    int offset = (16-strlen(displayFunctions[currentScreen].title))/2;
    u8x8_ClearDisplay(&u8x8);
    u8x8_DrawString(&u8x8,1,0,"PSoC AFE Shield");
    u8x8_DrawString(&u8x8,offset,1,displayFunctions[currentScreen].title);
   
    (*displayFunctions[currentScreen].fcn)();
   
}

// ISR for base button... it puts the baseboard into "bootloader mode" which actually
// enables the shield to bootload (by not writing to the screen)
void handleBaseSwitch()
{
    bootloaderMode = !bootloaderMode;
    Red_Write(bootloaderMode);
    SW1_ClearInterrupt();
}



int main(void)
{
  
    CyGlobalIntEnable; /* Enable global interrupts. */
  
    
    ADC_Start();
    ADC_StartConvert();
    I2C_Start();
    
    SWISR_StartEx(handleBaseSwitch);

    // Initialize the U8 Display
    u8x8_Setup(&u8x8, u8x8_d_ssd1306_128x64_noname, u8x8_cad_ssd13xx_i2c, u8x8_byte_hw_i2c, psoc_gpio_and_delay_cb);
    u8x8_InitDisplay(&u8x8);  
    u8x8_SetPowerSave(&u8x8,0);
    u8x8_SetFont(&u8x8,u8x8_font_amstrad_cpc_extended_f);
   
    switchScreen(); // Display the screen for the first time
        
    CySysTickStart();
    CySysTickSetCallback(0,sysTickISR);
   
    // This structure is used to send the ezi2c register address + the dac value
    typedef CY_PACKED struct SendBuff {
        uint8 address;
        float dacValue;
    } CY_PACKED_ATTR SendBuff_t;
    
    SendBuff_t sendBuff;
    sendBuff.address = 0; // Start in the first byte of the I2C register map
    sendBuff.dacValue = 0.0; // star the dac at 0.0v
   


    // set the PSoC AFE EZI2C Read Pointer to 0
    uint32 rval = I2C_I2CMasterSendStart(PSOC_AFE_I2C,I2C_I2C_WRITE_XFER_MODE);
    if(rval != I2C_I2C_MSTR_NO_ERROR) // if something bad happens hang the device and turn on the green LED
    {
        Green_Write(0);
        while(1);
    }
    I2C_I2CMasterWriteByte(0);
    I2C_I2CMasterSendStop();
        
    for(;;)
    {
        int b1Prev;
        int b1State;
        
        // Read the values of the Analog Arduino Pins
        if(ADC_IsEndConversion(ADC_RETURN_STATUS))
        {
            A0 = ADC_GetResult16(0);
            A1 = ADC_GetResult16(1);
            A2 = ADC_GetResult16(2);            
        }
        
        // Handle the user button which switches screens
        b1State = D3_Read();
        if(b1State == 0 && b1Prev == 1)
        {
            currentScreen = (currentScreen + 1) % NUM_SCREENS;
            switchScreen();
        }
        b1Prev = b1State;
      
        
        
        // Read the PSoC AFE Shield I2C Register Map
        I2C_I2CMasterReadBuf(PSOC_AFE_I2C,(uint8 *)&pafeDataSet,sizeof(pafeDataSet),I2C_I2C_MODE_COMPLETE_XFER);
        while (0u == (I2C_I2CMasterStatus() & I2C_I2C_MSTAT_RD_CMPLT));
        
        // If the timer ISR sets the updateDate flag then update the DAC & the Display
        if(updateData)
        {
            updateData = 0;
            (*displayFunctions[currentScreen].fcn)(); // Update the display
            
            
            // Update Dac Value
            sendBuff.dacValue = sendBuff.dacValue + 0.1;
            if(sendBuff.dacValue > 2.1)
                sendBuff.dacValue = 0.0;
        
            // Send the updated DAC value
            I2C_I2CMasterWriteBuf(PSOC_AFE_I2C,(uint8 *)&sendBuff,sizeof(sendBuff),I2C_I2C_MODE_COMPLETE_XFER);
            while (0u == (I2C_I2CMasterStatus() & I2C_I2C_MSTAT_WR_CMPLT));
   
        }
        
        while(bootloaderMode) // Disable all I2C traffic so PSoc AFE can bootload
        {
            CyDelay(100);
            Red_Write(~Red_Read());
        }
    }
}
