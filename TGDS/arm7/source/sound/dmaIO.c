#include "dmaIO.h"
#include "sound.h"
#include "main.h"
#include "biosTGDS.h"

#include "typedefsTGDS.h"
#include "dsregs.h"
#include "dsregs_asm.h"

#include "ipcfifoTGDS.h"
#include "specific_shared.h"

u16 callline = 0xFFFF;

/*
#ifdef anyarmcom
u32* amr7sendcom = 0;
u32* amr7senddma1 = 0;
u32* amr7senddma2 = 0;
u32* amr7recmuell = 0;
u32* amr7directrec = 0;
u32* amr7indirectrec = 0;
u32* amr7fehlerfeld;
u32 currfehler = 0;
#define maxfehler 8
#endif
*/
//#define checkforerror

//timerStart timeval sucks need a new idear to handel that
//also we are 3 sampels + some overlayer back in the file fix that


void senddebug32(u32 val)
{
	SendMultipleWordACK(0x4000BEEF,(u32)val,0x0,0x0);
}



bool autodetectdetect = false;


//SOUND IO WRITE
void iowrite(u32 addr,u32 val)
{
			switch(addr)
			{
			  
			
			//TM0CNT_L
			case 0x100:{
				SpecificIPCAlign->TM0CNT_L = val;
				//updatetakt();
			}
			break;
			//TM0CNT_H
			case 0x102:
			{
				SpecificIPCAlign->TM0CNT_H = val;
				//updatetakt();
				
			}
				
			break;
			//TM1CNT_L
			case 0x104:
				SpecificIPCAlign->TM1CNT_L = val;
				//updatetakt();
			break;
			//TM1CNT_H
			case 0x106:
				SpecificIPCAlign->TM1CNT_H = val;
				//updatetakt();
				
			break;
			
			
			//4000080h - SOUNDCNT_L (NR50, NR51) - Channel L/R Volume/Enable (R/W)
			case 0x80:
				SpecificIPCAlign->SOUNDCNT_L = val;
				updatevol();				
				break;
			break;
			
			//4000082h - SOUNDCNT_H (GBA only) - DMA Sound Control/Mixing (R/W)
			case 0x82:
				SpecificIPCAlign->SOUNDCNT_H = val;
				updatevol();
				
				//10    DMA Sound A Timer Select (0=Timer 0, 1=Timer 1)
				if(val & (1<<10))
				{
					DMA_A_TIMERSEL = 1;
				}
				else
				{
					DMA_A_TIMERSEL = 0;
				}
				
				//14    DMA Sound B Timer Select (0=Timer 0, 1=Timer 1)
				if(val & (1<<14))
				{
					DMA_B_TIMERSEL = 1;
				}
				else
				{
					DMA_B_TIMERSEL = 0;
				}
				
				//Reset FIFO
				if(val & (1<<11))
				{
					//void * memset ( void * ptr, int value, size_t num );
					memset((u8*)&SpecificIPCAlign->fifodmasA[0],0x0,(int)FIFO_BUFFER_SIZE);
					SpecificIPCAlign->fifoA_offset=0;
				}
				
				if(val & (1<<15))
				{
					memset((u8*)&SpecificIPCAlign->fifodmasB[0],0x0,(int)FIFO_BUFFER_SIZE);
					SpecificIPCAlign->fifoB_offset=0;
				}
				
				//updatetakt();
			break;
			case 0x84:
				if(val & 0x80)REG_SOUNDCNT |= 0x8000;
				else
				{
					REG_SOUNDCNT &= ~0x8000;
				}
			break;
			case 0x88:
					  //Amplitude Resolution/Sampling Cycle is not supported so only Bias
					  //it is better on the DS any way
					
					SpecificIPCAlign->SOUNDBIAS = val;
					//REG_SOUNDBIAS = val;
					REG_SOUNDBIAS = (val&0x3ff);
			break;
			
			
			//old gbaemu4ds
			/*
			case setdmasoundbuff:
			{
				  dmabuffer = val;
				soundbuffA = (u32*)(dmabuffer);
					SCHANNEL_SOURCE(4) = soundbuffA;
				soundbuffB = (u32*)(dmabuffer + 0x50);
					SCHANNEL_SOURCE(5) = soundbuffB;
			}
			break;
			*/
			
			case WaitforVblancarmcmd: //wait
				if(autodetectdetect  && (REG_KEYXY & 0x1) /* && (REG_VCOUNT > 160 || REG_VCOUNT < callline)*/ )
				{
					//REG_IPC_FIFO_TX = 0x4100BEEF; //send cmd 0x4100BEEF
					SendMultipleWordACK(0x4100BEEF,0x0,0x0,0x0);
/*
#ifdef anyarmcom
					*amr7sendcom = *amr7sendcom + 1;
#endif
*/
				}
				break;
			case enableWaitforVblancarmcmdirq: //setauto
				autodetectdetect = true;
				break;
			
			//unused
			/*
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
					SendMultipleWordACK((u32)1,(u32)keys,(u32)tempPos.px,(u32)tempPos.py);
				}
				break;
			*/
			case set_callline: //set callline
				callline = val;
				break;
			default:
/*
#ifdef anyarmcom
	*amr7recmuell = *amr7recmuell + 1;
	amr7fehlerfeld[currfehler] = addr;
	amr7fehlerfeld[currfehler + 1] = val;
	currfehler+= 2;
	if(currfehler == maxfehler)currfehler = 0;
#endif
*/
				break;
			}
}


bool ykeypp = false;
bool isincallline = false;
