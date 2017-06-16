// WW101 TCP server that supports a single connection at a time using  WWEP.
//
// See WW101 lab manual for more information on the custom protocol WWEP.
#include "wiced.h"
#include "linked_list.h" //usr the WICED linked list library (libraries/utilities/linked_list)
#include <stdlib.h>
#include "ctype.h"
#include "database.h"
#define TCP_SERVER_LISTEN_PORT              (27708)
#define TCP_SERVER_THREAD_PRIORITY          (WICED_DEFAULT_LIBRARY_PRIORITY)
#define TCP_SERVER_STACK_SIZE               (6200)
#define PING_THREAD_PRIORITY                (WICED_DEFAULT_LIBRARY_PRIORITY)


// Globals for the tcp/ip communication system
static void tcp_server_thread_main(wiced_thread_arg_t arg);
static void pingAP(wiced_thread_arg_t arg);
static wiced_thread_t      tcp_thread;
static wiced_thread_t      ping_thread;

static const wiced_ip_setting_t ip_settings =
{
        INITIALISER_IPV4_ADDRESS( .ip_address, MAKE_IPV4_ADDRESS( 198,51,  100,  3 ) ),
        INITIALISER_IPV4_ADDRESS( .netmask,    MAKE_IPV4_ADDRESS( 255,255,255,  0 ) ),
        INITIALISER_IPV4_ADDRESS( .gateway,    MAKE_IPV4_ADDRESS( 198,51,  100,  1 ) ),
};

// To make things easier while I was on the airplane I setup the server so that it
// could be a soft AP or a station. If it is set up as a station, then the server will
// connect to an existing wireless access point using client settings from the wifi_config_dct.h
// file. If it is NOT setup as a station, then the server starts an access point using the soft AP
// settings from the wifi_config_dct.h file.
//
#define USE_STA 		(0)
#define USE_AP  		(1)
#define USE_ETHERNET 	(2)

// The options for the NETWORK_TYPE define are:
//     USE_STA (connect to an existing Wi-Fi access point as specified in the DCT)
//     USE_AP  (create an access point as specified in the DCT that the clients can connect to)
//     USE_ETHERNET (connect via an ethernet cable)
#define NETWORK_TYPE USE_STA

#if (NETWORK_TYPE == USE_STA)
#define INTERFACE WICED_STA_INTERFACE
#define DHCP_MODE WICED_USE_STATIC_IP
#endif

#if (NETWORK_TYPE == USE_AP)
#define INTERFACE WICED_AP_INTERFACE
#define DHCP_MODE WICED_USE_INTERNAL_DHCP_SERVER
#endif

#if (NETWORK_TYPE == USE_ETHERNET)
#define INTERFACE WICED_ETHERNET_INTERFACE
#define DHCP_MODE WICED_USE_STATIC_IP
#endif

// Some APs will not stay attached if you don't talk periodically
void pingAP (wiced_thread_arg_t arg)
{
    uint32_t time_elapsed;
    wiced_ip_address_t routerAddress;
    wiced_result_t result;

    while(1)
    {
        SET_IPV4_ADDRESS(routerAddress,MAKE_IPV4_ADDRESS( 198, 51, 100,  1 ));
        result = wiced_ping (INTERFACE, &routerAddress, 500, &time_elapsed);
        if(result != WICED_SUCCESS)
        {
            WPRINT_APP_INFO(("Ping Failed\n"));
        }

        wiced_rtos_delay_milliseconds(60000);
    }
}

// Main application thread which is started by the RTOS after boot
void application_start(void)
{
    wiced_init( );

    WPRINT_APP_INFO(("Starting Single Connection Server\n"));

    while(wiced_network_up( INTERFACE, DHCP_MODE, &ip_settings ) != WICED_SUCCESS); // Keep trying until you get hooked up

    // I created all of the server code in a separate thread to make it easier to put the server
    // and client together in one application.

    wiced_rtos_create_thread(&tcp_thread, TCP_SERVER_THREAD_PRIORITY, "Server TCP Server", tcp_server_thread_main, TCP_SERVER_STACK_SIZE, 0);
    wiced_rtos_create_thread(&ping_thread, PING_THREAD_PRIORITY, "Ping", pingAP, 1024, 0);

    // just blink the led while the whole thing is running
    while(1)
    {
        wiced_gpio_output_low( WICED_SH_LED1 );
        wiced_rtos_delay_milliseconds( 250 );
        wiced_gpio_output_high( WICED_SH_LED1 );
        wiced_rtos_delay_milliseconds( 250 );
    }
}


#define MAX_LEGAL_MSG (13)

