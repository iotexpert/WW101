
#include <project.h>

/* LED on/off states - LEDs are active low */
#define ON (0x00)
#define OFF (0x01)


/* Temperature measurement paramters */
#define ADC_CHANNEL_VREF			(0u)
#define ADC_CHANNEL_VTH				(1u)

/* IIR Filter coefficients for each signal */
/* Cut off frequency = fs/(2 * pi * iir_filter_constant).  In this project fs ~= 1 ksps.
This results in a cut-off frequency of 4.97 Hz.  We are using IIR filter as FIR requires 
more order of filter to get the same cut-off frequency*/
#define FILTER_COEFFICIENT_TEMPERATURE	(32u)

/* Constants used to calculate humidity */
/* This is the capacitance of the sensor at 55% RH with 0.1pF resolution */
#define CAPACITANCE_AT_55_RH        (1800)
/* Sensitivity numerator and denominator indicate sensitivity of the sensor */
#define SENSITIVITY_NUMERATOR       (31)
#define SENSITIVITY_DENOMINATOR     (100)
/* Value of reference capacitor.  Note that this value includes the pin capacitance
    and the physical 180pF reference capacitor */
#define CREF                        (1930)
/* Offset Capacitance */
#define COFFSET                     (150)
/* This is raw count equivalent to trace capacitance */
#define OFFSETCOUNT                 (1536)
#define BUFFERSIZE                  (8)
/* Nominal humidity 55% */
#define NOMINAL_HUMIDITY            (550)
#define HUMIDITY_0_PERCENT          (0)
#define HUMIDITY_100_PERCENT        (1000)
#define HUMIDITY_50                 (500)   

/* EzI2C Read/Write Boundary */
#define READ_WRITE_BOUNDARY         (2u)

/* Function Prototypes */
void InitResources(void);
/* CapSense is used for humidity measurement */
__inline uint16 CalculateCapacitance(uint16 rawCounts, uint16 refSensorCounts);
__inline uint16 CalculateHumidity(uint16 capacitance);

/* Structure that holds the I2C data                 */
/* Use __attribute__((packed)) for GCC and MDK compilers to pack structures      */
/* For other compilers use the corresponding directive.                          */
/* For example, for IAR use the following directive                              */
/* typedef __packed struct {..}struct_name;                                      */
typedef struct __attribute__((packed))
{
	uint8  RedLED;               /* Red LED state */
    uint8  BlueLED;              /* Blue LED state */
	int16  temperature;			 /* Measured temperature */
    uint16 humidity;             /* Measured humidity */
}i2c_data;

/* Declare the i2cBuffer to exchange sensor data between Bridge Control 
Panel (BCP) and PSoC Analog Coprocessor */
i2c_data i2cBuffer = {0, 0, 0, 0};

/*******************************************************************************
* Function Name: main
********************************************************************************
*
* Summary:
*  This function initializes all the resources, and in an infinite loop:
*     Handles LEDs
*     Measures the temperature
*     Measures humidity
*    
*
* Parameters:
*  None
*
* Return:
*  int
*
* Side Effects:
*   None
*******************************************************************************/
int main()
{

    /* Temperature measurment variables */
    /* Variables to hold the the ADC readings */
    int16 adcResultVREF, adcResultVTH;
    
    /* Filter input and output variables for Vref and Vth measurements */
    int16 filterOutputVref=0;
    int16 filterOutputVth=0;
    
    /* Variables to hold calculated resistance and temperature */
    int16 thermistorResistance, temperature;
    
    /* Variable to store the status returned by CyEnterCriticalSection()*/
    uint8 interruptState = 0;
 
    /* Humidity measurement variables */ 
	uint16 humidityRawCounts;	/* Raw count from CapSense Component for the humidity sensor */
	uint16 capacitance;			/* Capacitance of the humidity sensor */
	uint16 rawCountsRefCap;     /* Raw count from CapSense Component for the Reference capacitor */    
    
    
    /* Enable global interrupts */
    CyGlobalIntEnable;
    
    /* Initialize hardware resources */
    InitResources();

    /* Infinite Loop */
    for(;;)
    {
        /* LED processing */
        /* Green LED is driven from P0_3 (Arduino A1) */
        GreenLED_Write(~A1_Read());
        
        /* Red and Blue LEDs based on I2C values */
        if(i2cBuffer.RedLED) /* Turn on Red LED */
        {
            RedLED_Write(ON);
        }
        else
        {
            RedLED_Write(OFF);
        }
        if(i2cBuffer.BlueLED) /* Turn on Red LED */
        {
            BlueLED_Write(ON);
        }
        else
        {
            BlueLED_Write(OFF);
        }
        
        /* Temperature Measurement */
        /* Check if the ADC data is ready */
        if(ADC_IsEndConversion(ADC_RETURN_STATUS))
        {
            /* Read the ADC result for reference and thermistor voltages */
            adcResultVREF = ADC_GetResult16(ADC_CHANNEL_VREF);
            adcResultVTH = ADC_GetResult16(ADC_CHANNEL_VTH);
            
            /* Low pass filter the measured ADC counts of Vref */            
            filterOutputVref = (adcResultVREF + (FILTER_COEFFICIENT_TEMPERATURE - 1) * filterOutputVref) / FILTER_COEFFICIENT_TEMPERATURE;
                    
            /* Low pass filter the measured ADC counts of Vth */         
            filterOutputVth = (adcResultVTH + (FILTER_COEFFICIENT_TEMPERATURE - 1) * filterOutputVth) / FILTER_COEFFICIENT_TEMPERATURE;
                        
            /* Calculate thermistor resistance */
            thermistorResistance = Thermistor_GetResistance(filterOutputVref, filterOutputVth);           
            
            /* Calculate temperature in degree Celsius using the Component API */
            temperature = Thermistor_GetTemperature(thermistorResistance);
                       
			/* Enter critical section to check if I2C bus is busy or not */
            interruptState = CyEnterCriticalSection();
            
        	if(!(EZI2C_EzI2CGetActivity() & EZI2C_EZI2C_STATUS_BUSY))
        	{
                /* Update I2C Buffer */
                i2cBuffer.temperature = temperature;
            }
			
			/* Exit critical section */
            CyExitCriticalSection(interruptState);
        } /* End of temperature processing */
        
        /* Humidity */
        /* Check if CapSense scan is complete */
        if(!(CSD_IsBusy()))
        {       
            humidityRawCounts = CSD_BUTTON0_SNS0_RAW0_VALUE;
            rawCountsRefCap = CSD_BUTTON0_SNS1_RAW0_VALUE;
            /* Convert raw counts to capacitance */
            capacitance = CalculateCapacitance(humidityRawCounts, rawCountsRefCap);
            
            /* Enter critical section to check if I2C bus is busy or not */
            interruptState = CyEnterCriticalSection();

            if(!(EZI2C_EzI2CGetActivity() & EZI2C_EZI2C_STATUS_BUSY))
            {
                /* Calculate humidity */
                i2cBuffer.humidity = CalculateHumidity(capacitance);                
            }
            CyExitCriticalSection(interruptState);
            /* Scan sensors */
            CSD_ScanAllWidgets();
        }                  
        
    } /* End of infinite loop */
}

