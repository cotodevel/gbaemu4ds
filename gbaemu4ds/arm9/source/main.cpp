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

	DISPCNT  = 0x0080;

	//rootenabelde[2] = fatMountSimple  ("sd", &__io_dsisd); //DSi//sems to be inited by fatInitDefault
	 //fatInitDefault();
	 //nitroFSInit();
	bool temptest = true;

	//data protbuff
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
	//main menü ende aber bleibe im while
	//dirfolder("nitro:/");	
	bool nichtausgewauhlt = true;
		
	//screen settings
	REG_POWERCNT &= ~(POWER_3D_CORE | POWER_MATRIX);	//poweroff 3d
	if(argv[5][0] == '1')
	{
		lcdSwap();
	}

		
	iprintf("\x1b[2J");
	//main menü

	strcpy(szFile,argv[1]);
	strcpy(savePath,argv[2]);
	strcpy(biosPath,argv[3]);
	strcpy(patchPath,argv[4]);
	if(argv[11][0] == '1')cpuIsMultiBoot = true;
	else cpuIsMultiBoot = false;
	int myflashsize = 0x10000;

	u32 manual_save_type = (u32)strtol(argv[6],NULL,16);
	frameskip = (u32)strtol(argv[7],NULL,16);
	arm9VCOUNTsyncline =(int)strtol(argv[9],NULL,16);
	bool slow;
	if(argv[10][0] == '1')slow = true;
	else slow = false;
	if(argv[8][0] == '1')
	{
		REG_IPC_FIFO_TX = 0x1FFFFFFC; //send cmd
		REG_IPC_FIFO_TX = 0;
	}
	
	//test the FIFO if NDS cmds and GBA cmds work .. ok they work
	//REG_IPC_FIFO_TX = FIFO_DEBUG;
	//REG_IPC_FIFO_TX = 0;
	
	bool extraram =false; 
	#ifdef usebuffedVcout
	initspeedupfelder();
	#endif
	
	iprintf("\x1b[2J");
	parseDebug = true;
    bool failed = false;
 
	iprintf("ARM9 @ARMv4 ok: CPULoadRom...");
    failed = !CPULoadRom(szFile,extraram);

	if(failed){
		printf("failed");
		while(1);
	}
	iprintf("OK\n");
	
	if(save_decider()==0){
		if(manual_save_type == 6)
		{
			myflashsize = 0x20000;
			cpuSaveType = 3;
		}
		else
		{
			cpuSaveType = manual_save_type;
		}
	}
	
	iprintf("CPUInit\n");
	CPUInit(biosPath, useBios,extraram);
	
	iprintf("CPUReset\n");
	CPUReset();

	int filepathlen = strlen(szFile);
	char  fn_noext[filepathlen] = {0};
	memcpy(fn_noext,szFile,filepathlen-3);

	//detect savefile (filename.sav)
	sprintf(fn_noext,"%ssav",fn_noext);
	FILE * frh = fopen(fn_noext,"r");

	//if(frh)
	//    iprintf("current save path: %s DO exists",fn_noext);
	//else
	//    iprintf("current save path: %s DONT exists",fn_noext);  
	//while(1);

	//coto: added create new savefile
	if(!frh){
		iprintf("no savefile found, creating new one... \n");
		//append "sav"
		
		//void * memcpy ( void * destination, const void * source, size_t num );
		
		//char * strcat ( char * destination, const char * source );
		
		savePath[0] = 0;
		strcpy ((char *)savePath, (const char *)fn_noext);
		CPUWriteBatteryFile(savePath);
		//void * memset ( void * ptr, int value, size_t num );
	}
	else{
		strcpy ((char *)savePath, (const char *)fn_noext);
		if(CPUReadBatteryFile(savePath))
		{
			if(cpuSaveType == 0)iprintf("SaveReadOK![AUTO]\n");
			if(cpuSaveType == 1)iprintf("SaveReadOK![EEPROM]\n");
			if(cpuSaveType == 2)iprintf("SaveReadOK![SRAM]\n");
			if(cpuSaveType == 3)iprintf("SaveReadOK![FLASHROM]\n");
			if(cpuSaveType == 4)iprintf("SaveReadOK![EEPROM+SENSOR]\n");
			if(cpuSaveType == 5)iprintf("SaveReadOK![NONE]\n");			
		}
		else
		{
			iprintf("failed reading: %s\n",savePath);
			while(1);
		}
		fclose(frh);
	}
	
	REG_IME = IME_ENABLE;
	
	iprintf("BIOS_RegisterRamReset\n");
	cpu_SetCP15Cnt(cpu_GetCP15Cnt() & ~0x1); //disable pu to write to the internalRAM
	BIOS_RegisterRamReset(0xFF);
	
	printf("Press A for Nifi");
	printf("Press B for Idle");
	while(1) {
		scanKeys();
		if (keysDown()&KEY_A){ 
			switch_dswnifi_mode(dswifi_localnifimode);
			break;
		}
		if (keysDown()&KEY_B){ 
			switch_dswnifi_mode(dswifi_idlemode);
			break;
		}
		if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
		while(!(REG_DISPSTAT & DISP_IN_VBLANK));
	}
	
	iprintf("arm7init\n");
	REG_IPC_FIFO_TX = 0x1FFFFFFF; //cmd
	REG_IPC_FIFO_TX = (u32)arm9VCOUNTsyncline;
	while(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)){
		u32 src = REG_IPC_FIFO_RX;
	}
	iprintf("irqinit\n");
	anytimejmpfilter = 0;
	
	setGBAVectors();
	ndsMode();
	iprintf("Vectors @ 0x00000000 \n");
    pu_Enable();
	#ifdef capture_and_pars
	videoBgDisableSub(0);
	vramSetBankH(VRAM_H_LCD); //only sub
	vramSetBankI(VRAM_I_LCD); //only sub
	int iback = bgInitSub(3, BgType_ExRotation, BgSize_B16_256x256, 0,0);

	bgSetRotateScale(iback,0,0x0F0,0x0D6);
	bgUpdate();
	#endif
	
	gbaInit(slow);
	gbaMode();
	iprintf("GBA mode. Jumping to %x \n",rom);
	cpu_ArmJumpforstackinit((u32)rom, 0);
	
	while(true){
		REG_IF = IRQ_HBLANK;
	}
	return 0;
}
