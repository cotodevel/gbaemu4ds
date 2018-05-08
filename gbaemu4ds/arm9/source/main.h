#ifndef __MAIN_H__
#define __MAIN_H__

#include <nds.h>
#include <nds/interrupts.h>
#include <nds/system.h>
#include <nds/ipc.h>

#ifdef ARM7
#include <nds/arm7/i2c.h>
#endif



#include <stdio.h>
#define Log(...) iprintf(__VA_ARGS__)

#define MAXPATHLEN 256 

extern char biosPath[MAXPATHLEN * 2];

extern char patchPath[MAXPATHLEN * 2];

extern char savePath[MAXPATHLEN * 2];

extern char szFile[MAXPATHLEN * 2];

extern u8 var8;
extern u16 var16;
extern u32 var32;

//libnds
extern void irqDummy(void);
extern struct IntTable irqTable[MAX_INTERRUPTS];
//extern struct IntTable irqTable[MAX_INTERRUPTS] INT_TABLE_SECTION;

#ifdef INT_TABLE_SECTION
#else
extern struct IntTable irqTable[MAX_INTERRUPTS];
#endif


#endif /*__MAIN_H__*/