#include <nds.h>
#include "wifi_shared.h"
#include "gba_ipc.h"

#ifdef ARM7
#include <string.h>
#include "wifi_arm7.h"
#endif

#ifdef ARM9

#include <stdbool.h>
#include "dsregs.h"
#include "wifi_arm9.h"
#include "../arm9/source/dswifi_arm9/sgIP.h"

extern sgIP_Hub_HWInterface * wifi_hw;

#endif


//internal DSWIFI code
void DeInitWIFI(){
	#ifdef ARM7
	Wifi_Deinit();
	#endif
	#ifdef ARM9
	Wifi_DisconnectAP();
	Wifi_DisableWifi();
	REG_IE &= ~IRQ_TIMER3;
	Wifi_SetSyncHandler(NULL);
	sgIP_Hub_RemoveHardwareInterface(wifi_hw);
	TIMER3_DATA = 0;
	TIMER3_CR = 0;
	SendArm7Command(DSWIFI_DEINIT_WIFI, 0);
	#endif
}