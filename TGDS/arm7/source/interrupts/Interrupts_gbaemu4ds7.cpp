/*
			Copyright (C) 2017  Coto
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301
USA
*/

#include "specific_shared.h"
#include "dsregs.h"
#include "dsregs_asm.h"

#include "InterruptsARMCores_h.h"
#include "wifi_arm7.h"
#include "main.h"
#include "timer.h"


//User Handler Definitions
#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void Timer0handlerUser(){
}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void Timer1handlerUser(){
}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void Timer2handlerUser(){
}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void Timer3handlerUser(){
}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void HblankUser(){
}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void VblankUser(){
	//REG_IPC_FIFO_TX = 0x3F00BEEF; //send cmd 0x3F00BEEF
	//SendMultipleWordACK(0x3F00BEEF,0x0,0x0,0x0);	///freezes DS, so a way to trigger VBLANK in ARM9 needs to be rethought
	
	//if DMA1 enabled:
	if(((SpecificIPCAlign->DM1CNT_H)>>15)&1){
		
		//Start Timing: FIFO
		if( ((SpecificIPCAlign->DM1CNT_H>>12)&3) && (SpecificIPCAlign->dma1_donearm9 == false) ){
			//FIFO DMA is enabled, execute interrupts
			//Values should be written in TIMER CNT / DMA so far
			SendMultipleWordACK(0xc1710003,0x0,0x0,0x0);  //DMA1:FIFO CPUCheckDMA(3, 2);	
		}
		//DMA Done, play and perform the usual DMA procedure
		else if(SpecificIPCAlign->dma1_donearm9 == true){
			
			//Timer0
			//DMA A/B Sound Timer Select (0=Timer 0, 1=Timer 1)
			if(SpecificIPCAlign->TM0CNT_H & TIMER_ENABLE)
			{
				//Timer0 DMA_A?
				if(DMA_A_TIMERSEL == 0){
					/*
					//REG_SOUNDCNT_H = SOUNDA_VOLUME_100 | SOUNDA_LOUT | SOUNDA_ROUT | SOUNDA_FIFORESET;   // = 0x0604
					//REG_SOUNDCNT_L = SOUND_ENABLE;   // = 0x80
					//REG_TM0D = 65536 - (16777216 / soundFreq);
					//REG_TM0CNT = TIMER_ENABLE;   // = 0x80
					//REG_DM1SAD = (u32) soundDataAddr;
					//REG_DM1DAD = (u32) &(REG_SGFIFOA);
					//REG_DM1CNT_H = DMA_DEST_FIXED | DMA_REPEAT | DMA_WORD | DMA_MODE_FIFO | DMA_ENABLE;   // = 0xB640
					*/
					
					//senddebug32(0xc070);	//works here
					
					//TIMER0_DATA  = 65536 - (16777216 / (int)SpecificIPCAlign->TM0CNT_L);
					//TIMER0_CR = TIMER_ENABLE | TIMER_IRQ_REQ ;
					
					//GBA Freq
					
					//corrupted sound
					/*
					TIMER0_DATA  = 65536 - ((BUS_CLOCK) / SpecificIPCAlign->TM0CNT_L<<5);
					TIMER1_DATA =  65536 - (int)FIFO_BUFFER_SIZE;
					*/
					
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
					
					TIMERXDATA(0)  = (65536 - ((16777216*2)/ (sample_cycle)));
					TIMERXCNT(0) = TIMER_DIV_1 | TIMER_ENABLE | TIMER_IRQ_REQ;
					
					//TIMER1_DATA =  0x10000 - 1;	//
					//TIMER1_CR = TIMER_CASCADE | TIMER_IRQ_REQ | TIMER_ENABLE;
					
					//REG_DM1SAD = (u32) soundDataAddr;
					//REG_DM1DAD = (u32) &(REG_SGFIFOA);
					//REG_DM1CNT_H = DMA_DEST_FIXED | DMA_REPEAT | DMA_WORD | DMA_MODE_FIFO | DMA_ENABLE;   // = 0xB640
				}
				//Timer1 DMA_A?
				if(DMA_A_TIMERSEL == 1){
					//if(!(TIMER1_CR & TIMER_ENABLE))timerStart(1, ClockDivider_1,SpecificIPCAlign->TM0CNT_L<<5, timer1interrupt_thread);
				}
			}
				
			//Timer1
			//DMA A/B Sound Timer Select (0=Timer 0, 1=Timer 1)
			if(SpecificIPCAlign->TM1CNT_H & TIMER_ENABLE)
			{
				//Timer0 DMA_B?
				if(DMA_B_TIMERSEL == 0){
					//if(!(TIMER0_CR & TIMER_ENABLE))timerStart(0, ClockDivider_1,SpecificIPCAlign->TM0CNT_L<<5, timer0interrupt_thread);
				}
				//Timer1 DMA_B?
				if(DMA_B_TIMERSEL == 1){
					//if(!(TIMER1_CR & TIMER_ENABLE))timerStart(1, ClockDivider_1,SpecificIPCAlign->TM0CNT_L<<5, timer1interrupt_thread);
				}		
			}
			
			//Repeat Bit set?
			if(((SpecificIPCAlign->DM1CNT_H)>>9)&1){
				
			}
			else{
				SpecificIPCAlign->DM1CNT_H |= ~(1<<15); //DMA1 Disabled!
			}
				
			//Serve FIFO DMA1 IRQ
			if( (SpecificIPCAlign->GBA_IF & IRQ_DMA1)  & (SpecificIPCAlign->GBA_IE & IRQ_DMA1) ){
				
				//Serve DMA1
				SpecificIPCAlign->GBA_IF ^= IRQ_DMA1;
				SendMultipleWordACK(0xc4740005,0x4000202,SpecificIPCAlign->GBA_IF,0x0);	//update GBA IO Map
			}
			
			//timer makes sure frame is played, thus we dont need that frame anymore
			SpecificIPCAlign->dma1_donearm9 = false;
		}
		
		/*
		char charbuf[64];
		char * charptr;
		charptr = "hello how are you";
		arm7printdebug(charptr,sizeof(charbuf));
		*/
		
		//senddebug32(0xc070);	//works here
	}
}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void VcounterUser(){
}