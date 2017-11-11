@
@			Copyright (C) 2017  Coto
@This program is free software; you can redistribute it and/or modify
@it under the terms of the GNU General Public License as published by
@the Free Software Foundation; either version 2 of the License, or
@(at your option) any later version.

@This program is distributed in the hope that it will be useful, but
@WITHOUT ANY WARRANTY; without even the implied warranty of
@MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
@General Public License for more details.

@You should have received a copy of the GNU General Public License
@along with this program; if not, write to the Free Software
@Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301
@USA
@


#ifdef ARM7
.text
#endif

#ifdef ARM9
.section .itcm,"ax",%progbits
#endif

.arm
.code 32

#include "dsregs_asm.h"

.global 	InterruptServiceRoutineARMCoresSpecial, __cpsr_mask
InterruptServiceRoutineARMCoresSpecial:

push {r0-r12,lr}

@only treat interrupts when IME = 1
mov	r12, #0x4000000		
ldr	r1, [r12, #0x208]	
cmp	r1, #0
popeq {r0-r12,lr} 
bxeq	lr

mov	r0, #0
str	r0, [r12, #0x208]	@REG_IME = 0;

push {r0,r1,r12,lr}
bl NDS_IRQHandlerSpecial
pop {r0,r1,r12,lr}

str r1,[r12,#0x208]	@REG_IME = 1;

pop {r0-r12,lr}
bx lr



.pool
.end