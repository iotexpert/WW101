#include "wiced.h"


volatile wiced_bool_t buttonPress = WICED_FALSE;

/* Interrupt service routine for the button */
void button_isr(void* arg)
{
	buttonPress = WICED_TRUE;
}

/* Main application */
void application_start( )
{
    wiced_bool_t redLed = WICED_FALSE;
    wiced_bool_t blueLed = WICED_TRUE;

	wiced_init();	/* Initialize the WICED device */

    wiced_gpio_input_irq_enable(WICED_BUTTON1, IRQ_TRIGGER_FALLING_EDGE, button_isr, NULL); /* Setup interrupt */

    /* Setup I2C master */
    const wiced_i2c_device_t i2cDevice = {
    	.port = WICED_I2C_1,
		.address = 0x08,
		.address_width = I2C_ADDRESS_WIDTH_7BIT,
		.speed_mode = I2C_STANDARD_SPEED_MODE
    };

    wiced_i2c_init(&i2cDevice);

    /* Setup transmit buffer and message */
    uint8_t tx_buffer[] = {0, redLed, blueLed};
    wiced_i2c_message_t msg;
    wiced_i2c_init_tx_message(&msg, tx_buffer, sizeof(tx_buffer), 1, 0);

    while ( 1 )
    {
    	if(buttonPress)
    	{
    		/* Invert LED states */
    		redLed = !redLed;
    		blueLed = !blueLed;

    		/* Send new I2C data */
    		tx_buffer[1] = redLed;
    		tx_buffer[2] = blueLed;
    		wiced_i2c_transfer(&i2cDevice, &msg, 1);

    		buttonPress = WICED_FALSE; /* Reset flag for next button press */
    	}
    }
}
