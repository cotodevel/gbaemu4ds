.align 4
.code 32
.arm

@ARM7 swi opcodes


@swi0x19(u16 bias_level,u16 delay_count);
.global swi0x19
.type   swi0x19 STT_FUNC
swi0x19:
    swi 0x8
    bx lr

.pool
.end