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

#ifndef __main9_h__
#define __main9_h__

#include "typedefs.h"
#include "dsregs.h"
#include "dsregs_asm.h"

#include "gbaemu4ds_fat_ext.h"

#define GBA_EWRAM ((void*)(0x02000000))

#define public

//#define MAXPATHLEN 256 
#include <stdio.h>
#define Log(...) printf(__VA_ARGS__)

//extern volatile u8 arm7exchangefild[0x100];


#endif

#ifdef __cplusplus
extern "C" {
#endif

extern char biosPath[MAXPATHLEN * 2];
extern char patchPath[MAXPATHLEN * 2];
extern char savePath[MAXPATHLEN * 2];
extern char szFile[MAXPATHLEN * 2];
extern char* savetypeschar[7];
extern char* memoryWaitrealram[8];

extern int argc;
extern sint8 **argv;
extern int main(int _argc, sint8 **_argv);

#ifdef __cplusplus
}
#endif