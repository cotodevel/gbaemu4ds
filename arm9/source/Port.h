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

#ifndef VBA_PORT_H
#define VBA_PORT_H

//little-endian GBA == little-endian NDS
#define READ16LE(x) \
  *((u16 *)x)
#define READ32LE(x) \
  *((u32 *)x)
#define WRITE16LE(x,v) \
  *((u16 *)x) = (v)
#define WRITE32LE(x,v) \
  *((u32 *)x) = (v)

#define Log(...) iprintf(__VA_ARGS__)


#endif	//VBA_PORT_H end