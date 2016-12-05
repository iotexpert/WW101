// Read the value from the ambient light sensor using the ADC and display the result via UART
#include "wiced.h"

void application_start( )
{
    /* Variables */   
	uint16_t light_value;


    wiced_init();	/* Initialize the WICED device */
    wiced_adc_init(WICED_LIGHT, 5); 	/* Initialize the ADC */

    while ( 1 )
    {
    	wiced_adc_take_sample(WICED_LIGHT, &light_value); /* Get an ADC sample */
    	WPRINT_APP_INFO(("Light Sensor Value: %d\n", light_value));
    	wiced_rtos_delay_milliseconds(250); /* Wait 250ms */
    }
}
