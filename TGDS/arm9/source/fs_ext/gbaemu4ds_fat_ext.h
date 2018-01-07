#ifndef __gbaemu4dsfatext_h__
#define __gbaemu4dsfatext_h__

#include "typedefsTGDS.h"
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

#include "GBA.h"
#include "guiTGDS.h"
#include "nds_cp15_misc.h"
#include "dldi.h"
#include "fsfatlayerTGDS.h"
#include "fileHandleTGDS.h"
#include "InterruptsARMCores_h.h"
#include "specific_shared.h"
#include "ff.h"
#include "memoryHandleTGDS.h"
#include "reent.h"
#include "sys/types.h"
#include "consoleTGDS.h"
#include "utilsTGDS.h"
#include "devoptab_devices.h"
#include "posixHandleTGDS.h"
#include "about.h"
#include "xenofunzip.h"
#include "ichflysettings.h"

#define MAXPATHLEN (sint32)(256)

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

extern int PosixStructFDLastLoadFile;	//Coto: a StructFD non-posix file descriptor having the handle for the FILE * opened at the time the sector table map was generated.
										//TGDS uses this to access the FSFAT level file attributes
extern u32 current_pointer;
extern u32 allocedfild[buffslots];
extern u8* greatownfilebuffer;

extern void generatefilemap(FILE * f,int size);
extern void getandpatchmap(int offsetgba,int offsetthisfile,FILE * fhandle);

//part of gbaemu4ds_fat_ext.cpp

extern u8 ichfly_readu8(unsigned int pos);
extern u16 ichfly_readu16(unsigned int pos);
extern u32 ichfly_readu32(unsigned int pos);
extern void ichfly_readdma_rom(u32 pos,u8 *ptr,u32 c,int readal);

extern void testGBAEMU4DSFSTGDS(FILE * f,sint32 fileSize);
#ifdef __cplusplus
}
#endif


