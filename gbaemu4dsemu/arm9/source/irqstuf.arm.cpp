#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <fat.h>
#include <dirent.h>
#include <filesystem.h>

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

#include <nds/input.h> //ARM9 cant access HINGE,TOUCH,XY BUTTONS
#include <nds/touch.h>


#include "httpstack/client_http_handler.h"
#include "irqstuf.arm.h"

#include "GBA.h"
#include "Sound.h"
#include "Util.h"
#include "getopt.h"
#include "System.h"

#include "cpumg.h"
#include "bios.h"
#include "main.h"
#include "mydebuger.h"
#include "file_browse.h"
#include "fatmore.h"

//reload app
#include "./hbmenustub/nds_loader_arm9.h"
#include "./ipc_arm9.h"


//Shared memory region static object
#include "../../common/clock.h"
#include "../../common/gba_ipc.h"
#include "../../common/wifi_shared.h"
#include "ichflysettings.h"


#include <nds/disc_io.h>
#include <dirent.h>

__attribute__((section(".dtcm")))
u16 DISPCNT;
//volatile u16 DISPCNT  = 0x0080;
int framenummer;

char* rootdirnames [3] = {(char*)"nitro:/",(char*)"fat:/",(char*)"sd:/"};
int ausgewauhlt = 0;
int bg = 0;
int ignorenextY = 0;

