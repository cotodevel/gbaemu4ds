#include <nds.h>
#include <stdio.h>
#include "ichflysettings.h"
#include "../../common/cpuglobal.h"
#include "../../common/gba_ipc.h"
#include <filesystem.h>
#include <fat.h>
#include <dirent.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <nds/memory.h>//#include <memory.h> ichfly
#include <nds/ndstypes.h>
#include <nds/memory.h>
#include <nds/bios.h>
#include <nds/system.h>
#include <nds/arm9/math.h>
#include <nds/arm9/video.h>
#include <nds/arm9/videoGL.h>
#include <nds/arm9/trig_lut.h>
#include <nds/arm9/sassert.h>
#include "ichflysettings.h"
#include "GBA.h"
#include "Sound.h"
#include "Util.h"
#include "getopt.h"
#include "System.h"
#include "cpumg.h"
#include "bios.h"
#include "mydebuger.h"
#include "arm7sound.h"
#include "Cheats.h"
#include "main.h"

#ifdef anyarmcom
u32 recDMA1 = 0;
u32 recDMA2 = 0;
u32 recdir = 0;
u32 recdel = 0;
#endif

int cheatsCheckKeys();
int counttrans = 0;
int dasistnurzumtesten = 0;

//ori
/*
__attribute__((section(".itcm")))
void arm7dmareq()
{
#ifdef advanced_irq_check
	REG_IF = IRQ_FIFO_NOT_EMPTY;
#endif
#ifdef anyarmcom
recdir++;
#endif
	while(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)) //handel all cmds
	{
#ifdef anyarmcom
recdel++;
#endif
		//counttrans++;
		//iprintf("spirq %x \r\n",spirq);
		int i = 0;
		u32 src = REG_IPC_FIFO_RX;
#ifdef unsecamr7com
		if(src < 0x10000000)		
#else
		if(src < 0x8000000 && src > 0x2000000)
#endif
		{
#ifdef neu_sound_16fifo
#ifdef anyarmcom
			register u32 temp_dat = 0;
			     if((src & 0x3) == 1)
				 {
					 temp_dat = 0x10;
					recDMA1++;
				 }
			else if((src & 0x3) == 2)
			{
				temp_dat = 0x50;
				recDMA2++;
			}
			else if((src & 0x3) == 3)
			{
				temp_dat = 0x60;
				recDMA2++;
			}
			else recDMA1++;
			memcpy((void*)(arm7amr9buffer + temp_dat),(void*)(src & ~0x3),0x10);
#else
			register u32 temp_dat = 0;
			     if((src & 0x3) == 1)temp_dat = 0x10;
			else if((src & 0x3) == 2)temp_dat = 0x50;
			else if((src & 0x3) == 3)temp_dat = 0x60;
			memcpy((void*)(arm7amr9buffer + temp_dat),(void*)(src & ~0x3),0x10);
#endif
#else
			while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY);
			void * destination = (void*)REG_IPC_FIFO_RX;
			memcpy(destination,(u32*)src,0x10);
#endif
		}
		else
		{
			if(src == 0x3F00BEEF)
			{
				VblankHandler();
				continue;
			}

			if(src == 0x4000BEEF)
			{
				while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY);
				iprintf("arm7 %08X\r\n",REG_IPC_FIFO_RX);
				continue;
			}
			if(src == 0x4200BEEF)
			{
				if(savePath[0] == 0)sprintf(savePath,"%s.sav",szFile);
				CPUWriteBatteryFile(savePath);
				//REG_IPC_FIFO_TX = 0;
				continue;
			}
			if(src == 0x4300BEEF)
			{
				pausemenue();
				continue;
			}
			iprintf("error rec %08X %08X\r\n",src,REG_IPC_FIFO_CR);
			while(1); //stop to prevent dammage
		}
		//iprintf("e %08X\r\n",REG_IPC_FIFO_CR);
	}
}
*/


void arm7dmareqandcheat()
{
#ifdef advanced_irq_check
	REG_IF = IRQ_FIFO_NOT_EMPTY;
#endif
	while(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)) //handel all cmds
	{
		//iprintf("spirq %x \r\n",spirq);
		int i = 0;
		u32 src = REG_IPC_FIFO_RX;
		//iprintf("i %08X\r\n",src);
#ifdef unsecamr7com
		if(src < 0x10000000)		
#else
		if(src < 0x8000000 && src > 0x2000000)
#endif
		{
#ifdef neu_sound_16fifo
			register u32 temp_dat = 0;
			     if((src & 0x3) == 1)temp_dat = 0x10;
			else if((src & 0x3) == 2)temp_dat = 0x50;
			else if((src & 0x3) == 3)temp_dat = 0x60;
			memcpy((void*)(arm7amr9buffer + temp_dat),(void*)(src & ~0x3),0x10);
#else
			while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY);
			void * destination = (void*)REG_IPC_FIFO_RX;
			memcpy(destination,(u32*)src,0x10);
#endif
		}
		else
		{
			if(src == 0x3F00BEEF)
			{
				cheatsCheckKeys();
				VblankHandler();
			}
			if(src == 0x4200BEEF)
			{
				if(savePath[0] == 0)sprintf(savePath,"%s.sav",szFile);
				CPUWriteBatteryFile(savePath);
				SendArm7Command(0,0);
			}
		}
		//if(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY))arm7dmareqandcheat();
	}

 }