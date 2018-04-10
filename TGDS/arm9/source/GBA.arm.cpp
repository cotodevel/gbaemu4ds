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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "typedefsTGDS.h"
#include "dsregs.h"
#include "dsregs_asm.h"

#include "fsfatlayerTGDSLegacy.h"
#include "fileHandleTGDS.h"
#include "InterruptsARMCores_h.h"
#include "specific_shared.h"
#include "ff.h"
#include "memoryHandleTGDS.h"
#include "reent.h"
#include "sys/types.h"
#include "consoleTGDS.h"
#include "utilsTGDS.h"
#include "devoptab_devices.h"
#include "posixHandleTGDS.h"
#include "about.h"
#include "xenofunzip.h"

#include "ipcfifoTGDS.h"
#include "specific_shared.h"

#include "GBA.h"
#include "Sound.h"
#include "Util.h"
#include "getopt.h"
#include "System.h"
#include "ichflysettings.h"
#include "dmaTGDS.h"

#ifdef loadEmbedded
#include "puzzleorginal_bin.h"
#endif

bool ichflytest = false;

#include "GBAinline.h"
#include "Globals.h"
//#include "Gfx.h" //ichfly not that
#include "EEprom.h"
#include "Flash.h"
#include "Sound.h"
#include "Sram.h"
#include "biosTGDS.h"
#include "Cheats.h"
#include "NLS.h"
#include "elf.h"
#include "Port.h"
#ifdef PROFILING
#include "prof/prof.h"
#endif


__attribute__((section(".dtcm")))
int SWITicks = 0;
__attribute__((section(".dtcm")))
int IRQTicks = 0;

__attribute__((section(".dtcm")))
int layerEnableDelay = 0;
__attribute__((section(".dtcm")))
bool busPrefetch = false;
__attribute__((section(".dtcm")))
bool busPrefetchEnable = false;
__attribute__((section(".dtcm")))
u32 busPrefetchCount = 0;
__attribute__((section(".dtcm")))
int cpuDmaTicksToUpdate = 0;
__attribute__((section(".dtcm")))
int cpuDmaCount = 0;
__attribute__((section(".dtcm")))
bool cpuDmaHack = false;
__attribute__((section(".dtcm")))
u32 cpuDmaLast = 0;
__attribute__((section(".dtcm")))
int dummyAddress = 0;
__attribute__((section(".dtcm")))
bool cpuBreakLoop = false;
__attribute__((section(".dtcm")))
int cpuNextEvent = 0;
__attribute__((section(".dtcm")))
int gbaSaveType = 0; // used to remember the save type on reset
__attribute__((section(".dtcm")))
bool intState = false;
__attribute__((section(".dtcm")))
bool stopState = false;
__attribute__((section(".dtcm")))
bool holdState = false;
__attribute__((section(".dtcm")))
int holdType = 0;
__attribute__((section(".dtcm")))
bool cpuSramEnabled = true;
__attribute__((section(".dtcm")))
bool cpuFlashEnabled = true;
__attribute__((section(".dtcm")))
bool cpuEEPROMEnabled = true;
__attribute__((section(".dtcm")))
bool cpuEEPROMSensorEnabled = false;

__attribute__((section(".dtcm")))
u32 cpuPrefetch[2];
__attribute__((section(".dtcm")))
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

__attribute__((section(".dtcm")))
int lcdTicks = (useBios && !skipBios) ? 1008 : 208;
__attribute__((section(".dtcm")))
u8 timerOnOffDelay = 0;


void (*cpuSaveGameFunc)(u32,u8) = flashSaveDecide;
//void (*renderLine)() = mode0RenderLine;
__attribute__((section(".dtcm")))
bool fxOn = false;
__attribute__((section(".dtcm")))
bool windowOn = false;
__attribute__((section(".dtcm")))
int frameCount = 0;
__attribute__((section(".dtcm")))
char buffer[1024];
FILE *out = NULL;
__attribute__((section(".dtcm")))
int count = 0;

__attribute__((section(".dtcm")))
int capture = 0;
__attribute__((section(".dtcm")))
int capturePrevious = 0;
__attribute__((section(".dtcm")))
int captureNumber = 0;
__attribute__((section(".dtcm")))
const int TIMER_TICKS[4] = {
  0,
  6,
  8,
  10
};


__attribute__((section(".dtcm")))
u32  objTilesAddress [3] = {0x010000, 0x014000, 0x014000};

__attribute__((section(".dtcm")))
const u8 gamepakRamWaitState[4] = { 4, 3, 2, 8 };
__attribute__((section(".dtcm")))
const u8 gamepakWaitState[4] =  { 4, 3, 2, 8 };
__attribute__((section(".dtcm")))
const u8 gamepakWaitState0[2] = { 2, 1 };
__attribute__((section(".dtcm")))
const u8 gamepakWaitState1[2] = { 4, 1 };
__attribute__((section(".dtcm")))
const u8 gamepakWaitState2[2] = { 8, 1 };
__attribute__((section(".dtcm")))
const bool isInRom [16]=
  { false, false, false, false, false, false, false, false,
    true, true, true, true, true, true, false, false };              

__attribute__((section(".dtcm")))
u8 memoryWait[16] =
  { 0, 0, 2, 0, 0, 0, 0, 0, 4, 4, 4, 4, 4, 4, 4, 0 };
__attribute__((section(".dtcm")))
u8 memoryWait32[16] =
  { 0, 0, 5, 0, 0, 1, 1, 0, 7, 7, 9, 9, 13, 13, 4, 0 };
__attribute__((section(".dtcm")))
u8 memoryWaitSeq[16] =
  { 0, 0, 2, 0, 0, 0, 0, 0, 2, 2, 4, 4, 8, 8, 4, 0 };
__attribute__((section(".dtcm")))
u8 memoryWaitSeq32[16] =
  { 0, 0, 5, 0, 0, 1, 1, 0, 5, 5, 9, 9, 17, 17, 4, 0 };

// The videoMemoryWait constants are used to add some waitstates
// if the opcode access video memory data outside of vblank/hblank
// It seems to happen on only one ticks for each pixel.
// Not used for now (too problematic with current code).
//const u8 videoMemoryWait[16] =
//  {0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0};

__attribute__((section(".dtcm")))
u8 biosProtected[4];

#ifdef WORDS_BIGENDIAN
bool cpuBiosSwapped = false;
#endif

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
  { &GBA_DISPCNT  , sizeof(u16) },
  { &GBA_DISPSTAT , sizeof(u16) },
  { &GBA_VCOUNT   , sizeof(u16) },
  { &GBA_BG0CNT   , sizeof(u16) },
  { &GBA_BG1CNT   , sizeof(u16) },
  { &GBA_BG2CNT   , sizeof(u16) },
  { &GBA_BG3CNT   , sizeof(u16) },
  { &GBA_BG0HOFS  , sizeof(u16) },
  { &GBA_BG0VOFS  , sizeof(u16) },
  { &GBA_BG1HOFS  , sizeof(u16) },
  { &GBA_BG1VOFS  , sizeof(u16) },
  { &GBA_BG2HOFS  , sizeof(u16) },
  { &GBA_BG2VOFS  , sizeof(u16) },
  { &GBA_BG3HOFS  , sizeof(u16) },
  { &GBA_BG3VOFS  , sizeof(u16) },
  { &GBA_BG2PA    , sizeof(u16) },
  { &GBA_BG2PB    , sizeof(u16) },
  { &GBA_BG2PC    , sizeof(u16) },
  { &GBA_BG2PD    , sizeof(u16) },
  { &GBA_BG2X_L   , sizeof(u16) },
  { &GBA_BG2X_H   , sizeof(u16) },
  { &GBA_BG2Y_L   , sizeof(u16) },
  { &GBA_BG2Y_H   , sizeof(u16) },
  { &GBA_BG3PA    , sizeof(u16) },
  { &GBA_BG3PB    , sizeof(u16) },
  { &GBA_BG3PC    , sizeof(u16) },
  { &GBA_BG3PD    , sizeof(u16) },
  { &GBA_BG3X_L   , sizeof(u16) },
  { &GBA_BG3X_H   , sizeof(u16) },
  { &GBA_BG3Y_L   , sizeof(u16) },
  { &GBA_BG3Y_H   , sizeof(u16) },
  { &GBA_WIN0H    , sizeof(u16) },
  { &GBA_WIN1H    , sizeof(u16) },
  { &GBA_WIN0V    , sizeof(u16) },
  { &GBA_WIN1V    , sizeof(u16) },
  { &GBA_WININ    , sizeof(u16) },
  { &GBA_WINOUT   , sizeof(u16) },
  { &GBA_MOSAIC   , sizeof(u16) },
  { &GBA_BLDMOD   , sizeof(u16) },
  { &GBA_COLEV    , sizeof(u16) },
  { &GBA_COLY     , sizeof(u16) },
  { &SpecificIPCAlign->DM0SAD_L , sizeof(u16) },
  { &SpecificIPCAlign->DM0SAD_H , sizeof(u16) },
  { &SpecificIPCAlign->DM0DAD_L , sizeof(u16) },
  { &SpecificIPCAlign->DM0DAD_H , sizeof(u16) },
  { &SpecificIPCAlign->DM0CNT_L , sizeof(u16) },
  { &SpecificIPCAlign->DM0CNT_H , sizeof(u16) },
  { &SpecificIPCAlign->DM1SAD_L , sizeof(u16) },
  { &SpecificIPCAlign->DM1SAD_H , sizeof(u16) },
  { &SpecificIPCAlign->DM1DAD_L , sizeof(u16) },
  { &SpecificIPCAlign->DM1DAD_H , sizeof(u16) },
  { &SpecificIPCAlign->DM1CNT_L , sizeof(u16) },
  { &SpecificIPCAlign->DM1CNT_H , sizeof(u16) },
  { &SpecificIPCAlign->DM2SAD_L , sizeof(u16) },
  { &SpecificIPCAlign->DM2SAD_H , sizeof(u16) },
  { &SpecificIPCAlign->DM2DAD_L , sizeof(u16) },
  { &SpecificIPCAlign->DM2DAD_H , sizeof(u16) },
  { &SpecificIPCAlign->DM2CNT_L , sizeof(u16) },
  { &SpecificIPCAlign->DM2CNT_H , sizeof(u16) },
  { &SpecificIPCAlign->DM3SAD_L , sizeof(u16) },
  { &SpecificIPCAlign->DM3SAD_H , sizeof(u16) },
  { &SpecificIPCAlign->DM3DAD_L , sizeof(u16) },
  { &SpecificIPCAlign->DM3DAD_H , sizeof(u16) },
  { &SpecificIPCAlign->DM3CNT_L , sizeof(u16) },
  { &SpecificIPCAlign->DM3CNT_H , sizeof(u16) },
  { &SpecificIPCAlign->TM0D     , sizeof(u16) },
  { &TM0CNT   , sizeof(u16) },
  { &SpecificIPCAlign->TM1D     , sizeof(u16) },
  { &TM1CNT   , sizeof(u16) },
  { &SpecificIPCAlign->TM2D     , sizeof(u16) },
  { &TM2CNT   , sizeof(u16) },
  { &SpecificIPCAlign->TM3D     , sizeof(u16) },
  { &TM3CNT   , sizeof(u16) },
  { &P1       , sizeof(u16) },
  { &SpecificIPCAlign->GBA_IE       , sizeof(u16) },
  { &SpecificIPCAlign->GBA_IF       , sizeof(u16) },
  { &SpecificIPCAlign->GBA_IME      , sizeof(u16) },
  { &holdState, sizeof(bool) },
  { &holdType, sizeof(int) },
  { &lcdTicks, sizeof(int) },
  { &SpecificIPCAlign->timer0On , sizeof(bool) },
  { &SpecificIPCAlign->timer0Ticks , sizeof(int) },
  { &SpecificIPCAlign->timer0Reload , sizeof(int) },
  { &SpecificIPCAlign->timer0ClockReload  , sizeof(int) },
  { &SpecificIPCAlign->timer1On , sizeof(bool) },
  { &SpecificIPCAlign->timer1Ticks , sizeof(int) },
  { &SpecificIPCAlign->timer1Reload , sizeof(int) },
  { &SpecificIPCAlign->timer1ClockReload  , sizeof(int) },
  { &SpecificIPCAlign->timer2On , sizeof(bool) },
  { &SpecificIPCAlign->timer2Ticks , sizeof(int) },
  { &SpecificIPCAlign->timer2Reload , sizeof(int) },
  { &SpecificIPCAlign->timer2ClockReload  , sizeof(int) },
  { &SpecificIPCAlign->timer3On , sizeof(bool) },
  { &SpecificIPCAlign->timer3Ticks , sizeof(int) },
  { &SpecificIPCAlign->timer3Reload , sizeof(int) },
  { &SpecificIPCAlign->timer3ClockReload  , sizeof(int) },
  { &SpecificIPCAlign->dma0Source , sizeof(u32) },
  { &SpecificIPCAlign->dma0Dest , sizeof(u32) },
  { &SpecificIPCAlign->dma1Source , sizeof(u32) },
  { &SpecificIPCAlign->dma1Dest , sizeof(u32) },
  { &SpecificIPCAlign->dma2Source , sizeof(u32) },
  { &SpecificIPCAlign->dma2Dest , sizeof(u32) },
  { &SpecificIPCAlign->dma3Source , sizeof(u32) },
  { &SpecificIPCAlign->dma3Dest , sizeof(u32) },
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

__attribute__((section(".dtcm")))
int romSize = 0x200000; //test normal 0x2000000 current 1/10 oh no only 2.4 MB


