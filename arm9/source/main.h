#ifndef __MAIN_H__
#define __MAIN_H__

#include <nds.h>
#include <nds/interrupts.h>
#include <nds/system.h>
#include <nds/ipc.h>

#include <nds/arm9/dldi.h>
#include <stdio.h>
#define INT_TABLE_SECTION __attribute__((section(".dtcm")))

#define MAXPATHLEN 256 	//libfat directory path length

#endif /*__MAIN_H__*/


#ifdef __cplusplus
extern "C" {
#endif

extern char biosPath[MAXPATHLEN * 2];
extern char patchPath[MAXPATHLEN * 2];
extern char savePath[MAXPATHLEN * 2];
extern char szFile[MAXPATHLEN * 2];

//libnds
extern void irqDummy(void);
extern struct IntTable irqTable[MAX_INTERRUPTS] INT_TABLE_SECTION;
extern void __irqSet(u32 mask, IntFn handler, struct IntTable irqTable[] );

// The only built in driver
extern DLDI_INTERFACE _io_dldi_stub;

extern u8 arm7exchangefild[0x100];
#ifdef __cplusplus
}
#endif
