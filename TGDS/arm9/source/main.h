#ifndef __MAIN_H__
#define __MAIN_H__

#include <nds.h>


#include <stdio.h>
#define Log(...) printf(__VA_ARGS__)

#define MAXPATHLEN 256 

extern char biosPath[MAXPATHLEN * 2];

extern char patchPath[MAXPATHLEN * 2];

extern char savePath[MAXPATHLEN * 2];

extern char szFile[MAXPATHLEN * 2];

extern int main( int argc, char **argv);

//extern volatile u8 arm7exchangefild[0x100];

#endif /*__MAIN_H__*/
