#include <nds.h>
#include <stdio.h> 

#include <filesystem.h>
#include "getopt.h"
#include "System.h"
#include <fat.h>
#include <dirent.h>

#include <stdio.h>
#include <stdlib.h>
#include <nds/memory.h>//#include <memory.h> ichfly
#include <nds/ndstypes.h>
#include <nds/memory.h>
#include <nds/bios.h>
#include <nds/system.h>
#include <nds/arm9/math.h>
#include <nds/arm9/video.h>
#include <nds/arm9/videoGL.h>
#include <nds/arm9/trig_lut.h>
#include <nds/arm9/sassert.h>
#include <stdarg.h>

#include <filesystem.h>
#include "GBA.h"
#include "Sound.h"
#include "Util.h"
#include "getopt.h"
#include "System.h"
#include <fat.h>
#include <dirent.h>
#include "cpumg.h"
#include "GBAinline.h"
#include "main.h"
#include "armdis.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <nds/memory.h>//#include <memory.h> ichfly
#include <nds/ndstypes.h>
#include <nds/memory.h>
#include <nds/bios.h>
#include <nds/system.h>
#include <nds/arm9/math.h>
#include <nds/arm9/video.h>
#include <nds/arm9/videoGL.h>
#include <nds/arm9/trig_lut.h>
#include <nds/arm9/sassert.h>
#include <stdarg.h>
#include <string.h>

#include "GBA.h"
#include "GBAinline.h"
#include "Globals.h"
#include "EEprom.h"
#include "Flash.h"
#include "Sound.h"
#include "Sram.h"
#include "bios.h"
#include "Cheats.h"
#include "NLS.h"
#include "elf.h"
#include "Util.h"
#include "Port.h"
#include "agbprint.h"
#include "../../../common/gba_ipc.h"
#include "../../../common/cpuglobal.h"
#include "main.h"
#include "cpumg.h"


bool disableMessage = false;
FILE * pFile;

__attribute__((section(".dtcm")))
u32 PU_PAGE_4K		= (0x0B << 1);

__attribute__((section(".dtcm")))
u32 PU_PAGE_8K		= (0x0C << 1);

__attribute__((section(".dtcm")))
u32 PU_PAGE_16K		= (0x0D << 1);

__attribute__((section(".dtcm")))
u32 PU_PAGE_32K		= (0x0E << 1);

__attribute__((section(".dtcm")))
u32 PU_PAGE_64K		= (0x0F << 1);

__attribute__((section(".dtcm")))
u32 PU_PAGE_128K		= (0x10 << 1);

__attribute__((section(".dtcm")))
u32 PU_PAGE_256K		= (0x11 << 1);

__attribute__((section(".dtcm")))
u32 PU_PAGE_512K		= (0x12 << 1);

__attribute__((section(".dtcm")))
u32 PU_PAGE_1M		= (0x13 << 1);

__attribute__((section(".dtcm")))
u32 PU_PAGE_2M		= (0x14 << 1);

__attribute__((section(".dtcm")))
u32 PU_PAGE_4M		= (0x15 << 1);

__attribute__((section(".dtcm")))
u32 PU_PAGE_8M		= (0x16 << 1);

__attribute__((section(".dtcm")))
u32 PU_PAGE_16M		= (0x17 << 1);

__attribute__((section(".dtcm")))
u32 PU_PAGE_32M		= (0x18 << 1);

__attribute__((section(".dtcm")))
u32 PU_PAGE_64M		= (0x19 << 1);

__attribute__((section(".dtcm")))
u32 PU_PAGE_128M		= (0x1A << 1);

__attribute__((section(".dtcm")))
u32 PU_PAGE_256M		= (0x1B << 1);

__attribute__((section(".dtcm")))
u32 PU_PAGE_512M		= (0x1C << 1);

__attribute__((section(".dtcm")))
u32 PU_PAGE_1G		= (0x1D << 1);

__attribute__((section(".dtcm")))
u32 PU_PAGE_2G		= (0x1E << 1);

__attribute__((section(".dtcm")))
u32 PU_PAGE_4G		= (0x1F << 1);

__attribute__((section(".dtcm")))
u16 gbaIME = 0;

__attribute__((section(".dtcm")))
u16 gbaDISPCNT = 0;

__attribute__((section(".dtcm")))
u16 gbaBGxCNT[4] = {0, 0, 0, 0};

char disbuffer[0x2000];

#ifdef lastdebug
u32 lasttime[6];
int lastdebugcurrent = 0;
int lastdebugsize = 6;
#endif

