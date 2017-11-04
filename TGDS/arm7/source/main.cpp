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


#include <string.h>
#include "main.h"
#include "InterruptsARMCores_h.h"
#include "interrupts.h"

#include "common_shared.h"
#include "specific_shared.h"

#include "wifi_arm7.h"
#include "usrsettings.h"
#include "timer.h"
#include "bios.h"
#include "spifw.h"
#include "dmaIO.h"

//---------------------------------------------------------------------------------
int main(int _argc, sint8 **_argv) {
//---------------------------------------------------------------------------------
	IRQInit();
	
	// Block execution until we get control of vram D
	while (!(*((vuint8*)0x04000240) & 0x2));
	
	installWifiFIFO();
	
	ykeypp = false;
	isincallline = false;
	
	powerON(POWER_SOUND);
	REG_SOUNDCNT = SOUND_ENABLE | SOUND_VOL(0x7F);
	
	//4000500h - NDS7 - SOUNDCNT - Sound Control Register (R/W)
    //Bit0-6   Master Volume       (0..127=silent..loud)
    //Bit7     Not used            (always zero)
    //Bit15    Master Enable       (0=Disable, 1=Enable)
    u8 master_vol = 120;
    *(u16*)0x04000500 = (master_vol<<0) | (1<<15);
	
    while (1) {
		
		//sound alloc
		//0-3 matching gba
		//4-5 FIFO
		//ledBlink(1);
		//swiWaitForVBlank();
		if(!(REG_KEYXY & 0x2))
		{
			if(!ykeypp)
			{
				//REG_IPC_FIFO_TX = 0x4200BEEF;
				SendMultipleWordACK(0x4200BEEF,0x0,0x0,0x0);
/*
#ifdef anyarmcom
				*amr7sendcom = *amr7sendcom + 1;
#endif				//while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY);
*/
				//int val2 = REG_IPC_FIFO_RX; //Value skip
				ykeypp = true;

			}
		}
		else
		{
			ykeypp = false;
		}
		
		/*
		if(*(u16*)0x04000136 & 0x80) //close nds
		{
			u32 ie_save = REG_IE;
			// Turn the speaker down.
			if (REG_POWERCNT & 1) swiChangeSndBias(0,0x400);
			// Save current power state.
			u32 power = PowerManagementDeviceRead((int)POWMAN_CONTROL_BIT);
			// Set sleep LED.
			//writePowerManagement(POWMAN_CONTROL_BIT, PM_LED_CONTROL(1));
			// Register for the lid interrupt.
			REG_IE = IRQ_SCREENLID;
			// Power down till we get our interrupt.
			//swiSleep(); //waits for PM (lid open) interrupt
			//100ms
			//swiDelay(838000);
			// Restore the interrupt state.
			//REG_IE = ie_save;
			// Restore power state.
			//writePowerManagement(PM_CONTROL_REG, power);
			// Turn the speaker up.
			if (REG_POWERCNT & 1) swiChangeSndBias(1,0x400);
			// update clock tracking
			//resyncClock(); 
		}
		*/
		
		IRQVBlankWait();
	}
   
	return 0;
}