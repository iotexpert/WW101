// When the button on the base board is pressed, I2C is used to read
// the temperature, humidity, light, and PWM values from the analog
// co-processor on the shield board
#include "wiced.h"
#include "u8g_arm.h"
#include "mqtt_api.h"
#include "resources.h"

#define MY_THING 00

/* I2C parameters */
#define PSOC_ADDRESS (0x42)
#define DISP_ADDRESS (0x3C)
#define RETRIES (1)
#define DISABLE_DMA (WICED_TRUE)
#define NUM_MESSAGES (1)

/* I2C Offset register for button measurement */
#define OFFSET_REG (0x06)

/* CapSense Buttons */
#define B0_MASK  (0x01)
#define B1_MASK  (0x02)
#define B2_MASK  (0x04)
#define B3_MASK  (0x08)
#define ALL_MASK (0x0F)

/* Queue configuration */
#define MESSAGE_SIZE		(4)
#define QUEUE_SIZE			(10)

/* Thread parameters */
#define THREAD_BASE_PRIORITY 	(10)
#define THREAD_STACK_SIZE		(8192)

/* Time in milliseconds for automatic publish of temperature and humidity */
#define TIMER_TIME (30000)

/* Broker */
#define MQTT_BROKER_ADDRESS                 "amk6m51qrxr2u.iot.us-east-1.amazonaws.com"
#define CLIENT_ID                           "wiced_weather_aws"
#define TOPIC_HEAD							"$aws/things/ww101_"
#define TOPIC_UPDATE						"/shadow/update"
#define MQTT_REQUEST_TIMEOUT                (5000)
#define MQTT_DELAY_IN_MILLISECONDS          (1000)
#define MQTT_MAX_RESOURCE_SIZE              (0x7fffffff)
#define MQTT_PUBLISH_RETRY_COUNT            (3)

/* Publish commands */
typedef enum command {
	WEATHER_CMD,
	TEMPERATURE_CMD,
	HUMIDITY_CMD,
	ALERT_CMD,
	IP_CMD
} CMD;

/*************** Global Variables *****************/
/* Data from the PSoC Analog Co-processor  */
struct {
	uint8_t buttons;
	float temp;
	float humidity;
} __attribute__((packed)) psoc_data;

/* Flag to determine if we have an active weather alert */
wiced_bool_t weatherAlert = WICED_FALSE;

/* Flag to cause a weather publish to occur */
wiced_bool_t publish30Flag = WICED_FALSE;

/* IP Address of the local host in IP format and as a string */
wiced_ip_address_t ipAddress;
static char ip_str[25];

static wiced_ip_address_t                   broker_address;
static wiced_mqtt_event_type_t              expected_event;
static wiced_semaphore_t                    msg_semaphore;
static wiced_mqtt_security_t                security;

/* RTOS global constructs */
static wiced_semaphore_t displaySemaphoreHandle;
static wiced_semaphore_t commandStartupSemaphoreHandle;
static wiced_mutex_t i2cMutexHandle;
static wiced_queue_t pubQueueHandle;
static wiced_timer_t publishTimerHandle;
static wiced_thread_t getDataThreadHandle;
static wiced_thread_t displayThreadHandle;
static wiced_thread_t commandThreadHandle;
static wiced_thread_t publishThreadHandle;
static wiced_thread_t subscribeThreadHandle;

/*************** Function Prototypes ***************/
void getDataThread(wiced_thread_arg_t arg);
void displayThread(wiced_thread_arg_t arg);
void commandThread(wiced_thread_arg_t arg);
void publishThread(wiced_thread_arg_t arg);
void subscribeThread(wiced_thread_arg_t arg);
void publish30sec(void* arg);
/* Functions from the demo/aws_iot/pub_sub/publisher project used for publishing */
static wiced_result_t wait_for_response( wiced_mqtt_event_type_t event, uint32_t timeout );
static wiced_result_t mqtt_conn_open( wiced_mqtt_object_t mqtt_obj, wiced_ip_address_t *address, wiced_interface_t interface, wiced_mqtt_callback_t callback, wiced_mqtt_security_t *security );
static wiced_result_t mqtt_app_publish( wiced_mqtt_object_t mqtt_obj, uint8_t qos, uint8_t *topic, uint8_t *data, uint32_t data_len );
static wiced_result_t mqtt_conn_close( wiced_mqtt_object_t mqtt_obj );
static wiced_result_t mqtt_connection_event_cb( wiced_mqtt_object_t mqtt_object, wiced_mqtt_event_info_t *event );

