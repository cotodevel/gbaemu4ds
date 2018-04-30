/*---------------------------------------------------------------------------------

	Basic template code for starting a GBA app

---------------------------------------------------------------------------------*/
#include <nds.h>
#include <stdio.h>

//Shared memory region static object
#include "../../common/clock.h"
#include "../../common/gba_ipc.h"

//reload app
#include "./hbmenustub/nds_loader_arm9.h"

//IPC
#include "./ipc_arm9.h"

//SBRK mem map Handler
#include "./memory_handler_gba/mem_handler.h"

//irq
#include "arm9_interrupts.h"

//wifi
#include "multi.h"

//adpcm
#include "adpcm/av_mem.h"
#include "adpcm/avcodec.h"


#include <filesystem.h>
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
#include "ds_dma.h"
#include "file_browse.h"
#include "main.h"
#include "fatmore.h"
#include "irqstuf.arm.h"

#include <stdio.h>
#include <stdlib.h>
#include <nds/memory.h>
#include <nds/ndstypes.h>
#include <nds/memory.h>
#include <nds/bios.h>
#include <nds/system.h>
#include <nds/arm9/math.h>
#include <nds/arm9/video.h>
#include <nds/arm9/videoGL.h>
#include <nds/arm9/trig_lut.h>
#include <nds/arm9/sassert.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

#include <dswifi9.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "httpstack/http_utils.h"
#include "httpstack/client_http_handler.h"

//single codec context setup
AVCodecContext AVCodecContext_inst;

int adpcm_codec_status = 0;

u8 arm7exchangefild[0x100];

char* savetypeschar[7] =
	{(char*)"SaveTypeAutomatic",(char*)"SaveTypeEeprom",(char*)"SaveTypeSram",(char*)"SaveTypeFlash64KB",(char*)"SaveTypeEepromSensor",(char*)"SaveTypeNone",(char*)"SaveTypeFlash128KB"};

char* memoryWaitrealram[8] =
  { (char*)"10 and 6",(char*)"8 and 6",(char*)"6 and 6",(char*)"18 and 6",(char*)"10 and 4",(char*)"8 and 4",(char*)"6 and 4",(char*)"18 and 4" };


