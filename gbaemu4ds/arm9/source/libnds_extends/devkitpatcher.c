#include <nds/ndstypes.h>
#include <nds/memory.h>//#include <memory.h> ichfly
#include <nds/timers.h>
#include <nds/dma.h>
#include <nds/ipc.h>
#include <nds/arm9/video.h>
#include <nds/arm9/sprite.h>
#include <nds/arm9/input.h>
#include <nds/arm9/videoGL.h>
#include <nds/arm9/trig_lut.h>
#include <nds/system.h>
#include <nds/interrupts.h>
#include <nds/fifocommon.h>
#include <time.h>
#include <sys/iosupport.h>
#include <math.h>
#include <nds/ndstypes.h>
#include <nds/bios.h>
#include <nds/system.h>
#include <nds/arm9/math.h>
#include <nds/arm9/sassert.h>
#include <stdlib.h>

#include "libnds_internal.h"
#include "../arm7sound.h"
#include "../ichflysettings.h"
#include "../GBA.h"
#include "../bios.h"
#include "../Globals.h"

static time_t *punixTime;

//---------------------------------------------------------------------------------
// Reset the DS registers to sensible defaults
//---------------------------------------------------------------------------------
void initSystem(void) {
//---------------------------------------------------------------------------------
	register int i;
	// stop timers and dma
	for (i=0; i<4; i++) 
	{
		DMA_CR(i) = 0;
		DMA_SRC(i) = 0;
		DMA_DEST(i) = 0;
		TIMER_CR(i) = 0;
		TIMER_DATA(i) = 0;
	}


	// clear video display registers
	dmaFillWords(0, (void*)0x04000000, 0x56);
	dmaFillWords(0, (void*)0x04001008, 0x56);

	videoSetModeSub(0);

	vramDefault();

	VRAM_E_CR = 0;
	VRAM_F_CR = 0;
	VRAM_G_CR = 0;
	VRAM_H_CR = 0;
	VRAM_I_CR = 0;

	irqInit();
	//fifoInit(); //ichfly
	ichflyfifoinit();

	//fifoSetValue32Handler(FIFO_SYSTEM, systemValueHandler, 0); //not needed
	//fifoSetDatamsgHandler(FIFO_SYSTEM, systemMsgHandler, 0);

	punixTime = (time_t*)memUncached((void *)&__transferRegion()->unixTime);

	//__syscalls.exit = __libnds_exit;
	extern  char *fake_heap_end;
	__transferRegion()->bootcode = (struct __bootstub *)fake_heap_end;
	//irqEnable(IRQ_VBLANK);

}
void ichflyfifoinit()
{
	REG_IPC_SYNC = 0;
	REG_IPC_FIFO_CR = IPC_FIFO_ENABLE;
	REG_IPC_FIFO_CR = IPC_FIFO_SEND_CLEAR | IPC_FIFO_ENABLE;
	REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_RECV_IRQ | IPC_FIFO_ERROR;
}

void triggerdmairq(u8 num) //tigger an dma irq if dma is free
{
	DMA_SRC(num) = 0x1100000; //don't care about that it is open by pu and not writeabel by the dma
	DMA_DEST(num) = 0x1200000; //don't care about that it is open by pu and not writeabel by the dma
	DMA_CR(num) = DMA_ENABLE | DMA_IRQ_REQ | DMA_START_NOW | DMA_16_BIT | DMA_SRC_INC | DMA_DST_INC | 1;
}