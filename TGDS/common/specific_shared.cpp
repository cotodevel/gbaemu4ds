
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

#include "common_shared.h"
#include "specific_shared.h"
#include "dsregs.h"
#include "dsregs_asm.h"
#include "InterruptsARMCores_h.h"

#ifdef ARM7
#include <string.h>

#include "main.h"
#include "wifi_arm7.h"
#include "spifw.h"
#include "dmaIO.h"

#endif

#ifdef ARM9

#include <stdbool.h>
#include "main.h"
#include "wifi_arm9.h"

#endif


//inherits what is defined in: common_shared.c
#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void HandleFifoNotEmptyWeakRef(uint32 cmd1,uint32 cmd2,uint32 cmd3,uint32 cmd4){
	
	switch (cmd1) {
		
		//NDS7: uses NDS IPC FIFO as a layer from GBA IO @ ARM9
		#ifdef ARM7
		//debug
		case(0xc0700100):{
			SendMultipleWordACK(0xc0700101,0x0,0x0,0x0);
		}
		break;
		
		//vcounter
		case(0xc1710000):{
			
		}
		break;
		
		//fifo test
		case(0xc1710001):{
			SendMultipleWordACK(0xc1710002,0x0,0x0,0x0);
		}
		break;
		
		//FIFO DMA SA update FIFO_INTERNAL_BUFFER(A)
		case(0xc1710004):{
			SendMultipleWordACK(0xc6760000,0x0,0x0,0x0);
		}
		break;
		
		//FIFO DMA SB update FIFO_INTERNAL_BUFFER(B)
		case(0xc1710005):{
			SendMultipleWordACK(0xc6760001,0x0,0x0,0x0);
		}
		break;
		
		case(0xc3730000):{
			iowrite(cmd2,cmd3);
		}
		break;
		#endif
		
		//NDS9: uses NDS IPC FIFO as a layer from GBA IO @ ARM7
		#ifdef ARM9
		
		#endif
	}
	
}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void HandleFifoEmptyWeakRef(uint32 cmd1,uint32 cmd2,uint32 cmd3,uint32 cmd4){
}

//project specific stuff
