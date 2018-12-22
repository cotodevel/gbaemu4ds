#include <nds.h>
#include <time.h>

#include "gba_ipc.h"

//GBA RTC
//gbaemu4ds clock opcodes
u8 gba_get_yearbytertc(){
	return (u8)GBA_IPC->clockdata[0];
}

u8 gba_get_monthrtc(){
	return (u8)GBA_IPC->clockdata[1];
}

u8 gba_get_dayrtc(){
	return (u8)GBA_IPC->clockdata[2];
}

u8 gba_get_dayofweekrtc(){
	return (u8)GBA_IPC->clockdata[3];
}


u8 gba_get_hourrtc(){
	return (u8)GBA_IPC->clockdata[4];
}

u8 gba_get_minrtc(){
	return (u8)GBA_IPC->clockdata[5];
}

u8 gba_get_secrtc(){
	return (u8)GBA_IPC->clockdata[6];
}