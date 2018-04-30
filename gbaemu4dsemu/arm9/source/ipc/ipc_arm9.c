#include "ipc_arm9.h"

#include <nds.h>
#include <nds/memory.h>
#include <nds/ndstypes.h>
#include <nds/memory.h>
#include <nds/bios.h>
#include <nds/system.h>
#include <nds/arm9/math.h>
#include <nds/arm9/video.h>
#include <nds/arm9/videoGL.h>
#include <nds/arm9/trig_lut.h>
#include <nds/arm9/sassert.h>
#include <filesystem.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <fat.h>
#include <dirent.h>

#include "../ichflysettings.h"
#include "../main.h"

//touchscreen ARM9
#include "../irqstuf.arm.h"

//Shared memory region static object
#include "../../common/clock.h"
#include "../../common/gba_ipc.h"

//wifi 
#include "../dswifi_arm9/wifi_arm9.h"
#include "../nifi/multi.h"
#include "../httpstack/client_http_handler.h"

#include "../GBA.h"
#include "../Sound.h"
#include "../Util.h"
#include "../System.h"
#include "../cpumg.h"
#include "../bios.h"
#include "../disk_fs/file_browse.h"
#include "../mydebuger.h"
#include "../Cheats.h"

u8 recv_buf[0x1000] = {0}; //4K

//Coto:
__attribute__((section(".itcm")))
__attribute__ ((aligned(4)))
inline void handlefifo(){
	
	REG_IF = IRQ_FIFO_NOT_EMPTY;
	while(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)){
		volatile u32 value0 = REG_IPC_FIFO_RX;	//we already have arg0 otherwise this wouldnt be triggered
		volatile u32 value1 = 0;
		volatile u32 value2 = 0;
		volatile u32 value3 = 0;
		
		if (!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)){
			value1 = REG_IPC_FIFO_RX;
		}
		
		if (!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)){
			value2 = REG_IPC_FIFO_RX;
		}
		
		if (!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)){
			value3 = REG_IPC_FIFO_RX;
		}
		
		//iprintf("arm9: IRQ not empty received value: %x ! \n",(unsigned int)value0);

		//if there is a cmd to be executed..
		ipc9_handler(value0,value1,value2,value3);
		continue; //required: keep reading commands from the IPC RX Vector
	}
	
}

