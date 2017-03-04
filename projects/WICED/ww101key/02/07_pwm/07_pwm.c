// Use a PWM to adjust the brightness of an LED on the shield board
#include "wiced.h"

void application_start( )
{
    float duty_cycle = 50.0;
    float cutoff = 100.0;
    uint32_t loopDelay = 10;
    uint32_t frequency = 500;

    wiced_init();	/* Initialize the WICED device */

    wiced_pwm_init(WICED_PWM_4, frequency, duty_cycle);
    wiced_pwm_start(WICED_PWM_4);

    while ( 1 )
    {
        wiced_pwm_init(WICED_PWM_4, frequency, duty_cycle);
        duty_cycle += 1.0;
        if(duty_cycle > cutoff)
        {
        	duty_cycle = 0.0;
        }
        wiced_rtos_delay_milliseconds(loopDelay);
    }
}
