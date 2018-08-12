.arch	armv4t
.cpu arm7tdmi
.section	.itcm,"ax",%progbits
.align 4
.arm
#include "../ichflysettings.h"
.pool

@========== Exception code ====================	
.section	.vectors,"ax",%progbits
.align 4
.arm
	
.global irqhandler2
irqhandler2:
b	inter_Reset + 0x01FF8000
b	inter_undefined + 0x01FF8000
b	inter_swi + 0x01FF8000
b	inter_prefetchAbt + 0x01FF8000
b	inter_dataAbt + 0x01FF8000
b	inter_res + 0x01FF8000
b	inter_irq + 0x01FF8000
b	inter_fast + 0x01FF8000
b	inter_res2 + 0x01FF8000


somethingfailed:
inter_Reset:
inter_res:
inter_fast:
inter_res2:
	str sp,[pc, #0x10]
	str lr,[pc, #0x10]		
	b inter_dataAbt

.pool

.global savedsp
savedsp:
	.word 0
.global savedlr
savedlr:
	.word 0

.global spirq
spirq:
	.word __sp_irq

.global spsvc
spsvc:
	.word __sp_svc

.pool

inter_irq:
	sub lr,lr,#4
	stmfd  SP!, {R0-R12,LR}		@save registers to SP_irq
	
	ldr	r1,=0x36333333		@MPU is set free to write everything except this area (vectors)
	mcr	p15, 0, r1, c5, c0, 2
	
	bl IntrMain				@serve IFs that are enabled on IE (NDS IO), r1 alternatively has the remaining IFs from vector table
							@BL is faster than BLX and IntrMain is ARM code (otherwise this would crash inmediately)
							
	
	mov	r0, #0x04000000		@03FFFFFC || 03007FFC(mirror) is user handler
	ldr	r1, [r0, #0x214]	@get IF
	
	ldr	r2, =anytimejmpfilter
	ldr r2, [r2]
	ands r1,r1,r2 			@bankedIE (NDS enabled hardware) & IF
	
	ldr	r1, =0x06300033        	@puGba();
	mcr	p15, 0, r1, c5, c0, 2	@setDataPermiss
	@mcr	p15, 0, r1, c5, c0, 3	@setCodePermiss	/ no need for that here since only happens when: the instructions pool failed to fetch data at AHB level or instruction not understood/unaligned access

	BEQ	irqexit		@ IF > 0 ? GBA IRQ Handler

	@GBA IRQ Handler
	add    LR,PC,#0            @retadr for USER handler
	ldr    PC,[R0, #-0x4] 	     @jump to [03FFFFFC] USER handler

irqexit:
	ldmfd  SP!, {R0-R12,PC}^ @restore registers from SP_irq + armv4 format at arm9

.pool
	
inter_swi:
	@ change the PU to nds mode
	ldr	SP,=0x36333333	@ see cpumg.cpp for meanings protections
	mcr	p15, 0, SP, c5, c0, 2
	ldr	SP, =exRegs
	
	str	lr, [SP, #(15 * 4)]	@ save r15 (lr is r15)
	
	@ save the registres 0->12
	stmia	SP, {r0-r12}
	
	ldr	sp, =spsvc	@ use the new stack
	ldr sp, [sp]
	bl	swiExceptionHandler @ichfly change back if possible
	
	ldr	r1, =spsvc	@save old stack
	str sp, [r1]
	
	@ restore the registres 0->12
	ldr	lr, =exRegs
	ldmia	lr, {r0-r12}
	
	ldr	lr, [lr, #(15 * 4)]
	subs    pc, lr, #0 @ichfly this is not working	

.pool

inter_prefetchAbt: @ break function todo
	subs    lr, lr, #0x8000000
	ldr		sp,=rom
	ldr		sp,[sp]
	@thisi4:	
	@ldr r2, [pc,#(rom - thisi4 -8)]
	add		lr,lr,sp
	subs    pc, lr, #4

.pool

inter_undefined:
@change the PU to nds mode
	push {r0-r12,lr}
	ldr	r12, =exRegs
	str sp, [r12,#(4*13)]
	
	ldr	SP,=0x36333333	
	mcr	p15, 0, SP, c5, c0, 2	
	
	@ restore nds undef stack context
	ldr	sp, =__sp_undef
	ldr sp, [sp]

	BL undefinedExceptionHandler

	@save nds swi stack context
	ldr	r1, =__sp_undef
	str sp, [r1]	

	@restore the GBA frame context (this LR is reusable, r13 no because we need to restore it and r15 is the on-exit frame address) 
	ldr	sp, =exRegs
	ldr sp, [sp,#(4*13)]
	pop {r0-r12,lr}
	subs	pc, lr, #4

.pool

inter_dataAbt:
	
	ldr	SP, =exRegs
	str	lr, [SP, #(15 * 4)]	@ save r15 (lr is r15)
	@ save the registres 0->12
	stmia	SP!, {r0-r12}
	
	@ change the PU to nds mode
	ldr	r7,=0x36333333	@ see cpumg.cpp for meanings protections
	mcr	p15, 0, r7, c5, c0, 2
	
	MRS r5,spsr
	mov r6,SP
	@ change the mode  @ on change de mode (on se mets dans le mode qui était avant l'exception)
	mrs	r3, cpsr
	bic	r4, r3, #0x1F
	and	r1, r5, #0x1F
	
	cmp r1,#0x10 @ichfly user is system
	moveq r1,#0x1F
	
	orr	r4, r4, r1
	msr	cpsr, r4	@ hop, c'est fait
	
	stmia r6, {r13-r14} @save the registrers	@ on sauvegarde les registres bankés (r13 et r14)
	msr	cpsr, r3	@ back to normal mode @ on revient au mode "normal"
	
	@ldr r1,=exRegs
	sub r1,SP,#13 * 4
	
	lsls r2,r5, #0x1A
	BMI itisTHUMB
	
itisARM:

	ldr r0, [LR, #-0x8]
	ldr	sp, =__sp_undef	@ use the new stack
	BL emuInstrARM 	@ is emuInstrARM(u32 opcode, u32 *R)
	B exitdirectcpu
itisTHUMB:
	ldrh r0, [LR,#-0x8]
	sub LR, #0x2
	str LR, [r1, #15*4]
	ldr	sp, =__sp_undef	@ use the new stack
	BL emuInstrTHUMB @ is emuInstrTHUMB(u16 opcode, u32 *R)
exitdirectcpu:

	@ restore SPSR @ on restaure les bankés
	MSR spsr,r5

	@change mode to the saved mode @ on change de mode (on se mets dans le mode qui était avant l'exception)
	mrs	r3, cpsr
	bic	r4, r3, #0x1F
	and	r5, r5, #0x1F
	
	cmp r5,#0x10 @ichfly user is system
	moveq r5,#0x1F	
	
	orr	r4, r4, r5
	msr	cpsr, r4	@ hop, c'est fait
	ldmia r6, {r13-r14}	@restor r13 and r14  @ on restaure les registres bankés (r13 et r14). /!\ N'allons pas croire qu'on les a restauré dans notre contexte: en effet, on a changé de mode là !
	msr	cpsr, r3	@chagne to mode "normal"@ on revient au mode "normal"
	
	ldr	SP, =0x06300033			@puGba();
	mcr	p15, 0, SP, c5, c0, 2	@setDataPermiss
	@mcr	p15, 0, r1, c5, c0, 3	@setCodePermiss	/ prefetch abort handles different logic. Policy is when GBAMode enters is set once.
	
	@restore r0-r12 easy
	sub lr,r6,#13 * 4 @ldr	lr, =exRegs
	ldmia	lr, {r0-r12}

	@restore PU from the handler @ restaure la protection du PU, comme voulue par l'handler perso	
	@my speedup code
	ldr	lr, [lr, #(15 * 4)] 
	
	subs    pc, lr, #4	

.pool

.section	.dtcm,"ax",%progbits

.global BIOSDBG_SPSR
BIOSDBG_SPSR:
	.word 0

.global MPUPERMBACKUPSET_SWI	@swi mpu save sleep mode
MPUPERMBACKUPSET_SWI:
	.word 0x00000000

.global exRegs
exRegs:
	.word	0	@r0     a1
	.word	0	@r1		a2
	.word	0	@r2		a3
	.word	0	@r3		a4
	.word	0	@r4		s1
	.word	0	@r5		s2
	.word	0	@r6		s3
	.word	0	@r7		s4
	.word	0	@r8		s5
	.word	0	@r9		s6
	.word	0	@r10	sl
	.word	0	@r11	ip
	.word	0	@r12	fp
	.word	0	@r13	sp
	.word	0	@r14	lr
	.word	0	@r15	pc
	
.pool
.end