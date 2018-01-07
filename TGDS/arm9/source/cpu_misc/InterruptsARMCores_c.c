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

#include "typedefsTGDS.h"
#include "dsregs.h"
#include "dsregs_asm.h"

#include "InterruptsARMCores_h.h"
#include "interrupts.h"
#include "specific_shared.h"

#ifdef ARM7
#include "wifi_arm7.h"
#endif

#ifdef ARM9
#include "wifi_arm9.h"
#include "dswnifi_lib.h"
#include "main.h"
#include "GBA.h"
#endif


#ifdef ARM7
u32 interrupts_to_wait_arm7 = 0;
#endif

//u32 GLOBAL_IME = 0;

#ifdef ARM9
//__attribute__((section(".dtcm")))
//volatile u32 interrupts_to_wait_arm9 = 0;
#endif

//stubs because we don't disable IME on DSWIFI
/*
void SGIP_INTR_PROTECT(){}
void SGIP_INTR_REPROTECT(){}
void SGIP_INTR_UNPROTECT(){}
void SGIP_WAITEVENT(){}
*/

/*
void IRQInit(){
	
	//fifo setups
	REG_IME = 0;
	
	int i = 0;
	for(i = 0; i <32 ; i++){
		if((REG_IF & (1 << i)) == (1 << i)){
			REG_IF = (1 << i);
		}
	}
	
	REG_IE = 0;
	
	REG_IPC_SYNC = 0;
    REG_IPC_FIFO_CR = RECV_FIFO_IPC_IRQ  | SEND_FIFO_IPC_IRQ | FIFO_IPC_ENABLE;
	
	//set up ppu: do irq on hblank/vblank/vcount/and vcount line is 159
    REG_DISPSTAT = REG_DISPSTAT | DISP_HBLANK_IRQ | DISP_VBLANK_IRQ | (DISP_YTRIGGER_IRQ | (VCOUNT_LINE_INTERRUPT << 15));
	
	u32 interrupts_to_wait_armX = 0;
	
	#ifdef ARM7
	interrupts_to_wait_armX = interrupts_to_wait_arm7 = IRQ_TIMER1 | IRQ_HBLANK | IRQ_VBLANK | IRQ_VCOUNT | IRQ_RECVFIFO_NOT_EMPTY  | IRQ_SENDFIFO_EMPTY;
	#endif
	
	#ifdef ARM9
	interrupts_to_wait_armX = interrupts_to_wait_arm9 = IRQ_HBLANK| IRQ_VBLANK | IRQ_VCOUNT | IRQ_RECVFIFO_NOT_EMPTY | IRQ_SENDFIFO_EMPTY;
	#endif
	
	REG_IE = interrupts_to_wait_armX; 
	
	INTERRUPT_VECTOR = (u32)&InterruptServiceRoutineARMCores;
	REG_IME = 1;
}
*/



//Software bios irq more or less emulated. (replaces default NDS bios for some parts)

//Runs on ARM9 for now... until GBAEMU4DS in TGDS is stable.
#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void NDS_IRQHandlerSpecial(){
	
	u32 REG_IE_SET = REG_IF & REG_IE;
	
	// 2/2
	REG_IE_SET |= SWI_CHECKBITS;
	
	/*
	#ifdef ARM7
	//arm7 wifi cart irq
	if(REG_IE_SET & IRQ_WIFI){
		//Wifi_Interrupt();
		REG_IF = IRQ_WIFI;
	}
	
	//clock //could cause freezes
	if(REG_IE_SET & IRQ_RTCLOCK){
		REG_IF = IRQ_RTCLOCK;
	}
	#endif
	*/
	
	////			Common
	/*
	if(REG_IE_SET & IRQ_TIMER0){
		Timer0handlerUser();
		REG_IF = IRQ_TIMER0;
	}
	
	if(REG_IE_SET & IRQ_TIMER1){
		#ifdef ARM7
		//arm7 sound here...
		#endif
		Timer1handlerUser();
		REG_IF = IRQ_TIMER1;
	}
	
	if(REG_IE_SET & IRQ_TIMER2){
		Timer2handlerUser();
		REG_IF = IRQ_TIMER2;
	}
	
	if(REG_IE_SET & IRQ_TIMER3){
		#ifdef ARM9
		//wifi arm9 irq
		//Timer_50ms();
		#endif
		Timer3handlerUser();
		REG_IF = IRQ_TIMER3;
	}
	*/
	if(REG_IE_SET & IRQ_HBLANK){
		HblankHandler();
		//REG_IF = IRQ_HBLANK;
	}
	/*
	if(REG_IE_SET & IRQ_VBLANK){
		
		#ifdef ARM7
		Wifi_Update();
		#endif
		
		#ifdef ARM9
		//handles DS-DS Comms
		if(doMULTIDaemon() >=0){
		}
		#endif
		
		//key event between frames
		do_keys();
		
		VblankUser();
	
		REG_IF = IRQ_VBLANK;
	}
	*/
	/*
	if(REG_IE_SET & IRQ_VCOUNT){
		#ifdef ARM7
		doSPIARM7IO();
		#endif
		VcounterUser();
		REG_IF = IRQ_VCOUNT;
	}
	*/
	
	/*
	if(REG_IE_SET & IRQ_SENDFIFO_EMPTY){
		HandleFifoEmpty();
		REG_IF = IRQ_SENDFIFO_EMPTY;
	}
	*/
	if(REG_IE_SET & IRQ_RECVFIFO_NOT_EMPTY){
		HandleFifo();
		//REG_IF=IRQ_RECVFIFO_NOT_EMPTY;
	}
	
	//Update BIOS flags
	SWI_CHECKBITS = REG_IF;
	
}

