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
void HandleFifo() {
	volatile uint32 command1 = 0, command2 = 0, command3 = 0, command4 = 0;
	while(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)){
		if (!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)){
			command1 = (uint32)REG_IPC_FIFO_RX;
		}
		
		if (!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)){
			command2 = (uint32)REG_IPC_FIFO_RX;
		}
		
		if (!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)){
			command3 = (uint32)REG_IPC_FIFO_RX;
		}
		
		if (!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)){
			command4 = (uint32)REG_IPC_FIFO_RX;
		}
		
		//gbaemu4ds SOUND driver
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
			//while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY);
			//void * destination = (void*)REG_IPC_FIFO_RX;
			void * destination = (void*)command2;
			memcpy(destination,(u32*)command1,0x10);
#endif
		}
		else
		{
			switch (command1) {
				case (ARM9_DOGBASHADOWCOPY):{
					struct sIPCSharedGBA* sIPCSharedGBAInst = GetsIPCSharedGBA();
					sIPCSharedGBAInst->GBAIE = IE;
					sIPCSharedGBAInst->GBAIF = IF;
					sIPCSharedGBAInst->GBAIME = IME;
					sIPCSharedGBAInst->DM0SAD_L = DM0SAD_L;
					sIPCSharedGBAInst->DM0SAD_H = DM0SAD_H;
					sIPCSharedGBAInst->DM0DAD_L = DM0DAD_L;
					sIPCSharedGBAInst->DM0DAD_H = DM0DAD_H;
					sIPCSharedGBAInst->DM0CNT_L = DM0CNT_L;
					sIPCSharedGBAInst->DM0CNT_H = DM0CNT_H;
					sIPCSharedGBAInst->DM1SAD_L = DM1SAD_L;
					sIPCSharedGBAInst->DM1SAD_H = DM1SAD_H;
					sIPCSharedGBAInst->DM1DAD_L = DM1DAD_L;
					sIPCSharedGBAInst->DM1DAD_H = DM1DAD_H;
					sIPCSharedGBAInst->DM1CNT_L = DM1CNT_L;
					sIPCSharedGBAInst->DM1CNT_H = DM1CNT_H;
					sIPCSharedGBAInst->DM2SAD_L = DM2SAD_L;
					sIPCSharedGBAInst->DM2SAD_H = DM2SAD_H;
					sIPCSharedGBAInst->DM2DAD_L = DM2DAD_L;
					sIPCSharedGBAInst->DM2DAD_H = DM2DAD_H;
					sIPCSharedGBAInst->DM2CNT_L = DM2CNT_L;
					sIPCSharedGBAInst->DM2CNT_H = DM2CNT_H;
					sIPCSharedGBAInst->DM3SAD_L = DM3SAD_L;
					sIPCSharedGBAInst->DM3SAD_H = DM3SAD_H;
					sIPCSharedGBAInst->DM3DAD_L = DM3DAD_L;
					sIPCSharedGBAInst->DM3DAD_H = DM3DAD_H;
					sIPCSharedGBAInst->DM3CNT_L = DM3CNT_L;
					sIPCSharedGBAInst->DM3CNT_H = DM3CNT_H;
					sIPCSharedGBAInst->TM0CNT_L = TM0CNT_L;
					sIPCSharedGBAInst->TM1CNT_L = TM1CNT_L;
					sIPCSharedGBAInst->TM2CNT_L = TM2CNT_L;
					sIPCSharedGBAInst->TM3CNT_L = TM3CNT_L;
					sIPCSharedGBAInst->TM0CNT_H = TM0CNT_H;
					sIPCSharedGBAInst->TM1CNT_H = TM1CNT_H;
					sIPCSharedGBAInst->TM2CNT_H = TM2CNT_H;
					sIPCSharedGBAInst->TM3CNT_H = TM3CNT_H;
					sIPCSharedGBAInst->SOUNDBIAS = SOUNDBIAS;
					
					setShadowCopyStatus(true);
				}
				break;
				case(ARM7_DOGBASHADOWUPDATE):{
					
				}
				break;
				case(FIFO_DEBUG):{
					//printf("FIFO ok");	//test the FIFO if NDS cmds and GBA cmds work .. ok they work
				}
				break;
				
				//swi 0x3 gba call from arm7
				case(FIFO_SWIGBA_FROM_ARM7):{
					enterGBASleepMode();
				}
				break;
				
				//sleepmode: swi 0x3 gba
				case(FIFO_SWI_SLEEPMODE_PHASE2):{
					//coto: sleep mode from GBA request SWI
					backup_mpu_setprot();
					
					while(Getarm7Sleep() == true){
						ichflyswiHalt();
					}
					
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
					
				}
				break;
				
				case(0x3F00BEEF):{
					VblankHandler();
				}
				break;
				
				case(0x4000BEEF):{
					//while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY);
					iprintf("arm7 %08X\r\n",REG_IPC_FIFO_RX);
					//continue;
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
				
				//arm7 wants to WifiSync
				case(WIFI_SYNC_GBAEMU4DS):{
					//Wifi_Sync();	//can't, nifi frame is handled during hblank
				}
				break;
			}
		}
	}
	
	if(REG_IPC_FIFO_CR & IPC_FIFO_ERROR)
	{
		//clear fifo inmediately
		REG_IPC_FIFO_CR |= (1<<3);
		REG_IPC_FIFO_CR |= IPC_FIFO_ERROR;
	}
	
	REG_IF = IRQ_FIFO_NOT_EMPTY;
}
