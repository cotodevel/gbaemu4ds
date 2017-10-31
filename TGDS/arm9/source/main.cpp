/***********************************************************/
/* This source is part of SNEmulDS                         */
/* ------------------------------------------------------- */
/* (c) 1997-1999, 2006-2007 archeide, All rights reserved. */
/***********************************************************/
/*
 This program is free software; you can redistribute it and/or 
 modify it under the terms of the GNU General Public License as 
 published by the Free Software Foundation; either version 2 of 
 the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, 
 but WITHOUT ANY WARRANTY; without even the implied warranty of 
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
 GNU General Public License for more details.
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

int argc;
sint8 **argv;
int main(int _argc, sint8 **_argv) {
	
	IRQInit();
	
	bool project_specific_console = true;	//set default console or custom console: custom console
	GUI_init(project_specific_console);
	GUI_clear();
	
	sint32 fwlanguage = (sint32)getLanguage();
	GUI_setLanguage(fwlanguage);
	
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
	//switch_dswnifi_mode(dswifi_udpnifimode);	//UDP NIFI: Check readme
	//local nifi: 
	//switch_dswnifi_mode(dswifi_localnifimode);	//LOCAL NIFI:
	
	while (1)
	{
        /*	//todo
		if (REG_POWERCNT & POWER_SWAP_LCDS){
			GUI_update();
		}
        */
		
		/*
		//Press L to send a frame to the other DS
		if ((keysPressed() & KEY_L)){
			FrameSenderUser = HandleSendUserspace((uint8*)nfdata,sizeof(nfdata));	//make room for crc16 frame
		}
		*/
		
		//EntryPoint here.
	}

}