int durchlauf = 0;
void debugDump()
{
#ifdef ichflyDebugdumpon
// 	Log("dbgDump\n");
// 	return;
	//readbankedextra(cpuGetSPSR());

	int i;
	for(i = 0; i <= 15; i++) {
		Log("R%d=%X ", i, exRegs[i]);
	} 
	Log("\n");
	Log("sp %X \n",spirq);

	/*if((exRegs[13] &0xFF000000) != 0x3000000)
	{
		REG_IME = IME_DISABLE;
		while(1);
	}*/
DC_FlushAll();
	cpu_SetCP15Cnt(cpu_GetCP15Cnt() & ~0x1); //disable pu else we may cause an endless loop

	for(i = 0; i < 4; i++) {
		Log("+%02X: %08X %08X %08X\n", i*3*4, ((u32*)exRegs[13])[i*3], ((u32*)exRegs[13])[i*3+1], ((u32*)exRegs[13])[i*3+2]);
	}

	pu_Enable(); //back to normal code
DC_FlushAll();
	Log("SPSR %08x ",BIOSDBG_SPSR);
	Log("CPSR %08x\n",cpuGetCPSR());
	//Log("irqBank %08x",readbankedsp(0x12));
	Log("irqBank %08x %08x\n",readbankedlr(0x12),readbankedsp(0x12));
	Log("userBank %08x %08x\n",readbankedlr(0x1F),readbankedsp(0x1F));
#ifdef lastdebug
	int ipr = lastdebugcurrent - 1;
	if(ipr < 0)ipr= lastdebugsize - 1;
	while(ipr != lastdebugcurrent)
	{
		Log("run %08X\n",lasttime[ipr]);
		ipr--;
		if(ipr < 0)ipr= lastdebugsize - 1;
	}
	Log("run %08X\n",lasttime[lastdebugcurrent]); //last
#endif
	Log("IEF: %08X\n",CPUReadMemoryreal(0x4000200));
	  u32 joy = ((~REG_KEYINPUT)&0x3ff);
	if((joy & KEY_B) && (joy & KEY_R) && (joy & KEY_L))
	{
					FILE* file = fopen("fat:/gbadump.bin", "wb"); // 396.288 Byte @outdate
					fwrite((u8*)(0x03000000), 1, 0x8000, file);
					fwrite(ioMem, 1, 0x400, file);
					fwrite((u8*)(0x04000000), 1, 0x400, file);//IO
					fwrite((u8*)(0x05000000), 1, 0x400, file);
					fwrite((u8*)(0x07000000), 1, 0x800, file);
					fwrite((u8*)(0x01000000), 1, 0x8000, file);
					fwrite((u8*)(0x0b000000), 1, 0x4000, file);
					fwrite((u8*)(0x06000000), 1, 0x18000, file); //can't get this with half dumps
					fwrite((u8*)(0x02000000), 1, 0x40000, file); //can't get this with half dumps
	}
#endif
}


void failcpphandler()
{
	iprintf("something failed\r\n");
	REG_IME = IME_DISABLE;
	debugDump();
	Log("SSP %08x SLR %08x\n",savedsp,savedlr);
		while(1);
}

__attribute__((section(".itcm")))
void undefinedExceptionHandler()
{
	//coto: by downgrading (ARM9) armv5 to armv4 and running always in ARMv4 mode we prevent the below undefined exception.
	
	//ichfly
	/*
	u32 tempforwtf = *(u32*)(exRegs[15] - 4);
	if((tempforwtf &0x0F200090) == 0x00200090) //wtf why dos this tigger an exeption it is strh r1,[r0]+2! ²àà 0xB2 10 E0 E0 on gba 0xE0E010B2 so think all strh rx,[ry]+z! do that
	{
		*(u32*)(exRegs[15] - 4) = tempforwtf & ~0x200000;//ther is just a wrong bit so don't worry patch it to strh r1,[r0]+2
	}
	else
	*/
	
	{
		printf("unknown OP:%x:[%x] \n",(exRegs[15] - 4), *(u32*)(exRegs[15] - 4));
		debugDump();
		REG_IME = IME_DISABLE;
		while(1);
	}
}
int durchgang = 0;

