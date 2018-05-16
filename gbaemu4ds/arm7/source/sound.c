#include "sound.h"
#include <nds.h>

#define SOUND_16BIT      (1<<29)
#define SOUND_8BIT       (0)

#include "../../../common/cpuglobal.h"
#include "../../../common/gba_ipc.h"
// 10    DMA Sound A Timer Select (0=Timer 0, 1=Timer 1)
u8 DMA_A_TIMERSEL = 0;
// 14    DMA Sound B Timer Select (0=Timer 0, 1=Timer 1)
u8 DMA_B_TIMERSEL = 0;
//Timer0 / Timer1 reserved for GBA

//writes a halfword/word to either DMA A or DMA B (Internal FIFO)
//if datatype == 0xa0,0xa2,0xa4,0xa6, then direct FIFO write
void updateInternalDMAFIFO(bool channelA, u32 value, u32 datatype, u32 fifowriteDataType ){
	struct sIPCSharedGBA* sIPCSharedGBAInst = GetsIPCSharedGBA();
	int DMAFIFOoffset = 0;
	if(channelA == true){	//DMA FIFO channel A
		DMAFIFOoffset = sIPCSharedGBAInst->fifoA_offset;
		switch(datatype){
			//emulator DMA fifo write (do update internal counter) (not gamecode)
			//Increase internal buffer (write FIFO logic handled outside)
			case(WRITEWORD_UPDATEDMAFIFO_8):{
				if((DMAFIFOoffset) < INTERNAL_FIFO_SIZE){
					sIPCSharedGBAInst->fifodmasA[DMAFIFOoffset+0] = value&0xff;
					DMAFIFOoffset++;
				}
			}
			break;
			case(WRITEWORD_UPDATEDMAFIFO_16):{
				if((DMAFIFOoffset + 1) < INTERNAL_FIFO_SIZE){
					sIPCSharedGBAInst->fifodmasA[DMAFIFOoffset+0] = value&0xff;
					sIPCSharedGBAInst->fifodmasA[DMAFIFOoffset+1] = (value>>8)&0xff;
					DMAFIFOoffset+=2;
				}
			}
			break;
			case(WRITEWORD_UPDATEDMAFIFO_32):{
				if((DMAFIFOoffset + 3) < INTERNAL_FIFO_SIZE){
					sIPCSharedGBAInst->fifodmasA[DMAFIFOoffset+0] = value&0xff;
					sIPCSharedGBAInst->fifodmasA[DMAFIFOoffset+1] = (value>>8)&0xff;
					sIPCSharedGBAInst->fifodmasA[DMAFIFOoffset+2] = (value>>16)&0xff;
					sIPCSharedGBAInst->fifodmasA[DMAFIFOoffset+3] = (value>>24)&0xff;
					DMAFIFOoffset+=4;
				}
			}
			break;
			
			//gamecode DMA fifo write (do update internal counter) / truncated always as : DMA SA: 0xa0 = u8 u16 u32 / 0xa1 u8 / 0xa2 = u8 u16 / DMA SB: same as DMA SA
			//simple fifo write(always u8) (updates the internal buffer counter as well)
			case (0xa0):{
				if(fifowriteDataType == WRITEWORD_PLAINDMAFIFO_8){
					sIPCSharedGBAInst->FIFO_A_L[0] = (value)&0xff;
					bool channelA = true;
					updateInternalDMAFIFO(channelA, value, WRITEWORD_UPDATEDMAFIFO_8, 0); //update the internal FIFO buffer
				}
				else if (fifowriteDataType == WRITEWORD_PLAINDMAFIFO_16){
					sIPCSharedGBAInst->FIFO_A_L[0] = (value)&0xff;
					sIPCSharedGBAInst->FIFO_A_L[1] = (value>>8)&0xff;
					bool channelA = true;
					updateInternalDMAFIFO(channelA, value, WRITEWORD_UPDATEDMAFIFO_16, 0); //update the internal FIFO buffer
				}
				else if (fifowriteDataType == WRITEWORD_PLAINDMAFIFO_32){
					sIPCSharedGBAInst->FIFO_A_L[0] = (value)&0xff;
					sIPCSharedGBAInst->FIFO_A_L[1] = (value>>8)&0xff;
					sIPCSharedGBAInst->FIFO_A_H[0] = (value>>16)&0xff;
					sIPCSharedGBAInst->FIFO_A_H[1] = (value>>24)&0xff;
					bool channelA = true;
					updateInternalDMAFIFO(channelA, value, WRITEWORD_UPDATEDMAFIFO_32, 0); //update the internal FIFO buffer
				}
			}
			break;
			case (0xa1):{
				if(fifowriteDataType == WRITEWORD_PLAINDMAFIFO_8){
					sIPCSharedGBAInst->FIFO_A_L[0] = (value)&0xff;
					bool channelA = true;
					updateInternalDMAFIFO(channelA, value, WRITEWORD_UPDATEDMAFIFO_8, 0);  //update the internal FIFO buffer
				}
			}
			break;
			case (0xa2):{
				if(fifowriteDataType == WRITEWORD_PLAINDMAFIFO_8){
					sIPCSharedGBAInst->FIFO_A_L[0] = (value)&0xff;
					bool channelA = true;
					updateInternalDMAFIFO(channelA, value, WRITEWORD_UPDATEDMAFIFO_8, 0);  //update the internal FIFO buffer
				}
				else if (fifowriteDataType == WRITEWORD_PLAINDMAFIFO_16){
					sIPCSharedGBAInst->FIFO_A_L[0] = (value)&0xff;
					sIPCSharedGBAInst->FIFO_A_L[1] = (value>>8)&0xff;
					bool channelA = true;
					updateInternalDMAFIFO(channelA, value, WRITEWORD_UPDATEDMAFIFO_16, 0);  //update the internal FIFO buffer
				}
			}
			break;
		}
		sIPCSharedGBAInst->fifoA_offset = DMAFIFOoffset;
	}
	else{ //DMA FIFO channel B 
		DMAFIFOoffset = sIPCSharedGBAInst->fifoB_offset;		
		switch(datatype){
			//emulator DMA fifo write (do update internal counter) (not gamecode)
			//Increase internal buffer (write FIFO logic handled outside)
			case(WRITEWORD_UPDATEDMAFIFO_8):{
				if((DMAFIFOoffset) < INTERNAL_FIFO_SIZE){
					sIPCSharedGBAInst->fifodmasB[DMAFIFOoffset+0] = value&0xff;
					DMAFIFOoffset++;
				}
			}
			break;
			case(WRITEWORD_UPDATEDMAFIFO_16):{
				if((DMAFIFOoffset + 1) < INTERNAL_FIFO_SIZE){
					sIPCSharedGBAInst->fifodmasB[DMAFIFOoffset+0] = value&0xff;
					sIPCSharedGBAInst->fifodmasB[DMAFIFOoffset+1] = (value>>8)&0xff;
					DMAFIFOoffset+=2;
				}
			}
			break;
			case(WRITEWORD_UPDATEDMAFIFO_32):{
				if((DMAFIFOoffset + 3) < INTERNAL_FIFO_SIZE){
					sIPCSharedGBAInst->fifodmasB[DMAFIFOoffset+0] = value&0xff;
					sIPCSharedGBAInst->fifodmasB[DMAFIFOoffset+1] = (value>>8)&0xff;
					sIPCSharedGBAInst->fifodmasB[DMAFIFOoffset+2] = (value>>16)&0xff;
					sIPCSharedGBAInst->fifodmasB[DMAFIFOoffset+3] = (value>>24)&0xff;
					DMAFIFOoffset+=4;
				}
			}
			break;
			
			//gamecode DMA fifo write (do update internal counter) / truncated always as : DMA SA: 0xa0 = u8 u16 u32 / 0xa1 u8 / 0xa2 = u8 u16 / DMA SB: same as DMA SA
			//simple fifo write(always u8) (updates the internal buffer counter as well)
			case (0xa4):{
				if(fifowriteDataType == WRITEWORD_PLAINDMAFIFO_8){
					sIPCSharedGBAInst->FIFO_B_L[0] = (value)&0xff;
					bool channelA = false;
					updateInternalDMAFIFO(channelA, value, WRITEWORD_UPDATEDMAFIFO_8, 0);	//update the internal FIFO buffer
				}
				else if (fifowriteDataType == WRITEWORD_PLAINDMAFIFO_16){
					sIPCSharedGBAInst->FIFO_B_L[0] = (value)&0xff;
					sIPCSharedGBAInst->FIFO_B_L[1] = (value>>8)&0xff;
					bool channelA = false;
					updateInternalDMAFIFO(channelA, value, WRITEWORD_UPDATEDMAFIFO_16, 0);  //update the internal FIFO buffer
				}
				else if (fifowriteDataType == WRITEWORD_PLAINDMAFIFO_32){
					sIPCSharedGBAInst->FIFO_B_L[0] = (value)&0xff;
					sIPCSharedGBAInst->FIFO_B_L[1] = (value>>8)&0xff;
					sIPCSharedGBAInst->FIFO_B_H[0] = (value>>16)&0xff;
					sIPCSharedGBAInst->FIFO_B_H[1] = (value>>24)&0xff;
					bool channelA = false;
					updateInternalDMAFIFO(channelA, value, WRITEWORD_UPDATEDMAFIFO_32, 0);  //update the internal FIFO buffer
				}
			}
			break;
			case (0xa5):{
				if(fifowriteDataType == WRITEWORD_PLAINDMAFIFO_8){
					sIPCSharedGBAInst->FIFO_B_L[0] = (value)&0xff;
					bool channelA = false;
					updateInternalDMAFIFO(channelA, value, WRITEWORD_UPDATEDMAFIFO_8, 0);  //update the internal FIFO buffer
				}
			}
			break;
			case (0xa6):{
				if(fifowriteDataType == WRITEWORD_PLAINDMAFIFO_8){
					sIPCSharedGBAInst->FIFO_B_L[0] = (value)&0xff;
					bool channelA = false;
					updateInternalDMAFIFO(channelA, value, WRITEWORD_UPDATEDMAFIFO_8, 0);  //update the internal FIFO buffer
				}
				else if (fifowriteDataType == WRITEWORD_PLAINDMAFIFO_16){
					sIPCSharedGBAInst->FIFO_B_L[0] = (value)&0xff;
					sIPCSharedGBAInst->FIFO_B_L[1] = (value>>8)&0xff;
					bool channelA = false;
					updateInternalDMAFIFO(channelA, value, WRITEWORD_UPDATEDMAFIFO_16, 0); //update the internal FIFO buffer
				}
			}
			break;
		}
		sIPCSharedGBAInst->fifoB_offset = DMAFIFOoffset;
	}
}