/*******************************************************************************
* Function Name: void InitResources(void)
********************************************************************************
*
* Summary:
*  This function initializes all the hardware resources
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side Effects:
*   None
*******************************************************************************/
void InitResources(void)
{
    /* Start EZI2C Slave Component and initialize buffer */
    EZI2C_Start();
    EZI2C_EzI2CSetBuffer1(sizeof(i2cBuffer), READ_WRITE_BOUNDARY, (uint8 *)&i2cBuffer);
     
    /* Intitialization for temperature measurement */
    /* Start the Scanning SAR ADC Component and start conversion */
    ADC_Start();
    ADC_StartConvert();
    
    /* Start Reference buffer */
    VrefBuffer_Start();
    
    /* Start Programmable Voltage Reference */
    PVref_Start();
    
    /* Enable Programmable Voltage Reference */
    PVref_Enable();
    
    
    /* Intitialization for humidity measurement */
    /* Start CapSense Component */
    CSD_Start();    
}


/*******************************************************************************
* Function Name: __inline uint16 CalculateCapacitance(uint16 RawCounts, uint16 RefsensorCounts)
********************************************************************************
*
* Summary:
*  This function calculates capacitance from raw count.
*
* Parameters:
*  uint16 RawCounts - Raw count corresponding to Humidity sensor
*  uint16 RefsensorCounts - Raw count corresponding to Reference capacitor
*
* Return:
*  Capacitance of the Humidity sensor
*
* Side Effects:
*   None
*******************************************************************************/
__inline uint16 CalculateCapacitance(uint16 rawCounts, uint16 refsensorCounts)
{
    return (uint16)((float)(rawCounts - OFFSETCOUNT) * (CREF - COFFSET) / (float)(refsensorCounts - OFFSETCOUNT));
}

/*******************************************************************************
* Function Name: __inline uint16 CalculateHumidity(uint16 Capacitance)
********************************************************************************
*
* Summary:
*  This function calculates humidity from capacitance

* Parameters:
*  uint16 Capacitance - Capacitance of the humidity sensor
*
* Return:
*  Calculated Humidity value
*
* Side Effects:
*   None
*******************************************************************************/
__inline uint16 CalculateHumidity(uint16 capacitance)
{
    int16 humidity;
    int16 delta;
    
    /* Find capacitance difference from nominal capacitance at 55% RH */
    delta = capacitance - CAPACITANCE_AT_55_RH;
    
    /* Calculate humidity from capacitance difference and sensor sensitivity */
    humidity = ((delta * SENSITIVITY_DENOMINATOR) / SENSITIVITY_NUMERATOR) + NOMINAL_HUMIDITY;
    
    /* If humidity is less than zero, limit it to 0; If humidity is greater than 1000 (100%), limit to 1000 */
    humidity = (humidity < HUMIDITY_0_PERCENT) ? HUMIDITY_0_PERCENT : (humidity > HUMIDITY_100_PERCENT) ? HUMIDITY_100_PERCENT : humidity;

    /* Return Humidity value */
    return humidity;
}
/* [] END OF FILE */
