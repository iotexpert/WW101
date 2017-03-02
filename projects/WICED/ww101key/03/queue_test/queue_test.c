// Use a queue to send a message to an LED blink thread indicating how many times to blink.
//
// Each time the button is pressed, the blink count increases by one. Even if you press the
// button while the previous set of blinks is occurring, it will still work since the queue is
// 10 values deep. If you press the button more than 10 times while the LED is still blinking from
// a previous press,  the LED will not blink for any press after the 10th one but the blink
// count will still increment.
#include "wiced.h"

/* Thread parameters */
#define THREAD_PRIORITY 	(10)
#define THREAD_STACK_SIZE	(1024)
/* The queue messages will be 4 bytes each (a 32 bit integer) */
#define MESSAGE_SIZE		(4)
#define QUEUE_SIZE			(10)

static wiced_queue_t queueHandle;

wiced_result_t result;

/* Interrupt service routine for the button */
void button_isr(void* arg)
{

}

/* Define the thread function that will toggle the LED */
void queueThread(wiced_thread_arg_t arg)
{
	static int32_t message;

	while(1)
	{
		/* If not empty, pull the value off the queue. If empty, this will
		 * suspend the thread until a value is available. */
		wiced_rtos_pop_from_queue(&queueHandle, &message, WICED_WAIT_FOREVER);

		WPRINT_APP_INFO(("Message: %d\n",(int)message));
	}
}

void application_start( )
{
	uint32_t testmessage = 0;

	wiced_thread_t queueThreadHandle;

	wiced_init();	/* Initialize the WICED device */

    /* Initialize the queue */
	wiced_rtos_init_queue(&queueHandle, NULL, MESSAGE_SIZE, QUEUE_SIZE); /* Setup the semaphore which will be set by the button interrupt */
	WPRINT_APP_INFO(("wiced_rtos_init_queue: %d\n",result));

	/* Initialize and start thread */
    wiced_rtos_create_thread(&queueThreadHandle, THREAD_PRIORITY, NULL, queueThread, THREAD_STACK_SIZE, NULL);
	WPRINT_APP_INFO(("wiced_rtos_create_thread: %d\n",result));

    while ( 1 )
    {
		if(wiced_rtos_is_queue_full(&queueHandle) != WICED_SUCCESS) /* There is room in the queue*/
		{
			result = wiced_rtos_push_to_queue(&queueHandle, &testmessage, WICED_WAIT_FOREVER); /* Push value onto queue*/
			WPRINT_APP_INFO(("wiced_rtos_push_to_queue: %d\n",result));
		}
		testmessage++;
    	wiced_rtos_delay_milliseconds( 1000 );
    }
}
