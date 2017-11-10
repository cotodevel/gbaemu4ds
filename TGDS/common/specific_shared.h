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

//inherits what is defined in: common_shared.h
#ifndef __specific_shared_h__
#define __specific_shared_h__

#include "dsregs.h"
#include "dsregs_asm.h"
#include "common_shared.h"
#include "dswnifi.h"
#include "mem_handler_shared.h"

//gba dma fifo
#define INTERNAL_FIFO_SIZE 	(sint32)(16)	//each DMA
#define FIFO_BUFFER_SIZE	(sint32)(4)		//FIFO_A/FIFO_B = 4 Bytes


typedef struct
{
	uint32 entryPoint;
	uint8 logo[156];
	char title[0xC];
	char gamecode[0x4];
	uint16 makercode;
	uint8 is96h;
	uint8 unitcode;
	uint8 devicecode;
	uint8 unused[7];
	uint8 version;
	uint8 complement;
	uint16 checksum;
} __attribute__ ((__packed__)) gbaHeader_t;


//aligned struct, shared code works just fine from here
struct sAlignedIPCProy	//sAlignedIPC as in common_shared.h but project specific implementation
{
	//GBA Header
    gbaHeader_t gbaheader;
	
	uint16 GBA_IE;	//a.k.a GBAEMU4DS_IPC->IE
    uint16 GBA_IF;	//a.k.a GBAEMU4DS_IPC->IF
    uint16 GBA_IME;	//a.k.a GBAEMU4DS_IPC->IME
	
	//The next hardware mapped here: DMA 1,2 and Timers 0,1 since they belong to GBA sound system
    //These two registers may receive 32bit (4 bytes) of audio data (Data 0-3, Data 0 being located in least significant byte which is replayed first).
    //Internally, the capacity of the FIFO is 8 x 32bit (32 bytes), allowing to buffer a small amount of samples. As the name says (First In First Out), oldest data is replayed first.
    
    //40000A0h - FIFO_A_L - Sound A FIFO, Data 0 and Data 1 (W)
    //uint8 FIFO_A_L[2];
    //40000A2h - FIFO_A_H - Sound A FIFO, Data 2 and Data 3 (W)
    //uint8 FIFO_A_H[2];

    //40000A4h - FIFO_B_L - Sound B FIFO, Data 0 and Data 1 (W)
    //uint8 FIFO_B_L[2];
    //40000A6h - FIFO_B_H - Sound B FIFO, Data 2 and Data 3 (W)
    //uint8 FIFO_B_H[2];
    
    //DMA
    uint32 dma0Source;
    uint32 dma0Dest;
    uint16 DM0SAD_L;
    uint16 DM0SAD_H;
    uint16 DM0DAD_L;
    uint16 DM0DAD_H;
    uint16 DM0CNT_L;
    uint16 DM0CNT_H;
	
	//for ARM7 <-> ARM9 DMA Sync Process
	bool dma1_donearm9;
    bool dma2_donearm9;
    
    uint32 dma1Source;
    uint32 dma1Dest;
    uint16 DM1SAD_L;
    uint16 DM1SAD_H;
    uint16 DM1DAD_L;
    uint16 DM1DAD_H;
    uint16 DM1CNT_L;
    uint16 DM1CNT_H;
    
    uint32 dma2Source;
    uint32 dma2Dest;
    uint16 DM2SAD_L;
    uint16 DM2SAD_H;
    uint16 DM2DAD_L;
    uint16 DM2DAD_H;
    uint16 DM2CNT_L;
    uint16 DM2CNT_H;
    
    uint32 dma3Source;
    uint32 dma3Dest;
    uint16 DM3SAD_L;
    uint16 DM3SAD_H;
    uint16 DM3DAD_L;
    uint16 DM3DAD_H;
    uint16 DM3CNT_L;
    uint16 DM3CNT_H;
    
    //Timers
	//4000102h - TM0CNT_H - Timer 0 Control (R/W)
	//uint16 timer0Value;
	uint16 TM0CNT_H;
	
