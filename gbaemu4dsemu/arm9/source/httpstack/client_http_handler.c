
#include "client_http_handler.h"

#include <nds.h>
#include <dswifi9.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>  
#include <ctype.h>

#include <filesystem.h>
#include "../GBA.h"
#include "../Sound.h"
#include "../Util.h"
#include "../System.h"
#include <fat.h>
#include <dirent.h>
#include "../cpumg.h"
#include "../bios.h"
#include "../mydebuger.h"
#include "../ds_dma.h"
#include "../disk_fs/file_browse.h"
#include "../main.h"
#include "../disk_fs/fatmore.h"

#include <stdio.h>
#include <stdlib.h>
#include <nds/memory.h>
#include <nds/ndstypes.h>
#include <nds/memory.h>
#include <nds/bios.h>
#include <nds/system.h>
#include <nds/arm9/math.h>
#include <nds/arm9/video.h>
#include <nds/arm9/videoGL.h>
#include <nds/arm9/trig_lut.h>
#include <nds/arm9/sassert.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

char* server_ip = (char*)"192.168.43.220";

//coworker that handles commands in background

client_http_handler client_http_handler_context;


//used to send a single request, closes connection then
__attribute__((section(".itcm")))
void request_connection(char* str_url,int str_url_size)
{   

    int safe_length = str_url_size + 1;
    char str_url_safe[safe_length];
	
    //void * memcpy ( void * destination, const void * source, size_t num );
    memcpy(str_url_safe,str_url,safe_length);
    
    // Create a TCP socket
    
    // Find the IP address of the server, with gethostbyname
    struct hostent * myhost = gethostbyname( str_url_safe );
    
    //void * memcpy ( void * destination, const void * source, size_t num );
    memcpy((u8*)&client_http_handler_context.myhost,(u8*)myhost,sizeof(client_http_handler_context.myhost));
    
    iprintf("Found Server IP Address! %s \n",str_url_safe);
 
    // Create a TCP socket
    client_http_handler_context.socket_id = socket( AF_INET, SOCK_STREAM, 0 );
    iprintf("Created Socket!\n");

    // Tell the socket to connect to the IP address we found, on port 80 (HTTP)
    client_http_handler_context.sain.sin_family = AF_INET;
    client_http_handler_context.sain.sin_port = htons(80);
    client_http_handler_context.sain.sin_addr.s_addr= *( (unsigned long *)(client_http_handler_context.myhost.h_addr_list[0]) );
    connect( client_http_handler_context.socket_id,(struct sockaddr *)&client_http_handler_context.sain, sizeof(client_http_handler_context.sain) );
    iprintf("Connected to server!\n");
}


//send a form that issues data passed as string
__attribute__((section(".itcm")))
bool send_response(char * str_params)
{
    // send our request
    send( client_http_handler_context.socket_id, str_params, strlen(str_params), 0 );
    //iprintf("Sent our request!\n");

    // Print incoming data
    //iprintf("Printing incoming data:\n");

    /*
    //recv causes freezes
    static int recvd_len;
    static char incoming_buffer[256];
    
    while( ( recvd_len = recv( client_http_handler_context.socket_id, incoming_buffer, 255, 0 ) ) != 0 ) { // if recv returns 0, the socket has been closed.
        if(recvd_len>0) { // data was received!
            incoming_buffer[recvd_len] = 0; // null-terminate
            //iprintf(incoming_buffer);
            break;//when get-response was acquired, exit.
		}
	}
    */
    
	//iprintf("Other side closed connection!");
    return true;
}

//libnds
/*
//---------------------------------------------------------------------------------
void getHttp(char* url) {
//---------------------------------------------------------------------------------
 // Let's send a simple HTTP request to a server and print the results!
 // store the HTTP request for later
 const char * request_text = 
 "GET /dswifi/example1.php HTTP/1.1\r\n"
 "Host: www.akkit.org\r\n"
 "User-Agent: Nintendo DS\r\n\r\n";
 // Find the IP address of the server, with gethostbyname
 struct hostent * myhost = gethostbyname( url );
 iprintf("Found IP Address!\n");
 
 // Create a TCP socket
 int my_socket;
 my_socket = socket( AF_INET, SOCK_STREAM, 0 );
 iprintf("Created Socket!\n");
 // Tell the socket to connect to the IP address we found, on port 80 (HTTP)
 struct sockaddr_in sain;
 sain.sin_family = AF_INET;
 sain.sin_port = htons(80);
 sain.sin_addr.s_addr= *( (unsigned long *)(myhost->h_addr_list[0]) );
 connect( my_socket,(struct sockaddr *)&sain, sizeof(sain) );
 iprintf("Connected to server!\n");
 // send our request
 send( my_socket, request_text, strlen(request_text), 0 );
 iprintf("Sent our request!\n");
 // Print incoming data
 iprintf("Printing incoming data:\n");
 int recvd_len;
 char incoming_buffer[256];
 while( ( recvd_len = recv( my_socket, incoming_buffer, 255, 0 ) ) != 0 ) { // if recv returns 0, the socket has been closed.
 if(recvd_len>0) { // data was received!
 incoming_buffer[recvd_len] = 0; // null-terminate
 iprintf(incoming_buffer);
 }
 }
 iprintf("Other side closed connection!");
 shutdown(my_socket,0); // good practice to shutdown the socket.
 closesocket(my_socket); // remove the socket.
}
*/