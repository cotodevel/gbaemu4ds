#include "sound.h"

#include "typedefsTGDS.h"
#include "dsregs.h"
#include "dsregs_asm.h"

#include "specific_shared.h"
#include "ipcfifoTGDS.h"

#include "timerTGDS.h"
#include "dmaTGDS.h"

u8* soundbuffA = 0;
u8* soundbuffB = 0;

u8 dmaApart = 0;
u8 dmaBpart = 0;

u32 dmabuffer = 0;

// 10    DMA Sound A Timer Select (0=Timer 0, 1=Timer 1)
u8 DMA_A_TIMERSEL = 0;

// 14    DMA Sound B Timer Select (0=Timer 0, 1=Timer 1)
u8 DMA_B_TIMERSEL = 0;


/*
//@ IPC
u16 SOUNDCNT_L = 0;
u16 SOUNDCNT_H = 0;

u16 TM0CNT_L = 0;
u16 TM1CNT_L = 0;

u16 TM0CNT_H = 0;
u16 TM1CNT_H = 0;

u16 DMA1CNT_H = 0;
u16 DMA2CNT_H = 0;


u16 DMA1SAD_L = 0;
u16 DMA1SAD_H  = 0;
u16 DMA1DAD_L = 0;
u16 DMA1DAD_H  = 0;

u16 DMA2SAD_L = 0;
u16 DMA2SAD_H  = 0;
u16 DMA2DAD_L = 0;
u16 DMA2DAD_H  = 0;

*/


//debug stuff
//vu32 debugsrc1 = 0;	//GBA DMA 1 IO SRC
//vu32 debugsrc2  = 0;//GBA DMA 2 IO SRC
//vu32 debugfr1 = 0;
//vu32 debugfr2  = 0;


void dmaAtimerinter()
{


//old gbaemu4ds fifo logic: linear buffer gets memcpy'd source from GBA DMA IO, and a linearbuffer + offset feeds the SOUND_CHANNELS for DMA (native) mode
/*
	if(dmaApart){
		//REG_IPC_FIFO_TX = debugsrc1 + 0x400001;
		SendArm9Command((debugsrc1 + 0x400001),0x0,0x0,0x0);	//1 == 0x10 offset + FIFOBUF
	}
	else {
		//REG_IPC_FIFO_TX = debugsrc1 + 0x400000;
		SendArm9Command((debugsrc1 + 0x400000),0x0,0x0,0x0);	//0 == 0x0 + FIFOBUF
	}
	debugsrc1+=0x10;
	if(dmaApart == 0) dmaApart = 1;
	else dmaApart = 0;
*/


	//SendArm9Command(0xc6760000,0x0,0x0,0x0);	//DMA SA Works ok!
	
}

void dmaBtimerinter()
{
//old gbaemu4ds fifo logic: linear buffer gets memcpy'd source from GBA DMA IO, and a linearbuffer + offset feeds the SOUND_CHANNELS for DMA (native) mode
/*
	if(dmaApart){
		//REG_IPC_FIFO_TX = debugsrc1 + 0x400002;
		SendArm9Command((debugsrc1 + 0x400002),0x0,0x0,0x0);	//2 == 0x50 offset + FIFOBUF
	}
	else { 
		//REG_IPC_FIFO_TX = debugsrc1 + 0x400003;
		SendArm9Command((debugsrc1 + 0x400003),0x0,0x0,0x0);	//3 == 0x60 offset + FIFOBUF
	}

	debugsrc2+=0x10;
	if(dmaBpart == 0) dmaBpart = 1;
	else dmaBpart = 0;
*/

	//SendArm9Command(0xc6760000,0x0,0x0,0x0);	//DMA SB Works ok!
	
}





//GBA IO -> NDS update hardware opcodes

