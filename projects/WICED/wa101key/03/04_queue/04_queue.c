#include "wiced.h"

/* Thread parameters */
#define THREAD_PRIORITY 	(10)
#define THREAD_STACK_SIZE	(1024)
/* The queue messages will be 4 bytes each (a 32 bit integer) */
#define MESSAGE_SIZE		(4)
#define QUEUE_SIZE			(1)

static wiced_queue_t queueHandle;

/* Interrupt service routine for the button */
void button_isr(void* arg)
{
	static uint32_t blinks = 0;

	blinks ++;
	wiced_rtos_push_to_queue(&queueHandle, &blinks, WICED_WAIT_FOREVER); /* Push value onto queue*/
}

/* Define the thread function that will toggle the LED */
void ledThread(wiced_thread_arg_t arg)
{
	static uint32_t message;

	while(1)
	{
		uint32_t i; /* Loop Counter */
		/* Check to see if the queue is empty. If it is, suspend the thread */


		/* If not empty, pull the value off the queue */
		wiced_rtos_pop_from_queue(&queueHandle, &message, WICED_WAIT_FOREVER);

		/* Blink LED1 the specified number of times */
		for(i=0; i < message; i++)
		{
			wiced_gpio_output_high( WICED_LED1 );
			wiced_rtos_delay_milliseconds(250);
			wiced_gpio_output_low( WICED_LED1 );
			wiced_rtos_delay_milliseconds(250);
		}
	}
}

void application_start( )
{
	wiced_thread_t ledThreadHandle;

	wiced_init();	/* Initialize the WICED device */

	/* Setup button interrupt */
	wiced_gpio_input_irq_enable(WICED_BUTTON1, IRQ_TRIGGER_FALLING_EDGE, button_isr, NULL);

	/* Initialize and start LED thread */
    wiced_rtos_create_thread(&ledThreadHandle, THREAD_PRIORITY, "ledThread", ledThread, THREAD_STACK_SIZE, NULL);

    wiced_rtos_init_queue(&queueHandle, "blinkQueue", MESSAGE_SIZE, QUEUE_SIZE); /* Setup the semaphore which will be set by the button interrupt */

    while ( 1 )
    {
		wiced_rtos_delay_milliseconds( 1 ); /* Allow other threads to have a turn */
    }
}
