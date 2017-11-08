/*

			Copyright (C) 2017  Coto
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301
USA

*/

#ifndef __gui_console_h__
#define __gui_console_h__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <malloc.h>
#include <ctype.h>
#include "specific_shared.h"

#include "gui.h"
#include "gbaemu4ds_fat_ext.h"
#include "bios.h"

#include "typedefs.h"
#include "dsregs.h"
#include "console.h"
#include "gui_widgets.h"
#include "console_str.h"
#include "about.h"
#include "dma.h"
#include "dmaIO.h"
#include "InterruptsARMCores_h.h"
#include "posix_hook_shared.h"
#include "fsfat_layer.h"
#include "keypad.h"
#include "video.h"

#endif

#ifdef __cplusplus
extern "C" {
#endif

//Definition that overrides the weaksymbol expected from toolchain to init console video subsystem
extern vramSetup * getProjectSpecificVRAMSetup();

//Custom console VRAM layout setup

//1) VRAM Layout
extern bool InitProjectSpecificConsole();

//2) Uses subEngine: VRAM Layout -> Console Setup
extern vramSetup * GBAEMU4DS_2DVRAM_SETUP();

extern void GUI_getROM(sint8 *rompath);
extern void GUI_deleteROMSelector();
extern int FirstROMSelectorHandler(t_GUIZone *zone, int msg, int param, void *arg);

#ifdef __cplusplus
}
#endif
