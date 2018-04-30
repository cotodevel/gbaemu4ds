#include <nds.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void IntrMain_gba();
extern void irqDummy_gba(void);

//IEBACKUP: saves the NDS IE so they can be re-entrant later
#ifdef ARM9
	extern u32 IEBACKUP;
#else
	#define INT_TABLE_SECTION
#endif

extern void __irqSet_gba(u32 mask, IntFn handler, struct IntTable irqTable[] );
extern void irqSet_gba(u32 mask, IntFn handler);
extern void irqInit_gba();
extern void irqInitHandler_gba(IntFn handler);
extern void irqEnable_gba(uint32 irq);
extern void irqDisable_gba(uint32 irq);
extern void __irqClear_gba(u32 mask, struct IntTable irqTable[]);
extern void irqClear_gba(u32 mask);


#ifdef __cplusplus
}
#endif