void updatevol()
{

	/*****************************************************/
	/*                  Update Voll                      */
	/*****************************************************/


	//Sound_chan = (Volume_Right * enabeled + Volume_Left * enabeled) * (Soundcnt(1,2,4))*static_for_max
	//DMA = (Soundcnt(1,2) * enabeled + Soundcnt(1,2) * enabeled) * sataic_for_max
	SCHANNEL_CR(4) = (SCHANNEL_CR(4) & ~0xFF) | ((( 1 + ((SpecificIPCAlign->SOUNDCNT_H & 0x4) >> 2))*((SpecificIPCAlign->SOUNDCNT_H & (1<<8)) >> 8) + ( 1 + ((SpecificIPCAlign->SOUNDCNT_H & 0x4) >> 2))*((SpecificIPCAlign->SOUNDCNT_H & (1<<9)) >> 9))*31);     //max:124
	SCHANNEL_CR(5) = (SCHANNEL_CR(5) & ~0xFF) | ((( 1 + ((SpecificIPCAlign->SOUNDCNT_H & 0x8) >> 3))*((SpecificIPCAlign->SOUNDCNT_H & (1<<12)) >> 12) + ( 1 + ((SpecificIPCAlign->SOUNDCNT_H & 0x8) >> 3))*((SpecificIPCAlign->SOUNDCNT_H & (1<<13)) >> 13))*31); //max:124
	int Vol = SpecificIPCAlign->SOUNDCNT_H & 0x3;
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
	int Masterright = SpecificIPCAlign->SOUNDCNT_L & 0x7;
	int Masterleft =  (SpecificIPCAlign->SOUNDCNT_L << 4) & 0x7;
	SCHANNEL_CR(0) = (SCHANNEL_CR(0) & ~0xFF) | ((Masterright * ((SpecificIPCAlign->SOUNDCNT_L & (1<<8)) >> 8) + Masterleft * ((SpecificIPCAlign->SOUNDCNT_L & (1<<12)) >> 12) ) * Vol *2);  //max:112
	SCHANNEL_CR(1) = (SCHANNEL_CR(1) & ~0xFF) | ((Masterright * ((SpecificIPCAlign->SOUNDCNT_L & (1<<9)) >> 9) + Masterleft * ((SpecificIPCAlign->SOUNDCNT_L & (1<<13)) >> 13) ) * Vol *2);  //max:112
	SCHANNEL_CR(2) = (SCHANNEL_CR(2) & ~0xFF) | ((Masterright * ((SpecificIPCAlign->SOUNDCNT_L & (1<<10)) >> 10) + Masterleft * ((SpecificIPCAlign->SOUNDCNT_L & (1<<14)) >> 14) ) * Vol *2);//max:112
	SCHANNEL_CR(3) = (SCHANNEL_CR(3) & ~0xFF) | ((Masterright * ((SpecificIPCAlign->SOUNDCNT_L & (1<<11)) >> 11) + Masterleft * ((SpecificIPCAlign->SOUNDCNT_L & (1<<15)) >> 15) ) * Vol *2);//max:112


    /*****************************************************/
	/*                  Update Panning                   */
	/*****************************************************/
	

	if(SpecificIPCAlign->SOUNDCNT_H & (1<<9)) SCHANNEL_CR(4) = (SCHANNEL_CR(4) & ~0xFF0000);
	if(SpecificIPCAlign->SOUNDCNT_H & (1<<8)) SCHANNEL_CR(4) = (SCHANNEL_CR(4) & ~0xFF0000) | 0x7F0000;
	if(SpecificIPCAlign->SOUNDCNT_H & (1<<8) && SpecificIPCAlign->SOUNDCNT_H & (1<<9)) SCHANNEL_CR(4) = (SCHANNEL_CR(4) & ~0xFF0000) | 0x400000; //same on both

	if(SpecificIPCAlign->SOUNDCNT_H & (1<<13)) SCHANNEL_CR(5) = (SCHANNEL_CR(5) & ~0xFF0000);
	if(SpecificIPCAlign->SOUNDCNT_H & (1<<12)) SCHANNEL_CR(5) = (SCHANNEL_CR(5) & ~0xFF0000) | 0x7F0000;
	if(SpecificIPCAlign->SOUNDCNT_H & (1<<12) && SpecificIPCAlign->SOUNDCNT_H & (1<<13)) SCHANNEL_CR(5) = (SCHANNEL_CR(5) & ~0xFF0000) | 0x400000; //same on both

	int right = SpecificIPCAlign->SOUNDCNT_L & 7;
	int left = (SpecificIPCAlign->SOUNDCNT_L << 4) & 7;
	int tempmixedvol1_4 = 0;
	if((left + right) != 0) //don't work
	{
		tempmixedvol1_4 = ((right*0x7F0000)/(right + left) & 0x7F0000);
	}
	
	if(SpecificIPCAlign->SOUNDCNT_L & (1<<12)) SCHANNEL_CR(0) = (SCHANNEL_CR(0) & ~0xFF0000);
	if(SpecificIPCAlign->SOUNDCNT_L & (1<<8)) SCHANNEL_CR(0) = (SCHANNEL_CR(0) & ~0xFF0000) | 0x7F0000;
	if(SpecificIPCAlign->SOUNDCNT_L & (1<<8) && SpecificIPCAlign->SOUNDCNT_L & (1<<12)) SCHANNEL_CR(0) = (SCHANNEL_CR(0) & ~0xFF0000) | tempmixedvol1_4;

	if(SpecificIPCAlign->SOUNDCNT_L & (1<<13)) SCHANNEL_CR(4) = (SCHANNEL_CR(1) & ~0xFF0000);
	if(SpecificIPCAlign->SOUNDCNT_L & (1<<9)) SCHANNEL_CR(4) = (SCHANNEL_CR(1) & ~0xFF0000) | 0x7F0000;
	if(SpecificIPCAlign->SOUNDCNT_L & (1<<9) && SpecificIPCAlign->SOUNDCNT_L & (1<<13)) SCHANNEL_CR(1) = (SCHANNEL_CR(1) & ~0xFF0000) | tempmixedvol1_4; 

	if(SpecificIPCAlign->SOUNDCNT_L & (1<<14)) SCHANNEL_CR(4) = (SCHANNEL_CR(2) & ~0xFF0000);
	if(SpecificIPCAlign->SOUNDCNT_L & (1<<10)) SCHANNEL_CR(4) = (SCHANNEL_CR(2) & ~0xFF0000) | 0x7F0000;
	if(SpecificIPCAlign->SOUNDCNT_L & (1<<10) && SpecificIPCAlign->SOUNDCNT_L & (1<<14)) SCHANNEL_CR(2) = (SCHANNEL_CR(2) & ~0xFF0000) | tempmixedvol1_4;

	if(SpecificIPCAlign->SOUNDCNT_L & (1<<15)) SCHANNEL_CR(4) = (SCHANNEL_CR(3) & ~0xFF0000);
	if(SpecificIPCAlign->SOUNDCNT_L & (1<<11)) SCHANNEL_CR(4) = (SCHANNEL_CR(3) & ~0xFF0000) | 0x7F0000;
	if(SpecificIPCAlign->SOUNDCNT_L & (1<<11) && SpecificIPCAlign->SOUNDCNT_L & (1<<15)) SCHANNEL_CR(3) = (SCHANNEL_CR(3) & ~0xFF0000) | tempmixedvol1_4;

}