void gbaInit(bool slow)
{
	REG_IME = IME_DISABLE;

	cpu_SetCP15Cnt(cpu_GetCP15Cnt() & ~0x1); //disable pu while configurating pu
	
	pu_SetDataCachability(   0b00111110);
	pu_SetCodeCachability(   0b00111110);
	pu_GetWriteBufferability(0b00100010);	
	
	WRAM_CR = 0; //swap wram in
	pu_SetRegion(0, 0x00000000 | PU_PAGE_128M | 1);
	pu_SetRegion(1, 0x027C0000 | PU_PAGE_16K | 1);	//dtcm helper: enable I/Dtcm caches in DTCM region: gives speedup
	pu_SetRegion(2, 0x02040000 | PU_PAGE_256K | 1);	//ewram mirror gba #1, nds mode mpu traps this region: #1: 0x02040000 ~ 0x0207ffff / speedup access by MPU
	pu_SetRegion(3, 0x02080000 | PU_PAGE_256K | 1); //ewram mirror gba #2, nds mode mpu traps this region: #2: 0x02080000 ~ 0x020fffff / speedup access by MPU
	pu_SetRegion(4, 0x020C0000 | PU_PAGE_256K | 1); //ewram mirror gba #3, nds mode mpu traps this region: #3: 0x02100000 ~ 0x0213ffff / speedup access by MPU
	pu_SetRegion(5, 0x02100000 | PU_PAGE_1M | 1);	//nds ewram emu helper: enable I/Dtcm caches in EWRAM NDS emulator code region: gives speedup
	pu_SetRegion(6, 0x00000000 | PU_PAGE_16M | 1);
	pu_SetRegion(7, 0x04000000 | PU_PAGE_16M | 1);
	
	pu_Enable(); //PU go
	DC_FlushAll(); //try it	
	IC_InvalidateAll();
}

__attribute__((section(".itcm")))
void puGba()
{
	pu_SetCodePermissions(0x06300033);	
	pu_SetDataPermissions(0x06300033);
}

__attribute__((section(".itcm")))
void puNds()
{	
	pu_SetDataPermissions(0x33333333);
	pu_SetCodePermissions(0x33333333);
}

__attribute__((section(".itcm")))
void swiExceptionHandler()
{
	//Log("\n\rswi\n\r");
	//debugDump();
	//while(1);
	//Log("%08X S\n", readbankedsp(0x12));
	
	u16 tempforwtf = *(u16*)(exRegs[15] - 2);	//coto: todo: range check, swi from GBA area fetch will cause problems
	BIOScall(tempforwtf,  exRegs);
#ifdef lastdebug
	if(readbankedsp(0x12) < 0x1000000)debugandhalt();
lasttime[lastdebugcurrent] = exRegs[15] | 0x80000000;
lastdebugcurrent++;
if(lastdebugcurrent == lastdebugsize)lastdebugcurrent = 0;
#endif

	gbaMode();
	//while(1);
}


