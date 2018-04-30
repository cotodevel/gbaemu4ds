#ifndef arm7opcodes
#define arm7opcodes

#include <nds.h>
#ifdef __cplusplus
extern "C"{
#endif

//SWI 19h (GBA) or SWI 08h (NDS7/DSi7) - SoundBias
//Increments or decrements the current level of the SOUNDBIAS register (with short delays) until reaching the desired new level. The upper bits of the register are kept unchanged.

//r0   BIAS level (0=Level 000h, any other value=Level 200h)
//r1   Delay Count (NDS/DSi only) (GBA uses a fixed delay count of 8)

extern void swi0x19(u16 bias_level,u16 delay_count);


#ifdef __cplusplus
}
#endif

#endif
