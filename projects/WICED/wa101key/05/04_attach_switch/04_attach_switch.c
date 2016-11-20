
#include "wiced.h"

/******************************************************
 *               Function Definitions
 ******************************************************/

// Print the KEY WiFi information out of the current DCT settings
void printNetworkDCT()
{
	platform_dct_wifi_config_t*     dct_wifi_config          = NULL;
	wiced_dct_read_lock( (void**) &dct_wifi_config, WICED_FALSE, DCT_WIFI_CONFIG_SECTION, 0, sizeof( *dct_wifi_config ) );

	WPRINT_APP_INFO(("SSID = %s\n",dct_wifi_config->stored_ap_list[0].details.SSID.value));
	WPRINT_APP_INFO(("SSID Length = %d\n",dct_wifi_config->stored_ap_list[0].details.SSID.length));
	WPRINT_APP_INFO(("Passphrase = %s\n",dct_wifi_config->stored_ap_list[0].security_key));
	WPRINT_APP_INFO(("Passphrase Length = %d\n",dct_wifi_config->stored_ap_list[0].security_key_length));
	WPRINT_APP_INFO(("Security = %d\n",dct_wifi_config->stored_ap_list[0].details.security));
	wiced_dct_read_unlock( dct_wifi_config, WICED_FALSE );  // free the ram buffer
}

// Change which SSID you are attached to and save it to the DCT and re-attach to the network
void changeNetwork(const char *ssid,const char * passkey,wiced_security_t security)
{

	wiced_result_t res;
	WPRINT_APP_INFO(("SSID=%s Passkey=%s Security=%d\n",ssid,passkey,security));

	wiced_network_down(WICED_STA_INTERFACE);

	platform_dct_wifi_config_t*     dct_wifi_config          = NULL;
	wiced_dct_read_lock( (void**) &dct_wifi_config, WICED_TRUE, DCT_WIFI_CONFIG_SECTION, 0, sizeof( platform_dct_wifi_config_t) );

	// save the input parameters into the ram buffer
	strcpy((char *)dct_wifi_config->stored_ap_list[0].details.SSID.value,ssid);
	dct_wifi_config->stored_ap_list[0].details.SSID.length = strlen(ssid);
	strcpy(dct_wifi_config->stored_ap_list[0].security_key,passkey);
	dct_wifi_config->stored_ap_list[0].security_key_length = strlen(passkey);
	dct_wifi_config->stored_ap_list[0].details.security = security;

	// save the RAM Buffer to the Flash DCT
    res = wiced_dct_write( (const void*) dct_wifi_config, DCT_WIFI_CONFIG_SECTION, 0, sizeof(platform_dct_wifi_config_t) );
    if ( res == WICED_SUCCESS )
         WPRINT_APP_INFO( ( "\nDCT Write Succeeded\n\n" ) );
    else
    	WPRINT_APP_INFO( ( "\nDCT Write Failed\n\n" ) );

	wiced_dct_read_unlock( dct_wifi_config, WICED_TRUE ); // free the RAM buffer

	printNetworkDCT(); // print out the current DCT

    res = wiced_network_up( WICED_STA_INTERFACE, WICED_USE_EXTERNAL_DHCP_SERVER, NULL );
    if ( res != WICED_SUCCESS )
    {
        WPRINT_APP_INFO( ( "\nUnable able to join the requested AP\n\n" ) );
    }
    else
    {
    	WPRINT_APP_INFO( ( "\nSucceeded\n\n" ) );
    }
}



void application_start( void )
{

    wiced_init( );

	char    receiveChar;
    uint32_t expected_data_size = 1;
	wiced_init();	/* Initialize the WICED device */

    while ( 1 )
    {
    	wiced_uart_receive_bytes( STDIO_UART, &receiveChar, &expected_data_size, WICED_NEVER_TIMEOUT );
        switch(receiveChar)
        {
        case '0':
        	changeNetwork("WA101WPA","kywpa123",WICED_SECURITY_WPA_MIXED_PSK);
        	break;
        case '1':
        	changeNetwork("WA101OPEN","",WICED_SECURITY_OPEN);
        	break;
        case 'p':
        	printNetworkDCT();
        	break;
        }

    }

    return;
}
