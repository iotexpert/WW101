// Monitor button on the base board and set the LED based on the button's state
#include "wiced.h"

void application_start( )
{
    wiced_bool_t button1_pressed;

    wiced_init();	/* Initialize the WICED device */

    while ( 1 )
    {
        /* Read the state of Button 1 */
        button1_pressed = wiced_gpio_input_get( WICED_SH_MB1 ) ? WICED_FALSE : WICED_TRUE;  /* The button has inverse logic */

        if ( button1_pressed == WICED_TRUE )
        {   /* Turn LED1 on */
            wiced_gpio_output_high( WICED_SH_LED1 );
        }
        else
        {   /* Turn LED1 off */
        	wiced_gpio_output_low( WICED_SH_LED1 );
        }
    }
}
