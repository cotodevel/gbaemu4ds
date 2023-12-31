
#define loadindirect

// VisualBoyAdvance - Nintendo Gameboy/GameboyAdvance (TM) emulator.
// Copyright (C) 1999-2003 Forgotten
// Copyright (C) 2005-2006 Forgotten and the VBA development team

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

#include "GBA.h"
#include "Sound.h"
#include "Util.h"
#include "getopt.h"
#include "System.h"
#include "main.h"

#define ichflytestkeypossibillity

bool ichflytest = false;
bool disableMessage = false;


#include <stdio.h>
#include <stdlib.h>
#include <nds/memory.h>//#include <memory.h> ichfly
#include <nds/ndstypes.h>
#include <nds/memory.h>
#include <nds/bios.h>
#include <nds/system.h>
#include <nds/arm9/math.h>
#include <nds/arm9/video.h>
#include <nds/arm9/videoGL.h>
#include <nds/arm9/trig_lut.h>
#include <nds/arm9/sassert.h>
#include <stdarg.h>
#include <string.h>

#include "GBA.h"
#include "EEprom.h"
#include "Flash.h"
#include "Sound.h"
#include "Sram.h"
#include "bios.h"
#include "Cheats.h"
#include "NLS.h"
#include "elf.h"
#include "Util.h"
#include "RTC.h"
#include "agbprint.h"
#include "fatmore.h"
#include "ds_dma.h"

#ifdef __GNUC__
#define _stricmp strcasecmp
#endif


#include "GBA.h"
#include "ichflysettings.h"
#include "gba_ipc.h"

reg_pair reg[45];
memoryMap map[256];
bool ioReadable[0x400];

__attribute__((section(".dtcm")))
bool N_FLAG = 0;

__attribute__((section(".dtcm")))
bool C_FLAG = 0;

__attribute__((section(".dtcm")))
bool Z_FLAG = 0;

__attribute__((section(".dtcm")))
bool V_FLAG = 0;

__attribute__((section(".dtcm")))
bool armState = true;

__attribute__((section(".dtcm")))
bool armIrqEnable = true;

__attribute__((section(".dtcm")))
u32 armNextPC = 0x00000000;

__attribute__((section(".dtcm")))
int armMode = 0x1f;

__attribute__((section(".dtcm")))
u32 stop = 0x08000568;

__attribute__((section(".dtcm")))
int saveType = 0;

__attribute__((section(".dtcm")))
bool useBios = false;

__attribute__((section(".dtcm")))
bool skipBios = false;

__attribute__((section(".dtcm")))
int frameSkip = 1;

__attribute__((section(".dtcm")))
bool speedup = false;

__attribute__((section(".dtcm")))
bool synchronize = true;

__attribute__((section(".dtcm")))
bool cpuDisableSfx = false;

__attribute__((section(".dtcm")))
bool cpuIsMultiBoot = false;

__attribute__((section(".dtcm")))
bool parseDebug = true;

__attribute__((section(".dtcm")))
int layerSettings = 0xff00;

__attribute__((section(".dtcm")))
int layerEnable = 0xff00;

__attribute__((section(".dtcm")))
bool speedHack = false;

__attribute__((section(".dtcm")))
int cpuSaveType = 0;

__attribute__((section(".dtcm")))
bool cheatsEnabled = true;

__attribute__((section(".dtcm")))
bool mirroringEnable = false;

u8 *bios = NULL; //calloc
u8 *rom = NULL; //calc
/*static u8 *internalRAM = (u8*)0x03000000; 
static u8 *workRAM = (u8*)0x02000000;
static u8 *paletteRAM = (u8*)0x05000000;
static u8 *vram = (u8*)0x06000000;
static u8 *oam = (u8*)0x07000000;*/
u8 ioMem[0x400];

__attribute__((section(".dtcm")))
u16 DISPSTAT = 0x0000;

__attribute__((section(".dtcm")))
u16 VCOUNT   = 0x0000;

__attribute__((section(".dtcm")))
u16 BG0CNT   = 0x0000;

__attribute__((section(".dtcm")))
u16 BG1CNT   = 0x0000;

__attribute__((section(".dtcm")))
u16 BG2CNT   = 0x0000;

__attribute__((section(".dtcm")))
u16 BG3CNT   = 0x0000;

__attribute__((section(".dtcm")))
u16 BG0HOFS  = 0x0000;

__attribute__((section(".dtcm")))
u16 BG0VOFS  = 0x0000;

__attribute__((section(".dtcm")))
u16 BG1HOFS  = 0x0000;

__attribute__((section(".dtcm")))
u16 BG1VOFS  = 0x0000;

__attribute__((section(".dtcm")))
u16 BG2HOFS  = 0x0000;

__attribute__((section(".dtcm")))
u16 BG2VOFS  = 0x0000;

__attribute__((section(".dtcm")))
u16 BG3HOFS  = 0x0000;

__attribute__((section(".dtcm")))
u16 BG3VOFS  = 0x0000;

__attribute__((section(".dtcm")))
u16 BG2PA    = 0x0100;

__attribute__((section(".dtcm")))
u16 BG2PB    = 0x0000;

__attribute__((section(".dtcm")))
u16 BG2PC    = 0x0000;

__attribute__((section(".dtcm")))
u16 BG2PD    = 0x0100;

__attribute__((section(".dtcm")))
u16 BG2X_L   = 0x0000;

__attribute__((section(".dtcm")))
u16 BG2X_H   = 0x0000;

__attribute__((section(".dtcm")))
u16 BG2Y_L   = 0x0000;

__attribute__((section(".dtcm")))
u16 BG2Y_H   = 0x0000;

__attribute__((section(".dtcm")))
u16 BG3PA    = 0x0100;

__attribute__((section(".dtcm")))
u16 BG3PB    = 0x0000;

__attribute__((section(".dtcm")))
u16 BG3PC    = 0x0000;

__attribute__((section(".dtcm")))
u16 BG3PD    = 0x0100;

__attribute__((section(".dtcm")))
u16 BG3X_L   = 0x0000;

__attribute__((section(".dtcm")))
u16 BG3X_H   = 0x0000;

__attribute__((section(".dtcm")))
u16 BG3Y_L   = 0x0000;

__attribute__((section(".dtcm")))
u16 BG3Y_H   = 0x0000;

__attribute__((section(".dtcm")))
u16 WIN0H    = 0x0000;

__attribute__((section(".dtcm")))
u16 WIN1H    = 0x0000;

__attribute__((section(".dtcm")))
u16 WIN0V    = 0x0000;

__attribute__((section(".dtcm")))
u16 WIN1V    = 0x0000;

__attribute__((section(".dtcm")))
u16 WININ    = 0x0000;

__attribute__((section(".dtcm")))
u16 WINOUT   = 0x0000;

__attribute__((section(".dtcm")))
u16 MOSAIC   = 0x0000;

__attribute__((section(".dtcm")))
u16 BLDMOD   = 0x0000;

__attribute__((section(".dtcm")))
u16 COLEV    = 0x0000;

__attribute__((section(".dtcm")))
u16 COLY     = 0x0000;

__attribute__((section(".dtcm")))
u16 DM0SAD_L = 0x0000;

__attribute__((section(".dtcm")))
u16 DM0SAD_H = 0x0000;

__attribute__((section(".dtcm")))
u16 DM0DAD_L = 0x0000;

__attribute__((section(".dtcm")))
u16 DM0DAD_H = 0x0000;

__attribute__((section(".dtcm")))
u16 DM0CNT_L = 0x0000;

__attribute__((section(".dtcm")))
u16 DM0CNT_H = 0x0000;

__attribute__((section(".dtcm")))
u16 DM1SAD_L = 0x0000;

__attribute__((section(".dtcm")))
u16 DM1SAD_H = 0x0000;

__attribute__((section(".dtcm")))
u16 DM1DAD_L = 0x0000;

__attribute__((section(".dtcm")))
u16 DM1DAD_H = 0x0000;

__attribute__((section(".dtcm")))
u16 DM1CNT_L = 0x0000;

__attribute__((section(".dtcm")))
u16 DM1CNT_H = 0x0000;

__attribute__((section(".dtcm")))
u16 DM2SAD_L = 0x0000;

__attribute__((section(".dtcm")))
u16 DM2SAD_H = 0x0000;

__attribute__((section(".dtcm")))
u16 DM2DAD_L = 0x0000;

__attribute__((section(".dtcm")))
u16 DM2DAD_H = 0x0000;

__attribute__((section(".dtcm")))
u16 DM2CNT_L = 0x0000;

__attribute__((section(".dtcm")))
u16 DM2CNT_H = 0x0000;

__attribute__((section(".dtcm")))
u16 DM3SAD_L = 0x0000;

__attribute__((section(".dtcm")))
u16 DM3SAD_H = 0x0000;

__attribute__((section(".dtcm")))
u16 DM3DAD_L = 0x0000;

__attribute__((section(".dtcm")))
u16 DM3DAD_H = 0x0000;

__attribute__((section(".dtcm")))
u16 DM3CNT_L = 0x0000;

__attribute__((section(".dtcm")))
u16 DM3CNT_H = 0x0000;

__attribute__((section(".dtcm")))
u16 TM0D     = 0x0000;

__attribute__((section(".dtcm")))
u16 TM0CNT   = 0x0000;

__attribute__((section(".dtcm")))
u16 TM1D     = 0x0000;

__attribute__((section(".dtcm")))
u16 TM1CNT   = 0x0000;

__attribute__((section(".dtcm")))
u16 TM2D     = 0x0000;

__attribute__((section(".dtcm")))
u16 TM2CNT   = 0x0000;

__attribute__((section(".dtcm")))
u16 TM3D     = 0x0000;

__attribute__((section(".dtcm")))
u16 TM3CNT   = 0x0000;

__attribute__((section(".dtcm")))
u16 P1       = 0xFFFF;

__attribute__((section(".dtcm")))
u16 IE       = 0x0000;

__attribute__((section(".dtcm")))
u16 IF       = 0x0000;

__attribute__((section(".dtcm")))
u16 IME      = 0x0000;


//gba arm core variables
int SWITicks = 0;
int IRQTicks = 0;

int layerEnableDelay = 0;
bool busPrefetch = false;
bool busPrefetchEnable = false;
u32 busPrefetchCount = 0;
int cpuDmaTicksToUpdate = 0;
int cpuDmaCount = 0;
bool cpuDmaHack = false;
u32 cpuDmaLast = 0;
int dummyAddress = 0;

bool cpuBreakLoop = false;
int cpuNextEvent = 0;

bool intState = false;
bool stopState = false;
bool holdState = false;
int holdType = 0;
bool cpuSramEnabled = true;
bool cpuFlashEnabled = true;
bool cpuEEPROMEnabled = true;
bool cpuEEPROMSensorEnabled = false;

u32 cpuPrefetch[2];

int cpuTotalTicks = 0;
#ifdef PROFILING
int profilingTicks = 0;
int profilingTicksReload = 0;
static profile_segment *profilSegment = NULL;
#endif

/*#ifdef BKPT_SUPPORT //ichfly test
u8 freezeWorkRAM[0x40000];
u8 freezeInternalRAM[0x8000];
u8 freezeVRAM[0x18000];
u8 freezePRAM[0x400];
u8 freezeOAM[0x400];
bool debugger_last;
#endif*/

int lcdTicks = (useBios && !skipBios) ? 1008 : 208;
u8 timerOnOffDelay = 0;
u16 timer0Value = 0;
bool timer0On = false;
int timer0Ticks = 0;
int timer0Reload = 0;
int timer0ClockReload  = 0;
u16 timer1Value = 0;
bool timer1On = false;
int timer1Ticks = 0;
int timer1Reload = 0;
int timer1ClockReload  = 0;
u16 timer2Value = 0;
bool timer2On = false;
int timer2Ticks = 0;
int timer2Reload = 0;
int timer2ClockReload  = 0;
u16 timer3Value = 0;
bool timer3On = false;
int timer3Ticks = 0;
int timer3Reload = 0;
int timer3ClockReload  = 0;
u32 dma0Source = 0;
u32 dma0Dest = 0;
u32 dma1Source = 0;
u32 dma1Dest = 0;
u32 dma2Source = 0;
u32 dma2Dest = 0;
u32 dma3Source = 0;
u32 dma3Dest = 0;
void (*cpuSaveGameFunc)(u32,u8) = flashSaveDecide;
//void (*renderLine)() = mode0RenderLine;
bool fxOn = false;
bool windowOn = false;
int frameCount = 0;
char buffer[1024];
FILE *out = NULL;
//u32 lastTime = 0;
int count = 0;

int capture = 0;
int capturePrevious = 0;
int captureNumber = 0;

const int TIMER_TICKS[4] = {
  0,
  6,
  8,
  10
};

const u32  objTilesAddress [3] = {0x010000, 0x014000, 0x014000};
const u8 gamepakRamWaitState[4] = { 4, 3, 2, 8 };
const u8 gamepakWaitState[4] =  { 4, 3, 2, 8 };
const u8 gamepakWaitState0[2] = { 2, 1 };
const u8 gamepakWaitState1[2] = { 4, 1 };
const u8 gamepakWaitState2[2] = { 8, 1 };
const bool isInRom [16]=
  { false, false, false, false, false, false, false, false,
    true, true, true, true, true, true, false, false };              

u8 memoryWait[16] =
  { 0, 0, 2, 0, 0, 0, 0, 0, 4, 4, 4, 4, 4, 4, 4, 0 };
u8 memoryWait32[16] =
  { 0, 0, 5, 0, 0, 1, 1, 0, 7, 7, 9, 9, 13, 13, 4, 0 };
u8 memoryWaitSeq[16] =
  { 0, 0, 2, 0, 0, 0, 0, 0, 2, 2, 4, 4, 8, 8, 4, 0 };
u8 memoryWaitSeq32[16] =
  { 0, 0, 5, 0, 0, 1, 1, 0, 5, 5, 9, 9, 17, 17, 4, 0 };

// The videoMemoryWait constants are used to add some waitstates
// if the opcode access video memory data outside of vblank/hblank
// It seems to happen on only one ticks for each pixel.
// Not used for now (too problematic with current code).
//const u8 videoMemoryWait[16] =
//  {0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0};


u8 biosProtected[4];

