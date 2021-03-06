/*
 * Cypress Proprietary and Confidential. Copyright 2017 Cypress
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */

/** @file
 *
 * This is used to reset shadows for the WW101_** things on the WW-101 class AWS broker.
 *
 * To use:
 * Open a UART terminal (115200 baud).
 * Enter a 2 digit value on for the thing to be reset.
 * Enter ** to reset all thing shadows.
 */

#include "wiced.h"
#include "mqtt_api.h"
#include "resources.h"

/******************************************************
 *                      Macros
 ******************************************************/
// Broker address for the class server
#define MQTT_BROKER_ADDRESS                 "amk6m51qrxr2u.iot.us-east-1.amazonaws.com"
// Topic used to update the shadow
#define WICED_TOPIC                         "$aws/things/ww101_30/shadow/update"
#define TOPIC_HEAD							"$aws/things/ww101_"
#define TOPIC_DELETE						"/shadow/delete"
#define TOPIC_UPDATE						"/shadow/update"
#define CLIENT_ID                           "wiced_publisher_aws"
#define MQTT_REQUEST_TIMEOUT                (5000)
#define MQTT_DELAY_IN_MILLISECONDS          (1000)
#define MQTT_MAX_RESOURCE_SIZE              (0x7fffffff)
#define MQTT_PUBLISH_RETRY_COUNT            (3)
#define MSG_SHADOW                          "{\"state\" : {\"reported\" : {\"temperature\":0.0,\"humidity\":0.0,\"light\":0.0,\"weatherAlert\":false,\"IPAddress\":\"0.0.0.0\"} } }"
/* The queue messages will be 4 bytes each (a 32 bit integer) */
#define MESSAGE_SIZE		(4)
#define QUEUE_SIZE			(100)
/* UART Thread parameters */
#define THREAD_PRIORITY 	(10)
#define THREAD_STACK_SIZE	(1024)

/******************************************************
 *               Variable Definitions
 ******************************************************/
static wiced_ip_address_t                   broker_address;
static wiced_mqtt_event_type_t              expected_event;
static wiced_semaphore_t                    msg_semaphore;
static wiced_queue_t                    	wake_queue;
static uint32_t 							queue_msg;
static wiced_thread_t						uartThreadHandle;
static wiced_mqtt_security_t                security;

/******************************************************
 *               Static Function Definitions
 ******************************************************/
static void uartThread( wiced_thread_arg_t arg )
{
	char     receiveChar;
    uint32_t expected_data_size = 1;
	char     bytesReceived[2];
    uint8_t  counter = 0;

	while (1)
    {
		// Get UART data and echo back to terminal
		if ( wiced_uart_receive_bytes( STDIO_UART, &receiveChar, &expected_data_size, WICED_NEVER_TIMEOUT ) == WICED_SUCCESS )
		{
    		//Echo character back to terminal
			wiced_uart_transmit_bytes(STDIO_UART, &receiveChar , 1);
			bytesReceived[counter] = receiveChar;
			counter++;
			if(counter == 2) // If we have 2 characters from the UART, check to see if they are valid
			{
				WPRINT_APP_INFO(("\n")); // Carriage return after the number is printed
				counter = 0;
				// Look for reset all command
				if(bytesReceived[0] == '*' && bytesReceived[1] == '*')
				{
					// Reset all - fill queue with all things
					for(uint8_t i=0; i <= 3 ; i++)
					{
						for(uint8_t j=0; j <= 9 ; j++)
						{
							queue_msg = ((i+'0') << 8) + (j+'0');
							wiced_rtos_push_to_queue(&wake_queue, &queue_msg, WICED_NEVER_TIMEOUT);
						}
					}
				}
				// Look for single valid command
				else if(bytesReceived[0] <= '9' && bytesReceived[0] >= '0' && bytesReceived[1] <= '9' && bytesReceived[1] >= '0')
				{
					queue_msg = (bytesReceived[0] << 8) + bytesReceived[1];
					wiced_rtos_push_to_queue(&wake_queue, &queue_msg, WICED_NEVER_TIMEOUT);
				}
				else
				{
					WPRINT_APP_INFO(("Invalid Thing Number\n"));
				}
			}
		}
		wiced_rtos_delay_milliseconds(10);
    }
}

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