/*
void EnableIrq(u32 IRQ){
	#ifdef ARM7
	interrupts_to_wait_arm7	|=	IRQ;
	#endif
	
	#ifdef ARM9
	interrupts_to_wait_arm9	|=	IRQ;
	#endif
	
	REG_IE	|=	IRQ;
	
}

void DisableIrq(u32 IRQ){
	#ifdef ARM7
	interrupts_to_wait_arm7	&=	~(IRQ);
	#endif
	
	#ifdef ARM9
	interrupts_to_wait_arm9	&=	~(IRQ);
	#endif
	
	REG_IE	&=	~(IRQ);
}
*/

#ifdef ARM9

//Coto:
__attribute__((section(".itcm")))
void HandleFifo() {
	//dont work as expected
	/*
	volatile u32 command1=0,command2=0,command3=0,command4=0;
    while(!(REG_IPC_FIFO_CR & RECV_FIFO_IPC_EMPTY)){
        if(!(REG_IPC_FIFO_CR & RECV_FIFO_IPC_EMPTY)){
            command1 = REG_IPC_FIFO_RX;
        }
        if(!(REG_IPC_FIFO_CR & RECV_FIFO_IPC_EMPTY)){
            command2 = REG_IPC_FIFO_RX;
        }
        if(!(REG_IPC_FIFO_CR & RECV_FIFO_IPC_EMPTY)){
            command3 = REG_IPC_FIFO_RX;
        }
        if(!(REG_IPC_FIFO_CR & RECV_FIFO_IPC_EMPTY)){
            command4 = REG_IPC_FIFO_RX;
            break; //dont care after this
        }
    }
    
    fifo9_handler(command1,command2,command3,command4);
    
    REG_IPC_FIFO_CR |= ~(1<<14); //flush receive
    REG_IF = IRQ_FIFO_NOT_EMPTY;
    */
	
	//this does
	REG_IF = IRQ_RECVFIFO_NOT_EMPTY;
	while(!(REG_IPC_FIFO_CR & RECV_FIFO_IPC_EMPTY)){
		volatile u32 command1 = REG_IPC_FIFO_RX;	//we already have arg0 otherwise this wouldnt be triggered
		volatile u32 command2 = 0;
		volatile u32 command3 = 0;
		volatile u32 command4 = 0;
		
		if (!(REG_IPC_FIFO_CR & RECV_FIFO_IPC_EMPTY)){
			command2 = REG_IPC_FIFO_RX;
		}
		
		if (!(REG_IPC_FIFO_CR & RECV_FIFO_IPC_EMPTY)){
			command3 = REG_IPC_FIFO_RX;
		}
		
		if (!(REG_IPC_FIFO_CR & RECV_FIFO_IPC_EMPTY)){
			command4 = REG_IPC_FIFO_RX;
		}
		
		//printf("arm9: IRQ not empty received value: %x ! \n",(unsigned int)value0);

		//if there is a cmd to be executed..
		fifo9_handler(command1,command2,command3,command4);
		continue; //required: keep reading commands from the IPC RX Vector
	}
}