//---------------------------------------------------------------------------------
int main( int argc, char **argv) {

set_cp15_vectors_0xFFFF0000();

//reboot app
bool isdsisdhardware=false;
installBootStub(isdsisdhardware);

biosPath[0] = 0;
savePath[0] = 0;
patchPath[0] = 0;

//needed to raise IRQs otherwise ARM7 is stuck on an interrupt that is not served
REG_DISPCNT  = 0x0080;
DISPCNT  = 0x0080;

REG_IME = IME_DISABLE;
pu_Disable(); //disable pu to setup the whole NDS MAP

set_gba_ipc_regs();

//bugged mode 0
/*
//set the first two banks as background memory and the third as sub background memory
//D is not used..if you need a bigger background then you will need to map
//more vram banks consecutivly (VRAM A-D are all 0x20000 bytes in size)
vramSetPrimaryBanks(	
VRAM_A_MAIN_BG_0x06000000,  //for gba
VRAM_B_MAIN_BG_0x06020000, 
VRAM_C_SUB_BG_0x06200000 , VRAM_D_LCD); //needed for main emulator

vramSetBanks_EFG(
VRAM_E_MAIN_SPRITE, //for gba sprite
VRAM_F_LCD,         //cant use
VRAM_G_LCD,        //cant use
);

vramSetBankH(VRAM_H_SUB_BG); //only sub //for prints to lowern screan
vramSetBankI(VRAM_I_SUB_BG_0x06208000); //only sub

//---------------------------------------------------------------------------------
//set the mode for 2 text layers and two extended background layers
//videoSetMode(MODE_5_2D);
videoSetModeSub(MODE_5_2D);
*/

//https://mtheall.com/vram.html#MS=2&TS=2&T0=1&NT0=256&MB0=0&TB0=2&S0=3&T1=1&NT1=256&MB1=4&TB1=3&S1=3&T2=1&NT2=256&MB2=8&TB2=4&S2=3&T3=1&NT3=256&MB3=12&TB3=5&S3=3

//new correct PPU setup : set the mode for 2 text layers and two extended background layers
videoSetModeSub(MODE_5_2D);

vramSetPrimaryBanks(	
VRAM_A_MAIN_BG_0x06000000,      //Mode0 Tile/Map mode
VRAM_B_MAIN_BG_0x06020000,      //Mode 1/2/3/4 special bitmap/rotscale modes (engine A bg0,1,2,3 needs them)
VRAM_C_LCD, 
VRAM_D_LCD
); //needed for main emulator


vramSetBanks_EFG(
VRAM_E_MAIN_SPRITE,        //GBA -> NDS Sprites (same PPU format!)
VRAM_F_LCD,                
VRAM_G_LCD
);
vramSetBankH(VRAM_H_SUB_BG); //only sub //for prints to lowern screan 
vramSetBankI(VRAM_I_SUB_BG_0x06208000); //only sub

//IRQ Setup
//coto: ARM9 exception vectors are MPU protected (returns gba.bios ,if any, reads)
//You can't use the default exception handlers because the GBA mode writes the REG_IF / REG_IE directly (through MPU protection)
//NDS IRQs do not clean IF (not at least NDS/GBA IRQs & 0xFFFF), so we have to clean the REG_IF we use manually (on handler)
//ARM7 trigger ARM9 HBLANK/VBLANK/VCOUNT IRQs. (resembles the GBA cpu overall hardware timings better resulting in less battery usage)

	__irqSet_gba(IRQ_HBLANK,(IntFn)HblankHandler_nds,	irqTable); 
	//__irqSet_gba(IRQ_VBLANK,(IntFn)VblankHandler_nds,	irqTable);
	//__irqSet_gba(IRQ_VCOUNT,(IntFn)vcounthandler,	irqTable);
	__irqSet_gba(IRQ_FIFO_NOT_EMPTY,(IntFn)handlefifo,	irqTable);

	//fifoInit();
	
	REG_IPC_SYNC = 0;
	REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_SEND_CLEAR | IPC_FIFO_RECV_IRQ;
	REG_IPC_FIFO_CR |= (1<<2);  //send empty irq
	REG_IPC_FIFO_CR |= (1<<10); //recv empty irq
	
	irqEnable_gba(IRQ_HBLANK | IRQ_FIFO_NOT_EMPTY);

	*(u16*)0x04000184 = *(u16*)0x04000184 | (1<<15); //enable fifo send recv

	REG_IF = ~0;

REG_POWERCNT &= ~((POWER_3D_CORE | POWER_MATRIX) & 0xFFFF);//powerOff(POWER_3D_CORE | POWER_MATRIX); //3D use power so that is not needed

//bg = bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0,0);
consoleDemoInit();    

asm("nop");
asm("nop");

//coto: initial sbrk handler setup
//0x3a000 = 237.568 bytes reserved to ARM9
// free kernel mem = (unsigned int)( (u32)this_heap_ptr - ((u32)&__end__) )

this_heap_ptr = sbrk_init();
free_allocable_mem = calc_remaining_kernel_mem();

asm("nop");
asm("nop");

iprintf("gbaemu4DS \n");

bool temptest = true;
if(argv[5][0] == '1')
{
	lcdSwap();
}

if(!(_io_dldi_stub.friendlyName[0] == 0x52 && _io_dldi_stub.friendlyName[5] == 0x4E) && temptest)
{
	iprintf("if %s is real hardware , as long it has dldi r/w gbaemu4ds should work. \n Press A ",_io_dldi_stub.friendlyName);
	while(1) {
		scanKeys();
		if (keysDown()&KEY_A) break;
		if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
		while(!(REG_DISPSTAT & DISP_IN_VBLANK));
	}
}
//data protbuff: install dma buffer over arm7
SendArm7Command(0xc0700100,0x1FFFFFFA,(u32)(&dmaexchange_buf),0x0);

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

//frameskip
frameskip = (u32)strtol(argv[7],NULL,16);

//gba_frame_line
gba_frame_line = (u32)strtol(argv[9],NULL,16);

//ARM7 go
if(argv[8][0] == '1')
{
	SendArm7Command(0xc0700100,0x1FFFFFFC,0x0,0x0);
}

bool extraram =false; 
parseDebug = true;

bool failed = false;
strcpy(szFile,argv[1]);
strcpy(savePath,argv[2]);
strcpy(biosPath,argv[3]);
strcpy(patchPath,argv[4]);

if(argv[11][0] == '1')cpuIsMultiBoot = true;
else cpuIsMultiBoot = false;


iprintf("Press A to enable WIFI then, choose Continue when asked. \n");
iprintf("Press B to disable WIFI. \n");

client_http_handler_context.wifi_enabled = false;
while(1) {
    scanKeys();
	if (keysDown()&KEY_A){ 
        client_http_handler_context.wifi_enabled = true;
        break;
    }
    if (keysDown()&KEY_B){ 
        client_http_handler_context.wifi_enabled = false;
        break;
    }
	if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
	while(!(REG_DISPSTAT & DISP_IN_VBLANK));
}

//initNiFi();   //localplay not yet

//wifi init  
if(client_http_handler_context.wifi_enabled == true){
    
    client_http_handler_context.is_busy = true; //required otherwise youll pump data that does not exist!
    
    iprintf("WifiInit \n");
    
    if(Wifi_InitDefault(WFC_CONNECT) == true) 
    {
        iprintf("Connected : NDS IP Client Address: %s \n\n ",(char*)print_ip((u32)Wifi_GetIP()));
    }
    else
        iprintf("Failed to connect! \n\n ");
    
    //new connection
    request_connection(server_ip,strlen(server_ip));

    client_http_handler_context.is_busy = false;
    
    REG_IME = IME_DISABLE;
}
else{
    //coto: WIFI disabled then
}

iprintf("CPULoadRom...");
failed = !CPULoadRom(szFile,extraram);
if(failed)
{
    printf("failed");
	while(1);
}
iprintf("OK\n");

//conflicts with wifi
//detect size here

//coto: decide savetype based off gamecode. Returns VBA save enum index
save_decider();

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
    //iprintf("no savefile found, creating new one... \n");
    //append "sav"
    
    //void * memcpy ( void * destination, const void * source, size_t num );
    
    //char * strcat ( char * destination, const char * source );
    
    savePath[0] = 0;
    strcpy ((char *)savePath, (const char *)fn_noext);
    CPUWriteBatteryFile(savePath);
    
    //void * memset ( void * ptr, int value, size_t num );
}