//Timer0 / Timer1 reserved for GBA
void timer0interrupt_thread(){
	
	//senddebug32(0xc171);	//works here
	
	u32 * soundch_dmaAptr = 0;
	u32 * soundch_dmaBptr = 0;
	
	if(SpecificIPCAlign->dma1_donearm9 == true){
		soundch_dmaAptr = (u32*)SpecificIPCAlign->fifodmasA;
	}
	
	if(SpecificIPCAlign->dma2_donearm9 == true){
		soundch_dmaBptr = (u32*)SpecificIPCAlign->fifodmasB;
	}
	
	if((u32*)soundch_dmaAptr != 0){
		//senddebug32(0xc171);	//works here
	
		//Timer DMA FIFO
		int channel = 4;	//NDS 4 DMA A /5 DMA B
		SCHANNEL_SOURCE(channel) = (u32)soundch_dmaAptr;
		
		//repeat
		if(((SpecificIPCAlign->DM1CNT_H)>>9)&1){
			SCHANNEL_REPEAT_POINT(channel) = (int)FIFO_BUFFER_SIZE;
		}
		else
			SCHANNEL_REPEAT_POINT(channel) = 0;
		int volume = 120;
		
		SCHANNEL_LENGTH(channel) = FIFO_BUFFER_SIZE;
		
		//TM0CNT_H: 0-1   Prescaler Selection (0=F/1, 1=F/64, 2=F/256, 3=F/1024)
		int presc = 0;
					
		if((SpecificIPCAlign->TM0CNT_L&3)==0){
			presc = 1;
		}
		
		if((SpecificIPCAlign->TM0CNT_L&3)==1){
			presc = 64;
		}
		
		if((SpecificIPCAlign->TM0CNT_L&3)==2){
			presc = 256;
		}
		
		if((SpecificIPCAlign->TM0CNT_L&3)==3){
			presc = 1024;
		}
		
		//14-15  Amplitude Resolution/Sampling Cycle (Default=0, see below)
		//16-31  Not used
  
		//Amplitude Resolution/Sampling Cycle (0-3):
		int sample_cycle = 0;
		
		//  0  9bit / 32.768kHz   (Default, best for DMA channels A,B)
		//  1  8bit / 65.536kHz
		//  2  7bit / 131.072kHz
		//  3  6bit / 262.144kHz  (Best for PSG channels 1-4)
		switch((SpecificIPCAlign->SOUNDBIAS>>14)&0x3){
			case 0:{
				sample_cycle = 32768;
			}
			break;
			case 1:{
				sample_cycle = 65536;
			}
			break;
			case 2:{
				sample_cycle = 131072;
			}
			break;
			case 3:{
				sample_cycle = 262144;
			}
			break;
			
		}
		
		
		//(65536 - (16777216 / Frequency))
		//SCHANNEL_TIMER(channel) = SOUND_FREQ(((16777216)/ (sample_cycle)));
		updatetakt();
		updatevol();
		SCHANNEL_CR(channel) |= SCHANNEL_ENABLE | SOUND_8BIT ;
		//SCHANNEL_CR(channel) = SCHANNEL_ENABLE | SOUND_VOL(0x7f) | SOUND_PAN(0x0) | SOUND_8BIT | ((((SpecificIPCAlign->DM1CNT_H)>>9)&1) ? SOUND_REPEAT : SOUND_ONE_SHOT);
		
		//SendArm9Command(0xc6760000,0x0,0x0,0x0);
		
	}
	
	
	
	//REG_IF = IRQ_TIMER0;
	//timer0 irqs work! not needed to clean IRQ TIMER since the tick irq is timed
}

