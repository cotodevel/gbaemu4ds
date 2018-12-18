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

#ifndef VBA_GLOBALS_H
#define VBA_GLOBALS_H

#include "GBA.h"
#include "ichflysettings.h"

#ifdef lastdebug
extern u32 lasttime[6];
extern int lastdebugcurrent;
extern int lastdebugsize;
#endif

#define DISPCAPCNT (*(vu32*)0x4000064)
#define internalRAM ((u8*)0x03000000)
#define workRAM ((u8*)0x02000000)
#define paletteRAM ((u8*)0x05000000)
#define vram ((u8*)0x06000000)
#define emultoroam ((u8*)0x07000000)

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

#endif // VBA_GLOBALS_H




#ifdef __cplusplus
extern "C" {
#endif

#ifdef uppern_read_emulation
extern FILE* ichflyfilestream;
extern int ichflyfilestreamsize;
#endif

#ifdef countpagefalts
extern u32 pagefehler;
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
extern int layerSettings;
extern int layerEnable;
extern bool speedHack;
extern int cpuSaveType;
extern bool cheatsEnabled;
extern bool mirroringEnable;

extern u8 *bios;
extern u8 *rom;
extern u8 * ioMem;//extern u8 ioMem[0x400];
extern u8 currentVRAMcapblock;

extern u16 GBADISPCNT;
extern u16 GBADISPSTAT;
extern u16 GBAVCOUNT;
extern u16 GBABG0CNT;
extern u16 GBABG1CNT;
extern u16 GBABG2CNT;
extern u16 GBABG3CNT;
extern u16 GBABG0HOFS;
extern u16 GBABG0VOFS;
extern u16 GBABG1HOFS;
extern u16 GBABG1VOFS;
extern u16 GBABG2HOFS;
extern u16 GBABG2VOFS;
extern u16 GBABG3HOFS;
extern u16 GBABG3VOFS;
extern u16 GBABG2PA;
extern u16 GBABG2PB;
extern u16 GBABG2PC;
extern u16 GBABG2PD;
extern u16 GBABG2X_L;
extern u16 GBABG2X_H;
extern u16 GBABG2Y_L;
extern u16 GBABG2Y_H;
extern u16 GBABG3PA;
extern u16 GBABG3PB;
extern u16 GBABG3PC;
extern u16 GBABG3PD;
extern u16 GBABG3X_L;
extern u16 GBABG3X_H;
extern u16 GBABG3Y_L;
extern u16 GBABG3Y_H;
extern u16 GBAWIN0H;
extern u16 GBAWIN1H;
extern u16 GBAWIN0V;
extern u16 GBAWIN1V;
extern u16 GBAWININ;
extern u16 GBAWINOUT;
extern u16 GBAMOSAIC;
extern u16 GBABLDMOD;
extern u16 GBACOLEV;
extern u16 GBACOLY;
extern u16 GBADM0SAD_L;
extern u16 GBADM0SAD_H;
extern u16 GBADM0DAD_L;
extern u16 GBADM0DAD_H;
extern u16 GBADM0CNT_L;
extern u16 GBADM0CNT_H;
extern u16 GBADM1SAD_L;
extern u16 GBADM1SAD_H;
extern u16 GBADM1DAD_L;
extern u16 GBADM1DAD_H;
extern u16 GBADM1CNT_L;
extern u16 GBADM1CNT_H;
extern u16 GBADM2SAD_L;
extern u16 GBADM2SAD_H;
extern u16 GBADM2DAD_L;
extern u16 GBADM2DAD_H;
extern u16 GBADM2CNT_L;
extern u16 GBADM2CNT_H;
extern u16 GBADM3SAD_L;
extern u16 GBADM3SAD_H;
extern u16 GBADM3DAD_L;
extern u16 GBADM3DAD_H;
extern u16 GBADM3CNT_L;
extern u16 GBADM3CNT_H;
extern u16 GBATM0D;
extern u16 GBATM0CNT;
extern u16 GBATM1D;
extern u16 GBATM1CNT;
extern u16 GBATM2D;
extern u16 GBATM2CNT;
extern u16 GBATM3D;
extern u16 GBATM3CNT;
extern u16 GBAP1;
extern u16 GBAIE;
extern u16 GBAIF;
extern u16 GBAIME;

//ichfly
extern u16 timer0Value;
extern u16 timer1Value;
extern u16 timer2Value;
extern u16 timer3Value;

#ifdef __cplusplus
}
#endif