/*************** Functions **********************/
/*************** Main application ***************/
void application_start( )
{
	wiced_init();	/* Initialize the WICED device */

	char pubCmd[4]; /* Command pushed onto the queue to determine what to publish */

	/* Setup Thread Control functions */
    wiced_rtos_init_semaphore(&displaySemaphoreHandle);
    wiced_rtos_init_semaphore(&commandStartupSemaphoreHandle);
    wiced_rtos_init_mutex(&i2cMutexHandle);
    wiced_rtos_init_queue(&pubQueueHandle, NULL, MESSAGE_SIZE, QUEUE_SIZE);
    wiced_rtos_init_semaphore( &msg_semaphore );

	/* Setup/Start Threads */
    wiced_rtos_create_thread(&getDataThreadHandle, THREAD_BASE_PRIORITY+2, NULL, getDataThread, THREAD_STACK_SIZE, NULL);
    wiced_rtos_create_thread(&displayThreadHandle, THREAD_BASE_PRIORITY+4, NULL, displayThread, THREAD_STACK_SIZE, NULL);
    wiced_rtos_create_thread(&commandThreadHandle, THREAD_BASE_PRIORITY+3, NULL, commandThread, THREAD_STACK_SIZE, NULL);
    wiced_rtos_create_thread(&publishThreadHandle, THREAD_BASE_PRIORITY+1, NULL, publishThread, THREAD_STACK_SIZE, NULL);
    wiced_rtos_create_thread(&subscribeThreadHandle, THREAD_BASE_PRIORITY+1, NULL, subscribeThread, THREAD_STACK_SIZE, NULL);

    /* Start timer to publish weather data every 30 seconds */
    wiced_rtos_init_timer(&publishTimerHandle, TIMER_TIME, publish30sec, NULL);
    wiced_rtos_start_timer(&publishTimerHandle);

    while ( 1 )
    {
		if (publish30Flag == WICED_TRUE)
		{
			publish30Flag = WICED_FALSE;
			pubCmd[0] = WEATHER_CMD;
			wiced_rtos_push_to_queue(&pubQueueHandle, &pubCmd, WICED_WAIT_FOREVER); /* Push value onto queue*/
		}

    	/* Give other threads a turn */
    	wiced_rtos_delay_milliseconds( 1000 );
    }
}

