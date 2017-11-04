#ifndef __gbaemu4dsfatext_h__
#define __gbaemu4dsfatext_h__

#include "typedefs.h"
#include "dsregs.h"
#include "dsregs_asm.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/dir.h>
#include <fcntl.h>

#include "fs.h"
#include "gui.h"
#include "nds_cp15_misc.h"

#include "fsfat_layer.h"
#include "file.h"
#include "InterruptsARMCores_h.h"
#include "specific_shared.h"
#include "ff.h"
#include "mem_handler_shared.h"
#include "reent.h"
#include "sys/types.h"
#include "console.h"
#include "toolchain_utils.h"
#include "devoptab_devices.h"
#include "posix_hook_shared.h"
#include "about.h"
#include "xenofunzip.h"

#endif

#ifdef __cplusplus
extern "C" {
#endif

//part of fatfile.c
extern u32 *sectortabel;
extern void * lastopen;
extern void * lastopenlocked;

/*
PARTITION* partitionlocked;
FN_MEDIUM_READSECTORS	readSectorslocked;
*/
extern u32 current_pointer = 0;
extern u32 allocedfild[buffslots];
extern u8* greatownfilebuffer;

extern void generatefilemap(int size);
extern void getandpatchmap(int offsetgba,int offsetthisfile);

//part of fatfileextract.cpp

extern u8 ichfly_readu8(unsigned int pos);
extern u16 ichfly_readu16(unsigned int pos);
extern u32 ichfly_readu32(unsigned int pos);
extern void ichfly_readdma_rom(u32 pos,u8 *ptr,u32 c,int readal);

#ifdef __cplusplus
}
#endif


