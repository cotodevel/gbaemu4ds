/*

			Copyright (C) 2017  Coto
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301
USA

*/
#include "socket.h"
#include "in.h"
#include <netdb.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>


#include "gbaemu4ds_fat_ext.h"

#include "main.h"
#include "InterruptsARMCores_h.h"
#include "specific_shared.h"
#include "ff.h"
#include "mem_handler_shared.h"
#include "reent.h"
#include "sys/types.h"
#include "dsregs.h"
#include "dsregs_asm.h"
#include "typedefs.h"
#include "console.h"
#include "toolchain_utils.h"
#include "devoptab_devices.h"
#include "fsfat_layer.h"
#include "usrsettings.h"
#include "video.h"
#include "keypad.h"
#include "gui.h"
#include "dswnifi_lib.h"

#include "Globals.h"
#include "interrupts.h"
#include "GBA.h"
#include "Util.h"
#include "bios.h"
#include "ipc.h"
#include "dma.h"
#include "cpumg.h"
#include "posix_hook_shared.h"

int argc;
sint8 **argv;

//dmabuffer (old gbaemu4ds way)
//volatile u8 arm7exchangefild[0x100];

/*
#ifdef anyarmcom
u32 amr7sendcom = 0;
u32 amr7senddma1 = 0;
u32 amr7senddma2 = 0;
u32 amr7recmuell = 0;
u32 amr7directrec = 0;
u32 amr7indirectrec = 0;
u32 amr7fehlerfeld[10];
#endif
*/


char biosPath[MAXPATHLEN * 2];
char patchPath[MAXPATHLEN * 2];
char savePath[MAXPATHLEN * 2];
char szFile[MAXPATHLEN * 2];

char* savetypeschar[7] =
	{"SaveTypeAutomatic","SaveTypeEeprom","SaveTypeSram","SaveTypeFlash64KB","SaveTypeEepromSensor","SaveTypeNone","SaveTypeFlash128KB"};

char* memoryWaitrealram[8] =
  { "10 and 6","8 and 6","6 and 6","18 and 6","10 and 4","8 and 4","6 and 4","18 and 4" };