/*************** Data Acquisition Thread ***************/
/* Thread to read CapSense buttons, temperature and humidity from the PSoC analog Co-processor */
void getDataThread(wiced_thread_arg_t arg)
{
    /* Variables to remember previous values */
	float tempPrev = 0;
	float humPrev = 0;

	char pubCmd[4]; /* Command pushed onto the queue to determine what to publish */

	wiced_bool_t buttonPressed = WICED_FALSE;

	/* Setup I2C master */
    const wiced_i2c_device_t i2cDevice = {
    	.port = WICED_I2C_2,
		.address = PSOC_ADDRESS,
		.address_width = I2C_ADDRESS_WIDTH_7BIT,
		.speed_mode = I2C_STANDARD_SPEED_MODE
    };

    wiced_i2c_init(&i2cDevice);

    /* Tx buffer is used to set the offset */
    uint8_t tx_buffer[] = {OFFSET_REG};
    wiced_i2c_message_t setOffset;
    wiced_i2c_init_tx_message(&setOffset, tx_buffer, sizeof(tx_buffer), RETRIES, DISABLE_DMA);

    wiced_i2c_message_t msg;
    wiced_i2c_init_rx_message(&msg, &psoc_data, sizeof(psoc_data), RETRIES, DISABLE_DMA);

    /* Initialize offset */
    wiced_i2c_transfer(&i2cDevice, &setOffset, NUM_MESSAGES);

	while(1)
	{
   		/* Get I2C data */
		wiced_rtos_lock_mutex(&i2cMutexHandle);
		wiced_i2c_transfer(&i2cDevice, &msg, NUM_MESSAGES); /* Get new data from I2C */
		wiced_rtos_unlock_mutex(&i2cMutexHandle);

		/* Look for CapSense button presses */
		if(buttonPressed == WICED_FALSE) /* Only look for new button presses */
		{
			if((psoc_data.buttons & B0_MASK) == B0_MASK) /* Button 0 goes to the local thing's screen */
			{
				buttonPressed = WICED_TRUE;
			}
			if((psoc_data.buttons & B1_MASK) == B1_MASK) /* Button 1 scrolls through screens */
			{
				buttonPressed = WICED_TRUE;

			}
			if((psoc_data.buttons & B2_MASK) == B2_MASK) /* Button 2 toggles the weather alert */
			{
				buttonPressed = WICED_TRUE;
				if(weatherAlert == WICED_TRUE)
				{
					weatherAlert = WICED_FALSE;
				}
				else
				{
					weatherAlert = WICED_TRUE;
				}
				/* Set a semaphore for the OLED to update the display */
				wiced_rtos_set_semaphore(&displaySemaphoreHandle);
				/* Publish the alert */
				pubCmd[0] = ALERT_CMD;
				wiced_rtos_push_to_queue(&pubQueueHandle, &pubCmd, WICED_WAIT_FOREVER); /* Push value onto queue*/
			}
			if((psoc_data.buttons & B3_MASK) == B3_MASK) /* Button 3 publishes temperature and humidity */
			{
				buttonPressed = WICED_TRUE;
				pubCmd[0] = WEATHER_CMD;
				wiced_rtos_push_to_queue(&pubQueueHandle, &pubCmd, WICED_WAIT_FOREVER); /* Push value onto queue*/
			}
		}
		if(((psoc_data.buttons | ~ALL_MASK) & ALL_MASK) == 0) /* All buttons released */
		{
			buttonPressed = WICED_FALSE;
		}

		/* Look at weather data - only update display if a value has changed*/
		if((tempPrev != psoc_data.temp) || (humPrev != psoc_data.humidity)) /* Only update display if value has changed */
		{
			/* Set a semaphore for the OLED to update the display */
			wiced_rtos_set_semaphore(&displaySemaphoreHandle);
		}

		/* Wait 100 milliseconds */
		wiced_rtos_delay_milliseconds( 100 );
	}
}

/*************** OLED Display Thread ***************/
/* Thread to display data on the OLED */
void displayThread(wiced_thread_arg_t arg)
{
    /* Strings to hold the results to print */
    char thing_str[25];
	char temp_str[25];
    char humidity_str[25];

	/* Initialize the OLED display */
	wiced_i2c_device_t display_i2c =
    {
        .port          = WICED_I2C_2,
        .address       = DISP_ADDRESS,
        .address_width = I2C_ADDRESS_WIDTH_7BIT,
        .flags         = 0,
        .speed_mode    = I2C_STANDARD_SPEED_MODE,
    };

    u8g_t display;

    u8g_init_wiced_i2c_device(&display_i2c);

    u8g_InitComFn(&display, &u8g_dev_ssd1306_128x64_i2c, u8g_com_hw_i2c_fn);
    u8g_SetFont(&display, u8g_font_unifont);
    u8g_SetFontPosTop(&display);

	while(1)
	{
		/* Wait until new data is ready to display */
		wiced_rtos_get_semaphore(&displaySemaphoreHandle, WICED_WAIT_FOREVER);

		/* Setup Display Strings */
		if(weatherAlert)
		{
			sprintf(thing_str,    "ww101_%02d *", MY_THING);
		} else {
			sprintf(thing_str,    "ww101_%02d", MY_THING);
		}
		sprintf(temp_str,     "Temp:     %.1f", psoc_data.temp);
		sprintf(humidity_str, "Humidity: %.1f", psoc_data.humidity);

		/* Send data to the display */
		u8g_FirstPage(&display);
		wiced_rtos_lock_mutex(&i2cMutexHandle);
		do {
			u8g_DrawStr(&display, 0, 5, thing_str);
			u8g_DrawStr(&display, 0, 20, ip_str);
			u8g_DrawStr(&display, 0, 35,  temp_str);
			u8g_DrawStr(&display, 0, 50, humidity_str);
		} while (u8g_NextPage(&display));
		wiced_rtos_unlock_mutex(&i2cMutexHandle);
	}
}

