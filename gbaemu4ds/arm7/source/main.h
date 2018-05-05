#ifndef gbaemu4ds_arm7_main
#define gbaemu4ds_arm7_main

#include <nds.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern u16 callline;

extern u8* soundbuffA;
extern u8* soundbuffB;

extern u8 dmaApart;
extern u8 dmaBpart;

extern u32 dmabuffer;

extern u16 SOUNDCNT_L;
extern u16 SOUNDCNT_H;

extern u8 tacktgeber_sound_FIFO_DMA_A;
extern u8 tacktgeber_sound_FIFO_DMA_B;
extern u16 TM0CNT_L;
extern u16 TM1CNT_L;

extern u16 TM0CNT_H;
extern u16 TM1CNT_H;

extern u16 DMA1CNT_H;
extern u16 DMA2CNT_H;


extern u16 DMA1SAD_L;
extern u16 DMA1SAD_H;
extern u16 DMA1DAD_L;
extern u16 DMA1DAD_H;

extern u16 DMA2SAD_L;
extern u16 DMA2SAD_H;
extern u16 DMA2DAD_L;
extern u16 DMA2DAD_H;


//debug stuff

extern vu32 debugsrc1;
extern vu32 debugsrc2;
extern vu32 debugfr1;
extern vu32 debugfr2;

extern void vcount_handler();

extern u32 power;
extern u32 ie_save;
extern void lid_open_irq_handler();
extern void lid_closing_handler(u32 WAKEUP_IRQS);

#ifdef __cplusplus
}
#endif
