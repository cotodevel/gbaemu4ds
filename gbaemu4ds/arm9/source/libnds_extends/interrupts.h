#ifndef gbaemu4ds_libndsinterrupts
#define gbaemu4ds_libndsinterrupts

#include <nds/interrupts.h>
#include <nds/system.h>
#include <nds/ipc.h>
#ifdef ARM7
#include <nds/arm7/i2c.h>
#endif

#endif

#ifdef __cplusplus
extern "C" {
#endif

extern void IntrMain();
extern void irqDummy();
extern void __irqSet(u32 mask, IntFn handler, struct IntTable irqTable[] );
extern void __irqClear(u32 mask, struct IntTable irqTable[]);

#ifdef __cplusplus
}
#endif
