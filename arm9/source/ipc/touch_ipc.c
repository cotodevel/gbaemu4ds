#include "touch_ipc.h"
#include <nds.h>
#include <nds/touch.h>
#include "../../../common/gba_ipc.h"

//usage: struct touchPosition touchposition;
//touchReadXY_gbaemu4ds(&touchposition);

void touchReadXY_gbaemu4ds(touchPosition * touchpos_inst){
    
	struct sIPCSharedGBA* GBAIPC = GetsIPCSharedGBA();
    touchpos_inst->rawx =   GBAIPC->touchX;
    touchpos_inst->rawy =   GBAIPC->touchY;
    
    //TFT x/y pixel
    touchpos_inst->px   =   GBAIPC->touchXpx;
    touchpos_inst->py   =   GBAIPC->touchYpx;
    
    touchpos_inst->z1   =   GBAIPC->touchZ1;
    touchpos_inst->z2   =   GBAIPC->touchZ2;
}