/******************************************************
 *               Function Definitions
 ******************************************************/
void application_start( void )
{
    wiced_mqtt_object_t   mqtt_object;
    wiced_result_t        ret = WICED_SUCCESS;
    uint32_t              size_out = 0;
    int                   connection_retries = 0;
    int                   retries = 0;
    char*                 msg = MSG_SHADOW;
    char			  	  topic[50];

    wiced_init( );

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
        WPRINT_APP_ERROR("Dont have memory to allocate for mqtt object...\n");
        return;
    }

    WPRINT_APP_INFO( ( "Resolving IP address of MQTT broker...\n" ) );
    ret = wiced_hostname_lookup( MQTT_BROKER_ADDRESS, &broker_address, 10000, WICED_STA_INTERFACE);
    WPRINT_APP_INFO(("Resolved Broker IP: %u.%u.%u.%u\n\n", (uint8_t)(GET_IPV4_ADDRESS(broker_address) >> 24),
                    (uint8_t)(GET_IPV4_ADDRESS(broker_address) >> 16),
                    (uint8_t)(GET_IPV4_ADDRESS(broker_address) >> 8),
                    (uint8_t)(GET_IPV4_ADDRESS(broker_address) >> 0)));
    if ( ret == WICED_ERROR || broker_address.ip.v4 == 0 )
    {
        WPRINT_APP_INFO(("Error in resolving DNS\n"));
        return;
    }

    wiced_rtos_init_queue( &wake_queue, "Wake_Queue", MESSAGE_SIZE, QUEUE_SIZE );
    wiced_mqtt_init( mqtt_object );
    wiced_rtos_init_semaphore( &msg_semaphore );

    // UART thread
    wiced_rtos_create_thread(&uartThreadHandle, THREAD_PRIORITY, "rtos_thread", uartThread, THREAD_STACK_SIZE, NULL);

    do
    {
        WPRINT_APP_INFO(("[MQTT] Opening connection..."));
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

        WPRINT_APP_INFO(("Enter 2-digit number of the thing shadow to reset or ** to reset all\n"));

        while ( 1 )
        {
            wiced_rtos_pop_from_queue(&wake_queue, &queue_msg, WICED_NEVER_TIMEOUT);

			char char1 = (char) ((queue_msg & 0x0000FF00) >> 8);
			char char2 = (char) (queue_msg & 0x000000FF);
			WPRINT_APP_INFO(("[MQTT] Deleting Existing Shadow for  ww101_%c%c ...",char1,char2));

			//Create the topic string to delete the existing shadow
			sprintf(topic, "%s%c%c%s", TOPIC_HEAD,char1,char2,TOPIC_DELETE);
			do
			{
				ret = mqtt_app_publish( mqtt_object, WICED_MQTT_QOS_DELIVER_AT_LEAST_ONCE, (uint8_t*) topic, (uint8_t*) msg, strlen( msg ) );
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
			}

			WPRINT_APP_INFO(("[MQTT] Installing Default Shadow for ww101_%c%c ...",char1,char2));

			//Create the topic string to update the shadow to the default
			sprintf(topic, "%s%c%c%s", TOPIC_HEAD,char1,char2,TOPIC_UPDATE);
			do
			{
				ret = mqtt_app_publish( mqtt_object, WICED_MQTT_QOS_DELIVER_AT_LEAST_ONCE, (uint8_t*) topic, (uint8_t*) msg, strlen( msg ) );
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
			}

            wiced_rtos_delay_milliseconds( 100 );
        }

        WPRINT_APP_INFO(("[MQTT] Closing connection..."));
        mqtt_conn_close( mqtt_object );

        wiced_rtos_delay_milliseconds( MQTT_DELAY_IN_MILLISECONDS * 2 );
    } while ( 1 );

    // The code should never get here...
    wiced_rtos_deinit_semaphore( &msg_semaphore );
    WPRINT_APP_INFO(("[MQTT] Deinit connection...\n"));
    ret = wiced_mqtt_deinit( mqtt_object );
    wiced_rtos_deinit_queue( &wake_queue );
    free( mqtt_object );
    mqtt_object = NULL;

    return;
}