else
{
    asm("nop");
    strcpy ((char *)savePath, (const char *)fn_noext);
    /*
    if(cpuSaveType == 0)iprintf("[AUTO]");
    if(cpuSaveType == 1)iprintf("[EEPROM]");
    if(cpuSaveType == 2)iprintf("[SRAM]");
    if(cpuSaveType == 3)iprintf("[FLASHROM]");
    if(cpuSaveType == 4)iprintf("[EEPROM+SENSOR]");
    if(cpuSaveType == 5)iprintf("[NONE]");
    */
    iprintf("CPUReadBatteryFile...");
	if(CPUReadBatteryFile(savePath))
	{
        iprintf("OK\n");
	}
	else
	{
        iprintf("failed reading: %s\n",savePath);
		while(1);
	}
    fclose(frh);
}
	
iprintf("BIOS_RegisterRamReset\n");
BIOS_RegisterRamReset(0xFF);

iprintf("arm7vcountirqinit\n");
SendArm7Command(0xc4740100,gba_frame_line,0x0,0x0);

iprintf("irqinit\n");
IEBACKUP = 0;

iprintf("MPU Vectors @ 0x00000000 \n");
set_cp15_vectors_0x00000000();
	
asm("nop");
asm("nop");
asm("nop");

iprintf("ndsMode\n");
ndsMode();

asm("nop");
asm("nop");
asm("nop");

//asm("nop");
//asm("nop");
//asm("nop");

//adpcm_codec_status = adpcm_encode_init(&AVCodecContext_inst);

REG_IF = IRQ_FIFO_NOT_EMPTY;
//iprintf("GBAEMU4DS_IPC Size: %d bytes. \n",sizeof(tMyIPC));
iprintf("%d-%d-%d / %d:%d:%d \n GBA mode jump @ 0x02040000 %x \n",gba_get_monthrtc(),gba_get_dayrtc(),gba_get_yearbytertc(),gba_get_hourrtc(),gba_get_minrtc(),gba_get_secrtc(),rom[0]);
cpu_ArmJumpforstackinit((u32)0x02040000,0);

while(true){
    asm("swi 0x5");	//swi wait vblank gba mode
}

return 0;
}