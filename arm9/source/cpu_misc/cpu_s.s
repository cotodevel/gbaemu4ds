	.arch	armv4t
	.cpu arm7tdmi
	
	.section	.itcm,"ax",%progbits
	.align 4
	.arm

	.global readbankedsp
   .type   readbankedsp STT_FUNC
readbankedsp:
	push {r1-r4}
	
	mrs	r3, cpsr
	bic	r4, r3, #0x1F
	and	r0, r0, #0x1F
	
	cmp r0,#0x10 @ichfly user is system
	moveq r0,#0x1F
	
	orr	r4, r4, r0
	msr	cpsr, r4	@ hop, c'est fait
	
	mov r0,sp
	
	msr	cpsr, r3	@ back to normal mode @ on revient au mode "normal"
	
	pop {r1-r4}
	
	bx lr
	
	.global readbankedlr
   .type   readbankedlr STT_FUNC
readbankedlr:
	push {r1-r4}
	
	mrs	r3, cpsr
	bic	r4, r3, #0x1F
	and	r0, r0, #0x1F
	
	cmp r0,#0x10 @ichfly user is system
	moveq r0,#0x1F
	
	orr	r4, r4, r0
	msr	cpsr, r4	@ hop, c'est fait
	
	mov r0,lr
	
	msr	cpsr, r3	@ back to normal mode @ on revient au mode "normal"
	
	pop {r1-r4}
	bx lr
	
	
	.global cpuSetCPSR
   .type   cpuSetCPSR STT_FUNC
cpuSetCPSR:
	msr cpsr, r0
	bx lr
	
	.global puSetGbaIWRAM
   .type   puSetGbaIWRAM STT_FUNC
puSetGbaIWRAM:
	ldr	r0,=( (0b01110 << 1) | 0x03000000 | 1)	
	mcr	p15, 0, r0, c6, c2, 0
	bx lr
	
	.global cpu_GetMemPrem
   .type   cpu_GetMemPrem STT_FUNC

cpu_GetMemPrem:
	mrc p15, 0, r0, c5, c0, 2
	bx lr

	.global puSetMemPerm
   .type   puSetMemPerm STT_FUNC

puSetMemPerm:
	mcr p15, 0, r0, c5, c0, 2
	bx lr
	
	.global pu_Enable
   .type   pu_Enable STT_FUNC

pu_Enable:
 	mrc	p15,0,r0,c1,c0,0
 	orr	r0,r0,#1
	mcr	p15,0,r0,c1,c0,0
	bx lr
	
	.global cpu_SetCP15Cnt
   .type   cpu_SetCP15Cnt STT_FUNC

cpu_SetCP15Cnt:
	mcr p15, 0, r0, c1, c0, 0
	bx lr
	
	.global cpu_GetCP15Cnt
   .type   cpu_GetCP15Cnt STT_FUNC
cpu_GetCP15Cnt:
	mrc p15, 0, r0, c1, c0, 0
	bx lr		
 
	.global cpu_ArmJump
   .type   cpu_ArmJump STT_FUNC

cpu_ArmJump:
@      reg[13].I = 0x03007F00;

@      reg[R13_IRQ].I = 0x03007FA0;
@      reg[R13_SVC].I = 0x03007FE0;

	bic r0, r0, #1
	bx r0

	.global cpu_ArmJumpforstackinit
   .type   cpu_ArmJumpforstackinit STT_FUNC

cpu_ArmJumpforstackinit:
@      reg[13].I = 0x03007F00;

@      reg[R13_IRQ].I = 0x03007FA0;
@      reg[R13_SVC].I = 0x03007FE0;
	
	mov	r2, #0x12		@ Switch to IRQ Mode
	msr	cpsr, r2
	ldr	sp, =0x03007FA0		@ Set IRQ stack

	mov	r2, #0x13		@ Switch to SVC Mode
	msr	cpsr, r2
	ldr	sp, =0x03007FE0		@ Set SVC stack
	
	mov	r2, #0x1F		@ Switch to System Mode
	msr	cpsr, r2
	ldr sp,=0x03007F00
	
	bic r0, r0, #1
	bx r0

	.global cpuGetSPSR
   .type   cpuGetSPSR STT_FUNC
