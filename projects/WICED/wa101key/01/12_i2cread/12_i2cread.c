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

    /* Tx buffer is used to set the offset */
    uint8_t tx_buffer[] = {2};
    wiced_i2c_message_t setOffset;
    wiced_i2c_init_tx_message(&setOffset, tx_buffer, sizeof(tx_buffer), 1, 0);

    /* Rx buffer is used to get temperature and humidity data */
    uint16_t rx_buffer[] = {0,0};
    wiced_i2c_message_t msg;
    wiced_i2c_init_rx_message(&msg, rx_buffer, sizeof(rx_buffer), 1, 0);

    /* Initialize offset */
    wiced_i2c_transfer(&i2cDevice, &setOffset, 1);


    while ( 1 )
    {
    	if(buttonPress)
    	{
    		wiced_i2c_transfer(&i2cDevice, &msg, 1); /* Get new data from I2C */
    		WPRINT_APP_INFO(("Temperature: %d\tHumidity: %d\n", rx_buffer[0], rx_buffer[1])); /* Print data to terminal */

    		buttonPress = WICED_FALSE; /* Reset flag for next button press */
    	}
    }
}
