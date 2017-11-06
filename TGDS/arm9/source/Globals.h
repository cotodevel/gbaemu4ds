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
extern bool parseDebug;
extern int layerSettings;
extern int layerEnable;
extern bool speedHack;
extern int cpuSaveType;
extern bool cheatsEnabled;
extern bool mirroringEnable;

extern u8 *bios;
extern u8 *rom;
#define internalRAM ((u8*)0x03000000)
#define workRAM ((u8*)0x02000000)
#define paletteRAM ((u8*)0x05000000)
#define vram ((u8*)0x06000000)
#define emultoroam ((u8*)0x07000000)
extern u8 ioMem[0x400];

#define DISPCAPCNT (*(vu32*)0x4000064)

extern u8 currentVRAMcapblock;

extern volatile u16 GBA_DISPCNT;
extern u16 GBA_DISPSTAT;	
extern u16 GBA_VCOUNT;	
extern u16 GBA_BG0CNT;	
extern u16 GBA_BG1CNT;	
extern u16 GBA_BG2CNT;	
extern u16 GBA_BG3CNT;	
extern u16 GBA_BG0HOFS;
extern u16 GBA_BG0VOFS;		
extern u16 GBA_BG1HOFS;	
extern u16 GBA_BG1VOFS;	
extern u16 GBA_BG2HOFS;
extern u16 GBA_BG2VOFS;	
extern u16 GBA_BG3HOFS;
extern u16 GBA_BG3VOFS;	
extern u16 GBA_BG2PA;	
extern u16 GBA_BG2PB;	
extern u16 GBA_BG2PC;
extern u16 GBA_BG2PD;	
extern u16 GBA_BG2X_L;	
extern u16 GBA_BG2X_H;	
extern u16 GBA_BG2Y_L;
extern u16 GBA_BG2Y_H;
extern u16 GBA_BG3PA;
extern u16 GBA_BG3PB;	
extern u16 GBA_BG3PC;
extern u16 GBA_BG3PD;
extern u16 GBA_BG3X_L;
extern u16 GBA_BG3X_H;
extern u16 GBA_BG3Y_L;
extern u16 GBA_BG3Y_H;
extern u16 GBA_WIN0H;
extern u16 GBA_WIN1H;
extern u16 GBA_WIN0V;
extern u16 GBA_WIN1V;
extern u16 GBA_WININ;
extern u16 GBA_WINOUT;
extern u16 GBA_MOSAIC;	
extern u16 GBA_BLDMOD;
extern u16 GBA_COLEV;
extern u16 GBA_COLY;	//SO FAR HERE

//IPC
/*
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
extern u16 IE;
extern u16 IF;
extern u16 IME;
*/

extern u16 P1;

//ichfly
extern u16 timer0Value;
extern u16 timer1Value;
extern u16 timer2Value;
extern u16 timer3Value;

#endif // VBA_GLOBALS_H
