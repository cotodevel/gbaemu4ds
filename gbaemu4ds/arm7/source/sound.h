#ifndef __sound_gba_7__
#define __sound_gba_7__


#include <nds.h>


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