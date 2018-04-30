#ifndef ipc_arm9
#define ipc_arm9

#include <nds.h>

#endif


#ifdef __cplusplus
extern "C" {
#endif

extern inline void handlefifo();
extern inline void ipc9_handler(u32 command1,u32 command2,u32 command3,u32 command4);

extern u8 recv_buf[0x1000];

#ifdef __cplusplus
}
#endif
