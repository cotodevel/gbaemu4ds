#include "main.h"
#include "ipc/touch_ipc.h"
#include "wireless/wifi_arm7.h"

#include <nds.h>
#include <nds/arm7/audio.h>
#include <nds/interrupts.h>

#include "../../common/cpuglobal.h"
#include "../../common/gba_ipc.h"

#include "sound.h"

#define 	REG_FIFOA   *(vu32*)(0x04000000+0x00A0)
#define 	REG_FIFOB   *(vu32*)(0x04000000+0x00A4)
#define 	REG_DMA0CNT_L   *(vu16*)(0x04000000+0x00B8)
#define 	REG_DMA0CNT_H   *(vu16*)(0x04000000+0x00BA)
#define 	REG_DMA1CNT_L   *(vu16*)(0x04000000+0x00C4)
#define 	REG_DMA1CNT_H   *(vu16*)(0x04000000+0x00C6)
#define 	REG_DMA2CNT_L   *(vu16*)(0x04000000+0x00D0)
#define 	REG_DMA2CNT_H   *(vu16*)(0x04000000+0x00D2)
#define 	REG_DMA3CNT_L   *(vu16*)(0x04000000+0x00DC)
#define 	REG_DMA3CNT_H   *(vu16*)(0x04000000+0x00DE)
#define 	REG_TM0CNT_L   *(vu16*)(0x04000000+0x0100)
#define 	REG_TM0CNT_H   *(vu16*)(0x04000000+0x0102)
#define 	REG_TM1CNT_L   *(vu16*)(0x04000000+0x0104)
#define 	REG_TM1CNT_H   *(vu16*)(0x04000000+0x0106)
#define 	REG_TM2CNT_L   *(vu16*)(0x04000000+0x0108)
#define 	REG_TM2CNT_H   *(vu16*)(0x04000000+0x010a)
#define 	REG_TM3CNT_L   *(vu16*)(0x04000000+0x010c)
#define 	REG_TM3CNT_H   *(vu16*)(0x04000000+0x010e)

u16 arm7VCOUNTsyncline = 0xFFFF;

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

//#define checkforerror

//timerStart timeval sucks need a new idear to handel that
//also we are 3 sampels + some overlayer back in the file fix that


u8* soundbuffA = 0;
u8* soundbuffB = 0;

u8 dmaApart = 0;
u8 dmaBpart = 0;

u32 dmabuffer = 0;


u8 tacktgeber_sound_FIFO_DMA_A = 0;
u8 tacktgeber_sound_FIFO_DMA_B = 0;

//move these to IPC
//u16 sIPCSharedGBAInst->SOUNDCNT_L = 0;
//u16 sIPCSharedGBAInst->SOUNDCNT_H = 0;

//u16 TM0CNT_L = 0;
//u16 sIPCSharedGBAInst->TM1CNT_L = 0;

//u16 TM0CNT_H = 0;
//u16 sIPCSharedGBAInst->TM1CNT_H = 0;

//u16 sIPCSharedGBAInst->DM1CNT_H = 0;
//u16 sIPCSharedGBAInst->DM2CNT_H = 0;


//u16 sIPCSharedGBAInst->DM1SAD_L = 0;
//u16 sIPCSharedGBAInst->DM1SAD_H  = 0;
//u16 sIPCSharedGBAInst->DM1DAD_L = 0;
//u16 sIPCSharedGBAInst->DM1DAD_H  = 0;

//u16 sIPCSharedGBAInst->DM2SAD_L = 0;
//u16 sIPCSharedGBAInst->DM2SAD_H  = 0;
//u16 sIPCSharedGBAInst->DM2DAD_L = 0;
//u16 sIPCSharedGBAInst->DM2DAD_H  = 0;


//(mirrored) GBA IRQS    . The DS is too slow for using the IPC as a shared IO GBA Map memory. So these registers "shadow copy" the current IE/IF/IME from the emu.
u16 IE;
u16 IF;
u16 IME;

	
//debug stuff
vu32 debugsrc1 = 0;
vu32 debugsrc2  = 0;
vu32 debugfr1 = 0;
vu32 debugfr2  = 0;

void senddebug32(u32 val)
{
	//REG_IPC_FIFO_TX = 0x4000BEEF;
	//REG_IPC_FIFO_TX = val;
	SendArm9Command(0x4000BEEF,val,0x0,0x0);
#ifdef anyarmcom
	*amr7sendcom = *amr7sendcom + 2;
#endif
}

