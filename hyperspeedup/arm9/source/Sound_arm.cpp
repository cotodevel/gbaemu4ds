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

#include "ichflysettings.h"
#include "main.h"

//Shared memory region static object
#include "../../common/clock.h"
#include "../../common/gba_ipc.h"

#include "GBA.h"
#include "Sound.h"
#include "Util.h"
#include "getopt.h"
#include "System.h"
#include "cpumg.h"
#include "bios.h"
#include "file_browse.h"
#include "mydebuger.h"
#include "Cheats.h"


u32 arm7arm9buffer = 0;
int cnt_cmd = 0;

void arm7dmareq()
{
#ifdef advanced_irq_check
	REG_IF = IRQ_FIFO_NOT_EMPTY;
#endif
	while(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)) //handel all cmds
	{
		//cnt_cmd++;
		//iprintf("SPtoload %x sptemp %x\r\n",SPtoload,SPtemp);
		u32 src = REG_IPC_FIFO_RX;
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
			memcpy((void*)(arm7arm9buffer + temp_dat),(void*)(src & ~0x3),0x10);
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
				//iprintf("arm7 %08X\r\n",(unsigned int)REG_IPC_FIFO_RX);
				continue;
			}
			if(src == 0x4100BEEF)
			{
				frameasyncsync();
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
			//iprintf("error rec %08X %08X\r\n",src,REG_IPC_FIFO_CR);
			while(1); //stop to prevent dammage
		}
		//iprintf("e %08X\r\n",REG_IPC_FIFO_CR);
	}

 }

void arm7dmareqandcheat()
{
#ifdef advanced_irq_check
	REG_IF = IRQ_FIFO_NOT_EMPTY;
#endif
	while(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)) //handel all cmds
	{
		//iprintf("SPtoload %x sptemp %x\r\n",SPtoload,SPtemp);
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
			memcpy((void*)(arm7arm9buffer + temp_dat),(void*)(src & ~0x3),0x10);
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
			if(src == 0x4100BEEF)
			{
				frameasyncsync();
			}
			if(src == 0x4200BEEF)
			{
				if(savePath[0] == 0)sprintf(savePath,"%s.sav",szFile);
				CPUWriteBatteryFile(savePath);
				REG_IPC_FIFO_TX = 0;
			}
		}
		//if(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY))arm7dmareqandcheat();
	}

 }