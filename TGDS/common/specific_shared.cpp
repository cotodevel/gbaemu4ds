
/*

			Copyright (C) 2017  Coto
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301
USA

*/

#include "common_shared.h"
#include "specific_shared.h"
#include "dsregs.h"
#include "dsregs_asm.h"
#include "InterruptsARMCores_h.h"

#ifdef ARM7
#include <string.h>

#include "main.h"
#include "wifi_arm7.h"
#include "spifw.h"
#include "dmaIO.h"

#endif

#ifdef ARM9

#include <stdbool.h>
#include "main.h"
#include "wifi_arm9.h"

#endif


//inherits what is defined in: common_shared.c
#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void HandleFifoNotEmptyWeakRef(uint32 cmd1,uint32 cmd2,uint32 cmd3,uint32 cmd4){
	
	switch (cmd1) {
		
		//NDS7: uses NDS IPC FIFO as a layer from GBA IO @ ARM9
		#ifdef ARM7
		//debug
		case(0xc0700100):{
			SendMultipleWordACK(0xc0700101,0x0,0x0,0x0);
		}
		break;
		
		//vcounter
		case(0xc1710000):{
			
		}
		break;
		
		//fifo test
		case(0xc1710001):{
			SendMultipleWordACK(0xc1710002,0x0,0x0,0x0);
		}
		break;
		
		//FIFO DMA SA update FIFO_INTERNAL_BUFFER(A)
		case(0xc1710004):{
			SendMultipleWordACK(0xc6760000,0x0,0x0,0x0);
		}
		break;
		
		//FIFO DMA SB update FIFO_INTERNAL_BUFFER(B)
		case(0xc1710005):{
			SendMultipleWordACK(0xc6760001,0x0,0x0,0x0);
		}
		break;
		
		case(0xc3730000):{
			iowrite(cmd2,cmd3);
		}
		break;
		#endif
		
		//NDS9: uses NDS IPC FIFO as a layer from GBA IO @ ARM7
		#ifdef ARM9
		
		//vcounter
		case(0xc1710000):{
			
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
			
			printf("\n \n [Timer %d] dma Source A %x %x \n ",((GBAEMU4DS_IPC->SOUNDCNT_H>>10)&1),GBAEMU4DS_IPC->DM1SAD_H,GBAEMU4DS_IPC->DM1SAD_L);
			printf("\n \n dma Dest A %x %x \n ",GBAEMU4DS_IPC->DM1DAD_H,GBAEMU4DS_IPC->DM1DAD_L);
			printf("dma A : %x %x %x %x \n ",GBAEMU4DS_IPC->fifodmasA[3],GBAEMU4DS_IPC->fifodmasA[2],GBAEMU4DS_IPC->fifodmasA[1],GBAEMU4DS_IPC->fifodmasA[0]);
			printf("DMA1CNT_H: %x \n ",GBAEMU4DS_IPC->DM1CNT_H);
			
			int reg;
			
			for(reg = 0; reg <= 15; reg++) {
				printf("R%d=%X t", (int)reg, (unsigned int)exRegs[reg]);
			}
			
			
			printf("\n \n [Timer %d] dma Source B %x %x \n ",((GBAEMU4DS_IPC->SOUNDCNT_H>>14)&1),GBAEMU4DS_IPC->DM2SAD_H,GBAEMU4DS_IPC->DM2SAD_L);
			printf("\n \n dma Dest B %x %x \n ",GBAEMU4DS_IPC->DM2DAD_H,GBAEMU4DS_IPC->DM2DAD_L);
			printf("dma B : %x %x %x %x \n ",GBAEMU4DS_IPC->fifodmasB[3],GBAEMU4DS_IPC->fifodmasB[2],GBAEMU4DS_IPC->fifodmasB[1],GBAEMU4DS_IPC->fifodmasB[0]);
			printf("DMA2CNT_H: %x \n ",GBAEMU4DS_IPC->DM2CNT_H);
			
			printf("TM0CNT_L %x / TM0CNT_H %x \n",GBAEMU4DS_IPC->TM0CNT_L,GBAEMU4DS_IPC->TM0CNT_H);
			printf("TM1CNT_L %x / TM1CNT_H %x \n",GBAEMU4DS_IPC->TM1CNT_L,GBAEMU4DS_IPC->TM1CNT_H);
			
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
				printf("error rec %08X %08X\r\n",command1,REG_IPC_FIFO_CR);
				while(1); //stop to prevent dammage
			}
			
			
			*/
			
			//printf("error rec %08X %08X\r\n",command1,REG_IPC_FIFO_CR);
			//while(1); //stop to prevent dammage
			
		}
		break;
		
		#endif
	}
	
}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void HandleFifoEmptyWeakRef(uint32 cmd1,uint32 cmd2,uint32 cmd3,uint32 cmd4){
}

//project specific stuff