/*
 LCD VRAM Overview

The GBA contains 96 Kbytes VRAM built-in, located at address 06000000-06017FFF, depending on the BG Mode used as follows:

BG Mode 0,1,2 (Tile/Map based Modes)

  06000000-0600FFFF  64 KBytes shared for BG Map and Tiles
  06010000-06017FFF  32 KBytes OBJ Tiles

The shared 64K area can be split into BG Map area(s), and BG Tiles area(s), the respective addresses for Map and Tile areas are set up by BG0CNT-BG3CNT registers. The Map address may be specified in units of 2K (steps of 800h), the Tile address in units of 16K (steps of 4000h).

BG Mode 0,1 (Tile/Map based Text mode)
The tiles may have 4bit or 8bit color depth, minimum map size is 32x32 tiles, maximum is 64x64 tiles, up to 1024 tiles can be used per map.

  Item        Depth     Required Memory
  One Tile    4bit      20h bytes
  One Tile    8bit      40h bytes
  1024 Tiles  4bit      8000h (32K)
  1024 Tiles  8bit      10000h (64K) - excluding some bytes for BG map
  BG Map      32x32     800h (2K)
  BG Map      64x64     2000h (8K)


BG Mode 1,2 (Tile/Map based Rotation/Scaling mode)
The tiles may have 8bit color depth only, minimum map size is 16x16 tiles, maximum is 128x128 tiles, up to 256 tiles can be used per map.

  Item        Depth     Required Memory
  One Tile    8bit      40h bytes
  256  Tiles  8bit      4000h (16K)
  BG Map      16x16     100h bytes
  BG Map      128x128   4000h (16K)


BG Mode 3 (Bitmap based Mode for still images)

  06000000-06013FFF  80 KBytes Frame 0 buffer (only 75K actually used)
  06014000-06017FFF  16 KBytes OBJ Tiles


BG Mode 4,5 (Bitmap based Modes)

  06000000-06009FFF  40 KBytes Frame 0 buffer (only 37.5K used in Mode 4)
  0600A000-06013FFF  40 KBytes Frame 1 buffer (only 37.5K used in Mode 4)
  06014000-06017FFF  16 KBytes OBJ Tiles


Note
Additionally to the above VRAM, the GBA also contains 1 KByte Palette RAM (at 05000000h) and 1 KByte OAM (at 07000000h) which are both used by the display controller as well.


 LCD VRAM Character Data

Each character (tile) consists of 8x8 dots (64 dots in total). The color depth may be either 4bit or 8bit (see BG0CNT-BG3CNT).

4bit depth (16 colors, 16 palettes)
Each tile occupies 32 bytes of memory, the first 4 bytes for the topmost row of the tile, and so on. Each byte representing two dots, the lower 4 bits define the color for the left (!) dot, the upper 4 bits the color for the right dot.

8bit depth (256 colors, 1 palette)
Each tile occupies 64 bytes of memory, the first 8 bytes for the topmost row of the tile, and so on. Each byte selects the palette entry for each dot.


 LCD VRAM BG Screen Data Format (BG Map)

The display background consists of 8x8 dot tiles, the arrangement of these tiles is specified by the BG Screen Data (BG Map). The separate entries in this map are as follows:

Text BG Screen (2 bytes per entry)
Specifies the tile number and attributes. Note that BG tile numbers are always specified in steps of 1 (unlike OBJ tile numbers which are using steps of two in 256 color/1 palette mode).

  Bit   Expl.
  0-9   Tile Number     (0-1023) (a bit less in 256 color mode, because
                           there'd be otherwise no room for the bg map)
  10    Horizontal Flip (0=Normal, 1=Mirrored)
  11    Vertical Flip   (0=Normal, 1=Mirrored)
  12-15 Palette Number  (0-15)    (Not used in 256 color/1 palette mode)

A Text BG Map always consists of 32x32 entries (256x256 pixels), 400h entries = 800h bytes. However, depending on the BG Size, one, two, or four of these Maps may be used together, allowing to create backgrounds of 256x256, 512x256, 256x512, or 512x512 pixels, if so, the first map (SC0) is located at base+0, the next map (SC1) at base+800h, and so on.

Rotation/Scaling BG Screen (1 byte per entry)
In this mode, only 256 tiles can be used. There are no x/y-flip attributes, the color depth is always 256 colors/1 palette.

  Bit   Expl.
  0-7   Tile Number     (0-255)

The dimensions of Rotation/Scaling BG Maps depend on the BG size. For size 0-3 that are: 16x16 tiles (128x128 pixels), 32x32 tiles (256x256 pixels), 64x64 tiles (512x512 pixels), or 128x128 tiles (1024x1024 pixels).

The size and VRAM base address of the separate BG maps for BG0-3 are set up by BG0CNT-BG3CNT registers.


 LCD VRAM Bitmap BG Modes

In BG Modes 3-5 the background is defined in form of a bitmap (unlike as for Tile/Map based BG modes). Bitmaps are implemented as BG2, with Rotation/Scaling support. As bitmap modes are occupying 80KBytes of BG memory, only 16KBytes of VRAM can be used for OBJ tiles.

BG Mode 3 - 240x160 pixels, 32768 colors
Two bytes are associated to each pixel, directly defining one of the 32768 colors (without using palette data, and thus not supporting a 'transparent' BG color).

  Bit   Expl.
  0-4   Red Intensity   (0-31)
  5-9   Green Intensity (0-31)
  10-14 Blue Intensity  (0-31)
  15    Not used

The first 480 bytes define the topmost line, the next 480 the next line, and so on. The background occupies 75 KBytes (06000000-06012BFF), most of the 80 Kbytes BG area, not allowing to redraw an invisible second frame in background, so this mode is mostly recommended for still images only.

BG Mode 4 - 240x160 pixels, 256 colors (out of 32768 colors)
One byte is associated to each pixel, selecting one of the 256 palette entries. Color 0 (backdrop) is transparent, and OBJs may be displayed behind the bitmap.
The first 240 bytes define the topmost line, the next 240 the next line, and so on. The background occupies 37.5 KBytes, allowing two frames to be used (06000000-060095FF for Frame 0, and 0600A000-060135FF for Frame 1).

BG Mode 5 - 160x128 pixels, 32768 colors
Colors are defined as for Mode 3 (see above), but horizontal and vertical size are cut down to 160x128 pixels only - smaller than the physical dimensions of the LCD screen.
The background occupies exactly 40 KBytes, so that BG VRAM may be split into two frames (06000000-06009FFF for Frame 0, and 0600A000-06013FFF for Frame 1).

In BG modes 4,5, one Frame may be displayed (selected by DISPCNT Bit 4), the other Frame is invisible and may be redrawn in background.
*/

int frameskip = 10;
int framewtf = 0;


//frame counter
int nds_frame = 0;

