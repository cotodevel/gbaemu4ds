#include <nds.h>
#include "gba_ipc.h"

#ifdef ARM7
void SendArm9Command(u32 command1, u32 command2, u32 command3, u32 command4){
    
    //while (!(REG_IPC_FIFO_CR & IPC_FIFO_SEND_EMPTY)){
    //}
    
    //if (REG_IPC_FIFO_CR & IPC_FIFO_ERROR) {
    //    REG_IPC_FIFO_CR |= IPC_FIFO_SEND_CLEAR;
    //}
    
    REG_IPC_FIFO_TX = (u32)command1;
    REG_IPC_FIFO_TX = (u32)command2;
    REG_IPC_FIFO_TX = (u32)command3;
    REG_IPC_FIFO_TX = (u32)command4;
    
    //REG_IPC_FIFO_CR |= (1<<14); //1
}
#endif


#ifdef ARM9
__attribute__((section(".itcm")))
void SendArm7Command(u32 command1, u32 command2, u32 command3, u32 command4){
    
    while (!(REG_IPC_FIFO_CR & IPC_FIFO_SEND_EMPTY)){
    }
    
    //if (REG_IPC_FIFO_CR & IPC_FIFO_ERROR) {
    //    REG_IPC_FIFO_CR |= IPC_FIFO_SEND_CLEAR;
    //}
    
    REG_IPC_FIFO_TX = (u32)command1;
    REG_IPC_FIFO_TX = (u32)command2;
    REG_IPC_FIFO_TX = (u32)command3;
    REG_IPC_FIFO_TX = (u32)command4;
    
    //REG_IPC_FIFO_CR |= (1<<14); //1
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