#ifdef notdef
void dmaAtimerinter()
{
/*	int oldIME = enterCriticalSection();
	REG_IPC_FIFO_TX = debugsrc1;
	debugsrc1+=16;
	while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY);
	u32 temp1 = REG_IPC_FIFO_RX;
	while(temp1 != 0x1) //can't happen else
	{
		while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY);
		u32 temp2 = REG_IPC_FIFO_RX;
		newvalwrite(temp1 & ~0xC0000000,temp2);
		while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY);
		temp1 = REG_IPC_FIFO_RX;
	}
	int i = 0;
	while(i < 4)
	{
		while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY);
		*(u32*)(soundbuffA + i*4 + 0x10 * dmaApart) = REG_IPC_FIFO_RX;
		i++;
	}
	if(dmaApart == 0) dmaApart = 1;
	else dmaApart = 0;
	leaveCriticalSection(oldIME);*/
}
void dmaBtimerinter()
{
	/*int oldIME = enterCriticalSection();
	REG_IPC_FIFO_TX = debugsrc2;
	debugsrc2+=0x10;
	while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY);
	u32 temp1 = REG_IPC_FIFO_RX;
	while(temp1 != 0x1)  //my the irq is not tiggerd yet
	{
		while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY);
		u32 temp2 = REG_IPC_FIFO_RX;
		newvalwrite(temp1 & ~0xC0000000,temp2);
		while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY);
		temp1 = REG_IPC_FIFO_RX;
	}
	int i = 0;
	while(i < 4)
	{
		while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY);
		*(u32*)(soundbuffB + i*4 + 0x10 * dmaBpart) = REG_IPC_FIFO_RX;
		i++;
	}
	if(dmaBpart == 0) dmaBpart = 1;
	else dmaBpart = 0;
	leaveCriticalSection(oldIME);*/
}
#endif
void dmaAtimerinter()
{
#ifdef neu_sound_16fifo
	if(dmaApart){
		//REG_IPC_FIFO_TX = debugsrc1 + 0x400001;
		SendArm9Command((debugsrc1 + 0x400001),0x0,0x0,0x0);
	}
	else {
		//REG_IPC_FIFO_TX = debugsrc1 + 0x400000;
		SendArm9Command((debugsrc1 + 0x400000),0x0,0x0,0x0);
	}
#else
	//REG_IPC_FIFO_TX = debugsrc1 + 0x400000;
	SendArm9Command((debugsrc1 + 0x400000),0x0,0x0,0x0);
	
	if(dmaApart){
		//REG_IPC_FIFO_TX = (u32)(soundbuffA + 0x10); // time criticall todo lockup
		SendArm9Command((soundbuffA + 0x10),0x0,0x0,0x0);
	}
	else {
		//REG_IPC_FIFO_TX = (u32)soundbuffA;
		SendArm9Command((u32)soundbuffA,0x0,0x0,0x0);
	}
#endif
	debugsrc1+=0x10;
	if(dmaApart == 0) dmaApart = 1;
	else dmaApart = 0;
#ifdef anyarmcom
	*amr7senddma1 = *amr7senddma1 + 1;
#endif
}
void dmaBtimerinter()
{

#ifdef neu_sound_16fifo
	if(dmaApart){
		//REG_IPC_FIFO_TX = debugsrc1 + 0x400002;
		SendArm9Command((debugsrc1 + 0x400002),0x0,0x0,0x0);
	}
	else { 
		//REG_IPC_FIFO_TX = debugsrc1 + 0x400003;
		SendArm9Command((debugsrc1 + 0x400003),0x0,0x0,0x0);
	}
#else
	//REG_IPC_FIFO_TX = debugsrc2 + 0x400000;
	SendArm9Command((debugsrc2 + 0x400000),0x0,0x0,0x0);
	
	if(dmaBpart){
		//REG_IPC_FIFO_TX = (u32)(soundbuffB + 0x10); //time criticall todo lockup
		SendArm9Command((u32)(soundbuffB + 0x10),0x0,0x0,0x0);
	}
	else {
		//REG_IPC_FIFO_TX = (u32)soundbuffB;
		SendArm9Command((u32)soundbuffB,0x0,0x0,0x0);
	}
#endif
	debugsrc2+=0x10;
	if(dmaBpart == 0) dmaBpart = 1;
	else dmaBpart = 0;
#ifdef anyarmcom
	*amr7senddma2 = *amr7senddma2 + 1;
#endif
}
bool autodetectdetect = false;

u32 power = 0;
u32 ie_save = 0;
void lid_closing_handler(){
	
	ie_save = REG_IE;
	// Turn the speaker down.
	if (REG_POWERCNT & 1) swiChangeSoundBias(0,0x400);
	// Save current power state.
	power = readPowerManagement(PM_CONTROL_REG);
	// Set sleep LED.
	writePowerManagement(PM_CONTROL_REG, PM_LED_CONTROL(1));
	
	// Enable IRQ_LID on interrupt vectors
	REG_IE = IRQ_LID;
	
	SendArm9Command(FIFO_SWI_SLEEPMODE_PHASE2,0,0,0);
	
	// Power down till we get our interrupt.
	swiIntrWait(1,IRQ_LID); //waits for PM lid open irq
}

//ok
void lid_open_irq_handler(){
	//100ms
	swiDelay(838000);
	// Restore the interrupt state.
	REG_IE = ie_save;
	// Restore power state.
	writePowerManagement(PM_CONTROL_REG, power);
	// Turn the speaker up.
	if (REG_POWERCNT & 1) swiChangeSoundBias(1,0x400);
	
	//REG_IF = IRQ_LID; //is it hw toggled? (physical lid)
	Setarm7Sleep(false);	//toggle switch for ARM9 resume
}


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


