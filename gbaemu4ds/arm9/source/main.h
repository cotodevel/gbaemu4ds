#ifndef __MAIN_H__
#define __MAIN_H__

#include <nds.h>


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


#endif /*__MAIN_H__*/