cpuGetSPSR:
	mrs r0, spsr
	bx lr
	
	.global cpuGetCPSR
   .type   cpuGetCPSR STT_FUNC
cpuGetCPSR:
	mrs r0, cpsr
	bx lr
	
	.global pu_SetRegion
   .type   pu_SetRegion STT_FUNC
pu_SetRegion:
	ldr	r2, =_puSetRegion_jmp
	add	r2, r0, lsl #3
	bx		r2
_puSetRegion_jmp:
	mcr	p15, 0, r1, c6, c0, 0
	bx		lr
	mcr	p15, 0, r1, c6, c1, 0
	bx		lr
	mcr	p15, 0, r1, c6, c2, 0
	bx		lr
	mcr	p15, 0, r1, c6, c3, 0
	bx		lr
	mcr	p15, 0, r1, c6, c4, 0
	bx		lr
	mcr	p15, 0, r1, c6, c5, 0
	bx		lr
	mcr	p15, 0, r1, c6, c6, 0
	bx		lr
	mcr	p15, 0, r1, c6, c7, 0
	bx		lr

	.global pu_SetDataPermissions
   .type   pu_SetDataPermissions STT_FUNC

pu_SetDataPermissions:

	mcr	p15, 0, r0, c5, c0, 2
	bx		lr

	.global pu_SetCodePermissions
   .type   pu_SetCodePermissions STT_FUNC

pu_SetCodePermissions:
	mcr	p15, 0, r0, c5, c0, 3
	bx		lr

	.global pu_SetDataCachability
   .type   pu_SetDataCachability STT_FUNC

pu_SetDataCachability:
	mcr	p15, 0, r0, c2, c0, 0
	bx		lr

	.global pu_SetCodeCachability
   .type   pu_SetCodeCachability STT_FUNC

pu_SetCodeCachability:
	mcr	p15, 0, r0, c2, c0, 1
	bx		lr

	.global pu_GetWriteBufferability 
   .type   pu_GetWriteBufferability STT_FUNC

pu_GetWriteBufferability:
	mcr	p15, 0, r0, c3, c0, 0
	bx		lr




@swicode

@---------------------------------------------------------------------------------
	.global ichflyswiWaitForVBlank
   .type   ichflyswiWaitForVBlank STT_FUNC

@---------------------------------------------------------------------------------
ichflyswiWaitForVBlank:
@---------------------------------------------------------------------------------
	mov	r0, #1
	mov	r1, #1
	mov	r2, #0
@---------------------------------------------------------------------------------
	.global ichflyswiIntrWait
   .type   ichflyswiIntrWait STT_FUNC

@---------------------------------------------------------------------------------
ichflyswiIntrWait:
@---------------------------------------------------------------------------------

	push {lr}
	cmp	r0, #0
	blne	testirq

wait_irq:

	@swi	#(6<<16) @ichfly my code
	
	mov r12 ,#0
	mcr p15,0,r12,c7,c0,4	
	
	@ichfly einschub
	
	mrs	r2, cpsr
	bic	r3, r2, #0xC0
	
	
	@push regs
	push {r0-r3}
	sub r0,sp,#4*17 @+1 res you know
	ldr r1,=exRegs
	mov r2,#4*16
	BL memcpy
	pop {r0-r3}
	
	sub sp,sp,#0x58
	ldr	r12, =spsvc	@save old stack
	str sp, [r12]
	add sp,sp,#0x58
	
	mov r12,sp

	msr	cpsr,r3 @irq
	msr cpsr,r2
	
	mov sp,r12
	ldr	r12,=0x33333333	@ see cpumg.cpp for meanings protections
	mcr	p15, 0, r12, c5, c0, 2	

	@pop regs
	push {r0-r3}
	sub r1,sp,#4*17 @+1 res you know
	ldr r0,=exRegs
	mov r2,#4*16
	BL memcpy
	pop {r0-r3}


	@ichfly my code end
	
	bl	testirq
	beq	wait_irq
	pop {lr}
	bx	lr

