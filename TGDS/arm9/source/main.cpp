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
#include "memoryHandleTGDS.h"
#include "reent.h"
#include "sys/types.h"
#include "dsregs.h"
#include "dsregs_asm.h"
#include "typedefsTGDS.h"
#include "consoleTGDS.h"
#include "utilsTGDS.h"
#include "devoptab_devices.h"
#include "fsfatlayerTGDSLegacy.h"
#include "usrsettingsTGDS.h"
#include "videoTGDS.h"
#include "keypadTGDS.h"
#include "guiTGDS.h"
#include "dswnifi_lib.h"

#include "Globals.h"
#include "interrupts.h"
#include "GBA.h"
#include "Util.h"
#include "biosTGDS.h"
#include "dmaTGDS.h"
#include "cpumg.h"
#include "posixHandleTGDS.h"
#include "gui_console_connector.h"
#include "keypadTGDS.h"
#include "bios.h"

int argc;
sint8 **argv;

u32 anytimejmpfilter = 0; 

//todo
void show_mem(){}

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

uint8 * stubbedGBAEWRAMPtr;

char biosPath[MAXPATHLEN * 2];		//bios path
char patchPath[MAXPATHLEN * 2];		//patch path
char savePath[MAXPATHLEN * 2];		//save path
char szFile[MAXPATHLEN * 2];		//file path

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
	
	REG_IE = 0;
	
	EnableIrq(IRQ_HBLANK | IRQ_RECVFIFO_NOT_EMPTY);
	REG_IPC_FIFO_CR |= (1<<2);  //send empty irq
    REG_IPC_FIFO_CR |= (1<<10); //recv empty irq
    
    *(u16*)0x04000184 = *(u16*)0x04000184 | (1<<15); //enable fifo send recv
	REG_IF = ~0;
	REG_POWERCNT &= ~((POWER_3D_CORE | POWER_MATRIX) & 0xFFFF);//powerOff(POWER_3D_CORE | POWER_MATRIX); //3D use power so that is not needed
	DISPCNT  = 0x0080;
	
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
	
	//this project uses the start 0x02000000 ~ 0x02040000 as gba wram (256K), so malloc must stub that memory for further malloc operations.
	stubbedGBAEWRAMPtr=(uint8*)malloc(256*1024);
	if(stubbedGBAEWRAMPtr){
		//OK stubbed correctly
	}
	else{
		clrscr();
		printf("stubPtr Failed. Can't proceed");
		while(1==1){}
	}
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
		printf("%08X\r ",i);
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
	
	//add GUI!
	GUI_getROM(getfatfsPath((char*)"gba"));	//returns: file.ext-> szFile	<- char * recv from arg: full dir path to look files by extension at
	GUI_deleteROMSelector();
	
	//file.ext -> fullpath/file.ext
	char tempFname[0x400];
	strcpy(tempFname, (const char*)szFile);
	memset ((uint8*)&szFile[0], 0, sizeof(szFile));
	sprintf(szFile,"%s%s",getfatfsPath((char*)"gba/"),tempFname);
	printf("Trying:%s Press A",szFile);
	while (1)
	{	
		if ((KEYCNT_READ() & KEY_A))
		break;
	}
	
	printf("CPULoadRom...");
	
	failed = !CPULoadRom(szFile,extraram);

	if(failed)
	{
		printf("failed:%s",szFile);
		while(1);
	}
	else{
		printf("OK:%s",szFile);
	}
	
	//so far here OK
	
	//coto: save detection code from whitelist
	printf("trying save_decider()");
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
	gbaSaveType = cpuSaveType;
	//so far here ok
	
	printf("trying CPUInit...");
	CPUInit(biosPath, useBios,extraram);
	printf("CPUInit!");
	
	//so far here ok
	
	CPUReset();
	
	printf("CPUReset ");
	
	// so far here ok
	
	//Create new savefile or load if exists...
	int filepathlen = strlen(szFile);
	char  fn_noext[filepathlen] = {0};
	memcpy(fn_noext,szFile,filepathlen-3);

	//detect savefile (filename.sav)
	sprintf(fn_noext,"%ssav",fn_noext);
	FILE * frh = fopen(fn_noext,"r");

	//if(frh)
	//    printf("current save path: %s DO exists",fn_noext);
	//else
	//    printf("current save path: %s DONT exists",fn_noext);  
	//while(1);
	
	//coto: added create new savefile
	
	sint32 fd = -1;
	struct fd * fdinst = NULL;
	if(frh){
		fd = fileno(frh);
		fdinst = fd_struct_get(fd);
		strcpy ((char *)savePath, (const char *)fn_noext);	//override always saveFile name
		if(fdinst->filPtr){	//File open OK.
			if(CPUReadBatteryFile(savePath))
			{
				if(cpuSaveType == 0)printf("SaveReadOK![AUTO] ");
				if(cpuSaveType == 1)printf("SaveReadOK![EEPROM] ");
				if(cpuSaveType == 2)printf("SaveReadOK![SRAM] ");
				if(cpuSaveType == 3)printf("SaveReadOK![FLASHROM] ");
				if(cpuSaveType == 4)printf("SaveReadOK![EEPROM+SENSOR] ");
				if(cpuSaveType == 5)printf("SaveReadOK![NONE] ");			
			}
			else
			{
				printf("well failed reading: %s ",savePath);
				while(1);
			}
		}
		else{	//File open ERROR.
			printf("no savefile found, creating new one...  ");
			//append "sav"
			CPUWriteBatteryFile(savePath);
		}
	}
	
	fclose(frh);
	//printf("SaveFile Handle! ");
	
	//so far ok
	cpu_SetCP15Cnt(cpu_GetCP15Cnt() & ~0x1); //disable pu to write to the internalRAM
	
	BIOS_RegisterRamReset(0xFF);
	printf("BIOS_RegisterRamReset ");
	
	pu_Enable();
	
	
	//dmaTransfer(0, (uint32)rom, 0x2000000, 0x40000);	//file copy: 256K minimal payload	//already copied image earlier
	//printf("dmaCopy Success:%x",(uint32)*(uint32*)0x2000000);
	
	//printf("arm7init ");
	anytimejmpfilter = 0;
	
	//VblankHandler();
	
	u32 syncline = 159;
	
	//ok so far here
	
	VblankHandler();
	REG_IPC_FIFO_TX = 0x1FFFFFFF; //cmd
	REG_IPC_FIFO_TX = syncline;
	while(!(REG_IPC_FIFO_CR & IRQ_RECVFIFO_NOT_EMPTY))u32 src = REG_IPC_FIFO_RX;
	printf("irqinit\n");
	
	anytimejmpfilter = 0;
	emulateedbiosstart();
	printf("emulateedbiosstart ");
	
	//so far here
	ndsMode();
	printf("ndsMode ");
	//so far here
	SendMultipleWordACK(0xc1710001,0x0,0x0,0x0);//fifotest
	printf("gbaInit ");
	
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
	REG_IME = IME_ENABLE;
	gbaMode2();
	
	#ifndef capture_and_pars
		printf("jump to %x-> %x  ",(unsigned int)rom,*(uint32*)rom);
	#endif
	
	
	//can't use printf here lol the writes are handled by the MPU
	cpu_ArmJumpforstackinit((u32)0x02000000, 0);

	while (1)
	{
		IRQVBlankWait();
	}

}