.section	.itcm,"ax",%progbits
.align 4
.code 32
.arm	

.global cpu_ArmJumpforstackinit
.type   cpu_ArmJumpforstackinit STT_FUNC
cpu_ArmJumpforstackinit:

    bl gbaInit
    bl gbaMode
    bl armv5_to_v4_compatibilitymode

	mov	r2, #0x12			@ Set IRQ stack
	msr	cpsr, r2
	ldr	sp, =0x03007FA0		

	mov	r2, #0x13			@ Set SVC stack
	msr	cpsr, r2
	ldr	sp, =0x03007FE0		
    
    mov	r2, #0x10			@ Set USR stack
	msr	cpsr, r2
	ldr sp,=0x03007F00
	
	mov	r2, #0x1F			@ Set SYS stack
	msr	cpsr, r2
	ldr sp,=0x03007F00
	
    ldr r0,=0x02040000
	@bic r0, r0, #1
	bx r0


@coto: this is because theres stack per psr cpu mode. This address must be changed manually through mrs/msr (system opcodes)
	.global readbankedsp
   .type   readbankedsp STT_FUNC
readbankedsp:
	push {r1-r4}
	
	mrs	r3, cpsr
	bic	r4, r3, #0x1F
	and	r0, r0, #0x1F
	
	cmp r0,#0x10 	@ichfly user is system
	moveq r0,#0x1F
	
	orr	r4, r4, r0
	msr	cpsr, r4
	
	mov r0,sp
	
	msr	cpsr, r3	@ back to whatever normal mode
	
	pop {r1-r4}
	bx lr
	
@coto: this is because theres stack per psr cpu mode. This address must be changed manually through mrs/msr (system opcodes)
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
	msr	cpsr, r4	@ hop
	
	mov r0,lr
	msr	cpsr, r3	@ back to normal mode
	
	pop {r1-r4}
	bx lr
	


@coto:	Modded IRQ so it is RE-ENTRANT to where program rests waiting for idle callback
@		Uses SVC stacks, DO NOT MIX with SWI stacks as they are already used for virtualization (ie: anything can cause a swi call such as games using bios functions or LIBNDS).
@---------------------------------------------------------------------------------
.global gba4ds_swiWaitForVBlank
.type   gba4ds_swiWaitForVBlank STT_FUNC
gba4ds_swiWaitForVBlank:
@---------------------------------------------------------------------------------
	mov	r0, #1
	mov	r1, #1
	mov	r2, #0
@---------------------------------------------------------------------------------
.global gba4ds_swiIntrWait
.type   gba4ds_swiIntrWait STT_FUNC
gba4ds_swiIntrWait:
@---------------------------------------------------------------------------------
	push {lr}
	cmp	r0, #0
	blne	servepassedif_irq

wait_irq:
	mov r12 ,#0				@wait for interrupt (idle CPU if no IF & IE)
	mcr p15,0,r12,c7,c0,4

	mrs	r2, cpsr
	bic	r3, r2, #0xC0		@remove THUMB mode and Interrupts are disabled
	
	@call backup reg here
	push {r0-r12}
		ldr r0,=exRegs
		ldr r1,=swigbastack	
		ldmia r0!,{r2,r3,r4,r5,r6,r7,r8,r9}	@prefetch write buffer (4*8 bytes) align
		stmia r1!,{r2,r3,r4,r5,r6,r7,r8,r9}
		ldmia r0!,{r2,r3,r4,r5,r6,r7,r8,r9}
		stmia r1!,{r2,r3,r4,r5,r6,r7,r8,r9}
	pop {r0-r12}
	
	ldr	r12, =spsvc	@leaving SWI mode
	str sp, [r12]
	
	msr	cpsr,r3 			@force IRQ hw exception
	msr cpsr,r2				@done IRQ? restore this mode 
	
	ldr	r12, =spsvc	@entering SWI mode
	ldr sp, [r12]
	
	@change the PU to nds mode
	ldr	r12,=0x33333333	@ see cpumg.cpp for meanings protections
	mcr	p15, 0, r12, c5, c0, 2	

	@restore regs here
	push {r0-r12}
		ldr r0,=swigbastack
		ldr r1,=exRegs
		ldmia r0!,{r2,r3,r4,r5,r6,r7,r8,r9}
		stmia r1!,{r2,r3,r4,r5,r6,r7,r8,r9}
		ldmia r0!,{r2,r3,r4,r5,r6,r7,r8,r9}
		stmia r1!,{r2,r3,r4,r5,r6,r7,r8,r9}
	pop {r0-r12}
	
	bl	servepassedif_irq 	@r1 Interrupt Flags to wait for
	beq	wait_irq
	pop {lr}
	bx	lr

@check IE & IF
servepassedif_irq:
	mov	r12, #0x4000000				@IME:0
	strb	r12, [r12,#0x208]
	ldr	r3, [r12,#-8]				@IE read
	ands	r0, r1,	r3
	eorne	r3, r3,	r0				@unset IFlags passed (serve right now)
	strne	r3, [r12,#-8]			@BIOS irq flags at 3007FF8h | 3fffff8
	mov	r0, #1
	strb	r0, [r12,#0x208]		@IME:1
	bx	lr

@---------------------------------------------------------------------------------
	.global gba4ds_swiHalt
	.type   gba4ds_swiHalt STT_FUNC
@---------------------------------------------------------------------------------
gba4ds_swiHalt:
@---------------------------------------------------------------------------------
	push {r12}
	mov r12 ,#0
	mcr p15,0,r12,c7,c0,4			@HALTCNT keeps ARM9 idle low power saving mode
	pop {r12}
	bx	lr

	
