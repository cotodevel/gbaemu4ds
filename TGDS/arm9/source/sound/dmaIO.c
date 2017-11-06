#include "dmaIO.h"
#include "socket.h"
#include "in.h"
#include <netdb.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "main.h"
#include "InterruptsARMCores_h.h"
#include "specific_shared.h"
#include "ff.h"
#include "mem_handler_shared.h"
#include "reent.h"
#include "sys/types.h"
#include "dsregs.h"
#include "dsregs_asm.h"
#include "typedefs.h"
#include "console.h"
#include "toolchain_utils.h"
#include "devoptab_devices.h"
#include "fsfat_layer.h"
#include "usrsettings.h"
#include "video.h"
#include "keypad.h"
#include "gui.h"
#include "dswnifi_lib.h"

#include "dma.h"


//DMA Interface from ARM9 -> ARM7 here



void triggerdmairq(u8 num) //tigger an dma irq if dma is free
{
	DMAXSAD(num) = 0x1100000; //don't care about that it is open by pu and not writeabel by the dma
	DMAXDAD(num) = 0x1200000; //don't care about that it is open by pu and not writeabel by the dma
	DMAXCNT(num) = DMAENABLED | DMARAISEIRQ | DMASTART_INMEDIATE | DMA16BIT | DMAINCR_SRC | DMAINCR_DEST | 1; //DMA_ENABLE | DMA_IRQ_REQ | DMA_START_NOW | DMA_16_BIT | DMA_SRC_INC | DMA_DST_INC | 1;
}
