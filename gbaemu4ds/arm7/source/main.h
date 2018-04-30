#ifndef gbaemu4ds_arm7_main
#define gbaemu4ds_arm7_main

#include <nds.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern bool vblank_gba_period;

extern u16 callline;

extern u8* soundbuffA;
extern u8* soundbuffB;


extern u8 DMA_A_TIMERSEL;
extern u8 DMA_B_TIMERSEL;

//debug stuff

//extern vu32 debugsrc1;
//extern vu32 debugsrc2;
//extern vu32 debugfr1;
//extern vu32 debugfr2;

extern void hblank_handler();
extern void vblank_handler();
extern void vcount_handler();
extern void timer0interrupt_thread();

extern bool ykeypp;
extern bool isincallline;

extern void iowrite(u32 addr,u32 val);

#ifdef __cplusplus
}
#endif
