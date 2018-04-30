/*---------------------------------------------------------------------------------
	Touch screen control for the ARM7

	Copyright (C) 2005
		Michael Noland (joat)
		Jason Rogers (dovoto)
		Dave Murphy (WinterMute)
		put in pocketSPC by Nicolas Lannier (archeide)

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any
	damages arising from the use of this software.

	Permission is granted to anyone to use this software for any
	purpose, including commercial applications, and to alter it and
	redistribute it freely, subject to the following restrictions:

	1.	The origin of this software must not be misrepresented; you
			must not claim that you wrote the original software. If you use
			this software in a product, an acknowledgment in the product
			documentation would be appreciated but is not required.
	2.	Altered source versions must be plainly marked as such, and
			must not be misrepresented as being the original software.
	3.	This notice may not be removed or altered from any source
			distribution.

---------------------------------------------------------------------------------*/

#include "touch_ipc.h"
#include "../../common/gba_ipc.h"

#include <nds/ndstypes.h>
#include <nds/system.h>
#include <nds/arm7/touch.h>
#include <nds/input.h>
#include <nds/interrupts.h>

#include <nds/bios.h>
#include <nds/arm7/clock.h>
#include <nds/ipc.h>
#include <stdlib.h>

//coto: we better restore original libnds touchscreen... really.
void gbaemu4ds_ipc()
{
	uint16 but=0, batt=0;
	int t1=0, t2=0;
	uint32 temp=0;
	u32 i=0;
	u8 clock_buf[sizeof(GBAEMU4DS_IPC->clockdata)];
    
    touchPosition tempPos;
    touchReadXY(&tempPos);
    
	// Read the touch screen
	but = REG_KEYXY;
	batt = touchRead(TSC_MEASURE_BATTERY);
 
	// Read the time
	rtcGetTimeAndDate(clock_buf);

	// Read the temperature
	temp = touchReadTemperature(&t1, &t2);
 
	GBAEMU4DS_IPC->mailBusy = 1;
	
    // Update the IPC struct
	GBAEMU4DS_IPC->buttons		= but;
	GBAEMU4DS_IPC->touched      = ((tempPos.px > 0) || (tempPos.py > 0)) ? 1 : 0;
	
    //raw x/y
    GBAEMU4DS_IPC->touchX    = tempPos.rawx;
    GBAEMU4DS_IPC->touchY    = tempPos.rawy;
    
    //TFT x/y pixel
    GBAEMU4DS_IPC->touchXpx = tempPos.px;
    GBAEMU4DS_IPC->touchYpx = tempPos.py;    
    
	GBAEMU4DS_IPC->touchZ1 = tempPos.z1;
	GBAEMU4DS_IPC->touchZ2 = tempPos.z2;
	GBAEMU4DS_IPC->battery		= batt;
	
    //Get time
    for(i=0; i< sizeof(clock_buf); i++){
        GBAEMU4DS_IPC->clockdata[i] = clock_buf[i];
    }
    
	GBAEMU4DS_IPC->temperature = temp;
	GBAEMU4DS_IPC->tdiode1 = t1;
	GBAEMU4DS_IPC->tdiode2 = t2;	
    
    GBAEMU4DS_IPC->mailBusy = 0;
}