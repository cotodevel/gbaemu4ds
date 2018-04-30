#ifndef client_http_handler_declare
#define client_http_handler_declare


#include <nds.h>
#include <netinet/in.h>
#include <netdb.h>

//struct that spinlocks a current request
typedef struct {
    //dswifi socket
    struct sockaddr_in sain;
    //host socket entry
    struct hostent myhost;
    //dswifi socket id
    int socket_id;
    
    bool wifi_enabled;
    unsigned char http_buffer[4 * 1024];
    bool is_busy;
    

} client_http_handler;


#endif

#ifdef __cplusplus
extern "C"{
#endif

extern char* server_ip;

//HTTP calls:
extern void request_connection(char* str_url,int str_url_size);

//generate a GET request to a desired DNS/IP address
extern bool issue_get_response(char* str_url,int str_url_size);

//generate a POST request where str_params conforms the FORM HTTP 1.0 spec to send to url
extern bool send_response(char * str_params);

//coworker that deals with command execution from NDS side
extern client_http_handler client_http_handler_context;

//libnds
extern void getHttp(char* url);

#ifdef __cplusplus
}
#endif
