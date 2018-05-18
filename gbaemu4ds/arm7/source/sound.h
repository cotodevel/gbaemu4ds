#ifndef __sound_gba_7__
#define __sound_gba_7__


#include <nds.h>

#define 	REG_FIFOA   *(vu32*)(0x04000000+0x00A0)
#define 	REG_FIFOB   *(vu32*)(0x04000000+0x00A4)
#define 	REG_DMA0CNT_L   *(vu16*)(0x04000000+0x00B8)
#define 	REG_DMA0CNT_H   *(vu16*)(0x04000000+0x00BA)
#define 	REG_DMA1CNT_L   *(vu16*)(0x04000000+0x00C4)
#define 	REG_DMA1CNT_H   *(vu16*)(0x04000000+0x00C6)
#define 	REG_DMA2CNT_L   *(vu16*)(0x04000000+0x00D0)
#define 	REG_DMA2CNT_H   *(vu16*)(0x04000000+0x00D2)
#define 	REG_DMA3CNT_L   *(vu16*)(0x04000000+0x00DC)
#define 	REG_DMA3CNT_H   *(vu16*)(0x04000000+0x00DE)
#define 	REG_TM0CNT_L   *(vu16*)(0x04000000+0x0100)
#define 	REG_TM0CNT_H   *(vu16*)(0x04000000+0x0102)
#define 	REG_TM1CNT_L   *(vu16*)(0x04000000+0x0104)
#define 	REG_TM1CNT_H   *(vu16*)(0x04000000+0x0106)
#define 	REG_TM2CNT_L   *(vu16*)(0x04000000+0x0108)
#define 	REG_TM2CNT_H   *(vu16*)(0x04000000+0x010a)
#define 	REG_TM3CNT_L   *(vu16*)(0x04000000+0x010c)
#define 	REG_TM3CNT_H   *(vu16*)(0x04000000+0x010e)


#endif




#ifdef __cplusplus
extern "C" {
#endif

extern u8 DMA_A_TIMERSEL;
extern u8 DMA_B_TIMERSEL;
extern void timer0interrupt_thread();
extern void timer1interrupt_thread();

extern void updateInternalDMAFIFO(bool channelA, u32 value, u32 datatype, u32 fifowriteDataType );
extern void doFIFOUpdate();