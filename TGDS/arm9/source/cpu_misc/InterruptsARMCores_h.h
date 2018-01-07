/*

			Copyright (C) 2017  Coto
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301
USA

*/

#ifndef __interruptsARMCores_h__
#define __interruptsARMCores_h__

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "linkerTGDS.h"
#include "ipcfifoTGDS.h"

#ifdef ARM9

#define SWI_CHECKBITS *((uint32*)&_arm9_irqcheckbits)

#endif
#endif
#ifdef __cplusplus
extern "C"{
#endif

//project specific here

#ifdef ARM7
extern u32 interrupts_to_wait_arm7;
#endif

#ifdef ARM9
extern volatile u32 interrupts_to_wait_arm9;
#endif

// Common

//weak symbols : the implementation of these is project-defined
extern __attribute__((weak))	void HblankUser();
extern __attribute__((weak))	void VblankUser();
extern __attribute__((weak))	void VcounterUser();

extern __attribute__((weak))	void Timer0handlerUser();
extern __attribute__((weak))	void Timer1handlerUser();
extern __attribute__((weak))	void Timer2handlerUser();
extern __attribute__((weak))	void Timer3handlerUser();

//weak symbols end

extern u32 getIRQs();
extern void EnableIrq(u32 IRQ);
extern void DisableIrq(u32 IRQ);
extern void NDS_IRQHandlerSpecial();
extern void IRQWait(u32 reentrant,u32 irqstowait);
extern void IRQVBlankWait();
extern void IRQInit();
extern void InterruptServiceRoutineARMCoresSpecial();	//Actual Interrupt Handler

//stubs because we don't disable IME on DSWIFI
extern void SGIP_INTR_PROTECT();
extern void SGIP_INTR_REPROTECT();
extern void SGIP_INTR_UNPROTECT();
extern void SGIP_WAITEVENT();


extern void HandleFifo();
extern void fifo9_handler(u32 command1,u32 command2,u32 command3,u32 command4);

#ifdef __cplusplus
}
#endif
