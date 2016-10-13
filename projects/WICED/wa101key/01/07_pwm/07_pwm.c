#include "wiced.h"

void application_start( )
{
	float duty_cycle = 50.0;

    wiced_init();	/* Initialize the WICED device */

    wiced_pwm_init(WICED_PWM_4, 500, duty_cycle);
    wiced_pwm_start(WICED_PWM_4);

    while ( 1 )
    {
        wiced_pwm_init(WICED_PWM_4, 500, duty_cycle);
        duty_cycle += 1.0;
        if(duty_cycle > 100.0)
        {
        	duty_cycle = 0.0;
        }
        wiced_rtos_delay_milliseconds( 10 );
    }
}
