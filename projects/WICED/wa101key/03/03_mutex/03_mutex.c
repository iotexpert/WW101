#include "wiced.h"

/* Thread parameters */
#define THREAD_PRIORITY 	(10)
#define THREAD_STACK_SIZE	(1024)

static wiced_mutex_t printMutexHandle;

/* Define the thread function that will blink LED1 on/off every 500ms */
void led1Thread(wiced_thread_arg_t arg)
{
	wiced_bool_t led1 = WICED_FALSE;

	while(1)
	{
		wiced_rtos_lock_mutex(&printMutexHandle);
		WPRINT_APP_INFO(("TOGGLE LED1\n"));
		wiced_rtos_unlock_mutex(&printMutexHandle);

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

/* Define the thread function that will blink LED2 on/off every 100ms */
void led2Thread(wiced_thread_arg_t arg)
{
	wiced_bool_t led2 = WICED_FALSE;

	while(1)
	{
		wiced_rtos_lock_mutex(&printMutexHandle);
		WPRINT_APP_INFO(("TOGGLE LED2\n"));
		wiced_rtos_unlock_mutex(&printMutexHandle);

		/* Toggle LED2 */
		if ( led2 == WICED_TRUE )
		{
			wiced_gpio_output_low( WICED_LED2 );
			led2 = WICED_FALSE;
		}
		else
		{
			wiced_gpio_output_high( WICED_LED2 );
			led2 = WICED_TRUE;
		}
		wiced_rtos_delay_milliseconds( 249 );
	}
}

void application_start( )
{
	wiced_thread_t led1ThreadHandle;
	wiced_thread_t led2ThreadHandle;


	wiced_init();	/* Initialize the WICED device */

	/* Initialize and start threads */
    wiced_rtos_create_thread(&led1ThreadHandle, 11, "led1Thread", led1Thread, THREAD_STACK_SIZE, NULL);
    wiced_rtos_create_thread(&led2ThreadHandle, 12, "led2Thread", led2Thread, THREAD_STACK_SIZE, NULL);

    wiced_rtos_init_mutex(&printMutexHandle);

    while ( 1 )
    {
		wiced_rtos_delay_milliseconds( 1 ); /* The main application loop has to have something in it for the Thread to work */

    }
}
