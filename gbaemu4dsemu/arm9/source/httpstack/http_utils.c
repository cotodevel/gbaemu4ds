#include <nds.h>

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