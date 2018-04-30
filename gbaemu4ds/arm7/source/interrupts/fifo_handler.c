#include <nds.h>
#include <nds/ndstypes.h>

#include "fifo_handler.h"

#include "../main.h"
#include "../sound/sound.h"

#include "../../../common/gba_ipc.h"
#include "../../../common/cpuglobal.h"

//Coto:
inline void HandleFifo() {
	
	/*
	//dont work as expected
	volatile u32 command1=0,command2=0,command3=0,command4=0;
    while(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)){
        if(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)){
            command1 = REG_IPC_FIFO_RX;
        }
        if(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)){
            command2 = REG_IPC_FIFO_RX;
        }
        if(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)){
            command3 = REG_IPC_FIFO_RX;
        }
        if(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)){
            command4 = REG_IPC_FIFO_RX;
            break; //dont care after this
        }
    }
    
    fifo7_handler(command1,command2,command3,command4);
    
    //REG_IPC_FIFO_CR |= ~(1<<14); //flush receive
    REG_IF = IRQ_FIFO_NOT_EMPTY;
    */
	
	
	//this does
	REG_IF = IRQ_FIFO_NOT_EMPTY;
	while(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)){
		volatile u32 command1 = REG_IPC_FIFO_RX;	//we already have arg0 otherwise this wouldnt be triggered
		volatile u32 command2 = 0;
		volatile u32 command3 = 0;
		volatile u32 command4 = 0;
		
		if (!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)){
			command2 = REG_IPC_FIFO_RX;
		}
		
		if (!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)){
			command3 = REG_IPC_FIFO_RX;
		}
		
		if (!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)){
			command4 = REG_IPC_FIFO_RX;
		}
		
		//iprintf("arm9: IRQ not empty received value: %x ! \n",(unsigned int)value0);

		//if there is a cmd to be executed..
		fifo7_handler(command1,command2,command3,command4);
		continue; //required: keep reading commands from the IPC RX Vector
	}
	
}

void fifo7_handler(u32 command1,u32 command2,u32 command3,u32 command4){
	
	switch (command1) {
        
		//debug
		case(0xc0700100):{
			SendArm9Command(0xc0700101,0x0,0x0,0x0);
		}
		break;
		
        //MISC Inter-Processor Opcodes
        //perform u32 read (from other core)
        case(0xc2720000):{
            //require read
            if(GBAEMU4DS_IPC->status & ARM9_BUSYFLAGRD){
                switch(command3){
                    case(0):{
                        GBAEMU4DS_IPC->buf_queue[0]= *(u32*)command2; 
                    }
                    break;
                    case(1):{
                        GBAEMU4DS_IPC->buf_queue[0]= *(u16*)command2; 
                    }
                    break;
                    case(2):{
                        GBAEMU4DS_IPC->buf_queue[0]= *(u8*)command2; 
                    }
                    break;
                }
                GBAEMU4DS_IPC->status &= ~(ARM9_BUSYFLAGRD);
            }
        }
        break;
        
        //Direct Write
        //perform u32 write(u32 address,u32 value,u8 write_mode) (from other core)
        //command2 = address / command3 = value / command4 = write_mode
        case(0xc2720001):{
            //require write
            if(GBAEMU4DS_IPC->status & ARM9_BUSYFLAGWR){
                switch(command3){
                    case(0):{
                        *(u32*)command2 = (u32)(command4); 
                    }
                    break;
                    case(1):{
                        *(u16*)command2 = (u16)(command4); 
                    }
                    break;
                    case(2):{
                        *(u8*)command2 = (u8)(command4); 
                    }
                    break;
                }
                GBAEMU4DS_IPC->status &= ~(ARM9_BUSYFLAGWR);
            }
        }
        break;
        
		//vcounter
		case(0xc1710000):{
			
		}
		break;
		
		//fifo test
		case(0xc1710001):{
			SendArm9Command(0xc1710002,0x0,0x0,0x0);
		}
		break;
		
		//FIFO DMA SA update FIFO_INTERNAL_BUFFER(A)
		case(0xc1710004):{
			SendArm9Command(0xc6760000,0x0,0x0,0x0);
		}
		break;
		
		//FIFO DMA SB update FIFO_INTERNAL_BUFFER(B)
		case(0xc1710005):{
			SendArm9Command(0xc6760001,0x0,0x0,0x0);
		}
		break;
		
		case(0xc3730000):{
			iowrite(command2,command3);
		}
		break;
		
		
    }
}