__attribute__((section(".itcm")))
void BIOScall(int op,  u32 *R)
{
	int comment = op & 0x003F;
	
	switch(comment) {
	  case 0x00:
		BIOS_SoftReset();
		break;
	  case 0x01:
		BIOS_RegisterRamReset(R[0]);
		break;
	  case 0x02:
#ifdef DEV_VERSION
	    Log("Halt: IE %x\n",IE);
#endif
		//holdState = true;
		//holdType = -1;
		//cpuNextEvent = cpuTotalTicks;
		
		ichflyswiHalt();
		//durchlauf = 1;
		
		//debugDump();
		
		//VblankHandler();
		
		break;
	  case 0x03:
#ifdef DEV_VERSION
		  Log("Stop\n");
#endif
			//holdState = true;
			//holdType = -1;
			//stopState = true;
			//cpuNextEvent = cpuTotalTicks;
			
			//coto: raise sleepmode swi 0x3 gba
			enterGBASleepMode();
			
			//ichflyswiIntrWait(1,(IE & 0x6080));
		  break;
	  case 0x04:

#ifdef DEV_VERSION
		  Log("IntrWait: 0x%08x,0x%08x\n",
			  R[0],
			  R[1]);      
#endif
		ichflyswiIntrWait(R[0],R[1]);
		//CPUSoftwareInterrupt();
		break;    
	  case 0x05:
#ifdef showdebug
		extern u16 IntrWaitnum;
		extern u32 VBlankIntrWaitentertimes;
		IntrWaitnum = REG_VCOUNT;
		VBlankIntrWaitentertimes++;

#endif
#ifdef DEV_VERSION
		  Log("VBlankIntrWait: 0x%08X 0x%08X\n",REG_IE,anytimejmpfilter);
		  //VblankHandler(); //todo
	#endif
		//if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
#ifdef powerpatches
		if((REG_DISPSTAT & DISP_IN_VBLANK) || (REG_VCOUNT < 60))frameasyncsync(); //hope it don't need more than 100 Lines this give the emulator more power
#endif
		//while(!(REG_DISPSTAT & DISP_IN_VBLANK));
		//send cmd
		REG_IPC_FIFO_TX = 0x1FFFFFFB; //tell the arm7
		REG_IPC_FIFO_TX = 0;
		ichflyswiWaitForVBlank();

		break;
	  case 0x06:
		BIOS_Div();
		break;
	  case 0x07:
		BIOS_DivARM();
		break;
	  case 0x08:
		BIOS_Sqrt();
		break;
	  case 0x09:
		BIOS_ArcTan();
		break;
	  case 0x0A:
		BIOS_ArcTan2();
		break;
	  case 0x0B:
		BIOS_CpuSet();
		break;
	  case 0x0C:
		BIOS_CpuFastSet();
		break;
	  case 0x0D:
		BIOS_GetBiosChecksum();
		break;
	  case 0x0E:
		BIOS_BgAffineSet();
		break;
	  case 0x0F:
		BIOS_ObjAffineSet();
		break;
	  case 0x10:
		BIOS_BitUnPack();
		break;
	  case 0x11:
		BIOS_LZ77UnCompWram();
		break;
	  case 0x12:
		BIOS_LZ77UnCompVram();
		break;
	  case 0x13:
		BIOS_HuffUnComp();
		break;
	  case 0x14:
		BIOS_RLUnCompWram();
		break;
	  case 0x15:
		BIOS_RLUnCompVram();
		break;
	  case 0x16:
		BIOS_Diff8bitUnFilterWram();
		break;
	  case 0x17:
		BIOS_Diff8bitUnFilterVram();
		break;
	  case 0x18:
		BIOS_Diff16bitUnFilter();
		break;
	  case 0x19:
	//#ifdef DEV_VERSION
		  Log("SoundBiasSet: 0x%08x \n",
			  R[0]);      
	//#endif    
		if(reg[0].I) //ichfly sound todo
		{
		  UPDATE_REG(0x88, 0x200);
		}
		else //ichfly sound todo
		{
		  UPDATE_REG(0x88, 0);
		}
		break;
	  case 0x1F:
		BIOS_MidiKey2Freq();
		break;
	  case 0x2A:
		BIOS_SndDriverJmpTableCopy();
		break;
		// let it go, because we don't really emulate this function
	  case 0x2D: //debug call all
		debugDump();
		break;
	  case 0x2F: //debug call all break
		//Log("irqBank %08x",readbankedsp(0x12));
		  debugDump();

		REG_IME = IME_DISABLE;
		while(1);
		break;
	  default:
		if((comment & 0x30) == 0x30)
		{
			iprintf("r%x %08x",(comment & 0xF),R[(comment & 0x30)]);
		}
		else
		{
			if(!disableMessage) {
			  systemMessage(MSG_UNSUPPORTED_BIOS_FUNCTION,
							N_("Unsupported BIOS function %02x. A BIOS file is needed in order to get correct behaviour."),
							comment);
			  disableMessage = true;
			}
		}
		break;
	  }
}

__attribute__((section(".itcm")))
void switch_to_unprivileged_mode()
{
	u32 temp = cpuGetCPSR();
	temp = temp & ~0x1F;
	temp = temp |= 0x10;
	cpuSetCPSR(temp);
}

__attribute__((section(".itcm")))
void CP15Vectors0x00000000()
{
	cpu_SetCP15Cnt(cpu_GetCP15Cnt() &~BIT(13));
}

__attribute__((section(".itcm")))
void CP15Vectors0xFFFF0000()
{
	cpu_SetCP15Cnt(cpu_GetCP15Cnt() | BIT(13));
}

void setGBAVectors()
{
	cpu_SetCP15Cnt(cpu_GetCP15Cnt() & ~0x1); //disable pu while configurating pu
	CP15Vectors0x00000000();
	setVectorsAsm();
}



__attribute__((section(".itcm")))
void ARMV5toARMV4Mode()
{
	cpu_SetCP15Cnt(cpu_GetCP15Cnt() | BIT(15));
}

__attribute__((section(".itcm")))
void ARMV4toARMV5Mode()
{
	cpu_SetCP15Cnt(cpu_GetCP15Cnt() &~ BIT(15));
}

__attribute__((section(".itcm")))
void ndsMode()
{
	puNds();
}

__attribute__((section(".itcm")))
void gbaMode()
{
	puGba();	
}

__attribute__((section(".itcm")))
void debugExceptionHandler(){
	iprintf("jump to debugExceptionHandler");
	while(1==1);
}