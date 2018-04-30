#include <nds.h>
#include <nds/ndstypes.h>

#include "fifo_handler.h"

#include "../../../common/gba_ipc.h"
#include "../../../common/cpuglobal.h"

#include "../ichflysettings.h"
#include "../GBA.h"
#include "../Sound.h"
#include "../Util.h"
#include "../System.h"
#include "../cpumg.h"
#include "../GBAinline.h"
#include "../bios.h"
#include "../mydebuger.h"
#include "../arm7sound.h"
#include "../main.h"

//Coto:
__attribute__((section(".itcm")))
inline void HandleFifo() {
	//dont work as expected
	/*
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
    
    fifo9_handler(command1,command2,command3,command4);
    
    REG_IPC_FIFO_CR |= ~(1<<14); //flush receive
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
		fifo9_handler(command1,command2,command3,command4);
		continue; //required: keep reading commands from the IPC RX Vector
	}
}

__attribute__((section(".itcm")))
void fifo9_handler(u32 command1,u32 command2,u32 command3,u32 command4){
	
	switch (command1) {
        
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
		//gbaemu4ds SOUND driver
		default:{
		
#ifdef unsecamr7com
			if(command1 < 0x10000000)		
#else
			if(command1 < 0x8000000 && command1 > 0x2000000)
#endif
			{
#ifdef neu_sound_16fifo
	#ifdef anyarmcom
				register u32 temp_dat = 0;
				if((command1 & 0x3) == 1)
				{
					temp_dat = 0x10;
					recDMA1++;
				}
				else if((command1 & 0x3) == 2)
				{
					temp_dat = 0x50;
					recDMA2++;
				}
				else if((command1 & 0x3) == 3)
				{
					temp_dat = 0x60;
					recDMA2++;
				}
				else recDMA1++;
				memcpy((void*)(arm7amr9buffer + temp_dat),(void*)(command1 & ~0x3),0x10);
	#else
				register u32 temp_dat = 0;
					 if((command1 & 0x3) == 1)temp_dat = 0x10;
				else if((command1 & 0x3) == 2)temp_dat = 0x50;
				else if((command1 & 0x3) == 3)temp_dat = 0x60;
				memcpy((void*)(arm7amr9buffer + temp_dat),(void*)(command1 & ~0x3),0x10);
	#endif
#else
				while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY);
				void * destination = (void*)REG_IPC_FIFO_RX;
				memcpy(destination,(u32*)command1,0x10);
#endif
			}
			else
			{
				if(command1 == 0x3F00BEEF)
				{
					VblankHandler();
					//continue;
				}

				else if(command1 == 0x4000BEEF)
				{
					while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY);
					iprintf("arm7 %08X\r\n",REG_IPC_FIFO_RX);
					//continue;
				}
				
				else if(command1 == 0x4100BEEF)
				{
					frameasyncsync();
					//continue;
				}
				
				else if(command1 == 0x4200BEEF)
				{
					if(savePath[0] == 0)sprintf(savePath,"%s.sav",szFile);
					CPUWriteBatteryFile(savePath);
					//continue;
				}
				
				else if(command1 == 0x4300BEEF)
				{
					pausemenue();
					//continue;
				}
				else {
					iprintf("error rec %08X %08X\r\n",command1,REG_IPC_FIFO_CR);
					while(1); //stop to prevent dammage
				}
			}
			//iprintf("e %08X\r\n",REG_IPC_FIFO_CR);
			
		}
		break;
    }
}