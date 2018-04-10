#include "typedefsTGDS.h"
#include "dsregs.h"
#include "dsregs_asm.h"

#include "ipcfifoTGDS.h"
#include "specific_shared.h"

#include <stdio.h>
#include "ichflysettings.h"
#include "main.h"

#include <stdio.h>
#include <dirent.h>
#include <stdarg.h>
#include <string.h>

#include "ichflysettings.h"
#include "GBA.h"
#include "Sound.h"
#include "Util.h"
#include "getopt.h"
#include "System.h"
#include "cpumg.h"
#include "GBAinline.h"
#include "biosTGDS.h"
#include "mydebuger.h"
#include "arm7sound.h"
#include "main.h"

#include <stdlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "dsregs.h"
#include "dsregs_asm.h"

#include "fsfatlayerTGDSLegacy.h"
#include "fileHandleTGDS.h"
#include "InterruptsARMCores_h.h"
#include "specific_shared.h"
#include "ff.h"
#include "memoryHandleTGDS.h"
#include "reent.h"
#include "sys/types.h"
#include "consoleTGDS.h"
#include "utilsTGDS.h"
#include "devoptab_devices.h"
#include "posixHandleTGDS.h"
#include "about.h"
#include "xenofunzip.h"


/*
#ifdef anyarmcom
u32 recDMA1 = 0;
u32 recDMA2 = 0;
u32 recdir = 0;
u32 recdel = 0;
#endif
*/


#include "Cheats.h"

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
		//printf("SPtoload %x sptemp %x\r\n",SPtoload,SPtemp);
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
			memcpy((void*)((u32)&arm7exchangefild[0] + temp_dat),(void*)(src & ~0x3),0x10);
#else
			register u32 temp_dat = 0;
			     if((src & 0x3) == 1)temp_dat = 0x10;
			else if((src & 0x3) == 2)temp_dat = 0x50;
			else if((src & 0x3) == 3)temp_dat = 0x60;
			memcpy((void*)((u32)&arm7exchangefild[0] + temp_dat),(void*)(src & ~0x3),0x10);
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
				printf("arm7 %08X\r\n",REG_IPC_FIFO_RX);
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
			printf("error rec %08X %08X\r\n",src,REG_IPC_FIFO_CR);
			while(1); //stop to prevent dammage
		}
		//printf("e %08X\r\n",REG_IPC_FIFO_CR);
	}
}
*/

#ifdef arm7dmapluscheats
void arm7dmareqandcheat()
{
#ifdef advanced_irq_check
	REG_IF = IRQ_FIFO_NOT_EMPTY;
#endif
	while(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)) //handel all cmds
	{
		//printf("SPtoload %x sptemp %x\r\n",SPtoload,SPtemp);
		int i = 0;
		u32 src = REG_IPC_FIFO_RX;
		//printf("i %08X\r\n",src);
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
			memcpy((void*)((u32)&arm7exchangefild[0] + temp_dat),(void*)(src & ~0x3),0x10);
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
 #endif
 