//line that generates a gbaframe
int gba_frame_line = 0;

__attribute__ ((aligned (4)))
__attribute__((section(".itcm")))
//---------------------------------------------------------------------------------
void HblankHandler_nds(void) {
//---------------------------------------------------------------------------------
    REG_IF = IRQ_HBLANK;
	
}

//blackscreen
__attribute__ ((aligned (4)))
__attribute__((section(".itcm")))
void HblankHandler_gba(void){
    DISPSTAT |= (REG_DISPSTAT & 0x3);
    DISPSTAT |= 0x2;	//hblank
    DISPSTAT &= 0xFFFe; //remove vblank
    UPDATE_REG(0x04, DISPSTAT);
	
    CPUCheckDMA(2, 0x0f);
}
//---------------------------------------------------------------------------------
__attribute__ ((aligned (4)))
__attribute__((section(".itcm")))
void VblankHandler_nds(void) {
//---------------------------------------------------------------------------------

	REG_IF = IRQ_VBLANK;
}


__attribute__ ((aligned (4)))
__attribute__((section(".itcm")))
void VblankHandler_gba(void){
DISPSTAT |= (REG_DISPSTAT & 0x3);
	DISPSTAT |= 0x1;	//vblank
	DISPSTAT &= 0xFFFd; //remove hblank
	UPDATE_REG(0x04, DISPSTAT);

	CPUCheckDMA(1, 0x0f); //V-Blank	
    
    #ifndef own_bg_render
	if(framewtf == frameskip)
	{
		framewtf = 0;
		if((DISPCNT & 7) < 3)
		{
			dmaCopyWords(3,(void*)((u32)vram + 0x10000),(void*)0x06400000,0x8000);
		}
		else
		{
			dmaCopyWords(3,(void*)0x06014000,(void*)0x06404000,0x4000);
			if((DISPCNT & 7) == 3) //BG Mode 3 - 240x160 pixels, 32768 colors
			{
				u8 *pointertobild = (u8 *)(0x6000000);
				for(int iy = 0; iy <160; iy++){
					dmaCopy( (void*)pointertobild, (void*)(0x06020000+(512*iy)), 480);
					pointertobild+=480;
				}
			}
			else
			{
				if((DISPCNT & 7) == 4) //BG Mode 4 - 240x160 pixels, 256 colors (out of 32768 colors)
				{
					u8 *pointertobild = (u8 *)(0x6000000);
					if(BIT(4) & DISPCNT)pointertobild+=0xA000;
					for(int iy = 0; iy <160; iy++){
						dmaCopy( (void*)pointertobild, (void*)(0x06020000+(256*iy)), 240);
						pointertobild+=240;
						//pointertobild+=120;
					}
				}
				else
				{
					//if((DISPCNT & 7) == 5) //BG Mode 5 - 160x128 pixels, 32768 colors //ichfly can't be other mode
					{
						u8 *pointertobild = (u8 *)(0x6000000);
						if(BIT(4) & DISPCNT)pointertobild+=0xA000;
						for(int iy = 0; iy <128; iy++){
							dmaCopy( (void*)pointertobild, (void*)(0x06020000+(512*iy)), 320);
							pointertobild+=320;
						}
					}
				}
			}
		}
	}
	else
	{
		framewtf++;
	}
    
    #else
    
    if(framewtf == frameskip)
	{
		framewtf = 0;
        //render 
        switch(DISPCNT & 7){
            //text mode
            case(0):{
                
                //sprites
                dmaCopyWords(3,(void*)((u32)vram + 0x10000),(void*)0x06400000,32*1024);
                
                /* //too slow
                //char base block
                u8 charblock_bg0 = ((BG0CNT>>2)&0x3);
                u8 charblock_bg1 = ((BG1CNT>>2)&0x3);
                u8 charblock_bg2 = ((BG2CNT>>2)&0x3);
                u8 charblock_bg3 = ((BG3CNT>>2)&0x3);
                
                //map base 
                u8 mapblock_bg0 = ((BG0CNT>>8)&0x1f);
                u8 mapblock_bg1 = ((BG1CNT>>8)&0x1f);
                u8 mapblock_bg2 = ((BG2CNT>>8)&0x1f);
                u8 mapblock_bg3 = ((BG3CNT>>8)&0x1f);
                
                int charblock_bg0size = 0;
                int charblock_bg1size = 0;
                int charblock_bg2size = 0;
                int charblock_bg3size = 0;
                
                //define bg0 size
                switch(((BG0CNT>>14)&0x3)){
                    case(0):{
                        charblock_bg0size=1024*2;
                    }
                    break;
                    case(1):{
                        charblock_bg0size=1024*4;
                    }
                    break;
                    case(2):{
                        charblock_bg0size=1024*4;
                    }
                    break;
                    case(3):{
                        charblock_bg0size=1024*8;
                    }
                    break;
                }
                
                switch(((BG1CNT>>14)&0x3)){
                    case(0):{
                        charblock_bg1size=1024*2;
                    }
                    break;
                    case(1):{
                        charblock_bg1size=1024*4;
                    }
                    break;
                    case(2):{
                        charblock_bg1size=1024*4;
                    }
                    break;
                    case(3):{
                        charblock_bg1size=1024*8;
                    }
                    break;
                }
                
                switch(((BG2CNT>>14)&0x3)){
                    case(0):{
                        charblock_bg2size=1024*2;
                    }
                    break;
                    case(1):{
                        charblock_bg2size=1024*4;
                    }
                    break;
                    case(2):{
                        charblock_bg2size=1024*4;
                    }
                    break;
                    case(3):{
                        charblock_bg2size=1024*8;
                    }
                    break;
                }
                
                switch(((BG3CNT>>14)&0x3)){
                    case(0):{
                        charblock_bg3size=1024*2;
                    }
                    break;
                    case(1):{
                        charblock_bg3size=1024*4;
                    }
                    break;
                    case(2):{
                        charblock_bg3size=1024*4;
                    }
                    break;
                    case(3):{
                        charblock_bg3size=1024*8;
                    }
                    break;
                }
                
                //char base block
                //backgrounds: detect all of them from GBA regs and point them to NDS engine backgrounds.
                dmaCopyWords(3,(void*)((u32)vram + (charblock_bg0*16384)),(void*)bgGetMapBase(bg_0_setting),charblock_bg0size);
                dmaCopyWords(3,(void*)((u32)vram + (charblock_bg1*16384)),(void*)bgGetMapBase(bg_1_setting),charblock_bg1size);
                dmaCopyWords(3,(void*)((u32)vram + (charblock_bg2*16384)),(void*)bgGetMapBase(bg_2_setting),charblock_bg2size);
                dmaCopyWords(3,(void*)((u32)vram + (charblock_bg3*16384)),(void*)bgGetMapBase(bg_3_setting),charblock_bg3size);
                
                //map base block
                if((DISPCNT>>8)&1){
                    dmaCopyWords(3,(void*)((u32)vram + (charblock_bg0*16384)),(void*)bgGetGfxPtr(bg_0_setting)+ (charblock_bg0*16384),charblock_bg0size);
                    dmaCopyWords(3,(void*)((u32)vram + (mapblock_bg0*2048)),(void*)bgGetMapPtr(bg_0_setting)+(mapblock_bg0*2048),charblock_bg0size);
                }
                
                if((DISPCNT>>9)&1){
                    dmaCopyWords(3,(void*)((u32)vram + (charblock_bg1*16384)),(void*)bgGetGfxPtr(bg_1_setting)+ (charblock_bg1*16384),charblock_bg1size);
                    dmaCopyWords(3,(void*)((u32)vram + (mapblock_bg1*2048)),(void*)bgGetMapPtr(bg_1_setting)+(mapblock_bg1*2048),charblock_bg1size);
                }
                if((DISPCNT>>10)&1){
                    dmaCopyWords(3,(void*)((u32)vram + (charblock_bg2*16384)),(void*)bgGetGfxPtr(bg_2_setting)+ (charblock_bg2*16384),charblock_bg2size);
                    dmaCopyWords(3,(void*)((u32)vram + (mapblock_bg2*2048)),(void*)bgGetMapPtr(bg_2_setting)+(mapblock_bg2*2048),charblock_bg2size);
                }
                if((DISPCNT>>11)&1){
                    dmaCopyWords(3,(void*)((u32)vram + (charblock_bg3*16384)),(void*)bgGetGfxPtr(bg_3_setting)+ (charblock_bg3*16384),charblock_bg3size);
                    dmaCopyWords(3,(void*)((u32)vram + (mapblock_bg3*2048)),(void*)bgGetMapPtr(bg_3_setting)+(mapblock_bg3*2048),charblock_bg3size);
                }
                */
            }
            break;
            
            //rotscale
            
            case(1):{
                //sprites
                dmaCopyWords(3,(void*)((u32)vram + 0x10000),(void*)0x06400000,32*1024);
                
                /*
                //char base block
                u8 charblock_bg0 = ((BG0CNT>>2)&0x3);
                u8 charblock_bg1 = ((BG1CNT>>2)&0x3);
                u8 charblock_bg2 = ((BG2CNT>>2)&0x3);
                u8 charblock_bg3 = ((BG3CNT>>2)&0x3);
                
                //map base 
                u8 mapblock_bg0 = ((BG0CNT>>8)&0x1f);
                u8 mapblock_bg1 = ((BG1CNT>>8)&0x1f);
                u8 mapblock_bg2 = ((BG2CNT>>8)&0x1f);
                u8 mapblock_bg3 = ((BG3CNT>>8)&0x1f);
                
                int charblock_bg0size = 0;
                int charblock_bg1size = 0;
                int charblock_bg2size = 0;
                int charblock_bg3size = 0;
                
                //define bg0 size
                switch(((BG0CNT>>14)&0x3)){
                    case(0):{
                        charblock_bg0size=1024*2;
                    }
                    break;
                    case(1):{
                        charblock_bg0size=1024*4;
                    }
                    break;
                    case(2):{
                        charblock_bg0size=1024*4;
                    }
                    break;
                    case(3):{
                        charblock_bg0size=1024*8;
                    }
                    break;
                }
                
                switch(((BG1CNT>>14)&0x3)){
                    case(0):{
                        charblock_bg1size=1024*2;
                    }
                    break;
                    case(1):{
                        charblock_bg1size=1024*4;
                    }
                    break;
                    case(2):{
                        charblock_bg1size=1024*4;
                    }
                    break;
                    case(3):{
                        charblock_bg1size=1024*8;
                    }
                    break;
                }
                
                switch(((BG2CNT>>14)&0x3)){
                    case(0):{
                        charblock_bg2size=256;
                    }
                    break;
                    case(1):{
                        charblock_bg2size=1024*1;
                    }
                    break;
                    case(2):{
                        charblock_bg2size=1024*4;
                    }
                    break;
                    case(3):{
                        charblock_bg2size=1024*16;
                    }
                    break;
                }
                
                switch(((BG3CNT>>14)&0x3)){
                    case(0):{
                        charblock_bg3size=1024*2;
                    }
                    break;
                    case(1):{
                        charblock_bg3size=1024*4;
                    }
                    break;
                    case(2):{
                        charblock_bg3size=1024*4;
                    }
                    break;
                    case(3):{
                        charblock_bg3size=1024*8;
                    }
                    break;
                }
                
                */
                //char base block
                //backgrounds: detect all of them from GBA regs and point them to NDS engine backgrounds.
                //dmaCopyWords(3,(void*)((u32)vram + (charblock_bg0*16384)),(void*)bgGetMapBase(bg_0_setting),charblock_bg0size);
                //dmaCopyWords(3,(void*)((u32)vram + (charblock_bg1*16384)),(void*)bgGetMapBase(bg_1_setting),charblock_bg1size);
                //dmaCopyWords(3,(void*)((u32)vram + (charblock_bg2*16384)),(void*)bgGetMapBase(bg_2_setting),charblock_bg2size);
                //dmaCopyWords(3,(void*)((u32)vram + (charblock_bg3*16384)),(void*)bgGetMapBase(bg_3_setting),charblock_bg3size);
                
                //get base map for current bg2 (gba) engine and copy to nds engine 3 
                //dmaCopyWords(3,(void*)((u32)vram + (mapblock_bg2*0x800)),(void*)bgGetMapPtr(bg_2_setting),charblock_bg2size);
                
            }
            break;
            
            case(2):{
            }
            break;
            
            case(3):{
            }
            break;
            
        }
    
    }
	else
	{
		framewtf++;
	}
    
    #endif
    
    //swap window after render
    if(DISPCNT & 0x2000){ //window 0 is done
        //REG_DISPCNT = cur_REG_DISPCNT | ~BIT(13);
        //REG_DISPCNT = cur_REG_DISPCNT | BIT(14);
        DISPCNT &= ~BIT(13);
        DISPCNT |= BIT(14);
    }
    
    else{	//window 1 is done
        //REG_DISPCNT = cur_REG_DISPCNT | ~BIT(14);
        //REG_DISPCNT = cur_REG_DISPCNT | BIT(13);
        DISPCNT &= ~BIT(14);
        DISPCNT |= BIT(13);
    }
    
    
    P1 = REG_KEYINPUT&0x3ff;
    
	if(!(P1 & KEY_A) && !(P1 & KEY_B) && !(P1 & KEY_UP))
	{
		if(ignorenextY == 0)
		{
			pausemenue();
			ignorenextY = 60; // 1 sec break time
		}
		else {ignorenextY -= 1;}
	}            
    
    //touchpad: any touchscreen means
    if( (GBAEMU4DS_IPC->touchXpx > 0) || (GBAEMU4DS_IPC->touchYpx > 0) ){
        //GBA KEYPRESS: A
        P1 &= ~(KEY_A); 
    }
    
    UPDATE_REG(0x130, P1);
    
}