u32 myROM[] = {
0xEA000006,
0xEA000093,
0xEA000006,
0x00000000,
0x00000000,
0x00000000,
0xEA000088,
0x00000000,
0xE3A00302,
0xE1A0F000,
0xE92D5800,
0xE55EC002,
0xE28FB03C,
0xE79BC10C,
0xE14FB000,
0xE92D0800,
0xE20BB080,
0xE38BB01F,
0xE129F00B,
0xE92D4004,
0xE1A0E00F,
0xE12FFF1C,
0xE8BD4004,
0xE3A0C0D3,
0xE129F00C,
0xE8BD0800,
0xE169F00B,
0xE8BD5800,
0xE1B0F00E,
0x0000009C,
0x0000009C,
0x0000009C,
0x0000009C,
0x000001F8,
0x000001F0,
0x000000AC,
0x000000A0,
0x000000FC,
0x00000168,
0xE12FFF1E,
0xE1A03000,
0xE1A00001,
0xE1A01003,
0xE2113102,
0x42611000,
0xE033C040,
0x22600000,
0xE1B02001,
0xE15200A0,
0x91A02082,
0x3AFFFFFC,
0xE1500002,
0xE0A33003,
0x20400002,
0xE1320001,
0x11A020A2,
0x1AFFFFF9,
0xE1A01000,
0xE1A00003,
0xE1B0C08C,
0x22600000,
0x42611000,
0xE12FFF1E,
0xE92D0010,
0xE1A0C000,
0xE3A01001,
0xE1500001,
0x81A000A0,
0x81A01081,
0x8AFFFFFB,
0xE1A0000C,
0xE1A04001,
0xE3A03000,
0xE1A02001,
0xE15200A0,
0x91A02082,
0x3AFFFFFC,
0xE1500002,
0xE0A33003,
0x20400002,
0xE1320001,
0x11A020A2,
0x1AFFFFF9,
0xE0811003,
0xE1B010A1,
0xE1510004,
0x3AFFFFEE,
0xE1A00004,
0xE8BD0010,
0xE12FFF1E,
0xE0010090,
0xE1A01741,
0xE2611000,
0xE3A030A9,
0xE0030391,
0xE1A03743,
0xE2833E39,
0xE0030391,
0xE1A03743,
0xE2833C09,
0xE283301C,
0xE0030391,
0xE1A03743,
0xE2833C0F,
0xE28330B6,
0xE0030391,
0xE1A03743,
0xE2833C16,
0xE28330AA,
0xE0030391,
0xE1A03743,
0xE2833A02,
0xE2833081,
0xE0030391,
0xE1A03743,
0xE2833C36,
0xE2833051,
0xE0030391,
0xE1A03743,
0xE2833CA2,
0xE28330F9,
0xE0000093,
0xE1A00840,
0xE12FFF1E,
0xE3A00001,
0xE3A01001,
0xE92D4010,
0xE3A03000,
0xE3A04001,
0xE3500000,
0x1B000004,
0xE5CC3301,
0xEB000002,
0x0AFFFFFC,
0xE8BD4010,
0xE12FFF1E,
0xE3A0C301,
0xE5CC3208,
0xE15C20B8,
0xE0110002,
0x10222000,
0x114C20B8,
0xE5CC4208,
0xE12FFF1E,
0xE92D500F,
0xE3A00301,
0xE1A0E00F,
0xE510F004,
0xE8BD500F,
0xE25EF004,
0xE59FD044,
0xE92D5000,
0xE14FC000,
0xE10FE000,
0xE92D5000,
0xE3A0C302,
0xE5DCE09C,
0xE35E00A5,
0x1A000004,
0x05DCE0B4,
0x021EE080,
0xE28FE004,
0x159FF018,
0x059FF018,
0xE59FD018,
0xE8BD5000,
0xE169F00C,
0xE8BD5000,
0xE25EF004,
0x03007FF0,
0x09FE2000,
0x09FFC000,
0x03007FE0
};
/*
variable_desc saveGameStruct[] = {
  { &DISPCNT  , sizeof(u16) },
  { &DISPSTAT , sizeof(u16) },
  { &VCOUNT   , sizeof(u16) },
  { &BG0CNT   , sizeof(u16) },
  { &BG1CNT   , sizeof(u16) },
  { &BG2CNT   , sizeof(u16) },
  { &BG3CNT   , sizeof(u16) },
  { &BG0HOFS  , sizeof(u16) },
  { &BG0VOFS  , sizeof(u16) },
  { &BG1HOFS  , sizeof(u16) },
  { &BG1VOFS  , sizeof(u16) },
  { &BG2HOFS  , sizeof(u16) },
  { &BG2VOFS  , sizeof(u16) },
  { &BG3HOFS  , sizeof(u16) },
  { &BG3VOFS  , sizeof(u16) },
  { &BG2PA    , sizeof(u16) },
  { &BG2PB    , sizeof(u16) },
  { &BG2PC    , sizeof(u16) },
  { &BG2PD    , sizeof(u16) },
  { &BG2X_L   , sizeof(u16) },
  { &BG2X_H   , sizeof(u16) },
  { &BG2Y_L   , sizeof(u16) },
  { &BG2Y_H   , sizeof(u16) },
  { &BG3PA    , sizeof(u16) },
  { &BG3PB    , sizeof(u16) },
  { &BG3PC    , sizeof(u16) },
  { &BG3PD    , sizeof(u16) },
  { &BG3X_L   , sizeof(u16) },
  { &BG3X_H   , sizeof(u16) },
  { &BG3Y_L   , sizeof(u16) },
  { &BG3Y_H   , sizeof(u16) },
  { &WIN0H    , sizeof(u16) },
  { &WIN1H    , sizeof(u16) },
  { &WIN0V    , sizeof(u16) },
  { &WIN1V    , sizeof(u16) },
  { &WININ    , sizeof(u16) },
  { &WINOUT   , sizeof(u16) },
  { &MOSAIC   , sizeof(u16) },
  { &BLDMOD   , sizeof(u16) },
  { &COLEV    , sizeof(u16) },
  { &COLY     , sizeof(u16) },
  { &DM0SAD_L , sizeof(u16) },
  { &DM0SAD_H , sizeof(u16) },
  { &DM0DAD_L , sizeof(u16) },
  { &DM0DAD_H , sizeof(u16) },
  { &DM0CNT_L , sizeof(u16) },
  { &DM0CNT_H , sizeof(u16) },
  { &DM1SAD_L , sizeof(u16) },
  { &DM1SAD_H , sizeof(u16) },
  { &DM1DAD_L , sizeof(u16) },
  { &DM1DAD_H , sizeof(u16) },
  { &DM1CNT_L , sizeof(u16) },
  { &DM1CNT_H , sizeof(u16) },
  { &DM2SAD_L , sizeof(u16) },
  { &DM2SAD_H , sizeof(u16) },
  { &DM2DAD_L , sizeof(u16) },
  { &DM2DAD_H , sizeof(u16) },
  { &DM2CNT_L , sizeof(u16) },
  { &DM2CNT_H , sizeof(u16) },
  { &DM3SAD_L , sizeof(u16) },
  { &DM3SAD_H , sizeof(u16) },
  { &DM3DAD_L , sizeof(u16) },
  { &DM3DAD_H , sizeof(u16) },
  { &DM3CNT_L , sizeof(u16) },
  { &DM3CNT_H , sizeof(u16) },
  { &TM0D     , sizeof(u16) },
  { &TM0CNT   , sizeof(u16) },
  { &TM1D     , sizeof(u16) },
  { &TM1CNT   , sizeof(u16) },
  { &TM2D     , sizeof(u16) },
  { &TM2CNT   , sizeof(u16) },
  { &TM3D     , sizeof(u16) },
  { &TM3CNT   , sizeof(u16) },
  { &P1       , sizeof(u16) },
  { &IE       , sizeof(u16) },
  { &IF       , sizeof(u16) },
  { &IME      , sizeof(u16) },
  { &holdState, sizeof(bool) },
  { &holdType, sizeof(int) },
  { &lcdTicks, sizeof(int) },
  { &timer0On , sizeof(bool) },
  { &timer0Ticks , sizeof(int) },
  { &timer0Reload , sizeof(int) },
  { &timer0ClockReload  , sizeof(int) },
  { &timer1On , sizeof(bool) },
  { &timer1Ticks , sizeof(int) },
  { &timer1Reload , sizeof(int) },
  { &timer1ClockReload  , sizeof(int) },
  { &timer2On , sizeof(bool) },
  { &timer2Ticks , sizeof(int) },
  { &timer2Reload , sizeof(int) },
  { &timer2ClockReload  , sizeof(int) },
  { &timer3On , sizeof(bool) },
  { &timer3Ticks , sizeof(int) },
  { &timer3Reload , sizeof(int) },
  { &timer3ClockReload  , sizeof(int) },
  { &dma0Source , sizeof(u32) },
  { &dma0Dest , sizeof(u32) },
  { &dma1Source , sizeof(u32) },
  { &dma1Dest , sizeof(u32) },
  { &dma2Source , sizeof(u32) },
  { &dma2Dest , sizeof(u32) },
  { &dma3Source , sizeof(u32) },
  { &dma3Dest , sizeof(u32) },
  { &fxOn, sizeof(bool) },
  { &windowOn, sizeof(bool) },
  { &N_FLAG , sizeof(bool) },
  { &C_FLAG , sizeof(bool) },
  { &Z_FLAG , sizeof(bool) },
  { &V_FLAG , sizeof(bool) },
  { &armState , sizeof(bool) },
  { &armIrqEnable , sizeof(bool) },
  { &armNextPC , sizeof(u32) },
  { &armMode , sizeof(int) },
  { &saveType , sizeof(int) },
  { NULL, 0 } 
};
*/
int romSize = 0x200000; //test normal 0x2000000 current 1/10 oh no only 2.4 MB


bool CPUWriteBatteryFile(const char *fileName)
{
  if(saveType == 0) {
    if(eepromInUse){
		saveType = 3;
	}
    else switch(saveType) {
		case 1:
			saveType = 1;
		break;
		case 2:
			saveType = 2;
			break;
		}
	}
  
  if(saveType!=5) {
    FILE *filehandle = NULL;
	//coto: allow savefix support. Means if the read savefile (according to savefix logic) was fixed in memory, 
	//it is now updated back to file. So the next time the file is read correctly, and not as a saveFix file.
	
	if(pendingSaveFix == true){
		filehandle = fopen(fileName, "w+");
		pendingSaveFix = false;
		iprintf("[SaveFix]Write:OK \n");
		iprintf("File updated. ");
	}
	else{
		filehandle = fopen(fileName, "wb");
	}
	
    if(!filehandle) {
      systemMessage(MSG_ERROR_CREATING_FILE, N_("Error creating file %s"),
                    fileName);
      return false;
    }
    
    // only save if Flash/Sram in use or EEprom in use
    if(saveType != 3) {
      if(saveType == 2) {
        if(fwrite(flashSaveMemory, 1, flashSize, filehandle) != (size_t)flashSize) {
          fclose(filehandle);
          return false;
        }
      } else {
        if(fwrite(flashSaveMemory, 1, 0x10000, filehandle) != 0x10000) {
          fclose(filehandle);
          return false;
        }
      }
    } else {
      if(fwrite(eepromData, 1, eepromSize, filehandle) != (size_t)eepromSize) {
        fclose(filehandle);
        return false;
      }
    }
    fclose(filehandle);
  }
  return true;
}

bool CPUReadBatteryFile(const char *fileName)
{
  FILE *file = fopen(fileName, "rb");
    
  if(!file)
    return false;
  
  // check file size to know what we should read
  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  fseek(file, 0, SEEK_SET);
  
  systemSaveUpdateCounter = SYSTEM_SAVE_NOT_UPDATED;

  if(size == 512 || size == 0x2000) {
    if(fread(eepromData, 1, size, file) != (size_t)size) {
      fclose(file);
      return false;
    }
  } else {
    if(size == 0x20000) {
      if(fread(flashSaveMemory, 1, 0x20000, file) != 0x20000) {
        fclose(file);
        return false;
      }
      flashSetSize(0x20000);
    } 
	else {
		if(fread(flashSaveMemory, 1, 0x10000, file) != 0x10000) {
			fclose(file);
			return false;
		}
        
		//fix pokemon sapphire or ruby savefile if 64K
		//up to this point the header was read from file
		if(
			cpuSaveType == 3 
			&&
			(
				(save_deciderByTitle((char*)&gbaheader.title, (char*)"POKEMON SAPP",sizeof(gbaheader.title)) == true)
					||
				(save_deciderByTitle((char*)&gbaheader.title, (char*)"POKEMON RUBY",sizeof(gbaheader.title)) == true)
			)
		){
			flashSetSize(0x20000);
			memcpy((u8 *)(flashSaveMemory+0x10000), (u8 *)(flashSaveMemory), 0x10000);
			iprintf("[SaveFix]Performed.\n");
			iprintf("Please save in-game and save file (Y).");
			pendingSaveFix = true;
			SaveSizeBeforeFix = size;
			SaveSizeAfterFix = 0x20000;
		}		
	    else{
			flashSetSize(0x10000);
		}
	}
  }
  fclose(file);
  return true;
}

int CPULoadRom(const char *szFile,bool extram)
{

  bios = (u8 *)calloc(1,0x4000);
  if(bios == NULL) {
    systemMessage(MSG_OUT_OF_MEMORY, N_("Failed to allocate memory for %s"),
                  "BIOS");
    //CPUCleanUp();
    return 0;
  }    

  systemSaveUpdateCounter = SYSTEM_SAVE_NOT_UPDATED;
  
	rom = 0;
	romSize = 0x40000;
  /*workRAM = (u8*)0x02000000;(u8 *)calloc(1, 0x40000);
  if(workRAM == NULL) {
    systemMessage(MSG_OUT_OF_MEMORY, N_("Failed to allocate memory for %s"),
                  "WRAM");
    return 0;
  }*/
#ifdef loadindirect
  u8 *whereToLoad = rom;
  if(cpuIsMultiBoot)whereToLoad = workRAM;

		if(!utilLoad(szFile,
						  whereToLoad,
						  romSize,extram))
		{
			return 0;
		}
#else
rom = (u8*)puzzleorginal_bin;  //rom = (u8*)puzzleorginal_bin;
#endif

  /*internalRAM = (u8 *)0x03000000;//calloc(1,0x8000);
  if(internalRAM == NULL) {
    systemMessage(MSG_OUT_OF_MEMORY, N_("Failed to allocate memory for %s"),
                  "IRAM");
    //CPUCleanUp();
    return 0;
  }*/
  /*paletteRAM = (u8 *)0x05000000;//calloc(1,0x400);
  if(paletteRAM == NULL) {
    systemMessage(MSG_OUT_OF_MEMORY, N_("Failed to allocate memory for %s"),
                  "PRAM");
    //CPUCleanUp();
    return 0;
  }*/      
  /*vram = (u8 *)0x06000000;//calloc(1, 0x20000);
  if(vram == NULL) {
    systemMessage(MSG_OUT_OF_MEMORY, N_("Failed to allocate memory for %s"),
                  "VRAM");
    //CPUCleanUp();
    return 0;
  }*/      
  /*emultoroam = (u8 *)0x07000000;calloc(1, 0x400); //ichfly test
  if(emultoroam == NULL) {
    systemMessage(MSG_OUT_OF_MEMORY, N_("Failed to allocate memory for %s"),
                  "emultoroam");
    //CPUCleanUp();
    return 0;
  }      
  pix = (u8 *)calloc(1, 4 * 241 * 162);
  if(pix == NULL) {
    systemMessage(MSG_OUT_OF_MEMORY, N_("Failed to allocate memory for %s"),
                  "PIX");
    //CPUCleanUp();
    return 0;
  }  */
  /*ioMem = (u8 *)calloc(1, 0x400);
  if(ioMem == NULL) {
    systemMessage(MSG_OUT_OF_MEMORY, N_("Failed to allocate memory for %s"),
                  "IO");
    //CPUCleanUp();
    return 0;
  }*/      

  flashInit();
  eepromInit();

  //CPUUpdateRenderBuffers(true);

  return romSize;
}