void timer1interrupt_thread(){
	
	//ori
	/*
	u32 * soundch_dmaAptr = 0;
	u32 * soundch_dmaBptr = 0;
	
	if(SpecificIPCAlign->dma1_donearm9 == true){
		soundch_dmaAptr = (u32*)SpecificIPCAlign->fifodmasA;
	}
	
	if(SpecificIPCAlign->dma2_donearm9 == true){
		soundch_dmaBptr = (u32*)SpecificIPCAlign->fifodmasB;
	}
	
	if((u32*)soundch_dmaAptr != 0){
		//senddebug32(0xc171);	//works here
	
		//Timer DMA FIFO
		int channel = 4;	//NDS 4 DMA A /5 DMA B
		SCHANNEL_SOURCE(channel) = (u32)soundch_dmaAptr;
		
		//repeat
		if(((SpecificIPCAlign->DM1CNT_H)>>9)&1){
			SCHANNEL_REPEAT_POINT(channel) = (int)FIFO_BUFFER_SIZE;
		}
		else
			SCHANNEL_REPEAT_POINT(channel) = 0;
		int volume = 120;
		SCHANNEL_LENGTH(channel) = FIFO_BUFFER_SIZE;
		SCHANNEL_TIMER(channel) = ((BUS_CLOCK) / (SpecificIPCAlign->TM0CNT_L<<5));
		SCHANNEL_CR(channel) = SCHANNEL_ENABLE | (volume<<0) | (64<<16) | (0 << 29) | ((((SpecificIPCAlign->DM1CNT_H)>>9)&1) ? SOUND_REPEAT : SOUND_ONE_SHOT);
		
	}
	*/
	
	
	//REG_IF = IRQ_TIMER1;
}

//Free NDS Timer (Timer3 is for WIFI)
//Cascade Timer 
void timer2interrupt_thread(){
	
}

void updatetakt()
{
	//FIFO A
	if(DMA_A_TIMERSEL == 0)
	{
		//SCHANNEL_TIMER(4) = debugfr1 = (((-SpecificIPCAlign->TM0CNT_L) << seek) & 0xFFFF) << 1;
		SCHANNEL_TIMER(4) = (SpecificIPCAlign->TM0CNT_L);
	}
	else
	{
		//SCHANNEL_TIMER(4) = debugfr1 = (((-SpecificIPCAlign->TM1CNT_L) & 0xFFFF) << seek) << 1;
		SCHANNEL_TIMER(4) = (SpecificIPCAlign->TM1CNT_L);
	}
	//FIFO B
	if(DMA_B_TIMERSEL == 0)
	{
		//SCHANNEL_TIMER(5) = debugfr2 = (((-SpecificIPCAlign->TM0CNT_L) << seek) & 0xFFFF) << 1;
		SCHANNEL_TIMER(5) = (SpecificIPCAlign->TM0CNT_L);
	}
	else
	{
		//SCHANNEL_TIMER(5) = debugfr2 = (((-SpecificIPCAlign->TM1CNT_L) << seek) & 0xFFFF) << 1; //everything is 2 times faster than on ther gba here
		SCHANNEL_TIMER(5) = (SpecificIPCAlign->TM1CNT_L); //everything is 2 times faster than on ther gba here
	}
}