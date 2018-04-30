#include "touch_ipc.h"
#include <nds.h>
#include <nds/touch.h>
#include "../../../common/gba_ipc.h"

//usage: struct touchPosition touchposition;
//touchReadXY_gbaemu4ds(&touchposition);

void touchReadXY_gbaemu4ds(touchPosition * touchpos_inst){
    
    touchpos_inst->rawx =   GBAEMU4DS_IPC->touchX;
    touchpos_inst->rawy =   GBAEMU4DS_IPC->touchY;
    
    //TFT x/y pixel
    touchpos_inst->px   =   GBAEMU4DS_IPC->touchXpx;
    touchpos_inst->py   =   GBAEMU4DS_IPC->touchYpx;
    
    touchpos_inst->z1   =   GBAEMU4DS_IPC->touchZ1;
    touchpos_inst->z2   =   GBAEMU4DS_IPC->touchZ2;
}