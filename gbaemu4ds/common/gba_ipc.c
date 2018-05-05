#include <nds.h>
#include "gba_ipc.h"

#ifdef ARM7
void SendArm9Command(u32 command1, u32 command2, u32 command3, u32 command4){
    REG_IPC_FIFO_TX = (u32)command1;
    REG_IPC_FIFO_TX = (u32)command2;
    REG_IPC_FIFO_TX = (u32)command3;
    REG_IPC_FIFO_TX = (u32)command4;
}
#endif


#ifdef ARM9
__attribute__((section(".itcm")))
void SendArm7Command(u32 command1, u32 command2, u32 command3, u32 command4){
    REG_IPC_FIFO_TX = (u32)command1;
    REG_IPC_FIFO_TX = (u32)command2;
    REG_IPC_FIFO_TX = (u32)command3;
    REG_IPC_FIFO_TX = (u32)command4;
}
#endif


//gbaemu4ds clock opcodes
u8 gba_get_yearbytertc(){
	return (u8)(u32)GBAEMU4DS_IPC->clockdata[0];
}

u8 gba_get_monthrtc(){
	return (u8)(u32)GBAEMU4DS_IPC->clockdata[1];
}

u8 gba_get_dayrtc(){
	return (u8)(u32)GBAEMU4DS_IPC->clockdata[2];
}

u8 gba_get_dayofweekrtc(){
	return (u8)(u32)GBAEMU4DS_IPC->clockdata[3];
}


u8 gba_get_hourrtc(){
	return (u8)(u32)GBAEMU4DS_IPC->clockdata[4];
}

u8 gba_get_minrtc(){
	return (u8)(u32)GBAEMU4DS_IPC->clockdata[5];
}

u8 gba_get_secrtc(){
	return (u8)(u32)GBAEMU4DS_IPC->clockdata[6];
}

//coto: new GBA SWI sleepmode
#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void enterGBASleepMode(){
	#ifdef ARM7
	SendArm9Command((u32)ARM7_REQ_SWI_TO_ARM9,0x0,0x0,0x0);
	#endif
	#ifdef ARM9
	SendArm7Command(FIFO_SWI_SLEEPMODE,0x0,0x0,0x0);
	#endif
}