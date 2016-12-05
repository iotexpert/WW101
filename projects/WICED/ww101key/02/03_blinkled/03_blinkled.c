// Blink LED1 on the base board with a frequency of 2 Hz
#include "wiced.h"

void application_start( )
{
    wiced_init();	/* Initialize the WICED device */

    while ( 1 )
    {
		/* LED off */
    	wiced_gpio_output_low( WICED_LED1 );
    	wiced_rtos_delay_milliseconds( 250 );
		/* LED on */
    	wiced_gpio_output_high( WICED_LED1 );
        wiced_rtos_delay_milliseconds( 250 );
    }
}
