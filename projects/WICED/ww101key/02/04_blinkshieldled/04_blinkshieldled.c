// Blink LED1 on the shield board with a frequency of 2 Hz
#include "wiced.h"

void application_start( )
{
    wiced_init();	/* Initialize the WICED device */

    wiced_gpio_init(WICED_GPIO_3, OUTPUT_PUSH_PULL);

    while ( 1 )
    {
		/* LED off */
    	wiced_gpio_output_low( WICED_GPIO_3 );
    	wiced_rtos_delay_milliseconds( 250 );
		/* LED on */
    	wiced_gpio_output_high( WICED_GPIO_3 );
        wiced_rtos_delay_milliseconds( 250 );
    }
}
