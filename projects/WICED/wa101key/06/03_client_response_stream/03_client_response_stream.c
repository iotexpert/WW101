#include "wiced.h"

#define TCP_SERVER_STACK_SIZE               (6200)
#define SERVER_PORT (27708)


static wiced_ip_address_t serverAddress; 		// address of the WAEP server
static wiced_semaphore_t button_semaphore;		// Semaphore unlocks sending of data after button presses
static wiced_thread_t buttonThread;
static uint16_t myDeviceId; 					// A checksum of the MAC address

// This function is called by the RTOS when the button is pressed
// It just unlocks the button thread semaphore
void button_isr(void *arg)
{
    wiced_rtos_set_semaphore(&button_semaphore);
}

// sendData:
// This function opens a socket connection to the WAEP server
// then sends the state of the LED and gets the response
// The input data is 0=Off, 1=On
void sendData(int data)
{
	char sendMessage[12];
	wiced_tcp_socket_t socket;
	wiced_packet_t* tx_packet;
    uint8_t *tx_data;
    uint16_t available_data_length;
    wiced_result_t result;

    // format the data per the specification in section 6
	sprintf(sendMessage,"W%04X%02X%04X",myDeviceId,5,data); // 5 is the register from the lab manual
	WPRINT_APP_INFO(("Sent Message=%s\n",sendMessage)); // echo the message so that the user can see something

	// Open the connection to the remote server via a socket
	wiced_tcp_create_socket(&socket, WICED_STA_INTERFACE);
	wiced_tcp_bind(&socket,WICED_ANY_PORT);
	wiced_tcp_connect(&socket,&serverAddress,SERVER_PORT,2000); // 2 second timeout

	// Create the packet and send the data
	wiced_packet_create_tcp(&socket, strlen(sendMessage), &tx_packet, (uint8_t**)&tx_data, &available_data_length); // get a packet
    memcpy(tx_data, sendMessage, strlen(sendMessage)); // put our data in the packet
    wiced_packet_set_data_end(tx_packet, (uint8_t*)&tx_data[strlen(sendMessage)]); // set the end of the packet
    wiced_tcp_send_packet(&socket, tx_packet);
    wiced_packet_delete(tx_packet);

    // Get the response back from the WAEP server
    wiced_packet_t *rx_packet;
    result = wiced_tcp_receive(&socket, &rx_packet, 500); // wait up to 500ms for response
    if(result == WICED_SUCCESS)
    {
    	char *rbuffer;
    	uint16_t request_length;
    	wiced_packet_get_data( rx_packet, 0, (uint8_t**) &rbuffer, &request_length, &available_data_length );
    	rbuffer[available_data_length]=0;
    	if(available_data_length<12)
    	{
    		rbuffer[available_data_length]=0; // null terminate... it is OK because the packet buffer is 1500ish bytes
    		WPRINT_APP_INFO(("Server Response=%s\n",rbuffer));
    	}
    	else
    		WPRINT_APP_INFO(("Malformed response\n"));

    	wiced_packet_delete(rx_packet);
    }

    wiced_tcp_delete_socket(&socket);
}

// buttonThreadMain:
// This function is the thread that waits for button presses and then sends the
// data via the sendData function
//
// This is done as a separate thread to make the code easier to copy to a later program.
void buttonThreadMain()
{
	wiced_mac_t myMac;
	wiced_result_t result;

    // Find the MAC address and calculate 16 bit checksum
	wwd_wifi_get_mac_address(&myMac, WICED_STA_INTERFACE );
	myDeviceId = myMac.octet[0] + myMac.octet[1] + myMac.octet[2] + myMac.octet[3] + myMac.octet[4] + myMac.octet[5];

	// Use DNS to find the address.. if you can't look it up then hard code it.
	WPRINT_APP_INFO(("DNS Lookup waep.wa101.cypress.com\n"));
	result = wiced_hostname_lookup( "waep.wa101.cypress.com", &serverAddress, 10000 );
    if ( result == WICED_ERROR || serverAddress.ip.v4 == 0 )
	{
        WPRINT_APP_INFO(("Error in resolving DNS using hard coded address\n"));
        SET_IPV4_ADDRESS( serverAddress, MAKE_IPV4_ADDRESS( 198,51,  100,  3 ) );
	}
    else
    {
	    WPRINT_APP_INFO(("waep.wa101.cypress.com IP : %u.%u.%u.%u\n\n", (uint8_t)(GET_IPV4_ADDRESS(serverAddress) >> 24),
	                    (uint8_t)(GET_IPV4_ADDRESS(serverAddress) >> 16),
	                    (uint8_t)(GET_IPV4_ADDRESS(serverAddress) >> 8),
	                    (uint8_t)(GET_IPV4_ADDRESS(serverAddress) >> 0)));
	 }

    // Setup the Semaphore and Button Interrupt
	wiced_rtos_init_semaphore(&button_semaphore); // the semaphore unlocks when the user presses the button
    wiced_gpio_input_irq_enable(WICED_BUTTON1, IRQ_TRIGGER_FALLING_EDGE, button_isr, NULL); // call the ISR when the button is pressed

    WPRINT_APP_INFO(("Starting Main Loop\n"));

    // Main Loop: wait for semaphore.. then send the data
	while(1)
	{
		wiced_rtos_get_semaphore(&button_semaphore,WICED_WAIT_FOREVER);
	    wiced_gpio_output_low( WICED_LED1 );
	    sendData(0);
		wiced_rtos_get_semaphore(&button_semaphore,WICED_WAIT_FOREVER);
		sendData(1);
		wiced_gpio_output_high( WICED_LED1 );
	}
}


void application_start(void)
{
    wiced_init( );
    wiced_network_up( WICED_STA_INTERFACE, WICED_USE_EXTERNAL_DHCP_SERVER, NULL );
    wiced_rtos_create_thread(&buttonThread, WICED_DEFAULT_LIBRARY_PRIORITY, "Button Thread", buttonThreadMain, TCP_SERVER_STACK_SIZE, 0);
}
