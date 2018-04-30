#ifndef ipc_arm7
#define ipc_arm7

#include <nds.h>

#endif

#ifdef __cplusplus
extern "C"{
#endif

extern volatile void handlefifo();
extern volatile void ipc7_handler(u32 command1,u32 command2,u32 command3,u32 command4);
#ifdef __cplusplus
}
#endif