testirq:
	mov	r12, #0x4000000
	strb	r12, [r12,#0x208]
	ldr	r3, [r12,#-8]
	ands	r0, r1,	r3
	eorne	r3, r3,	r0
	strne	r3, [r12,#-8]
	mov	r0, #1
	strb	r0, [r12,#0x208]
	bx	lr

.pool

@---------------------------------------------------------------------------------
	.global ichflyswiHalt
   .type   ichflyswiHalt STT_FUNC

@---------------------------------------------------------------------------------
ichflyswiHalt:
@---------------------------------------------------------------------------------

	push {r12}
	mov r12 ,#0
	mcr p15,0,r12,c7,c0,4
	pop {r12}
	bx	lr
	
	

@coto: sleep mode protection for gba
@---------------------------------------------------------------------------------
	.global backup_mpu_setprot
	.type   backup_mpu_setprot STT_FUNC
@---------------------------------------------------------------------------------
backup_mpu_setprot:

	push {r6,r7}
	@read MPU mode (should be NDS)
	ldr	r6, =MPUPERMBACKUPSET_SWI		@MPU current Protection Settings for Data Region are backd
	mrc	p15, 0, r7, c5, c0, 2      
	str	r7, [r6]
	pop {r6,r7}
	bx lr
	
	
@-----------------------------------
	.global restore_mpu_setprot
	.type   restore_mpu_setprot STT_FUNC
@---------------------------------------------------------------------------------
restore_mpu_setprot:

	push {r6,r7}
	@restore MPU mode (should be NDS anyway)
	ldr	r6, =MPUPERMBACKUPSET_SWI  @MPU current Protection Settings for Data Region are restored
	ldr	r7, [r6]
	mcr	p15, 0, r7, c5, c0, 2	
	pop {r6,r7}
	
	bx lr

	
@---------------------------------------------------------------------------------
	.global resettostartup
	.type   resettostartup STT_FUNC

@---------------------------------------------------------------------------------
resettostartup:
@---------------------------------------------------------------------------------
	B main

.pool

@Use the actual 32byte ARM vectors unused by DS design, (and not the BIOS ROM vectors).
.global setVectorsAsm
.type   setVectorsAsm STT_FUNC
setVectorsAsm:
	push {r0-r3,lr}
	
	ldr                 r1,=(irqhandler2 + 0x0)     @Reset handler
	ldr					r1,[r1]
	mov                 r2,#(0x00000000 + 0x0)
	str r1,[r2]
	
	ldr                 r1,=(irqhandler2 + 0x4)     @Undefined instr. handler           
	ldr					r1,[r1]
	mov                 r2,#(0x00000000 + 0x4)
	str r1,[r2]
	
	ldr                 r1,=(irqhandler2 + 0x8)     @SWI handler           
	ldr					r1,[r1]
	mov                 r2,#(0x00000000 + 0x8)
	str r1,[r2]
	
	ldr                 r1,=(irqhandler2 + 0xC)     @Prefetch instr. handler           
	ldr					r1,[r1]
	mov                 r2,#(0x00000000 + 0xC)
	str r1,[r2]
	
	ldr                 r1,=(irqhandler2 + 0x10)     @Data Abort handler           
	ldr					r1,[r1]
	mov                 r2,#(0x00000000 + 0x10)
	str r1,[r2]
	
	@0x14 Reserved
	
	ldr                 r1,=(irqhandler2 + 0x18)     @IRQ handler           
	ldr					r1,[r1]
	mov                 r2,#(0x00000000 + 0x18)
	str r1,[r2]
	
	ldr                 r1,=(irqhandler2 + 0x1C)     @FIQ handler           
	ldr					r1,[r1]
	mov                 r2,#(0x00000000 + 0x1C)
	str r1,[r2]
	
	pop {r0-r3,lr}
	bx		lr

.pool