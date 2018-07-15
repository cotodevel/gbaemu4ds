#ifndef __CPU_H__
#define __CPU_H__

#include <nds.h>

#define debugandhalt()\
{\
	REG_IME = IME_DISABLE;\
    debugDump();\
	while(1);\
}\

//#define DEV_VERSION

#define B8(h,g,f,e,d,c,b,a) ((a)|((b)<<1)|((c)<<2)|((d)<<3)|((e)<<4)|((f)<<5)|((g)<<6)|((h)<<7))

#endif /*__CPU_H__*/
 
 
#ifdef __cplusplus
extern "C"{
#endif

// void cpu_GbaMemPerm();
// void cpu_NdsMemPerm();
// extern void cpu_GbaSetIwram();

//extern "C" void exMain(); 
//#define BIOSDBG_CP15 *((volatile u32*)0x027FFD8C)
//#define BIOSDBG_SPSR *((volatile u32*)0x027FFD90)
//#define BIOSDBG_R12  *((volatile u32*)0x027FFD94)
//#define BIOSDBG_PC   *((volatile u32*)0x027FFD98)


extern u32 savedsp;
extern u32 savedlr;

extern void gbaExceptionHdl();
extern int spirq;
extern int SPtemp;

// extern void puSetMemPerm(u32 perm);
extern void pu_Enable();
// extern void puSetGbaIWRAM();
extern void pu_SetRegion(u32 region, u32 value);

extern void pu_SetDataPermissions(u32 v);
extern void pu_SetCodePermissions(u32 v);
extern void  pu_SetDataCachability(u32 v);
extern void  pu_SetCodeCachability(u32 v);
extern void pu_GetWriteBufferability(u32 v);

extern void cpu_ArmJump(u32 address, u32 r1);
extern void cpu_ArmJumpforstackinit(u32 address, u32 r1);
extern void exInitswisystem(void (*customswiHdl)());
extern void gbaswieulatedbios();
extern void gbaInit(bool slow);
extern void switch_to_unprivileged_mode();
extern void gbaMode();
extern void ndsMode();
extern u32 getExceptionAddress( u32 opcodeAddress, u32 thumbState);
extern unsigned long ARMShift(unsigned long value,unsigned char shift);

extern void readbankedextra(u32 CPSR);
extern u32 cpuGetCPSR();
extern u32 cpuGetSPSR();
extern void cpuSetCPSR(u32 CPSR);

extern u32 readbankedsp(u32 CPSR);
extern u32 readbankedlr(u32 CPSR);

extern void cpupausemodeexit();
extern void cpupausemode();
//extern void debugDump();

extern bool disableMessage;

extern void ichflyswiHalt();
extern void ichflyswiWaitForVBlank();
extern void ichflyswiIntrWait(u32 i,u32 c);

extern void (*exHandler)();
extern void (*exHandlerswi)();
extern void (*exHandlerundifined)();
extern u32  exRegs[];
extern u32 BIOSDBG_SPSR;

extern void DrainWriteBuffer();

//GBA SWI sleep mode (swi 0x3)
extern void backup_mpu_setprot();
extern void restore_mpu_setprot();
extern u32 MPUPERMBACKUPSET_SWI;	//MPUd/itcmmemorypermissionsfromcaller

extern int arm9VCOUNTsyncline;
extern void failcpphandler();
extern void ARMV5toARMV4Mode();
extern void ARMV4toARMV5Mode();
extern void emulateedbiosstart();
extern void resettostartup();
extern void IntrMain();
extern void testasm(u32* feld);
extern int main( int argc, char **argv);
extern void testasm(u32* feld);
extern void cpu_SetCP15Cnt(u32 v);
extern u32 cpu_GetCP15Cnt();
extern void HblankHandler(void);
extern int frameskip;

#ifdef __cplusplus
}
#endif