//coto: added touchscreen support
u32 arm7_held_buttons = 0;

__attribute__((section(".itcm")))
void wifi_gba_thread(){
    
}

__attribute__((section(".itcm")))
__attribute__ ((aligned (4)))
void vcounthandler(void){
		
		VCOUNT = REG_VCOUNT&0xff;
		DISPSTAT |= (REG_DISPSTAT & 0x3);
		
		if(VCOUNT == (DISPSTAT >> 8)) //update by V-Count Setting
		{
			DISPSTAT |= 0x4;
            
            wifi_gba_thread();
		}
		else{
			DISPSTAT &= 0xFFFb; //remove vcount
		}
		UPDATE_REG(0x06, VCOUNT);
		UPDATE_REG(0x04, DISPSTAT);
        
        /* //mode 1 causes keypad to get stuck so moved to vblank
        //joypad
        P1 = (REG_KEYINPUT&0x3ff);
        
        //touchpad:
        if(!(REG_KEYXY & BIT(6))){
            
            //GBA KEYPRESS: A
            P1 &= ~(KEY_A); 
            
        }
        UPDATE_REG(0x130, P1);
        */
        
}


//pause menu
char* seloptions [5] = {(char*)"save save",(char*)"show mem",(char*)"Continue",(char*)"DEBUG",(char*)"SEND GET REQUEST TO HOST!"};