/*************** UART Command Interface Thread ***************/
/* Tread to handle UART command input/output */
void commandThread(wiced_thread_arg_t arg)
{
	#define ESC (0x1B)
	char    receiveChar;
	char	sendChar[10];
	uint32_t expected_data_size = 1;

	char pubCmd[4]; /* Command pushed onto the queue to determine what to publish */

	/* Wait until the firmware connects to the message broker before starting the command interface */
	wiced_rtos_get_semaphore(&commandStartupSemaphoreHandle, WICED_WAIT_FOREVER);
	wiced_rtos_deinit_semaphore(&commandStartupSemaphoreHandle);
	WPRINT_APP_INFO(("\n******************************************\n"));
	WPRINT_APP_INFO(("Enter '?' for a list of available commands\n"));
	WPRINT_APP_INFO(("******************************************\n"));

	while(1)
	{
		if ( wiced_uart_receive_bytes( STDIO_UART, &receiveChar, &expected_data_size, WICED_NEVER_TIMEOUT ) == WICED_SUCCESS )
		{
			 /* If we get here then a character has been received */
			switch(receiveChar)
			{
			case '?':
				WPRINT_APP_INFO(("Commands:\n"));
				WPRINT_APP_INFO(("\tt - Print temperature and publish\n"));
				WPRINT_APP_INFO(("\th - Print humidity and publish\n"));
				WPRINT_APP_INFO(("\tA - Publish weather alert ON\n"));
				WPRINT_APP_INFO(("\ta - Publish weather alert OFF\n"));
				WPRINT_APP_INFO(("\tS - Turn subscriptions for other things ON\n"));
				WPRINT_APP_INFO(("\ts - Turn subscriptions for other things OFF\n"));
				WPRINT_APP_INFO(("\tP - Turn printing of messages from other things ON\n"));
				WPRINT_APP_INFO(("\tp - Turn printing of messages from other things OFF\n"));
				WPRINT_APP_INFO(("\tl - Scan for all things with valid data and print the list\n"));
				WPRINT_APP_INFO(("\tx - Print the current known state of the data from all things\n"));
				WPRINT_APP_INFO(("\tc - Clear the terminal and set the cusor to the upper left corner\n"));
				WPRINT_APP_INFO(("\t? - Print the list of commands\n"));
				break;
			case 't': /* Print temperature to terminal and publish */
				WPRINT_APP_INFO(("Temperature: %.1f\n", psoc_data.temp)); /* Print temperature to terminal */
			    /* Publish temperature to the cloud */
				pubCmd[0] = TEMPERATURE_CMD;
				wiced_rtos_push_to_queue(&pubQueueHandle, &pubCmd, WICED_WAIT_FOREVER); /* Push value onto queue*/
				break;
			case 'h': /* Print humidity to terminal and publish */
				WPRINT_APP_INFO(("Humidity: %.1f\t\n", psoc_data.humidity)); /* Print humidity to terminal */
			    /* Publish humidity to the cloud */
				pubCmd[0] = HUMIDITY_CMD;
				wiced_rtos_push_to_queue(&pubQueueHandle, &pubCmd, WICED_WAIT_FOREVER); /* Push value onto queue*/
				break;
			case 'A': /* Publish Weather Alert ON */
				WPRINT_APP_INFO(("\tWeather Alert ON\n"));
				weatherAlert = WICED_TRUE;
				pubCmd[0] = ALERT_CMD;
				wiced_rtos_push_to_queue(&pubQueueHandle, &pubCmd, WICED_WAIT_FOREVER); /* Push value onto queue*/
				break;
			case 'a': /* Publish Weather Alert OFF */
				WPRINT_APP_INFO(("\tWeather Alert OFF\n"));
				weatherAlert = WICED_FALSE;
				pubCmd[0] = ALERT_CMD;
				wiced_rtos_push_to_queue(&pubQueueHandle, &pubCmd, WICED_WAIT_FOREVER); /* Push value onto queue*/
				break;
			case 'S':
				break;
			case 's':
				break;
			case 'P':
				break;
			case 'p':
				break;
			case 'l':
				break;
			case 'x':
				break;
			case 'c':	/* Send VT100 clear screen code ESC[2J and move cursor to upper left corner with ESC[H */
				sendChar[0] = ESC;
				sendChar[1] = '[';
				sendChar[2] = '2';
				sendChar[3] = 'J';
				sendChar[4] = ESC;
				sendChar[5] = '[';
				sendChar[6] = 'H';
	    		wiced_uart_transmit_bytes(STDIO_UART, &sendChar , 7);
				break;
			}
		}

		/* Wait 100ms between looking for characters */
		wiced_rtos_delay_milliseconds( 100 );
	}
}

