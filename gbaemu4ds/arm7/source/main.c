#include "main.h"
#include "ipc/touch_ipc.h"
#include "wireless/wifi_arm7.h"

#include <nds.h>
#include <nds/arm7/audio.h>
#include <nds/interrupts.h>

#include "../../common/cpuglobal.h"
#include "../../common/gba_ipc.h"

#include "sound.h"
#include "fifo_handler.h"

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


#define noenterCriticalSection
//---------------------------------------------------------------------------------
int main() {
//---------------------------------------------------------------------------------
	REG_IE = 0;
	
	ledBlink(0);
	readUserSettings();
	
	u32 curIRQ = IRQ_TIMER0 | IRQ_HBLANK | IRQ_VBLANK | IRQ_VCOUNT | IRQ_LID | IRQ_FIFO_NOT_EMPTY | IRQ_NETWORK | IRQ_WIFI;	//IRQ_NETWORK == initClockIRQ(); / IRQ_WIFI is handled by libnds irq handler so excluded REG_IF in wifi_arm7.c
	
	irqInit();
	fifoInit();
	
	installWifiFIFO();
	
	irqSet(IRQ_HBLANK,			(void*)hblank_handler);
	irqSet(IRQ_VBLANK, 			(void*)vblank_handler);
	irqSet(IRQ_VCOUNT, 			(void*)vcount_handler);					//irq when VCOUNTER time
	irqSet(IRQ_LID, 			(void*)lid_open_irq_handler);			//irq when opening the LID of DS time
	irqSet(IRQ_FIFO_NOT_EMPTY, 	(void*)fifo_handler);					//irq when receiving fifo msg from arm9
	irqSet(IRQ_TIMER0, 			(void*)timer0interrupt_thread);	    	//timer0 irq
    
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
	
	while (true) {
		//sound alloc
		//0-3 matching gba
		//4-5 FIFO
		//ledBlink(1);
		//swiWaitForVBlank();
		swiWaitForVBlank();
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

//ok
void hblank_handler(){
	
}

//ok
bool ykeypp = false;
void vblank_handler(){
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
	
	SendArm9Command(0x3F00BEEF,0x0,0x0,0x0);	//send cmd 0x3F00BEEF
	Wifi_Update();
	doFIFOUpdate();
}