int main(int _argc, sint8 **_argv) {
	
	IRQInit();
	
	//ori
	/*
		//set the first two banks as background memory and the third as sub background memory
		//D is not used..if you need a bigger background then you will need to map
		//more vram banks consecutivly (VRAM A-D are all 0x20000 bytes in size)
		vramSetPrimaryBanks(
		VRAM_A_MAIN_BG_0x06000000,//for gba 
		VRAM_B_LCD, 
		VRAM_C_SUB_BG_0x06200000,
		VRAM_D_MAIN_BG_0x06020000
		); //needed for main emulator
		
		vramSetBanks_EFG(
		VRAM_E_MAIN_SPRITE//for gba sprite,
		VRAM_F_LCD,
		VRAM_G_LCD
		);
		vramSetBankH(VRAM_H_SUB_BG); //only sub //for prints to lowern screen 
		vramSetBankI(VRAM_I_SUB_BG_0x06208000); //only sub
	*/
	
	//todo this
	//coto: fixed mode1 (partially)
	/*
	vramSetPrimaryBanks(	
	VRAM_A_MAIN_BG_0x06000000,      //Mode0 Tile/Map mode
	VRAM_B_MAIN_BG_0x06020000,      //Mode 1/2/3/4 special bitmap/rotscale modes (engine A bg0,1,2,3 needs them)
	VRAM_C_LCD,
	VRAM_D_LCD
	); //needed for main emulator
	*/

	bool project_specific_console = true;	//set default console or custom console: custom console
	GUI_init(project_specific_console);
	GUI_clear();
	
	REG_POWERCNT &= ~((POWER_3D_CORE | POWER_MATRIX) & 0xFFFF);//powerOff(POWER_3D_CORE | POWER_MATRIX); //3D use power so that is not needed
	
	sint32 fwlanguage = (sint32)getLanguage();
	GUI_setLanguage(fwlanguage);
	
	//SETDISPCNT_SUB(MODE_5_2D);	//already handled by the TGDS console
	
	biosPath[0] = 0;
	savePath[0] = 0;
	patchPath[0] = 0;

	int ret=FS_init();
	if (ret == 0)
	{
		printf("FS Init ok.");
	}
	else if(ret == -1)
	{
		printf("FS Init error.");
	}
	
	//single player:
	switch_dswnifi_mode(dswifi_idlemode);
	//udp nifi: 
	//switch_dswnifi_mode(dswifi_udpnifimode);	//UDP NIFI: Check https://github.com/cotodevel/ToolchainGenericDS-multiplayer-example	//so far NDS7 works
	//local nifi: 
	//switch_dswnifi_mode(dswifi_localnifimode);	//LOCAL NIFI:	//so far NDS7 works
	
	//set up GBA part
	GBA_DISPCNT  = 0x0080;
	
	//coto:
	/*
	//dma shared buffer (old)
	#ifdef arm9advsound
	REG_IPC_FIFO_TX = 0x1FFFFFFA; //load buffer
	REG_IPC_FIFO_TX =  (u32)&arm7exchangefild[0]; //buffer for arm7
	#endif
	*/

	/*REG_IPC_FIFO_TX = 0;
	while(true)
	{
		int i = REG_IPC_FIFO_RX;
		printf("%08X\r\n",i);
		REG_IPC_FIFO_TX = i;

	}*/
	
	//main menü ende aber bleibe im while
	bool extraram =false; 
	bool failed = false;
	bool slow = false;
	
	u32 manual_save_type = 0 ;
	
	//set up paths.. todo
	strcpy(szFile,"");
	strcpy(savePath,"");
	strcpy(biosPath,"");
	strcpy(patchPath,"");
	
	//64K default Savesize
	int myflashsize = 0x10000;
	
	#ifdef usebuffedVcout
	initspeedupfelder();
	#endif
	
	clrscr();
	printf("CPULoadRom...");
	
	failed = !CPULoadRom(szFile,extraram);

	if(failed)
	{
		printf("failed");
		while(1);
	}
	else{
		printf("OK\n");
	}
	
	//coto: save detection code from whitelist...
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
	
	//printf("Hello World2!");
	printf("CPUInit\n");
	CPUInit(biosPath, useBios,extraram);
	
	printf("CPUReset\n");
	CPUReset();
	
	//Create new savefile or load if exists...
	int filepathlen = strlen(szFile);
	char  fn_noext[filepathlen] = {0};
	memcpy(fn_noext,szFile,filepathlen-3);

	//detect savefile (filename.sav)
	sprintf(fn_noext,"%ssav",fn_noext);
	FILE * frh = fopen_fs(fn_noext,"r");

	//if(frh)
	//    printf("current save path: %s DO exists",fn_noext);
	//else
	//    printf("current save path: %s DONT exists",fn_noext);  
	//while(1);
	
	//coto: added create new savefile
	if(!frh){
		printf("no savefile found, creating new one... \n");
		//append "sav"
		
		//void * memcpy ( void * destination, const void * source, size_t num );
		
		//char * strcat ( char * destination, const char * source );
		
		savePath[0] = 0;
		strcpy ((char *)savePath, (const char *)fn_noext);
		CPUWriteBatteryFile(savePath);
	}
	else
	{
		strcpy ((char *)savePath, (const char *)fn_noext);
		if(CPUReadBatteryFile(savePath))
		{
			if(cpuSaveType == 0)printf("SaveReadOK![AUTO]\n");
			if(cpuSaveType == 1)printf("SaveReadOK![EEPROM]\n");
			if(cpuSaveType == 2)printf("SaveReadOK![SRAM]\n");
			if(cpuSaveType == 3)printf("SaveReadOK![FLASHROM]\n");
			if(cpuSaveType == 4)printf("SaveReadOK![EEPROM+SENSOR]\n");
			if(cpuSaveType == 5)printf("SaveReadOK![NONE]\n");			
		}
		else
		{
			printf("failed reading: %s\n",savePath);
			while(1);
		}
		fclose_fs(frh);
	}
	
	printf("BIOS_RegisterRamReset\n");
	cpu_SetCP15Cnt(cpu_GetCP15Cnt() & ~0x1); //disable pu to write to the internalRAM
	BIOS_RegisterRamReset(0xFF);
	pu_Enable();
	
	dmaTransfer(0, (uint32)rom, 0x2000000, 0x40000);	//file copy: 256K minimal payload
	printf("dmaCopy Success:%x",(uint32)*(uint32*)0x2000000);
	
	printf("arm7init ");
	anytimejmpfilter = 0;
	
	VblankHandler();
	
	u32 syncline = 159;
	//ori
	//REG_IPC_FIFO_TX = 0x1FFFFFFF; //cmd
	//REG_IPC_FIFO_TX = syncline;
	
	SendMultipleWordACK(0x1FFFFFFF,syncline,0,0);
	
	while(!(REG_IPC_FIFO_CR & RECV_FIFO_IPC_EMPTY))u32 src = REG_IPC_FIFO_RX;
	printf("irqinit\n");
	
	printf("emulateedbiosstart\n");
	emulateedbiosstart();
	
	printf("ndsMode\n");
	ndsMode();
	
	SendMultipleWordACK(0xc1710001,0x0,0x0,0x0);//fifotest
	printf("gbaInit\n");
	
	/*
	#ifdef capture_and_pars
		videoBgDisableSub(0);
		vramSetBankH(VRAM_H_LCD); //only sub
		vramSetBankI(VRAM_I_LCD); //only sub
		 int iback = bgInitSub(3, BgType_ExRotation, BgSize_B16_256x256, 0,0);

		//bgSetScale(3,0x111,0x133);
		//bgSetRotateScale(iback,0,0x111,0x133);
		//bgSetRotateScale(iback,0,0x0F0,0x0D5);
		bgSetRotateScale(iback,0,0x0F0,0x0D6);
		bgUpdate();
	#endif
	*/
	
	gbaInit(slow);
	
	#ifndef capture_and_pars
		printf("gbaMode2\n");
	#endif
		REG_IME = IME_ENABLE;
		gbaMode2();
	#ifndef capture_and_pars
		printf("jump to (%08X)  ",(unsigned int)rom);
	#endif

	//printf("\x1b[2J"); //reset (not working huh)
	//show_mem();
	
	cpu_ArmJumpforstackinit((u32)rom, 0);

	while (1)
	{
		IRQVBlankWait();
	}

}