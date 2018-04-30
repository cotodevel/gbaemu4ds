#include <nds.h>
#include "gba_ipc.h"
#include "../arm7/source/main.h"
#include "../arm9/source/main.h"

#ifdef ARM7
inline void SendArm9Command(u32 command1, u32 command2, u32 command3, u32 command4) {    
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
inline void SendArm7Command(u32 command1, u32 command2, u32 command3, u32 command4){
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

void set_ppu_vcountline_irq(u8 vcount_line_irq){
	REG_DISPSTAT &= ~(0xff<<15);
	REG_DISPSTAT |= (vcount_line_irq<<15);
}