/*************** Publish Thread ***************/
/* Thread to publish data to the cloud */
void publishThread(wiced_thread_arg_t arg)
{
	char json[128] = "TEST";	  /* json message to send */

	/* Connect to the message broker */
    wiced_mqtt_object_t   mqtt_object;
    wiced_result_t        ret = WICED_SUCCESS;
    uint32_t              size_out = 0;
    int                   connection_retries = 0;
    int                   retries = 0;

	char pubCmd[4]; /* Command pushed onto the queue to determine what to publish */

    char command[4]; /* Value poped from the queue to determine what to publish */

    char topic[50];
	sprintf(topic, "%s%02d%s", TOPIC_HEAD,MY_THING,TOPIC_UPDATE);

	wiced_bool_t firstPublish = WICED_TRUE;

    /* Get AWS root certificate, client certificate and private key respectively */
    resource_get_readonly_buffer( &resources_apps_DIR_aws_iot_DIR_rootca_cer, 0, MQTT_MAX_RESOURCE_SIZE, &size_out, (const void **) &security.ca_cert );
    security.ca_cert_len = size_out;

    resource_get_readonly_buffer( &resources_apps_DIR_aws_iot_DIR_client_cer, 0, MQTT_MAX_RESOURCE_SIZE, &size_out, (const void **) &security.cert );
    if(size_out < 64)
    {
        WPRINT_APP_INFO( ( "\nNot a valid Certificate! Please replace the dummy certificate file 'resources/app/aws_iot/client.cer' with the one got from AWS\n\n" ) );
        return;
    }
    security.cert_len = size_out;

    resource_get_readonly_buffer( &resources_apps_DIR_aws_iot_DIR_privkey_cer, 0, MQTT_MAX_RESOURCE_SIZE, &size_out, (const void **) &security.key );
    if(size_out < 64)
    {
        WPRINT_APP_INFO( ( "\nNot a valid Private Key! Please replace the dummy private key file 'resources/app/aws_iot/privkey.cer' with the one got from AWS\n\n" ) );
        return;
    }
    security.key_len = size_out;

    /* Disable roaming to other access points */
    wiced_wifi_set_roam_trigger( -99 ); /* -99dBm ie. extremely low signal level */

    /* Bring up the network interface */
    ret = wiced_network_up( WICED_STA_INTERFACE, WICED_USE_EXTERNAL_DHCP_SERVER, NULL );
    if ( ret != WICED_SUCCESS )
    {
        WPRINT_APP_INFO( ( "\nNot able to join the requested AP\n\n" ) );
        return;
    }

    /* Allocate memory for MQTT object*/
    mqtt_object = (wiced_mqtt_object_t) malloc( WICED_MQTT_OBJECT_MEMORY_SIZE_REQUIREMENT );
    if ( mqtt_object == NULL )
    {
        WPRINT_APP_ERROR("Don't have memory to allocate for mqtt object...\n");
        return;
    }

    WPRINT_APP_INFO( ( "Resolving IP address of MQTT broker...\n" ) );
    ret = wiced_hostname_lookup( MQTT_BROKER_ADDRESS, &broker_address, 10000 );
    WPRINT_APP_INFO(("Resolved Broker IP: %u.%u.%u.%u\n\n", (uint8_t)(GET_IPV4_ADDRESS(broker_address) >> 24),
                    (uint8_t)(GET_IPV4_ADDRESS(broker_address) >> 16),
                    (uint8_t)(GET_IPV4_ADDRESS(broker_address) >> 8),
                    (uint8_t)(GET_IPV4_ADDRESS(broker_address) >> 0)));
    if ( ret == WICED_ERROR || broker_address.ip.v4 == 0 )
    {
        WPRINT_APP_INFO(("Error in resolving DNS\n"));
        return;
    }

    wiced_mqtt_init( mqtt_object );

    wiced_ip_get_ipv4_address(WICED_STA_INTERFACE, &ipAddress);
	sprintf(ip_str, "%d.%d.%d.%d",
	               (int)((ipAddress.ip.v4 >> 24) & 0xFF), (int)((ipAddress.ip.v4 >> 16) & 0xFF),
	               (int)((ipAddress.ip.v4 >> 8) & 0xFF),  (int)(ipAddress.ip.v4 & 0xFF));

	/* Send the IP address */
	pubCmd[0] = IP_CMD;
	wiced_rtos_push_to_queue(&pubQueueHandle, &pubCmd, WICED_WAIT_FOREVER); /* Push value onto queue*/

    do
    {
        WPRINT_APP_INFO(("[MQTT] Opening connection..."));
        connection_retries = 0;
        do
        {
            ret = mqtt_conn_open( mqtt_object, &broker_address, WICED_STA_INTERFACE, mqtt_connection_event_cb, &security );
            connection_retries++ ;
        } while ( ( ret != WICED_SUCCESS ) && ( connection_retries < WICED_MQTT_CONNECTION_NUMBER_OF_RETRIES ) );

        if ( ret != WICED_SUCCESS )
        {
            WPRINT_APP_INFO(("Failed\n"));
            break;
        }
        WPRINT_APP_INFO(("Success\n"));

        while ( 1 )
        {
        	/* Wait until a publish is requested */
			wiced_rtos_pop_from_queue(&pubQueueHandle, &command, WICED_WAIT_FOREVER);

			/* Setup the JSON message based on the command */
			switch(command[0])
			{
				case WEATHER_CMD: 	/* publish temperature and humidity */
					sprintf(json, "{\"state\" : {\"reported\" : {\"temperature\":%.1f,\"humidity\":%.1f} } }", psoc_data.temp, psoc_data.humidity);
					break;
				case TEMPERATURE_CMD: 	/* publish temperature */
					sprintf(json, "{\"state\" : {\"reported\" : {\"temperature\":%.1f} } }", psoc_data.temp);
					break;
				case HUMIDITY_CMD: 	/* publish humidity */
					sprintf(json, "{\"state\" : {\"reported\" : {\"humidity\":%.1f} } }", psoc_data.humidity);
					break;
				case ALERT_CMD: /* weather alert */
					if(weatherAlert)
					{
						sprintf(json, "{\"state\" : {\"reported\" : {\"weatherAlert\":true} } }");
					}
					else
					{
						sprintf(json, "{\"state\" : {\"reported\" : {\"weatherAlert\":false} } }");
					}
					break;
				case IP_CMD:	/* IP address */
					sprintf(json, "{\"state\" : {\"reported\" : {\"IPAddress\":\"%s\"} } }",ip_str);
					break;
			}

			WPRINT_APP_INFO(("[MQTT] Publishing..."));
			retries = 0; // reset retries to 0 before going into the loop so that the next publish after a failure will still work
			do
			{
				ret = mqtt_app_publish( mqtt_object, WICED_MQTT_QOS_DELIVER_AT_LEAST_ONCE, (uint8_t*) topic, (uint8_t*) json, strlen( json ) );
				retries++ ;
			} while ( ( ret != WICED_SUCCESS ) && ( retries < MQTT_PUBLISH_RETRY_COUNT ) );
			if ( ret != WICED_SUCCESS )
			{
				WPRINT_APP_INFO((" Failed\n"));
				break;
			}
			else
			{
				WPRINT_APP_INFO((" Success\n"));
				if(firstPublish) /* Enable the command interface thread */
				{
					wiced_rtos_set_semaphore(&commandStartupSemaphoreHandle);
					firstPublish = WICED_FALSE;
				}
			}
            wiced_rtos_delay_milliseconds( 100 );
        }

        WPRINT_APP_INFO(("[MQTT] Closing connection..."));
        mqtt_conn_close( mqtt_object );

        wiced_rtos_delay_milliseconds( MQTT_DELAY_IN_MILLISECONDS * 2 );
    } while ( 1 );

    wiced_rtos_deinit_semaphore( &msg_semaphore );
    WPRINT_APP_INFO(("[MQTT] Deinit connection...\n"));
    ret = wiced_mqtt_deinit( mqtt_object );
    free( mqtt_object );
    mqtt_object = NULL;

}

