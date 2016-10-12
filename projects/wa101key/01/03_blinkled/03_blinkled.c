#include "wiced.h"

void application_start( )
{
    wiced_bool_t led1 = WICED_FALSE;

    wiced_init();	/* Initialize the WICED device */

    while ( 1 )
    {
         /* Toggle LED1 */
    	if ( led1 == WICED_TRUE )
    	{
    		wiced_gpio_output_low( WICED_LED1 );
    		led1 = WICED_FALSE;
    	}
		else
		{
			wiced_gpio_output_high( WICED_LED1 );
			led1 = WICED_TRUE;
		}
        wiced_rtos_delay_milliseconds( 250 );
    }
}
