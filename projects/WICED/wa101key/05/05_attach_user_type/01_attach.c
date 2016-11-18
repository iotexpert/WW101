/*
 * Broadcom Proprietary and Confidential. Copyright 2016 Broadcom
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */

/** @file
 *
 * A rudimentary AWS IOT publisher application which demonstrates how to connect to
 * AWS IOT cloud (MQTT Broker) and publish MQTT messages for a given topic.
 *
 * This application publishes "LIGHT ON" or "LIGHT OFF" message for each button press
 * to topic "WICED_BULB" with QOS1. Button used is WICED_BUTTON1 on the WICED board.
 * If we press the button first time it publishes "LIGHT OFF" and if we press next the same button
 * it will publish "LIGHT ON".
 *
 * To run the app, work through the following steps.
 *  1. Modify Wi-Fi configuration settings CLIENT_AP_SSID and CLIENT_AP_PASSPHRASE in wifi_config_dct.h to match your router settings.
 *  2. Update the AWS MQTT broker address (MQTT_BROKER_ADDRESS) if needed.
 *  3. Make sure AWS Root Certifcate 'resources/apps/aws_iot/rootca.cer' is up to date while building the app.
 *  4. Copy client certificate and private key for the given AWS IOT user in resources/apps/aws_iot folder.
 *     Ensure that valid client certificates and private keys are provided for the AWS IOT user in resources/apps/aws_iot folder.
 *  5. Build and run this application.
 *  6. Run another application which subscribes to the same topic.
 *  7. Press button WICED_BUTTON1 to publish the messages "LIGHT ON" or LIGHT OFF" alternatively and check if
 *     it reaches subscriber app running on the other WICED board (which can be anywhere but connected to internet)
 *
 */

#include "wiced.h"

/******************************************************
 *               Function Definitions
 ******************************************************/
void application_start( void )
{

    wiced_init( );
    wiced_bool_t led1 = WICED_FALSE;
    wiced_gpio_t led;


    /* Disable roaming to other access points */
    wiced_wifi_set_roam_trigger( -99 ); /* -99dBm ie. extremely low signal level */


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


/*
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
*/

    while ( 1 )
       {
            /* Toggle LED1 */
       	if ( led1 == WICED_TRUE )
       	{
       		wiced_gpio_output_low(led );
       		led1 = WICED_FALSE;
       	}
   		else
   		{
   			wiced_gpio_output_high( led);
   			led1 = WICED_TRUE;
   		}
           wiced_rtos_delay_milliseconds( 250 );
       }

    return;
}
