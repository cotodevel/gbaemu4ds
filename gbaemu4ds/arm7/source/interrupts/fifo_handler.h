
#ifndef __snemuldsv6_fifohandler_
#define __snemuldsv6_fifohandler_

//#include <nds.h>
#include <nds/ndstypes.h>

#ifdef __cplusplus
extern "C"{
#endif

extern void newvalwrite(u32 addr, u32 val, u32 cmd0, u32 command3, u32 command4);
extern void fifo_handler();
extern bool autodetectdetect;

#ifdef __cplusplus
}
#endif

#endif