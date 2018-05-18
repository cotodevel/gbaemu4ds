#include "fifo_handler.h"

#include <nds.h>
#include <nds/ndstypes.h>
#include <string.h>

#include "../ipc/touch_ipc.h"
#include "../wireless/wifi_arm7.h"

#include "../../../common/gba_ipc.h"
#include "../../../common/cpuglobal.h"

#include "../sound.h"
#include "../main.h"

bool autodetectdetect = false;
void newvalwrite(u32 addr, u32 val, u32 cmd0, u32 command3, u32 command4)	//cmd0 == addr but 0xc0000000 part kept
{
	bool NDSCmd = false;
	switch(cmd0){
		case(FIFO_DEBUG):{
			SendArm9Command(FIFO_DEBUG,0x0,0x0,0x0);
			NDSCmd = true;
		}
		//raise sleepmode from arm9 with custom IRQs
		case(FIFO_SWI_SLEEPMODE_PHASE1):{
			lid_closing_handler();
			NDSCmd = true;
		}
		break;
		//arm9 wants to WifiSync
		case(WIFI_SYNC_GBAEMU4DS):{
			Wifi_Sync();
			NDSCmd = true;
		}
		break;
		//arm9 wants to send a WIFI context block address / userdata is always zero here
		case(WIFI9_SETUP_GBAEMU4DS):{
			//	wifiAddressHandler( void * address, void * userdata )
			wifiAddressHandler((Wifi_MainStruct *)(u32)val, 0);
			NDSCmd = true;
		}
		break;
		// Deinit WIFI
		case((uint32)DSWIFI_DEINIT_WIFI):{
			DeInitWIFI();
			NDSCmd = true;
		}
		break;
		
		//emulator DMA fifo write (do update internal counter) (not gamecode)
		case(WRITEWORD_DMAFIFO_A):{
			bool channelA = true;
			updateInternalDMAFIFO(channelA, val, 0xa0, command4);	//newvalwrite(command1 & ~0xc0000000, command2, command1, command3, command4)
		}
		break;
		
		//emulator DMA fifo write (do update internal counter) (not gamecode)
		case(WRITEWORD_DMAFIFO_B):{
			bool channelA = false;
			updateInternalDMAFIFO(channelA, val, 0xa0, command4);	//newvalwrite(command1 & ~0xc0000000, command2, command1, command3, command4)
		}
		break;
		
	}
	
	if(NDSCmd == false){
		struct sIPCSharedGBA* sIPCSharedGBAInst = GetsIPCSharedGBA();
		switch(addr)
		{
			//(NR50, NR51) - Channel L/R Volume/Enable (R/W)
			case 0x80:
				sIPCSharedGBAInst->SOUNDCNT_L = (val&0xffff);
				updatevol();
			break;
			
			//4000082h - sIPCSharedGBAInst->SOUNDCNT_H (GBA only) - DMA Sound Control/Mixing (R/W)
			case 0x82:{
				if(!(val & (1<<10)))
				{
					DMA_A_TIMERSEL = 0;
					tacktgeber_sound_FIFO_DMA_A = 1;	//same as above
				}
				else
				{
					DMA_A_TIMERSEL = 1;
					tacktgeber_sound_FIFO_DMA_A = 0;	//same as above
				}
				
				if(!(val & (1<<14)))
				{
					DMA_B_TIMERSEL = 0;
					tacktgeber_sound_FIFO_DMA_B = 1;	//same as above
				}
				else
				{
					DMA_B_TIMERSEL = 1;
					tacktgeber_sound_FIFO_DMA_B = 0;	//same as above
				}
				
				//Reset FIFO
				if(val & (1<<11))
				{
					//void * memset ( void * ptr, int value, size_t num );
					memset((u8*)&sIPCSharedGBAInst->fifodmasA[0],0,(int)INTERNAL_FIFO_SIZE);
					sIPCSharedGBAInst->fifoA_offset=0;
					sIPCSharedGBAInst->FIFO_A_L[0] = 0;
					sIPCSharedGBAInst->FIFO_A_L[1] = 0;
					sIPCSharedGBAInst->FIFO_A_H[0] = 0;
					sIPCSharedGBAInst->FIFO_A_H[1] = 0;
				}
				
				if(val & (1<<15))
				{
					memset((u8*)&sIPCSharedGBAInst->fifodmasB[0],0,(int)INTERNAL_FIFO_SIZE);
					sIPCSharedGBAInst->fifoB_offset=0;
					sIPCSharedGBAInst->FIFO_B_L[0] = 0;
					sIPCSharedGBAInst->FIFO_B_L[1] = 0;
					sIPCSharedGBAInst->FIFO_B_H[0] = 0;
					sIPCSharedGBAInst->FIFO_B_H[1] = 0;
				}
				
				sIPCSharedGBAInst->SOUNDCNT_H = val;
			}
			break;
			//SOUNDCNT_X (NR52) - Sound on/off (R/W)
			case 0x84:{
				if(val & 0x80)
					REG_SOUNDCNT |= 0x8000;
				else
					REG_SOUNDCNT &= ~0x8000;
					
				if(val & 0x80)REG_SOUNDCNT |= 0x8000;
				else
				{
					REG_SOUNDCNT &= ~0x8000;
				}
				sIPCSharedGBAInst->SOUNDCNT_X = (val&0x8f);
			}
			break;
			case 0x88:
				//Amplitude Resolution/Sampling Cycle is not supported so only Bias
				//it is better on the DS any way
				REG_SOUNDBIAS = val;
			break;
			
			//gamecode DMA fifo write (do update internal counter)
			//SOUND FIFO IO (8bit writes only?? check u16 case)
			//FIFO_A_L - Sound A FIFO, Data 0 and Data 1 (W) / plain fifo write (no update internal counter)
			case 0xa0:
			case 0xa1:
			//FIFO_A_H - Sound A FIFO, Data 2 and Data 3 (W) / plain fifo write (no update internal counter)
			case 0xa2:
			case 0xa3:
			{
				bool channelA = true;
				updateInternalDMAFIFO(channelA, val, (u32)(addr),command3);	//WRITEWORD_PLAINDMAFIFO_8 / WRITEWORD_PLAINDMAFIFO_16 / WRITEWORD_PLAINDMAFIFO_32
			}
			break;
			
			//FIFO_B_L - Sound B FIFO, Data 0 and Data 1 (W) / plain fifo write (no update internal counter)
			case 0xa4:
			case 0xa5:
			//FIFO_B_H - Sound B FIFO, Data 2 and Data 3 (W) / plain fifo write (no update internal counter)
			case 0xa6:
			case 0xa7:
			{
				bool channelA = false;
				updateInternalDMAFIFO(channelA, val, (u32)(addr),command3);	//WRITEWORD_PLAINDMAFIFO_8 / WRITEWORD_PLAINDMAFIFO_16 / WRITEWORD_PLAINDMAFIFO_32
			}
			break;
			
			//Timers
			case 0x100:{
				//Reload/Counter val
				sIPCSharedGBAInst->TM0CNT_L = (u16)val;
				if(DMA_A_TIMERSEL==0)
					SCHANNEL_TIMER(4) = sIPCSharedGBAInst->TM0CNT_L;
				if(DMA_B_TIMERSEL==0)
					SCHANNEL_TIMER(5) = sIPCSharedGBAInst->TM0CNT_L;
			}
			break;
			case 0x102:{
				//Timer Controller
				sIPCSharedGBAInst->TM0CNT_H = (u16)val;
				if(sIPCSharedGBAInst->TM0CNT_H & 0x80) //started timer
				{
					if(!(TIMER0_CR & TIMER_ENABLE)){
						timerStart(0, ClockDivider_1,sIPCSharedGBAInst->TM0CNT_H << 5, timer0interrupt_thread);
					}
				}
				//timerOnOffDelay|=1;
				//cpuNextEvent = cpuTotalTicks;            
			}
			break;
			case 0x104:{
				//Reload/Counter val
				sIPCSharedGBAInst->TM1CNT_L = (u16)val;
				if(DMA_A_TIMERSEL==1)
					SCHANNEL_TIMER(4) = sIPCSharedGBAInst->TM1CNT_L;
				if(DMA_B_TIMERSEL==1)
					SCHANNEL_TIMER(5) = sIPCSharedGBAInst->TM1CNT_L;
			}
			break;
			case 0x106:
			{   
				//Timer Controller
				sIPCSharedGBAInst->TM1CNT_H = (u16)val;
				//sIPCSharedGBAInst->timer1Value = value;
				//timerOnOffDelay|=2;
				//cpuNextEvent = cpuTotalTicks;
			}    
			break;
			case 0x108:
			{
				//Reload/Counter val
				//*(u16*)0x04000108 = (u16)val;
			}    
			break;
			case 0x10A:
			{
				//Timer Controller
				//*(u16*)0x0400010A = (u16)val;
				//sIPCSharedGBAInst->TM2CNT = (u16)val;
				
				//timerOnOffDelay|=4;
				//cpuNextEvent = cpuTotalTicks;
			}
			break;
			case 0x10C:
			{
				//Reload/Counter val
				//*(u16*)0x0400010C = (u16)val;
			}
			break;
			case 0x10E:
			{
				//Timer Controller
				//*(u16*)0x0400010E = (u16)val;
				//sIPCSharedGBAInst->TM3CNT = (u16)val;
				
				//sIPCSharedGBAInst->timer3Value = value;
				//timerOnOffDelay|=8;
				//cpuNextEvent = cpuTotalTicks;
			}
			break;
		  case 0xBC:
			sIPCSharedGBAInst->DM1SAD_L = val;
			break;
		  case 0xBE:
			sIPCSharedGBAInst->DM1SAD_H = val & 0x0FFF;
			break;
		  case 0xC0:
			sIPCSharedGBAInst->DM1DAD_L = val;
			break;
		  case 0xC2:
			sIPCSharedGBAInst->DM1DAD_H = val & 0x07FF;
			break;
		  case 0xC4:
			sIPCSharedGBAInst->DM1CNT_L = val & 0x3FFF;
			break;
		  case 0xC6:
			sIPCSharedGBAInst->DM1CNT_H = val;
			break;
		  case 0xC8:
			sIPCSharedGBAInst->DM2SAD_L = val;
			break;
		  case 0xCA:
			sIPCSharedGBAInst->DM2SAD_H = val & 0x0FFF;
			break;
		  case 0xCC:
			sIPCSharedGBAInst->DM2DAD_L = val;
			break;
		  case 0xCE:
			sIPCSharedGBAInst->DM2DAD_H = val & 0x07FF;
		  case 0xD0:
			sIPCSharedGBAInst->DM2CNT_L = val & 0x3FFF;
			break;
		  case 0xD2:
			sIPCSharedGBAInst->DM2CNT_H = val;
			break;
		  case setdmasoundbuff:
			  dmabuffer = val;
#ifdef anyarmcom
			amr7sendcom =  (u32*)(*(u32*)(dmabuffer));
			amr7senddma1 = (u32*)(*(u32*)(dmabuffer + 4));
			amr7senddma2 = (u32*)(*(u32*)(dmabuffer + 8));
			amr7recmuell = (u32*)(*(u32*)(dmabuffer + 12));
			amr7directrec = (u32*)(*(u32*)(dmabuffer + 16));
			amr7indirectrec = (u32*)(*(u32*)(dmabuffer + 20));
			amr7fehlerfeld = (u32*)(*(u32*)(dmabuffer + 24));
#endif
			soundbuffA = (u32*)(dmabuffer);
				SCHANNEL_SOURCE(4) = soundbuffA;
			  soundbuffB = (u32*)(dmabuffer + 0x50);
				SCHANNEL_SOURCE(5) = soundbuffB;
				break;
		case WaitforVblancarmcmd: //wait
			if(autodetectdetect  && (REG_KEYXY & 0x1) /* && (REG_VCOUNT > 160 || REG_VCOUNT < callline)*/ )
			{
				//REG_IPC_FIFO_TX = 0x4100BEEF; //send cmd 0x4100BEEF
				SendArm9Command(0x4100BEEF,0x0,0x0,0x0);
#ifdef anyarmcom
				*amr7sendcom = *amr7sendcom + 1;
#endif
			}
			break;
		case enableWaitforVblancarmcmdirq: //setauto
			autodetectdetect = true;
			break;
		case getarm7keys: //getkeys
			{
				touchPosition tempPos = {0};
				u16 keys= REG_KEYXY;
				if(!touchPenDown()) {
					keys |= KEY_TOUCH;
				} else {
					keys &= ~KEY_TOUCH;
				}
				touchReadXY(&tempPos);	
				//REG_IPC_FIFO_TX = 1; //send cmd 1
				//REG_IPC_FIFO_TX = keys;
				//REG_IPC_FIFO_TX = tempPos.px;
				//REG_IPC_FIFO_TX = tempPos.py;
				SendArm9Command((u32)1,(u32)keys,(u32)tempPos.px,(u32)tempPos.py);
			}
			break;
		case set_callline: //set callline
			arm7VCOUNTsyncline = val;
			break;
		default:{
				#ifdef anyarmcom
					*amr7recmuell = *amr7recmuell + 1;
					amr7fehlerfeld[currfehler] = addr;
					amr7fehlerfeld[currfehler + 1] = val;
					currfehler+= 2;
					if(currfehler == maxfehler)currfehler = 0;
				#endif
		}
		break;
		}
	}
}

void fifo_handler(){
	if(REG_IPC_FIFO_CR & IPC_FIFO_ERROR)
	{
		//clear fifo inmediately
		REG_IPC_FIFO_CR |= (1<<3);
		REG_IPC_FIFO_CR |= IPC_FIFO_ERROR;
	}
	
	REG_IF = IRQ_FIFO_NOT_EMPTY;
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
		
		u32 cmd0 = (u32)(command1);
		u32 addr = (u32)(cmd0 & ~0xC0000000);
		u32 val = (u32)(command2); //Value skip add for speedup
		newvalwrite(addr,val,cmd0,command3,command4);		//args: command1 & ~0xc0000000, command2, command1, command3, command4
	}	
}
