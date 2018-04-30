/*---------------------------------------------------------------------------------

	Copyright (C) 2005
		Dave Murphy (WinterMute)

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any
	damages arising from the use of this software.

	Permission is granted to anyone to use this software for any
	purpose, including commercial applications, and to alter it and
	redistribute it freely, subject to the following restrictions:

	1.	The origin of this software must not be misrepresented; you
		must not claim that you wrote the original software. If you use
		this software in a product, an acknowledgment in the product
		documentation would be appreciated but is not required.
	2.	Altered source versions must be plainly marked as such, and
		must not be misrepresented as being the original software.
	3.	This notice may not be removed or altered from any source
		distribution.

---------------------------------------------------------------------------------*/

#include <nds/interrupts.h>
#include <nds/system.h>
#include <nds/ipc.h>

#include "arm9_interrupts.h"
#include "main.h"

//coto: these re-use libnds irq functionality but since some steps must be added to standard IRQ libs, we create a standalone version.

// Prototype for assembly interrupt dispatcher
//---------------------------------------------------------------------------------
void irqDummy_gba(void){}
//---------------------------------------------------------------------------------


#ifdef ARM9
	u32 IEBACKUP = 0;  
#else
	#define INT_TABLE_SECTION
#endif

//---------------------------------------------------------------------------------
void __irqSet_gba(u32 mask, IntFn handler, struct IntTable irqTable[] ) {
//---------------------------------------------------------------------------------
	if (!mask) return;

	int i;

	for	(i=0;i<MAX_INTERRUPTS;i++)
		if	(!irqTable[i].mask || irqTable[i].mask == mask) break;

	if ( i == MAX_INTERRUPTS ) return;

	irqTable[i].handler	= handler;
	irqTable[i].mask	= mask;
}

//---------------------------------------------------------------------------------
void irqSet_gba(u32 mask, IntFn handler) {
//---------------------------------------------------------------------------------
	int oldIME = enterCriticalSection();
	__irqSet_gba(mask,handler,irqTable);
	if(mask & IRQ_VBLANK)
		REG_DISPSTAT |= DISP_VBLANK_IRQ ;
	if(mask & IRQ_HBLANK)
		REG_DISPSTAT |= DISP_HBLANK_IRQ ;
	if(mask & IRQ_IPC_SYNC)
		REG_IPC_SYNC |= IPC_SYNC_IRQ_ENABLE;
	leaveCriticalSection(oldIME);
}

//---------------------------------------------------------------------------------
void irqInit_gba() {
//---------------------------------------------------------------------------------
	int i;

	REG_IE	= 0;			// disable all interrupts

	// Set all interrupts to dummy functions.
	for(i = 0; i < MAX_INTERRUPTS; i ++)
	{
		irqTable[i].handler = irqDummy_gba;
		irqTable[i].mask = 0;
	}

	IRQ_HANDLER = IntrMain_gba;

#ifdef ARM7
	irqSetAUX(IRQ_I2C, i2cIRQHandler);
	irqEnableAUX(IRQ_I2C);
#endif
	REG_IF	= IRQ_ALL;		// clear all pending interrupts
	REG_IME = 1;			// enable global interrupt

}


//---------------------------------------------------------------------------------
void irqInitHandler_gba(IntFn handler) {
//---------------------------------------------------------------------------------
	REG_IME = 0;
	REG_IF = ~0;
	REG_IE = 0;

	IRQ_HANDLER = handler;

	REG_IME = 1;
}

//---------------------------------------------------------------------------------
void irqEnable_gba(uint32 irq) {
//---------------------------------------------------------------------------------
	int oldIME = enterCriticalSection();
	if (irq & IRQ_VBLANK)
		REG_DISPSTAT |= DISP_VBLANK_IRQ ;
	if (irq & IRQ_HBLANK)
		REG_DISPSTAT |= DISP_HBLANK_IRQ ;
	if (irq & IRQ_VCOUNT)
		REG_DISPSTAT |= DISP_YTRIGGER_IRQ;
	if(irq & IRQ_IPC_SYNC)
		REG_IPC_SYNC |= IPC_SYNC_IRQ_ENABLE;

	REG_IE |= irq;
	leaveCriticalSection(oldIME);
}

//---------------------------------------------------------------------------------
void irqDisable_gba(uint32 irq) {
//---------------------------------------------------------------------------------
	int oldIME = enterCriticalSection();
	if (irq & IRQ_VBLANK)
		REG_DISPSTAT &= ~DISP_VBLANK_IRQ ;
	if (irq & IRQ_HBLANK)
		REG_DISPSTAT &= ~DISP_HBLANK_IRQ ;
	if (irq & IRQ_VCOUNT)
		REG_DISPSTAT &= ~DISP_YTRIGGER_IRQ;
	if(irq & IRQ_IPC_SYNC)
		REG_IPC_SYNC &= ~IPC_SYNC_IRQ_ENABLE;

	REG_IE &= ~irq;
	leaveCriticalSection(oldIME);
}

//---------------------------------------------------------------------------------
void __irqClear_gba(u32 mask, struct IntTable irqTable[]) {
//---------------------------------------------------------------------------------
	int i = 0;

	for	(i=0;i<MAX_INTERRUPTS;i++)
		if	(irqTable[i].mask == mask) break;

	if ( i == MAX_INTERRUPTS ) return;

	irqTable[i].handler	= irqDummy_gba;
}

//---------------------------------------------------------------------------------
void irqClear_gba(u32 mask) {
//---------------------------------------------------------------------------------
	int oldIME = enterCriticalSection();
	__irqClear_gba(mask,irqTable);
	irqDisable_gba( mask);
	leaveCriticalSection(oldIME);
}
