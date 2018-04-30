@0x0 reset
@0x4 undefined instr
@0x8 swi
@0xc prefetch abt
@0x10 data abt
@0x14 reserved
@0x18 irq
@0x1c fiq

@========== Exception code ====================
.section	.vectors,"ax",%progbits
.align 4
.code 32
.arm
.extern GBAEMU4DS_IPC
#include "../ichflysettings.h"

@offsetting add because vectors @ 0x01000000	
.global irqhandler2
irqhandler2:
b	inter_Reset + 0x01000000
b	inter_undefined + 0x01000000
b	inter_swi + 0x01000000
b	inter_fetch + 0x01000000
b	inter_data + 0x01000000
b	inter_res + 0x01000000
b	inter_irq + 0x01000000
b	inter_fast + 0x01000000
b	inter_res2 + 0x01000000


@SVC stacks

@shares NDS soft swi and hardware swi bios -- stacks
.global spsvc
spsvc:
	.word __sp_svc

@alignment: needed!!
nop
nop

inter_Reset:
somethingfailed:
inter_res:
inter_fast:
inter_res2:
	str sp,[pc, #0x10]
	str lr,[pc, #0x10]
	ldr sp, =failcpphandler
	ldr lr, =exHandler
	str sp,[lr]
	b inter_data        @retry opcode : fallback to software emulation if unhandled access so if an exception occurs once again, the exception is thrown.
    
    @add lr,lr,#-2       
    @b inter_data        @retry opcode : fallback to software emulation if unhandled access so if an exception occurs once again, the exception is thrown.
    
    
@------------------------------- IRQ routine ---------------------------------
@mixed irq stacks (gba mode should use its own stack, while nds same)
inter_irq:
	STMFD sp!,{r0-r3,R12,LR}		@save registers to SP_irq
	
	ldr	r1, =MPUPERMBACKUPSET_IRQ		@MPU current Protection Settings for Data Region are backd
	mrc	p15, 0, r2, c5, c0, 2      
	str	r2, [r1]
	
	ldr	r1,=0x36333333		@MPU is set free to write everything except ourselves (vectors)
	mcr	p15, 0, r1, c5, c0, 2
	
	bl IntrMain_gba			@serve IFs that are enabled on IE (NDS IO), r1 alternatively has the remaining IFs from vector table
							@BL is faster than BLX and IntrMain is ARM code (otherwise this would crash inmediately)
	
	mov	r0, #0x04000000		@03FFFFFC || 03007FFC(mirror) is user handler
	ldr	r1, [r0, #0x214]	@get IF
	
	ldr	r2, =IE_ADDRESS
	ldr r2, [r2]
    ldr r2, [r2]            @coto: yeah.. so far its the working method for accessing GBAEMU4DS_IPC->IE
    
	ands r1,r1,r2 			@IF & (GBA IE) 
	
	BEQ	irqexitdirect		@ IF > 0 ? continue serving interrupts
	
gba_handler:
	ldr	r1, =0x06333333        @prepare for GBA IRQ handler
	mcr	p15, 0, r1, c5, c0, 2

@acquire and jump to usr irq handler and go back right after
	add    LR,PC,#0            @retadr for USER handler
	ldr    PC,[R0, #-0x4]      @jump to [03FFFFFC] USER handler
	
irqexitdirect:	
	ldr	r1, =MPUPERMBACKUPSET_IRQ  @MPU current Protection Settings for Data Region are restored
	ldr	r1, [r1]
	mcr	p15, 0, r1, c5, c0, 2	
	
	LDMFD sp!,{r0-r3,R12,LR}	@restore registers from SP_irq
	subs   PC,LR, #0x4         @return from IRQ (PC=LR-4, CPSR=SPSR)
    
@code ok

@------------------------------- SWI routine ---------------------------------

inter_swi:
@change the PU to nds mode
ldr	SP,=0x36333333	
mcr	p15, 0, SP, c5, c0, 2	

@ save the GBA frame context
ldr	r12, =exRegs
stmia	r12, {r0-r11}	@lr is GBA PC
str lr, [r12,#(4*15)]

@ restore nds swi stack context
ldr	sp, =spsvc
ldr sp, [sp]

@coto: gba mode spsr to define what offset of swi we need.
#ifdef correct_swi_handle
mrs r0,spsr
#endif

@ jump into the personal handler
ldr	r1, =exHandlerswi
ldr	r1, [r1]	
blx	r1

@save nds swi stack context
ldr	r1, =spsvc
str sp, [r1]	

@restore the GBA frame context (this LR is reusable, r13 no because we need to restore it and r15 is the on-exit frame address) 
ldr	lr, =exRegs
ldmia	lr, {r0-r11}
ldr sp,[lr,#(4*13)]	@coto: yeah this means another instruction but even if upcoming PSR swap stacks we maintain original stack in future SPSR 
ldr lr,[lr,#(4*15)]
MOVS PC,LR			@GBA PC is next NDS PC
@code ok
@---- swi end


inter_fetch: @ break function todo
	subs    lr, lr, #0x8000000
	ldr		sp,=rom
	ldr		sp,[sp]
	add		lr,lr,sp
	subs    pc, lr, #4


inter_undefined:
	@change the PU to nds mode
	ldr	SP,=0x36333333	
	mcr	p15, 0, SP, c5, c0, 2	

	@ save the GBA frame context
	ldr	r12, =exRegs
	stmia	r12, {r0-r11}	@lr is GBA PC
	str lr, [r12,#(4*15)]

	@ restore nds undef stack context
	ldr	sp, =__sp_undef
	ldr sp, [sp]

	@coto : jump to handler,  make for arm9 is set for -marm ARM code (with interchange THUMB). If you switch back to -mthumb it will be slower & this will have to be BLX
	BL exHandlerundifined

	@save nds swi stack context
	ldr	r1, =__sp_undef
	str sp, [r1]	

	@restore the GBA frame context (this LR is reusable, r13 no because we need to restore it and r15 is the on-exit frame address) 
	ldr	lr, =exRegs
	ldmia	lr, {r0-r11}
	@ldr sp,[lr,#(4*13)]	@coto: yeah this means another instruction but even if upcoming PSR swap stacks we maintain original stack in future SPSR 
	ldr lr,[lr,#(4*15)]
	subs	pc, lr, #4

@code ok


inter_data:
	
	ldr	SP, =exRegs	@stacks for data abort/prefetch abort dont care
	str	lr, [SP, #(15 * 4)]	@ save r15 (lr is r15)
	
	@ save the registres 0->12
	stmia	SP!, {r0-r12}
	
	@ change the PU to nds mode
	ldr	r7,=0x36333333	@ see cpumg.cpp for meanings protections
	mcr	p15, 0, r7, c5, c0, 2
	
	MRS r5,spsr			@gbaframe spsr
	mov r6,SP			@cross-psr exRegs pointer

	@change the mode
	mrs	r3, cpsr
	bic	r4, r3, #0x1F
	and	r1, r5, #0x1F
	
	cmp r1,#0x10 		@ichfly: patch user as system
	moveq r1,#0x1F
	
	orr	r4, r4, r1
	msr	cpsr, r4		@hop
	
	stmia r6, {r13-r14} @save the registers of spsr
	msr	cpsr, r3		@back to normal mode @ on revient au mode "normal"
	
	
	@detect software ARM or THUMB code
	sub r1,SP,#(13 * 4)
	
	lsls r2,r5, #0x1A
	BMI itisTHUMB
	
itisARM:
	ldr r0, [LR, #-0x8]
	ldr	sp, =__sp_dabt	@ use the new stack
	BL emuInstrARM @ is emuInstrARM(u32 opcode, u32 *R)
	B exitdirectcpu
itisTHUMB:
	ldrh r0, [LR,#-0x8]
	sub LR, #0x2
	str LR, [r1, #15*4]
	ldr	sp, =__sp_dabt	@ use the new stack
	BL emuInstrTHUMB @ is emuInstrTHUMB(u16 opcode, u32 *R)

exitdirectcpu:
	@ restore SPSR & basically the gba Frame
	MSR spsr,r5
	mrs	r3, cpsr
	bic	r4, r3, #0x1F
	and	r5, r5, #0x1F	@hop
	
	cmp r5,#0x10 		@ichfly: patch user as system
	moveq r5,#0x1F	
	
	orr	r4, r4, r5
	msr	cpsr, r4
	
	ldmia r6, {r13-r14}	@restore the registers of spsr
	msr	cpsr, r3		@this cpsr
	
	@gbaromhook 
    @ldr	SP, =0x06033333
    
    @debug  
    ldr	SP, =0x06333333
    
	mcr	p15, 0, SP, c5, c0, 2

	@restore r0-r12 easy
	sub lr,r6,#(13 * 4)
	ldmia	lr, {r0-r12}
	
	@restore PU from the handler
	ldr	lr, [lr, #(15 * 4)] 
	subs    pc, lr, #4


@========== Timing Dependant Vars ====================	
.section	.dtcm,"ax",%progbits
.align 4
.code 32
.arm

@Store GBA IE address since its in shared wram
.global IE_ADDRESS
IE_ADDRESS:
	.word 0x00000000

.global BIOSDBG_SPSR
BIOSDBG_SPSR:
	.word 0x00000000

.global exHandlerprefetch
exHandlerprefetch:
	.word 0x00000000

.global exHandler
exHandler:
	.word 0x00000000
	
.global exHandlerundifined
exHandlerundifined:
	.word 0x00000000

.global exHandlerswi
exHandlerswi:
	.word 0x00000000

.global exPuProtection
exPuProtection:
	.word 0x00000000
	
.global exRegs
exRegs:
	.word 0x00000000
	.word 0x00000000
	.word 0x00000000
	.word 0x00000000
	.word 0x00000000
	.word 0x00000000
	.word 0x00000000
	.word 0x00000000
	.word 0x00000000
	.word 0x00000000
	.word 0x00000000
	.word 0x00000000
	.word 0x00000000
	.word 0x00000000
	.word 0x00000000
	.word 0x00000000

.global bankedLR
bankedLR:
	.word 0x00000000

.global bankedSP
bankedSP:
	.word 0x00000000

.global bankedCPSR
bankedCPSR:
	.word 0x00000000

.global bankedSPSR
bankedSPSR:
	.word 0x00000000

.global MPUPERMBACKUPSET_IRQ
MPUPERMBACKUPSET_IRQ:
	.word 0x00000000

.global MPUPERMBACKUPSET_SWI	@swi mpu save sleep mode
MPUPERMBACKUPSET_SWI:
	.word 0x00000000

@swi
.global swigbastack
swigbastack:
    .word   0x00000000      @r0     a1
    .word   0x00000000      @r1             a2
    .word   0x00000000      @r2             a3
    .word   0x00000000      @r3             a4
    .word   0x00000000      @r4             s1
    .word   0x00000000      @r5             s2
    .word   0x00000000      @r6             s3
    .word   0x00000000      @r7             s4
    .word   0x00000000      @r8             s5
    .word   0x00000000      @r9             s6
    .word   0x00000000      @r10    sl
    .word   0x00000000      @r11    ip
    .word   0x00000000      @r12    fp
    .word   0x00000000      @r13    sp
    .word   0x00000000      @r14    lr
    .word   0x00000000      @r15    pc
    
.pool