#include <nds.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void getcmd();
extern void sendcmd();

extern void Handler(int packetID, int readlength);
extern void initNiFi();

extern int framecount;
extern char data[4096];
extern char nfdata[128];

//0 not ready, 1 act as a host and waiting, 2 act as a guest and waiting, 3 connecting, 4 connected, 5 host ready, 6 guest ready
extern int nifi_stat ;
extern int nifi_cmd ;
extern int nifi_keys ;		//holds the keys for players.
extern int new_nkeys ;		//holds the next key for refreshing.
extern int old_nkeys ;		//if no new_keys coming, use the old_keys
extern int plykeys1 ;		//player1
extern int plykeys2 ;		//player2
extern int new_framecount ;	//used by the guest for syncing.
extern int guest_framecount ;	//used by the guest for syncing.
extern int old_guest_framecount ;	//used by the guest for syncing.

extern char key_maps[16];			//for guest to record the key_buf state.
extern int key_buf[16];			//for guest to record the keys...

extern int frame_stuck;		//ont player should not be stuck when the other failed to comm...


#ifdef __cplusplus
}
#endif