/*************** Subscribe Thread ***************/
/* Thread to subscribe to data from the cloud */
void subscribeThread(wiced_thread_arg_t arg)
{
	while(1)
	{
		wiced_rtos_delay_milliseconds(1000);
	}
}

/*************** Timer to publish weather data every 30sec ***************/
void publish30sec(void* arg)
{
	/* A flag is used because pushing to the queue doesn't work from inside the timer function */
	publish30Flag = WICED_TRUE;
}


/**************************************************************************************/
/* Functions copied from the demo/aws_iot/pub_sub/publisher application */
/*
 * A blocking call to an expected event.
 */
static wiced_result_t wait_for_response( wiced_mqtt_event_type_t event, uint32_t timeout )
{
    if ( wiced_rtos_get_semaphore( &msg_semaphore, timeout ) != WICED_SUCCESS )
    {
        return WICED_ERROR;
    }
    else
    {
        if ( event != expected_event )
        {
            return WICED_ERROR;
        }
    }
    return WICED_SUCCESS;
}

/*
 * Open a connection and wait for MQTT_REQUEST_TIMEOUT period to receive a connection open OK event
 */
static wiced_result_t mqtt_conn_open( wiced_mqtt_object_t mqtt_obj, wiced_ip_address_t *address, wiced_interface_t interface, wiced_mqtt_callback_t callback, wiced_mqtt_security_t *security )
{
    wiced_mqtt_pkt_connect_t conninfo;
    wiced_result_t ret = WICED_SUCCESS;

    memset( &conninfo, 0, sizeof( conninfo ) );
    conninfo.port_number = 0;
    conninfo.mqtt_version = WICED_MQTT_PROTOCOL_VER4;
    conninfo.clean_session = 1;
    conninfo.client_id = (uint8_t*) CLIENT_ID;
    conninfo.keep_alive = 5;
    conninfo.password = NULL;
    conninfo.username = NULL;
    conninfo.peer_cn = (uint8_t*) "*.iot.us-east-1.amazonaws.com";
    ret = wiced_mqtt_connect( mqtt_obj, address, interface, callback, security, &conninfo );
    if ( ret != WICED_SUCCESS )
    {
        return WICED_ERROR;
    }
    if ( wait_for_response( WICED_MQTT_EVENT_TYPE_CONNECT_REQ_STATUS, MQTT_REQUEST_TIMEOUT ) != WICED_SUCCESS )
    {
        return WICED_ERROR;
    }
    return WICED_SUCCESS;
}