bool dmasa_play = false;
bool dmasb_play = false;
void playdmafifo(){
	if(dmasa_play == true){
		struct sIPCSharedGBA* sIPCSharedGBAInst = GetsIPCSharedGBA();
		u32 * soundch_dmaAptr = 0;
		u32 * soundch_dmaBptr = 0;
		if(sIPCSharedGBAInst->dma1_donearm9 == true){
			soundch_dmaAptr = (u32*)&sIPCSharedGBAInst->fifodmasA[0];
		}
		if(sIPCSharedGBAInst->dma2_donearm9 == true){
			soundch_dmaBptr = (u32*)&sIPCSharedGBAInst->fifodmasB[0];
		}
		if((u32*)soundch_dmaAptr != 0){
			//senddebug32(0xc171);	//works here
			//Timer DMA FIFO
			int channel = 4;	//NDS 4 DMA A /5 DMA B
			SCHANNEL_SOURCE(channel) = (u32)soundch_dmaAptr;
			
			//repeat
			if(((sIPCSharedGBAInst->DM1CNT_H)>>9)&1){
				SCHANNEL_REPEAT_POINT(channel) = (int)INTERNAL_FIFO_SIZE;
			}
			else
				SCHANNEL_REPEAT_POINT(channel) = 0;
			int volume = 120;
			
			SCHANNEL_LENGTH(channel) = INTERNAL_FIFO_SIZE;
			
			//TM0CNT_H: 0-1   Prescaler Selection (0=F/1, 1=F/64, 2=F/256, 3=F/1024)
			int presc = 0;
						
			if((sIPCSharedGBAInst->TM0CNT_L&3)==0){
				presc = 1;
			}
			
			if((sIPCSharedGBAInst->TM0CNT_L&3)==1){
				presc = 64;
			}
			
			if((sIPCSharedGBAInst->TM0CNT_L&3)==2){
				presc = 256;
			}
			
			if((sIPCSharedGBAInst->TM0CNT_L&3)==3){
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
			switch((sIPCSharedGBAInst->SOUNDBIAS>>14)&0x3){
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
			//play the dmaFIFO A buffer.
			if(!(SCHANNEL_CR(4) & 0x80000000)){
				SCHANNEL_SOURCE(4) = (u32)&sIPCSharedGBAInst->fifodmasA[0];
				SCHANNEL_CR(4) |= SCHANNEL_ENABLE | SOUND_8BIT;   //play //SCHANNEL_CR(channel) = SCHANNEL_ENABLE | SOUND_VOL(0x7f) | SOUND_PAN(0x0) | SOUND_8BIT | ((((sIPCSharedGBAInst->DM1CNT_H)>>9)&1) ? SOUND_REPEAT : SOUND_ONE_SHOT);
			}
		}
	}
	
	if(dmasb_play == true){
		//when above logic work copy paste here...for the other channel
	}
}

void doFIFOUpdate(){
	playdmafifo();
	if(dmasa_play == true){	//Updates async DMA FIFOs
		if(!(SCHANNEL_CR(4) & 0x80000000)){
			dmasa_play = false;
		}
	}
	if(dmasb_play == true){
		if(!(SCHANNEL_CR(5) & 0x80000000)){
			dmasb_play = false;
		}
	}
}

//Timer 0 will drive DMA (frequency) Update/Play
//TIMER0 IRQ: //40000C6h - DMA1CNT_H - DMA 1 Control (R/W)
void timer0interrupt_thread(){
	struct sIPCSharedGBA * sIPCSharedGBAInst = GetsIPCSharedGBA();
	bool DoDMA1or2ShadowCopy = false;	//true == bring GBA IRQs here to do GBADMA handle / false == ((GBA DMA IF & IE ) == true), do playfifo(); instead
    //Timer0 for DMASoundA?
	if(DMA_A_TIMERSEL == 0){
		//if (DMA1 enabled && DMA FIFO COPY raised IRQ && not already a pending play): execute
		if( (((sIPCSharedGBAInst->DM1CNT_H>>15)&1)==1) && ((sIPCSharedGBAInst->GBAIF & IRQ_DMA1)  && ((sIPCSharedGBAInst->GBAIE & IRQ_DMA1))) && (dmasa_play == false) ){
			//DMA Fifo Playback
			dmasa_play = true;
			//Repeat Bit:  Real hardware updates the repeat bit after transfer, here I update it after play since the transfer->play takes place first. (FIFO irq dependant)
			if(((sIPCSharedGBAInst->DM1CNT_H>>9)&1)==1){
				sIPCSharedGBAInst->DM1CNT_H |= (1<<15); //DMA1 Enabled!
				SCHANNEL_CR(4) |= SOUND_REPEAT;		//for now until further playback is tested
			}
			else{
				sIPCSharedGBAInst->DM1CNT_H |= ~(1<<15); //DMA1 Disabled!
				SCHANNEL_CR(4) &= ~SOUND_REPEAT;	//for now until further playback is tested
			}
			sIPCSharedGBAInst->GBAIF ^= IRQ_DMA1;	//Serve DMA1
			SendArm9Command(ARM7_DOGBASHADOWUPDATE,0,0,0);	//do gbashadowupdate
		}
		else{
			//Start Timing: FIFO
			if(((sIPCSharedGBAInst->DM1CNT_H>>12)&3)==3){
				//Values should be written in TIMER CNT / DMA so far
				SendArm9Command(ARM7_CPUCHECKDMA1FIFO,(u32)0x00000003,(u32)0x00000002,0);  //CPUCheckDMA(3, 2);     //Serve DMA1 FIFO
			}
			else{
				DoDMA1or2ShadowCopy = true;	//doShadowCopy if DMA is disabled (by default)
			}
		}
	}
    
    //Timer0 for DMASoundB?
    if(DMA_B_TIMERSEL == 0){
        //if (DMA2 enabled && DMA FIFO COPY raised IRQ && not already a pending play): execute
		if( (((sIPCSharedGBAInst->DM2CNT_H>>15)&1)==1)  && ((sIPCSharedGBAInst->GBAIF & IRQ_DMA2)  && ((sIPCSharedGBAInst->GBAIE & IRQ_DMA2))) && (dmasb_play == false) ){
			//DMA Fifo Playback
			dmasb_play = true;
			//Repeat Bit:  Real hardware updates the repeat bit after transfer, here I update it after play since the transfer->play takes place first. (FIFO irq dependant)
			if(((sIPCSharedGBAInst->DM2CNT_H>>9)&1)==1){
				sIPCSharedGBAInst->DM2CNT_H |= (1<<15); //DMA2 Enabled!
				SCHANNEL_CR(5) |= SOUND_REPEAT;		//for now until further playback is tested
			}
			else{
				sIPCSharedGBAInst->DM2CNT_H |= ~(1<<15); //DMA2 Disabled!
				SCHANNEL_CR(5) &= ~SOUND_REPEAT;	//for now until further playback is tested
			}
			sIPCSharedGBAInst->GBAIF ^= IRQ_DMA2;	//Serve DMA2
			SendArm9Command(ARM7_DOGBASHADOWUPDATE,0,0,0);	//do gbashadowupdate
		}
		else{
			//Start Timing: FIFO
			if(((sIPCSharedGBAInst->DM2CNT_H>>12)&3)==3){
				//Values should be written in TIMER CNT / DMA so far
				SendArm9Command(ARM7_CPUCHECKDMA2FIFO,(u32)0x00000003,(u32)0x00000004,0);  //CPUCheckDMA(3, 4);   //Serve DMA2 FIFO
			}
			else{
				DoDMA1or2ShadowCopy = true;	//doShadowCopy if DMA is disabled (by default)
			}
		}
    }
	if(DoDMA1or2ShadowCopy == true){
		//call shadow ARM9_DOGBASHADOWCOPY only if (any) DMA Channel is enabled.
		SendArm9Command(ARM9_DOGBASHADOWCOPY,0,0,0);
	}
	//SendArm9Command(0xc4740006,0x0,0x0,0); //timer0 test
	//REG_IF = IRQ_TIMER0;	//timer0 irqs work! not needed to clean IRQ TIMER since the exception handler already acknowledges an interrupt
}