	//4000100h - TM0CNT_L - Timer 0 Counter/Reload (R/W)
	//int timer0Reload;
	uint16 TM0CNT_L;
	
    bool timer0On;
    int timer0Ticks;
	int timer0ClockReload;
    
	//4000106h - TM1CNT_H - Timer 1 Control (R/W)
	//uint16 timer1Value;
	uint16 TM1CNT_H;
	
	//4000104h - TM1CNT_L - Timer 1 Counter/Reload (R/W)
	//int timer1Reload;
    uint16 TM1CNT_L;
	
    bool timer1On;
    int timer1Ticks;
    int timer1ClockReload;
    
	//400010Ah - TM2CNT_H - Timer 2 Control (R/W)
	//uint16 timer2Value;
    uint16 TM2CNT_H;
	
	//4000108h - TM2CNT_L - Timer 2 Counter/Reload (R/W)
	//int timer2Reload;
    uint16 TM2CNT_L;
	
	bool timer2On;
    int timer2Ticks;
    int timer2ClockReload;
    
	//400010Eh - TM3CNT_H - Timer 3 Control (R/W)
	//uint16 timer3Value;
	uint16 TM3CNT_H;
	
	//400010Ch - TM3CNT_L - Timer 3 Counter/Reload (R/W)
	//int timer3Reload;
    uint16 TM3CNT_L;
	
    bool timer3On;
    int timer3Ticks;
    int timer3ClockReload;   
    
	//new
	uint8 fifodmasA[INTERNAL_FIFO_SIZE];
	uint8 fifodmasB[INTERNAL_FIFO_SIZE];

	//since FIFO is 16x2 bytes (DMA SA/DMASB) and FIFO stream registers are 4 bytes 16/4 = 4 steps, little endian.
	uint8 fifoA_offset;
	uint8 fifoB_offset;
	
    //GBA Sound Controller
    uint16 SOUNDCNT_L;
    uint16 SOUNDCNT_H;
	
	//SoundBias
	uint16 SOUNDBIAS;
	
};

//---------------------------------------------------------------------------------
typedef struct sSpecificIPC {
//---------------------------------------------------------------------------------
	//dswnifi specific
	//TdsnwifisrvStr dswifiSrv;	//the unaligned access here kills code. Must be word aligned, defined on ARM9 only
	//struct s_apu2 APU2;	//the unaligned access here kills the SnemulDS APU sync. Must be word aligned, so we define it below.
	uint32 stubSpecificIPC;
} tSpecificIPC __attribute__ ((aligned (4)));

//project specific IPC. tMyIPC is used by TGDS so don't overlap
#define SpecificIPCUnalign ((volatile tSpecificIPC*)(getUserIPCAddress()))
#define SpecificIPCAlign ((volatile struct sAlignedIPCProy*)(getUserIPCAddress()+(sizeof(tSpecificIPC))))

// Project Specific
#define neu_sound_16fifo

#define setdmasoundbuff 0x1FFFFFFA					//sets DMA NDS Channels to a desired sound buffer source
#define WaitforVblancarmcmd 0x1FFFFFFB				//frameasync on ARM9 (render async image from vmem)
#define enableWaitforVblancarmcmdirq 0x1FFFFFFC		//enable the above frame async render
//unused: #define getarm7keys 0x1FFFFFFD	//read XY Touch from IPC
#define set_callline 0x1FFFFFFF	//set vcounter irq line

//#define anyarmcom
//#define arm7dmapluscheats		//for dma fifo sound + cheats (old implementation)


//#define testGBAEMU4DSFSCode	//enable for generating a file you can later test in any emu, that file is created (you pick from the list) is using the same gbaemu4ds streaming driver.
#endif

#ifdef __cplusplus
extern "C" {
#endif

//NOT weak symbols : the implementation of these is project-defined (here)
extern void HandleFifoNotEmptyWeakRef(uint32 cmd1,uint32 cmd2,uint32 cmd3,uint32 cmd4);
extern void HandleFifoEmptyWeakRef(uint32 cmd1,uint32 cmd2,uint32 cmd3,uint32 cmd4);


#ifdef __cplusplus
}
#endif