__attribute__((section(".itcm")))
void  __attribute__ ((hot)) doDMAslow(u32 &s, u32 &d, u32 si, u32 di, u32 c, int transfer32) //ichfly veraltet
{

	cpuDmaCount = c;
  if(transfer32) {
    s &= 0xFFFFFFFC;
    if(s < 0x02000000 && (reg[15].I >> 24)) {
      while(c != 0) {
        CPUWriteMemory(d, 0);
        d += di;
        c--;
      }
    } else {
      while(c != 0) {
        cpuDmaLast = CPUReadMemory(s);
        CPUWriteMemory(d, cpuDmaLast);
        d += di;
        s += si;
        c--;
      }
    }
  } else {
    s &= 0xFFFFFFFE;
    si = (int)si >> 1;
    di = (int)di >> 1;
    if(s < 0x02000000 && (reg[15].I >> 24)) {
      while(c != 0) {
        CPUWriteHalfWord(d, 0);
        d += di;
        c--;
      }
    } else {
      while(c != 0) {
        cpuDmaLast = CPUReadHalfWord(s);
        CPUWriteHalfWord(d, cpuDmaLast);
        cpuDmaLast |= (cpuDmaLast<<16);
        d += di;
        s += si;
        c--;
      }
    }
  }

}
__attribute__((section(".itcm")))
void  __attribute__ ((hot)) doDMA(u32 &s, u32 &d, u32 si, u32 di, u32 c, int transfer32) //ichfly veraltet
{
	if(si == 0 || di == 0 || s < 0x02000000 || d < 0x02000000 || (d & ~0xFFFFFF) == 0x04000000 || (s & ~0xFFFFFF) == 0x04000000 || s >= 0x0D000000 || d >= 0x0D000000)
	{
		doDMAslow(s, d, si, di, c, transfer32); //some checks
		return;
	}
	else
	{
		if(s & 0x08000000)
		{
#ifdef uppern_read_emulation

			if( (int)((s&0x1FFFFFF) + (c*4)) > romSize) //slow
			{
#ifdef print_uppern_read_emulation
				iprintf("highdmaread %X %X %X %X %X %X\r\n",s,d,c,si,di,transfer32);
#endif
				if( ( (int)di == -4) || ((int)si == -4) )//this can't work the slow way so use the
				{
					doDMAslow(s, d, si, di, c, transfer32); //very slow way
					return;
				}
				if(transfer32)
				{
#ifdef ownfilebuffer
					//iprintf("4 %08X %08X %08X %08X ",s,d,c,*(u32 *)d);
					ichfly_readdma_rom((u32)(s&0x1FFFFFF),(u8 *)d,c,4);
					//iprintf("exit%08X ",*(u32 *)d);
					//while(1);
#else
					//doDMAslow(s, d, si, di, c, transfer32);
					fseek (ichflyfilestream , (s&0x1FFFFFF) , SEEK_SET);
					//iprintf("seek %08X\r\n",s&0x1FFFFFF);
					int dkdkdkdk = fread ((void*)d,1,c * 4,ichflyfilestream); // fist is buggy
					//iprintf("(%08X %08X %08X) ret %08X\r\n",d,c,ichflyfilestream,dkdkdkdk);
#endif
				}
				else
				{
#ifdef ownfilebuffer
					//iprintf("2 %08X %08X %08X %04X ",s,d,c,*(u16 *)d);
					ichfly_readdma_rom((u32)(s&0x1FFFFFF),(u8 *)d,c,2);
					//iprintf("exit%04X ",*(u16 *)d);
					//while(1);
#else
					//iprintf("teeees");
					//doDMAslow(s, d, si, di, c, transfer32);
					fseek (ichflyfilestream , (s&0x1FFFFFF) , SEEK_SET);
					//iprintf("seek %08X\r\n",s&0x1FFFFFF);
					int dkdkdkdk = fread ((void*)d,1,c * 2,ichflyfilestream);
					//iprintf("(%08X %08X %08X) ret %08X\r\n",d,c,ichflyfilestream,dkdkdkdk);
#endif
				}
				//doDMAslow(s, d, si, di, c, transfer32); //very slow way
				return;
			}
			else
			{
				s = (u32)(rom +  (s & 0x01FFFFFF));
			}

#else
			s = (u32)(rom +  (s & 0x01FFFFFF));
#endif
		}
		//while(dmaBusy(3)); // ichfly wait for dma 3 not needed
		DMA3_SRC = s;
		DMA3_DEST = d;

		int tmpnum = (DMA_ENABLE | c);
		if(transfer32)
			tmpnum |= DMA_32_BIT;
		if( (int)di == -4) 
			tmpnum |= DMA_DST_DEC;
		if( (int)si == -4) 
			tmpnum |= DMA_SRC_DEC;
		DMA3_CR = tmpnum;
		//iprintf("%x,%x,%x",(unsigned int)s,(unsigned int)d,(unsigned int)tmpnum);
	}

}
__attribute__((section(".itcm")))
void  __attribute__ ((hot)) CPUCheckDMA(int reason, int dmamask)
{
  // DMA 0
  if((DM0CNT_H & 0x8000) && (dmamask & 1)) {
    if(((DM0CNT_H >> 12) & 3) == reason) {
      u32 sourceIncrement = 4;
      u32 destIncrement = 4;
      switch((DM0CNT_H >> 7) & 3) {
      case 0:
        break;
      case 1:
        sourceIncrement = (u32)-4;
        break;
      case 2:
        sourceIncrement = 0;
        break;
      }
      switch((DM0CNT_H >> 5) & 3) {
      case 0:
        break;
      case 1:
        destIncrement = (u32)-4;
        break;
      case 2:
        destIncrement = 0;
        break;
      }      
#ifdef DEV_VERSION
      if(systemVerbose & VERBOSE_DMA0) {
        int count = (DM0CNT_L ? DM0CNT_L : 0x4000) << 1;
        if(DM0CNT_H & 0x0400)
          count <<= 1;
        iprintf("DMA0: s=%08x d=%08x c=%04x count=%08x\n", dma0Source, dma0Dest, 
            DM0CNT_H,
            count);
      }
#endif
      doDMA(dma0Source, dma0Dest, sourceIncrement, destIncrement,
            DM0CNT_L ? DM0CNT_L : 0x4000,
            DM0CNT_H & 0x0400);
      cpuDmaHack = true;

      /*if(DM0CNT_H & 0x4000) {
        IF |= 0x0100;
        UPDATE_REG(0x202, IF);
        cpuNextEvent = cpuTotalTicks;
      }*/ //ichfly todo
      
      if(((DM0CNT_H >> 5) & 3) == 3) {
        dma0Dest = DM0DAD_L | (DM0DAD_H << 16);
      }
      
      if(!(DM0CNT_H & 0x0200) || (reason == 0)) {
        DM0CNT_H &= 0x7FFF;
        UPDATE_REG(0xBA, DM0CNT_H);
      }
    }
  }
  
  // DMA 1
  if((DM1CNT_H & 0x8000) && (dmamask & 2)) {
    if(((DM1CNT_H >> 12) & 3) == reason) {
      u32 sourceIncrement = 4;
      u32 destIncrement = 4;
      switch((DM1CNT_H >> 7) & 3) {
      case 0:
        break;
      case 1:
        sourceIncrement = (u32)-4;
        break;
      case 2:
        sourceIncrement = 0;
        break;
      }
      switch((DM1CNT_H >> 5) & 3) {
      case 0:
        break;
      case 1:
        destIncrement = (u32)-4;
        break;
      case 2:
        destIncrement = 0;
        break;
      }      
      if(reason == 3) {
#ifdef DEV_VERSION
        if(systemVerbose & VERBOSE_DMA1) {
          iprintf("DMA1: s=%08x d=%08x c=%04x count=%08x\n", dma1Source, dma1Dest,
              DM1CNT_H,
              16);
        }
#endif  
        doDMA(dma1Source, dma1Dest, sourceIncrement, 0, 4,
              0x0400);
      } else {
#ifdef DEV_VERSION
        if(systemVerbose & VERBOSE_DMA1) {
          int count = (DM1CNT_L ? DM1CNT_L : 0x4000) << 1;
          if(DM1CNT_H & 0x0400)
            count <<= 1;
          iprintf("DMA1: s=%08x d=%08x c=%04x count=%08x\n", dma1Source, dma1Dest,
              DM1CNT_H,
              count);
        }
#endif          
        doDMA(dma1Source, dma1Dest, sourceIncrement, destIncrement,
              DM1CNT_L ? DM1CNT_L : 0x4000,
              DM1CNT_H & 0x0400);
      }
      cpuDmaHack = true;

      /*if(DM1CNT_H & 0x4000) {
        IF |= 0x0200;
        UPDATE_REG(0x202, IF);
        cpuNextEvent = cpuTotalTicks;
      }*/ //ichfly todo
      
      if(((DM1CNT_H >> 5) & 3) == 3) {
        dma1Dest = DM1DAD_L | (DM1DAD_H << 16);
      }
      
      if(!(DM1CNT_H & 0x0200) || (reason == 0)) {
        DM1CNT_H &= 0x7FFF;
        UPDATE_REG(0xC6, DM1CNT_H);
      }
    }
  }
  
  // DMA 2
  if((DM2CNT_H & 0x8000) && (dmamask & 4)) {
    if(((DM2CNT_H >> 12) & 3) == reason) {
      u32 sourceIncrement = 4;
      u32 destIncrement = 4;
      switch((DM2CNT_H >> 7) & 3) {
      case 0:
        break;
      case 1:
        sourceIncrement = (u32)-4;
        break;
      case 2:
        sourceIncrement = 0;
        break;
      }
      switch((DM2CNT_H >> 5) & 3) {
      case 0:
        break;
      case 1:
        destIncrement = (u32)-4;
        break;
      case 2:
        destIncrement = 0;
        break;
      }      
      if(reason == 3) {
#ifdef DEV_VERSION
        if(systemVerbose & VERBOSE_DMA2) {
          int count = (4) << 2;
          iprintf("DMA2: s=%08x d=%08x c=%04x count=%08x\n", dma2Source, dma2Dest,
              DM2CNT_H,
              count);
        }
#endif                  
        doDMA(dma2Source, dma2Dest, sourceIncrement, 0, 4,
              0x0400);
      } else {
#ifdef DEV_VERSION
        if(systemVerbose & VERBOSE_DMA2) {
          int count = (DM2CNT_L ? DM2CNT_L : 0x4000) << 1;
          if(DM2CNT_H & 0x0400)
            count <<= 1;
          iprintf("DMA2: s=%08x d=%08x c=%04x count=%08x\n", dma2Source, dma2Dest,
              DM2CNT_H,
              count);
        }
#endif                  
        doDMA(dma2Source, dma2Dest, sourceIncrement, destIncrement,
              DM2CNT_L ? DM2CNT_L : 0x4000,
              DM2CNT_H & 0x0400);
      }
      cpuDmaHack = true;

      /*if(DM2CNT_H & 0x4000) {
        IF |= 0x0400;
        UPDATE_REG(0x202, IF);
        cpuNextEvent = cpuTotalTicks;
      }*/ //ichfly todo

      if(((DM2CNT_H >> 5) & 3) == 3) {
        dma2Dest = DM2DAD_L | (DM2DAD_H << 16);
      }
      
      if(!(DM2CNT_H & 0x0200) || (reason == 0)) {
        DM2CNT_H &= 0x7FFF;
        UPDATE_REG(0xD2, DM2CNT_H);
      }
    }
  }

  // DMA 3
  if((DM3CNT_H & 0x8000) && (dmamask & 8)) {
    if(((DM3CNT_H >> 12) & 3) == reason) {
      u32 sourceIncrement = 4;
      u32 destIncrement = 4;
      switch((DM3CNT_H >> 7) & 3) {
      case 0:
        break;
      case 1:
        sourceIncrement = (u32)-4;
        break;
      case 2:
        sourceIncrement = 0;
        break;
      }
      switch((DM3CNT_H >> 5) & 3) {
      case 0:
        break;
      case 1:
        destIncrement = (u32)-4;
        break;
      case 2:
        destIncrement = 0;
        break;
      }      
#ifdef DEV_VERSION
      if(systemVerbose & VERBOSE_DMA3) {
        int count = (DM3CNT_L ? DM3CNT_L : 0x10000) << 1;
        if(DM3CNT_H & 0x0400)
          count <<= 1;
        iprintf("DMA3: s=%08x d=%08x c=%04x count=%08x\n", dma3Source, dma3Dest,
            DM3CNT_H,
            count);
      }
#endif                
      doDMA(dma3Source, dma3Dest, sourceIncrement, destIncrement,
            DM3CNT_L ? DM3CNT_L : 0x10000,
            DM3CNT_H & 0x0400);
      /*if(DM3CNT_H & 0x4000) {
        IF |= 0x0800;
        UPDATE_REG(0x202, IF);
        cpuNextEvent = cpuTotalTicks;
      }*/ //ichfly todo

      if(((DM3CNT_H >> 5) & 3) == 3) {
        dma3Dest = DM3DAD_L | (DM3DAD_H << 16);
      }
      
      if(!(DM3CNT_H & 0x0200) || (reason == 0)) {
        DM3CNT_H &= 0x7FFF;
        UPDATE_REG(0xDE, DM3CNT_H);
      }
    }
  }
}

