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

/************************************************************************/
/* Arm/Thumb command set disassembler                                   */
/************************************************************************/

#ifndef __ARMDIS_H__
#define __ARMDIS_H__

#include <nds.h>

#define DIS_VIEW_ADDRESS 1
#define DIS_VIEW_CODE 2

struct Opcodes {
  u32 mask;
  u32 cval;
  char *mnemonic;
};

#define debuggerReadMemory(addr) \
  READ32LE(((u32*)&map[(addr)>>24].address[(addr) & map[(addr)>>24].mask]))

#define debuggerReadHalfWord(addr) \
  READ16LE(((u16*)&map[(addr)>>24].address[(addr) & map[(addr)>>24].mask]))

#define debuggerReadByte(addr) \
  map[(addr)>>24].address[(addr) & map[(addr)>>24].mask]


#endif // __ARMDIS_H__


#ifdef __cplusplus
extern "C"{
#endif

extern int disThumb(u32 offset, char *dest, int flags);
extern int disArm(u32 offset, char *dest, int flags);

extern struct Opcodes armOpcodes[];
extern struct Opcodes thumbOpcodes[];
extern char *armMultLoadStore[12];
extern char *shifts[5];
extern char *conditions[16];
extern char *regs[16];
extern char *decVals[16];
extern char hdig[];

#ifdef __cplusplus
}
#endif
