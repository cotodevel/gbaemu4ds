
#ifndef __snemuldsv6_fifohandler_
#define __snemuldsv6_fifohandler_

//#include <nds.h>
#include <nds/ndstypes.h>

#ifdef __cplusplus
extern "C"{
#endif


extern void HandleFifo();

extern void fifo7_handler(u32 command1,u32 command2,u32 command3,u32 command4);

#ifdef __cplusplus
}
#endif

#endif