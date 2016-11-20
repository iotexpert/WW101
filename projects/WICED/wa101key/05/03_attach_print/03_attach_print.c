#include "wiced.h"


// This function takes an ipv4 address and converts it to a string ala inet_ntop
inline void convertAddressToString(wiced_ip_address_t myAddress,char *buff)
{
	sprintf(buff,"%lu.%lu.%lu.%lu",
	myAddress.ip.v4>>24 & 0xFF,
		myAddress.ip.v4>>16 & 0xFF,
		myAddress.ip.v4>>8 &0xFF,
		myAddress.ip.v4 &0xFF);

}

void application_start( void )
{

	char buff[128]; // scratch
    wiced_init( );
    wiced_gpio_t led=WICED_LED1;

    wiced_network_up( WICED_STA_INTERFACE, WICED_USE_EXTERNAL_DHCP_SERVER, NULL );

    // Get IP address format and print
    wiced_ip_address_t address;
    wiced_ip_get_ipv4_address(WICED_STA_INTERFACE,&address);
    convertAddressToString(address,buff);
    WPRINT_APP_INFO(("IP Address = %s\n",buff));

    // Get Netmask format and print
    wiced_ip_get_netmask(WICED_STA_INTERFACE,&address);
    convertAddressToString(address,buff);
    WPRINT_APP_INFO(("Netmask = %s\n",buff));

    // Get Gateway format and print
    wiced_ip_get_gateway_address(WICED_STA_INTERFACE,&address);
    convertAddressToString(address,buff);
    WPRINT_APP_INFO(("Gateway = %s\n",buff));

    // Get the MAC Address, format and print
    wiced_mac_t mac;
    wwd_wifi_get_mac_address(&mac,WICED_STA_INTERFACE);
    WPRINT_APP_INFO(("MAC Address = %02X:%02X:%02X:%02X:%02X:%02X\n",mac.octet[0],mac.octet[1],mac.octet[2],mac.octet[3],mac.octet[4],mac.octet[5]));

    // Get IP address for www.cypress.com and format/print it
    wiced_hostname_lookup("www.cypress.com",&address,2000); // 2 second timeout
    convertAddressToString(address,buff);
    WPRINT_APP_INFO(("www.cypress.com = %s\n",buff));



    while ( 1 )
    {
    	wiced_gpio_output_low(led );
        wiced_rtos_delay_milliseconds( 250 );
        wiced_gpio_output_high( led);
        wiced_rtos_delay_milliseconds( 250 );
    }
}
