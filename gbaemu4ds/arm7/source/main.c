#include "main.h"
#include "sound/dma.h"

#include <nds.h>
#include <nds/arm7/audio.h>

#include "../../common/cpuglobal.h"
#include "../../common/gba_ipc.h"

#include "interrupts/fifo_handler.h"
#include "interrupts/interrupts.h"

#include "ipc/touch_ipc.h"

#include "sound/sound.h"

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
	SendArm9Command(0x4000BEEF,(u32)val,0x0,0x0);
}



bool autodetectdetect = false;


//SOUND IO WRITE
void iowrite(u32 addr,u32 val)
{
			switch(addr)
			{
			  
			
			//TM0CNT_L
			case 0x100:{
				GBAEMU4DS_IPC->TM0CNT_L = val;
				//updatetakt();
			}
			break;
			//TM0CNT_H
			case 0x102:
			{
				GBAEMU4DS_IPC->TM0CNT_H = val;
				//updatetakt();
				
			}
				
			break;
			//TM1CNT_L
			case 0x104:
				GBAEMU4DS_IPC->TM1CNT_L = val;
				//updatetakt();
			break;
			//TM1CNT_H
			case 0x106:
				GBAEMU4DS_IPC->TM1CNT_H = val;
				//updatetakt();
				
			break;
			
			
			//4000080h - SOUNDCNT_L (NR50, NR51) - Channel L/R Volume/Enable (R/W)
			case 0x80:
				GBAEMU4DS_IPC->SOUNDCNT_L = val;
				updatevol();				
				break;
			break;
			
			//4000082h - SOUNDCNT_H (GBA only) - DMA Sound Control/Mixing (R/W)
			case 0x82:
				GBAEMU4DS_IPC->SOUNDCNT_H = val;
				updatevol();
				
				//10    DMA Sound A Timer Select (0=Timer 0, 1=Timer 1)
				if(val & BIT(10))
				{
					DMA_A_TIMERSEL = 1;
				}
				else
				{
					DMA_A_TIMERSEL = 0;
				}
				
				//14    DMA Sound B Timer Select (0=Timer 0, 1=Timer 1)
				if(val & BIT(14))
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
					memset((u8*)&GBAEMU4DS_IPC->fifodmasA[0],0x0,(int)FIFO_BUFFER_SIZE);
					GBAEMU4DS_IPC->fifoA_offset=0;
				}
				
				if(val & (1<<15))
				{
					memset((u8*)&GBAEMU4DS_IPC->fifodmasB[0],0x0,(int)FIFO_BUFFER_SIZE);
					GBAEMU4DS_IPC->fifoB_offset=0;
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
					
					GBAEMU4DS_IPC->SOUNDBIAS = val;
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
					SendArm9Command(0x4100BEEF,0x0,0x0,0x0);
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
					SendArm9Command((u32)1,(u32)keys,(u32)tempPos.px,(u32)tempPos.py);
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


#define noenterCriticalSection



//ori gbaemu4ds sound (alpha2fix5)
/*
//---------------------------------------------------------------------------------
int main() {
//---------------------------------------------------------------------------------
	ledBlink(0);
	readUserSettings();
	
	irqInit();
	// Start the RTC tracking IRQ
	initClockIRQ();

	enableSound();

	REG_IPC_SYNC = 0;
	REG_IPC_FIFO_CR = IPC_FIFO_ENABLE;
	REG_IPC_FIFO_CR = IPC_FIFO_SEND_CLEAR | IPC_FIFO_ENABLE | IPC_FIFO_ERROR;


	//soundbuffA = malloc(32);
	//soundbuffB = malloc(32);

	SCHANNEL_REPEAT_POINT(4) = 0;
	SCHANNEL_LENGTH(4) = 8;

	SCHANNEL_REPEAT_POINT(5) = 0;
	SCHANNEL_LENGTH(5) = 8;

	bool ykeypp = false;
	bool isincallline = false;
	while (true) {
		//sound alloc
		//0-3 matching gba
		//4-5 FIFO
		//ledBlink(1);
		//swiWaitForVBlank();
		if((REG_VCOUNT == callline) && (REG_KEYXY & 0x1)) //X not pressed && (REG_IPC_FIFO_CR & IPC_FIFO_SEND_EMPTY)
		{
			if(!isincallline)
			{
				//REG_IPC_FIFO_TX = 0x3F00BEEF; //send cmd 0x3F00BEEF
				SendArm9Command(0x3F00BEEF,0x0,0x0,0x0);
#ifdef anyarmcom
				*amr7sendcom = *amr7sendcom + 1;
#endif
			}
			isincallline = true;
			//while(REG_VCOUNT == callline); //don't send 2 or more
		}
		else
		{
			isincallline = false;
		}
		if(!(REG_KEYXY & 0x2))
		{
			if(!ykeypp)
			{
				//REG_IPC_FIFO_TX = 0x4200BEEF;
				SendArm9Command(0x4200BEEF,0x0,0x0,0x0);
#ifdef anyarmcom
				*amr7sendcom = *amr7sendcom + 1;
#endif				//while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY);
				//int val2 = REG_IPC_FIFO_RX; //Value skip
				ykeypp = true;

			}
		}
		else
		{
			ykeypp = false;
		}
		if(*(u16*)0x04000136 & 0x80) //close nds
		{
			u32 ie_save = REG_IE;
			// Turn the speaker down.
			if (REG_POWERCNT & 1) swiChangeSoundBias(0,0x400);
			// Save current power state.
			u32 power = readPowerManagement(PM_CONTROL_REG);
			// Set sleep LED.
			writePowerManagement(PM_CONTROL_REG, PM_LED_CONTROL(1));
			// Register for the lid interrupt.
			REG_IE = IRQ_LID;
			// Power down till we get our interrupt.
			swiSleep(); //waits for PM (lid open) interrupt
			//100ms
			swiDelay(838000);
			// Restore the interrupt state.
			REG_IE = ie_save;
			// Restore power state.
			writePowerManagement(PM_CONTROL_REG, power);
			// Turn the speaker up.
			if (REG_POWERCNT & 1) swiChangeSoundBias(1,0x400);
			// update clock tracking
			resyncClock(); 
		}

		while(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY))
		{
#ifndef noenterCriticalSection
			int oldIME = enterCriticalSection();
#endif
#ifdef checkforerror
			if(REG_IPC_FIFO_CR & IPC_FIFO_ERROR)
			{
				senddebug32(0x7FFFFFF0);
				//REG_IPC_FIFO_CR |= IPC_FIFO_ERROR;
			}
#endif
			u32 addr = (REG_IPC_FIFO_RX & ~0xC0000000); //addr + flags //flags 2 most upperen Bits dma = 0 u8 = 1 u16 = 2 u32 = 3
			while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY);
			u32 val = REG_IPC_FIFO_RX; //Value skip add for speedup
#ifdef checkforerror
			if(REG_IPC_FIFO_CR & IPC_FIFO_ERROR)
			{
				senddebug32(0x7FFFFFF1);
				//REG_IPC_FIFO_CR |= IPC_FIFO_ERROR;
			}
#endif

			newvalwrite(addr,val);
#ifdef anyarmcom
			*amr7directrec = *amr7directrec + 1;
			if(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY))*amr7indirectrec = *amr7indirectrec + 1;
#endif
#ifdef checkforerror
			if(REG_IPC_FIFO_CR & IPC_FIFO_ERROR)
			{
				senddebug32(0x7FFFFFF2);
				//REG_IPC_FIFO_CR |= IPC_FIFO_ERROR;
			}
#endif
#ifndef noenterCriticalSection
			leaveCriticalSection(oldIME);
#endif
		}
	}
	return 0;
}
*/





bool ykeypp = false;
bool isincallline = false;
	
u32 interrupts_to_wait_arm7 = 0;

//---------------------------------------------------------------------------------
int main() {
//---------------------------------------------------------------------------------
	ledBlink(0);
	readUserSettings();
	
	interrupts_to_wait_arm7 = 0;
	
	ykeypp = false;
	isincallline = false;
	
	enableSound();
    
	powerOn(POWER_SOUND);
	REG_SOUNDCNT = SOUND_ENABLE | SOUND_VOL(0x7F);
	
	irqInit();
	fifoInit();
    
	REG_IPC_SYNC = 0;
    REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_SEND_CLEAR | IPC_FIFO_RECV_IRQ;
    REG_IPC_FIFO_CR |= (1<<2);  //send empty irq
    REG_IPC_FIFO_CR |= (1<<10); //recv empty irq
	
	
    //4000500h - NDS7 - SOUNDCNT - Sound Control Register (R/W)
    //Bit0-6   Master Volume       (0..127=silent..loud)
    //Bit7     Not used            (always zero)
    //Bit15    Master Enable       (0=Disable, 1=Enable)
    u8 master_vol = 120;
    *(u16*)0x04000500 = (master_vol<<0) | (1<<15);
    
	//ORI: irqSet
	irqSet(IRQ_HBLANK, 			(void*)hblank_handler);					//when HBLANK time
	irqSet(IRQ_VBLANK, 			(void*)vblank_handler);					//when VBLANK time
	irqSet(IRQ_VCOUNT, 			(void*)vcount_handler);					//when VCOUNTER time
	irqSet(IRQ_FIFO_NOT_EMPTY, 			(void*)HandleFifo);					//FIFO
	
	//Hook Timers for GBA DMA
	irqSet(IRQ_TIMER0, 			(void*)timer0interrupt_thread);
	irqSet(IRQ_TIMER1, 			(void*)timer1interrupt_thread);
	
	
	//IRQ_NETWORK is because we use a modified DSWIFI
    interrupts_to_wait_arm7 = (IRQ_HBLANK | IRQ_VBLANK | IRQ_VCOUNT | IRQ_FIFO_NOT_EMPTY | IRQ_IPC_SYNC|IRQ_NETWORK|IRQ_TIMER0|IRQ_TIMER1);
	
    irqEnable(interrupts_to_wait_arm7);
	
	//enable fifo send recv
	*(u16*)0x04000184 = *(u16*)0x04000184 | (1<<15); 

    //set up ppu: do irq on hblank/vblank/vcount/and vcount line is programable (later)
    REG_DISPSTAT = REG_DISPSTAT | DISP_HBLANK_IRQ |  DISP_VBLANK_IRQ | DISP_YTRIGGER_IRQ | (VCOUNT_LINE_INTERRUPT << 15);
	
	REG_IF = ~0;
    REG_IME = 1;
    
    while (1) {
		
		//sound alloc
		//0-3 matching gba
		//4-5 FIFO
		//ledBlink(1);
		//swiWaitForVBlank();
		if(!(REG_KEYXY & 0x2))
		{
			if(!ykeypp)
			{
				//REG_IPC_FIFO_TX = 0x4200BEEF;
				SendArm9Command(0x4200BEEF,0x0,0x0,0x0);
/*
#ifdef anyarmcom
				*amr7sendcom = *amr7sendcom + 1;
#endif				//while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY);
*/
				//int val2 = REG_IPC_FIFO_RX; //Value skip
				ykeypp = true;

			}
		}
		else
		{
			ykeypp = false;
		}

		if(*(u16*)0x04000136 & 0x80) //close nds
		{
			u32 ie_save = REG_IE;
			// Turn the speaker down.
			if (REG_POWERCNT & 1) swiChangeSoundBias(0,0x400);
			// Save current power state.
			u32 power = readPowerManagement(PM_CONTROL_REG);
			// Set sleep LED.
			writePowerManagement(PM_CONTROL_REG, PM_LED_CONTROL(1));
			// Register for the lid interrupt.
			REG_IE = IRQ_LID;
			// Power down till we get our interrupt.
			swiSleep(); //waits for PM (lid open) interrupt
			//100ms
			swiDelay(838000);
			// Restore the interrupt state.
			REG_IE = ie_save;
			// Restore power state.
			writePowerManagement(PM_CONTROL_REG, power);
			// Turn the speaker up.
			if (REG_POWERCNT & 1) swiChangeSoundBias(1,0x400);
			// update clock tracking
			//resyncClock(); 
		}

		
		swiIntrWait(1,interrupts_to_wait_arm7);
	}
    
	return 0;
}

