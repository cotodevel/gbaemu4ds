/*---------------------------------------------------------------------------------

---------------------------------------------------------------------------------*/
#include <nds.h>
#include <dswifi9.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <stdio.h>
#include <string.h>


char ip_decimal[0x10];

char * print_ip(u32 ip)
{
    unsigned char bytes[4];
    bytes[0] = ip & 0xFF;
    bytes[1] = (ip >> 8) & 0xFF;
    bytes[2] = (ip >> 16) & 0xFF;
    bytes[3] = (ip >> 24) & 0xFF;	
    sprintf(ip_decimal,"%d.%d.%d.%d\n", bytes[0], bytes[1], bytes[2], bytes[3]);        

return ip_decimal;
}

char * print_ip(u32 ip);


void getHttp(char* url,int socket,char * body_send);

static char * server_ip = "192.168.43.220";

//---------------------------------------------------------------------------------
int main(void) {
//---------------------------------------------------------------------------------

	consoleDemoInit();  //setup the sub screen for printing

	iprintf("\n\n\tSimple Wifi Connection Demo\n\n");
	iprintf("Connecting via WFC data ...\n");

	if(!Wifi_InitDefault(WFC_CONNECT)) {
		iprintf("Failed to connect!");
	} else {

		iprintf("Connected %s\n\n",(char*)print_ip((u32)Wifi_GetIP()));
    }
	
	
    // Let's send a simple HTTP request to a server and print the results!

    // store the HTTP request for later
    const char * request_text = 
        "GET /dswifi/example1.php HTTP/1.1\r\n"
        "Host: www.akkit.org\r\n"
        "User-Agent: Nintendo DS\r\n\r\n"
        ;
    
    
    // Find the IP address of the server, with gethostbyname
    struct hostent * myhost = gethostbyname( server_ip );
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
    
    
	while(1) {
		
        if(0 == (REG_KEYINPUT & (KEY_A))){
            getHttp(server_ip,my_socket,(char*)request_text);
        }
        
        /*
        int recvd_len;
        char incoming_buffer[256];
        
        while( ( recvd_len = recv( my_socket, incoming_buffer, 255, 0 ) ) != 0 ) { // if recv returns 0, the socket has been closed.
            if(recvd_len>0) { // data was received!
                incoming_buffer[recvd_len] = 0; // null-terminate
                iprintf(incoming_buffer);
                break;//when get-response was acquired, exit.
            }
        }
        */
        
        char incoming_buffer[256];
        
        int recvd_len= recv( my_socket, incoming_buffer, 255, 0 );
        
        iprintf("recv len:%d \n",recvd_len);
        
        swiWaitForVBlank();
	}

	return 0;
}

//---------------------------------------------------------------------------------
void getHttp(char* url,int my_socket,char * body_send) {
//---------------------------------------------------------------------------------
  
    // send our request
    send( my_socket, body_send, strlen(body_send), 0 );
    iprintf("Sent our request!\n");

    // Print incoming data
    iprintf("Printing incoming data:\n");

    int recvd_len;
    char incoming_buffer[256];

    while( ( recvd_len = recv( my_socket, incoming_buffer, 255, 0 ) ) != 0 ) { // if recv returns 0, the socket has been closed.
        if(recvd_len>0) { // data was received!
            incoming_buffer[recvd_len] = 0; // null-terminate
            iprintf(incoming_buffer);
            break;//when get-response was acquired, exit.
		}
	}

	iprintf("Other side closed connection!");

	//shutdown(my_socket,0); // good practice to shutdown the socket. // unless we turn off console
 
	//closesocket(my_socket); // remove the socket.  // unless we turn off console
}

