#ifndef __sound7_h__
#define __sound7_h__

#include "typedefsTGDS.h"
#include "dsregs.h"
#include "dsregs_asm.h"

 #define SOUND_16BIT (1<<29)
 #define SOUND_8BIT (0)
 
#endif


#ifdef __cplusplus
extern "C"{
#endif

extern void dmaAtimerinter();
extern void dmaBtimerinter();

extern void updatevol();
extern void updatetakt();

extern void checkstart();


//debug stuff
//extern vu32 debugsrc1;
//extern vu32 debugsrc2;
//extern vu32 debugfr1;
//extern vu32 debugfr2;

extern u8* soundbuffA;
extern u8* soundbuffB;

extern u8 dmaApart;
extern u8 dmaBpart;

extern u32 dmabuffer;


extern void timer0interrupt_thread();
extern void timer1interrupt_thread();
extern void timer2interrupt_thread();

#ifdef __cplusplus
}
#endif