void processClientCommand(int dataReadCount, uint8_t *rbuffer,char *returnMessage,char *peerInfo,int connectionCount)
{

    int err;
    err = 1; // assume an error
    /////////
    if(dataReadCount > 12) // to many characters reject
    {
        sprintf(returnMessage, "X illegal message length");
        err = 1;
        goto cleanup;
    }

    dbEntry_t receive;
    char commandId;

    // You can test using the unix command "nc" ...but this appends a "0xA" to the end
    // echo "W1234567890" | nc 198.51.100.3 27708
    // 12 or 8 may mean that there is a 0x0A a the end of the string (if you used nc)
    if(dataReadCount == 12 || dataReadCount == 8)
    {
        if(rbuffer[dataReadCount-1] != 0x0A)
        {
            sprintf(returnMessage, "X illegal message length");
            err = 1;
            goto cleanup;
        }
        dataReadCount -= 1; // Ignore the 0x0A at the end of the string
    }

    if(!((dataReadCount  == 7 && rbuffer[0] == 'R') || (dataReadCount == 11 && rbuffer[0] == 'W'))) // if it isnt a R/W then it is illegal
    {
        err = 1;
        sprintf(returnMessage,"X illegal command");
        goto cleanup;
    }

    for(int i=1;i<dataReadCount; i++) // all of the bytes must be a ASCII hex digit from 1->end of string
    {
        if(!isxdigit(rbuffer[i]))
        {
            sprintf(returnMessage,"X illegal character");
            err = 1;
            goto cleanup;
        }
    }

    if(rbuffer[0] == 'W') // it is a write
    {
        // we have a legal string so parse it
        sscanf((const char *)rbuffer,"%c%4x%2x%4x",(char *)&commandId,( int *)&receive.deviceId,( int *)&receive.regId,( int *)&receive.value);

        if((dbFind(&receive) != NULL) || (dbGetCount() <= dbGetMax()))
        {
            sprintf(returnMessage,"A%04X%02X%04X",(unsigned int)receive.deviceId,(unsigned int)receive.regId,(unsigned int)receive.value);
            dbEntry_t *newDB;
            newDB = malloc(sizeof(dbEntry_t)); // make a new entry to put in the database
            memcpy(newDB,&receive,sizeof(dbEntry_t)); // copy the received data into the new entry
            dbSetValue(newDB); // save it.
            err = 0;
        }
        else
        {
            err = 1;
            sprintf(returnMessage,"X Database Full %d",(int)dbGetCount());
        }
    }

    if(rbuffer[0] == 'R')
    {// It is a read
        sscanf((const char *)rbuffer,"%c%4x%2x",(char *)&commandId,( int *)&receive.deviceId,( int *)&receive.regId);
        dbEntry_t *foundValue;

        foundValue = dbFind(&receive); // look through the database to find a previous write of the deviceId/regId
        if(foundValue)
        {
            err=0;
            sprintf(returnMessage,"A%04X%02X%04X",(unsigned int)foundValue->deviceId,(unsigned int)foundValue->regId,(unsigned int)foundValue->value);
        }
        else
        {
            sprintf(returnMessage,"X Not Found");
            err = 1;
        }
    }

    cleanup:


    WPRINT_APP_INFO(("%s",peerInfo));

    if(err)
    {
        WPRINT_APP_INFO(("%s\n",returnMessage));
    }
    else
    {
        WPRINT_APP_INFO(("%c\t%04X\t%02X\t%04X\t%d\n",commandId,(unsigned int)receive.deviceId,(unsigned int)receive.regId,(unsigned int)receive.value,(int)connectionCount));
    }


}

static void tcp_server_thread_main(wiced_thread_arg_t arg)
{
    wiced_result_t result;
    wiced_tcp_stream_t stream;                      // The TCP stream
    wiced_tcp_socket_t socket;
    uint8_t rbuffer[MAX_LEGAL_MSG];

    int connectionCount = 0;
    char returnMessage[128]; // better use less than 128 bytes
    char peerIP[32];

    // setup the server by creating the socket and hooking it to the correct TCP Port
    result = wiced_tcp_create_socket(&socket, INTERFACE);
    if(WICED_SUCCESS != result)
    {
        WPRINT_APP_INFO(("Create socket failed\n"));
        return; // this is a bad outcome
    }

    wiced_tcp_stream_init(&stream,&socket);
    if(WICED_SUCCESS != result)
    {
        WPRINT_APP_INFO(("Init stream failed\n"));
        return; // this is a bad outcome
    }

    result = wiced_tcp_listen( &socket, TCP_SERVER_LISTEN_PORT );
    if(WICED_SUCCESS != result)
    {
        WPRINT_APP_INFO(("Listen socket failed\n"));
        return;
    }

    WPRINT_APP_INFO(("IP\t\tPort\tC\tDEVICE\tREGID\tVALUE\tCon #\n"));
    WPRINT_APP_INFO(("----------------------------------------------------------------------\n"));

    while (1 )
    {

        result = wiced_tcp_accept( &socket ); // this halts the thread until there is a connection

        if(result != WICED_SUCCESS) // this occurs if the accept times out
            continue;

        connectionCount += 1;

        /// Figure out which client is talking to us... and on which port
        wiced_ip_address_t peerAddress;
        uint16_t	peerPort;
        wiced_tcp_server_peer(&socket,&peerAddress,&peerPort);
        sprintf(peerIP,"%u.%u.%u.%u\t%d\t",
                    (uint8_t)(GET_IPV4_ADDRESS(peerAddress) >> 24),
                    (uint8_t)(GET_IPV4_ADDRESS(peerAddress) >> 16),
                    (uint8_t)(GET_IPV4_ADDRESS(peerAddress) >> 8),
                    (uint8_t)(GET_IPV4_ADDRESS(peerAddress) >> 0),
                    peerPort);

        uint32_t dataReadCount;
        wiced_tcp_stream_read_with_count(&stream,&rbuffer,MAX_LEGAL_MSG,10,&dataReadCount); // timeout in 10ms

        processClientCommand(dataReadCount, rbuffer,returnMessage,peerIP,connectionCount);


        // send response and close things up
        wiced_tcp_stream_write(&stream,returnMessage,strlen(returnMessage));
        wiced_tcp_stream_flush(&stream);
        wiced_tcp_disconnect(&socket); // disconnect the connection

        wiced_tcp_stream_deinit(&stream); // clear the stream if any crap left
        wiced_tcp_stream_init(&stream,&socket); // setup for next connection

    }
}

