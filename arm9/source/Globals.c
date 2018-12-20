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

#include "GBA.h"

#include "ichflysettings.h"

#ifdef uppern_read_emulation
FILE* ichflyfilestream;
int ichflyfilestreamsize;
#endif

#ifdef countpagefalts
u32 pagefehler = 0;
#endif

#ifdef BKPT_SUPPORT
int  oldreg[17];
char oldbuffer[10];
#endif

#ifdef capture_and_pars
u8 currentVRAMcapblock = 1;
#endif
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
int framewtf = 0;

__attribute__((section(".dtcm")))
bool speedup = false;

__attribute__((section(".dtcm")))
bool synchronize = true;

__attribute__((section(".dtcm")))
bool cpuDisableSfx = false;

__attribute__((section(".dtcm")))
bool cpuIsMultiBoot = false;

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

__attribute__((section(".dtcm")))
u8 *bios = NULL; //calloc

__attribute__((section(".dtcm")))
u8 *rom = NULL; //calc

/*static u8 *internalRAM = (u8*)0x03000000; 
static u8 *workRAM = (u8*)0x02000000;
static u8 *paletteRAM = (u8*)0x05000000;
static u8 *vram = (u8*)0x06000000;
static u8 *oam = (u8*)0x07000000;*/

__attribute__((section(".dtcm")))
u16 GBABG0CNT   = 0x0000;

__attribute__((section(".dtcm")))
u16 GBABG1CNT   = 0x0000;

__attribute__((section(".dtcm")))
u16 GBABG2CNT   = 0x0000;

__attribute__((section(".dtcm")))
u16 GBABG3CNT   = 0x0000;

__attribute__((section(".dtcm")))
u16 GBABG0HOFS  = 0x0000;

__attribute__((section(".dtcm")))
u16 GBABG0VOFS  = 0x0000;

__attribute__((section(".dtcm")))
u16 GBABG1HOFS  = 0x0000;

__attribute__((section(".dtcm")))
u16 GBABG1VOFS  = 0x0000;

__attribute__((section(".dtcm")))
u16 GBABG2HOFS  = 0x0000;

__attribute__((section(".dtcm")))
u16 GBABG2VOFS  = 0x0000;

__attribute__((section(".dtcm")))
u16 GBABG3HOFS  = 0x0000;

__attribute__((section(".dtcm")))
u16 GBABG3VOFS  = 0x0000;

__attribute__((section(".dtcm")))
u16 GBABG2PA    = 0x0100;

__attribute__((section(".dtcm")))
u16 GBABG2PB    = 0x0000;

__attribute__((section(".dtcm")))
u16 GBABG2PC    = 0x0000;

__attribute__((section(".dtcm")))
u16 GBABG2PD    = 0x0100;

__attribute__((section(".dtcm")))
u16 GBABG2X_L   = 0x0000;

__attribute__((section(".dtcm")))
u16 GBABG2X_H   = 0x0000;

__attribute__((section(".dtcm")))
u16 GBABG2Y_L   = 0x0000;

__attribute__((section(".dtcm")))
u16 GBABG2Y_H   = 0x0000;

__attribute__((section(".dtcm")))
u16 GBABG3PA    = 0x0100;

__attribute__((section(".dtcm")))
u16 GBABG3PB    = 0x0000;

__attribute__((section(".dtcm")))
u16 GBABG3PC    = 0x0000;

__attribute__((section(".dtcm")))
u16 GBABG3PD    = 0x0100;

__attribute__((section(".dtcm")))
u16 GBABG3X_L   = 0x0000;

__attribute__((section(".dtcm")))
u16 GBABG3X_H   = 0x0000;

__attribute__((section(".dtcm")))
u16 GBABG3Y_L   = 0x0000;

__attribute__((section(".dtcm")))
u16 GBABG3Y_H   = 0x0000;

__attribute__((section(".dtcm")))
u16 GBAWIN0H    = 0x0000;

__attribute__((section(".dtcm")))
u16 GBAWIN1H    = 0x0000;

__attribute__((section(".dtcm")))
u16 GBAWIN0V    = 0x0000;