__attribute__ ((aligned (4)))
void pausemenue()
{
	REG_IE = 0; //no irq
	u16 tempvcount = REG_VCOUNT;
	TIMER0_CR = TIMER0_CR & ~TIMER_ENABLE; //timer off
	TIMER1_CR = TIMER1_CR & ~TIMER_ENABLE;
	TIMER2_CR = TIMER2_CR & ~TIMER_ENABLE;
	TIMER3_CR = TIMER3_CR & ~TIMER_ENABLE;
	//irqDisable(IRQ_VBLANK);
	//cpupausemode(); //don't need that
	int pressed;
	int ausgewauhlt = 2;
	while(1)
	{
		int itemcount=5; 
		iprintf("\x1b[2J");
		iprintf("Pause\n");
		iprintf ("--------------------------------");
		for(int i = 0; i < itemcount; i++)
		{
			if(i == ausgewauhlt) iprintf("->");
			else iprintf("  ");
			iprintf(seloptions[i]);
			iprintf("\n");
		}
		do {
			if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
			while(!(REG_DISPSTAT & DISP_IN_VBLANK));
            
            scanKeys();
            pressed = (keysDownRepeat()& ~0xFC00);
                
		} while (!pressed); //no communication here with arm7 so no more update
		//iprintf("%x",ausgewauhlt);
		if (pressed&KEY_A)
		{
			switch(ausgewauhlt)
				{
				case 0:{
					if(savePath[0] == 0)sprintf(savePath,"%s.sav",szFile);
					CPUWriteBatteryFile(savePath);
                }
                break;
				case 1:{
                    show_mem();
				}
                break;
				case 2:{
					iprintf("\x1b[2J");
					while(REG_VCOUNT != tempvcount); //wait for VCOUNT
					TIMER0_CR = GBAEMU4DS_IPC->timer0Value; //timer on
					TIMER1_CR = GBAEMU4DS_IPC->timer1Value;
					TIMER2_CR = GBAEMU4DS_IPC->timer2Value;
					TIMER3_CR = GBAEMU4DS_IPC->timer3Value;
					REG_IE = GBAEMU4DS_IPC->IE | IRQ_FIFO_NOT_EMPTY| IRQ_HBLANK; //irq on
					while(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY))
						//u32 src = REG_IPC_FIFO_RX; //get sync irqs back
					return ; //and return
				}
                break;
				//DEBUG!
				case 3:
                {
                    while(REG_VCOUNT != tempvcount); //wait for VCOUNT
					TIMER0_CR = GBAEMU4DS_IPC->timer0Value; //timer on
					TIMER1_CR = GBAEMU4DS_IPC->timer1Value;
					TIMER2_CR = GBAEMU4DS_IPC->timer2Value;
					TIMER3_CR = GBAEMU4DS_IPC->timer3Value;
					REG_IE = GBAEMU4DS_IPC->IE | IRQ_FIFO_NOT_EMPTY| IRQ_HBLANK; //irq on
					
					//pu_Disable()
					
                    SendArm7Command(0xc0700105,0x0,0x0,0x0);
					
					//pu_Enable();
				
                    while(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY))
						//u32 src = REG_IPC_FIFO_RX; //get sync irqs back
					return ; //and return
				}	
				break;
				case 4:{
                    
                    iprintf("test %x",rand()&0xff);
                    iprintf("test %x",rand()&0xff);
					iprintf("test %x",rand()&0xff);
					
                    //asm("nop");
                    const char * str_params = "GET /dswifi/example1.php HTTP/1.1\r\nHost: www.akkit.org\r\nUser-Agent: Nintendo DS\r\n\r\n";
                    send_response((char*)str_params);
                    //asm("nop");
                    
                    /*
                    if( ! (mConnection->outstanding())  && (client_http_handler_context.wifi_enabled == true) ){
                    
    
                    const char* params = "answer=42&foo=bar";
                    int l = strlen(params);
                    mConnection->putrequest( "POST", server_ip );
                    mConnection->putheader( "Connection", "close" );
                    mConnection->putheader( "Content-Length", l );
                    mConnection->putheader( "Content-type", "application/x-www-form-urlencoded" );
                    mConnection->putheader( "Accept", "text/plain" );
                    mConnection->endheaders();
                    mConnection->send( (const unsigned char*)params, l );
            
                    
                    
                    //should be called automatically
                    //while( conn.outstanding() )
                    //    conn.pump();
                    
                    }
                    */
                    
                    /*
					rom = 0x00000000;
					
					flashInit();
					eepromInit();
					
					//init begin
					//add loader code here
					//runNdsFile (const char* filename, int argc, const char** argv);
					runNdsFile ((const char*)"fat:/hbmenu.nds", 0, 0);
					//init end
					*/
                    break;
                    }
				}
		}
		if (pressed&KEY_DOWN && ausgewauhlt != itemcount){ ausgewauhlt++;}
		if (pressed&KEY_UP && ausgewauhlt != 0) {ausgewauhlt--;}
	}


}
