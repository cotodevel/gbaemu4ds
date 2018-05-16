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
#ifndef VBA_GBAinline_H
#define VBA_GBAinline_H


#include "fatfileextract.h"
#include "agbprint.h"
#include "System.h"
#include "Port.h"
#include "RTC.h"
#include "GBA.h"

#include "ichflysettings.h"
#include "main.h"

#include <nds/interrupts.h>

extern bool cpuSramEnabled;
extern bool cpuFlashEnabled;
extern bool cpuEEPROMEnabled;
extern bool cpuEEPROMSensorEnabled;
extern bool cpuDmaHack;
extern u32 cpuDmaLast;
extern bool timer0On;
extern bool timer1On;
extern bool timer2On;
extern bool timer3On;
extern int cpuTotalTicks;
extern void Logsd(const char *defaultMsg,...);

#endif //VBA_GBAinline_H