bool CPUWriteBatteryFile(const char *fileName)
{
  if(gbaSaveType == 0) {
    if(eepromInUse)
      gbaSaveType = 3;
    else switch(saveType) {
    case 1:
      gbaSaveType = 1;
      break;
    case 2:
      gbaSaveType = 2;
      break;
    }
  }
  
  if((gbaSaveType) && (gbaSaveType!=5)) {
    FILE *file = fopen(fileName, "w+");
    
    if(!file) {
      systemMessage(MSG_ERROR_CREATING_FILE, N_("Error creating file %s"),
                    fileName);
      return false;
    }
    
    // only save if Flash/Sram in use or EEprom in use
    if(gbaSaveType != 3) {
      if(gbaSaveType == 2) {
        if(fwrite(flashSaveMemory, 1, flashSize, file) != (size_t)flashSize) {
          fclose(file);
          return false;
        }
      } else {
        if(fwrite(flashSaveMemory, 1, 0x10000, file) != 0x10000) {
          fclose(file);
          return false;
        }
      }
    } else {
      if(fwrite(eepromData, 1, eepromSize, file) != (size_t)eepromSize) {
        fclose(file);
        return false;
      }
    }
    fclose(file);
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

  int size = ftell(file);
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
    } else {
      if(fread(flashSaveMemory, 1, 0x10000, file) != 0x10000) {
        fclose(file);
        return false;
      }
      flashSetSize(0x10000);
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
#ifndef loadEmbedded
  u8 *whereToLoad = rom;
  if(cpuIsMultiBoot)whereToLoad = workRAM;

		if(!utilLoad(szFile,whereToLoad,romSize,extram))
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
#ifdef WORDS_BIGENDIAN
static void CPUSwap(volatile u32 *a, volatile u32 *b)
{
  volatile u32 c = *b;
  *b = *a;
  *a = c;
}
#else
void CPUSwap(u32 *a, u32 *b)
{
  u32 c = *b;
  *b = *a;
  *a = c;
}
#endif


//DMA GBA 

//for non mapped nds IO (software layer), slow
__attribute__((section(".itcm")))
void  doDMAslow(u32 s, u32 d, u32 si, u32 di, u32 c, int transfer32) //ichfly veraltet
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

//for NDS <-> GBA io map = fast except for DMA Direct Sound Channel
__attribute__((section(".itcm")))
void  doDMA(u32 s, u32 d, u32 si, u32 di, u32 c, int transfer32) //ichfly veraltet
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

			if(((s&0x1FFFFFF) + c*4) > romSize) //slow
			{
#ifdef print_uppern_read_emulation
				printf("highdmaread %X %X %X %X %X %X\r ",s,d,c,si,di,transfer32);
#endif
				if(di == -4 || si == -4)//this can't work the slow way so use the
				{
					doDMAslow(s, d, si, di, c, transfer32); //very slow way
					return;
				}
				if(transfer32)
				{
#ifdef ownfilebuffer
					//printf("4 %08X %08X %08X %08X ",s,d,c,*(u32 *)d);
					ichfly_readdma_rom((u32)(s&0x1FFFFFF),(u8 *)d,c,4);
					//printf("exit%08X ",*(u32 *)d);
					//while(1);
#else
					//doDMAslow(s, d, si, di, c, transfer32);
					fseek (ichflyfilestream , (s&0x1FFFFFF) , SEEK_SET);
					//printf("seek %08X\r ",s&0x1FFFFFF);
					int dkdkdkdk = fread ((void*)d,1,c * 4,ichflyfilestream); // fist is buggy
					//printf("(%08X %08X %08X) ret %08X\r ",d,c,ichflyfilestream,dkdkdkdk);
#endif
				}
				else
				{
#ifdef ownfilebuffer
					//printf("2 %08X %08X %08X %04X ",s,d,c,*(u16 *)d);
					ichfly_readdma_rom((u32)(s&0x1FFFFFF),(u8 *)d,c,2);
					//printf("exit%04X ",*(u16 *)d);
					//while(1);
#else
					//printf("teeees");
					//doDMAslow(s, d, si, di, c, transfer32);
					fseek (ichflyfilestream , (s&0x1FFFFFF) , SEEK_SET);
					//printf("seek %08X\r ",s&0x1FFFFFF);
					int dkdkdkdk = fread ((void*)d,1,c * 2,ichflyfilestream);
					//printf("(%08X %08X %08X) ret %08X\r ",d,c,ichflyfilestream,dkdkdkdk);
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
		DMAXSAD(3) = s;	//DMA3_SRC = s;
		DMAXDAD(3) = d;	//DMA3_DEST = d;

		int tmpzahl = DMAENABLED | c;	//DMA_ENABLE | c;
		if(transfer32)tmpzahl |= DMA32BIT;	//DMA_32_BIT;
		if(di == -4) tmpzahl |= DMADECR_DEST;	//DMA_DST_DEC;
		if(si == -4) tmpzahl |= DMADECR_SRC;	//DMA_SRC_DEC;
		DMAXCNT(3) = tmpzahl;
		//printf("%x,%x,%x",s,d,tmpzahl);
	}

}


//coto: FIFO hardware when possible / Direct Sound DMA

__attribute__((section(".itcm")))
void doDMAFIFO(u32 s, u32 d, u32 si, u32 di, u32 c, int transfer32){
    switch(s&0x0f000000){
        case(0x02000000):
        case(0x03000000):
        case(0x05000000):
        case(0x06000000):
        case(0x07000000):
        {
            //FIFO select
            switch(d&0x000000ff){
                //FIFO DMA A
                case 0xa0:{
					
					int tmpzahl = DMAENABLED | INTERNAL_FIFO_SIZE;
					
                    if(transfer32) {
                        s &= 0xFFFFFFFC;
						tmpzahl |= DMA32BIT;
                    }
                    else{
                        s &= 0xFFFFFFFE;
						tmpzahl |= DMA16BIT;
                    }
					
					DMAXSAD(1) = s;
					DMAXDAD(1) = (u32)&SpecificIPCAlign->fifodmasA[0];
					
					//if(di == -4) tmpzahl |= DMA_DST_DEC;
					if(si == -4) tmpzahl |= DMADECR_SRC;
					
					DMAXCNT(1) = tmpzahl;
					
					SpecificIPCAlign->dma1_donearm9 = true;
					//SendMultipleWordACK(0xc1710004,0x0,0x0,0x0); //works
				}
                break;
                
				//FIFO DMA B
                case(0xa4):{
                    int tmpzahl = DMAENABLED | INTERNAL_FIFO_SIZE;
					
                    if(transfer32) {
                        s &= 0xFFFFFFFC;
						tmpzahl |= DMA32BIT;
                    }
                    else{
                        s &= 0xFFFFFFFE;
						tmpzahl |= DMA16BIT;
                    }
					
					DMAXSAD(2) = s;
					DMAXDAD(2) = (u32)&SpecificIPCAlign->fifodmasB[0];
					
					//if(di == -4) tmpzahl |= DMA_DST_DEC;
					if(si == -4) tmpzahl |= DMADECR_SRC;
					else tmpzahl |= DMAINCR_SRC;
					
					DMAXCNT(2) = tmpzahl;
					
					SpecificIPCAlign->dma2_donearm9 = true;
					//SendMultipleWordACK(0xc1710005,0x0,0x0,0x0); //works
				}
                break;
                
            }
        }
        break;
        
        default:{
            //FIFO select
            switch(d&0x000000ff){
                //FIFO DMA A
                case 0xa0:{
                    if(transfer32) {
                        s &= 0xFFFFFFFC;
						int dmacnt = 0;
						int len = sizeof(u32);
						for(dmacnt = 0; dmacnt < ((int)INTERNAL_FIFO_SIZE/len); dmacnt++){
							int offset = 0;
							//if(di == -4) 
							if(si == -4) 	offset = (s-(dmacnt*len));
							else 			offset = (s+(dmacnt*len));
							
							u32 fifo_fetch = CPUReadMemory(offset);
							SpecificIPCAlign->fifodmasA[(dmacnt*len)+0] = (fifo_fetch)&0xff;
							SpecificIPCAlign->fifodmasA[(dmacnt*len)+1] = (fifo_fetch>>8)&0xff;
							SpecificIPCAlign->fifodmasA[(dmacnt*len)+2] = (fifo_fetch>>16)&0xff;
							SpecificIPCAlign->fifodmasA[(dmacnt*len)+3] = (fifo_fetch>>24)&0xff;
						}
					}
                    else{
                        s &= 0xFFFFFFFE;
						int dmacnt = 0;
						int len = sizeof(u16);
						for(dmacnt = 0; dmacnt < ((int)INTERNAL_FIFO_SIZE/len); dmacnt++){
							int offset = 0;
							//if(di == -4) 
							if(si == -4) 	offset = (s-(dmacnt*len));
							else 			offset = (s+(dmacnt*len));
							
							u16 fifo_fetch = CPUReadHalfWord(offset);
							SpecificIPCAlign->fifodmasA[(dmacnt*len)+0] = (fifo_fetch)&0xff;
							SpecificIPCAlign->fifodmasA[(dmacnt*len)+1] = (fifo_fetch>>8)&0xff;
						}
					}   
					SendMultipleWordACK(0xc1710004,0x0,0x0,0x0);
				}
                break;
                //FIFO DMA B
                case(0xa4):{
                    if(transfer32) {
                        s &= 0xFFFFFFFC;
						int dmacnt = 0;
						int len = sizeof(u32);
						for(dmacnt = 0; dmacnt < ((int)INTERNAL_FIFO_SIZE/len); dmacnt++){
							int offset = 0;
							//if(di == -4) 
							if(si == -4) 	offset = (s-(dmacnt*len));
							else 			offset = (s+(dmacnt*len));
							u32 fifo_fetch = CPUReadMemory(offset);
							
							SpecificIPCAlign->fifodmasB[(dmacnt*len)+0] = (fifo_fetch)&0xff;
							SpecificIPCAlign->fifodmasB[(dmacnt*len)+1] = (fifo_fetch>>8)&0xff;
							SpecificIPCAlign->fifodmasB[(dmacnt*len)+2] = (fifo_fetch>>16)&0xff;
							SpecificIPCAlign->fifodmasB[(dmacnt*len)+3] = (fifo_fetch>>24)&0xff;
						}
                    }
                    else{
                        s &= 0xFFFFFFFE;
						int dmacnt = 0;
						int len = sizeof(u16);
						for(dmacnt = 0; dmacnt < ((int)INTERNAL_FIFO_SIZE/len); dmacnt++){
							int offset = 0;
							//if(di == -4) 
							if(si == -4) 	offset = (s-(dmacnt*len));
							else 			offset = (s+(dmacnt*len));
							u16 fifo_fetch = CPUReadHalfWord(offset);
							
							SpecificIPCAlign->fifodmasB[(dmacnt*len)+0] = (fifo_fetch)&0xff;
							SpecificIPCAlign->fifodmasB[(dmacnt*len)+1] = (fifo_fetch>>8)&0xff;
						}
                    }
					SendMultipleWordACK(0xc1710005,0x0,0x0,0x0);
                }
                break;
                
            }
        }
        break;
    }
}



__attribute__((section(".itcm")))
void  __attribute__ ((hot)) CPUCheckDMA(int reason, int dmamask)
{
  // DMA 0
  if((SpecificIPCAlign->DM0CNT_H & 0x8000) && (dmamask & 1)) {
    if(((SpecificIPCAlign->DM0CNT_H >> 12) & 3) == reason) {
      u32 sourceIncrement = 4;
      u32 destIncrement = 4;
      switch((SpecificIPCAlign->DM0CNT_H >> 7) & 3) {
      case 0:
        break;
      case 1:
        sourceIncrement = (u32)-4;
        break;
      case 2:
        sourceIncrement = 0;
        break;
      }
      switch((SpecificIPCAlign->DM0CNT_H >> 5) & 3) {
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
        int count = (SpecificIPCAlign->DM0CNT_L ? SpecificIPCAlign->DM0CNT_L : 0x4000) << 1;
        if(SpecificIPCAlign->DM0CNT_H & 0x0400)
          count <<= 1;
        printf("DMA0: s=%08x d=%08x c=%04x count=%08x ", SpecificIPCAlign->dma0Source, SpecificIPCAlign->dma0Dest, 
            SpecificIPCAlign->DM0CNT_H,
            count);
      }
#endif
      doDMA(SpecificIPCAlign->dma0Source, SpecificIPCAlign->dma0Dest, sourceIncrement, destIncrement,
            SpecificIPCAlign->DM0CNT_L ? SpecificIPCAlign->DM0CNT_L : 0x4000,
            SpecificIPCAlign->DM0CNT_H & 0x0400);
      cpuDmaHack = true;
		
	//new
	//do DMA IRQ here
      SpecificIPCAlign->DM0CNT_H |= (1<<14);
	  
      if(SpecificIPCAlign->DM0CNT_H & 0x4000) {
        SpecificIPCAlign->GBA_IF |= 0x0100;
        UPDATE_REG(0x202, SpecificIPCAlign->GBA_IF);
        //cpuNextEvent = cpuTotalTicks;
      }
	//new end
      
      if(((SpecificIPCAlign->DM0CNT_H >> 5) & 3) == 3) {
        SpecificIPCAlign->dma0Dest = SpecificIPCAlign->DM0DAD_L | (SpecificIPCAlign->DM0DAD_H << 16);
      }
      
      if(!(SpecificIPCAlign->DM0CNT_H & 0x0200) || (reason == 0)) {
        SpecificIPCAlign->DM0CNT_H &= 0x7FFF;
        UPDATE_REG(0xBA, SpecificIPCAlign->DM0CNT_H);
      }
    }
  }
  
  // DMA 1
  if((SpecificIPCAlign->DM1CNT_H & 0x8000) && (dmamask & 2)) {
    if(((SpecificIPCAlign->DM1CNT_H >> 12) & 3) == reason) {
      u32 sourceIncrement = 4;
      u32 destIncrement = 4;
      switch((SpecificIPCAlign->DM1CNT_H >> 7) & 3) {
      case 0:
        break;
      case 1:
        sourceIncrement = (u32)-4;
        break;
      case 2:
        sourceIncrement = 0;
        break;
      }
      switch((SpecificIPCAlign->DM1CNT_H >> 5) & 3) {
      case 0:
        break;
      case 1:
        destIncrement = (u32)-4;
        break;
      case 2:
        destIncrement = 0;
        break;
      }
	  
		//DMA 1: Sound FIFO DMA
		if(reason == 3) {
#ifdef DEV_VERSION
			if(systemVerbose & VERBOSE_DMA1) {
				Log("DMA1: s=%08x d=%08x c=%04x count=%08x ", SpecificIPCAlign->dma1Source, SpecificIPCAlign->dma1Dest,SpecificIPCAlign->DM1CNT_H,16);
			}
#endif  
		
			//ori: doDMA(dma1Source, dma1Dest, sourceIncrement, 0, 4,0x0400);
			  
			//map redirect to FIFO
			doDMAFIFO(SpecificIPCAlign->dma1Source, SpecificIPCAlign->dma1Dest, sourceIncrement, 0, 4,0x0400);
			
		} 
		
		else {
#ifdef DEV_VERSION
			if(systemVerbose & VERBOSE_DMA1) {
				int count = (SpecificIPCAlign->DM1CNT_L ? SpecificIPCAlign->DM1CNT_L : 0x4000) << 1;
				if(SpecificIPCAlign->DM1CNT_H & 0x0400)
					count <<= 1;
				printf("DMA1: s=%08x d=%08x c=%04x count=%08x ", SpecificIPCAlign->dma1Source, SpecificIPCAlign->dma1Dest,SpecificIPCAlign->DM1CNT_H,count);
			}
#endif
			doDMA(SpecificIPCAlign->dma1Source, SpecificIPCAlign->dma1Dest, sourceIncrement, destIncrement,SpecificIPCAlign->DM1CNT_L ? SpecificIPCAlign->DM1CNT_L : 0x4000,SpecificIPCAlign->DM1CNT_H & 0x0400);
		}
      cpuDmaHack = true;

	//new 
      //do DMA IRQ here
      if(SpecificIPCAlign->DM1CNT_H & 0x4000) {
        SpecificIPCAlign->GBA_IF |= 0x200;
        UPDATE_REG(0x202, SpecificIPCAlign->GBA_IF);
        //cpuNextEvent = cpuTotalTicks;
      }
	  
	  //new end
      
      if(((SpecificIPCAlign->DM1CNT_H >> 5) & 3) == 3) {
        SpecificIPCAlign->dma1Dest = SpecificIPCAlign->DM1DAD_L | (SpecificIPCAlign->DM1DAD_H << 16);
      }
      
      if(!(SpecificIPCAlign->DM1CNT_H & 0x0200) || (reason == 0)) {
        SpecificIPCAlign->DM1CNT_H &= 0x7FFF;
        UPDATE_REG(0xC6, SpecificIPCAlign->DM1CNT_H);
      }
    }
  }
  
  // DMA 2
  if((SpecificIPCAlign->DM2CNT_H & 0x8000) && (dmamask & 4)) {
    if(((SpecificIPCAlign->DM2CNT_H >> 12) & 3) == reason) {
      u32 sourceIncrement = 4;
      u32 destIncrement = 4;
      switch((SpecificIPCAlign->DM2CNT_H >> 7) & 3) {
      case 0:
        break;
      case 1:
        sourceIncrement = (u32)-4;
        break;
      case 2:
        sourceIncrement = 0;
        break;
      }
      switch((SpecificIPCAlign->DM2CNT_H >> 5) & 3) {
      case 0:
        break;
      case 1:
        destIncrement = (u32)-4;
        break;
      case 2:
        destIncrement = 0;
        break;
      }      
	  
		//DMA2: Sound FIFO DMA
		if(reason == 3) {
#ifdef DEV_VERSION
			if(systemVerbose & VERBOSE_DMA2) {
				int count = (4) << 2;
				Log("DMA2: s=%08x d=%08x c=%04x count=%08x ", SpecificIPCAlign->dma2Source, SpecificIPCAlign->dma2Dest,SpecificIPCAlign->DM2CNT_H,count);
			}
#endif             
			//ori: doDMA(dma2Source, dma2Dest, sourceIncrement, 0, 4,0x0400);
			  
			//map redirect to FIFO
			doDMAFIFO(SpecificIPCAlign->dma2Source, SpecificIPCAlign->dma2Dest, sourceIncrement, 0, 4,0x0400);
		}
		else {
#ifdef DEV_VERSION
			if(systemVerbose & VERBOSE_DMA2) {
				int count = (SpecificIPCAlign->DM2CNT_L ? SpecificIPCAlign->DM2CNT_L : 0x4000) << 1;
				if(SpecificIPCAlign->DM2CNT_H & 0x0400)
					count <<= 1;
				printf("DMA2: s=%08x d=%08x c=%04x count=%08x ", SpecificIPCAlign->dma2Source, SpecificIPCAlign->dma2Dest,SpecificIPCAlign->DM2CNT_H,count);
			}
#endif                  
			doDMA(SpecificIPCAlign->dma2Source, SpecificIPCAlign->dma2Dest, sourceIncrement, destIncrement,SpecificIPCAlign->DM2CNT_L ? SpecificIPCAlign->DM2CNT_L : 0x4000,SpecificIPCAlign->DM2CNT_H & 0x0400);
		}
      cpuDmaHack = true;
		
		//new
		//do DMA IRQ here
		if(SpecificIPCAlign->DM2CNT_H & 0x4000) {
			SpecificIPCAlign->GBA_IF |= 0x400;
			UPDATE_REG(0x202, SpecificIPCAlign->GBA_IF);
			//cpuNextEvent = cpuTotalTicks;
		}
	
      if(((SpecificIPCAlign->DM2CNT_H >> 5) & 3) == 3) {
        SpecificIPCAlign->dma2Dest = SpecificIPCAlign->DM2DAD_L | (SpecificIPCAlign->DM2DAD_H << 16);
      }
      
      if(!(SpecificIPCAlign->DM2CNT_H & 0x0200) || (reason == 0)) {
        SpecificIPCAlign->DM2CNT_H &= 0x7FFF;
        UPDATE_REG(0xD2, SpecificIPCAlign->DM2CNT_H);
      }
    }
  }

	// DMA 3
	if((SpecificIPCAlign->DM3CNT_H & 0x8000) && (dmamask & 8)) {
		if(((SpecificIPCAlign->DM3CNT_H >> 12) & 3) == reason) {
			u32 sourceIncrement = 4;
			u32 destIncrement = 4;
			switch((SpecificIPCAlign->DM3CNT_H >> 7) & 3) {
				case 0:
				break;
				case 1:
					sourceIncrement = (u32)-4;
				break;
				case 2:
					sourceIncrement = 0;
				break;
			}
      
			switch((SpecificIPCAlign->DM3CNT_H >> 5) & 3) {
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
        int count = (SpecificIPCAlign->DM3CNT_L ? SpecificIPCAlign->DM3CNT_L : 0x10000) << 1;
        if(SpecificIPCAlign->DM3CNT_H & 0x0400)
          count <<= 1;
        printf("DMA3: s=%08x d=%08x c=%04x count=%08x ", SpecificIPCAlign->dma3Source, SpecificIPCAlign->dma3Dest,
            SpecificIPCAlign->DM3CNT_H,
            count);
      }
#endif                
		doDMA(SpecificIPCAlign->dma3Source, SpecificIPCAlign->dma3Dest, sourceIncrement, destIncrement,SpecificIPCAlign->DM3CNT_L ? SpecificIPCAlign->DM3CNT_L : 0x10000,SpecificIPCAlign->DM3CNT_H & 0x0400);
      
		//new
		//do DMA IRQ here
		SpecificIPCAlign->DM3CNT_H |= (1<<14);
      
		if(SpecificIPCAlign->DM3CNT_H & 0x4000) {
			SpecificIPCAlign->GBA_IF |= 0x0800;
			UPDATE_REG(0x202, SpecificIPCAlign->GBA_IF);
			//cpuNextEvent = cpuTotalTicks;
		}
		
		//new end
		
      if(((SpecificIPCAlign->DM3CNT_H >> 5) & 3) == 3) {
        SpecificIPCAlign->dma3Dest = SpecificIPCAlign->DM3DAD_L | (SpecificIPCAlign->DM3DAD_H << 16);
      }
      
      if(!(SpecificIPCAlign->DM3CNT_H & 0x0200) || (reason == 0)) {
        SpecificIPCAlign->DM3CNT_H &= 0x7FFF;
        UPDATE_REG(0xDE, SpecificIPCAlign->DM3CNT_H);
      }
    }
  }
}

void  __attribute__ ((hot)) CPUUpdateRegister(u32 address, u16 value)
{
  	/*if(0x60 > address && address > 0x7)
	{
			//printf("UP16 %x %x\r ",address,value);
		    *(u16 *)((address & 0x3FF) + 0x4000000) = value;
			//swiDelay(0x2000000);
			//swiDelay(0x2000000);
			//swiDelay(0x2000000);
			//swiDelay(0x2000000);
			
	}*/

  switch(address) {
  case 0x00:
    {	
		if(value != GBA_DISPCNT)
		{
			
			switch(value & 0x7){
			
				//In this mode, four text background layers can be shown. In this mode backgrounds 0 - 3 all count as "text" backgrounds, and cannot be scaled or rotated. 
				//Check out the section on text backgrounds for details on this. 
				case(0):case(2):{
					//reset GBA_BG3HOFS and GBA_BG3VOFS
					REG_BG3HOFS = GBA_BG3HOFS;
					REG_BG3VOFS = GBA_BG3VOFS;

					//reset
					REG_BG3CNT = GBA_BG3CNT;
					REG_BG2CNT = GBA_BG2CNT;
					REG_BLDCNT = GBA_BLDMOD;
					WIN_IN = GBA_WININ;
					WIN_OUT = GBA_WINOUT;

					REG_BG2PA = GBA_BG2PA;
					REG_BG2PB = GBA_BG2PB;
					REG_BG2PC = GBA_BG2PC;
					REG_BG2PD = GBA_BG2PD;
					REG_BG2X = (GBA_BG2X_L | (GBA_BG2X_H << 16));
					REG_BG2Y = (GBA_BG2Y_L | (GBA_BG2Y_H << 16));

					REG_BG3PA = GBA_BG3PA;
					REG_BG3PB = GBA_BG3PB;
					REG_BG3PC = GBA_BG3PC;
					REG_BG3PD = GBA_BG3PD;
					REG_BG3X = (GBA_BG3X_L | (GBA_BG3X_H << 16));
					REG_BG3Y = (GBA_BG3Y_L | (GBA_BG3Y_H << 16));
					
					u32 dsValue;
					dsValue  = value & 0xFF87;
					dsValue |= (value & (1 << 5)) ? (1 << 23) : 0;	/* oam hblank access */
					dsValue |= (value & (1 << 6)) ? (1 << 4) : 0;	/* obj mapping 1d/2d */
	#ifndef capture_and_pars
					dsValue |= (value & (1 << 7)) ? 0 : (1 << 16);	/* forced blank => no display mode (both)*/
	#endif
					REG_DISPCNT = dsValue; 				
					//mode 0 does not use rotscale registers
				}
				break;
				
				//coto: mode 1
				case(1):{
					//This mode is similar in most respects to Mode 0, the main difference being that only 3 backgrounds are accessible -- 0, 1, and 2. 
					//Bgs 0 and 1 are text backgrounds, while bg 2 is a rotation/scaling background.
					
					//rotscale/affine
					/*
					REG_BG2PA = GBA_BG2PA;
					REG_BG2PB = GBA_BG2PB;
					REG_BG2PC = GBA_BG2PC;
					REG_BG2PD = GBA_BG2PD;
					
					REG_BG2X = (GBA_BG2X_L | (GBA_BG2X_H << 16));
					REG_BG2Y = (GBA_BG2Y_L | (GBA_BG2Y_H << 16));
					*/
					//NDS PPU mode
							//bg0   //bg1     //bg2    //bg3
					//1     Text/3D  Text     Text     Affine
					
					//gba mode
					//2     Yes      --23   128x128..1024x1024 256   256/1        S-MABP
					
					/* //works but char tiles appear
					u32 dsValue = (value & 0xFF87) | (1<<11);           // //Set BG(x) enabled + NDS PPU like GBA PPU + enable BG3
					dsValue |= (value & (1 << 5)) ? (1 << 23) : 0;	// oam hblank access 
					dsValue |= (value & (1 << 6)) ? (1 << 4) : 0;	    // obj mapping 1d/2d
					dsValue |= (value & (1 << 7)) ? 0 : (1 << 16);	// forced blank => no display mode (both)
					
					//dsValue |= DISPLAY_CHAR_BASE(1);                    //set purposely engine's tilebase to 0x06020000 , unused memory so it doesnt override the actual tiles copied over dma
					REG_DISPCNT = dsValue; 
					
					int charbase = (GBA_BG2CNT>>2)&0x3;
					int mapbase = (GBA_BG2CNT>>8)&0x1f;
					
					REG_BG3CNT = (GBA_BG2CNT&0x3) | (charbase<<2) | (mapbase<<8) | (((GBA_BG2CNT>>14)&0x3)<<14);
					*/
					
					int charbase = (GBA_BG2CNT>>2)&0x3;
					int mapbase = (GBA_BG2CNT>>8)&0x1f;
					
					REG_BG3CNT = (GBA_BG2CNT&0x3) | (charbase<<2) | (mapbase<<8) | (((GBA_BG2CNT>>14)&0x3)<<14);
					
					u32 dsValue = (value & 0xFF80) | (1<<11) | MODE_1_2D;           // //Set BG(x) enabled + NDS PPU like GBA PPU + enable BG3
					dsValue |= (value & (1 << 5)) ? (1 << 23) : 0;	// oam hblank access 
					dsValue |= (value & (1 << 6)) ? (1 << 4) : 0;	    // obj mapping 1d/2d
					dsValue |= (value & (1 << 7)) ? 0 : (1 << 16);	// forced blank => no display mode (both)
					
					REG_DISPCNT = dsValue; 
				}
				break;
				
				case(3):case(4):case(5):{
					//mode 3,4,5
					if((value & 0xFFEF) != (GBA_DISPCNT & 0xFFEF))
					{

						u32 dsValue;
						dsValue  = value & 0xF087;
						dsValue |= (value & (1 << 5)) ? (1 << 23) : 0;	/* oam hblank access */
						dsValue |= (value & (1 << 6)) ? (1 << 4) : 0;	/* obj mapping 1d/2d */
		#ifndef capture_and_pars
						dsValue |= (value & (1 << 7)) ? 0 : (1 << 16);	/* forced blank => no display mode (both)*/
		#endif
						REG_DISPCNT = (dsValue | (1<<11)); //enable BG3
						if((GBA_DISPCNT & 7) != (value & 7))
						{
							if((value & 7) == 4)
							{
								//bgInit_call(3, BgType_Bmp8, BgSize_B8_256x256,8,8);
								REG_BG3CNT = BG_MAP_BASE(/*mapBase*/8) | BG_TILE_BASE(/*tileBase*/8) | BgSize_B8_256x256;
							}
							else 
							{
								//bgInit_call(3, BgType_Bmp16, BgSize_B16_256x256,8,8);
								REG_BG3CNT = BG_MAP_BASE(/*mapBase*/8) | BG_TILE_BASE(/*tileBase*/8) | BgSize_B16_256x256;
							}
							if((GBA_DISPCNT & 7) < 3)
							{
								//reset GBA_BG3HOFS and GBA_BG3VOFS
								REG_BG3HOFS = 0;
								REG_BG3VOFS = 0;

								//BLDCNT(2 enabeled bits)
								int tempBLDMOD = GBA_BLDMOD & ~0x404;
								tempBLDMOD = tempBLDMOD | ((GBA_BLDMOD & 0x404) << 1);
								REG_BLDCNT = tempBLDMOD;

								//GBA_WINOUT(2 enabeled bits)
								int tempWINOUT = GBA_WINOUT & ~0x404;
								tempWINOUT = tempWINOUT | ((GBA_WINOUT & 0x404) << 1);
								WIN_OUT = tempWINOUT;

								//GBA_WININ(2 enabeled bits)
								int tempWININ = GBA_WININ & ~0x404;
								tempWININ = tempWININ | ((GBA_WININ & 0x404) << 1);
								WIN_IN = tempWININ;

								//swap LCD I/O BG Rotation/Scaling

								REG_BG3PA = GBA_BG2PA;
								REG_BG3PB = GBA_BG2PB;
								REG_BG3PC = GBA_BG2PC;
								REG_BG3PD = GBA_BG2PD;
								REG_BG3X = (GBA_BG2X_L | (GBA_BG2X_H << 16));
								REG_BG3Y = (GBA_BG2Y_L | (GBA_BG2Y_H << 16));
								REG_BG3CNT = REG_BG3CNT | (GBA_BG2CNT & 0x43); //swap GBA_BG2CNT (BG Priority and Mosaic) 
							}
						}
					}
				}
				break;
			}
		}
		
		GBA_DISPCNT = value & 0xFFF7;
		UPDATE_REG(0x00, GBA_DISPCNT);
    }
    break;
	
  case 0x04:
    GBA_DISPSTAT = (value & 0xFF38) | (GBA_DISPSTAT & 7) | (VCOUNT_LINE_INTERRUPT << 15);
    UPDATE_REG(0x04, GBA_DISPSTAT);

#ifdef usebuffedVcout
	{
	u16 tempDISPSTAT = (u16)((GBA_DISPSTAT&0xFF) | ((VCountgbatods[GBA_DISPSTAT>>8]) << 8));
#else
	//8-15  V-Count Setting (LYC)      (0..227)
	{
	u16 tempDISPSTAT = GBA_DISPSTAT >> 8;
		//float help = tempDISPSTAT;
		if(tempDISPSTAT < 160)
		{
			tempDISPSTAT = (tempDISPSTAT * 306);//tempDISPSTAT = (help * 1.2);
			tempDISPSTAT = tempDISPSTAT | (GBA_DISPSTAT & 0xFF); //1.15350877; //already seeked
			//help3 = (help + 1) * (1./1.2); //1.15350877;  // ichfly todo it is to slow
		}
		else
		{
			if(tempDISPSTAT < 220)
			{
				tempDISPSTAT = ((tempDISPSTAT - 160) * 266);//tempDISPSTAT = ((help - 160) * 1.04411764);//tempDISPSTAT = ((help - 160) * 1.04411764);
				tempDISPSTAT = (tempDISPSTAT + 192 * 0x100) | (GBA_DISPSTAT & 0xFF); //1.15350877;
			}
			else if(tempDISPSTAT < 228)
			{
				tempDISPSTAT = (GBA_DISPSTAT & 0x3F) | 0xFF00;
			}
			else tempDISPSTAT = (GBA_DISPSTAT & 0x1F);		
		}
		
#endif
#ifdef forceHBlankirqs
	*(u16 *)(0x4000004) = tempDISPSTAT | (1<<4);
#else
	*(u16 *)(0x4000004) = tempDISPSTAT;
#endif
	}
	
	
    break;
  case 0x06:
    // not writable in NDS mode bzw not possible todo
    break;
  case 0x08:
    GBA_BG0CNT = (value & 0xDFCF);
    UPDATE_REG(0x08, GBA_BG0CNT);
	*(u16 *)(0x4000008) = GBA_BG0CNT;
    break;
  case 0x0A:
    GBA_BG1CNT = (value & 0xDFCF);
    UPDATE_REG(0x0A, GBA_BG1CNT);
    *(u16 *)(0x400000A) = GBA_BG1CNT;
	break;
  case 0x0C:
    GBA_BG2CNT = (value & 0xFFCF);
    UPDATE_REG(0x0C, GBA_BG2CNT);
	
	//ichfly
    /*
    if((GBA_DISPCNT & 7) < 3){
        
        *(u16 *)(0x400000C) = GBA_BG2CNT;
    }
    else //ichfly some extra handling 
	{
		REG_BG3CNT = REG_BG3CNT | (GBA_BG2CNT & 0x43);
	}
    */
    
    //coto
    switch(REG_DISPCNT & 7){
        case(0):case(2):{
            *(u16 *)(0x400000C) = GBA_BG2CNT;
        }
        break;
        
        case(1):{
            *(u16 *)(0x400000C) = 3;        //NDS BG2 is hidden since
            *(u16 *)(0x400000E) = GBA_BG2CNT;   //we treat BG3 as BG2
        }
        break;
        
        case(3):case(4):case(5):{
            REG_BG3CNT = REG_BG3CNT | (GBA_BG2CNT & 0x43);
        }
        break;
        
    }
    break;
  case 0x0E:
    GBA_BG3CNT = (value & 0xFFCF);
    UPDATE_REG(0x0E, GBA_BG3CNT);
	
	//if((GBA_DISPCNT & 7) < 3)*(u16 *)(0x400000E) = GBA_BG3CNT;
    
    //coto
    switch(REG_DISPCNT & 7){
        case(0):case(2):{
            *(u16 *)(0x400000E) = GBA_BG3CNT;
        }
        break;
        
        case(1):{
			//unused
        }
        break;
    }
	
	break;
  case 0x10:
    GBA_BG0HOFS = value & 511;
    UPDATE_REG(0x10, GBA_BG0HOFS);
    *(u16 *)(0x4000010) = value;
	break;
  case 0x12:
    GBA_BG0VOFS = value & 511;
    UPDATE_REG(0x12, GBA_BG0VOFS);
    *(u16 *)(0x4000012) = value;
	break;
  case 0x14:
    GBA_BG1HOFS = value & 511;
    UPDATE_REG(0x14, GBA_BG1HOFS);
	*(u16 *)(0x4000014) = value;
    break;
  case 0x16:
    GBA_BG1VOFS = value & 511;
    UPDATE_REG(0x16, GBA_BG1VOFS);
    *(u16 *)(0x4000016) = value;
	break;      
  case 0x18:
    GBA_BG2HOFS = value & 511;
    UPDATE_REG(0x18, GBA_BG2HOFS);
	
	//*(u16 *)(0x4000018) = value;
    
	//coto
    switch(REG_DISPCNT & 7){
        case(0):case(2):case(3):case(4):case(5):{
            *(u16 *)(0x4000018) = value;
        }
        break;
        
        case(1):{
            *(u16 *)(0x4000018) = 0;
           *(u16 *)(0x400001C) = value; //bg3 nds hw map
        }
        break;
    }
	
	break;
  case 0x1A:
    GBA_BG2VOFS = value & 511;
    UPDATE_REG(0x1A, GBA_BG2VOFS);
    //if((GBA_DISPCNT & 7) < 3)*(u16 *)(0x400001A) = value; //ichfly only update if it is save
	
	//coto
    switch(REG_DISPCNT & 7){
        case(0):case(2):{
            *(u16 *)(0x400001A) = value;
        }
        break;
        
        case(1):{
            *(u16 *)(0x400001A) = 0;
           *(u16 *)(0x400001E) = value; //bg3 nds hw map
        }
        break;
    }
	
	break;
  case 0x1C:
    GBA_BG3HOFS = value & 511;
    UPDATE_REG(0x1C, GBA_BG3HOFS);
    //if((GBA_DISPCNT & 7) < 3)*(u16 *)(0x400001C) = value; //ichfly only update if it is save
	//coto
    switch(REG_DISPCNT & 7){
        case(0):case(2):{
            *(u16 *)(0x400001C) = value;
        }
        break;
        
        case(1):{
           //unused
        }
        break;
    }
	
	break;
  case 0x1E:
    GBA_BG3VOFS = value & 511;
    UPDATE_REG(0x1E, GBA_BG3VOFS);
    //*(u16 *)(0x400001E) = value;
	//coto
    switch(REG_DISPCNT & 7){
        case(0):case(2):case(3):case(4):case(5):{
            *(u16 *)(0x400001E) = value;
        }
        break;
        
        case(1):{
           //unused
        }
        break;
    }
	break;      
  case 0x20:
    GBA_BG2PA = value;
    UPDATE_REG(0x20, GBA_BG2PA);
    //if((GBA_DISPCNT & 7) < 3)*(u16 *)(0x4000020) = value;
	//else *(u16 *)(0x4000030) = value;
	//coto
    switch(REG_DISPCNT & 7){
        case(0):case(2):{
            *(u16 *)(0x4000020) = value;
        }
        break;
        
        case(1):{
            *(u16 *)(0x4000020) = 0;
           *(u16 *)(0x4000030) = value;
        }
        break;
        case(3):case(4):case(5):{
            *(u16 *)(0x4000030) = value;
        }
        break;
    }
	break;
  case 0x22:
    GBA_BG2PB = value;
    UPDATE_REG(0x22, GBA_BG2PB);
    //if((GBA_DISPCNT & 7) < 3)*(u16 *)(0x4000022) = value;
	//else *(u16 *)(0x4000032) = value;
	//coto
    switch(REG_DISPCNT & 7){
        case(0):case(2):{
            *(u16 *)(0x4000022) = value;
        }
        break;
        
        case(1):{
            *(u16 *)(0x4000022) = 0;
           *(u16 *)(0x4000032) = value;
        }
        break;
        case(3):case(4):case(5):{
            *(u16 *)(0x4000032) = value;
        }
        break;
    }
	break;
  case 0x24:
    GBA_BG2PC = value;
    UPDATE_REG(0x24, GBA_BG2PC);
    //if((GBA_DISPCNT & 7) < 3)*(u16 *)(0x4000024) = value;
	//else *(u16 *)(0x4000034) = value;
	//coto
    switch(REG_DISPCNT & 7){
        case(0):case(2):{
            *(u16 *)(0x4000024) = value;
        }
        break;
        
        case(1):{
            *(u16 *)(0x4000024) = 0;
           *(u16 *)(0x4000034) = value;
        }
        break;
        case(3):case(4):case(5):{
            *(u16 *)(0x4000034) = value;
        }
        break;
    }
	break;
  case 0x26:
    GBA_BG2PD = value;
    UPDATE_REG(0x26, GBA_BG2PD);
	//if((GBA_DISPCNT & 7) < 3)*(u16 *)(0x4000026) = value;
	//else *(u16 *)(0x4000036) = value;
	//coto
    switch(REG_DISPCNT & 7){
        case(0):case(2):{
            *(u16 *)(0x4000026) = value;
        }
        break;
        
        case(1):{
            *(u16 *)(0x4000026) = 0;
           *(u16 *)(0x4000036) = value;
        }
        break;
        case(3):case(4):case(5):{
            *(u16 *)(0x4000036) = value;
        }
        break;
    }
	break;
  case 0x28:
    GBA_BG2X_L = value;
    UPDATE_REG(0x28, GBA_BG2X_L);
    //gfxBG2Changed |= 1;
	//if((GBA_DISPCNT & 7) < 3)*(u16 *)(0x4000028) = value;
	//else *(u16 *)(0x4000038) = value;
    
	//4000020h - GBA_BG2PA - BG2 Rotation/Scaling Parameter A (alias dx) (W)
    //4000022h - GBA_BG2PB - BG2 Rotation/Scaling Parameter B (alias dmx) (W)
    //4000024h - GBA_BG2PC - BG2 Rotation/Scaling Parameter C (alias dy) (W)
    //4000026h - GBA_BG2PD - BG2 Rotation/Scaling Parameter D (alias dmy) (W)

    //4000028h - GBA_BG2X_L - BG2 Reference Point X-Coordinate, lower 16 bit (W)
    //400002Ah - GBA_BG2X_H - BG2 Reference Point X-Coordinate, upper 12 bit (W)
    //400002Ch - GBA_BG2Y_L - BG2 Reference Point Y-Coordinate, lower 16 bit (W)
    //400002Eh - GBA_BG2Y_H - BG2 Reference Point Y-Coordinate, upper 12 bit (W)
    
    //coto
    switch(REG_DISPCNT & 7){
        case(0):case(2):{
            *(u16 *)(0x4000028) = value;
        }
        break;
        
        case(1):{
            *(u16 *)(0x4000028) = 0;
            *(u16 *)(0x4000038) = value;    //engine 3
        }
        break;
        case(3):case(4):case(5):{
            *(u16 *)(0x4000038) = value;
        }
        break;
    }
	break;
  case 0x2A:
    GBA_BG2X_H = (value & 0xFFF);
    UPDATE_REG(0x2A, GBA_BG2X_H);
    //gfxBG2Changed |= 1;
	//if((GBA_DISPCNT & 7) < 3)*(u16 *)(0x400002A) = value;
	//else *(u16 *)(0x400003A) = value;
    
	//coto
    switch(REG_DISPCNT & 7){
        case(0):case(2):{
            *(u16 *)(0x400002A) = value;
        }
        break;
        
        case(1):{
            *(u16 *)(0x400002A) = 0;
            *(u16 *)(0x400003A) = value;    //engine 3
            
        }
        break;
        case(3):case(4):case(5):{
            *(u16 *)(0x400003A) = value;
        }
        break;
    }
	break;
  case 0x2C:
    GBA_BG2Y_L = value;
    UPDATE_REG(0x2C, GBA_BG2Y_L);
    //gfxBG2Changed |= 2;
	//if((GBA_DISPCNT & 7) < 3)*(u16 *)(0x400002C) = value;
	//else *(u16 *)(0x400003C) = value;
    
	//coto
    switch(REG_DISPCNT & 7){
        case(0):case(2):{
            *(u16 *)(0x400002C) = value;
        }
        break;
        
        case(1):{
            *(u16 *)(0x400002C) = 0;
            *(u16 *)(0x400003C) = value;    //engine 3
        }
        break;
        case(3):case(4):case(5):{
            *(u16 *)(0x400003C) = value;
        }
        break;
    }
	break;
  case 0x2E:
    GBA_BG2Y_H = value & 0xFFF;
    UPDATE_REG(0x2E, GBA_BG2Y_H);
    //gfxBG2Changed |= 2;
	//if((GBA_DISPCNT & 7) < 3)*(u16 *)(0x400002E) = value;
	//else *(u16 *)(0x400003E) = value;
    
	//coto
    switch(REG_DISPCNT & 7){
        case(0):case(2):{
            *(u16 *)(0x400002E) = value;
        }
        break;
        
        case(1):{
            *(u16 *)(0x400002E) = 0;
            *(u16 *)(0x400003E) = value;    //engine 3
        }
        break;
        case(3):case(4):case(5):{
            *(u16 *)(0x400003E) = value;
        }
        break;
    }
	break;
  case 0x30:
    GBA_BG3PA = value;
    UPDATE_REG(0x30, GBA_BG3PA);
    //if((GBA_DISPCNT & 7) < 3)*(u16 *)(0x4000030) = value;
	
	//coto
    switch(REG_DISPCNT & 7){
        case(0):case(2):{
            *(u16 *)(0x4000030) = value;
        }
        break;
        
        case(1):{
            //engine 3 in mode 1 writes are disabled
        }
    }
	
	break;
  case 0x32:
    GBA_BG3PB = value;
    UPDATE_REG(0x32, GBA_BG3PB);
    //if((GBA_DISPCNT & 7) < 3)*(u16 *)(0x4000032) = value;
	//coto
    switch(REG_DISPCNT & 7){
        case(0):case(2):{
            *(u16 *)(0x4000032) = value;
        }
        break;
        
        case(1):{
            //engine 3 in mode 1 writes are disabled
        }
    }
	break;
  case 0x34:
    GBA_BG3PC = value;
    UPDATE_REG(0x34, GBA_BG3PC);
    //if((GBA_DISPCNT & 7) < 3)*(u16 *)(0x4000034) = value;
	//coto
    switch(REG_DISPCNT & 7){
        case(0):case(2):{
            *(u16 *)(0x4000034) = value;
        }
        break;
        
        case(1):{
            //engine 3 in mode 1 writes are disabled
        }
    }
	break;
  case 0x36:
    GBA_BG3PD = value;
    UPDATE_REG(0x36, GBA_BG3PD);
    //if((GBA_DISPCNT & 7) < 3)*(u16 *)(0x4000036) = value;
	//coto
    switch(REG_DISPCNT & 7){
        case(0):case(2):{
            *(u16 *)(0x4000036) = value;
        }
        break;
        
        case(1):{
            //engine 3 in mode 1 writes are disabled
        }
    }
	break;
  case 0x38:
    GBA_BG3X_L = value;
    UPDATE_REG(0x38, GBA_BG3X_L);
    //gfxBG3Changed |= 1;
    //if((GBA_DISPCNT & 7) < 3)*(u16 *)(0x4000038) = value;
	//coto
    switch(REG_DISPCNT & 7){
        case(0):case(2):{
            *(u16 *)(0x4000038) = value;
        }
        break;
        
        case(1):{
            //engine 3 in mode 1 writes are disabled
        }
    }
	break;
  case 0x3A:
    GBA_BG3X_H = value & 0xFFF;
    UPDATE_REG(0x3A, GBA_BG3X_H);
    //gfxBG3Changed |= 1;    
    //if((GBA_DISPCNT & 7) < 3)*(u16 *)(0x400003A) = value;
	//coto
    switch(REG_DISPCNT & 7){
        case(0):case(2):{
            *(u16 *)(0x400003A) = value;
        }
        break;
        
        case(1):{
            //engine 3 in mode 1 writes are disabled
        }
    }
	break;
  case 0x3C:
    GBA_BG3Y_L = value;
    UPDATE_REG(0x3C, GBA_BG3Y_L);
    //gfxBG3Changed |= 2;    
    //if((GBA_DISPCNT & 7) < 3)*(u16 *)(0x400003C) = value;
	//coto
    switch(REG_DISPCNT & 7){
        case(0):case(2):{
            *(u16 *)(0x400003C) = value;
        }
        break;
        
        case(1):{
            //engine 3 in mode 1 writes are disabled
        }
    }
	break;
  case 0x3E:
    GBA_BG3Y_H = value & 0xFFF;
    UPDATE_REG(0x3E, GBA_BG3Y_H);
    //gfxBG3Changed |= 2;    
    //if((GBA_DISPCNT & 7) < 3)*(u16 *)(0x400003E) = value;
	//coto
    switch(REG_DISPCNT & 7){
        case(0):case(2):{
            *(u16 *)(0x400003E) = value;
        }
        break;
        
        case(1):{
            //engine 3 in mode 1 writes are disabled
        }
    }
	break;
  case 0x40:
    GBA_WIN0H = value;
    UPDATE_REG(0x40, GBA_WIN0H);
    //CPUUpdateWindow0();
    *(u16 *)(0x4000040) = value;
	break;
  case 0x42:
    GBA_WIN1H = value;
    UPDATE_REG(0x42, GBA_WIN1H);
	*(u16 *)(0x4000042) = value;
    //CPUUpdateWindow1();    
    break;      
  case 0x44:
    GBA_WIN0V = value;
    UPDATE_REG(0x44, GBA_WIN0V);
    *(u16 *)(0x4000044) = value;
	break;
  case 0x46:
    GBA_WIN1V = value;
    UPDATE_REG(0x46, GBA_WIN1V);
    *(u16 *)(0x4000046) = value;
	break;
  case 0x48:
    GBA_WININ = value & 0x3F3F;
    UPDATE_REG(0x48, GBA_WININ);
    if((GBA_DISPCNT & 7) < 3)*(u16 *)(0x4000048) = value;
	else
	{
		int tempWININ = GBA_WININ & ~0x404;
		tempWININ = tempWININ | ((GBA_WININ & 0x404) << 1);
		WIN_IN = tempWININ;
	}
	break;
  case 0x4A:
    GBA_WINOUT = value & 0x3F3F;
    UPDATE_REG(0x4A, GBA_WINOUT);
    if((GBA_DISPCNT & 7) < 3)*(u16 *)(0x400004A) = value;
	else
	{
		int tempWINOUT = GBA_WINOUT & ~0x404;
		tempWINOUT = tempWINOUT | ((GBA_WINOUT & 0x404) << 1);
		WIN_OUT = tempWINOUT;
	}
	break;
  case 0x4C:
    GBA_MOSAIC = value;
    UPDATE_REG(0x4C, GBA_MOSAIC);
    *(u16 *)(0x400004C) = value;
	break;
  case 0x50:
    GBA_BLDMOD = value & 0x3FFF;
    UPDATE_REG(0x50, GBA_BLDMOD);
    //fxOn = ((GBA_BLDMOD>>6)&3) != 0;
    //CPUUpdateRender();
	if((GBA_DISPCNT & 7) < 3)*(u16 *)(0x4000050) = value;
	else
	{
		int tempBLDMOD = GBA_BLDMOD & ~0x404;
		tempBLDMOD = tempBLDMOD | ((GBA_BLDMOD & 0x404) << 1);
		REG_BLDCNT = tempBLDMOD;
	}
    break;
  case 0x52:
    GBA_COLEV = value & 0x1F1F;
    UPDATE_REG(0x52, GBA_COLEV);
    *(u16 *)(0x4000052) = value;
	break;
  case 0x54:
    GBA_COLY = value & 0x1F;
    UPDATE_REG(0x54, GBA_COLY);
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
  case 0x90:
  case 0x92:
  case 0x94:
  case 0x96:
  case 0x98:
  case 0x9a:
  case 0x9c:
  case 0x9e:    
  
  //FIFO
  case 0xa0:
  case 0xa2:
  case 0xa4:
  case 0xa6:
  
    //soundEvent(address&0xFF, value);  //ichfly send sound to arm7
#ifdef soundwriteprint
	  printf("ur %04x to %08x\r ",value,address);
#endif
#ifdef arm9advsound
	  SendMultipleWordACK(0xc3730000,(u32)address,(u32)value,0x0);
#endif
	  UPDATE_REG(address,value);
    break;
	
	
//DMA
  case 0xB0:
    SpecificIPCAlign->DM0SAD_L = value;
    UPDATE_REG(0xB0, SpecificIPCAlign->DM0SAD_L);
    break;
  case 0xB2:
    SpecificIPCAlign->DM0SAD_H = value & 0x07FF;
    UPDATE_REG(0xB2, SpecificIPCAlign->DM0SAD_H);
    break;
  case 0xB4:
    SpecificIPCAlign->DM0DAD_L = value;
    UPDATE_REG(0xB4, SpecificIPCAlign->DM0DAD_L);
    break;
  case 0xB6:
    SpecificIPCAlign->DM0DAD_H = value & 0x07FF;
    UPDATE_REG(0xB6, SpecificIPCAlign->DM0DAD_H);
    break;
  case 0xB8:
    SpecificIPCAlign->DM0CNT_L = value & 0x3FFF;
    UPDATE_REG(0xB8, 0);
    break;
  case 0xBA:
    {
      bool start = ((SpecificIPCAlign->DM0CNT_H ^ value) & 0x8000) ? true : false;
      value &= 0xF7E0;

      SpecificIPCAlign->DM0CNT_H = value;
      UPDATE_REG(0xBA, SpecificIPCAlign->DM0CNT_H);    
    
      if(start && (value & 0x8000)) {
        SpecificIPCAlign->dma0Source = SpecificIPCAlign->DM0SAD_L | (SpecificIPCAlign->DM0SAD_H << 16);
        SpecificIPCAlign->dma0Dest = SpecificIPCAlign->DM0DAD_L | (SpecificIPCAlign->DM0DAD_H << 16);
        CPUCheckDMA(0, 1);
      }
    }
    break;      

  case 0xBC:
#ifdef dmawriteprint
	printf("ur %04x to %08x\r ",value,address);
#endif
#ifdef arm9advsound
	SendMultipleWordACK(0xc3730000,(u32)address,(u32)value,0x0);
	
#endif
    
	SpecificIPCAlign->DM1SAD_L = value;
    UPDATE_REG(0xBC, SpecificIPCAlign->DM1SAD_L);
    break;

  case 0xBE:
#ifdef dmawriteprint
	printf("ur %04x to %08x\r ",value,address);
#endif
#ifdef arm9advsound
	SendMultipleWordACK(0xc3730000,(u32)address,(u32)value,0x0);
#endif

    SpecificIPCAlign->DM1SAD_H = value & 0x0FFF;
    UPDATE_REG(0xBE, SpecificIPCAlign->DM1SAD_H);
    break;

  case 0xC0:
#ifdef dmawriteprint
    printf("ur %04x to %08x\r ",value,address);
#endif
#ifdef arm9advsound
	SendMultipleWordACK(0xc3730000,(u32)address,(u32)value,0x0);
#endif

	SpecificIPCAlign->DM1DAD_L = value;
    UPDATE_REG(0xC0, SpecificIPCAlign->DM1DAD_L);
    break;

  case 0xC2:
#ifdef dmawriteprint
	printf("ur %04x to %08x\r ",value,address);
#endif
#ifdef arm9advsound
	SendMultipleWordACK(0xc3730000,(u32)address,(u32)value,0x0);
#endif

    SpecificIPCAlign->DM1DAD_H = value & 0x07FF;
    UPDATE_REG(0xC2, SpecificIPCAlign->DM1DAD_H);
    break;

  case 0xC4:
#ifdef dmawriteprint
	printf("ur %04x to %08x\r ",value,address);
#endif
#ifdef arm9advsound
	SendMultipleWordACK(0xc3730000,(u32)address,(u32)value,0x0);
#endif

	SpecificIPCAlign->DM1CNT_L = value & 0x3FFF;
    UPDATE_REG(0xC4, 0);
    break;

  case 0xC6:
#ifdef dmawriteprint
	printf("ur %04x to %08x\r ",value,address);
#endif
#ifdef arm9advsound	
	SendMultipleWordACK(0xc3730000,(u32)address,(u32)value,0x0);
#endif
	  {
      bool start = ((SpecificIPCAlign->DM1CNT_H ^ value) & 0x8000) ? true : false;
      value &= 0xF7E0;
      
      SpecificIPCAlign->DM1CNT_H = value;
      UPDATE_REG(0xC6, SpecificIPCAlign->DM1CNT_H);
      
      if(start && (value & 0x8000)) {
        SpecificIPCAlign->dma1Source = SpecificIPCAlign->DM1SAD_L | (SpecificIPCAlign->DM1SAD_H << 16);
        SpecificIPCAlign->dma1Dest = SpecificIPCAlign->DM1DAD_L | (SpecificIPCAlign->DM1DAD_H << 16);
        CPUCheckDMA(0, 2);
      }
    }
    break;
  case 0xC8:
#ifdef dmawriteprint
	printf("ur %04x to %08x\r ",value,address);
#endif
#ifdef arm9advsound
	SendMultipleWordACK(0xc3730000,(u32)address,(u32)value,0x0);
#endif

	SpecificIPCAlign->DM2SAD_L = value;
    UPDATE_REG(0xC8, SpecificIPCAlign->DM2SAD_L);
    break;
  case 0xCA:
#ifdef dmawriteprint
	printf("ur %04x to %08x\r ",value,address);
#endif
#ifdef arm9advsound
	SendMultipleWordACK(0xc3730000,(u32)address,(u32)value,0x0);
#endif

	SpecificIPCAlign->DM2SAD_H = value & 0x0FFF;
    UPDATE_REG(0xCA, SpecificIPCAlign->DM2SAD_H);
    break;
  case 0xCC:
#ifdef dmawriteprint
	printf("ur %04x to %08x\r ",value,address);
#endif
#ifdef arm9advsound
	SendMultipleWordACK(0xc3730000,(u32)address,(u32)value,0x0);
#endif

	SpecificIPCAlign->DM2DAD_L = value;
    UPDATE_REG(0xCC, SpecificIPCAlign->DM2DAD_L);
    break;
  case 0xCE:
#ifdef dmawriteprint
	printf("ur %04x to %08x\r ",value,address);
#endif
#ifdef arm9advsound
	SendMultipleWordACK(0xc3730000,(u32)address,(u32)value,0x0);
#endif

	SpecificIPCAlign->DM2DAD_H = value & 0x07FF;
    UPDATE_REG(0xCE, SpecificIPCAlign->DM2DAD_H);
    break;

  case 0xD0:
#ifdef dmawriteprint

	printf("ur %04x to %08x\r ",value,address);
#endif
#ifdef arm9advsound
	SendMultipleWordACK(0xc3730000,(u32)address,(u32)value,0x0);
#endif

	SpecificIPCAlign->DM2CNT_L = value & 0x3FFF;
    UPDATE_REG(0xD0, 0);
    break;

  case 0xD2:
#ifdef dmawriteprint

	printf("ur %04x to %08x\r ",value,address);
#endif
#ifdef arm9advsound
	SendMultipleWordACK(0xc3730000,(u32)address,(u32)value,0x0);
#endif

	  {
      bool start = ((SpecificIPCAlign->DM2CNT_H ^ value) & 0x8000) ? true : false;
      
      value &= 0xF7E0;
      
      SpecificIPCAlign->DM2CNT_H = value;
      UPDATE_REG(0xD2, SpecificIPCAlign->DM2CNT_H);
      
      if(start && (value & 0x8000)) {
        SpecificIPCAlign->dma2Source = SpecificIPCAlign->DM2SAD_L | (SpecificIPCAlign->DM2SAD_H << 16);
        SpecificIPCAlign->dma2Dest = SpecificIPCAlign->DM2DAD_L | (SpecificIPCAlign->DM2DAD_H << 16);

        CPUCheckDMA(0, 4);
      }            
    }
    break;

  case 0xD4:
    SpecificIPCAlign->DM3SAD_L = value;
    UPDATE_REG(0xD4, SpecificIPCAlign->DM3SAD_L);
    break;

  case 0xD6:
    SpecificIPCAlign->DM3SAD_H = value & 0x0FFF;
    UPDATE_REG(0xD6, SpecificIPCAlign->DM3SAD_H);
    break;
  case 0xD8:
    SpecificIPCAlign->DM3DAD_L = value;
    UPDATE_REG(0xD8, SpecificIPCAlign->DM3DAD_L);
    break;
  case 0xDA:
    SpecificIPCAlign->DM3DAD_H = value & 0x0FFF;
    UPDATE_REG(0xDA, SpecificIPCAlign->DM3DAD_H);
    break;

  case 0xDC:
    SpecificIPCAlign->DM3CNT_L = value;
    UPDATE_REG(0xDC, 0);
    break;

  case 0xDE:
    {
      bool start = ((SpecificIPCAlign->DM3CNT_H ^ value) & 0x8000) ? true : false;

      value &= 0xFFE0;

      SpecificIPCAlign->DM3CNT_H = value;
      UPDATE_REG(0xDE, SpecificIPCAlign->DM3CNT_H);
    
      if(start && (value & 0x8000)) {
        SpecificIPCAlign->dma3Source = SpecificIPCAlign->DM3SAD_L | (SpecificIPCAlign->DM3SAD_H << 16);
        SpecificIPCAlign->dma3Dest = SpecificIPCAlign->DM3DAD_L | (SpecificIPCAlign->DM3DAD_H << 16);
        CPUCheckDMA(0,8);
      }
    }
    break;
	
	
	//Timers
	
	
	//TM0CNT_L
 case 0x100:
    //SpecificIPCAlign->timer0Reload = value;
#ifdef printsoundtimer
	printf("ur %04x to %08x\r ",value,address);
#endif
#ifdef arm9advsound
	SendMultipleWordACK(0xc3730000,(u32)address,(u32)value,0x0);
#endif
	
	*(u16 *)(0x4000100) = value;
    
	UPDATE_REG(0x100, value);
    break;
	
	
	//TM0CNT_H
  case 0x102:
    //SpecificIPCAlign->timer0Value = value;
    //timerOnOffDelay|=1;
    //cpuNextEvent = cpuTotalTicks;
	
#ifdef printsoundtimer
	printf("ur %04x to %08x\r ",value,address);
#endif
#ifdef arm9advsound
	SendMultipleWordACK(0xc3730000,(u32)address,(u32)value,0x0);
#endif
	
	*(u16 *)(0x4000102) = value;
    
	UPDATE_REG(0x102, value);
	break;
	
	//TM1CNT_L
  case 0x104:
    //SpecificIPCAlign->timer1Reload = value;
#ifdef printsoundtimer
	printf("ur %04x to %08x\r ",value,address);
#endif
#ifdef arm9advsound
	SendMultipleWordACK(0xc3730000,(u32)address,(u32)value,0x0);
#endif
	*(u16 *)(0x4000104) = value;
    
	UPDATE_REG(0x104, value);
	break;
	
	//TM1CNT_H
  case 0x106:
	//SpecificIPCAlign->timer1Value = value;
#ifdef printsoundtimer
	printf("ur %04x to %08x\r ",value,address);
#endif
#ifdef arm9advsound
	SendMultipleWordACK(0xc3730000,(u32)address,(u32)value,0x0);
#endif

    //timerOnOffDelay|=2;
    //cpuNextEvent = cpuTotalTicks;
	
	
	*(u16 *)(0x4000106) = value;
    
	UPDATE_REG(0x106, value);
	break;
  
  
  
  
  
  
  case 0x108:
    SpecificIPCAlign->TM2CNT_L = value;
	UPDATE_REG(0x108, value);
	*(u16 *)(0x4000108) = value;
    break;
	
	//TM2CNT_H
  case 0x10A:
	SpecificIPCAlign->TM2CNT_H = value;
    //timerOnOffDelay|=4;
    //cpuNextEvent = cpuTotalTicks;
	*(u16 *)(0x400010a) = value;
   	UPDATE_REG(0x10A, value);

	  break;
	  
  //TM3CNT_L
  case 0x10C:
    SpecificIPCAlign->TM3CNT_L = value;
	UPDATE_REG(0x10C, value);
	*(u16 *)(0x400010c) = value;
    
	  break;
  
  //TM3CNT_H
  case 0x10E:
    SpecificIPCAlign->TM3CNT_H = value;
    //timerOnOffDelay|=8;
    //cpuNextEvent = cpuTotalTicks;
	UPDATE_REG(0x10E, value);

	*(u16 *)(0x400010e) = value;
    
	
	
  break;
  case 0x128:
    if(value & 0x80) {
      value &= 0xff7f;
      if(value & 1 && (value & 0x4000)) {
        UPDATE_REG(0x12a, 0xFF);
        SpecificIPCAlign->GBA_IF |= 0x80;
        UPDATE_REG(0x202, SpecificIPCAlign->GBA_IF);
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
    SpecificIPCAlign->GBA_IE = value & 0x3FFF;
    UPDATE_REG(0x200, SpecificIPCAlign->GBA_IE);
    /*if ((SpecificIPCAlign->GBA_IME & 1) && (SpecificIPCAlign->GBA_IF & SpecificIPCAlign->GBA_IE) && armIrqEnable)
      cpuNextEvent = cpuTotalTicks;*/
#ifdef forceHBlankirqs
	REG_IE = SpecificIPCAlign->GBA_IE | (REG_IE & 0xFFFF0000) | IRQ_HBLANK;
#else
	REG_IE = SpecificIPCAlign->GBA_IE | (REG_IE & 0xFFFF0000);
#endif
	
	anytimejmpfilter = SpecificIPCAlign->GBA_IE;
	
    break;
  case 0x202:
	//REG_IF = value; //ichfly update at read outdated
	//SpecificIPCAlign->GBA_IF = REG_IF;
#ifdef gba_handel_IRQ_correct
	REG_IF = value;
#else
    SpecificIPCAlign->GBA_IF ^= (value & SpecificIPCAlign->GBA_IF);
    UPDATE_REG(0x202, SpecificIPCAlign->GBA_IF);
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
    SpecificIPCAlign->GBA_IME = value & 1;
    UPDATE_REG(0x208, SpecificIPCAlign->GBA_IME);
#ifdef gba_handel_IRQ_correct
	REG_IME = SpecificIPCAlign->GBA_IME;
#endif
    /*if ((SpecificIPCAlign->GBA_IME & 1) && (SpecificIPCAlign->GBA_IF & SpecificIPCAlign->GBA_IE) && armIrqEnable)
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
      WRITE32LE(&myROM[i], myROM[i]);
    }
    cpuBiosSwapped = true;
  }
#endif
  gbaSaveType = 0;
  eepromInUse = 0;
  saveType = 0;
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

	/*
  if(romSize < 0x1fe2000) {
    *((u16 *)&rom[0x1fe209c]) = 0xdffa; // SWI 0xFA
    *((u16 *)&rom[0x1fe209e]) = 0x4770; // BX LR
  } 
  else {
    agbPrintEnable(false);
  }
  */
  
  agbPrintEnable(false);
}

void CPUReset()
{


  if(gbaSaveType == 0) {
    if(eepromInUse)
      gbaSaveType = 3;
    else
      switch(saveType) {
      case 1:
        gbaSaveType = 1;
        break;
      case 2:
        gbaSaveType = 2;
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





  //GBA_DISPCNT  = 0x0000;
  GBA_DISPSTAT = 0x0000;
  GBA_VCOUNT   = (useBios && !skipBios) ? 0 :0x007E;
  GBA_BG0CNT   = 0x0000;
  GBA_BG1CNT   = 0x0000;
  GBA_BG2CNT   = 0x0000;
  GBA_BG3CNT   = 0x0000;
  GBA_BG0HOFS  = 0x0000;
  GBA_BG0VOFS  = 0x0000;
  GBA_BG1HOFS  = 0x0000;
  GBA_BG1VOFS  = 0x0000;
  GBA_BG2HOFS  = 0x0000;
  GBA_BG2VOFS  = 0x0000;
  GBA_BG3HOFS  = 0x0000;
  GBA_BG3VOFS  = 0x0000;
  GBA_BG2PA    = 0x0100;
  GBA_BG2PB    = 0x0000;
  GBA_BG2PC    = 0x0000;
  GBA_BG2PD    = 0x0100;
  GBA_BG2X_L   = 0x0000;
  GBA_BG2X_H   = 0x0000;
  GBA_BG2Y_L   = 0x0000;
  GBA_BG2Y_H   = 0x0000;
  GBA_BG3PA    = 0x0100;
  GBA_BG3PB    = 0x0000;
  GBA_BG3PC    = 0x0000;
  GBA_BG3PD    = 0x0100;
  GBA_BG3X_L   = 0x0000;
  GBA_BG3X_H   = 0x0000;
  GBA_BG3Y_L   = 0x0000;
  GBA_BG3Y_H   = 0x0000;
  GBA_WIN0H    = 0x0000;
  GBA_WIN1H    = 0x0000;
  GBA_WIN0V    = 0x0000;
  GBA_WIN1V    = 0x0000;
  GBA_WININ    = 0x0000;
  GBA_WINOUT   = 0x0000;
  GBA_MOSAIC   = 0x0000;
  GBA_BLDMOD   = 0x0000;
  GBA_COLEV    = 0x0000;
  GBA_COLY     = 0x0000;
  SpecificIPCAlign->DM0SAD_L = 0x0000;
  SpecificIPCAlign->DM0SAD_H = 0x0000;
  SpecificIPCAlign->DM0DAD_L = 0x0000;
  SpecificIPCAlign->DM0DAD_H = 0x0000;
  SpecificIPCAlign->DM0CNT_L = 0x0000;
  SpecificIPCAlign->DM0CNT_H = 0x0000;
  SpecificIPCAlign->DM1SAD_L = 0x0000;
  SpecificIPCAlign->DM1SAD_H = 0x0000;
  SpecificIPCAlign->DM1DAD_L = 0x0000;
  SpecificIPCAlign->DM1DAD_H = 0x0000;
  SpecificIPCAlign->DM1CNT_L = 0x0000;
  SpecificIPCAlign->DM1CNT_H = 0x0000;
  SpecificIPCAlign->DM2SAD_L = 0x0000;
  SpecificIPCAlign->DM2SAD_H = 0x0000;
  SpecificIPCAlign->DM2DAD_L = 0x0000;
  SpecificIPCAlign->DM2DAD_H = 0x0000;
  SpecificIPCAlign->DM2CNT_L = 0x0000;
  SpecificIPCAlign->DM2CNT_H = 0x0000;
  SpecificIPCAlign->DM3SAD_L = 0x0000;
  SpecificIPCAlign->DM3SAD_H = 0x0000;
  SpecificIPCAlign->DM3DAD_L = 0x0000;
  SpecificIPCAlign->DM3DAD_H = 0x0000;
  SpecificIPCAlign->DM3CNT_L = 0x0000;
  SpecificIPCAlign->DM3CNT_H = 0x0000;
  SpecificIPCAlign->TM0CNT_L   = 0x0000;
  SpecificIPCAlign->TM1CNT_L   = 0x0000;
  SpecificIPCAlign->TM2CNT_L   = 0x0000;
  SpecificIPCAlign->TM3CNT_L   = 0x0000;
  SpecificIPCAlign->TM0CNT_H   = 0x0000;
  SpecificIPCAlign->TM1CNT_H   = 0x0000;
  SpecificIPCAlign->TM2CNT_H   = 0x0000;
  SpecificIPCAlign->TM3CNT_H   = 0x0000;
  
  P1       = 0x03FF;
  SpecificIPCAlign->GBA_IE       = 0x0000;
  SpecificIPCAlign->GBA_IF       = 0x0000;
  SpecificIPCAlign->GBA_IME      = 0x0000;

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
  UPDATE_REG(0x00, GBA_DISPCNT);
  UPDATE_REG(0x06, GBA_VCOUNT);
  UPDATE_REG(0x20, GBA_BG2PA);
  UPDATE_REG(0x26, GBA_BG2PD);
  UPDATE_REG(0x30, GBA_BG3PA);
  UPDATE_REG(0x36, GBA_BG3PD);
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
  SpecificIPCAlign->timer0On = false;
  SpecificIPCAlign->timer0Ticks = 0;
  
  //SpecificIPCAlign->timer0Reload = 0;
  SpecificIPCAlign->TM0CNT_L = 0;
  
  SpecificIPCAlign->timer0ClockReload  = 0;
  SpecificIPCAlign->timer1On = false;
  SpecificIPCAlign->timer1Ticks = 0;
  
  //SpecificIPCAlign->timer1Reload = 0;
  SpecificIPCAlign->TM1CNT_L = 0;
  
  SpecificIPCAlign->timer1ClockReload  = 0;
  SpecificIPCAlign->timer2On = false;
  SpecificIPCAlign->timer2Ticks = 0;
  
  //SpecificIPCAlign->timer2Reload = 0;
  SpecificIPCAlign->TM2CNT_L = 0;
  
  SpecificIPCAlign->timer2ClockReload  = 0;
  SpecificIPCAlign->timer3On = false;
  SpecificIPCAlign->timer3Ticks = 0;
  
  //SpecificIPCAlign->timer3Reload = 0;
  SpecificIPCAlign->TM3CNT_L = 0;
  
  SpecificIPCAlign->timer3ClockReload  = 0;
  
  SpecificIPCAlign->dma0Source = 0;
  SpecificIPCAlign->dma0Dest = 0;
  SpecificIPCAlign->dma1Source = 0;
  SpecificIPCAlign->dma1Dest = 0;
  SpecificIPCAlign->dma2Source = 0;
  SpecificIPCAlign->dma2Dest = 0;
  SpecificIPCAlign->dma3Source = 0;
  SpecificIPCAlign->dma3Dest = 0;
  
  cpuSaveGameFunc = flashSaveDecide;
  //renderLine = mode0RenderLine;
  fxOn = false;
  windowOn = false;
  frameCount = 0;
  saveType = 0;
  //layerEnable = GBA_DISPCNT & layerSettings;

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
    saveType = gbaSaveType = 0;
    break;
  case 1: // EEPROM
    cpuSramEnabled = false;
    cpuFlashEnabled = false;
    cpuEEPROMEnabled = true;
    cpuEEPROMSensorEnabled = false;
    saveType = gbaSaveType = 3;
    // EEPROM usage is automatically detected
    break;
  case 2: // SRAM
    cpuSramEnabled = true;
    cpuFlashEnabled = false;
    cpuEEPROMEnabled = false;
    cpuEEPROMSensorEnabled = false;
    cpuSaveGameFunc = sramDelayedWrite; // to insure we detect the write
    saveType = gbaSaveType = 1;
    break;
  case 3: // FLASH
    cpuSramEnabled = false;
    cpuFlashEnabled = true;
    cpuEEPROMEnabled = false;
    cpuEEPROMSensorEnabled = false;
    cpuSaveGameFunc = flashDelayedWrite; // to insure we detect the write
    saveType = gbaSaveType = 2;
    break;
  case 4: // EEPROM+Sensor
    cpuSramEnabled = false;
    cpuFlashEnabled = false;
    cpuEEPROMEnabled = true;
    cpuEEPROMSensorEnabled = true;
    // EEPROM usage is automatically detected
    saveType = gbaSaveType = 3;
    break;
  case 5: // NONE
    cpuSramEnabled = false;
    cpuFlashEnabled = false;
    cpuEEPROMEnabled = false;
    cpuEEPROMSensorEnabled = false;
    // no save at all
    saveType = gbaSaveType = 5;
    break;
  } 

  //ARM_PREFETCH;

  systemSaveUpdateCounter = SYSTEM_SAVE_NOT_UPDATED;

  cpuDmaHack = false;

  //lastTime = systemGetClock();

  //SWITicks = 0;
  
  rtcEnable(true); //coto: nds7 RTC support ;D
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




/*struct EmulatedSystem GBASystem = {
  // emuMain
  //CPULoop,
  // emuReset
  //CPUReset,
  // emuCleanUp
  ////CPUCleanUp,
  // emuReadBattery
  CPUReadBatteryFile,
  // emuWriteBattery
  CPUWriteBatteryFile,
  // emuReadState
  //CPUReadState,
  // emuWriteState 
  //CPUWriteState,
  // emuReadMemState
  //CPUReadMemState,
  // emuWriteMemState
  //CPUWriteMemState,
  // emuWritePNG
  //CPUWritePNGFile,
  // emuWriteBMP
  //CPUWriteBMPFile,
  // emuUpdateCPSR
  //CPUUpdateCPSR,
  // emuHasDebugger
  //true,
  // .
  //2500000
};/*

//ichfly

/*u32 systemGetClock()
{
  return 0; //ichfly todo ftp counter 
}

void winLog(char const*, ...)
{
	return; //todo ichfly
}
*/




__attribute__((section(".itcm")))
void CPUWriteMemory(u32 address, u32 value) //ichfly not inline is faster because it is smaler
{
#ifdef printreads
    printf("w32 %08x to %08x ",value,address);
#endif		  
		

#ifdef DEV_VERSION
  if(address & 3) {
    if(systemVerbose & VERBOSE_UNALIGNED_MEMORY) {
      Log("Unaligned word write: %08x to %08x from %08x ",
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
      WRITE32LE(((u32 *)&workRAM[address & 0x3FFFC]), value);
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
      WRITE32LE(((u32 *)&internalRAM[address & 0x7ffC]), value);
    break;
  case 0x04:
    if(address < 0x4000400) {

	/*if((0x4000060 > address && address > 0x4000007) || (address > 0x40000FF && address < 0x4000110)) //timer and lcd
	{
			//printf("32 %x %x\r ",address,value);
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
    WRITE32LE(((u32 *)&paletteRAM[address & 0x3FC]), value);
    break;
  case 0x06:
#ifdef checkclearaddrrw
	if(address > 0x06020000)goto unreadable;
#endif
    address = (address & 0x1fffc);
    if (((GBA_DISPCNT & 7) >2) && ((address & 0x1C000) == 0x18000))
        return;
    if ((address & 0x18000) == 0x18000)
      address &= 0x17fff;

#ifdef BKPT_SUPPORT
    if(*((u32 *)&freezeVRAM[address]))
      cheatsWriteMemory(address + 0x06000000, value);
    else
#endif
    
    WRITE32LE(((u32 *)&vram[address]), value);
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
    WRITE32LE(((u32 *)&emultoroam[address & 0x3fc]), value);
    break;
  case 0x0D:
#ifdef printsavewrite
	  	  printf("%X %X \r",address,value);
#endif
    if(cpuEEPROMEnabled) {
      eepromWrite(address, value);
      break;
    }
    goto unwritable;
  case 0x0E:
#ifdef printsavewrite
	  	  printf("%X %X \r",address,value);
#endif
    if(!eepromInUse | cpuSramEnabled | cpuFlashEnabled) {
      (*cpuSaveGameFunc)(address, (u8)value);
      break;
    }
    // default
  default:
  unwritable:
 unreadable:
#ifdef checkclearaddrrw
      //Log("Illegal word read: %08x at %08x ", address,reg[15].I);
	  Log("Illegal word write: %08x to %08x ",value, address);
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
printf("w16 %04x to %08x\r ",value,address);
#endif

#ifdef DEV_VERSION
  if(address & 1) {
    if(systemVerbose & VERBOSE_UNALIGNED_MEMORY) {
      Log("Unaligned halfword write: %04x to %08x from %08x ",
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
      WRITE16LE(((u16 *)&workRAM[address & 0x3FFFE]),value);
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
      WRITE16LE(((u16 *)&internalRAM[address & 0x7ffe]), value);
    break;    
  case 4:
  
	/*if(address > 0x40000FF && address < 0x4000110)
	{
		*(u16 *)(address) = value;
		break;
	}*/ //don't need that
  
  	/*if(0x4000060 > address && address > 0x4000008)
	{
			printf("16 %x %x\r ",address,value);
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
    WRITE16LE(((u16 *)&paletteRAM[address & 0x3fe]), value);
    break;
  case 6:
#ifdef checkclearaddrrw
	if(address > 0x06020000)goto unwritable;
#endif
    address = (address & 0x1fffe);
    if (((GBA_DISPCNT & 7) >2) && ((address & 0x1C000) == 0x18000))
        return;
    if ((address & 0x18000) == 0x18000)
      address &= 0x17fff;
#ifdef BKPT_SUPPORT
    if(*((u16 *)&freezeVRAM[address]))
      cheatsWriteHalfWord(address + 0x06000000,
                          value);
    else
#endif
    WRITE16LE(((u16 *)&vram[address]), value); 
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
    WRITE16LE(((u16 *)&emultoroam[address & 0x3fe]), value);
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
	  	  printf("%X %X \r",address,value);
#endif
    if(cpuEEPROMEnabled) {
      eepromWrite(address, (u8)value);
      break;
    }
    goto unwritable;
  case 14:
#ifdef printsavewrite
	  	  printf("%X %X \r",address,value);
#endif
    if(!eepromInUse | cpuSramEnabled | cpuFlashEnabled) {
      (*cpuSaveGameFunc)(address, (u8)value);
      break;
    }
    goto unwritable;
  default:
  unwritable:
#ifdef checkclearaddrrw
      //Log("Illegal word read: %08x at %08x ", address,reg[15].I);
	  Log("Illegal hword write: %04x to %08x ",value, address);
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
	printf("w8 %02x to %08x\r ",b,address);
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
		  printf("b %02x to %08x\r ",b,address);
#endif
		#ifdef arm9advsound
			SendMultipleWordACK(0xc3730000,(u32)(address & 0x3FF),(u32)b,0x0);
		#endif
	break;
      default:
	/*if((0x4000060 > address && address > 0x4000008) || (address > 0x40000FF && address < 0x4000110))
	{
			//printf("8 %x %x\r ",address,b);
		    *(u8 *)(address) = b;
	}*/ //ichfly don't need that
	if(address & 1)
	{
	  CPUUpdateRegister(address & 0x3fe,
			    ((READ16LE(((u16 *)&ioMem[address & 0x3fe])))
			     & 0x00FF) |
			    b<<8);

	}
	else
	  CPUUpdateRegister(address & 0x3fe,
			    ((READ16LE(((u16 *)&ioMem[address & 0x3fe])) & 0xFF00) | b));
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
    if (((GBA_DISPCNT & 7) >2) && ((address & 0x1C000) == 0x18000))
        return;
    if ((address & 0x18000) == 0x18000)
      address &= 0x17fff;

    // no need to switch 
    // byte writes to OBJ VRAM are ignored
    if ((address) < objTilesAddress[((GBA_DISPCNT&7)+1)>>2])
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
	  	  printf("%X %X \r",address,b);
#endif

    if(cpuEEPROMEnabled) {
      eepromWrite(address, b);
      break;
    }
    goto unwritable;
  case 14:
#ifdef printsavewrite
	  	  printf("%X %X \r",address,b);
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
      //Log("Illegal word read: %08x at %08x ", address,reg[15].I);
	  Log("Illegal byte write: %02x to %08x ",b, address);
	  REG_IME = IME_DISABLE;
	  debugDump();
	  while(1);
#endif
    break;
  }
}

__attribute__((section(".itcm")))
void updateVC()
{
		u32 temp = REG_VCOUNT;
		u32 temp2 = REG_DISPSTAT;
		//printf("Vcountreal: %08x ",temp);
		u16 help3;
#ifdef usebuffedVcout
		GBA_VCOUNT = VCountdstogba[temp];
#else
		if(temp < 192)
		{
			GBA_VCOUNT = ((temp * 214) >> 8);//GBA_VCOUNT = help * (1./1.2); //1.15350877;
			//help3 = (help + 1) * (1./1.2); //1.15350877;  // ichfly todo it is to slow
		}
		else
		{
			GBA_VCOUNT = (((temp - 192) * 246) >>  8)+ 160;//GBA_VCOUNT = ((temp - 192) * (1./ 1.04411764)) + 160 //* (1./ 1.04411764)) + 160; //1.15350877;
			//help3 = ((help - 191) *  (1./ 1.04411764)) + 160; //1.15350877;  // ichfly todo it is to slow			
		}
#endif
		GBA_DISPSTAT &= 0xFFF8; //reset h-blanc and V-Blanc and V-Count Setting
		//if(help3 == GBA_VCOUNT) //else it is a extra long V-Line // ichfly todo it is to slow
		//{
			GBA_DISPSTAT |= (temp2 & 0x3); //temporary patch get original settings
		//}
		//if(GBA_VCOUNT > 160 && GBA_VCOUNT != 227)GBA_DISPSTAT |= 1;//V-Blanc
		UPDATE_REG(0x06, GBA_VCOUNT);
		if(GBA_VCOUNT == (GBA_DISPSTAT >> 8)) //update by V-Count Setting
		{
			GBA_DISPSTAT |= 0x4;
			/*if(GBA_DISPSTAT & 0x20) {
			  SpecificIPCAlign->GBA_IF |= 4;
			  UPDATE_REG(0x202, SpecificIPCAlign->GBA_IF);
			}*/
		}
		UPDATE_REG(0x04, GBA_DISPSTAT);
		//printf("Vcountreal: %08x ",temp);
		//printf("GBA_DISPSTAT: %08x ",temp2);
}

__attribute__((section(".itcm")))
u32 CPUReadMemoryreal(u32 address) //ichfly not inline is faster because it is smaler
{
#ifdef DEV_VERSION
  if(address & 3) {  
    if(systemVerbose & VERBOSE_UNALIGNED_MEMORY) {
      Log("Unaligned word read: %08x at %08x ", address, armMode ?
          armNextPC - 4 : armNextPC - 2);
    }
  }
#endif
  
#ifdef printreads
  printf("r32 %08x ",address);
#endif
  
  u32 value;
  switch(address >> 24) {
  case 0:
    if(reg[15].I >> 24) {
      if(address < 0x4000) {
#ifdef DEV_VERSION
        if(systemVerbose & VERBOSE_ILLEGAL_READ) {
          Log("Illegal word read: %08x at %08x ", address, armMode ?
              armNextPC - 4 : armNextPC - 2);
        }
#endif
        
        value = READ32LE(((u32 *)&biosProtected));
      }
      else goto unreadable;
    } else
      value = READ32LE(((u32 *)&bios[address & 0x3FFC]));
    break;
  case 2:
#ifdef checkclearaddrrw
	if(address >0x023FFFFF)goto unreadable;
#endif
    value = READ32LE(((u32 *)&workRAM[address & 0x3FFFC]));
    break;
  case 3:
#ifdef checkclearaddrrw
	if(address > 0x03008000 && !(address > 0x03FF8000)/*upern mirrow*/)goto unreadable;
#endif
    value = READ32LE(((u32 *)&internalRAM[address & 0x7ffC]));
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
		SpecificIPCAlign->GBA_IF = *(vuint16*)0x04000214; //VBlanc
		UPDATE_REG(0x202, SpecificIPCAlign->GBA_IF);
	}
#endif	


	if(address > 0x4000003 && address < 0x4000008)//ichfly update
	{
		updateVC();
	}
	
	
	if(address==0x04000006){
		value = GBA_VCOUNT;
		break;
	}
	
    if((address < 0x4000400) && ioReadable[address & 0x3fc]) {
      if(ioReadable[(address & 0x3fc) + 2])
        value = READ32LE(((u32 *)&ioMem[address & 0x3fC]));
      else
        value = READ16LE(((u16 *)&ioMem[address & 0x3fc]));
    } else goto unreadable;
    break;
  case 5:
#ifdef checkclearaddrrw
	if(address > 0x05000400)goto unreadable;
#endif
    value = READ32LE(((u32 *)&paletteRAM[address & 0x3fC]));
    break;
  case 6:
#ifdef checkclearaddrrw
	if(address > 0x06020000)goto unreadable;
#endif
    address = (address & 0x1fffc);
    if (((GBA_DISPCNT & 7) >2) && ((address & 0x1C000) == 0x18000))
    {
        value = 0;
        break;
    }
    if ((address & 0x18000) == 0x18000)
      address &= 0x17fff;
    value = READ32LE(((u32 *)&vram[address]));
    break;
  case 7:
#ifdef checkclearaddrrw
	if(address > 0x07000400)goto unreadable;
#endif
    value = READ32LE(((u32 *)&emultoroam[address & 0x3FC]));
    break;
  case 8:
  case 9:
  case 10:
  case 11:
  case 12:
#ifdef uppern_read_emulation
	if((address&0x1FFFFFC) > romSize)
	{
#ifdef print_uppern_read_emulation
		printf("high r32 %08x ",address);
#endif
		if(ichflyfilestreamsize > (address&0x1FFFFFC))
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
		value = READ32LE(((u32 *)&rom[address&0x1FFFFFC]));
	}
#else
    value = READ32LE(((u32 *)&rom[address&0x1FFFFFC]));
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
      //Log("Illegal word read: %08x at %08x ", address,reg[15].I);
	  Log("Illegal word read: %08x ", address);
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
u32 CPUReadHalfWordreal(u32 address) //ichfly not inline is faster because it is smaler
{
#ifdef printreads
	printf("r16 %08x ",address);
#endif
#ifdef DEV_VERSION      
  if(address & 1) {
    if(systemVerbose & VERBOSE_UNALIGNED_MEMORY) {
      Log("Unaligned halfword read: %08x at %08x ", address, armMode ?
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
          Log("Illegal halfword read: %08x at %08x ", address, armMode ?
              armNextPC - 4 : armNextPC - 2);
        }
#endif
        value = READ16LE(((u16 *)&biosProtected[address&2]));
      } else goto unreadable;
    } else
      value = READ16LE(((u16 *)&bios[address & 0x3FFE]));
    break;
  case 2:
#ifdef checkclearaddrrw
	if(address >0x023FFFFF)goto unreadable;
#endif
    value = READ16LE(((u16 *)&workRAM[address & 0x3FFFE]));
    break;
  case 3:
#ifdef checkclearaddrrw
	if(address > 0x03008000 && !(address > 0x03FF8000)/*upern mirrow*/)goto unreadable;
#endif
    value = READ16LE(((u16 *)&internalRAM[address & 0x7ffe]));
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
		updateVC();
	}
	
	if(address==0x04000006){
		value = GBA_VCOUNT;
		break;
	}
	
#ifdef gba_handel_IRQ_correct
	if(address == 0x4000202)//ichfly update
	{
		SpecificIPCAlign->GBA_IF = *(vuint16*)0x04000214;
		UPDATE_REG(0x202, SpecificIPCAlign->GBA_IF);
	}
#endif
	
    if((address < 0x4000400) && ioReadable[address & 0x3fe])
    {
		value =  READ16LE(((u16 *)&ioMem[address & 0x3fe]));
    }
    else goto unreadable;
    break;
  case 5:
#ifdef checkclearaddrrw
	if(address > 0x05000400)goto unreadable;
#endif
    value = READ16LE(((u16 *)&paletteRAM[address & 0x3fe]));
    break;
  case 6:
#ifdef checkclearaddrrw
	if(address > 0x06020000)goto unreadable;
#endif
    address = (address & 0x1fffe);
    if (((GBA_DISPCNT & 7) >2) && ((address & 0x1C000) == 0x18000))
    {
        value = 0;
        break;
    }
    if ((address & 0x18000) == 0x18000)
      address &= 0x17fff;
    value = READ16LE(((u16 *)&vram[address]));
    break;
  case 7:
#ifdef checkclearaddrrw
	if(address > 0x07000400)goto unreadable;
#endif
    value = READ16LE(((u16 *)&emultoroam[address & 0x3fe]));
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
	if((address&0x1FFFFFE) > romSize)
	{
#ifdef print_uppern_read_emulation
		printf("high r16 %08x ",address);
#endif
		if(ichflyfilestreamsize > (address&0x1FFFFFE))
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
		value = READ16LE(((u16 *)&rom[address & 0x1FFFFFE]));
	}
#else
    value = READ16LE(((u16 *)&rom[address & 0x1FFFFFE]));
#endif
	}
    break;    
  case 13:
#ifdef printsaveread
	  printf("%X \r",address);
#endif
    if(cpuEEPROMEnabled)
      // no need to swap this
      return  eepromRead(address);
    goto unreadable;
  case 14:
#ifdef printsaveread
	  printf("%X \r",address);
#endif
    if(cpuFlashEnabled | cpuSramEnabled)
      // no need to swap this
      return flashRead(address);
    // default
  default:
  unreadable:
#ifdef checkclearaddrrw
      //Log("Illegal word read: %08x at %08x ", address,reg[15].I);
	  Log("Illegal hword read: %08x ", address);
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
u16 CPUReadHalfWordSigned(u32 address)
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
printf("r8 %02x ",address);
#endif

  switch(address >> 24) {
  case 0:
    if (reg[15].I >> 24) {
      if(address < 0x4000) {
#ifdef DEV_VERSION
        if(systemVerbose & VERBOSE_ILLEGAL_READ) {
          Log("Illegal byte read: %08x at %08x ", address, armMode ?
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
		updateVC();
	}
	
	if(address==0x04000006){
		return (u8)GBA_VCOUNT;
		break;
	}
	
#ifdef gba_handel_IRQ_correct
	if(address == 0x4000202 || address == 0x4000203)//ichfly update
	{
		SpecificIPCAlign->GBA_IF = *(vuint16*)0x04000214; //VBlanc
		UPDATE_REG(0x202, SpecificIPCAlign->GBA_IF);
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
    if (((GBA_DISPCNT & 7) >2) && ((address & 0x1C000) == 0x18000))
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
	if((address&0x1FFFFFF) > romSize)
	{
#ifdef print_uppern_read_emulation
		printf("high r8 %08x ",address);
#endif
		if(ichflyfilestreamsize > (address&0x1FFFFFF))
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
	  printf("%X \r",address);
#endif
    if(cpuEEPROMEnabled)
      return eepromRead(address);
    goto unreadable;
  case 14:
#ifdef printsaveread
	  printf("%X \r",address);
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
      //Log("Illegal word read: %08x at %08x ", address,reg[15].I);
	  Log("Illegal byte read: %08x ", address);
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
void updateVCsub()
{
		u32 temp = REG_VCOUNT;
		u32 temp2 = REG_DISPSTAT;
		//printf("Vcountreal: %08x ",temp);
		u16 help3;
#ifdef usebuffedVcout
		GBA_VCOUNT = VCountdstogba[temp];
#else
		if(temp < 192)
		{
			GBA_VCOUNT = ((temp * 214) >> 8);//GBA_VCOUNT = help * (1./1.2); //1.15350877;
			//help3 = (help + 1) * (1./1.2); //1.15350877;  // ichfly todo it is to slow
		}
		else
		{
			GBA_VCOUNT = (((temp - 192) * 246) >>  8)+ 160;//GBA_VCOUNT = ((temp - 192) * (1./ 1.04411764)) + 160 //* (1./ 1.04411764)) + 160; //1.15350877;
			//help3 = ((help - 191) *  (1./ 1.04411764)) + 160; //1.15350877;  // ichfly todo it is to slow			
		}
#endif
		GBA_DISPSTAT &= 0xFFF8; //reset h-blanc and V-Blanc and V-Count Setting
		//if(help3 == GBA_VCOUNT) //else it is a extra long V-Line // ichfly todo it is to slow
		//{
			GBA_DISPSTAT |= (temp2 & 0x3); //temporary patch get original settings
		//}
		//if(GBA_VCOUNT > 160 && GBA_VCOUNT != 227)GBA_DISPSTAT |= 1;//V-Blanc
		UPDATE_REG(0x06, GBA_VCOUNT);
		if(GBA_VCOUNT == (GBA_DISPSTAT >> 8)) //update by V-Count Setting
		{
			GBA_DISPSTAT |= 0x4;
			/*if(GBA_DISPSTAT & 0x20) {
			  SpecificIPCAlign->GBA_IF |= 4;
			  UPDATE_REG(0x202, SpecificIPCAlign->GBA_IF);
			}*/
		}
		UPDATE_REG(0x04, GBA_DISPSTAT);
		//printf("Vcountreal: %08x ",temp);
		//printf("GBA_DISPSTAT: %08x ",temp2);
}


__attribute__((section(".itcm")))
u32 CPUReadMemoryrealpu(u32 address)
{

	//printf("%08X",REG_IME);
#ifdef DEV_VERSION
  if(address & 3) {  
    if(systemVerbose & VERBOSE_UNALIGNED_MEMORY) {
      Log("Unaligned word read: %08x at %08x ", address, armMode ?
          armNextPC - 4 : armNextPC - 2);
    }
  }
#endif
  
#ifdef printreads
  printf("r32 %08x %08X ",address,cpu_GetMemPrem());
#endif
  
  u32 value;
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
		SpecificIPCAlign->GBA_IF = *(vuint16*)0x04000214; //VBlanc
		UPDATE_REG(0x202, SpecificIPCAlign->GBA_IF);
	}
#endif	


	if(address > 0x4000003 && address < 0x4000008)//ichfly update
	{
		updateVCsub();
	}
    
	if(address==0x04000006){
		value = GBA_VCOUNT;
		break;
	}
	
	if((address < 0x4000400) && ioReadable[address & 0x3fc]) {
      if(ioReadable[(address & 0x3fc) + 2])
        value = READ32LE(((u32 *)&ioMem[address & 0x3fC]));
      else
        value = READ16LE(((u16 *)&ioMem[address & 0x3fc]));
    } else goto unreadable;
    break;
  case 8:
  case 9:
  case 10:
  case 11:
  case 12:
#ifdef uppern_read_emulation
	if((address&0x1FFFFFC) > romSize)
	{
#ifdef print_uppern_read_emulation
		printf("high r32 %08x ",address);
#endif
		if(ichflyfilestreamsize > (address&0x1FFFFFC))
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
		value = READ32LE(((u32 *)&rom[address&0x1FFFFFC]));
	}
#else
    value = READ32LE(((u32 *)&rom[address&0x1FFFFFC]));
#endif
    break;
  case 13:
#ifdef printsaveread
	  printf("%X \r",address);
#endif
    if(cpuEEPROMEnabled)
      // no need to swap this
      return eepromRead(address);
    goto unreadable;
  case 14:
#ifdef printsaveread
	  printf("%X \r",address);
#endif
    if(cpuFlashEnabled | cpuSramEnabled)
      // no need to swap this
      return flashRead(address);
    // default
  default:
  unreadable:
  //while(1);
#ifdef checkclearaddrrw
      //Log("Illegal word read: %08x at %08x ", address,reg[15].I);
	  Log("Illegal word read: %08x ", address);
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
u16 CPUReadHalfWordrealpuSigned(u32 address)
{
  u16 value = CPUReadHalfWordrealpu(address);
  if((address & 1))
    value = (s8)value;
  return value;
}


__attribute__((section(".itcm")))
u32 CPUReadHalfWordrealpu(u32 address) //ichfly not inline is faster because it is smaler
{
#ifdef printreads
	printf("r16 %08x (%08X) ",address,cpu_GetMemPrem());
#endif
#ifdef DEV_VERSION      
  if(address & 1) {
    if(systemVerbose & VERBOSE_UNALIGNED_MEMORY) {
      Log("Unaligned halfword read: %08x at %08x ", address, armMode ?
          armNextPC - 4 : armNextPC - 2);
    }
  }
#endif
  
  u32 value;
  
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
		updateVCsub();
	}
	
	if(address==0x04000006){
		value = GBA_VCOUNT;
		break;
	}
	
#ifdef gba_handel_IRQ_correct
	if(address == 0x4000202)//ichfly update
	{
		SpecificIPCAlign->GBA_IF = *(vuint16*)0x04000214;
		UPDATE_REG(0x202, SpecificIPCAlign->GBA_IF);
	}
#endif
	
    if((address < 0x4000400) && ioReadable[address & 0x3fe])
    {
		value =  READ16LE(((u16 *)&ioMem[address & 0x3fe]));
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
	if((address&0x1FFFFFE) > romSize)
	{
#ifdef print_uppern_read_emulation
		printf("high r16 %08x ",address);
#endif
		if(ichflyfilestreamsize > (address&0x1FFFFFE))
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
		value = READ16LE(((u16 *)&rom[address & 0x1FFFFFE]));
	}
#else
    value = READ16LE(((u16 *)&rom[address & 0x1FFFFFE]));
#endif
	}
    break;    
  case 13:
#ifdef printsaveread
	  printf("%X \r",address);
#endif    if(cpuEEPROMEnabled)
      // no need to swap this
      return  eepromRead(address);
    goto unreadable;
  case 14:
#ifdef printsaveread
	  printf("%X \r",address);
#endif    if(cpuFlashEnabled | cpuSramEnabled)
      // no need to swap this
      return flashRead(address);
    // default
  default:
  unreadable:
#ifdef checkclearaddrrw
      //Log("Illegal word read: %08x at %08x ", address,reg[15].I);
	  Log("Illegal hword read: %08x ", address);
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
printf("r8 %02x ",address);
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
		updateVCsub();
	}
	
	if(address==0x04000006){
		return (u8)GBA_VCOUNT;
		break;
	}
	
#ifdef gba_handel_IRQ_correct
	if(address == 0x4000202 || address == 0x4000203)//ichfly update
	{
		SpecificIPCAlign->GBA_IF = *(vuint16*)0x04000214; //VBlanc
		UPDATE_REG(0x202, SpecificIPCAlign->GBA_IF);
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
	if((address&0x1FFFFFF) > romSize)
	{
#ifdef print_uppern_read_emulation
		printf("high r8 %08x ",address);
#endif
		if(ichflyfilestreamsize > (address&0x1FFFFFF))
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
	  printf("%X \r",address);
#endif    if(cpuEEPROMEnabled)
      return eepromRead(address);
    goto unreadable;
  case 14:
#ifdef printsaveread
	  printf("%X \r",address);
#endif    if(cpuSramEnabled | cpuFlashEnabled)
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
      //Log("Illegal word read: %08x at %08x ", address,reg[15].I);
	  Log("Illegal byte read: %08x ", address);
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
}

__attribute__((section(".itcm")))
void CPUWriteMemorypu(u32 address, u32 value) //ichfly not inline is faster because it is smaler
{
#ifdef printreads
    printf("w32 %08x to %08x ",value,address);
#endif		  
		

#ifdef DEV_VERSION
  if(address & 3) {
    if(systemVerbose & VERBOSE_UNALIGNED_MEMORY) {
      Log("Unaligned word write: %08x to %08x from %08x ",
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
    if (((GBA_DISPCNT & 7) >2) && ((address & 0x1C000) == 0x18000))
        return;
    if ((address & 0x18000) == 0x18000)
      address &= 0x17fff;

#ifdef BKPT_SUPPORT
    if(*((u32 *)&freezeVRAM[address]))
      cheatsWriteMemory(address + 0x06000000, value);
    else
#endif
    
    WRITE32LE(((u32 *)&vram[address]), value);
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
    WRITE32LE(((u32 *)&emultoroam[address & 0x3fc]), value);
#endif

  case 0x04:
    if(address < 0x4000400) {

	/*if((0x4000060 > address && address > 0x4000007) || (address > 0x40000FF && address < 0x4000110)) //timer and lcd
	{
			//printf("32 %x %x\r ",address,value);
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
	  printf("%X %X \r",address,value);
#endif
    if(cpuEEPROMEnabled) {
      eepromWrite(address, value);
      break;
    }
    goto unwritable;
  case 0x0E:
#ifdef printsavewrite
	  printf("%X %X \r",address,value);
#endif
    if(!eepromInUse | cpuSramEnabled | cpuFlashEnabled) {
      (*cpuSaveGameFunc)(address, (u8)value);
      break;
    }
    // default
  default:
  unwritable:
 unreadable:
#ifdef checkclearaddrrw
      //Log("Illegal word read: %08x at %08x ", address,reg[15].I);
	  Log("Illegal word write: %08x to %08x ",value, address);
	  REG_IME = IME_DISABLE;
	  debugDump();
	  while(1);
#endif
    break;
  }
}

__attribute__((section(".itcm")))
void CPUWriteHalfWordpu(u32 address, u16 value)
{
#ifdef printreads
printf("w16 %04x to %08x\r ",value,address);
#endif

#ifdef DEV_VERSION
  if(address & 1) {
    if(systemVerbose & VERBOSE_UNALIGNED_MEMORY) {
      Log("Unaligned halfword write: %04x to %08x from %08x ",
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
    WRITE16LE(((u16 *)&paletteRAM[address & 0x3fe]), value);
    break;
  case 6:
#ifdef checkclearaddrrw
	if(address > 0x06020000)goto unwritable;
#endif
    address = (address & 0x1fffe);
    if (((GBA_DISPCNT & 7) >2) && ((address & 0x1C000) == 0x18000))
        return;
    if ((address & 0x18000) == 0x18000)
      address &= 0x17fff;
#ifdef BKPT_SUPPORT
    if(*((u16 *)&freezeVRAM[address]))
      cheatsWriteHalfWord(address + 0x06000000,
                          value);
    else
#endif
    WRITE16LE(((u16 *)&vram[address]), value); 
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
	  printf("%X %X \r",address,value);
#endif
    if(cpuEEPROMEnabled) {
      eepromWrite(address, (u8)value);
      break;
    }
    goto unwritable;
  case 14:
#ifdef printsavewrite
	  printf("%X %X \r",address,value);
#endif
    if(!eepromInUse | cpuSramEnabled | cpuFlashEnabled) {
      (*cpuSaveGameFunc)(address, (u8)value);
      break;
    }
    goto unwritable;
  default:
  unwritable:
#ifdef checkclearaddrrw
      //Log("Illegal word read: %08x at %08x ", address,reg[15].I);
	  Log("Illegal hword write: %04x to %08x ",value, address);
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
	printf("w8 %02x to %08x\r ",b,address);
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
    if (((GBA_DISPCNT & 7) >2) && ((address & 0x1C000) == 0x18000))
        return;
    if ((address & 0x18000) == 0x18000)
      address &= 0x17fff;

    // no need to switch 
    // byte writes to OBJ VRAM are ignored
    if ((address) < objTilesAddress[((GBA_DISPCNT&7)+1)>>2])
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
		  printf("b %02x to %08x\r ",b,address);
#endif
	#ifdef arm9advsound
		SendMultipleWordACK(0xc3730000,(u32)(address & 0x3FF),(u32)b,0x0);
	#endif
	break;
      default:
	/*if((0x4000060 > address && address > 0x4000008) || (address > 0x40000FF && address < 0x4000110))
	{
			//printf("8 %x %x\r ",address,b);
		    *(u8 *)(address) = b;
	}*/ //ichfly don't need that
	if(address & 1)
	{
	  CPUUpdateRegister(address & 0x3fe,
			    ((READ16LE(((u16 *)&ioMem[address & 0x3fe])))
			     & 0x00FF) |
			    b<<8);

	}
	else
	  CPUUpdateRegister(address & 0x3fe,
			    ((READ16LE(((u16 *)&ioMem[address & 0x3fe])) & 0xFF00) | b));
      }
      break;
    } else goto unwritable;
    break;   
  case 13:
#ifdef printsavewrite
	  printf("%X %X \r",address,b);
#endif
    if(cpuEEPROMEnabled) {
      eepromWrite(address, b);
      break;
    }
    goto unwritable;
  case 14:
#ifdef printsavewrite
	  printf("%X %X \r",address,b);
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
      //Log("Illegal word read: %08x at %08x ", address,reg[15].I);
	  Log("Illegal byte write: %02x to %08x ",b, address);
	  REG_IME = IME_DISABLE;
	  debugDump();
	  while(1);
#endif
    break;
  }
}
