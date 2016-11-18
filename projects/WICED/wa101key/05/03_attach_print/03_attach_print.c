#include "wiced.h"

void application_start( void )
{

    wiced_init( );
    wiced_gpio_t led;

    wiced_result_t ret = wiced_network_up( WICED_STA_INTERFACE, WICED_USE_EXTERNAL_DHCP_SERVER, NULL );
    if ( ret != WICED_SUCCESS )
    {
        WPRINT_APP_INFO( ( "\nUnable able to join the requested AP\n\n" ) );
        led = WICED_LED2;
    }
    else
    {
    	WPRINT_APP_INFO( ( "\nSucceeded\n\n" ) );
    	led = WICED_LED1;
    }
    while ( 1 )
    {
    	wiced_gpio_output_low(led );
        wiced_rtos_delay_milliseconds( 250 );
        wiced_gpio_output_high( led);
        wiced_rtos_delay_milliseconds( 250 );
    }
}