__attribute__((section(".itcm")))
__attribute__ ((aligned(4)))
inline void ipc9_handler(u32 command1,u32 command2,u32 command3,u32 command4){
	switch(command1){
		//#arm7 -> arm9 commands
		//power misc commands
    
		case(0xc0700100):{	//0x4200BEEF
			/*
			if(savePath[0] == 0){
			sprintf(savePath,"%s.sav",szFile);
			}
			CPUWriteBatteryFile(savePath);
			*/

			//detect GBAMode through CP15
			u32 saved_cp15_bits = cpu_GetCP15Cnt();
			if( saved_cp15_bits & (1<<0) ){
				pu_Disable();
			}

			//detect if cpu was downgraded to v4 so we restore compiler's ARMv5mode
			if( saved_cp15_bits & (1<<15) ){
				armv5_mode();
			}

			const char * str_params = "GET /dswifi/example1.php HTTP/1.1\r\nHost: www.akkit.org\r\nUser-Agent: Nintendo DS\r\n\r\n";
			send_response((char*)str_params);
			//SGIP_ERROR(a)
			//extern int recv(int socket, void * data, int recvlength, int flags);

			//recv causes freezes
			static u8 buf[256];
			int recvd_len = 0;
			while( ( recvd_len = recv( client_http_handler_context.socket_id, (u8*)recv_buf, 255, 0 ) ) != 0 ) { // if recv returns 0, the socket has been closed.
				if(recvd_len>0) { // data was received!
					recv_buf[recvd_len] = 0; // null-terminate
					//iprintf((const char*)buf);
					break;//when get-response was acquired, exit.
				}
			}

			//restore GBAMode/NDSMode through CP15
			cpu_SetCP15Cnt(saved_cp15_bits);
		}
		break;
		//h-blank arm9
		case(0xc0700107):{
			HblankHandler_gba();
		}
		break;
		//v-blank arm9
		case(0xc0700103):{
			VblankHandler_gba(); //0x4500BEEF /0x3F00BEEF (arm7)
		}
		break;
		//sleepmode//0x4600BEEF
		case(0xc0700104):{
			//coto: sleep mode from GBA request SWI
			backup_mpu_setprot();
				
			//call arm7 sleep with one of IRQs to wake up from
			SendArm7Command(0xc0700102,IRQ_LID,0x0,0x0);
				
			gba4ds_swiHalt();
				
			asm("nop");
			asm("nop");
			asm("nop");
			asm("nop");
			asm("nop");
			asm("nop");
			asm("nop");
			asm("nop");
		
			restore_mpu_setprot();
				
			asm("nop");
			asm("nop");
			asm("nop");
			asm("nop");
			asm("nop");
			asm("nop");
			asm("nop");
			asm("nop");
			
			pu_Enable(); //activate MPU regardless 
		}
		break;
		
		case(0xc0700105):{
			pu_Disable(); //disable pu while configurating pu        
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
			iprintf("dma A : %x %x %x %x \n ",GBAEMU4DS_IPC->FIFO_A_H[1],GBAEMU4DS_IPC->FIFO_A_H[0],GBAEMU4DS_IPC->FIFO_A_L[1],GBAEMU4DS_IPC->FIFO_A_L[0]);
			
			int reg;
			
			for(reg = 0; reg <= 15; reg++) {
				iprintf("R%d=%X t", (int)reg, (unsigned int)exRegs[reg]);
			}
			
			
			iprintf("\n \n [Timer %d] dma Source B %x %x \n ",((GBAEMU4DS_IPC->SOUNDCNT_H>>14)&1),GBAEMU4DS_IPC->DM2SAD_H,GBAEMU4DS_IPC->DM2SAD_L);
			iprintf("\n \n dma Dest B %x %x \n ",GBAEMU4DS_IPC->DM2DAD_H,GBAEMU4DS_IPC->DM2DAD_L);
			iprintf("dma B : %x %x %x %x \n ",GBAEMU4DS_IPC->FIFO_B_H[1],GBAEMU4DS_IPC->FIFO_B_H[0],GBAEMU4DS_IPC->FIFO_B_L[1],GBAEMU4DS_IPC->FIFO_B_L[0]);
			
			iprintf("TM0CNT %x \n",GBAEMU4DS_IPC->TM0CNT);
			iprintf("TM1CNT %x \n",GBAEMU4DS_IPC->TM1CNT);
			
			asm("nop");
			asm("nop");
			asm("nop");
			asm("nop");
			asm("nop");
			asm("nop");
			asm("nop");
			asm("nop");
			pu_Enable();
		}
		break;
		
		case(0xc0700106):{ //0x4800BEEF
			vcounthandler();
		}
		break;
		
		//WIFI cmds
	
		//coto: the way interrupts from ARM7 are treated here are tested. The below procedure executes ARM9 compiled code when you are in GBA Mode
		case(0xc1710100):{
			//detect GBAMode through CP15
			u32 saved_cp15_bits = cpu_GetCP15Cnt();
			if( saved_cp15_bits & (1<<0) ){
				pu_Disable();
			}
			
			//detect if cpu was downgraded to v4 so we restore compiler's ARMv5mode
			if( saved_cp15_bits & (1<<15) ){
				armv5_mode();
			}
			
			
			Wifi_Sync(); //calls Wifi_Update(); on ARM9
			
			//restore GBAMode/NDSMode through CP15
			cpu_SetCP15Cnt(saved_cp15_bits);
		}
		break;
		
		//misc
    
		//raise FIFO IRQ from the other core
		case(0xc3730100):{
			SendArm7Command(0xc3730101,0x0,0x0,0x0);
		}
		break;
	
		//acknowledge event, does nothing but to receive the above interrupt
		case(0xc3730101):{
		}
		break;
		
		//raises arm9_synctoarm7(); (sync WIFI stacks with ARM7)
		case(0xc3730102):{
			SendArm7Command(0xc3730103,0x0,0x0,0x0);
		}
		break;
		
		//receives the signal for syncing WIFI stacks from the other ARM core
		case(0xc3730103):{
			arm9_synctoarm7();
		}
		break;
		
		//DMA/Timer process
    
    
		// GBA Opcodes
		//Write data from ARM7 to GBA
		case(0xc4740001):{
			CPUWriteMemory(command2,command3);
		}
		break;
		
		case(0xc4740002):{
			CPUWriteHalfWord(command2,(u16)(command3&0xffff));
		}
		break;
    
		case(0xc4740003):{
			CPUWriteByte(command2,(u8)(command3&0xff));
		}
		break;
		//Trigger GBA DMA
		case(0xc4740004):{
			CPUCheckDMA(command2,command3);
		}
		break;
		
		//GBA MAP IO update
		case(0xc4740005):{
			UPDATE_REG(command2, (u16)command3);
		}
		break;
		case(0xc4740006):{
			//iprintf("timer irq! \n");
		}
		break;
		
		//misc arm9 utils
	
		//set ppu's vcounter irq
		case(0xc4740100):{
			set_ppu_vcountline_irq(command2);	//u8 vcount_line_irq
		}
		break;	
	}
	
}