void  __attribute__ ((hot)) CPUUpdateRegister(u32 address, u16 value)
{
  	/*if(0x60 > address && address > 0x7)
	{
			//iprintf("UP16 %x %x\r\n",address,value);
		    *(u16 *)((address & 0x3FF) + 0x4000000) = value;
			//swiDelay(0x2000000);
			//swiDelay(0x2000000);
			//swiDelay(0x2000000);
			//swiDelay(0x2000000);
			
	}*/

  switch(address) {
  case 0x00:
    {
		DISPCNT = value & 0xFFF7;	//ignored bit: 3     gba: Reserved / CGB Mode    (0=GBA, 1=CGB; can be set only by BIOS opcodes)
														//ds: BG0 2D/3D Selection (instead CGB Mode) (0=2D, 1=3D)
		UPDATE_REG(0x00, DISPCNT);
		
		u32 dsValue = 0;
		dsValue  = value & 0xFF80;
		dsValue |= (value & (1 << 5)) ? (1 << 23) : 0;	// oam hblank access
		dsValue |= (value & (1 << 6)) ? (1 << 4) : 0;	// obj mapping 1d/2d 
		#ifndef capture_and_pars
		dsValue |= (value & (1 << 7)) ? 0 : (1 << 16);	// forced blank => no display mode (both)
		#endif
		
		//gba 2 ds proper bg mode patch
		switch((value&0x7)){
			case(0):{
				//mode 0: 
				dsValue|=(0);	//direct mode0 (bg0:text--3D/bg1:text/bg2:text/bg3:text)
			}
			break;
			case(1):{
				//mode 1: //affine == rot-scal
				dsValue|=(2);
			}
			break;
			case(2):{
				//mode 2: //There are 2 available tiled backgrounds, but both can be rotated and scaled.
										//gba:(bg0:disabled/bg1:disabled/bg2:rot-scal/bg3:rot-scal)
										//ds: (bg0:disabled/bg1:disabled/bg2:rot-scal/bg3:rot-scal)
				dsValue|=(2);
			}
			break;
			case(3):{
				dsValue|=(3);
			}
			break;
			case(4):{
				dsValue|=(4);
			}
			break;
			case(5):{
				dsValue|=(5);
			}
			break;
		}
		REG_DISPCNT = dsValue;
    }
    break;
  case 0x04:
    DISPSTAT = (value & 0xFF38) | (DISPSTAT & 7);
    UPDATE_REG(0x04, DISPSTAT);

#ifdef usebuffedVcout
	{
	u16 tempDISPSTAT = (u16)((DISPSTAT&0xFF) | ((VCountgbatods[DISPSTAT>>8]) << 8));
#else
	//8-15  V-Count Setting (LYC)      (0..227)

	{
		tempDISPSTAT = DISPSTAT >> 8;
		//float help = tempDISPSTAT;
		if(tempDISPSTAT < 160)
		{
			tempDISPSTAT = (tempDISPSTAT * 306);//tempDISPSTAT = (help * 1.2);
			tempDISPSTAT = tempDISPSTAT | (DISPSTAT & 0xFF); //1.15350877; //already seeked
			//help3 = (help + 1) * (1./1.2); //1.15350877;  // ichfly todo it is to slow
		}
		else
		{
			if(tempDISPSTAT < 220)
			{
				tempDISPSTAT = ((tempDISPSTAT - 160) * 266);//tempDISPSTAT = ((help - 160) * 1.04411764);//tempDISPSTAT = ((help - 160) * 1.04411764);
				tempDISPSTAT = (tempDISPSTAT + 192 * 0x100) | (DISPSTAT & 0xFF); //1.15350877;
			}
			else if(tempDISPSTAT < 228)
			{
				tempDISPSTAT = (DISPSTAT & 0x3F) | 0xFF00;
			}
			else tempDISPSTAT = (DISPSTAT & 0x1F);		
		}
#endif
#ifdef forceHBlankirqs
	*(u16 *)(0x4000004) = tempDISPSTAT | BIT(4);
#else
	*(u16 *)(0x4000004) = tempDISPSTAT;
#endif
	}


    break;
  case 0x06:
    // not writable in NDS mode bzw not possible todo
    break;
  case 0x08:
    BG0CNT = (value & 0xDFCF);
    UPDATE_REG(0x08, BG0CNT);
	*(u16 *)(0x4000008) = BG0CNT;
    break;
  case 0x0A:
    BG1CNT = (value & 0xDFCF);
    UPDATE_REG(0x0A, BG1CNT);
    *(u16 *)(0x400000A) = BG1CNT;
	break;
  case 0x0C:
    BG2CNT = (value & 0xFFCF);
    UPDATE_REG(0x0C, BG2CNT);
	if((DISPCNT & 7) < 3)*(u16 *)(0x400000C) = BG2CNT;
	else //ichfly some extra handling 
	{
		REG_BG3CNT = REG_BG3CNT | (BG2CNT & 0x43);
	}
    break;
  case 0x0E:
    BG3CNT = (value & 0xFFCF);
    UPDATE_REG(0x0E, BG3CNT);
	if((DISPCNT & 7) < 3)*(u16 *)(0x400000E) = BG3CNT;
    break;
  case 0x10:
    BG0HOFS = value & 511;
    UPDATE_REG(0x10, BG0HOFS);
    *(u16 *)(0x4000010) = value;
	break;
  case 0x12:
    BG0VOFS = value & 511;
    UPDATE_REG(0x12, BG0VOFS);
    *(u16 *)(0x4000012) = value;
	break;
  case 0x14:
    BG1HOFS = value & 511;
    UPDATE_REG(0x14, BG1HOFS);
	*(u16 *)(0x4000014) = value;
    break;
  case 0x16:
    BG1VOFS = value & 511;
    UPDATE_REG(0x16, BG1VOFS);
    *(u16 *)(0x4000016) = value;
	break;      
  case 0x18:
    BG2HOFS = value & 511;
    UPDATE_REG(0x18, BG2HOFS);
	*(u16 *)(0x4000018) = value;
    break;
  case 0x1A:
    BG2VOFS = value & 511;
    UPDATE_REG(0x1A, BG2VOFS);
    if((DISPCNT & 7) < 3)*(u16 *)(0x400001A) = value; //ichfly only update if it is save
	break;
  case 0x1C:
    BG3HOFS = value & 511;
    UPDATE_REG(0x1C, BG3HOFS);
    if((DISPCNT & 7) < 3)*(u16 *)(0x400001C) = value; //ichfly only update if it is save
	break;
  case 0x1E:
    BG3VOFS = value & 511;
    UPDATE_REG(0x1E, BG3VOFS);
    *(u16 *)(0x400001E) = value;
	break;      
  case 0x20:
    BG2PA = value;
    UPDATE_REG(0x20, BG2PA);
    if((DISPCNT & 7) < 3)*(u16 *)(0x4000020) = value;
	else *(u16 *)(0x4000030) = value;
	break;
  case 0x22:
    BG2PB = value;
    UPDATE_REG(0x22, BG2PB);
    if((DISPCNT & 7) < 3)*(u16 *)(0x4000022) = value;
	else *(u16 *)(0x4000032) = value;
	break;
  case 0x24:
    BG2PC = value;
    UPDATE_REG(0x24, BG2PC);
    if((DISPCNT & 7) < 3)*(u16 *)(0x4000024) = value;
	else *(u16 *)(0x4000034) = value;
	break;
  case 0x26:
    BG2PD = value;
    UPDATE_REG(0x26, BG2PD);
	if((DISPCNT & 7) < 3)*(u16 *)(0x4000026) = value;
	else *(u16 *)(0x4000036) = value;
	break;
  case 0x28:
    BG2X_L = value;
    UPDATE_REG(0x28, BG2X_L);
    //gfxBG2Changed |= 1;
	if((DISPCNT & 7) < 3)*(u16 *)(0x4000028) = value;
	else *(u16 *)(0x4000038) = value;
    break;
  case 0x2A:
    BG2X_H = (value & 0xFFF);
    UPDATE_REG(0x2A, BG2X_H);
    //gfxBG2Changed |= 1;
	if((DISPCNT & 7) < 3)*(u16 *)(0x400002A) = value;
	else *(u16 *)(0x400003A) = value;
    break;
  case 0x2C:
    BG2Y_L = value;
    UPDATE_REG(0x2C, BG2Y_L);
    //gfxBG2Changed |= 2;
	if((DISPCNT & 7) < 3)*(u16 *)(0x400002C) = value;
	else *(u16 *)(0x400003C) = value;
    break;
  case 0x2E:
    BG2Y_H = value & 0xFFF;
    UPDATE_REG(0x2E, BG2Y_H);
    //gfxBG2Changed |= 2;
	if((DISPCNT & 7) < 3)*(u16 *)(0x400002E) = value;
	else *(u16 *)(0x400003E) = value;
    break;
  case 0x30:
    BG3PA = value;
    UPDATE_REG(0x30, BG3PA);
    if((DISPCNT & 7) < 3)*(u16 *)(0x4000030) = value;
	break;
  case 0x32:
    BG3PB = value;
    UPDATE_REG(0x32, BG3PB);
    if((DISPCNT & 7) < 3)*(u16 *)(0x4000032) = value;
	break;
  case 0x34:
    BG3PC = value;
    UPDATE_REG(0x34, BG3PC);
    if((DISPCNT & 7) < 3)*(u16 *)(0x4000034) = value;
	break;
  case 0x36:
    BG3PD = value;
    UPDATE_REG(0x36, BG3PD);
    if((DISPCNT & 7) < 3)*(u16 *)(0x4000036) = value;
	break;
  case 0x38:
    BG3X_L = value;
    UPDATE_REG(0x38, BG3X_L);
    //gfxBG3Changed |= 1;
    if((DISPCNT & 7) < 3)*(u16 *)(0x4000038) = value;
	break;
  case 0x3A:
    BG3X_H = value & 0xFFF;
    UPDATE_REG(0x3A, BG3X_H);
    //gfxBG3Changed |= 1;    
    if((DISPCNT & 7) < 3)*(u16 *)(0x400003A) = value;
	break;
  case 0x3C:
    BG3Y_L = value;
    UPDATE_REG(0x3C, BG3Y_L);
    //gfxBG3Changed |= 2;    
    if((DISPCNT & 7) < 3)*(u16 *)(0x400003C) = value;
	break;
  case 0x3E:
    BG3Y_H = value & 0xFFF;
    UPDATE_REG(0x3E, BG3Y_H);
    //gfxBG3Changed |= 2;    
    if((DISPCNT & 7) < 3)*(u16 *)(0x400003E) = value;
	break;
  case 0x40:
    WIN0H = value;
    UPDATE_REG(0x40, WIN0H);
    //CPUUpdateWindow0();
    *(u16 *)(0x4000040) = value;
	break;
  case 0x42:
    WIN1H = value;
    UPDATE_REG(0x42, WIN1H);
	*(u16 *)(0x4000042) = value;
    //CPUUpdateWindow1();    
    break;      
  case 0x44:
    WIN0V = value;
    UPDATE_REG(0x44, WIN0V);
    *(u16 *)(0x4000044) = value;
	break;
  case 0x46:
    WIN1V = value;
    UPDATE_REG(0x46, WIN1V);
    *(u16 *)(0x4000046) = value;
	break;
  case 0x48:
    WININ = value & 0x3F3F;
    UPDATE_REG(0x48, WININ);
    if((DISPCNT & 7) < 3)*(u16 *)(0x4000048) = value;
	else
	{
		int tempWININ = WININ & ~0x404;
		tempWININ = tempWININ | ((WININ & 0x404) << 1);
		WIN_IN = tempWININ;
	}
	break;
  case 0x4A:
    WINOUT = value & 0x3F3F;
    UPDATE_REG(0x4A, WINOUT);
    if((DISPCNT & 7) < 3)*(u16 *)(0x400004A) = value;
	else
	{
		int tempWINOUT = WINOUT & ~0x404;
		tempWINOUT = tempWINOUT | ((WINOUT & 0x404) << 1);
		WIN_OUT = tempWINOUT;
	}
	break;
  case 0x4C:
    MOSAIC = value;
    UPDATE_REG(0x4C, MOSAIC);
    *(u16 *)(0x400004C) = value;
	break;
  case 0x50:
    BLDMOD = value & 0x3FFF;
    UPDATE_REG(0x50, BLDMOD);
    //fxOn = ((BLDMOD>>6)&3) != 0;
    //CPUUpdateRender();
	if((DISPCNT & 7) < 3)*(u16 *)(0x4000050) = value;
	else
	{
		int tempBLDMOD = BLDMOD & ~0x404;
		tempBLDMOD = tempBLDMOD | ((BLDMOD & 0x404) << 1);
		REG_BLDCNT = tempBLDMOD;
	}
    break;
  case 0x52:
    COLEV = value & 0x1F1F;
    UPDATE_REG(0x52, COLEV);
    *(u16 *)(0x4000052) = value;
	break;
  case 0x54:
    COLY = value & 0x1F;
    UPDATE_REG(0x54, COLY);
	*(u16 *)(0x4000054) = value;
    break;
  case 0x60:
  case 0x62:
  case 0x64:
  case 0x68:
  case 0x6c:
  case 0x70:
  case 0x72:
  case 0x74:
  case 0x78:
  case 0x7c:
  case 0x80:
  case 0x84:
    /*soundEvent(address&0xFF, (u8)(value & 0xFF));
    soundEvent((address&0xFF)+1, (u8)(value>>8));
    break;*/ //ichfly disable sound
  case 0x82:
  case 0x88:
  case 0xa0:
  case 0xa2:
  case 0xa4:
  case 0xa6:
  case 0x90:
  case 0x92:
  case 0x94:
  case 0x96:
  case 0x98:
  case 0x9a:
  case 0x9c:
  case 0x9e:    
    //soundEvent(address&0xFF, value);  //ichfly send sound to arm7
#ifdef soundwriteprint
	  iprintf("ur %04x to %08x\r\n",value,address);
#endif
#ifdef arm9advsound
	  SendArm7Command((u32)(address | 0x80000000),(u32)value);	//faster in case we send a 0
#endif
	  UPDATE_REG(address,value);
    break;
  case 0xB0:
    DM0SAD_L = value;
    UPDATE_REG(0xB0, DM0SAD_L);
    break;
  case 0xB2:
    DM0SAD_H = value & 0x07FF;
    UPDATE_REG(0xB2, DM0SAD_H);
    break;
  case 0xB4:
    DM0DAD_L = value;
    UPDATE_REG(0xB4, DM0DAD_L);
    break;
  case 0xB6:
    DM0DAD_H = value & 0x07FF;
    UPDATE_REG(0xB6, DM0DAD_H);
    break;
  case 0xB8:
    DM0CNT_L = value & 0x3FFF;
    UPDATE_REG(0xB8, 0);
    break;
  case 0xBA:
    {
      bool start = ((DM0CNT_H ^ value) & 0x8000) ? true : false;
      value &= 0xF7E0;

      DM0CNT_H = value;
      UPDATE_REG(0xBA, DM0CNT_H);    
    
      if(start && (value & 0x8000)) {
        dma0Source = DM0SAD_L | (DM0SAD_H << 16);
        dma0Dest = DM0DAD_L | (DM0DAD_H << 16);
        CPUCheckDMA(0, 1);
      }
    }
    break;      
  case 0xBC:
#ifdef dmawriteprint
	iprintf("ur %04x to %08x\r\n",value,address);
#endif
#ifdef arm9advsound
	SendArm7Command((u32)(address | 0x80000000), (u32)value);	//faster in case we send a 0
#endif
    
	DM1SAD_L = value;
    UPDATE_REG(0xBC, DM1SAD_L);
    break;
  case 0xBE:
#ifdef dmawriteprint
	iprintf("ur %04x to %08x\r\n",value,address);
#endif
#ifdef arm9advsound
	SendArm7Command((u32)(address | 0x80000000), (u32)value);	//faster in case we send a 0
#endif

    DM1SAD_H = value & 0x0FFF;
    UPDATE_REG(0xBE, DM1SAD_H);
    break;
  case 0xC0:
#ifdef dmawriteprint
    iprintf("ur %04x to %08x\r\n",value,address);
#endif
#ifdef arm9advsound
	SendArm7Command((u32)(address | 0x80000000), (u32)value);	//faster in case we send a 0
#endif

	DM1DAD_L = value;
    UPDATE_REG(0xC0, DM1DAD_L);
    break;
  case 0xC2:
#ifdef dmawriteprint
	iprintf("ur %04x to %08x\r\n",value,address);
#endif
#ifdef arm9advsound
	SendArm7Command((u32)(address | 0x80000000), (u32)value);	//faster in case we send a 0
#endif

    DM1DAD_H = value & 0x07FF;
    UPDATE_REG(0xC2, DM1DAD_H);
    break;
  case 0xC4:
#ifdef dmawriteprint
	iprintf("ur %04x to %08x\r\n",value,address);
#endif
#ifdef arm9advsound
	SendArm7Command((u32)(address | 0x80000000), (u32)value);	//faster in case we send a 0
#endif

	DM1CNT_L = value & 0x3FFF;
    UPDATE_REG(0xC4, 0);
    break;
  case 0xC6:
#ifdef dmawriteprint
	iprintf("ur %04x to %08x\r\n",value,address);
#endif
#ifdef arm9advsound
	SendArm7Command((u32)(address | 0x80000000), (u32)value);	//faster in case we send a 0
#endif
	  {
      bool start = ((DM1CNT_H ^ value) & 0x8000) ? true : false;
      value &= 0xF7E0;
      
      DM1CNT_H = value;
      UPDATE_REG(0xC6, DM1CNT_H);
      
      if(start && (value & 0x8000)) {
        dma1Source = DM1SAD_L | (DM1SAD_H << 16);
        dma1Dest = DM1DAD_L | (DM1DAD_H << 16);
        CPUCheckDMA(0, 2);
      }
    }
    break;
  case 0xC8:
#ifdef dmawriteprint
	iprintf("ur %04x to %08x\r\n",value,address);
#endif
#ifdef arm9advsound
	SendArm7Command((u32)(address | 0x80000000), (u32)value);	//faster in case we send a 0
#endif

	DM2SAD_L = value;
    UPDATE_REG(0xC8, DM2SAD_L);
    break;
  case 0xCA:
#ifdef dmawriteprint
	iprintf("ur %04x to %08x\r\n",value,address);
#endif
#ifdef arm9advsound
	SendArm7Command((u32)(address | 0x80000000), (u32)value);	//faster in case we send a 0
#endif

	DM2SAD_H = value & 0x0FFF;
    UPDATE_REG(0xCA, DM2SAD_H);
    break;
  case 0xCC:
#ifdef dmawriteprint
	iprintf("ur %04x to %08x\r\n",value,address);
#endif
#ifdef arm9advsound
	SendArm7Command((u32)(address | 0x80000000), (u32)value);	//faster in case we send a 0
#endif

	DM2DAD_L = value;
    UPDATE_REG(0xCC, DM2DAD_L);
    break;
  case 0xCE:
#ifdef dmawriteprint
	iprintf("ur %04x to %08x\r\n",value,address);
#endif
#ifdef arm9advsound
	SendArm7Command((u32)(address | 0x80000000), (u32)value);	//faster in case we send a 0
#endif

	DM2DAD_H = value & 0x07FF;
    UPDATE_REG(0xCE, DM2DAD_H);
    break;
  case 0xD0:
#ifdef dmawriteprint

	iprintf("ur %04x to %08x\r\n",value,address);
#endif
#ifdef arm9advsound
	SendArm7Command((u32)(address | 0x80000000), (u32)value);	//faster in case we send a 0
#endif

	DM2CNT_L = value & 0x3FFF;
    UPDATE_REG(0xD0, 0);
    break;
  case 0xD2:
#ifdef dmawriteprint

	iprintf("ur %04x to %08x\r\n",value,address);
#endif
#ifdef arm9advsound
	SendArm7Command((u32)(address | 0x80000000), (u32)value);
#endif

	  {
      bool start = ((DM2CNT_H ^ value) & 0x8000) ? true : false;
      
      value &= 0xF7E0;
      
      DM2CNT_H = value;
      UPDATE_REG(0xD2, DM2CNT_H);
      
      if(start && (value & 0x8000)) {
        dma2Source = DM2SAD_L | (DM2SAD_H << 16);
        dma2Dest = DM2DAD_L | (DM2DAD_H << 16);

        CPUCheckDMA(0, 4);
      }            
    }
    break;
  case 0xD4:
    DM3SAD_L = value;
    UPDATE_REG(0xD4, DM3SAD_L);
    break;
  case 0xD6:
    DM3SAD_H = value & 0x0FFF;
    UPDATE_REG(0xD6, DM3SAD_H);
    break;
  case 0xD8:
    DM3DAD_L = value;
    UPDATE_REG(0xD8, DM3DAD_L);
    break;
  case 0xDA:
    DM3DAD_H = value & 0x0FFF;
    UPDATE_REG(0xDA, DM3DAD_H);
    break;
  case 0xDC:
    DM3CNT_L = value;
    UPDATE_REG(0xDC, 0);
    break;
  case 0xDE:
    {
      bool start = ((DM3CNT_H ^ value) & 0x8000) ? true : false;

      value &= 0xFFE0;

      DM3CNT_H = value;
      UPDATE_REG(0xDE, DM3CNT_H);
    
      if(start && (value & 0x8000)) {
        dma3Source = DM3SAD_L | (DM3SAD_H << 16);
        dma3Dest = DM3DAD_L | (DM3DAD_H << 16);
        CPUCheckDMA(0,8);
      }
    }
    break;
 case 0x100:
    timer0Reload = value;
#ifdef printsoundtimer
	iprintf("ur %04x to %08x\r\n",value,address);
#endif
#ifdef arm9advsound
	SendArm7Command((u32)(address | 0x80000000), (u32)value);	//faster in case we send a 0
#endif

	UPDATE_REG(0x100, value);
    break;
  case 0x102:
    timer0Value = value;
    //timerOnOffDelay|=1;
    //cpuNextEvent = cpuTotalTicks;
	UPDATE_REG(0x102, value);
#ifdef printsoundtimer
	iprintf("ur %04x to %08x\r\n",value,address);
#endif
#ifdef arm9advsound
	SendArm7Command((u32)(address | 0x80000000), (u32)value);	//faster in case we send a 0
#endif
	/*if(timer0Reload & 0x8000)
	{
		if((value & 0x3) == 0)
		{
			*(u16 *)(0x4000100) = timer0Reload >> 5;
			*(u16 *)(0x4000102) = value + 1;
			break;
		}
		if((value & 0x3) == 1)
		{
			*(u16 *)(0x4000100) = timer0Reload >> 1;
			*(u16 *)(0x4000102) = value + 1;
			break;
		}
		if((value & 3) == 2)
		{
			*(u16 *)(0x4000100) = timer0Reload >> 1;
			*(u16 *)(0x4000102) = value + 1;
			break;
		}
		*(u16 *)(0x4000102) = value;
		iprintf("big reload0\r\n");//todo 
	}
	else*/
	{	
		*(u16 *)(0x4000100) = timer1Reload << 1;
		*(u16 *)(0x4000102) = value;
	}
    break;
  case 0x104:
    timer1Reload = value;
#ifdef printsoundtimer
	iprintf("ur %04x to %08x\r\n",value,address);
#endif
#ifdef arm9advsound
	SendArm7Command((u32)(address | 0x80000000), (u32)value);	//faster in case we send a 0
#endif

	UPDATE_REG(0x104, value);
	break;
  case 0x106:
#ifdef printsoundtimer
	iprintf("ur %04x to %08x\r\n",value,address);
#endif
#ifdef arm9advsound
	SendArm7Command((u32)(address | 0x80000000), (u32)value);	//faster in case we send a 0
#endif

    timer1Value = value;
    //timerOnOffDelay|=2;
    //cpuNextEvent = cpuTotalTicks;
	UPDATE_REG(0x106, value);

	/*if(timer1Reload & 0x8000)
	{
		if((value & 0x3) == 0)
		{
			*(u16 *)(0x4000104) = timer1Reload >> 5;
			*(u16 *)(0x4000106) = value + 1;
			break;
		}
		if((value & 0x3) == 1)
		{
			*(u16 *)(0x4000104) = timer1Reload >> 1;
			*(u16 *)(0x4000106) = value + 1;
			break;
		}
		if((value & 3) == 2)
		{
			*(u16 *)(0x4000104) = timer1Reload >> 1;
			*(u16 *)(0x4000106) = value + 1;
			break;
		}
		*(u16 *)(0x4000106) = value;
		iprintf("big reload1\r\n");//todo 
	}
	else*/
	{	
		*(u16 *)(0x4000104) = timer1Reload << 1;
		*(u16 *)(0x4000106) = value;
	}
	  break;
  case 0x108:
    timer2Reload = value;
	UPDATE_REG(0x108, value);
	*(u16 *)(0x4000108) = value;
    break;
  case 0x10A:
    timer2Value = value;
    //timerOnOffDelay|=4;
    //cpuNextEvent = cpuTotalTicks;
	UPDATE_REG(0x10A, value);

	/*if(timer2Reload & 0x8000)
	{
		if((value & 0x3) == 0)
		{
			*(u16 *)(0x4000108) = timer2Reload >> 5;
			*(u16 *)(0x400010A) = value + 1;
			break;
		}
		if((value & 0x3) == 1)
		{
			*(u16 *)(0x4000108) = timer2Reload >> 1;
			*(u16 *)(0x400010A) = value + 1;
			break;
		}
		if((value & 3) == 2)
		{
			*(u16 *)(0x4000108) = timer2Reload >> 1;
			*(u16 *)(0x400010A) = value + 1;
			break;
		}
		iprintf("big reload2\r\n");//todo 
		*(u16 *)(0x400010A) = value;
	}
	else*/
	{	
		*(u16 *)(0x4000108) = timer2Reload << 1;
		*(u16 *)(0x400010A) = value;
	}
	  break;
  case 0x10C:
    timer3Reload = value;
	UPDATE_REG(0x10C, value);
	  break;
  case 0x10E:
    timer3Value = value;
    //timerOnOffDelay|=8;
    //cpuNextEvent = cpuTotalTicks;
	UPDATE_REG(0x10E, value);

	/*if(timer3Reload & 0x8000)
	{
		if((value & 0x3) == 0)
		{
			*(u16 *)(0x400010C) = timer3Reload >> 5;
			*(u16 *)(0x400010E) = value + 1;
			break;
		}
		if((value & 0x3) == 1)
		{
			*(u16 *)(0x400010C) = timer3Reload >> 1;
			*(u16 *)(0x400010E) = value + 1;
			break;
		}
		if((value & 3) == 2)
		{
			*(u16 *)(0x400010C) = timer3Reload >> 1;
			*(u16 *)(0x400010E) = value + 1;
			break;
		}
		iprintf("big reload3\r\n");//todo 
		*(u16 *)(0x400010E) = value;
	}
	else*/
	{	
		*(u16 *)(0x400010C) = timer3Reload << 1;
		*(u16 *)(0x400010E) = value;
	}
  break;
  case 0x128:
    if(value & 0x80) {
      value &= 0xff7f;
      if(value & 1 && (value & 0x4000)) {
        UPDATE_REG(0x12a, 0xFF);
        IF |= 0x80;
        UPDATE_REG(0x202, IF);
        value &= 0x7f7f;
      }
    }
    UPDATE_REG(0x128, value);
    break;
  case 0x130:
    //P1 |= (value & 0x3FF); //ichfly readonly
    //UPDATE_REG(0x130, P1);
    break;
  case 0x132:
    UPDATE_REG(0x132, value & 0xC3FF);
	*(u16 *)(0x4000132) = value;
    break;
  case 0x200:
    IE = value & 0x3FFF;
    UPDATE_REG(0x200, IE);
    /*if ((IME & 1) && (IF & IE) && armIrqEnable)
      cpuNextEvent = cpuTotalTicks;*/
#ifdef forceHBlankirqs
	REG_IE = IE | (REG_IE & 0xFFFF0000) | IRQ_HBLANK;
#else
	REG_IE = IE | (REG_IE & 0xFFFF0000);
#endif
	
	anytimejmpfilter = IE;
	
    break;
  case 0x202:
	//REG_IF = value; //ichfly update at read outdated
	//IF = REG_IF;
#ifdef gba_handel_IRQ_correct
	REG_IF = IF = value;
#else
    IF ^= (value & IF);
    UPDATE_REG(0x202, IF);
#endif
    break;
  case 0x204:
    { //ichfly can't emulate that
      /*memoryWait[0x0e] = memoryWaitSeq[0x0e] = gamepakRamWaitState[value & 3];
      
      if(!speedHack) {
        memoryWait[0x08] = memoryWait[0x09] = gamepakWaitState[(value >> 2) & 3];
        memoryWaitSeq[0x08] = memoryWaitSeq[0x09] =
          gamepakWaitState0[(value >> 4) & 1];
        
        memoryWait[0x0a] = memoryWait[0x0b] = gamepakWaitState[(value >> 5) & 3];
        memoryWaitSeq[0x0a] = memoryWaitSeq[0x0b] =
          gamepakWaitState1[(value >> 7) & 1];
        
        memoryWait[0x0c] = memoryWait[0x0d] = gamepakWaitState[(value >> 8) & 3];
        memoryWaitSeq[0x0c] = memoryWaitSeq[0x0d] =
          gamepakWaitState2[(value >> 10) & 1];
      } else {
        memoryWait[0x08] = memoryWait[0x09] = 3;
        memoryWaitSeq[0x08] = memoryWaitSeq[0x09] = 1;
        
        memoryWait[0x0a] = memoryWait[0x0b] = 3;
        memoryWaitSeq[0x0a] = memoryWaitSeq[0x0b] = 1;
        
        memoryWait[0x0c] = memoryWait[0x0d] = 3;
        memoryWaitSeq[0x0c] = memoryWaitSeq[0x0d] = 1;
      }
         
      for(int i = 8; i < 15; i++) {
        memoryWait32[i] = memoryWait[i] + memoryWaitSeq[i] + 1;
        memoryWaitSeq32[i] = memoryWaitSeq[i]*2 + 1;
      }

      if((value & 0x4000) == 0x4000) {
        busPrefetchEnable = true;
        busPrefetch = false;
        busPrefetchCount = 0;
      } else {
        busPrefetchEnable = false;
        busPrefetch = false;
        busPrefetchCount = 0;
      }*/
      UPDATE_REG(0x204, value & 0x7FFF);

    }
    break;
  case 0x208:
    IME = value & 1;
    UPDATE_REG(0x208, IME);
#ifdef gba_handel_IRQ_correct
	REG_IME = IME;
#endif
    /*if ((IME & 1) && (IF & IE) && armIrqEnable)
      cpuNextEvent = cpuTotalTicks;*/
    break;
  case 0x300:
    if(value != 0) //ichfly this is todo
      value &= 0xFFFE;
    UPDATE_REG(0x300, value);
    break;
  default:
    UPDATE_REG(address&0x3FE, value);
    break;
  }
}

u8 cpuBitsSet[256];
u8 cpuLowestBitSet[256];

void CPUInit(const char *biosFileName, bool useBiosFile,bool extram)
{
#ifdef WORDS_BIGENDIAN
  if(!cpuBiosSwapped) {
    for(unsigned int i = 0; i < sizeof(myROM)/4; i++) {
      WRITE32LE((u32*)&myROM[i], myROM[i]);
    }
    cpuBiosSwapped = true;
  }
#endif
  saveType = 0;
  eepromInUse = 0;
  useBios = false;
  
  if(useBiosFile) {
    int size = 0x4000;
    if(utilLoad(biosFileName,
                bios,
                size,extram)) {
      if(size == 0x4000)
        useBios = true;
      else
        systemMessage(MSG_INVALID_BIOS_FILE_SIZE, N_("Invalid BIOS file size"));
    }
  }
  
  if(!useBios) {
    memcpy(bios, myROM, sizeof(myROM));
  }

  int i = 0;

  biosProtected[0] = 0x00;
  biosProtected[1] = 0xf0;
  biosProtected[2] = 0x29;
  biosProtected[3] = 0xe1;

  for(i = 0; i < 256; i++) {
    int count = 0;
    int j;
    for(j = 0; j < 8; j++)
      if(i & (1 << j))
        count++;
    cpuBitsSet[i] = count;
    
    for(j = 0; j < 8; j++)
      if(i & (1 << j))
        break;
    cpuLowestBitSet[i] = j;
  }

  for(i = 0; i < 0x400; i++)
    ioReadable[i] = true;
  for(i = 0x10; i < 0x48; i++)
    ioReadable[i] = false;
  for(i = 0x4c; i < 0x50; i++)
    ioReadable[i] = false;
  for(i = 0x54; i < 0x60; i++)
    ioReadable[i] = false;
  for(i = 0x8c; i < 0x90; i++)
    ioReadable[i] = false;
  for(i = 0xa0; i < 0xb8; i++)
    ioReadable[i] = false;
  for(i = 0xbc; i < 0xc4; i++)
    ioReadable[i] = false;
  for(i = 0xc8; i < 0xd0; i++)
    ioReadable[i] = false;
  for(i = 0xd4; i < 0xdc; i++)
    ioReadable[i] = false;
  for(i = 0xe0; i < 0x100; i++)
    ioReadable[i] = false;
  for(i = 0x110; i < 0x120; i++)
    ioReadable[i] = false;
  for(i = 0x12c; i < 0x130; i++)
    ioReadable[i] = false;
  for(i = 0x138; i < 0x140; i++)
    ioReadable[i] = false;
  for(i = 0x144; i < 0x150; i++)
    ioReadable[i] = false;
  for(i = 0x15c; i < 0x200; i++)
    ioReadable[i] = false;
  for(i = 0x20c; i < 0x300; i++)
    ioReadable[i] = false;
  for(i = 0x304; i < 0x400; i++)
    ioReadable[i] = false;

	agbPrintEnable(false);
}

void CPUReset(){

  if(saveType == 0) {
    if(eepromInUse)
      saveType = 3;
    else
		switch(saveType) {
			case 1:
				saveType = 1;
			break;
			case 2:
				saveType = 2;
			break;
		}
  }
  rtcReset();

  // clean registers
  memset(&reg[0], 0, sizeof(reg));
  // clean OAM
  //memset(emultoroam, 0, 0x400);
  // clean palette
  //memset(paletteRAM, 0, 0x400);
  // clean picture
  //memset(pix, 0, 4*160*240);
  // clean vram
  //memset(vram, 0, 0x20000);
  // clean io memory
  memset(ioMem, 0, 0x400);





  //DISPCNT  = 0x0000;
  DISPSTAT = 0x0000;
  VCOUNT   = (useBios && !skipBios) ? 0 :0x007E;
  BG0CNT   = 0x0000;
  BG1CNT   = 0x0000;
  BG2CNT   = 0x0000;
  BG3CNT   = 0x0000;
  BG0HOFS  = 0x0000;
  BG0VOFS  = 0x0000;
  BG1HOFS  = 0x0000;
  BG1VOFS  = 0x0000;
  BG2HOFS  = 0x0000;
  BG2VOFS  = 0x0000;
  BG3HOFS  = 0x0000;
  BG3VOFS  = 0x0000;
  BG2PA    = 0x0100;
  BG2PB    = 0x0000;
  BG2PC    = 0x0000;
  BG2PD    = 0x0100;
  BG2X_L   = 0x0000;
  BG2X_H   = 0x0000;
  BG2Y_L   = 0x0000;
  BG2Y_H   = 0x0000;
  BG3PA    = 0x0100;
  BG3PB    = 0x0000;
  BG3PC    = 0x0000;
  BG3PD    = 0x0100;
  BG3X_L   = 0x0000;
  BG3X_H   = 0x0000;
  BG3Y_L   = 0x0000;
  BG3Y_H   = 0x0000;
  WIN0H    = 0x0000;
  WIN1H    = 0x0000;
  WIN0V    = 0x0000;
  WIN1V    = 0x0000;
  WININ    = 0x0000;
  WINOUT   = 0x0000;
  MOSAIC   = 0x0000;
  BLDMOD   = 0x0000;
  COLEV    = 0x0000;
  COLY     = 0x0000;
  DM0SAD_L = 0x0000;
  DM0SAD_H = 0x0000;
  DM0DAD_L = 0x0000;
  DM0DAD_H = 0x0000;
  DM0CNT_L = 0x0000;
  DM0CNT_H = 0x0000;
  DM1SAD_L = 0x0000;
  DM1SAD_H = 0x0000;
  DM1DAD_L = 0x0000;
  DM1DAD_H = 0x0000;
  DM1CNT_L = 0x0000;
  DM1CNT_H = 0x0000;
  DM2SAD_L = 0x0000;
  DM2SAD_H = 0x0000;
  DM2DAD_L = 0x0000;
  DM2DAD_H = 0x0000;
  DM2CNT_L = 0x0000;
  DM2CNT_H = 0x0000;
  DM3SAD_L = 0x0000;
  DM3SAD_H = 0x0000;
  DM3DAD_L = 0x0000;
  DM3DAD_H = 0x0000;
  DM3CNT_L = 0x0000;
  DM3CNT_H = 0x0000;
  TM0D     = 0x0000;
  TM0CNT   = 0x0000;
  TM1D     = 0x0000;
  TM1CNT   = 0x0000;
  TM2D     = 0x0000;
  TM2CNT   = 0x0000;
  TM3D     = 0x0000;
  TM3CNT   = 0x0000;
  P1       = 0x03FF;
  IE       = 0x0000;
  IF       = 0x0000;
  IME      = 0x0000;

  armMode = 0x1F;
  
  /*if(cpuIsMultiBoot) {
    reg[13].I = 0x03007F00;
    reg[15].I = 0x02000000;
    reg[16].I = 0x00000000;
    reg[R13_IRQ].I = 0x03007FA0;
    reg[R13_SVC].I = 0x03007FE0;
    armIrqEnable = true;
  } else {
    if(useBios && !skipBios) {
      reg[15].I = 0x00000000;
      armMode = 0x13;
      armIrqEnable = false;  
    } else {
      reg[13].I = 0x03007F00;
      reg[15].I = 0x08000000;
      reg[16].I = 0x00000000;
      reg[R13_IRQ].I = 0x03007FA0;
      reg[R13_SVC].I = 0x03007FE0;
      armIrqEnable = true;      
    }    
  }
  armState = true;
  C_FLAG = V_FLAG = N_FLAG = Z_FLAG = false;*/
  armState = true;
  UPDATE_REG(0x00, DISPCNT);
  UPDATE_REG(0x06, VCOUNT);
  UPDATE_REG(0x20, BG2PA);
  UPDATE_REG(0x26, BG2PD);
  UPDATE_REG(0x30, BG3PA);
  UPDATE_REG(0x36, BG3PD);
  UPDATE_REG(0x130, P1);
  UPDATE_REG(0x88, 0x200);

  // disable FIQ
  //reg[16].I |= 0x40;
  
  //CPUUpdateCPSR();
  
  //armNextPC = reg[15].I;
  //reg[15].I += 4;

  // reset internal state
  //holdState = false;
  //holdType = 0;
  
  biosProtected[0] = 0x00;
  biosProtected[1] = 0xf0;
  biosProtected[2] = 0x29;
  biosProtected[3] = 0xe1;
  
  lcdTicks = (useBios && !skipBios) ? 1008 : 208;
  timer0On = false;
  timer0Ticks = 0;
  timer0Reload = 0;
  timer0ClockReload  = 0;
  timer1On = false;
  timer1Ticks = 0;
  timer1Reload = 0;
  timer1ClockReload  = 0;
  timer2On = false;
  timer2Ticks = 0;
  timer2Reload = 0;
  timer2ClockReload  = 0;
  timer3On = false;
  timer3Ticks = 0;
  timer3Reload = 0;
  timer3ClockReload  = 0;
  dma0Source = 0;
  dma0Dest = 0;
  dma1Source = 0;
  dma1Dest = 0;
  dma2Source = 0;
  dma2Dest = 0;
  dma3Source = 0;
  dma3Dest = 0;
  cpuSaveGameFunc = flashSaveDecide;
  //renderLine = mode0RenderLine;
  fxOn = false;
  windowOn = false;
  frameCount = 0;
  saveType = 0;
  //layerEnable = DISPCNT & layerSettings;

  //CPUUpdateRenderBuffers(true);
  
  for(int i = 0; i < 256; i++) {
    map[i].address = (u8 *)&dummyAddress;
    map[i].mask = 0;
  }




  map[0].address = bios;
  map[0].mask = 0x3FFF;
  map[2].address = workRAM;
  map[2].mask = 0x3FFFF;
  map[3].address = internalRAM;
  map[3].mask = 0x7FFF;
  map[4].address = ioMem;
  map[4].mask = 0x3FF;
  map[5].address = paletteRAM;
  map[5].mask = 0x3FF;
  map[6].address = vram;
  map[6].mask = 0x1FFFF;
  map[7].address = emultoroam;
  map[7].mask = 0x3FF;
  map[8].address = rom;
  map[8].mask = 0x1FFFFFF;
  map[9].address = rom;
  map[9].mask = 0x1FFFFFF;  
  map[10].address = rom;
  map[10].mask = 0x1FFFFFF;
  map[12].address = rom;
  map[12].mask = 0x1FFFFFF;
  map[14].address = flashSaveMemory;
  map[14].mask = 0xFFFF;



  eepromReset();
  flashReset();
  
  //soundReset(); //ichfly sound

  //CPUUpdateWindow0();
  //CPUUpdateWindow1();

  // make sure registers are correctly initialized if not using BIOS
  /*if(!useBios) {
    if(cpuIsMultiBoot)
      BIOS_RegisterRamReset(0xfe);
    else
      BIOS_RegisterRamReset(0xff);
  } else {
    if(cpuIsMultiBoot)
      BIOS_RegisterRamReset(0xfe);
  }*/ //ararar

  switch(cpuSaveType) {
  case 0: // automatic
    cpuSramEnabled = true;
    cpuFlashEnabled = true;
    cpuEEPROMEnabled = true;
    cpuEEPROMSensorEnabled = false;
    saveType = 0;
    break;
  case 1: // EEPROM
    cpuSramEnabled = false;
    cpuFlashEnabled = false;
    cpuEEPROMEnabled = true;
    cpuEEPROMSensorEnabled = false;
    saveType = 3;
    // EEPROM usage is automatically detected
    break;
  case 2: // SRAM
    cpuSramEnabled = true;
    cpuFlashEnabled = false;
    cpuEEPROMEnabled = false;
    cpuEEPROMSensorEnabled = false;
    cpuSaveGameFunc = sramDelayedWrite; // to insure we detect the write
    saveType = 1;
    break;
  case 3: // FLASH
    cpuSramEnabled = false;
    cpuFlashEnabled = true;
    cpuEEPROMEnabled = false;
    cpuEEPROMSensorEnabled = false;
    cpuSaveGameFunc = flashDelayedWrite; // to insure we detect the write
    saveType = 2;
    break;
  case 4: // EEPROM+Sensor
    cpuSramEnabled = false;
    cpuFlashEnabled = false;
    cpuEEPROMEnabled = true;
    cpuEEPROMSensorEnabled = true;
    // EEPROM usage is automatically detected
    saveType = 3;
    break;
  case 5: // NONE
    cpuSramEnabled = false;
    cpuFlashEnabled = false;
    cpuEEPROMEnabled = false;
    cpuEEPROMSensorEnabled = false;
    // no save at all
    saveType = 5;
    break;
  } 

  //ARM_PREFETCH;
  systemSaveUpdateCounter = SYSTEM_SAVE_NOT_UPDATED;
  cpuDmaHack = false;
  //lastTime = systemGetClock();
  //SWITicks = 0;
  rtcEnable(true); //Coto: GBA RTC Support
}

#ifdef SDL
void Log(const char *defaultMsg, ...)
{
  char buffer[2048];
  va_list valist;
  
  va_start(valist, defaultMsg);
  vsprintf(buffer, defaultMsg, valist);

  if(out == NULL) {
    out = fopen("trace.Log","w");
  }

  fputs(buffer, out);
  
  va_end(valist);
}
#else
extern void winLog(const char *, ...);
#endif



//CPU Read/Writes

u32 CPUReadMemorypu(u32 address)
{
	return CPUReadMemoryrealpu(address);
}

u16 CPUReadHalfWordpu(u32 address)
{
	return CPUReadHalfWordrealpu(address);
}

u8 CPUReadBytepu(u32 address)
{
	return CPUReadByterealpu(address);
}

u32 CPUReadMemory(u32 address)
{
	return CPUReadMemoryreal(address);
}

s16 CPUReadHalfWordSignedoutline(u32 address)
{
	return (s16)CPUReadHalfWordSigned(address);
}

s8 CPUReadByteSigned(u32 address)
{
	return (s8)CPUReadBytereal(address);
}

u16 CPUReadHalfWord(u32 address)
{
	return CPUReadHalfWordreal(address);
}

u8 CPUReadByte(u32 address)
{
	return CPUReadBytereal(address);
}

void CPUWriteMemoryextern(u32 address, u32 value)
{
	CPUWriteMemory(address,value);
}

void CPUWriteHalfWordextern(u32 address, u16 value)
{
	CPUWriteHalfWord(address,value);
}

void CPUWriteByteextern(u32 address, u8 b)
{
	CPUWriteByte(address,b);
}

void CPUWriteMemorypuextern(u32 address, u32 value)
{
	CPUWriteMemorypu(address,value);
}

void CPUWriteHalfWordpuextern(u32 address, u16 value)
{
	CPUWriteHalfWordpu(address,value);
}

void CPUWriteBytepuextern(u32 address, u8 b)
{
	CPUWriteBytepu(address,b);
}

s16 CPUReadHalfWordrealpuSignedoutline(u32 address)
{
	return (s16)CPUReadHalfWordrealpuSigned(address);
}

s8 CPUReadByteSignedpu(u32 address)
{
	return (s8)CPUReadByterealpu(address);
}



//HBLANK IRQ Disabled emulation
__attribute__((section(".itcm")))
void vcounthandler(void){		
	DISPSTAT |= (REG_DISPSTAT & 0x3);
	VCOUNT = REG_VCOUNT&0xff;
	
	if(VCOUNT == (DISPSTAT >> 8)) //update by V-Count Setting
	{
		DISPSTAT |= 0x4;
	}
	else{
		DISPSTAT &= 0xFFFb; //remove vcount
	}
	//io update
	UPDATE_REG(0x06, VCOUNT);
	UPDATE_REG(0x04, DISPSTAT);
}

//normal cpu read/write opcodes
__attribute__((section(".itcm")))
void updateVC()
{
		u32 temp = REG_VCOUNT;
		u32 temp2 = REG_DISPSTAT;
		//iprintf("Vcountreal: %08x\n",temp);
		//u16 help3;
#ifdef usebuffedVcout
		VCOUNT = VCountdstogba[temp];
#else
		if(temp < 192)
		{
			VCOUNT = ((temp * 214) >> 8);//VCOUNT = help * (1./1.2); //1.15350877;
			//help3 = (help + 1) * (1./1.2); //1.15350877;  // ichfly todo it is to slow
		}
		else
		{
			VCOUNT = (((temp - 192) * 246) >>  8)+ 160;//VCOUNT = ((temp - 192) * (1./ 1.04411764)) + 160 //* (1./ 1.04411764)) + 160; //1.15350877;
			//help3 = ((help - 191) *  (1./ 1.04411764)) + 160; //1.15350877;  // ichfly todo it is to slow			
		}
#endif
		DISPSTAT &= 0xFFF8; //reset h-blanc and V-Blanc and V-Count Setting
		//if(help3 == VCOUNT) //else it is a extra long V-Line // ichfly todo it is to slow
		//{
			DISPSTAT |= (temp2 & 0x3); //temporary patch get original settings
		//}
		//if(VCOUNT > 160 && VCOUNT != 227)DISPSTAT |= 1;//V-Blanc
		UPDATE_REG(0x06, VCOUNT);
		if(VCOUNT == (DISPSTAT >> 8)) //update by V-Count Setting
		{
			DISPSTAT |= 0x4;
			/*if(DISPSTAT & 0x20) {
			  IF |= 4;
			  UPDATE_REG(0x202, IF);
			}*/
		}
		UPDATE_REG(0x04, DISPSTAT);
		//iprintf("Vcountreal: %08x\n",temp);
		//iprintf("DISPSTAT: %08x\n",temp2);
}

__attribute__((section(".itcm")))
u32 CPUReadMemoryreal(u32 address) //ichfly not inline is faster because it is smaler
{
#ifdef DEV_VERSION
  if(address & 3) {  
    if(systemVerbose & VERBOSE_UNALIGNED_MEMORY) {
      iprintf("Unaligned word read: %08x at %08x\n", address, armMode ?
          armNextPC - 4 : armNextPC - 2);
    }
  }
#endif
  
#ifdef printreads
  iprintf("r32 %08x\n",address);
#endif
  
  u32 value;
  switch(address >> 24) {
  case 0:
    if(reg[15].I >> 24) {
      if(address < 0x4000) {
#ifdef DEV_VERSION
        if(systemVerbose & VERBOSE_ILLEGAL_READ) {
          iprintf("Illegal word read: %08x at %08x\n", address, armMode ?
              armNextPC - 4 : armNextPC - 2);
        }
#endif
        
        value = READ32LE(((u32*)&biosProtected));
      }
      else goto unreadable;
    } else
      value = READ32LE(((u32*)&bios[address & 0x3FFC]));
    break;
  case 2:
#ifdef checkclearaddrrw
	if(address >0x023FFFFF)goto unreadable;
#endif
    value = READ32LE(((u32*)&workRAM[address & 0x3FFFC]));
    break;
  case 3:
#ifdef checkclearaddrrw
	if(address > 0x03008000 && !(address > 0x03FF8000)/*upern mirrow*/)goto unreadable;
#endif
    value = READ32LE(((u32*)&internalRAM[address & 0x7ffC]));
    break;
  case 4:
  
	if(address > 0x40000FF && address < 0x4000111)
	{
		//todo timer shift
		value = *(u32 *)(address);
		break;
	}
#ifdef gba_handel_IRQ_correct
	if(address == 0x4000202 || address == 0x4000200)//ichfly update
	{
		IF = *(vuint16*)0x04000214; //VBlanc
		UPDATE_REG(0x202, IF);
	}
#endif	


	if(address > 0x4000003 && address < 0x4000008)//ichfly update
	{
		if(disableHBLANKIRQ == true){
			vcounthandler();
		}
		else{
			updateVC();
		}
	}
    if((address < 0x4000400) && ioReadable[address & 0x3fc]) {
      if(ioReadable[(address & 0x3fc) + 2])
        value = READ32LE(((u32*)&ioMem[address & 0x3fC]));
      else
        value = READ16LE(((u16*)&ioMem[address & 0x3fc]));
    } else goto unreadable;
    break;
  case 5:
#ifdef checkclearaddrrw
	if(address > 0x05000400)goto unreadable;
#endif
    value = READ32LE(((u32*)&paletteRAM[address & 0x3fC]));
    break;
  case 6:
#ifdef checkclearaddrrw
	if(address > 0x06020000)goto unreadable;
#endif
    address = (address & 0x1fffc);
    if (((DISPCNT & 7) >2) && ((address & 0x1C000) == 0x18000))
    {
        value = 0;
        break;
    }
    if ((address & 0x18000) == 0x18000)
      address &= 0x17fff;
    value = READ32LE(((u32*)&vram[address]));
    break;
  case 7:
#ifdef checkclearaddrrw
	if(address > 0x07000400)goto unreadable;
#endif
    value = READ32LE(((u32*)&emultoroam[address & 0x3FC]));
    break;
  case 8:
  case 9:
  case 10:
  case 11:
  case 12:
#ifdef uppern_read_emulation
	if( (int)(address&0x1FFFFFC) > romSize)
	{
#ifdef print_uppern_read_emulation
		iprintf("high r32 %08x\n",address);
#endif
		if(ichflyfilestreamsize > (int)(address&0x1FFFFFC))
		{
			//fseek(ichflyfilestream , address&0x1FFFFFC , SEEK_SET);
			//fread(&value,1,4,ichflyfilestream);
			//ichfly_readfrom(ichflyfilestream,(address&0x1FFFFFC),(char*)&value,4);
			value = ichfly_readu32(address&0x1FFFFFC);
		}
		else
		{
			value = 0;
		}
	}
	else
	{
		value = READ32LE(((u32*)&rom[address&0x1FFFFFC]));
	}
#else
    value = READ32LE(((u32*)&rom[address&0x1FFFFFC]));
#endif
    break;
  case 13:
    if(cpuEEPROMEnabled)
      // no need to swap this
      return eepromRead(address);
    goto unreadable;
  case 14:
    if(cpuFlashEnabled | cpuSramEnabled)
      // no need to swap this
      return flashRead(address);
    // default
  default:
  unreadable:
  //while(1);
#ifdef checkclearaddrrw
      //iprintf("Illegal word read: %08x at %08x\n", address,reg[15].I);
	  iprintf("Illegal word read: %08x\n", address);
	  REG_IME = IME_DISABLE;
	  debugDump();
	  while(1);
#endif

    //if(cpuDmaHack) { //only this is possible here
      value = cpuDmaLast;
    /*} else {
      if(armState) {
        value = CPUReadMemoryQuick(reg[15].I);
      } else {
        value = CPUReadHalfWordQuick(reg[15].I) |
          CPUReadHalfWordQuick(reg[15].I) << 16;
      }
    }*/
  }

  if(address & 3) {
//#ifdef C_CORE
    int shift = (address & 3) << 3;
    value = (value >> shift) | (value << (32 - shift));
/*#else    
#ifdef __GNUC__ ichfly
    asm("and $3, %%ecx;"
        "shl $3 ,%%ecx;"
        "ror %%cl, %0"
        : "=r" (value)
        : "r" (value), "c" (address));
#else
    asm(
      mov ecx, address;
      and ecx, 3;
      shl ecx, 3;
      ror [dword ptr value], cl;
    )
//#endif
#endif*/
  }
  return value;
}

__attribute__((section(".itcm")))
u16 CPUReadHalfWordreal(u32 address) //ichfly not inline is faster because it is smaler
{
#ifdef printreads
	iprintf("r16 %08x\n",address);
#endif
#ifdef DEV_VERSION      
  if(address & 1) {
    if(systemVerbose & VERBOSE_UNALIGNED_MEMORY) {
      iprintf("Unaligned halfword read: %08x at %08x\n", address, armMode ?
          armNextPC - 4 : armNextPC - 2);
    }
  }
#endif
  
  u32 value;
  
  switch(address >> 24) {
  case 0:
    if (reg[15].I >> 24) {
      if(address < 0x4000) {
#ifdef DEV_VERSION
        if(systemVerbose & VERBOSE_ILLEGAL_READ) {
          iprintf("Illegal halfword read: %08x at %08x\n", address, armMode ?
              armNextPC - 4 : armNextPC - 2);
        }
#endif
        value = READ16LE(((u16*)&biosProtected[address&2]));
      } else goto unreadable;
    } else
      value = READ16LE(((u16*)&bios[address & 0x3FFE]));
    break;
  case 2:
#ifdef checkclearaddrrw
	if(address >0x023FFFFF)goto unreadable;
#endif
    value = READ16LE(((u16*)&workRAM[address & 0x3FFFE]));
    break;
  case 3:
#ifdef checkclearaddrrw
	if(address > 0x03008000 && !(address > 0x03FF8000)/*upern mirrow*/)goto unreadable;
#endif
    value = READ16LE(((u16*)&internalRAM[address & 0x7ffe]));
    break;
  case 4:
  
	if(address > 0x40000FF && address < 0x4000111)
	{
		
		if((address&0x2) == 0)
		{
			if(ioMem[address & 0x3fe] & 0x8000)
			{
				value = ((*(u16 *)(address)) >> 1) | 0x8000;
			}
			else
			{
				value = (*(u16 *)(address)) >> 1;
			}
			return value;
		}
	}
  
	if(address > 0x4000003 && address < 0x4000008)//ichfly update
	{
		if(disableHBLANKIRQ == true){
			vcounthandler();
		}
		else{
			updateVC();
		}
	}
	
#ifdef gba_handel_IRQ_correct
	if(address == 0x4000202)//ichfly update
	{
		IF = *(vuint16*)0x04000214;
		UPDATE_REG(0x202, IF);
	}
#endif
	
    if((address < 0x4000400) && ioReadable[address & 0x3fe])
    {
		value =  READ16LE(((u16*)&ioMem[address & 0x3fe]));
    }
    else goto unreadable;
    break;
  case 5:
#ifdef checkclearaddrrw
	if(address > 0x05000400)goto unreadable;
#endif
    value = READ16LE(((u16*)&paletteRAM[address & 0x3fe]));
    break;
  case 6:
#ifdef checkclearaddrrw
	if(address > 0x06020000)goto unreadable;
#endif
    address = (address & 0x1fffe);
    if (((DISPCNT & 7) >2) && ((address & 0x1C000) == 0x18000))
    {
        value = 0;
        break;
    }
    if ((address & 0x18000) == 0x18000)
      address &= 0x17fff;
    value = READ16LE(((u16*)&vram[address]));
    break;
  case 7:
#ifdef checkclearaddrrw
	if(address > 0x07000400)goto unreadable;
#endif
    value = READ16LE(((u16*)&emultoroam[address & 0x3fe]));
    break;
  case 8:
  case 9:
  case 10:
  case 11:
  case 12:
    if(address == 0x80000c4 || address == 0x80000c6 || address == 0x80000c8)
      value = rtcRead(address);
    else
	{
#ifdef uppern_read_emulation
	if( (int)(address&0x1FFFFFE) > romSize)
	{
#ifdef print_uppern_read_emulation
		iprintf("high r16 %08x\n",address);
#endif
		if(ichflyfilestreamsize > (int)(address&0x1FFFFFE))
		{
			//fseek (ichflyfilestream , address&0x1FFFFFE , SEEK_SET);
			//fread (&value,1,2,ichflyfilestream);
			//ichfly_readfrom(ichflyfilestream,(address&0x1FFFFFE),(char*)&value,2);
			value = ichfly_readu16(address&0x1FFFFFE);
		}
		else
		{
			value = 0;
		}
	}
	else
	{
		value = READ16LE(((u16*)&rom[address & 0x1FFFFFE]));
	}
#else
    value = READ16LE(((u16*)&rom[address & 0x1FFFFFE]));
#endif
	}
    break;    
  case 13:
#ifdef printsaveread
	  iprintf("%X\n\r",address);
#endif
    if(cpuEEPROMEnabled)
      // no need to swap this
      return  eepromRead(address);
    goto unreadable;
  case 14:
#ifdef printsaveread
	  iprintf("%X\n\r",address);
#endif
    if(cpuFlashEnabled | cpuSramEnabled)
      // no need to swap this
      return flashRead(address);
    // default
  default:
  unreadable:
#ifdef checkclearaddrrw
      //iprintf("Illegal word read: %08x at %08x\n", address,reg[15].I);
	  iprintf("Illegal hword read: %08x\n", address);
	  REG_IME = IME_DISABLE;
	  debugDump();
	  while(1);
#endif

    //if(cpuDmaHack) { //only this is possible here
      value = cpuDmaLast & 0xFFFF;
    /*} else {
      if(armState) {
        value = CPUReadHalfWordQuick(reg[15].I + (address & 2));
      } else {
        value = CPUReadHalfWordQuick(reg[15].I);
      }
    }*/
    break;
  }

  if(address & 1) {
    value = (value >> 8) | (value << 24);
  }
  
  return value;
}

__attribute__((section(".itcm")))
s16 CPUReadHalfWordSigned(u32 address)
{
  u16 value = CPUReadHalfWordreal(address);
  if((address & 1))
    value = (s8)value;
  return value;
}

__attribute__((section(".itcm")))
u8 CPUReadBytereal(u32 address) //ichfly not inline is faster because it is smaler
{
#ifdef printreads
iprintf("r8 %02x\n",address);
#endif

  switch(address >> 24) {
  case 0:
    if (reg[15].I >> 24) {
      if(address < 0x4000) {
#ifdef DEV_VERSION
        if(systemVerbose & VERBOSE_ILLEGAL_READ) {
          iprintf("Illegal byte read: %08x at %08x\n", address, armMode ?
              armNextPC - 4 : armNextPC - 2);
        }
#endif
        return biosProtected[address & 3];
      } else goto unreadable;
    }
    return bios[address & 0x3FFF];
  case 2:
#ifdef checkclearaddrrw
	if(address >0x023FFFFF)goto unreadable;
#endif
    return workRAM[address & 0x3FFFF];
  case 3:
#ifdef checkclearaddrrw
	if(address > 0x03008000 && !(address > 0x03FF8000)/*upern mirrow*/)goto unreadable;
#endif
    return internalRAM[address & 0x7fff];
  case 4:
  
	if(address > 0x40000FF && address < 0x4000111)
	{
		//todo timer shift
		return *(u8 *)(address);
	}
  
  	if(address > 0x4000003 && address < 0x4000008)//ichfly update
	{
		if(disableHBLANKIRQ == true){
			vcounthandler();
		}
		else{
			updateVC();
		}
	}
#ifdef gba_handel_IRQ_correct
	if(address == 0x4000202 || address == 0x4000203)//ichfly update
	{
		IF = *(vuint16*)0x04000214; //VBlanc
		UPDATE_REG(0x202, IF);
	}
#endif
    if((address < 0x4000400) && ioReadable[address & 0x3ff])
      return ioMem[address & 0x3ff];
    else goto unreadable;
  case 5:
#ifdef checkclearaddrrw
	if(address > 0x05000400)goto unreadable;
#endif
    return paletteRAM[address & 0x3ff];
  case 6:
#ifdef checkclearaddrrw
	if(address > 0x06020000)goto unreadable;
#endif
    address = (address & 0x1ffff);
    if (((DISPCNT & 7) >2) && ((address & 0x1C000) == 0x18000))
        return 0;
    if ((address & 0x18000) == 0x18000)
      address &= 0x17fff;
    return vram[address];
  case 7:
#ifdef checkclearaddrrw
	if(address > 0x07000400)goto unreadable;
#endif
    return emultoroam[address & 0x3ff];
  case 8:
  case 9:
  case 10:
  case 11:
  case 12:

#ifdef uppern_read_emulation
	if( (int)(address&0x1FFFFFF) > romSize)
	{
#ifdef print_uppern_read_emulation
		iprintf("high r8 %08x\n",address);
#endif
		if(ichflyfilestreamsize > (int)(address&0x1FFFFFF))
		{
			//u8 temp = 0;
			//fseek (ichflyfilestream , address&0x1FFFFFF , SEEK_SET);
			//fread (&temp,1,1,ichflyfilestream);
			//ichfly_readfrom(ichflyfilestream,(address&0x1FFFFFF),(char*)&temp,1);
			return ichfly_readu8(address&0x1FFFFFF);
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return rom[address & 0x1FFFFFF];
	}
#else
    return rom[address & 0x1FFFFFF];
#endif        
  case 13:
#ifdef printsaveread
	  iprintf("%X\n\r",address);
#endif
    if(cpuEEPROMEnabled)
      return eepromRead(address);
    goto unreadable;
  case 14:
#ifdef printsaveread
	  iprintf("%X\n\r",address);
#endif
    if(cpuSramEnabled | cpuFlashEnabled)
      return flashRead(address);
    if(cpuEEPROMSensorEnabled) {
      switch(address & 0x00008f00) {
      case 0x8200:
        return systemGetSensorX() & 255;
      case 0x8300:
        return (systemGetSensorX() >> 8)|0x80;
      case 0x8400:
        return systemGetSensorY() & 255;
      case 0x8500:
        return systemGetSensorY() >> 8;
      }
    }
    // default
  default:
  unreadable:
#ifdef checkclearaddrrw
      //iprintf("Illegal word read: %08x at %08x\n", address,reg[15].I);
	  iprintf("Illegal byte read: %08x\n", address);
	  REG_IME = IME_DISABLE;
	  debugDump();
	  while(1);
#endif

    //if(cpuDmaHack) { //only this is possible here
      return cpuDmaLast & 0xFF;
    /*} else {
      if(armState) {
        return CPUReadByteQuick(reg[15].I+(address & 3));
      } else {
        return CPUReadByteQuick(reg[15].I+(address & 1));
      }
    }*/
    break;
  }
}

__attribute__((section(".itcm")))
void CPUWriteMemory(u32 address, u32 value) //ichfly not inline is faster because it is smaler
{
#ifdef printreads
    iprintf("w32 %08x to %08x\n",value,address);
#endif		  
		

#ifdef DEV_VERSION
  if(address & 3) {
    if(systemVerbose & VERBOSE_UNALIGNED_MEMORY) {
      iprintf("Unaligned word write: %08x to %08x from %08x\n",
          value,
          address,
          armMode ? armNextPC - 4 : armNextPC - 2);
    }
  }
#endif
  
  switch(address >> 24) {
  case 0x02:
#ifdef BKPT_SUPPORT
    if(*((u32 *)&freezeWorkRAM[address & 0x3FFFC]))
      cheatsWriteMemory(address & 0x203FFFC,
                        value);
    else
#endif
#ifdef checkclearaddrrw
	if(address >0x023FFFFF)goto unreadable;
#endif
      WRITE32LE(((u32*)&workRAM[address & 0x3FFFC]), value);
    break;
  case 0x03:
#ifdef BKPT_SUPPORT
    if(*((u32 *)&freezeInternalRAM[address & 0x7ffc]))
      cheatsWriteMemory(address & 0x3007FFC,
                        value);
    else
#endif
#ifdef checkclearaddrrw
	if(address > 0x03008000 && !(address > 0x03FF8000)/*upern mirrow*/)goto unreadable;
#endif
      WRITE32LE(((u32*)&internalRAM[address & 0x7ffC]), value);
    break;
  case 0x04:
    if(address < 0x4000400) {

	/*if((0x4000060 > address && address > 0x4000007) || (address > 0x40000FF && address < 0x4000110)) //timer and lcd
	{
			//iprintf("32 %x %x\r\n",address,value);
		    *(u32 *)(address) = value;
	}
	else //dont do twice*/ //don't need that any more
	{
      CPUUpdateRegister((address & 0x3FC), value & 0xFFFF);
      CPUUpdateRegister((address & 0x3FC) + 2, (value >> 16));
    }
	} else goto unwritable;
    break;
  case 0x05:
#ifdef BKPT_SUPPORT
    if(*((u32 *)&freezePRAM[address & 0x3fc]))
      cheatsWriteMemory(address & 0x70003FC,
                        value);
    else
#endif
#ifdef checkclearaddrrw
	if(address > 0x05000400)goto unreadable;
#endif
    WRITE32LE(((u32*)&paletteRAM[address & 0x3FC]), value);
    break;
  case 0x06:
#ifdef checkclearaddrrw
	if(address > 0x06020000)goto unreadable;
#endif
    address = (address & 0x1fffc);
    if (((DISPCNT & 7) >2) && ((address & 0x1C000) == 0x18000))
        return;
    if ((address & 0x18000) == 0x18000)
      address &= 0x17fff;

#ifdef BKPT_SUPPORT
    if(*((u32 *)&freezeVRAM[address]))
      cheatsWriteMemory(address + 0x06000000, value);
    else
#endif
    
    WRITE32LE(((u32*)&vram[address]), value);
    break;
  case 0x07:
#ifdef checkclearaddrrw
	if(address > 0x07000400)goto unreadable;
#endif
#ifdef BKPT_SUPPORT
    if(*((u32 *)&freezeOAM[address & 0x3fc]))
      cheatsWriteMemory(address & 0x70003FC,
                        value);
    else
#endif
    WRITE32LE(((u32*)&emultoroam[address & 0x3fc]), value);
    break;
  case 0x0D:
#ifdef printsavewrite
	  	  iprintf("%X %X\n\r",address,value);
#endif
    if(cpuEEPROMEnabled) {
      eepromWrite(address, value);
      break;
    }
    goto unwritable;
  case 0x0E:
#ifdef printsavewrite
	  	  iprintf("%X %X\n\r",address,value);
#endif
    if(!eepromInUse | cpuSramEnabled | cpuFlashEnabled) {
      (*cpuSaveGameFunc)(address, (u8)value);
      break;
    }
    // default
  default:
  unwritable:
#ifdef checkclearaddrrw
      //iprintf("Illegal word read: %08x at %08x\n", address,reg[15].I);
	  iprintf("Illegal word write: %08x to %08x\n",value, address);
	  REG_IME = IME_DISABLE;
	  debugDump();
	  while(1);
#endif
    break;
  }
}

__attribute__((section(".itcm")))
void CPUWriteHalfWord(u32 address, u16 value)
{
#ifdef printreads
iprintf("w16 %04x to %08x\r\n",value,address);
#endif

#ifdef DEV_VERSION
  if(address & 1) {
    if(systemVerbose & VERBOSE_UNALIGNED_MEMORY) {
      iprintf("Unaligned halfword write: %04x to %08x from %08x\n",
          value,
          address,
          armMode ? armNextPC - 4 : armNextPC - 2);
    }
  }
#endif
  
  switch(address >> 24) {
  case 2:
#ifdef BKPT_SUPPORT
    if(*((u16 *)&freezeWorkRAM[address & 0x3FFFE]))
      cheatsWriteHalfWord(address & 0x203FFFE,
                          value);
    else
#endif
#ifdef checkclearaddrrw
	if(address >0x023FFFFF)goto unwritable;
#endif
      WRITE16LE(((u16*)&workRAM[address & 0x3FFFE]),value);
    break;
  case 3:
#ifdef BKPT_SUPPORT
    if(*((u16 *)&freezeInternalRAM[address & 0x7ffe]))
      cheatsWriteHalfWord(address & 0x3007ffe,
                          value);
    else
#endif
#ifdef checkclearaddrrw
	if(address > 0x03008000 && !(address > 0x03FF8000)/*upern mirrow*/)goto unwritable;
#endif
      WRITE16LE(((u16*)&internalRAM[address & 0x7ffe]), value);
    break;    
  case 4:
  
	/*if(address > 0x40000FF && address < 0x4000110)
	{
		*(u16 *)(address) = value;
		break;
	}*/ //don't need that
  
  	/*if(0x4000060 > address && address > 0x4000008)
	{
			iprintf("16 %x %x\r\n",address,value);
		    *(u16 *)((address & 0x3FF) + 0x4000000) = value;
	}*/ //dont do dobble
    if(address < 0x4000400)
      CPUUpdateRegister(address & 0x3fe, value);
    else goto unwritable;
    break;
  case 5:
#ifdef BKPT_SUPPORT
    if(*((u16 *)&freezePRAM[address & 0x03fe]))
      cheatsWriteHalfWord(address & 0x70003fe,
                          value);
    else
#endif
#ifdef checkclearaddrrw
	if(address > 0x05000400)goto unwritable;
#endif
    WRITE16LE(((u16*)&paletteRAM[address & 0x3fe]), value);
    break;
  case 6:
#ifdef checkclearaddrrw
	if(address > 0x06020000)goto unwritable;
#endif
    address = (address & 0x1fffe);
    if (((DISPCNT & 7) >2) && ((address & 0x1C000) == 0x18000))
        return;
    if ((address & 0x18000) == 0x18000)
      address &= 0x17fff;
#ifdef BKPT_SUPPORT
    if(*((u16 *)&freezeVRAM[address]))
      cheatsWriteHalfWord(address + 0x06000000,
                          value);
    else
#endif
    WRITE16LE(((u16*)&vram[address]), value); 
    break;
  case 7:
#ifdef BKPT_SUPPORT
    if(*((u16 *)&freezeOAM[address & 0x03fe]))
      cheatsWriteHalfWord(address & 0x70003fe,
                          value);
    else
#endif
#ifdef checkclearaddrrw
	if(address > 0x07000400)goto unwritable;
#endif
    WRITE16LE(((u16*)&emultoroam[address & 0x3fe]), value);
    break;
  case 8:
  case 9:
    if(address == 0x80000c4 || address == 0x80000c6 || address == 0x80000c8) {
      if(!rtcWrite(address, value))
        goto unwritable;
    } else if(!agbPrintWrite(address, value)) goto unwritable;
    break;
  case 13:
#ifdef printsavewrite
	  	  iprintf("%X %X\n\r",address,value);
#endif
    if(cpuEEPROMEnabled) {
      eepromWrite(address, (u8)value);
      break;
    }
    goto unwritable;
  case 14:
#ifdef printsavewrite
	  	  iprintf("%X %X\n\r",address,value);
#endif
    if(!eepromInUse | cpuSramEnabled | cpuFlashEnabled) {
      (*cpuSaveGameFunc)(address, (u8)value);
      break;
    }
    goto unwritable;
  default:
  unwritable:
#ifdef checkclearaddrrw
      //iprintf("Illegal word read: %08x at %08x\n", address,reg[15].I);
	  iprintf("Illegal hword write: %04x to %08x\n",value, address);
	  REG_IME = IME_DISABLE;
	  debugDump();
	  while(1);
#endif
    break;
  }
}

__attribute__((section(".itcm")))
void CPUWriteByte(u32 address, u8 b)
{
#ifdef printreads
	iprintf("w8 %02x to %08x\r\n",b,address);
#endif
  switch(address >> 24) {
  case 2:
#ifdef BKPT_SUPPORT
      if(freezeWorkRAM[address & 0x3FFFF])
        cheatsWriteByte(address & 0x203FFFF, b);
      else
#endif
#ifdef checkclearaddrrw
	if(address >0x023FFFFF)goto unwritable;
#endif
        workRAM[address & 0x3FFFF] = b;
    break;
  case 3:
#ifdef BKPT_SUPPORT
    if(freezeInternalRAM[address & 0x7fff])
      cheatsWriteByte(address & 0x3007fff, b);
    else
#endif
#ifdef checkclearaddrrw
	if(address > 0x03008000 && !(address > 0x03FF8000)/*upern mirrow*/)goto unwritable;
#endif
      internalRAM[address & 0x7fff] = b;
    break;
  case 4:
  
    if(address < 0x4000400) {
      switch(address & 0x3FF) {
      case 0x301:
	/*if(b == 0x80) //todo
	  stopState = true;
	holdState = 1;
	holdType = -1;
  cpuNextEvent = cpuTotalTicks;
	break;*/
      case 0x60:
      case 0x61:
      case 0x62:
      case 0x63:
      case 0x64:
      case 0x65:
      case 0x68:
      case 0x69:
      case 0x6c:
      case 0x6d:
      case 0x70:
      case 0x71:
      case 0x72:
      case 0x73:
      case 0x74:
      case 0x75:
      case 0x78:
      case 0x79:
      case 0x7c:
      case 0x7d:
      case 0x80:
      case 0x81:
      case 0x84:
      case 0x85:
      case 0x90:
      case 0x91:
      case 0x92:
      case 0x93:
      case 0x94:
      case 0x95:
      case 0x96:
      case 0x97:
      case 0x98:
      case 0x99:
      case 0x9a:
      case 0x9b:
      case 0x9c:
      case 0x9d:
      case 0x9e:
      case 0x9f:      
	//soundEvent(address&0xFF, b);  //ichfly disable sound
#ifdef printsoundwrites
		  iprintf("b %02x to %08x\r\n",b,address);
#endif
	  #ifdef arm9advsound
			SendArm7Command((u32)((address & 0x3FF) | 0x40000000), (u32)b);	//faster in case we send a 0
		#endif
	break;
      default:
	/*if((0x4000060 > address && address > 0x4000008) || (address > 0x40000FF && address < 0x4000110))
	{
			//iprintf("8 %x %x\r\n",address,b);
		    *(u8 *)(address) = b;
	}*/ //ichfly don't need that
	if(address & 1)
	{
	  CPUUpdateRegister(address & 0x3fe,
			    ((READ16LE(((u16*)&ioMem[address & 0x3fe])))
			     & 0x00FF) |
			    b<<8);

	}
	else
	  CPUUpdateRegister(address & 0x3fe,
			    ((READ16LE(((u16*)&ioMem[address & 0x3fe])) & 0xFF00) | b));
      }
      break;
    } else goto unwritable;
    break;
  case 5:
#ifdef checkclearaddrrw
	if(address > 0x05000400)goto unwritable;
#endif
    // no need to switch
    *((u16 *)&paletteRAM[address & 0x3FE]) = (b << 8) | b;
    break;
  case 6:
#ifdef checkclearaddrrw
	if(address > 0x06020000)goto unwritable;
#endif
    address = (address & 0x1fffe);
    if (((DISPCNT & 7) >2) && ((address & 0x1C000) == 0x18000))
        return;
    if ((address & 0x18000) == 0x18000)
      address &= 0x17fff;

    // no need to switch 
    // byte writes to OBJ VRAM are ignored
    if ((address) < objTilesAddress[((DISPCNT&7)+1)>>2])
    {
#ifdef BKPT_SUPPORT
      if(freezeVRAM[address])
        cheatsWriteByte(address + 0x06000000, b);
      else
#endif  
            *((u16 *)&vram[address]) = (b << 8) | b;
    }
    break;
  case 7:
#ifdef checkclearaddrrw
	goto unwritable;
#endif
    // no need to switch
    // byte writes to OAM are ignored
    //    *((u16 *)&emultoroam[address & 0x3FE]) = (b << 8) | b;
    break;    
  case 13:
#ifdef printsavewrite
	  	  iprintf("%X %X\n\r",address,b);
#endif

    if(cpuEEPROMEnabled) {
      eepromWrite(address, b);
      break;
    }
    goto unwritable;
  case 14:
#ifdef printsavewrite
	  	  iprintf("%X %X\n\r",address,b);
#endif
      if (!(saveType == 5) && (!eepromInUse | cpuSramEnabled | cpuFlashEnabled)) {

    //if(!cpuEEPROMEnabled && (cpuSramEnabled | cpuFlashEnabled)) { 

        (*cpuSaveGameFunc)(address, b);
      break;
    }
    // default
  default:
  unwritable:
#ifdef checkclearaddrrw
      //iprintf("Illegal word read: %08x at %08x\n", address,reg[15].I);
	  iprintf("Illegal byte write: %02x to %08x\n",b, address);
	  REG_IME = IME_DISABLE;
	  debugDump();
	  while(1);
#endif
    break;
  }
}



//for protection unit (arm exception triggers from virtualized ARM code running from GBA memory)
__attribute__((section(".itcm")))
void updateVCsub()
{
		u32 temp = REG_VCOUNT;
		u32 temp2 = REG_DISPSTAT;
		//iprintf("Vcountreal: %08x\n",temp);
		#ifdef usebuffedVcout
			VCOUNT = VCountdstogba[temp];
		#else
		if(temp < 192)
		{
			VCOUNT = ((temp * 214) >> 8);//VCOUNT = help * (1./1.2); //1.15350877;
			//help3 = (help + 1) * (1./1.2); //1.15350877;  // ichfly todo it is to slow
		}
		else
		{
			VCOUNT = (((temp - 192) * 246) >>  8)+ 160;//VCOUNT = ((temp - 192) * (1./ 1.04411764)) + 160 //* (1./ 1.04411764)) + 160; //1.15350877;
			//help3 = ((help - 191) *  (1./ 1.04411764)) + 160; //1.15350877;  // ichfly todo it is to slow			
		}
		#endif
		DISPSTAT &= 0xFFF8; //reset h-blanc and V-Blanc and V-Count Setting
		//if(help3 == VCOUNT) //else it is a extra long V-Line // ichfly todo it is to slow
		//{
			DISPSTAT |= (temp2 & 0x3); //temporary patch get original settings
		//}
		//if(VCOUNT > 160 && VCOUNT != 227)DISPSTAT |= 1;//V-Blanc
		UPDATE_REG(0x06, VCOUNT);
		if(VCOUNT == (DISPSTAT >> 8)) //update by V-Count Setting
		{
			DISPSTAT |= 0x4;
			/*if(DISPSTAT & 0x20) {
			  IF |= 4;
			  UPDATE_REG(0x202, IF);
			}*/
		}
		UPDATE_REG(0x04, DISPSTAT);
		//iprintf("Vcountreal: %08x\n",temp);
		//iprintf("DISPSTAT: %08x\n",temp2);
}


__attribute__((section(".itcm")))
u32 CPUReadMemoryrealpu(u32 address)
{

	//iprintf("%08X",REG_IME);
#ifdef DEV_VERSION
  if(address & 3) {  
    if(systemVerbose & VERBOSE_UNALIGNED_MEMORY) {
      iprintf("Unaligned word read: %08x at %08x\n", address, armMode ?
          armNextPC - 4 : armNextPC - 2);
    }
  }
#endif
  
#ifdef printreads
  iprintf("r32 %08x\n",address);
#endif
  
  u32 value=0;
  switch(address >> 24) {
  case 4:
  
	if(address > 0x40000FF && address < 0x4000111)
	{
		//todo timer shift
		value = *(u32 *)(address);
		break;
	}
#ifdef gba_handel_IRQ_correct
	if(address == 0x4000202 || address == 0x4000200)//ichfly update
	{
		IF = *(vuint16*)0x04000214; //VBlanc
		UPDATE_REG(0x202, IF);
	}
#endif	


	if(address > 0x4000003 && address < 0x4000008)//ichfly update
	{
		if(disableHBLANKIRQ == true){
			vcounthandler();
		}
		else{
			updateVCsub();
		}
	}
    if((address < 0x4000400) && ioReadable[address & 0x3fc]) {
      if(ioReadable[(address & 0x3fc) + 2])
        value = READ32LE(((u32*)&ioMem[address & 0x3fC]));
      else
        value = READ16LE(((u16*)&ioMem[address & 0x3fc]));
    } else goto unreadable;
    break;
  case 8:
  case 9:
  case 10:
  case 11:
  case 12:
#ifdef uppern_read_emulation
	if( (int)(address&0x1FFFFFC) > romSize)
	{
#ifdef print_uppern_read_emulation
		iprintf("high r32 %08x\n",address);
#endif
		if(ichflyfilestreamsize > (int)(address&0x1FFFFFC))
		{
			//fseek(ichflyfilestream , address&0x1FFFFFC , SEEK_SET);
			//fread(&value,1,4,ichflyfilestream);
			//ichfly_readfrom(ichflyfilestream,(address&0x1FFFFFC),(char*)&value,4);
			value = ichfly_readu32(address&0x1FFFFFC);
		}
		else
		{
			value = 0;
		}
	}
	else
	{
		value = READ32LE(((u32*)&rom[address&0x1FFFFFC]));
	}
#else
    value = READ32LE(((u32*)&rom[address&0x1FFFFFC]));
#endif
    break;
  case 13:
#ifdef printsaveread
	  iprintf("%X\n\r",address);
#endif
    if(cpuEEPROMEnabled)
      // no need to swap this
      return eepromRead(address);
    goto unreadable;
  case 14:
#ifdef printsaveread
	  iprintf("%X\n\r",address);
#endif
    if(cpuFlashEnabled | cpuSramEnabled)
      // no need to swap this
      return flashRead(address);
    // default
  default:
  unreadable:
  //while(1);
#ifdef checkclearaddrrw
      //iprintf("Illegal word read: %08x at %08x\n", address,reg[15].I);
	  iprintf("Illegal word read: %08x\n", address);
	  REG_IME = IME_DISABLE;
	  debugDump();
	  while(1);
#endif

    /*if(cpuDmaHack) { //ichly won't work
      value = cpuDmaLast;
    } else {
      if(armState) {
        value = CPUReadMemoryQuick(reg[15].I);
      } else {
        value = CPUReadHalfWordQuick(reg[15].I) |
          CPUReadHalfWordQuick(reg[15].I) << 16;
      }
    }*/
	  break;
  }

  if(address & 3) {
//#ifdef C_CORE
    int shift = (address & 3) << 3;
    value = (value >> shift) | (value << (32 - shift));
/*#else    
#ifdef __GNUC__ ichfly
    asm("and $3, %%ecx;"
        "shl $3 ,%%ecx;"
        "ror %%cl, %0"
        : "=r" (value)
        : "r" (value), "c" (address));
#else
    asm(
      mov ecx, address;
      and ecx, 3;
      shl ecx, 3;
      ror [dword ptr value], cl;
    )
//#endif
#endif*/
  }
  return value;
}

__attribute__((section(".itcm")))
u16 CPUReadHalfWordrealpu(u32 address) //ichfly not inline is faster because it is smaler
{
#ifdef printreads
	iprintf("r16 %08x\n",address);
#endif
#ifdef DEV_VERSION      
  if(address & 1) {
    if(systemVerbose & VERBOSE_UNALIGNED_MEMORY) {
      iprintf("Unaligned halfword read: %08x at %08x\n", address, armMode ?
          armNextPC - 4 : armNextPC - 2);
    }
  }
#endif
  
  u32 value=0;
  
  switch(address >> 24) {
  case 4:
  
	if(address > 0x40000FF && address < 0x4000111)
	{
		
		if((address&0x2) == 0)
		{
			if(ioMem[address & 0x3fe] & 0x8000)
			{
				value = ((*(u16 *)(address)) >> 1) | 0x8000;
			}
			else
			{
				value = (*(u16 *)(address)) >> 1;
			}
			return value;
		}
	}
  
	if(address > 0x4000003 && address < 0x4000008)//ichfly update
	{
		if(disableHBLANKIRQ == true){
			vcounthandler();
		}
		else{
			updateVCsub();
		}
	}
	
#ifdef gba_handel_IRQ_correct
	if(address == 0x4000202)//ichfly update
	{
		IF = *(vuint16*)0x04000214;
		UPDATE_REG(0x202, IF);
	}
#endif
	
    if((address < 0x4000400) && ioReadable[address & 0x3fe])
    {
		value =  READ16LE(((u16*)&ioMem[address & 0x3fe]));
    }
    else goto unreadable;
    break;
  case 8:
  case 9:
  case 10:
  case 11:
  case 12:
    if(address == 0x80000c4 || address == 0x80000c6 || address == 0x80000c8)
      value = rtcRead(address);
    else
	{
#ifdef uppern_read_emulation
	if( (int)(address&0x1FFFFFE) > romSize)
	{
#ifdef print_uppern_read_emulation
		iprintf("high r16 %08x\n",address);
#endif
		if(ichflyfilestreamsize > (int)(address&0x1FFFFFE))
		{
			//fseek (ichflyfilestream , address&0x1FFFFFE , SEEK_SET);
			//fread (&value,1,2,ichflyfilestream);
			//ichfly_readfrom(ichflyfilestream,(address&0x1FFFFFE),(char*)&value,2);
			value = ichfly_readu16(address&0x1FFFFFE);
		}
		else
		{
			value = 0;
		}
	}
	else
	{
		value = READ16LE(((u16*)&rom[address & 0x1FFFFFE]));
	}
#else
    value = READ16LE(((u16*)&rom[address & 0x1FFFFFE]));
#endif
	}
    break;    
  case 13:
#ifdef printsaveread
	  iprintf("%X\n\r",address);
#endif    

	if(cpuEEPROMEnabled)
      // no need to swap this
      return  eepromRead(address);
    goto unreadable;
  case 14:
#ifdef printsaveread
	  iprintf("%X\n\r",address);
#endif    
	
	if(cpuFlashEnabled | cpuSramEnabled)
      // no need to swap this
      return flashRead(address);
    // default
  default:
  unreadable:
#ifdef checkclearaddrrw
      //iprintf("Illegal word read: %08x at %08x\n", address,reg[15].I);
	  iprintf("Illegal hword read: %08x\n", address);
	  REG_IME = IME_DISABLE;
	  debugDump();
	  while(1);
#endif

    /*if(cpuDmaHack) {
      value = cpuDmaLast & 0xFFFF;
    } else {
      if(armState) {
        value = CPUReadHalfWordQuick(reg[15].I + (address & 2));
      } else {
        value = CPUReadHalfWordQuick(reg[15].I);
      }
    }*/
    break;
  }

  if(address & 1) {
    value = (value >> 8) | (value << 24);
  }
  
  return value;
}

__attribute__((section(".itcm")))
u8 CPUReadByterealpu(u32 address) //ichfly not inline is faster because it is smaler
{
#ifdef printreads
iprintf("r8 %02x\n",address);
#endif

  switch(address >> 24) {
  case 4:
  
	if(address > 0x40000FF && address < 0x4000111)
	{
		//todo timer shift
		return *(u8 *)(address);
	}
  
  	if(address > 0x4000003 && address < 0x4000008)//ichfly update
	{
		if(disableHBLANKIRQ == true){
			vcounthandler();
		}
		else{
			updateVCsub();
		}
	}
#ifdef gba_handel_IRQ_correct
	if(address == 0x4000202 || address == 0x4000203)//ichfly update
	{
		IF = *(vuint16*)0x04000214; //VBlanc
		UPDATE_REG(0x202, IF);
	}
#endif
    if((address < 0x4000400) && ioReadable[address & 0x3ff])
      return ioMem[address & 0x3ff];
    else goto unreadable;
  case 8:
  case 9:
  case 10:
  case 11:
  case 12:

#ifdef uppern_read_emulation
	if( (int)(address&0x1FFFFFF) > romSize)
	{
#ifdef print_uppern_read_emulation
		iprintf("high r8 %08x\n",address);
#endif
		if(ichflyfilestreamsize > (int)(address&0x1FFFFFF))
		{
			//u8 temp = 0;
			//fseek (ichflyfilestream , address&0x1FFFFFF , SEEK_SET);
			//fread (&temp,1,1,ichflyfilestream);
			//ichfly_readfrom(ichflyfilestream,(address&0x1FFFFFF),(char*)&temp,1);
			return ichfly_readu8(address&0x1FFFFFF);
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return rom[address & 0x1FFFFFF];
	}
#else
    return rom[address & 0x1FFFFFF];
#endif        
  case 13:
#ifdef printsaveread
	  iprintf("%X\n\r",address);
#endif    
	
	if(cpuEEPROMEnabled)
      return eepromRead(address);
    goto unreadable;
  case 14:
#ifdef printsaveread
	  iprintf("%X\n\r",address);
#endif    
if(cpuSramEnabled | cpuFlashEnabled)
      return flashRead(address);
    if(cpuEEPROMSensorEnabled) {
      switch(address & 0x00008f00) {
      case 0x8200:
        return systemGetSensorX() & 255;
      case 0x8300:
        return (systemGetSensorX() >> 8)|0x80;
      case 0x8400:
        return systemGetSensorY() & 255;
      case 0x8500:
        return systemGetSensorY() >> 8;
      }
    }
    // default
  default:
  unreadable:
#ifdef checkclearaddrrw
      //iprintf("Illegal word read: %08x at %08x\n", address,reg[15].I);
	  iprintf("Illegal byte read: %08x\n", address);
	  REG_IME = IME_DISABLE;
	  debugDump();
	  while(1);
#endif

    /*if(cpuDmaHack) {
      return cpuDmaLast & 0xFF;
    } else {
      if(armState) {
        return CPUReadByteQuick(reg[15].I+(address & 3));
      } else {
        return CPUReadByteQuick(reg[15].I+(address & 1));
      }
    }*/
    break;
  }

return 0;
}

#ifndef asmspeedup
__attribute__((section(".itcm")))
void CPUWriteMemorypu(u32 address, u32 value) //ichfly not inline is faster because it is smaler
{
#ifdef printreads
    iprintf("w32 %08x to %08x\n",value,address);
#endif		  

#ifdef DEV_VERSION
  if(address & 3) {
    if(systemVerbose & VERBOSE_UNALIGNED_MEMORY) {
      iprintf("Unaligned word write: %08x to %08x from %08x\n",
          value,
          address,
          armMode ? armNextPC - 4 : armNextPC - 2);
    }
  }
#endif
  
  switch(address >> 24) {


#ifdef fullsync
  case 0x06:
#ifdef checkclearaddrrw
	if(address > 0x06020000)goto unreadable;
#endif
    address = (address & 0x1fffc);
    if (((DISPCNT & 7) >2) && ((address & 0x1C000) == 0x18000))
        return;
    if ((address & 0x18000) == 0x18000)
      address &= 0x17fff;

#ifdef BKPT_SUPPORT
    if(*((u32 *)&freezeVRAM[address]))
      cheatsWriteMemory(address + 0x06000000, value);
    else
#endif
    
    WRITE32LE(((u32*)&vram[address]), value);
    break;
  case 0x07:
#ifdef checkclearaddrrw
	if(address > 0x07000400)goto unreadable;
#endif
#ifdef BKPT_SUPPORT
    if(*((u32 *)&freezeOAM[address & 0x3fc]))
      cheatsWriteMemory(address & 0x70003FC,
                        value);
    else
#endif
    WRITE32LE(((u32*)&emultoroam[address & 0x3fc]), value);
#endif

  case 0x04:
    if(address < 0x4000400) {

	/*if((0x4000060 > address && address > 0x4000007) || (address > 0x40000FF && address < 0x4000110)) //timer and lcd
	{
			//iprintf("32 %x %x\r\n",address,value);
		    *(u32 *)(address) = value;
	}
	else //dont do twice*/ //don't need that any more
	{
      CPUUpdateRegister((address & 0x3FC), value & 0xFFFF);
      CPUUpdateRegister((address & 0x3FC) + 2, (value >> 16));
    }
	} else goto unwritable;
    break;
  case 0x0D:
#ifdef printsavewrite
	  iprintf("%X %X\n\r",address,value);
#endif
    if(cpuEEPROMEnabled) {
      eepromWrite(address, value);
      break;
    }
    goto unwritable;
  case 0x0E:
#ifdef printsavewrite
	  iprintf("%X %X\n\r",address,value);
#endif
    if(!eepromInUse | cpuSramEnabled | cpuFlashEnabled) {
      (*cpuSaveGameFunc)(address, (u8)value);
      break;
    }
    // default
  default:
  unwritable:
  
#ifdef checkclearaddrrw
      //iprintf("Illegal word read: %08x at %08x\n", address,reg[15].I);
	  iprintf("Illegal word write: %08x to %08x\n",value, address);
	  REG_IME = IME_DISABLE;
	  debugDump();
	  while(1);
#endif
    break;
  }
}
#else
	extern "C" void CPUWriteMemorypu(u32 address, u32 value);
#endif

__attribute__((section(".itcm")))
void CPUWriteHalfWordpu(u32 address, u16 value)
{
#ifdef printreads
iprintf("w16 %04x to %08x\r\n",value,address);
#endif

#ifdef DEV_VERSION
  if(address & 1) {
    if(systemVerbose & VERBOSE_UNALIGNED_MEMORY) {
      iprintf("Unaligned halfword write: %04x to %08x from %08x\n",
          value,
          address,
          armMode ? armNextPC - 4 : armNextPC - 2);
    }
  }
#endif
  
  switch(address >> 24) {  


#ifdef fullsync
  case 5:
#ifdef BKPT_SUPPORT
    if(*((u16 *)&freezePRAM[address & 0x03fe]))
      cheatsWriteHalfWord(address & 0x70003fe,
                          value);
    else
#endif
#ifdef checkclearaddrrw
	if(address > 0x05000400)goto unwritable;
#endif
    WRITE16LE(((u16*)&paletteRAM[address & 0x3fe]), value);
    break;
  case 6:
#ifdef checkclearaddrrw
	if(address > 0x06020000)goto unwritable;
#endif
    address = (address & 0x1fffe);
    if (((DISPCNT & 7) >2) && ((address & 0x1C000) == 0x18000))
        return;
    if ((address & 0x18000) == 0x18000)
      address &= 0x17fff;
#ifdef BKPT_SUPPORT
    if(*((u16 *)&freezeVRAM[address]))
      cheatsWriteHalfWord(address + 0x06000000,
                          value);
    else
#endif
    WRITE16LE(((u16*)&vram[address]), value); 
    break;
#endif

case 4:
    if(address < 0x4000400)
      CPUUpdateRegister(address & 0x3fe, value);
    else goto unwritable;
    break;
  case 8:
  case 9:
    if(address == 0x80000c4 || address == 0x80000c6 || address == 0x80000c8) {
      if(!rtcWrite(address, value))
        goto unwritable;
    } else if(!agbPrintWrite(address, value)) goto unwritable;
    break;
  case 13:
#ifdef printsavewrite
	  iprintf("%X %X\n\r",address,value);
#endif
    if(cpuEEPROMEnabled) {
      eepromWrite(address, (u8)value);
      break;
    }
    goto unwritable;
  case 14:
#ifdef printsavewrite
	  iprintf("%X %X\n\r",address,value);
#endif
    if(!eepromInUse | cpuSramEnabled | cpuFlashEnabled) {
      (*cpuSaveGameFunc)(address, (u8)value);
      break;
    }
    goto unwritable;
  default:
  unwritable:
#ifdef checkclearaddrrw
      //iprintf("Illegal word read: %08x at %08x\n", address,reg[15].I);
	  iprintf("Illegal hword write: %04x to %08x\n",value, address);
	  REG_IME = IME_DISABLE;
	  debugDump();
	  while(1);
#endif
    break;
  }
}

__attribute__((section(".itcm")))
void CPUWriteBytepu(u32 address, u8 b)
{
#ifdef printreads
	iprintf("w8 %02x to %08x\r\n",b,address);
#endif
  switch(address >> 24) {


#ifdef fullsync
  case 5:
#ifdef checkclearaddrrw
	if(address > 0x05000400)goto unwritable;
#endif
    // no need to switch
    *((u16 *)&paletteRAM[address & 0x3FE]) = (b << 8) | b;
    break;
  case 6:
#ifdef checkclearaddrrw
	if(address > 0x06020000)goto unwritable;
#endif
    address = (address & 0x1fffe);
    if (((DISPCNT & 7) >2) && ((address & 0x1C000) == 0x18000))
        return;
    if ((address & 0x18000) == 0x18000)
      address &= 0x17fff;

    // no need to switch 
    // byte writes to OBJ VRAM are ignored
    if ((address) < objTilesAddress[((DISPCNT&7)+1)>>2])
    {
#ifdef BKPT_SUPPORT
      if(freezeVRAM[address])
        cheatsWriteByte(address + 0x06000000, b);
      else
#endif  
            *((u16 *)&vram[address]) = (b << 8) | b;
    }
    break;
#endif


  case 4:
    if(address < 0x4000400) {
      switch(address & 0x3FF) {
      case 0x301:
	/*if(b == 0x80) //todo
	  stopState = true;
	holdState = 1;
	holdType = -1;
  cpuNextEvent = cpuTotalTicks;
	break;*/
      case 0x60:
      case 0x61:
      case 0x62:
      case 0x63:
      case 0x64:
      case 0x65:
      case 0x68:
      case 0x69:
      case 0x6c:
      case 0x6d:
      case 0x70:
      case 0x71:
      case 0x72:
      case 0x73:
      case 0x74:
      case 0x75:
      case 0x78:
      case 0x79:
      case 0x7c:
      case 0x7d:
      case 0x80:
      case 0x81:
      case 0x84:
      case 0x85:
      case 0x90:
      case 0x91:
      case 0x92:
      case 0x93:
      case 0x94:
      case 0x95:
      case 0x96:
      case 0x97:
      case 0x98:
      case 0x99:
      case 0x9a:
      case 0x9b:
      case 0x9c:
      case 0x9d:
      case 0x9e:
      case 0x9f:      
	//soundEvent(address&0xFF, b);  //ichfly disable sound
#ifdef printsoundwrites
		  iprintf("b %02x to %08x\r\n",b,address);
#endif
	  #ifdef arm9advsound
		  SendArm7Command((u32)((address & 0x3FF) | 0x40000000), (u32)b);	//faster in case we send a 0
		#endif
	break;
      default:
	/*if((0x4000060 > address && address > 0x4000008) || (address > 0x40000FF && address < 0x4000110))
	{
			//iprintf("8 %x %x\r\n",address,b);
		    *(u8 *)(address) = b;
	}*/ //ichfly don't need that
	if(address & 1)
	{
	  CPUUpdateRegister(address & 0x3fe,
			    ((READ16LE(((u16*)&ioMem[address & 0x3fe])))
			     & 0x00FF) |
			    b<<8);

	}
	else
	  CPUUpdateRegister(address & 0x3fe,
			    ((READ16LE(((u16*)&ioMem[address & 0x3fe])) & 0xFF00) | b));
      }
      break;
    } else goto unwritable;
    break;   
  case 13:
#ifdef printsavewrite
	  iprintf("%X %X\n\r",address,b);
#endif
    if(cpuEEPROMEnabled) {
      eepromWrite(address, b);
      break;
    }
    goto unwritable;
  case 14:
#ifdef printsavewrite
	  iprintf("%X %X\n\r",address,b);
#endif
      if (!(saveType == 5) && (!eepromInUse | cpuSramEnabled | cpuFlashEnabled)) {

    //if(!cpuEEPROMEnabled && (cpuSramEnabled | cpuFlashEnabled)) { 
        (*cpuSaveGameFunc)(address, b);
      break;
    }
    // default
  default:
  unwritable:
#ifdef checkclearaddrrw
      //iprintf("Illegal word read: %08x at %08x\n", address,reg[15].I);
	  iprintf("Illegal byte write: %02x to %08x\n",b, address);
	  REG_IME = IME_DISABLE;
	  debugDump();
	  while(1);
#endif
    break;
  }
}

__attribute__((section(".itcm")))
s16 CPUReadHalfWordrealpuSigned(u32 address)
{
  u16 value = CPUReadHalfWordrealpu(address);
  if((address & 1))
    value = (s8)value;
  return value;
}