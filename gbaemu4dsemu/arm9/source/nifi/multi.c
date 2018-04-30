#include <nds.h>
#include "multi.h"

#include "../dswifi_arm9/wifi_arm9.h"

//Shared memory region static object
#include "../../../common/clock.h"
#include "../../../common/gba_ipc.h"

char data[4096];
static const char nesds[32]		= {0xB2, 0xD1, 'n', 'e', 's', 'd', 's', 0};
static const char nfconnect[32]	= {0xB2, 0xD1, 'c', 'o', 'n', 'e', 'd', 0};
static char nfcrc[32]				= {0xB2, 0xD1, 0x81, 0, 0, 0};

// nfdata[3..6]	= nifi_cmd
// 7..8			= framecount	used by guest, sent by host
// 9..10		= player1 keys  sent by host
// 11.12		= player2 keys  sent by guest, re-sent by guest
// 13.77		= hosts' key_buf... needed by guest when the recent packages were lost.
char nfdata[128]			= {0xB2, 0xD1, 0x82, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
//static char nfsync[]			= {0xB2, 0xD1, 0x83, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};		//sync the memory....

//0 not ready, 1 act as a host and waiting, 2 act as a guest and waiting, 3 connecting, 4 connected, 5 host ready, 6 guest ready
int nifi_stat = 0;
int nifi_cmd = 0;
int nifi_keys = 0;		//holds the keys for players.
int new_nkeys = 0;		//holds the next key for refreshing.
int old_nkeys = 0;		//if no new_keys coming, use the old_keys
int plykeys1 = 0;		//player1
int plykeys2 = 0;		//player2
int new_framecount = 0;	//used by the guest for syncing.
int guest_framecount = 0;	//used by the guest for syncing.
int old_guest_framecount = 0;	//used by the guest for syncing.
int framecount = 0;

char key_maps[16];			//for guest to record the key_buf state.
int key_buf[16];			//for guest to record the keys...

int frame_stuck;		//ont player should not be stuck when the other failed to comm...

void deep_reset()
{
	//unsigned char *memp = (unsigned char *)NES_RAM;
	//memset(memp, 0, 0x7800);
	memset(key_buf, 0, 16 * 4);
	memset(key_maps, 0, 16);
}

void sendcmd()
{
	memcpy(nfdata + 3, &nifi_cmd, 4);
	if(nifi_stat == 5) {	//host
		memcpy(nfdata + 9, &new_nkeys, 4);
		memcpy(nfdata + 7, &new_framecount, 2);
		memcpy(nfdata + 13, key_buf, 4*16);
		Wifi_RawTxFrame(78, 0x0014, (unsigned short *)nfdata);
	} else {
		//plykeys2 = IPC_KEYS & MP_KEY_MSK;
		memcpy(nfdata + 7, &framecount, 2);
		memcpy(nfdata + 11, &plykeys2, 2);
		Wifi_RawTxFrame(14, 0x0014, (unsigned short *)nfdata);
	}
}


void getcmd()
{
	memcpy(&nifi_cmd, data + 35, 4);
	if(nifi_stat == 5) {	//host
		memcpy(&plykeys2, data + 43, 2);
		memcpy(&guest_framecount, data + 39, 2);
	} else {
		int i;
		char *kbufp = data + 45;
		memcpy(&new_framecount, data + 39, 2);
		memcpy(&new_nkeys, data + 41, 4);
		key_buf[(new_framecount >> 1) &15] = new_nkeys;
		key_maps[(new_framecount >> 1) &15] = 1;			//the key is valid.
		for(i = framecount + 1; i < new_framecount; i+=2) {
			int tmp = (i >> 1) & 15;
			memcpy(&key_buf[tmp], kbufp + tmp * 4, 4);
			key_maps[tmp] = 1;
		}
	}
}


void Handler(int packetID, int readlength)
{
	Wifi_RxRawReadPacket(packetID, readlength, (unsigned short *)data);

	// Lazy test to see if this is our packet.
	if (data[32] == 0xB2 && data[33] == 0xD1)  {//a packet from another ds.
		switch(nifi_stat) {
			case 0:
				return;
			case 1:			//love this.
				if(strncmp(data + 34, nesds + 2, 8) == 0) {
					nifi_stat = 3;
				}
				break;
			case 2:			//love this.
				if(strncmp(data + 34, nfconnect + 2, 8) == 0) {
					nifi_stat = 4;
				}
				break;
			case 3:
				
				if(data[34] == 0x81) {		//Check the CRC. Make sure that both players are using the same game.
					int remotecrc = (data[35] | (data[36] << 8));
					//ori if(debuginfo[17] == remotecrc) {	//ok. same game
						if(1==1){
						nifi_stat = 5;
						//nifi_cmd |= MP_CONN;
						sendcmd();
						//hideconsole();
						//NES_reset();
						//deep_reset();
						new_nkeys = 0;
						nifi_keys = 0;
						plykeys1 = 0;
						plykeys2 = 0;
						framecount = 0;
						new_framecount = 0;
						guest_framecount = 0;
						//global_playcount = 0;
						//joyflags &= ~AUTOFIRE;
						//__af_st = __af_start;
						//menu_game_reset();	//menu is closed.
					} else {		//bad crc. disconnect the comm.
						nifi_stat = 0;
						//nifi_cmd &= ~MP_CONN;
						sendcmd();
					}
				}
				
				break;
			case 4:
				if(data[34] == 0x82) {
					
					getcmd();
					//ori: if(nifi_cmd & MP_CONN) {	//CRC ok, get ready for multi-play.
						if(1==0){
						nifi_stat = 6;
						//hideconsole();
						//NES_reset();
						//deep_reset();
						new_nkeys = 0;
						nifi_keys = 0;
						plykeys1 = 0;
						plykeys2 = 0;
						framecount = 0;
						new_framecount = 0;
						guest_framecount = 0;
						//global_playcount = 0;
						//joyflags &= ~AUTOFIRE;
						//__af_st = __af_start;
						//menu_game_reset();	//menu is closed.
					} else {					//CRC error, the both sides should choose the some game.
						nifi_stat = 0;
					}
					
				}
				break;
			case 5:						//as a host, and receiving the package from guest.
				getcmd();
				break;
			case 6:						//update player1's joystate
				getcmd();
				break;
		}
	}
	
	//debug ok
	//iprintf("\n wifi handler running!");
	
}


void initNiFi()
{
    /*
	Wifi_InitDefault_gba(false); //uses ipc / fixed / irq is not inited here
	Wifi_SetPromiscuousMode(1);
	//ori: //Wifi_EnableWifi();
	Wifi_RawSetPacketHandler(Handler);
	Wifi_SetChannel(10);
	*/
    
	if(1) {
		//for secial configuration for wifi / moved to ARM7
		/*
		irqDisable(IRQ_TIMER3);
		irqSet(IRQ_TIMER3, Timer_10ms); // replace timer IRQ
		// re-set timer3
		TIMER3_CR = 0;
		TIMER3_DATA = -(6553 / 5); // 6553.1 * 256 / 5 cycles = ~10ms;
		TIMER3_CR = 0x00C2; // enable, irq, 1/256 clock
		irqEnable(IRQ_TIMER3);
		*/
	}
	
	//Disable ARM7 Timer
	SendArm7Command(0xc2720102,IRQ_TIMER3,0x0,0x0); //Installs the timer that drives WIFI card through a TIMER IRQ on ARM7 (10ms)
	
	//init timer3 settings on ARM7
	SendArm7Command(0xc2720104,5,0x0,0x0); //Installs the timer that drives WIFI card through a TIMER IRQ on ARM7 (10ms)
	
	//init timer3 handler on ARM7 
	SendArm7Command(0xc2720101,IRQ_TIMER3,0x0,0x0); //Installs the timer that drives WIFI card through a TIMER IRQ on ARM7
	
	//Enable ARM7 Timer
	SendArm7Command(0xc2720103,IRQ_TIMER3,0x0,0x0); //Installs the timer that drives WIFI card through a TIMER IRQ on ARM7 (10ms)
	
	
	//debug
	/*
	nifi_stat = 1;
	int cou=0;
	
	while(1==1){
		Wifi_RawTxFrame(14, 0x0014, (unsigned short *)nfdata);
	}
	*/
}