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
		
		//fifo test
		case(0xc1710002):{
			iprintf("fifotest ok \n");
		}
		break;
		
		//FIFO DMA Sound A IRQ from ARM7 Sound
		case(0xc1710003):{
			CPUCheckDMA(3, 2);     //Serve DMA1 FIFO
		}
		break;
		
		//FIFO DMA Sound B IRQ from ARM7 Sound
		case(0xc1710004):{
			CPUCheckDMA(3, 4);     //Serve DMA2 FIFO
		}
		break;
		
		//debug
		case(0xc0700100):{
			SendArm7Command(0xc0700101,0x0,0x0,0x0);
		}
		break;
		
		//debug print
		case(0xc0700101):{ 
        
			//pu_Disable(); //disable pu while configurating pu
			
			asm("nop");
			asm("nop");
			asm("nop");
			asm("nop");
			asm("nop");
			asm("nop");
			asm("nop");
			asm("nop");
			
			iprintf("\n \n [Timer %d] dma Source A %x %x \n ",((GBAEMU4DS_IPC->SOUNDCNT_H>>10)&1),GBAEMU4DS_IPC->DM1SAD_H,GBAEMU4DS_IPC->DM1SAD_L);
			iprintf("\n \n dma Dest A %x %x \n ",GBAEMU4DS_IPC->DM1DAD_H,GBAEMU4DS_IPC->DM1DAD_L);
			iprintf("dma A : %x %x %x %x \n ",GBAEMU4DS_IPC->fifodmasA[3],GBAEMU4DS_IPC->fifodmasA[2],GBAEMU4DS_IPC->fifodmasA[1],GBAEMU4DS_IPC->fifodmasA[0]);
			iprintf("DMA1CNT_H: %x \n ",GBAEMU4DS_IPC->DM1CNT_H);
			
			int reg;
			
			for(reg = 0; reg <= 15; reg++) {
				iprintf("R%d=%X t", (int)reg, (unsigned int)exRegs[reg]);
			}
			
			
			iprintf("\n \n [Timer %d] dma Source B %x %x \n ",((GBAEMU4DS_IPC->SOUNDCNT_H>>14)&1),GBAEMU4DS_IPC->DM2SAD_H,GBAEMU4DS_IPC->DM2SAD_L);
			iprintf("\n \n dma Dest B %x %x \n ",GBAEMU4DS_IPC->DM2DAD_H,GBAEMU4DS_IPC->DM2DAD_L);
			iprintf("dma B : %x %x %x %x \n ",GBAEMU4DS_IPC->fifodmasB[3],GBAEMU4DS_IPC->fifodmasB[2],GBAEMU4DS_IPC->fifodmasB[1],GBAEMU4DS_IPC->fifodmasB[0]);
			iprintf("DMA2CNT_H: %x \n ",GBAEMU4DS_IPC->DM2CNT_H);
			
			iprintf("TM0CNT_L %x / TM0CNT_H %x \n",GBAEMU4DS_IPC->TM0CNT_L,GBAEMU4DS_IPC->TM0CNT_H);
			iprintf("TM1CNT_L %x / TM1CNT_H %x \n",GBAEMU4DS_IPC->TM1CNT_L,GBAEMU4DS_IPC->TM1CNT_H);
			
			asm("nop");
			asm("nop");
			asm("nop");
			asm("nop");
			asm("nop");
			asm("nop");
			asm("nop");
			asm("nop");
			//pu_Enable();
			
		}
		break;
		
		case(0x3F00BEEF):{
			VblankHandler();
		}
		break;
		
		
		case(0x4000BEEF):{
			//while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY);
			iprintf("Debug: arm7 %08X\r\n",command2);
		}
		break;
		
		case(0x4100BEEF):{
			frameasyncsync();
		}
		break;
		
		case(0x4200BEEF):{
			if(savePath[0] == 0)sprintf(savePath,"%s.sav",szFile);
			CPUWriteBatteryFile(savePath);
		}
		break;
		
		case(0x4300BEEF):{
			pausemenue();
		}
		break;
		
		case(0xc4740005):{
			UPDATE_REG(command2, (u32)(command3&0xffff));
		}
		break;
		
		//debug
		case(0xc6760000):{
			//iprintf("DMASA IF IRQ \n");	//works
			//iprintf("DMASB IF IRQ \n");	//works
			//iprintf("VBLANK driven sound \n");	//works
			
			iprintf("TIMER1 IRQ \n");	//works
			
			//volatile char charbuf[64];
			//memcpy((u32*)&charbuf[0],(u32*)&GBAEMU4DS_IPC->charbuf[0],(int)64);
			//iprintf("%s \n",charbuf);
		}
		break;
		
		
		default:{
		
		/*
		//gbaemu4ds SOUND driver 

#ifdef unsecamr7com
			if(command1 < 0x10000000)		
#else
			if(command1 < 0x8000000 && command1 > 0x2000000)
#endif
			{
	
				register u32 temp_dat = 0;
					 if((command1 & 0x3) == 1)temp_dat = 0x10;
				else if((command1 & 0x3) == 2)temp_dat = 0x50;
				else if((command1 & 0x3) == 3)temp_dat = 0x60;
				memcpy((void*)((u32)&arm7exchangefild[0] + temp_dat),(void*)(command1 & ~0x3),0x10);
			}
			else
			{
				iprintf("error rec %08X %08X\r\n",command1,REG_IPC_FIFO_CR);
				while(1); //stop to prevent dammage
			}
			
			
			*/
			
			iprintf("error rec %08X %08X\r\n",command1,REG_IPC_FIFO_CR);
			while(1); //stop to prevent dammage
			
		}
		break;
    }
}