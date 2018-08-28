// -*- C++ -*-
// VisualBoyAdvance - Nintendo Gameboy/GameboyAdvance (TM) emulator.
// Copyright (C) 1999-2003 Forgotten
// Copyright (C) 2004 Forgotten and the VBA development team

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

#define gzFile void* //ichfly
#define voidp void*

#include <nds.h>
#include <stdio.h>
#include "GBA.h"

#ifndef VBA_UTIL_H
#define VBA_UTIL_H
enum IMAGE_TYPE {
  IMAGE_UNKNOWN = -1,
  IMAGE_GBA     = 0,
  IMAGE_GB      = 1
};

// save game

typedef struct {
  void *address;
  int size;
} variable_desc;


//fs dir
#define entriesPerList (int)(30)

//heap/vram alloc defs
typedef int sint32;
#define vramSize (sint32)(128*1024*3)	//3 128K VRAM blocks free currently
#define vramBlockA (uint32)(0xa)
#define vramBlockB (uint32)(0xb)
#define vramBlockC (uint32)(0xc)
#define vramBlockD (uint32)(0xd)

#define HeapSize (uint32)(128*1024)
#define HeapBlock (uint32)(0xe)

#endif


#ifdef __cplusplus
extern "C"{
#endif


void generatefilemap(int size);

extern bool utilWritePNGFile(const char *, int, int, u8 *);
extern bool utilWriteBMPFile(const char *, int, int, u8 *);
extern void utilApplyIPS(const char *ips, u8 **rom, int *size);
extern void utilWriteBMP(char *, int, int, u8 *);
extern bool utilIsGBAImage(const char *);
extern bool utilIsSAV(const char * file);
extern bool utilIsGBImage(const char *);
extern bool utilIsZipFile(const char *);
extern bool utilIsGzipFile(const char *);
extern bool utilIsRarFile(const char *);
extern void utilGetBaseName(const char *, char *);
//extern IMAGE_TYPE utilFindType(const char *);
extern void utilPutDword(u8 *, u32);
extern void utilPutWord(u8 *, u16);
extern void utilWriteData(gzFile, variable_desc *);
extern void utilReadData(gzFile, variable_desc *);
extern int utilReadInt(gzFile);
extern void utilWriteInt(gzFile, int);
extern gzFile utilGzOpen(const char *file, const char *mode);
extern gzFile utilMemGzOpen(char *memory, int available, char *mode);
extern int utilGzWrite(gzFile file, const voidp buffer, unsigned int len);
extern int utilGzRead(gzFile file, voidp buffer, unsigned int len);
extern int utilGzClose(gzFile file);
extern long utilGzMemTell(gzFile file);
extern void utilGBAFindSave(const u8 *, const int);
extern void utilUpdateSystemColorMaps();
extern u32 anytimejmpfilter;


extern u32 CPUReadMemoryrealpu(u32 address);
extern u8 CPUReadByterealpu(u32 address);

extern void CPUWriteMemorypu(u32 address, u32 value);
extern void CPUWriteHalfWordpu(u32 address, u16 value);
extern void CPUWriteBytepu(u32 address, u8 b);

extern void CPUWriteMemory(u32 address, u32 value);
extern void CPUWriteHalfWord(u32 address, u16 value);
extern void CPUWriteByte(u32 address, u8 b);

extern u32 CPUReadMemoryrealpu(u32 address);
extern u32 CPUReadHalfWordrealpu(u32 address);
extern u16 CPUReadHalfWordrealpuSigned(u32 address);
extern u8 CPUReadByterealpu(u32 address);

extern u32 CPUReadMemoryreal(u32 address);
extern u32 CPUReadHalfWordreal(u32 address);
extern u16 CPUReadHalfWordSigned(u32 address);
extern u8 CPUReadBytereal(u32 address);

extern u32 CPUReadMemory(u32 address);
extern u32 CPUReadHalfWord(u32 address);
extern u8  CPUReadByte(u32 address);


extern const u8 minilut[0x10];
extern u8 lutu16bitcnt(u16 x);
extern u8 lutu32bitcnt(u32 x);
extern u8 clzero(u32 var);
extern char* strtoupper(char* s);
extern char* strtolower(char* s);

extern bool save_deciderByTitle(char * headerTitleSource, char * headerTitleHaystack, int SizeToCheck);
extern int save_decider();

extern bool pendingSaveFix;	//false if already saved new forked save / or game doesn't meet savefix conditions // true if pending a save that was fixed in gba core, but still has not been written/updated to file.
extern int  SaveSizeBeforeFix;	//only valid if pendingSaveFix == true
extern int  SaveSizeAfterFix;	//only valid if pendingSaveFix == true

//static global instance of actual ARM core volatile registers.
extern reg_pair * myregs;

//heap/vram alloc parts
extern uint32 getVRAMHeapStart();
extern sint32 vramABlockOfst;	//offset pointer to free memory, user alloced memory is (baseAddr + (sizeAlloced - vramBlockPtr))
extern sint32 vramBBlockOfst;
extern sint32 vramCBlockOfst;
extern sint32 vramDBlockOfst;
extern sint32 HeapBlockOfst;

extern uint32 * vramHeapAlloc(uint32 vramBlock,uint32 StartAddr,int size);
extern uint32 * vramHeapFree(uint32 vramBlock,uint32 StartAddr,int size);

extern void patchit(int romSize2);
extern u16 swap16(u16 v);
extern u32 swap32(u32 v);

extern bool reloadGBA(char * filename, u32 manual_save_type);
extern bool ShowBrowser();
extern char * bufNames[entriesPerList][512];

extern u16 READ16LE(u16 * x);
extern u32 READ32LE(u32 * x);
extern void WRITE16LE(u16 * x,u16 v);
extern void WRITE32LE(u32 * x, u32 v);
extern void UPDATE_REG(u16 address, u16 value);

extern bool useMPUFast;

#ifdef __cplusplus
}
#endif