__attribute__((section(".itcm")))
void fifo9_handler(u32 command1,u32 command2,u32 command3,u32 command4){
	
	switch (command1) {
        
        //MISC Inter-Processor Opcodes
        
		//vcounter
		case(0xc1710000):{
			
		}
		break;
		
		//fifo test
		case(0xc1710002):{
			printf("fifotest ok \n");
		}
		break;
		
		//FIFO DMA Sound A IRQ from ARM7 Sound
		case(0xc1710003):{
			CPUCheckDMA(3, 2);     //Serve DMA1 FIFO
		}
		break;
		
		//FIFO DMA Sound B IRQ from ARM7 Sound
		case(0xc1710004):{
			CPUCheckDMA(3, 4);     //Serve DMA2 FIFO
		}
		break;
		
		//debug
		case(0xc0700100):{
			//SendArm7Command(0xc0700101,0x0,0x0,0x0);
		}
		break;
		
		//debug print
		case(0xc0700101):{ 
        
			//pu_Disable(); //disable pu while configurating pu
			
			asm("nop");
			asm("nop");
			asm("nop");
			asm("nop");
			asm("nop");
			asm("nop");
			asm("nop");
			asm("nop");
			
			printf("\n \n [Timer %d] dma Source A %x %x \n ",((SpecificIPCAlign->SOUNDCNT_H>>10)&1),SpecificIPCAlign->DM1SAD_H,SpecificIPCAlign->DM1SAD_L);
			printf("\n \n dma Dest A %x %x \n ",SpecificIPCAlign->DM1DAD_H,SpecificIPCAlign->DM1DAD_L);
			printf("dma A : %x %x %x %x \n ",SpecificIPCAlign->fifodmasA[3],SpecificIPCAlign->fifodmasA[2],SpecificIPCAlign->fifodmasA[1],SpecificIPCAlign->fifodmasA[0]);
			printf("DMA1CNT_H: %x \n ",SpecificIPCAlign->DM1CNT_H);
			
			int reg;
			
			for(reg = 0; reg <= 15; reg++) {
				printf("R%d=%X t", (int)reg, (unsigned int)exRegs[reg]);
			}
			
			
			printf("\n \n [Timer %d] dma Source B %x %x \n ",((SpecificIPCAlign->SOUNDCNT_H>>14)&1),SpecificIPCAlign->DM2SAD_H,SpecificIPCAlign->DM2SAD_L);
			printf("\n \n dma Dest B %x %x \n ",SpecificIPCAlign->DM2DAD_H,SpecificIPCAlign->DM2DAD_L);
			printf("dma B : %x %x %x %x \n ",SpecificIPCAlign->fifodmasB[3],SpecificIPCAlign->fifodmasB[2],SpecificIPCAlign->fifodmasB[1],SpecificIPCAlign->fifodmasB[0]);
			printf("DMA2CNT_H: %x \n ",SpecificIPCAlign->DM2CNT_H);
			
			printf("TM0CNT_L %x / TM0CNT_H %x \n",SpecificIPCAlign->TM0CNT_L,SpecificIPCAlign->TM0CNT_H);
			printf("TM1CNT_L %x / TM1CNT_H %x \n",SpecificIPCAlign->TM1CNT_L,SpecificIPCAlign->TM1CNT_H);
			
			asm("nop");
			asm("nop");
			asm("nop");
			asm("nop");
			asm("nop");
			asm("nop");
			asm("nop");
			asm("nop");
			//pu_Enable();
			
		}
		break;
		
		case(0x3F00BEEF):{
			VblankHandler();
		}
		break;
		
		
		case(0x4000BEEF):{
			//while(REG_IPC_FIFO_CR & RECV_FIFO_IPC_EMPTY);
			printf("Debug: arm7 %08X\r\n",command2);
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
		
		case(0xc4740005):{
			UPDATE_REG(command2, (u32)(command3&0xffff));
		}
		break;
		
		//debug
		case(0xc6760000):{
			//printf("DMASA IF IRQ \n");	//works
			//printf("DMASB IF IRQ \n");	//works
			//printf("VBLANK driven sound \n");	//works
			
			printf("TIMER1 IRQ \n");	//works
			
			//volatile char charbuf[64];
			//memcpy((u32*)&charbuf[0],(u32*)&SpecificIPCAlign->charbuf[0],(int)64);
			//printf("%s \n",charbuf);
		}
		break;
		
		
		default:{
		
		/*
		//gbaemu4ds SOUND driver 

#ifdef unsecamr7com
			if(command1 < 0x10000000)		
#else
			if(command1 < 0x8000000 && command1 > 0x2000000)
#endif
			{
	
				register u32 temp_dat = 0;
					 if((command1 & 0x3) == 1)temp_dat = 0x10;
				else if((command1 & 0x3) == 2)temp_dat = 0x50;
				else if((command1 & 0x3) == 3)temp_dat = 0x60;
				memcpy((void*)((u32)&arm7exchangefild[0] + temp_dat),(void*)(command1 & ~0x3),0x10);
			}
			else
			{
				printf("error rec %08X %08X\r\n",command1,REG_IPC_FIFO_CR);
				while(1); //stop to prevent dammage
			}
			
			
			*/
			
			printf("error rec %08X %08X\r\n",command1,REG_IPC_FIFO_CR);
			while(1); //stop to prevent dammage
			
		}
		break;
    }
}

#endif