__attribute__((section(".dtcm")))
u16 GBAWIN1V    = 0x0000;

__attribute__((section(".dtcm")))
u16 GBAWININ    = 0x0000;

__attribute__((section(".dtcm")))
u16 GBAWINOUT   = 0x0000;

__attribute__((section(".dtcm")))
u16 GBAMOSAIC   = 0x0000;

__attribute__((section(".dtcm")))
u16 GBABLDMOD   = 0x0000;

__attribute__((section(".dtcm")))
u16 GBACOLEV    = 0x0000;

__attribute__((section(".dtcm")))
u16 GBACOLY     = 0x0000;

__attribute__((section(".dtcm")))
u16 GBADM2SAD_L = 0x0000;

__attribute__((section(".dtcm")))
u16 GBADM2SAD_H = 0x0000;

__attribute__((section(".dtcm")))
u16 GBADM2DAD_L = 0x0000;

__attribute__((section(".dtcm")))
u16 GBADM2DAD_H = 0x0000;

__attribute__((section(".dtcm")))
u16 GBADM2CNT_L = 0x0000;

__attribute__((section(".dtcm")))
u16 GBADM2CNT_H = 0x0000;

__attribute__((section(".dtcm")))
u16 GBADM3SAD_L = 0x0000;

__attribute__((section(".dtcm")))
u16 GBADM3SAD_H = 0x0000;

__attribute__((section(".dtcm")))
u16 GBADM3DAD_L = 0x0000;

__attribute__((section(".dtcm")))
u16 GBADM3DAD_H = 0x0000;

__attribute__((section(".dtcm")))
u16 GBADM3CNT_L = 0x0000;

__attribute__((section(".dtcm")))
u16 GBADM3CNT_H = 0x0000;

__attribute__((section(".dtcm")))
u16 GBATM0D     = 0x0000;

__attribute__((section(".dtcm")))
u16 GBATM0CNT   = 0x0000;

__attribute__((section(".dtcm")))
u16 GBATM1D     = 0x0000;

__attribute__((section(".dtcm")))
u16 GBATM1CNT   = 0x0000;

__attribute__((section(".dtcm")))
u16 GBATM2D     = 0x0000;

__attribute__((section(".dtcm")))
u16 GBATM2CNT   = 0x0000;

__attribute__((section(".dtcm")))
u16 GBATM3D     = 0x0000;

__attribute__((section(".dtcm")))
u16 GBATM3CNT   = 0x0000;

__attribute__((section(".dtcm")))
u16 GBAP1       = 0xFFFF;

__attribute__((section(".dtcm")))
u16 GBAIE       = 0x0000;

__attribute__((section(".dtcm")))
u16 GBAIF       = 0x0000;

__attribute__((section(".dtcm")))
u16 GBAIME      = 0x0000;

__attribute__((section(".dtcm")))
u16 GBADISPSTAT = 0x0000;

__attribute__((section(".dtcm")))
u16 GBAVCOUNT   = 0x0000;

__attribute__((section(".dtcm")))
u16 GBADM0SAD_L = 0x0000;

__attribute__((section(".dtcm")))
u16 GBADM0SAD_H = 0x0000;

__attribute__((section(".dtcm")))
u16 GBADM0DAD_L = 0x0000;

__attribute__((section(".dtcm")))
u16 GBADM0DAD_H = 0x0000;

__attribute__((section(".dtcm")))
u16 GBADM0CNT_L = 0x0000;

__attribute__((section(".dtcm")))
u16 GBADM0CNT_H = 0x0000;

__attribute__((section(".dtcm")))
u16 GBADM1SAD_L = 0x0000;

__attribute__((section(".dtcm")))
u16 GBADM1SAD_H = 0x0000;

__attribute__((section(".dtcm")))
u16 GBADM1DAD_L = 0x0000;

__attribute__((section(".dtcm")))
u16 GBADM1DAD_H = 0x0000;

__attribute__((section(".dtcm")))
u16 GBADM1CNT_L = 0x0000;

__attribute__((section(".dtcm")))
u16 GBADM1CNT_H = 0x0000;

__attribute__((section(".dtcm")))
u8 * ioMem = NULL;	//vram [0x400];
