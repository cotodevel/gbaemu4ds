// -*- C++ -*-
// VisualBoyAdvance - Nintendo Gameboy/GameboyAdvance (TM) emulator.
// Copyright (C) 1999-2003 Forgotten
// Copyright (C) 2005 Forgotten and the VBA development team

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2, or(at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#ifndef VBA_GBA_H
#define VBA_GBA_H

#include <nds/arm9/background.h>
#include <nds/arm9/trig_lut.h>

#include "System.h"
#include "ichflysettings.h"

#define SAVE_GAME_VERSION_1 1
#define SAVE_GAME_VERSION_2 2
#define SAVE_GAME_VERSION_3 3
#define SAVE_GAME_VERSION_4 4
#define SAVE_GAME_VERSION_5 5
#define SAVE_GAME_VERSION_6 6
#define SAVE_GAME_VERSION_7 7
#define SAVE_GAME_VERSION_8 8
#define SAVE_GAME_VERSION_9 9
#define SAVE_GAME_VERSION  SAVE_GAME_VERSION_9

typedef struct {
  u8 *address;
  u32 mask;
} memoryMap;

typedef union {
  struct {
#ifdef WORDS_BIGENDIAN
    u8 B3;
    u8 B2;
    u8 B1;
    u8 B0;
#else
    u8 B0;
    u8 B1;
    u8 B2;
    u8 B3;
#endif
  } B;
  struct {
#ifdef WORDS_BIGENDIAN
    u16 W1;
    u16 W0;
#else
    u16 W0;
    u16 W1;
#endif
  } W;
#ifdef WORDS_BIGENDIAN
  volatile u32 I;
#else
	u32 I;
#endif
} reg_pair;

#define R13_IRQ  18
#define R14_IRQ  19
#define SPSR_IRQ 20
#define R13_USR  26
#define R14_USR  27
#define R13_SVC  28
#define R14_SVC  29
#define SPSR_SVC 30
#define R13_ABT  31
#define R14_ABT  32
#define SPSR_ABT 33
#define R13_UND  34
#define R14_UND  35
#define SPSR_UND 36
#define R8_FIQ   37
#define R9_FIQ   38
#define R10_FIQ  39
#define R11_FIQ  40
#define R12_FIQ  41
#define R13_FIQ  42
#define R14_FIQ  43
#define SPSR_FIQ 44

#include "Cheats.h"
#include "EEprom.h"
#include "Flash.h"

#endif //VBA_GBA_H





#ifndef VBA_GLOBALS_H
#define VBA_GLOBALS_H

#include "GBA.h"
#include "ichflysettings.h"

#define VERBOSE_SWI                  1
#define VERBOSE_UNALIGNED_MEMORY     2
#define VERBOSE_ILLEGAL_WRITE        4
#define VERBOSE_ILLEGAL_READ         8
#define VERBOSE_DMA0                16
#define VERBOSE_DMA1                32
#define VERBOSE_DMA2                64
#define VERBOSE_DMA3               128
#define VERBOSE_UNDEFINED          256
#define VERBOSE_AGBPRINT           512

/*
#define internalRAM ((u8*)0x03000000)
#define workRAM ((u8*)0x02000000)
#define paletteRAM ((u8*)0x05000000)
#define vram ((u8*)0x06000000)
#define emultoroam ((u8*)0x07000000)
*/

#define DISPCAPCNT (*(vu32*)0x4000064)

#endif // VBA_GLOBALS_H











#ifdef __cplusplus
extern "C" {
#endif


#ifndef NO_GBA_MAP
extern memoryMap map[256];
#endif

extern reg_pair reg[45];
extern u8 biosProtected[4];

extern bool N_FLAG;
extern bool Z_FLAG;
extern bool C_FLAG;
extern bool V_FLAG;
extern bool armIrqEnable;
extern bool armState;
extern int armMode;

extern u32 stop;
extern int saveType;
extern bool useBios;
extern bool skipBios;
extern int frameSkip;
extern bool speedup;
extern bool synchronize;
extern bool cpuDisableSfx;
extern bool cpuIsMultiBoot;
extern bool parseDebug;
extern int layerSettings;
extern int layerEnable;
extern bool speedHack;
extern int cpuSaveType;
extern bool cheatsEnabled;
extern bool mirroringEnable;


//#ifdef BKPT_SUPPORT //ichfly
extern u8 freezeWorkRAM[0x40000];
extern u8 freezeInternalRAM[0x8000];
extern u8 freezeVRAM[0x18000];
extern u8 freezeOAM[0x400];
extern u8 freezePRAM[0x400];
extern bool debugger_last;
extern int  oldreg[17];
extern char oldbuffer[10];
//#endif

extern bool CPUReadGSASnapshot(const char *);
extern bool CPUWriteGSASnapshot(const char *, const char *, const char *, const char *);
extern bool CPUWriteBatteryFile(const char *);
extern bool CPUReadBatteryFile(const char *);
extern bool CPUExportEepromFile(const char *);
extern bool CPUImportEepromFile(const char *);
extern bool CPUWritePNGFile(const char *);
extern bool CPUWriteBMPFile(const char *);
extern void CPUCleanUp();
extern void CPUUpdateRender();
extern bool CPUReadMemState(char *, int);
extern bool CPUReadState(const char *);
extern bool CPUWriteMemState(char *, int);
extern bool CPUWriteState(const char *);
extern int CPULoadRom(const char *,bool);
extern void doMirroring(bool);
extern void applyTimer ();
//extern void CPUWriteHalfWord(u32, u16);
//extern void CPUWriteByte(u32, u8);
extern void CPUInit(const char *,bool,bool);
extern void CPUReset();
extern void CPULoop(int);
extern void CPUCheckDMA(int,int);
extern bool CPUIsGBAImage(const char *);
extern bool CPUIsZipFile(const char *);

#ifdef PROFILING
	#include "prof/prof.h"
	extern void cpuProfil(profile_segment *seg);
	extern void cpuEnableProfiling(int hz);
#endif

extern int romSize;

extern reg_pair reg[45];
extern bool ioReadable[0x400];
extern bool N_FLAG;
extern bool C_FLAG;
extern bool Z_FLAG;
extern bool V_FLAG;
extern bool armState;
extern bool armIrqEnable;
extern u32 armNextPC;
extern int armMode;
extern u32 stop;
extern int saveType;
extern bool useBios;
extern bool skipBios;
extern int frameSkip;
extern bool speedup;
extern bool synchronize;
extern bool cpuDisableSfx;
extern bool cpuIsMultiBoot;
extern bool parseDebug;
extern int layerSettings;
extern int layerEnable;
extern bool speedHack;
extern int cpuSaveType;
extern bool cheatsEnabled;
extern bool mirroringEnable;

extern u8 *bios;
extern u8 *rom;

//extern u8 ioMem[0x400];   //IPC

extern u8 currentVRAMcapblock;

extern u16 DISPCNT;
extern u16 DISPSTAT;
extern u16 VCOUNT;
extern u16 BG0CNT;
extern u16 BG1CNT;
extern u16 BG2CNT;
extern u16 BG3CNT;
extern u16 BG0HOFS;
extern u16 BG0VOFS;
extern u16 BG1HOFS;
extern u16 BG1VOFS;
extern u16 BG2HOFS;
extern u16 BG2VOFS;
extern u16 BG3HOFS;
extern u16 BG3VOFS;
extern u16 BG2PA;
extern u16 BG2PB;
extern u16 BG2PC;
extern u16 BG2PD;
extern u16 BG2X_L;
extern u16 BG2X_H;
extern u16 BG2Y_L;
extern u16 BG2Y_H;
extern u16 BG3PA;
extern u16 BG3PB;
extern u16 BG3PC;
extern u16 BG3PD;
extern u16 BG3X_L;
extern u16 BG3X_H;
extern u16 BG3Y_L;
extern u16 BG3Y_H;
extern u16 WIN0H;
extern u16 WIN1H;
extern u16 WIN0V;
extern u16 WIN1V;
extern u16 WININ;
extern u16 WINOUT;
extern u16 MOSAIC;
extern u16 BLDMOD;
extern u16 COLEV;
extern u16 COLY;
extern u16 DM0SAD_L;
extern u16 DM0SAD_H;
extern u16 DM0DAD_L;
extern u16 DM0DAD_H;
extern u16 DM0CNT_L;
extern u16 DM0CNT_H;
extern u16 DM1SAD_L;
extern u16 DM1SAD_H;
extern u16 DM1DAD_L;
extern u16 DM1DAD_H;
extern u16 DM1CNT_L;
extern u16 DM1CNT_H;
extern u16 DM2SAD_L;
extern u16 DM2SAD_H;
extern u16 DM2DAD_L;
extern u16 DM2DAD_H;
extern u16 DM2CNT_L;
extern u16 DM2CNT_H;
extern u16 DM3SAD_L;
extern u16 DM3SAD_H;
extern u16 DM3DAD_L;
extern u16 DM3DAD_H;
extern u16 DM3CNT_L;
extern u16 DM3CNT_H;
extern u16 TM0D;
extern u16 TM0CNT;
extern u16 TM1D;
extern u16 TM1CNT;
extern u16 TM2D;
extern u16 TM2CNT;
extern u16 TM3D;
extern u16 TM3CNT;
extern u16 P1;
extern u16 IE;
extern u16 IF;
extern u16 IME;

extern u32 myROM[];
extern bool disableMessage;

//ichfly
extern u16 timer0Value;
extern u16 timer1Value;
extern u16 timer2Value;
extern u16 timer3Value;



//GBA cpu memory opcodes
extern u32 CPUReadMemory_stack(u32 address);
extern u16 CPUReadHalfWord_stack(u32 address);
extern u8 CPUReadByte_stack(u32 address);
extern s8 CPUReadByteSigned_stack(u32 address);

extern s16 CPUReadHalfWordSigned_stack(u32 address);
extern s8 CPUReadByteSigned_stack(u32 address);

extern void CPUWriteMemory_stack(u32 address, u32 value);
extern void CPUWriteHalfWord_stack(u32 address, u16 value);
extern void CPUWriteByte_stack(u32 address, u8 b);


//intern
extern u32 CPUReadMemory(u32 address);
extern u16 CPUReadHalfWord(u32 address);
extern u8 CPUReadByte(u32 address);
extern s8 CPUReadByteSigned(u32 address);

extern s16 CPUReadHalfWordSigned(u32 address);
extern s8 CPUReadByteSigned(u32 address);

extern void CPUWriteMemory(u32 address, u32 value);
extern void CPUWriteHalfWord(u32 address, u16 value);
extern void CPUWriteByte(u32 address, u8 b);


//cpumg.cpp
extern u16 gbaIME;
extern u16 gbaDISPCNT;
extern u16 gbaBGxCNT[4];

extern char disbuffer[0x2000];

#ifdef lastdebug
	extern u32 lasttime[6];
	extern int lastdebugcurrent;
	extern int lastdebugsize;
#endif

extern int durchlauf;
extern void debugDump();
extern int durchgang;
extern u32 getExceptionAddress( u32 opcodeAddress, u32 thumbState);
extern unsigned long ARMShift(unsigned long value,unsigned char shift);

extern void unknowndebugprint(reg_pair *myregs);


//normal cpu rw opcodes
void updateVC();
void updateVCsub();

//gba.core.cpp end

//recompiler.cpp
extern	void emuInstrARM(u32 opcode, u32 *R);
extern	const volatile void emuInstrTHUMB(u16 opcode, u32 *R);

extern int offset;
extern int base;
extern int dest;
extern u32 address;


extern u32 STMW_myregs(u32 opcode, u32 temp, u32 address,u32 val,u32 num);
extern u32 STM_myregs(u32 opcode, u32 address,u32 val,u32 num);
extern u32 LDM_myregs(u32 opcode, u32 address,u32 val,u32 num);
extern u32 THUMB_STM_myregs(u32 opcode,u32 temp,u32 address,u32 val,u32 r,u32 b);
extern u32 THUMB_LDM_myregs(u32 opcode,u32 temp,u32 address,u32 val,u32 r);
extern u32 PUSH_myregs(u32 opcode, u32 address,u32 val, u32 r);
extern u32 POP_myregs(u32 opcode, u32 address,u32 val, u32 r);

extern void vcounthandler(void);

//sound.cpp

//irqstuf.arm.cpp
extern void resettostartup();
//extern void frameasyncsync(void); //removed
extern void pausemenue();

//devkitpatcher.c
extern void ichflyfifoinit();

//system.cpp
extern int len;

//gba.armcore.cpp
extern u8 cpuBitsSet[256];
extern u8 cpuLowestBitSet[256];

extern int SWITicks;
extern int IRQTicks;

extern int layerEnableDelay;
extern bool busPrefetch;
extern bool busPrefetchEnable;
extern u32 busPrefetchCount;
extern int cpuDmaTicksToUpdate;
extern int cpuDmaCount;
extern bool cpuDmaHack;
extern u32 cpuDmaLast;
extern int dummyAddress;

extern bool cpuBreakLoop;
extern int cpuNextEvent;

extern int gbaSaveType; // used to remember the save type on reset
extern bool intState;
extern bool stopState;
extern bool holdState;
extern int holdType;
extern bool cpuSramEnabled;
extern bool cpuFlashEnabled;
extern bool cpuEEPROMEnabled;
extern bool cpuEEPROMSensorEnabled;

extern u32 cpuPrefetch[2];

extern int cpuTotalTicks;
#ifdef PROFILING
	extern int profilingTicks;
	extern int profilingTicksReload;
	//static profile_segment *profilSegment = NULL;
#endif

/*#ifdef BKPT_SUPPORT //ichfly test
	extern u8 freezeWorkRAM[0x40000];
	extern u8 freezeInternalRAM[0x8000];
	extern u8 freezeVRAM[0x18000];
	extern u8 freezePRAM[0x400];
	extern u8 freezeOAM[0x400];
	extern bool debugger_last;
#endif*/

extern int lcdTicks;
extern u8 timerOnOffDelay;
extern u16 timer0Value;
extern bool timer0On;
extern int timer0Ticks;
extern int timer0Reload;
extern int timer0ClockReload;
extern u16 timer1Value;
extern bool timer1On;
extern int timer1Ticks;
extern int timer1Reload;
extern int timer1ClockReload;
extern u16 timer2Value;
extern bool timer2On;
extern int timer2Ticks;
extern int timer2Reload;
extern int timer2ClockReload;
extern u16 timer3Value;
extern bool timer3On;
extern int timer3Ticks;
extern int timer3Reload;
extern int timer3ClockReload;
extern u32 dma0Source;
extern u32 dma0Dest;
extern u32 dma1Source;
extern u32 dma1Dest;
extern u32 dma2Source;
extern u32 dma2Dest;
extern u32 dma3Source;
extern u32 dma3Dest;

extern void (*cpuSaveGameFunc)(u32,u8);
//void (*renderLine)() = mode0RenderLine;
extern bool fxOn;
extern bool windowOn;
extern int frameCount;
extern char buffer[1024];

//extern FILE *out; //don't know if this is a good idea to extern
//u32 lastTime = 0;
extern int count;

extern int capture;
extern int capturePrevious;
extern int captureNumber;

extern int TIMER_TICKS[4];

extern u32  objTilesAddress [3];
extern u8 gamepakRamWaitState[4];
extern u8 gamepakWaitState[4];
extern u8 gamepakWaitState0[2];
extern u8 gamepakWaitState1[2];
extern u8 gamepakWaitState2[2];
extern bool isInRom [16];
extern u8 memoryWait[16];

// The videoMemoryWait constants are used to add some waitstates
// if the opcode access video memory data outside of vblank/hblank
// It seems to happen on only one ticks for each pixel.
// Not used for now (too problematic with current code).
//const u8 videoMemoryWait[16] =
//  {0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0};

#ifdef WORDS_BIGENDIAN
	extern bool cpuBiosSwapped;
#endif

/*
//ori:
static u8 *internalRAM = (u8*)0x03000000; 
static u8 *workRAM = (u8*)0x02000000;
static u8 *paletteRAM = (u8*)0x05000000;
static u8 *vram = (u8*)0x06000000;
static u8 *oam = (u8*)0x07000000; //emultoroam
*/

extern u8 *bios ; //calloc
extern u8 *rom ; //calc
extern u8 * internalRAM;
extern u8 * workRAM;
extern u8 * paletteRAM;
extern u8 * vram;
extern u8 * oam; //emultoroam


//linker script hardware address setup
extern int __ewram_start;
extern int __dtcm_start;

extern int __gbarom_start;
extern int __gbarom_end;

extern int __gbaew_start;
extern int __gbaew_end;

extern int __itcm_start;

extern int gba_frame_line;  //gba line that generates a frame
extern int nds_frame;       //nds frame counter

extern void wifi_gba_thread();

extern void CPUUpdateRegister(u32 address, u16 value);

//hardware background setup
#ifdef own_bg_render
    extern u32 bg_0_setting; //that is assigned to NDS ppu

    extern u32 bg_1_setting; //that is assigned to NDS ppu

    extern u32 bg_2_setting; //that is assigned to NDS ppu

    extern u32 bg_3_setting; //that is assigned to NDS ppu
#endif

extern int bgInit_call_gbaemu4ds(int layer, BgType type, BgSize size, int mapBase, int tileBase);
extern void bgUpdate_gbaemu4ds(void);

extern bool ioReadable[0x400];     //GBA IO MAP Permissions (offset[n] true == GBA IO MAP Write / false == cant write GBA IO MAP)
extern u8 ioMem[0x400];            //GBA IO MAP

extern void doDMAFIFO(u32 s, u32 d, u32 si, u32 di, u32 c, int transfer32);

#ifdef __cplusplus
}
#endif
