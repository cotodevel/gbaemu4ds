.arch	armv4t
.cpu arm7tdmi
	
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
.section	.itcm,"ax",%progbits
.align 4
.arm

#include "../ichflysettings.h"
.extern	irqTable

.global	IntrMain, __cpsr_mask
.type   IntrMain STT_FUNC
@---------------------------------------------------------------------------------
IntrMain:
@---------------------------------------------------------------------------------
	mov	r12, #0x4000000		@ REG_BASE
		
	add	r12, r12, #0x210
	ldmia	r12, {r1,r2}
	ands	r1, r1, r2
	ldr	r2, =irqTable


@---------------------------------------------------------------------------------
findIRQ:
@---------------------------------------------------------------------------------
	ldr r0, [r2, #4]
	cmp	r0, #0
	beq	no_handler
	ands	r0, r0, r1
	bne	jump_intr
	add	r2, r2, #8
	b	findIRQ

@---------------------------------------------------------------------------------
no_handler:
@---------------------------------------------------------------------------------
	mov	pc,lr

@---------------------------------------------------------------------------------
jump_intr:
@---------------------------------------------------------------------------------
	ldr	r1, [r2]		@ user IRQ handler address
	cmp	r1, #0
	bne	got_handler
	@mov	r1, r0
	b	no_handler
@---------------------------------------------------------------------------------
got_handler:
@---------------------------------------------------------------------------------
	mrs	r12, spsr
	ldr r0,=__sp_irq
	stmfd	r0!, {r12,SP,lr}	@ {spsr,SP, lr_irq}
	mov SP,r0
	mov lr,pc
	bx	r1
	
exitichfly:
	ldmfd   SP, {r0,SP,lr}	@ {spsr,SP, lr_irq}
	msr	spsr, r0		@ restore spsr
	mov	pc,lr

	.pool
	.end