#define noenterCriticalSection
//---------------------------------------------------------------------------------
int main() {
//---------------------------------------------------------------------------------
	REG_IE = 0;
	
	ledBlink(0);
	readUserSettings();
	
	u32 curIRQ = IRQ_HBLANK | IRQ_VBLANK | IRQ_VCOUNT | IRQ_LID | IRQ_FIFO_NOT_EMPTY | IRQ_NETWORK | IRQ_WIFI;	//IRQ_NETWORK == initClockIRQ();
	
	irqInit();
	fifoInit();
	
	installWifiFIFO();
	
	irqSet(IRQ_HBLANK,			(void*)hblank_handler);
	irqSet(IRQ_VBLANK, 			(void*)vblank_handler);
	irqSet(IRQ_VCOUNT, 			(void*)vcount_handler);					//irq when VCOUNTER time
	irqSet(IRQ_LID, 			(void*)lid_open_irq_handler);			//irq when opening the LID of DS time
	irqSet(IRQ_FIFO_NOT_EMPTY, 			(void*)fifo_handler);			//irq when receiving fifo msg from arm9
	irqSet(IRQ_TIMER0, 			(void*)timer0interrupt_thread);	    
    
	REG_IPC_SYNC = 0;
    REG_IPC_FIFO_CR = IPC_FIFO_RECV_IRQ | IPC_FIFO_SEND_IRQ | IPC_FIFO_ERROR | IPC_FIFO_ENABLE;
	
	//set up ppu: do irq on hblank/vblank/vcount/and vcount line is 159
    REG_DISPSTAT = REG_DISPSTAT | DISP_HBLANK_IRQ | DISP_VBLANK_IRQ | DISP_YTRIGGER_IRQ | (159 << 15);
	
	// Start the RTC tracking IRQ
	initClockIRQ();
	enableSound();
	irqEnable(curIRQ);
	
	REG_IF = ~0;
    REG_IME = 1;
	
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
		if((REG_VCOUNT == arm7VCOUNTsyncline) && (REG_KEYXY & 0x1)) //X not pressed && (REG_IPC_FIFO_CR & IPC_FIFO_SEND_EMPTY)
		{
			if(!isincallline)
			{
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
		
		//Close nds lid @ ARM7
		if(*(u16*)0x04000136 & 0x80)
		{
			SendArm9Command(FIFO_SWIGBA_FROM_ARM7,0x0,0x0,0x0);
		}
		
		//swiWaitForVBlank();
	}
	return 0;
}

void updatevol()
{

	/*****************************************************/
	/*                  Update Voll                      */
	/*****************************************************/
	struct sIPCSharedGBA* sIPCSharedGBAInst = GetsIPCSharedGBA();
	//Sound_chan = (Volume_Right * enabeled + Volume_Left * enabeled) * (Soundcnt(1,2,4))*static_for_max
	//DMA = (Soundcnt(1,2) * enabeled + Soundcnt(1,2) * enabeled) * sataic_for_max
	SCHANNEL_CR(4) = (SCHANNEL_CR(4) & ~0xFF) | ((( 1 + ((sIPCSharedGBAInst->SOUNDCNT_H & 0x4) >> 2))*((sIPCSharedGBAInst->SOUNDCNT_H & BIT(8)) >> 8) + ( 1 + ((sIPCSharedGBAInst->SOUNDCNT_H & 0x4) >> 2))*((sIPCSharedGBAInst->SOUNDCNT_H & BIT(9)) >> 9))*31);     //max:124
	SCHANNEL_CR(5) = (SCHANNEL_CR(5) & ~0xFF) | ((( 1 + ((sIPCSharedGBAInst->SOUNDCNT_H & 0x8) >> 3))*((sIPCSharedGBAInst->SOUNDCNT_H & BIT(12)) >> 12) + ( 1 + ((sIPCSharedGBAInst->SOUNDCNT_H & 0x8) >> 3))*((sIPCSharedGBAInst->SOUNDCNT_H & BIT(13)) >> 13))*31); //max:124
	int Vol = sIPCSharedGBAInst->SOUNDCNT_H & 0x3;
	switch(Vol)
	{
	case 3:
	case 0:
		Vol = 1;
		break;
	case 1:
		Vol = 2;
		break;
	case 2:
		Vol = 4;
		break;
	}
	int Masterright = sIPCSharedGBAInst->SOUNDCNT_L & 0x7;
	int Masterleft =  (sIPCSharedGBAInst->SOUNDCNT_L << 4) & 0x7;
	SCHANNEL_CR(0) = (SCHANNEL_CR(0) & ~0xFF) | ((Masterright * ((sIPCSharedGBAInst->SOUNDCNT_L & BIT(8)) >> 8) + Masterleft * ((sIPCSharedGBAInst->SOUNDCNT_L & BIT(12)) >> 12) ) * Vol *2);  //max:112
	SCHANNEL_CR(1) = (SCHANNEL_CR(1) & ~0xFF) | ((Masterright * ((sIPCSharedGBAInst->SOUNDCNT_L & BIT(9)) >> 9) + Masterleft * ((sIPCSharedGBAInst->SOUNDCNT_L & BIT(13)) >> 13) ) * Vol *2);  //max:112
	SCHANNEL_CR(2) = (SCHANNEL_CR(2) & ~0xFF) | ((Masterright * ((sIPCSharedGBAInst->SOUNDCNT_L & BIT(10)) >> 10) + Masterleft * ((sIPCSharedGBAInst->SOUNDCNT_L & BIT(14)) >> 14) ) * Vol *2);//max:112
	SCHANNEL_CR(3) = (SCHANNEL_CR(3) & ~0xFF) | ((Masterright * ((sIPCSharedGBAInst->SOUNDCNT_L & BIT(11)) >> 11) + Masterleft * ((sIPCSharedGBAInst->SOUNDCNT_L & BIT(15)) >> 15) ) * Vol *2);//max:112


    /*****************************************************/
	/*                  Update Panning                   */
	/*****************************************************/
	

	if(sIPCSharedGBAInst->SOUNDCNT_H & BIT(9)) SCHANNEL_CR(4) = (SCHANNEL_CR(4) & ~0xFF0000);
	if(sIPCSharedGBAInst->SOUNDCNT_H & BIT(8)) SCHANNEL_CR(4) = (SCHANNEL_CR(4) & ~0xFF0000) | 0x7F0000;
	if(sIPCSharedGBAInst->SOUNDCNT_H & BIT(8) && sIPCSharedGBAInst->SOUNDCNT_H & BIT(9)) SCHANNEL_CR(4) = (SCHANNEL_CR(4) & ~0xFF0000) | 0x400000; //same on both

	if(sIPCSharedGBAInst->SOUNDCNT_H & BIT(13)) SCHANNEL_CR(5) = (SCHANNEL_CR(5) & ~0xFF0000);
	if(sIPCSharedGBAInst->SOUNDCNT_H & BIT(12)) SCHANNEL_CR(5) = (SCHANNEL_CR(5) & ~0xFF0000) | 0x7F0000;
	if(sIPCSharedGBAInst->SOUNDCNT_H & BIT(12) && sIPCSharedGBAInst->SOUNDCNT_H & BIT(13)) SCHANNEL_CR(5) = (SCHANNEL_CR(5) & ~0xFF0000) | 0x400000; //same on both

	int right = sIPCSharedGBAInst->SOUNDCNT_L & 7;
	int left = (sIPCSharedGBAInst->SOUNDCNT_L << 4) & 7;
	int tempmixedvol1_4 = 0;
	if((left + right) != 0) //don't work
	{
		tempmixedvol1_4 = ((right*0x7F0000)/(right + left) & 0x7F0000);
	}
	
	if(sIPCSharedGBAInst->SOUNDCNT_L & BIT(12)) SCHANNEL_CR(0) = (SCHANNEL_CR(0) & ~0xFF0000);
	if(sIPCSharedGBAInst->SOUNDCNT_L & BIT(8)) SCHANNEL_CR(0) = (SCHANNEL_CR(0) & ~0xFF0000) | 0x7F0000;
	if(sIPCSharedGBAInst->SOUNDCNT_L & BIT(8) && sIPCSharedGBAInst->SOUNDCNT_L & BIT(12)) SCHANNEL_CR(0) = (SCHANNEL_CR(0) & ~0xFF0000) | tempmixedvol1_4;

	if(sIPCSharedGBAInst->SOUNDCNT_L & BIT(13)) SCHANNEL_CR(4) = (SCHANNEL_CR(1) & ~0xFF0000);
	if(sIPCSharedGBAInst->SOUNDCNT_L & BIT(9)) SCHANNEL_CR(4) = (SCHANNEL_CR(1) & ~0xFF0000) | 0x7F0000;
	if(sIPCSharedGBAInst->SOUNDCNT_L & BIT(9) && sIPCSharedGBAInst->SOUNDCNT_L & BIT(13)) SCHANNEL_CR(1) = (SCHANNEL_CR(1) & ~0xFF0000) | tempmixedvol1_4; 

	if(sIPCSharedGBAInst->SOUNDCNT_L & BIT(14)) SCHANNEL_CR(4) = (SCHANNEL_CR(2) & ~0xFF0000);
	if(sIPCSharedGBAInst->SOUNDCNT_L & BIT(10)) SCHANNEL_CR(4) = (SCHANNEL_CR(2) & ~0xFF0000) | 0x7F0000;
	if(sIPCSharedGBAInst->SOUNDCNT_L & BIT(10) && sIPCSharedGBAInst->SOUNDCNT_L & BIT(14)) SCHANNEL_CR(2) = (SCHANNEL_CR(2) & ~0xFF0000) | tempmixedvol1_4;

	if(sIPCSharedGBAInst->SOUNDCNT_L & BIT(15)) SCHANNEL_CR(4) = (SCHANNEL_CR(3) & ~0xFF0000);
	if(sIPCSharedGBAInst->SOUNDCNT_L & BIT(11)) SCHANNEL_CR(4) = (SCHANNEL_CR(3) & ~0xFF0000) | 0x7F0000;
	if(sIPCSharedGBAInst->SOUNDCNT_L & BIT(11) && sIPCSharedGBAInst->SOUNDCNT_L & BIT(15)) SCHANNEL_CR(3) = (SCHANNEL_CR(3) & ~0xFF0000) | tempmixedvol1_4;

}


void updatetakt()
{
	struct sIPCSharedGBA* sIPCSharedGBAInst = GetsIPCSharedGBA();
	//FIFO A
	if(tacktgeber_sound_FIFO_DMA_A == 0)
	{
		/*int seek;
		switch(TM0CNT_H & 0x3)
		{
			case 0:
				seek = 0;
				break;
			case 1:
				seek = 6;
				break;
			case 2:
				seek = 8;
				break;
			case 3:
				seek = 10;
				break;
		}*/
		//SCHANNEL_TIMER(4) = debugfr1 = (((-TM0CNT_L) << seek) & 0xFFFF) << 1;
		SCHANNEL_TIMER(4) = debugfr1 = sIPCSharedGBAInst->TM0CNT_L;
	}
	else
	{
		/*int seek;
		switch(sIPCSharedGBAInst->TM1CNT_H & 0x3)
		{
			case 0:
				seek = 0;
				break;
			case 1:
				seek = 6;
				break;
			case 2:
				seek = 8;
				break;
			case 3:
				seek = 10;
				break;
		}*/
		//SCHANNEL_TIMER(4) = debugfr1 = (((-sIPCSharedGBAInst->TM1CNT_L) & 0xFFFF) << seek) << 1;
		SCHANNEL_TIMER(4) = debugfr1 = sIPCSharedGBAInst->TM1CNT_L;
	}
	//FIFO B
	if(tacktgeber_sound_FIFO_DMA_B == 0)
	{
		/*int seek;
		switch(TM0CNT_H & 0x3)
		{
			case 0:
				seek = 0;
				break;
			case 1:
				seek = 6;
				break;
			case 2:
				seek = 8;
				break;
			case 3:
				seek = 10;
				break;
		}*/
		//SCHANNEL_TIMER(5) = debugfr2 = (((-TM0CNT_L) << seek) & 0xFFFF) << 1;
		SCHANNEL_TIMER(5) = debugfr2 = sIPCSharedGBAInst->TM0CNT_L;
	}
	else
	{
		/*int seek;
		switch(sIPCSharedGBAInst->TM1CNT_H & 0x3)
		{
			case 0:
				seek = 0;
				break;
			case 1:
				seek = 6;
				break;
			case 2:
				seek = 8;
				break;
			case 3:
				seek = 10;
				break;
		}*/
		//SCHANNEL_TIMER(5) = debugfr2 = (((-sIPCSharedGBAInst->TM1CNT_L) << seek) & 0xFFFF) << 1; //everything is 2 times faster than on ther gba here
		SCHANNEL_TIMER(5) = debugfr2 = sIPCSharedGBAInst->TM1CNT_L; //everything is 2 times faster than on ther gba here
	}
}

void checkstart()
{
	struct sIPCSharedGBA* sIPCSharedGBAInst = GetsIPCSharedGBA();
	//DMA 1
	if(sIPCSharedGBAInst->DM1DAD_L == 0x00A0 && sIPCSharedGBAInst->DM1DAD_H == 0x0400 && (sIPCSharedGBAInst->DM1CNT_H & 0x8000))
	{
		//FIFO A
		if(tacktgeber_sound_FIFO_DMA_A == 0)
		{
			if(sIPCSharedGBAInst->TM0CNT_H & 0x80) //started timer
			{
				//SCHANNEL_LENGTH(4) = 0xFFFFFFFF;
				debugsrc1 = sIPCSharedGBAInst->DM1SAD_L | (sIPCSharedGBAInst->DM1SAD_H << 16);
				//senddebug32(debugsrc1);
				if(!(SCHANNEL_CR(4) & 0x80000000))SCHANNEL_CR(4) |= 0x80000000 |SOUND_REPEAT; //start now if not already started
				if(!(TIMER0_CR & TIMER_ENABLE))timerStart(0, ClockDivider_1,debugfr1 << 5, dmaAtimerinter);
				////senddebug32(debugfr1 << 5);
			}
		}
		else
		{
			if(sIPCSharedGBAInst->TM1CNT_H & 0x80)
			{
				//SCHANNEL_LENGTH(4) = 0xFFFFFFFF;
				debugsrc1 = sIPCSharedGBAInst->DM1SAD_L | (sIPCSharedGBAInst->DM1SAD_H << 16);
				//senddebug32(debugsrc1);
				if(!(SCHANNEL_CR(4) & 0x80000000))SCHANNEL_CR(4) |= 0x80000000 |SOUND_REPEAT; //start now
				if(!(TIMER0_CR & TIMER_ENABLE))timerStart(0, ClockDivider_1,debugfr1 << 5, dmaAtimerinter);
				//senddebug32(debugfr1 << 5);
			}
		}
	}
	else
	{
		if(sIPCSharedGBAInst->DM1DAD_L == 0x00A4 && sIPCSharedGBAInst->DM1DAD_H == 0x0400 && (sIPCSharedGBAInst->DM1CNT_H & 0x8000))
		{
			//FIFO B
			if(tacktgeber_sound_FIFO_DMA_B == 0)
			{
				if(sIPCSharedGBAInst->TM0CNT_H & 0x80) //started timer
				{
					//SCHANNEL_LENGTH(5) = 0xFFFFFFFF;
					debugsrc2 = sIPCSharedGBAInst->DM1SAD_L | (sIPCSharedGBAInst->DM1SAD_H << 16);
					//senddebug32(debugsrc2);
					if(!(SCHANNEL_CR(5) & 0x80000000))SCHANNEL_CR(5) |= 0x80000000 |SOUND_REPEAT; //start now
					if(!(TIMER1_CR & TIMER_ENABLE))timerStart(1, ClockDivider_1,debugfr2 << 5, dmaBtimerinter);
				//senddebug32(debugfr2 << 5);
				}
			}
			else
			{
				if(sIPCSharedGBAInst->TM1CNT_H & 0x80)
				{
					//SCHANNEL_LENGTH(5) = 0xFFFFFFFF;
					debugsrc2 = sIPCSharedGBAInst->DM1SAD_L | (sIPCSharedGBAInst->DM1SAD_H << 16);
					//senddebug32(debugsrc2);
					if(!(SCHANNEL_CR(5) & 0x80000000))SCHANNEL_CR(5) |= 0x80000000 |SOUND_REPEAT; //start now
					if(!(TIMER1_CR & TIMER_ENABLE))timerStart(1, ClockDivider_1,debugfr2 << 5, dmaBtimerinter);
				//senddebug32(debugfr2 << 5);
				}
			}
		}

	}


	//DMA 2
	if(sIPCSharedGBAInst->DM2DAD_L == 0x00A0 && sIPCSharedGBAInst->DM2DAD_H == 0x0400 && (sIPCSharedGBAInst->DM2CNT_H & 0x8000))
	{
		//FIFO A
		if(tacktgeber_sound_FIFO_DMA_A == 0)
		{
			if(sIPCSharedGBAInst->TM0CNT_H & 0x80) //started timer
			{
				//SCHANNEL_LENGTH(4) = 0xFFFFFFFF;
				debugsrc1 = sIPCSharedGBAInst->DM2SAD_L | (sIPCSharedGBAInst->DM2SAD_H << 16);
				//senddebug32(debugsrc1);
				if(!(SCHANNEL_CR(4) & 0x80000000))SCHANNEL_CR(4) |= 0x80000000 |SOUND_REPEAT; //start now
				if(!(TIMER0_CR & TIMER_ENABLE))timerStart(0, ClockDivider_1,debugfr1 << 5, dmaAtimerinter);
				//senddebug32(debugfr1 << 5);
			}
		}
		else
		{
			if(sIPCSharedGBAInst->TM1CNT_H & 0x80)
			{
				//SCHANNEL_LENGTH(4) = 0xFFFFFFFF;
				debugsrc1 = sIPCSharedGBAInst->DM2SAD_L | (sIPCSharedGBAInst->DM2SAD_H << 16);
				//senddebug32(debugsrc1);
				if(!(SCHANNEL_CR(4) & 0x80000000))SCHANNEL_CR(4) |= 0x80000000 |SOUND_REPEAT; //start now
				if(!(TIMER0_CR & TIMER_ENABLE))timerStart(0, ClockDivider_1,debugfr1 << 5, dmaAtimerinter);
				//senddebug32(debugfr1 << 5);
			}
		}
	}
	else
	{
		if(sIPCSharedGBAInst->DM2DAD_L == 0x00A4 && sIPCSharedGBAInst->DM2DAD_H == 0x0400 && (sIPCSharedGBAInst->DM2CNT_H & 0x8000))
		{
			//FIFO B
			if(tacktgeber_sound_FIFO_DMA_B == 0)
			{
				if(sIPCSharedGBAInst->TM0CNT_H & 0x80) //started timer
				{
					//SCHANNEL_LENGTH(5) = 0xFFFFFFFF;
					debugsrc2 = sIPCSharedGBAInst->DM2SAD_L | (sIPCSharedGBAInst->DM2SAD_H << 16);
					//senddebug32(debugsrc2);
					if(!(SCHANNEL_CR(5) & 0x80000000))SCHANNEL_CR(5) |= 0x80000000 |SOUND_REPEAT; //start now
					if(!(TIMER1_CR & TIMER_ENABLE))timerStart(1, ClockDivider_1,debugfr2 << 5, dmaBtimerinter);
				//senddebug32(debugfr2 << 5);
				}
			}
			else
			{
				if(sIPCSharedGBAInst->TM1CNT_H & 0x80)
				{
					//SCHANNEL_LENGTH(5) = 0xFFFFFFFF;
					debugsrc2 = sIPCSharedGBAInst->DM2SAD_L | (sIPCSharedGBAInst->DM2SAD_H << 16);
					//senddebug32(debugsrc2);
					if(!(SCHANNEL_CR(5) & 0x80000000))SCHANNEL_CR(5) |= 0x80000000 |SOUND_REPEAT; //start now
					if(!(TIMER1_CR & TIMER_ENABLE))timerStart(1, ClockDivider_1,debugfr2 << 5, dmaBtimerinter);
				}
			}
		}
	}
}


#ifdef nichtdef
void checkstart()
{
	sIPCSharedGBAInst
	//DMA 1
	if(sIPCSharedGBAInst->DM1DAD_L == 0x00A0 && sIPCSharedGBAInst->DM1DAD_H == 0x0400 && (sIPCSharedGBAInst->DM1CNT_H & 0x8000))
	{
		//FIFO A
		if(tacktgeber_sound_FIFO_DMA_A == 0)
		{
			if(sIPCSharedGBAInst->TM0CNT_H & 0x80) //started timer
			{
				//SCHANNEL_LENGTH(4) = 0xFFFFFFFF;
				debugsrc1 = sIPCSharedGBAInst->DM1SAD_L | (sIPCSharedGBAInst->DM1SAD_H << 16);
				//senddebug32(debugsrc1);
				dmaAtimerinter();
				dmaAtimerinter();
				if(!(SCHANNEL_CR(4) & 0x80000000))SCHANNEL_CR(4) |= 0x80000000 |SOUND_REPEAT; //start now
				if(!(TIMER0_CR & TIMER_ENABLE))timerStart(0, ClockDivider_1,debugfr1 << 5, dmaAtimerinter);
				////senddebug32(debugfr1 << 5);
			}
			else
			{
				SCHANNEL_CR(4) &= 0x80000000; //stop now
				dmaApart = 0;
				SCHANNEL_SOURCE(4) = soundbuffA;
				SCHANNEL_REPEAT_POINT(4) = 8;
				SCHANNEL_LENGTH(4) = 0;
			}
		}
		else
		{
			if(sIPCSharedGBAInst->TM1CNT_H & 0x80)
			{
				//SCHANNEL_LENGTH(4) = 0xFFFFFFFF;
				debugsrc1 = sIPCSharedGBAInst->DM1SAD_L | (sIPCSharedGBAInst->DM1SAD_H << 16);
				//senddebug32(debugsrc1);
				dmaAtimerinter();
				dmaAtimerinter();
				if(!(SCHANNEL_CR(4) & 0x80000000))SCHANNEL_CR(4) |= 0x80000000 |SOUND_REPEAT; //start now
				if(!(TIMER0_CR & TIMER_ENABLE))timerStart(0, ClockDivider_1,debugfr1 << 5, dmaAtimerinter);
				//senddebug32(debugfr1 << 5);
			}
			else
			{
				SCHANNEL_CR(4) &= 0x80000000; //stop now
				dmaApart = 0;
				SCHANNEL_SOURCE(4) = soundbuffA;
				SCHANNEL_REPEAT_POINT(4) = 8;
				SCHANNEL_LENGTH(4) = 0;
				////senddebug32(SCHANNEL_CR(4));
			}
		}
	}
	else
	{
		if(sIPCSharedGBAInst->DM1DAD_L == 0x00A4 && sIPCSharedGBAInst->DM1DAD_H == 0x0400 && (sIPCSharedGBAInst->DM1CNT_H & 0x8000))
		{
			//FIFO B
			if(tacktgeber_sound_FIFO_DMA_B == 0)
			{
				if(sIPCSharedGBAInst->TM0CNT_H & 0x80) //started timer
				{
					//SCHANNEL_LENGTH(5) = 0xFFFFFFFF;
					debugsrc2 = sIPCSharedGBAInst->DM1SAD_L | (sIPCSharedGBAInst->DM1SAD_H << 16);
					//senddebug32(debugsrc2);
					dmaBtimerinter();
					dmaBtimerinter();
					if(!(SCHANNEL_CR(5) & 0x80000000))SCHANNEL_CR(5) |= 0x80000000 |SOUND_REPEAT; //start now
					if(!(TIMER1_CR & TIMER_ENABLE))timerStart(1, ClockDivider_1,debugfr2 << 5, dmaBtimerinter);
				//senddebug32(debugfr2 << 5);
				}
				else
				{
					SCHANNEL_CR(5) &= 0x80000000; //stop now
					dmaApart = 0;
					SCHANNEL_SOURCE(5) = soundbuffB;
					SCHANNEL_REPEAT_POINT(5) = 8;
					SCHANNEL_LENGTH(5) = 0;
					////senddebug32(SCHANNEL_CR(5));
				}
			}
			else
			{
				if(sIPCSharedGBAInst->TM1CNT_H & 0x80)
				{
					//SCHANNEL_LENGTH(5) = 0xFFFFFFFF;
					debugsrc2 = sIPCSharedGBAInst->DM1SAD_L | (sIPCSharedGBAInst->DM1SAD_H << 16);
					//senddebug32(debugsrc2);
					dmaBtimerinter();
					dmaBtimerinter();
					if(!(SCHANNEL_CR(5) & 0x80000000))SCHANNEL_CR(5) |= 0x80000000 |SOUND_REPEAT; //start now
					if(!(TIMER1_CR & TIMER_ENABLE))timerStart(1, ClockDivider_1,debugfr2 << 5, dmaBtimerinter);
				//senddebug32(debugfr2 << 5);
				}
				else
				{
					SCHANNEL_CR(5) &= 0x80000000; //stop now
					dmaApart = 0;
					SCHANNEL_SOURCE(5) = soundbuffB;
					SCHANNEL_REPEAT_POINT(5) = 8;
					SCHANNEL_LENGTH(5) = 0;
					////senddebug32(SCHANNEL_CR(5));
				}
			}
		}

	}


	//DMA 2
	if(sIPCSharedGBAInst->DM2DAD_L == 0x00A0 && sIPCSharedGBAInst->DM2DAD_H == 0x0400 && (sIPCSharedGBAInst->DM2CNT_H & 0x8000))
	{
		//FIFO A
		if(tacktgeber_sound_FIFO_DMA_A == 0)
		{
			if(sIPCSharedGBAInst->TM0CNT_H & 0x80) //started timer
			{
				//SCHANNEL_LENGTH(4) = 0xFFFFFFFF;
				debugsrc1 = sIPCSharedGBAInst->DM2SAD_L | (sIPCSharedGBAInst->DM2SAD_H << 16);
				//senddebug32(debugsrc1);
				dmaAtimerinter();
				dmaAtimerinter();
				if(!(SCHANNEL_CR(4) & 0x80000000))SCHANNEL_CR(4) |= 0x80000000 |SOUND_REPEAT; //start now
				if(!(TIMER0_CR & TIMER_ENABLE))timerStart(0, ClockDivider_1,debugfr1 << 5, dmaAtimerinter);
				//senddebug32(debugfr1 << 5);
			}
			else
			{
				SCHANNEL_CR(4) &= 0x80000000; //stop now
				dmaApart = 0;
				SCHANNEL_SOURCE(4) = soundbuffA;
				SCHANNEL_REPEAT_POINT(4) = 8;
				SCHANNEL_LENGTH(4) = 0;
				////senddebug32(SCHANNEL_CR(5));
			}
		}
		else
		{
			if(sIPCSharedGBAInst->TM1CNT_H & 0x80)
			{
				//SCHANNEL_LENGTH(4) = 0xFFFFFFFF;
				debugsrc1 = sIPCSharedGBAInst->DM2SAD_L | (sIPCSharedGBAInst->DM2SAD_H << 16);
				//senddebug32(debugsrc1);
				dmaAtimerinter();
				dmaAtimerinter();
				if(!(SCHANNEL_CR(4) & 0x80000000))SCHANNEL_CR(4) |= 0x80000000 |SOUND_REPEAT; //start now
				if(!(TIMER0_CR & TIMER_ENABLE))timerStart(0, ClockDivider_1,debugfr1 << 5, dmaAtimerinter);
				//senddebug32(debugfr1 << 5);
			}
			else
			{
				SCHANNEL_CR(4) &= 0x80000000; //stop now
				dmaApart = 0;
				SCHANNEL_SOURCE(4) = soundbuffA;
				SCHANNEL_REPEAT_POINT(4) = 8;
				SCHANNEL_LENGTH(4) = 0;
				////senddebug32(SCHANNEL_CR(5));
			}
		}
	}
	else
	{
		if(sIPCSharedGBAInst->DM2DAD_L == 0x00A4 && sIPCSharedGBAInst->DM2DAD_H == 0x0400 && (sIPCSharedGBAInst->DM2CNT_H & 0x8000))
		{
			//FIFO B
			if(tacktgeber_sound_FIFO_DMA_B == 0)
			{
				if(sIPCSharedGBAInst->TM0CNT_H & 0x80) //started timer
				{
					//SCHANNEL_LENGTH(5) = 0xFFFFFFFF;
					debugsrc2 = sIPCSharedGBAInst->DM2SAD_L | (sIPCSharedGBAInst->DM2SAD_H << 16);
					//senddebug32(debugsrc2);
					dmaBtimerinter();
					dmaBtimerinter();
					if(!(SCHANNEL_CR(5) & 0x80000000))SCHANNEL_CR(5) |= 0x80000000 |SOUND_REPEAT; //start now
					if(!(TIMER1_CR & TIMER_ENABLE))timerStart(1, ClockDivider_1,debugfr2 << 5, dmaBtimerinter);
				//senddebug32(debugfr2 << 5);
				}
				else
				{
					SCHANNEL_CR(5) &= 0x80000000; //stop now
					dmaApart = 0;
					SCHANNEL_SOURCE(5) = soundbuffB;
					SCHANNEL_REPEAT_POINT(5) = 8;
					SCHANNEL_LENGTH(5) = 0;
					////senddebug32(SCHANNEL_CR(5));
				}
			}
			else
			{
				if(sIPCSharedGBAInst->TM1CNT_H & 0x80)
				{
					//SCHANNEL_LENGTH(5) = 0xFFFFFFFF;
					debugsrc2 = sIPCSharedGBAInst->DM2SAD_L | (sIPCSharedGBAInst->DM2SAD_H << 16);
					//senddebug32(debugsrc2);
					dmaBtimerinter();
					dmaBtimerinter();
					if(!(SCHANNEL_CR(5) & 0x80000000))SCHANNEL_CR(5) |= 0x80000000 |SOUND_REPEAT; //start now
					if(!(TIMER1_CR & TIMER_ENABLE))timerStart(1, ClockDivider_1,debugfr2 << 5, dmaBtimerinter);
				//senddebug32(debugfr2 << 5);
				}
				else
				{
					SCHANNEL_CR(5) &= 0x80000000; //stop now
					dmaApart = 0;
					SCHANNEL_SOURCE(5) = soundbuffB;
					SCHANNEL_REPEAT_POINT(5) = 8;
					SCHANNEL_LENGTH(5) = 0;
					////senddebug32(SCHANNEL_CR(5));
				}
			}
		}

	}



}
#endif
//ok
void vcount_handler(){
	//IPC ARM7/ARM9 process: handles touchscreen,time,etc
	gbaemu4ds_ipc();
}
void hblank_handler(){
	
}
void vblank_handler(){
	//REG_IPC_FIFO_TX = 0x3F00BEEF; //send cmd 0x3F00BEEF
	SendArm9Command(0x3F00BEEF,0x0,0x0,0x0);
	Wifi_Update();
	doFIFOUpdate();
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