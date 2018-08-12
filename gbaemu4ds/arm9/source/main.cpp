/*---------------------------------------------------------------------------------

	Basic template code for starting a GBA app

---------------------------------------------------------------------------------*/
#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <filesystem.h>

#include "../../common/cpuglobal.h"
#include "../../common/gba_ipc.h"
#include "interrupts/fifo_handler.h"

#include "GBA.h"
#include "Sound.h"
#include "Util.h"
#include "getopt.h"
#include "System.h"
#include <fat.h>
#include <dirent.h>
#include "cpumg.h"
#include "bios.h"
#include "mydebuger.h"
#include "file_browse.h"
#include "arm7sound.h"
#include "main.h"
#include "ipc/touch_ipc.h"
#include "dswifi_arm9/dswnifi_lib.h"

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

char biosPath[MAXPATHLEN * 2];
char patchPath[MAXPATHLEN * 2];
char savePath[MAXPATHLEN * 2];
char szFile[MAXPATHLEN * 2];

u8 arm7exchangefild[0x100];

char* savetypeschar[7] =
	{"SaveTypeAutomatic","SaveTypeEeprom","SaveTypeSram","SaveTypeFlash64KB","SaveTypeEepromSensor","SaveTypeNone","SaveTypeFlash128KB"};

char* memoryWaitrealram[8] =
  { "10 and 6","8 and 6","6 and 6","18 and 6","10 and 4","8 and 4","6 and 4","18 and 4" };

u32 arm7amr9buffer = 0;

#ifdef anyarmcom
u32 amr7sendcom = 0;
u32 amr7senddma1 = 0;
u32 amr7senddma2 = 0;
u32 amr7recmuell = 0;
u32 amr7directrec = 0;
u32 amr7indirectrec = 0;
u32 amr7fehlerfeld[10];
#endif


__attribute__((section(".dtcm")))
int frameskip = 10;

//---------------------------------------------------------------------------------
int main( int argc, char **argv) {
	
	ARMV5toARMV4Mode();	//so undefined resolver deals with proper armv4 opcodes and we iron out patches // for upcoming prefetch logic
	
	biosPath[0] = 0;
	savePath[0] = 0;
	patchPath[0] = 0;

	//---------------------------------------------------------------------------------
	//set the mode for 2 text layers and two extended background layers
	//videoSetMode(MODE_5_2D); 
	videoSetModeSub(MODE_5_2D);	
	//defaultExceptionHandler();	//for debug befor gbainit
	
	vramSetPrimaryBanks(	
	VRAM_A_MAIN_BG_0x06000000,      //Mode0 Tile/Map mode	//Mode 1/2/3/4 special bitmap/rotscale modes
	VRAM_B_LCD, //6820000h-683FFFFh	getVRAMHeapStart(); here
	VRAM_C_LCD,	//6840000h-685FFFFh
	VRAM_D_LCD	//6860000h-687FFFFh
	);

	vramSetBanks_EFG(
	VRAM_E_MAIN_SPRITE,        //E       64K   2    -     6400000h
	VRAM_F_LCD,                	//F       16K   0    -     6890000h-6893FFFh
	VRAM_G_LCD
	);
	vramSetBankH(VRAM_H_SUB_BG); //only sub //for prints to lowern screan 
	vramSetBankI(VRAM_I_SUB_BG_0x06208000); //only sub

	#ifdef advanced_irq_check
		irqSet(IRQ_HBLANK,HblankHandler); //todo async
	#ifdef forceHBlankirqs
		irqEnable(IRQ_HBLANK);
	#endif
	#endif
	
	__irqSet(IRQ_FIFO_NOT_EMPTY,HandleFifo,irqTable); //todo async
	irqEnable(IRQ_FIFO_NOT_EMPTY);
	REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_SEND_CLEAR | IPC_FIFO_RECV_IRQ;
	
	//bg = bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0,0);
	consoleDemoInit();

	iprintf("Init Fat...");
    if(fatInitDefault()){
        iprintf("OK\n");
    }else{
        iprintf("failed\n");
        int i = 0;
		while(i< 300)
		{
			//swiWaitForVBlank();
			if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
			while(!(REG_DISPSTAT & DISP_IN_VBLANK));
			i++;
		}
    }
	
	REG_POWERCNT &= ~(POWER_3D_CORE | POWER_MATRIX);	//poweroff 3d
	
	//ds init end
	
	
	//gba init (non gba system related, but hypervisor init)
	
	//screen settings
	if(argv[5][0] == '1')
	{
		lcdSwap();
	}
	
	//main menü
	strcpy(szFile,argv[1]);
	strcpy(savePath,argv[2]);
	strcpy(biosPath,argv[3]);
	strcpy(patchPath,argv[4]);
	if(argv[11][0] == '1')cpuIsMultiBoot = true;
	else cpuIsMultiBoot = false;
	
	u32 manual_save_type = (u32)strtol(argv[6],NULL,16);
	frameskip = (u32)strtol(argv[7],NULL,16);
	arm9VCOUNTsyncline =(int)strtol(argv[9],NULL,16);
	//bool slow;
	//if(argv[10][0] == '1')slow = true;
	//else slow = false;
	if(argv[8][0] == '1')
	{
		REG_IPC_FIFO_TX = 0x1FFFFFFC; //send cmd
		REG_IPC_FIFO_TX = 0;
	}
	
	//test the FIFO if NDS cmds and GBA cmds work .. ok they work
	//REG_IPC_FIFO_TX = FIFO_DEBUG;
	//REG_IPC_FIFO_TX = 0;
	
	//rootenabelde[2] = fatMountSimple  ("sd", &__io_dsisd); //DSi//sems to be inited by fatInitDefault
	//fatInitDefault();
	//nitroFSInit();
	
	//sound data buff
	#ifdef arm9advsound
	REG_IPC_FIFO_TX = 0x1FFFFFFA; //load buffer

	#ifdef anyarmcom
	*(u32*)arm7exchangefild = (u32)&amr7sendcom;
	*(u32*)(arm7exchangefild + 4) = (u32)&amr7senddma1;
	*(u32*)(arm7exchangefild + 8) = (u32)&amr7senddma2;
	*(u32*)(arm7exchangefild + 12) = (u32)&amr7recmuell;
	*(u32*)(arm7exchangefild + 16) = (u32)&amr7directrec;
	*(u32*)(arm7exchangefild + 20) = (u32)&amr7indirectrec;
	*(u32*)(arm7exchangefild + 24) = (u32)&amr7fehlerfeld[0];
	#endif

	REG_IPC_FIFO_TX = arm7amr9buffer = (u32)arm7exchangefild; //buffer for arm7
	#endif
	
	//main menü ende aber bleibe im while
	//dirfolder("nitro:/");	
	
	//one-way GBA subsystem initializers
	
	#ifdef usebuffedVcout
	initspeedupfelder();
	#endif
	
	parseDebug = true;
    
	//GBA subsystem that can be re-launched
	reloadGBA(szFile,manual_save_type);
	
	while(true){
	}
	return 0;
}
