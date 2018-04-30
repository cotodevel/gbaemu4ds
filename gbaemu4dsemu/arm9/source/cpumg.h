#include <nds/ndstypes.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void  (*exHandler)();
extern void  (*exHandlerswi)();
extern void  (*exHandlerundifined)();

extern u32   exRegs[];
extern u32   BIOSDBG_SPSR;

extern u32   bankedLR;
extern u32   bankedSP;
extern u32   bankedCPSR;
extern u32   bankedSPSR;


//ex_s.s
extern u32 IE_ADDRESS;

//stacks
extern u32 spsvc;

extern void gbaExceptionHdl();

extern void readbankedextra(u32 CPSR);
extern u32 cpuGetCPSR();
extern u32 cpuGetSPSR();
extern void cpuSetCPSR(u32 CPSR);

extern u32 readbankedsp(u32 CPSR);
extern u32 readbankedlr(u32 CPSR);

extern void cpupausemodeexit();
extern void cpupausemode();

extern u16 gbaIME;
extern u16 gbaDISPCNT;
extern u16 gbaBGxCNT[4];
extern char disbuffer[0x2000];

extern void debugDump();
extern void failcpphandler();

extern void exInitundifinedsystem(void (*)());
extern void exInitswisystem(void (*)());
extern void exInit(void (*)());

extern void undifinedresolver();

extern void gbaInit();
extern void enter_arm_usermode();
extern void gbaMode();

extern void ndsMode();
extern u32 getExceptionAddress( u32 opcodeAddress, u32 thumbState);
extern unsigned long ARMShift(unsigned long value,unsigned char shift);

extern void BIOScall(int op,  u32 *R);

extern void cpu_ArmJumpforstackinit(u32 address, u32 r1);
extern void exInitswisystem(void (*customswiHdl)());
extern void gbaswieulatedbios(u32 spsr_from_gbands_mode);

//icache.s | dcache.s

// extern void puSetMemPerm(u32 perm);
extern void pu_Enable();
extern void pu_Disable();


// extern void puSetGbaIWRAM();
extern void pu_SetRegion(u32 region, u32 value);

extern void pu_SetDataPermissions(u32 v);
extern void pu_SetCodePermissions(u32 v);
extern void pu_SetDataCachability(u32 v);
extern void pu_SetCodeCachability(u32 v);
extern void pu_GetWriteBufferability(u32 v);

extern void cpu_SetCP15Cnt(u32 v); //mask bit 1 for: 0 disable, 1 enable, PU
extern u32 cpu_GetCP15Cnt(); //get PU status: 0 disable, 1 enable

//instruction cache CP15
extern void IC_InvalidateAll();
extern void IC_InvalidateRange(const void *, u32 v);
extern void setitcmbase(); //@ ITCM base = 0 , size = 32 MB
extern void icacheenable(int);

//data cache CP15
extern void DC_FlushAll();
extern void DC_FlushRange(const void *, u32 v);
extern void setdtcmbase(); //@ DTCM base = __dtcm_start, size = 16 KB
extern void drainwrite();
extern void dcacheenable(int); //Cachability Bits for Data/Unified Protection Region (R/W)
extern void setgbamap();

extern u32 DC_clean_invalidate_range(u32 VA_range);
extern void setgbamap();
u32 getdtcmbase();
u32 getitcmbase();


//cpu_s.s
//irq code for ARM9 v5te
extern void gba4ds_swiHalt();
extern void gba4ds_swiWaitForVBlank();
extern void gba4ds_swiIntrWait(u32 i,u32 c);

//MPU settings intra
extern u32 MPUPERMBACKUPSET_IRQ;	    //MPUd  // IRQ Mode
extern u32 MPUPERMBACKUPSET_SWI;	//MPUd/itcmmemorypermissionsfromcaller: SWI bios

extern void set_cp15_vectors_0x00000000();
extern void set_cp15_vectors_0xFFFF0000();

extern void armv5_to_v4_compatibilitymode();
extern void armv5_mode();

extern inline void puGba();
extern inline void puNds();

// (cpu_utils.s)
//coto: sleep mode 
extern void backup_mpu_setprot();
extern void restore_mpu_setprot();

extern void HALTCNT_ARM9OPT();
#ifdef __cplusplus
}
#endif
 