/*
 * Publish (send) message to WICED_TOPIC and wait for 5 seconds to receive a PUBCOMP (as it is QoS=2).
 */
static wiced_result_t mqtt_app_publish( wiced_mqtt_object_t mqtt_obj, uint8_t qos, uint8_t *topic, uint8_t *data, uint32_t data_len )
{
    wiced_mqtt_msgid_t pktid;

    pktid = wiced_mqtt_publish( mqtt_obj, topic, data, data_len, qos );

    if ( pktid == 0 )
    {
        return WICED_ERROR;
    }

    if ( wait_for_response( WICED_MQTT_EVENT_TYPE_PUBLISHED, MQTT_REQUEST_TIMEOUT ) != WICED_SUCCESS )
    {
        return WICED_ERROR;
    }
    return WICED_SUCCESS;
}

/*
 * Close a connection and wait for 5 seconds to receive a connection close OK event
 */
static wiced_result_t mqtt_conn_close( wiced_mqtt_object_t mqtt_obj )
{
    if ( wiced_mqtt_disconnect( mqtt_obj ) != WICED_SUCCESS )
    {
        return WICED_ERROR;
    }
    if ( wait_for_response( WICED_MQTT_EVENT_TYPE_DISCONNECTED, MQTT_REQUEST_TIMEOUT ) != WICED_SUCCESS )
    {
        return WICED_ERROR;
    }
    return WICED_SUCCESS;
}

/*
 * Call back function to handle connection events.
 */
static wiced_result_t mqtt_connection_event_cb( wiced_mqtt_object_t mqtt_object, wiced_mqtt_event_info_t *event )
{
    switch ( event->type )
    {
        case WICED_MQTT_EVENT_TYPE_CONNECT_REQ_STATUS:
        case WICED_MQTT_EVENT_TYPE_DISCONNECTED:
        case WICED_MQTT_EVENT_TYPE_PUBLISHED:
        case WICED_MQTT_EVENT_TYPE_SUBCRIBED:
        case WICED_MQTT_EVENT_TYPE_UNSUBSCRIBED:
        {
            expected_event = event->type;
            wiced_rtos_set_semaphore( &msg_semaphore );
        }
            break;
        case WICED_MQTT_EVENT_TYPE_PUBLISH_MSG_RECEIVED:
        default:
            break;
    }
    return WICED_SUCCESS;
}
