// VisualBoyAdvance - Nintendo Gameboy/GameboyAdvance (TM) emulator.
// Copyright (C) 1999-2003 Forgotten
// Copyright (C) 2004-2006 Forgotten and the VBA development team

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2, or(at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#include <nds.h>
#include <string.h>
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
#include <assert.h>

#include "interrupts/fifo_handler.h"
#include "../../common/cpuglobal.h"
#include "../../common/gba_ipc.h"

#include <filesystem.h>
#include "GBA.h"
#include "Sound.h"
#include "Util.h"
#include "getopt.h"
#include <fat.h>
#include <dirent.h>
#include "cpumg.h"
#include "bios.h"
#include "mydebuger.h"
#include "file_browse.h"
#include "main.h"
#include "Sound.h"
#include <unistd.h>    // for sbrk()
#include <fat.h>
#include "ichflysettings.h"
#include "NLS.h"
#include "Util.h"
#include "Flash.h"
#include "GBA.h"
#include "Globals.h"
#include "RTC.h"
#include "fatfile.h"
#include "dswnifi_lib.h"
#include "disk_fs/ichflyfs.h"

#ifndef _MSC_VER
#define _stricmp strcasecmp
#endif // ! _MSC_VER

/*
static int (ZEXPORT *utilGzWriteFunc)(gzFile, const voidp, unsigned int) = NULL;
static int (ZEXPORT *utilGzReadFunc)(gzFile, voidp, unsigned int) = NULL;
static int (ZEXPORT *utilGzCloseFunc)(gzFile) = NULL;

bool utilWritePNGFile(const char *fileName, int w, int h, u8 *pix)
{
  u8 writeBuffer[512 * 3];
  
  FILE *fp = fopen(fileName,"wb");

  if(!fp) {
    systemMessage(MSG_ERROR_CREATING_FILE, N_("Error creating file %s"), fileName);
    return false;
  }
  
  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                                NULL,
                                                NULL,
                                                NULL);
  if(!png_ptr) {
    fclose(fp);
    return false;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);

  if(!info_ptr) {
    png_destroy_write_struct(&png_ptr,NULL);
    fclose(fp);
    return false;
  }

  if(setjmp(png_ptr->jmpbuf)) {
    png_destroy_write_struct(&png_ptr,NULL);
    fclose(fp);
    return false;
  }

  png_init_io(png_ptr,fp);

  png_set_IHDR(png_ptr,
               info_ptr,
               w,
               h,
               8,
               PNG_COLOR_TYPE_RGB,
               PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_DEFAULT,
               PNG_FILTER_TYPE_DEFAULT);

  png_write_info(png_ptr,info_ptr);

  u8 *b = writeBuffer;

  int sizeX = w;
  int sizeY = h;

  switch(systemColorDepth) {
  case 16:
    {
      u16 *p = (u16 *)(pix+(w+2)*2); // skip first black line
      for(int y = 0; y < sizeY; y++) {
         for(int x = 0; x < sizeX; x++) {
          u16 v = *p++;
          
          *b++ = ((v >> systemRedShift) & 0x001f) << 3; // R
          *b++ = ((v >> systemGreenShift) & 0x001f) << 3; // G 
          *b++ = ((v >> systemBlueShift) & 0x01f) << 3; // B
        }
        p++; // skip black pixel for filters
        p++; // skip black pixel for filters
        png_write_row(png_ptr,writeBuffer);
        
        b = writeBuffer;
      }
    }
    break;
  case 24:
    {
      u8 *pixU8 = (u8 *)pix;
      for(int y = 0; y < sizeY; y++) {
        for(int x = 0; x < sizeX; x++) {
          if(systemRedShift < systemBlueShift) {
            *b++ = *pixU8++; // R
            *b++ = *pixU8++; // G
            *b++ = *pixU8++; // B
          } else {
            int blue = *pixU8++;
            int green = *pixU8++;
            int red = *pixU8++;
            
            *b++ = red;
            *b++ = green;
            *b++ = blue;
          }
        }
        png_write_row(png_ptr,writeBuffer);
        
        b = writeBuffer;
      }
    }
    break;
  case 32:
    {
      u32 *pixU32 = (u32 *)(pix+4*(w+1));
      for(int y = 0; y < sizeY; y++) {
        for(int x = 0; x < sizeX; x++) {
          u32 v = *pixU32++;
          
          *b++ = ((v >> systemRedShift) & 0x001f) << 3; // R
          *b++ = ((v >> systemGreenShift) & 0x001f) << 3; // G
          *b++ = ((v >> systemBlueShift) & 0x001f) << 3; // B
        }
        pixU32++;
        
        png_write_row(png_ptr,writeBuffer);
        
        b = writeBuffer;
      }
    }
    break;
  }
  
  png_write_end(png_ptr, info_ptr);

  png_destroy_write_struct(&png_ptr, &info_ptr);

  fclose(fp);

  return true;  
}
*/
extern bool cpuIsMultiBoot;


// swaps a 16-bit value
__attribute__((section(".itcm")))
u16 swap16(u16 v)
{
	return (v<<8)|(v>>8);
}

// swaps a 32-bit value
__attribute__((section(".itcm")))
u32 swap32(u32 v)
{
	return (v<<24)|((v<<8)&0xff0000)|((v>>8)&0xff00)|(v>>24);
}


bool utilIsSAV(const char * file)
{
  if(strlen(file) > 4) {
    const char * p = strrchr(file,'.');

    if(p != NULL) {
      if(_stricmp(p, ".sav") == 0) {
        cpuIsMultiBoot = true;
        return true;
      }
    }
  }

  return false;
}



bool utilIsGBAImage(const char * file)
{
  cpuIsMultiBoot = false;
  if(strlen(file) > 4) {
    const char * p = strrchr(file,'.');

    if(p != NULL) {
      if(_stricmp(p, ".gba") == 0)
        return true;
      if(_stricmp(p, ".agb") == 0)
        return true;
      if(_stricmp(p, ".bin") == 0)
        return true;
      /*if(_stricmp(p, ".elf") == 0)
        return true;*/ //todo
      if(_stricmp(p, ".mb") == 0) {
        cpuIsMultiBoot = true;
        return true;
      }
    }
  }

  return false;
}

/*
IMAGE_TYPE utilFindType(const char *file)
{
  char buffer[2048];
  
  if(utilIsZipFile(file)) {
    unzFile unz = unzOpen(file);
    
    if(unz == NULL) {
      systemMessage(MSG_CANNOT_OPEN_FILE, N_("Cannot open file %s"), file);
      return IMAGE_UNKNOWN;
    }
    
    int r = unzGoToFirstFile(unz);
    
    if(r != UNZ_OK) {
      unzClose(unz);
      systemMessage(MSG_BAD_ZIP_FILE, N_("Bad ZIP file %s"), file);
      return IMAGE_UNKNOWN;
    }
    
    IMAGE_TYPE found = IMAGE_UNKNOWN;
    
    unz_file_info info;
    
    while(true) {
      r = unzGetCurrentFileInfo(unz,
                                &info,
                                buffer,
                                sizeof(buffer),
                                NULL,
                                0,
                                NULL,
                                0);
      
      if(r != UNZ_OK) {
        unzClose(unz);
        systemMessage(MSG_BAD_ZIP_FILE, N_("Bad ZIP file %s"), file);
        return IMAGE_UNKNOWN;
      }
      
      if(utilIsGBAImage(buffer)) {
        found = IMAGE_GBA;
        break;
      }

      if(utilIsGBImage(buffer)) {
        found = IMAGE_GB;
        break;
      }
        
      r = unzGoToNextFile(unz);
      
      if(r != UNZ_OK)
        break;
    }
    unzClose(unz);
    
    if(found == IMAGE_UNKNOWN) {
      systemMessage(MSG_NO_IMAGE_ON_ZIP,
                    N_("No image found on ZIP file %s"), file);
      return found;
    }
    return found;
#if 0
  } else if(utilIsRarFile(file)) {
    IMAGE_TYPE found = IMAGE_UNKNOWN;
    
    ArchiveList_struct *rarList = NULL;
    if(urarlib_list((void *)file, (ArchiveList_struct *)&rarList)) {
      ArchiveList_struct *p = rarList;

      while(p) {
        if(utilIsGBAImage(p->item.Name)) {
          found = IMAGE_GBA;
          break;
        }

        if(utilIsGBImage(p->item.Name)) {
          found = IMAGE_GB;
          break;
        }
        p = p->next;
      }
      
      urarlib_freelist(rarList);
    }
    return found;
#endif
  } else {
    if(utilIsGzipFile(file))
      utilGetBaseName(file, buffer);
    else
      strcpy(buffer, file);
    
    if(utilIsGBAImage(buffer))
      return IMAGE_GBA;
    if(utilIsGBImage(buffer))
      return IMAGE_GB;
  }
  return IMAGE_UNKNOWN;  
}
*/
static int utilGetSize(int size)
{
  int res = 1;
  while(res < size)
    res <<= 1;
  return res;
}

__attribute__((section(".itcm")))
void entersu()
{
			DC_FlushAll(); //try it
			IC_InvalidateAll();
			cpu_SetCP15Cnt(cpu_GetCP15Cnt() & ~0x1); //disable pu
			DC_FlushAll(); //try it
			IC_InvalidateAll();
}

__attribute__((section(".itcm")))
void leavesu()
{
			DC_FlushAll(); //try it
			IC_InvalidateAll();
			pu_Enable(); //back to normal code
			DC_FlushAll(); //try it
			IC_InvalidateAll();
}

void patchit(int romSize2)
{
	FILE *patchf = fopen(patchPath, "rb");
	u8 header[0x50];
	fread(header,1,0x50,patchf);
	if(memcmp(header,"flygbapatch",0xC))
	{
		systemMessage(MSG_ERROR_OPENING_IMAGE, N_("Error in patchfile"));
		while(1);
	}
	int patchnum = *(u32*)&header[0x10];
	for(int i = 0;i < patchnum;i++)
	{
		int type;
		fread((void*)&type,1,0x4,patchf);
		switch (type)
		{
		case 0:
			{
			int offsetgba;
			int offsetthisfile;
			fread((void*)&offsetgba,1,0x4,patchf);
			fread((void*)&offsetthisfile,1,0x4,patchf);
			if(offsetgba + chucksize < romSize2)
			{
				int coo = ftell(patchf);
				fseek(patchf,offsetthisfile,SEEK_SET);
				fread(rom + offsetgba,1,chucksize,patchf);
				fseek(patchf,coo,SEEK_SET);
			}
			else
			{
				getandpatchmap(offsetgba,offsetthisfile);
			}
#ifdef debugpatch
			iprintf("patch gbaedit from %08X to %08X (%08X)\n\r",offsetthisfile,offsetgba,ftell(patchf));
#endif
			}
			break;
		case 1:
			{
			//fread((void*)&cheatsNumber,1,0x4,patchf);
			int offset;
			fread((void*)&offset,1,0x4,patchf);
			int coo5 = ftell(patchf);
			fseek(patchf,offset,SEEK_SET);
			//fread((void*)cheatsList,1,cheatsNumber*28,patchf);
			fseek(patchf,coo5,SEEK_SET);
			__irqSet(IRQ_FIFO_NOT_EMPTY,arm7dmareqandcheat,irqTable);
#ifdef debugpatch
			iprintf("patch cheats %08X from %08X (%08X)\n\r",cheatsNumber,cheatsList,ftell(patchf));
#endif
			}
			break;
		case 2:
			{
			u32 gbaoffset;
			fread((void*)&gbaoffset,1,0x4,patchf);
			u32 payloadsize;
			fread((void*)&payloadsize,1,0x4,patchf);
			int offset;
			fread((void*)&offset,1,0x4,patchf);
			int coo = ftell(patchf);
			fseek(patchf,offset,SEEK_SET);
			entersu();
			fread((void*)gbaoffset,1,payloadsize,patchf);
			leavesu();
			fseek(patchf,coo,SEEK_SET);
#ifdef debugpatch
			iprintf("patch direct write to %08X from %08X size %08X (%08X)\n\r",gbaoffset,offset,payloadsize,ftell(patchf));
#endif
			break;
			}
		case 3:
			{
			u8 type;
			fread((void*)&type,1,0x1,patchf);
			u32 offset;
			fread((void*)&offset,1,0x4,patchf);
			int address;
			fread((void*)&address,1,0x4,patchf);
			u32 Condition;
			fread((void*)&Condition,1,0x4,patchf);
			if(offset & BIT(31))offset = (offset & ~BIT(31)) + (u32)rom;
			u32 topatchoffset = address - offset - 8;
			entersu();
			switch (type)
			{
				case 0:
					topatchoffset =+ 4;
					*(u16*)offset = (u16)0xF000 | (u16)((topatchoffset >> 12) & 0x7FF);
					*(u16*)(offset + 2) = (u16)0xF800 | (u16)((topatchoffset >> 1) & 0x7FF);
					break;
				case 1:
					topatchoffset =+ 4;
					*(u16*)offset = (u16)0xF000 + (u16)((topatchoffset >> 12) & 0x7FF);
					*(u16*)(offset + 2) = (u16)0xE800 + (u16)((topatchoffset >> 1) & 0x7FF);
					break;
				case 2:
					*(u32*)offset = (Condition << 0x1B) | 0x0A000000 | ((topatchoffset >> 2) & ~0xFF000000);
					break;
				case 3:
					*(u32*)offset = (Condition << 0x1B) | 0x0B000000 | ((topatchoffset >> 2) & ~0xFF000000);
					break;
			}
			leavesu();
#ifdef debugpatch
			iprintf("link to sa type %08X where %08X dest %08X Condition %08X (%08X)\n\r",type,offset,address,Condition,ftell(patchf));
#endif
			}
			break;
			case 4:
			{
			u8 type;
			fread((void*)&type,1,0x1,patchf);
			u32 offset;
			fread((void*)&offset,1,0x4,patchf);
			int function;
			int address;
			fread((void*)&function,1,0x4,patchf);
			switch (function)
			{
				case 0:
					address = (u32)CPUReadMemorypu;
					break;
				case 1:
					address = (u32)CPUReadHalfWordpu;
					break;
				case 2:
					address = (u32)CPUReadBytepu;
					break;
				case 3:
					address = (u32)CPUWriteMemorypuextern;
					break;
				case 4:
					address = (u32)CPUWriteHalfWordpuextern;
					break;
				case 5:
					address = (u32)CPUWriteBytepuextern;
					break;





				case 6:
					address = (u32)CPUReadMemory;
					break;
				case 7:
					address = (u32)CPUReadHalfWord;
					break;
				case 8:
					address = (u32)CPUReadByte;
					break;
				case 9:
					address = (u32)CPUWriteMemoryextern;
					break;
				case 10:
					address = (u32)CPUWriteHalfWordextern;
					break;
				case 11:
					address = (u32)CPUWriteByteextern;
					break;
				case 12:
					address = (u32)CPUReadHalfWordSignedoutline;
					break;
				case 13:
					address = (u32)CPUReadByteSigned;
					break;

				case 14:
					address = (u32)CPUReadHalfWordrealpuSignedoutline;
					break;
				case 15:
					address = (u32)CPUReadByteSignedpu;
					break;


				case 100:
					address = (u32)ichfly_readu32extern;
					break;
				case 101:
					address = (u32)ichfly_readu16extern;
					break;
				case 102:
					address = (u32)ichfly_readu8extern;
					break;
			}
			u32 Condition;
			fread((void*)&Condition,1,0x4,patchf);
			if(offset & BIT(31))offset = (offset & ~BIT(31)) + (u32)rom;
			u32 topatchoffset = address - offset - 8;
			entersu();
			switch (type)
			{
				case 0:
					topatchoffset =+ 4;
					*(u16*)offset = (u16)0xF000 | (u16)((topatchoffset >> 12) & 0x7FF);
					*(u16*)(offset + 2) = (u16)0xF800 | (u16)((topatchoffset >> 1) & 0x7FF);
					break;
				case 1:
					topatchoffset =+ 4;
					*(u16*)offset = (u16)0xF000 + (u16)((topatchoffset >> 12) & 0x7FF);
					*(u16*)(offset + 2) = (u16)0xE800 + (u16)((topatchoffset >> 1) & 0x7FF);
					break;
				case 2:
					*(u32*)offset = (Condition << 0x1C) | 0x0A000000 | ((topatchoffset >> 2) & ~0xFF000000);
					break;
				case 3:
					*(u32*)offset = (Condition << 0x1C) | 0x0B000000 | ((topatchoffset >> 2) & ~0xFF000000);
					break;
			}
			leavesu();
#ifdef debugpatch
			iprintf("link to sf type %08X where %08X function %08X Condition %08X (%08X)\n\r",type,offset,function,Condition,ftell(patchf));
#endif
			}
			break;
			case 5:
			{	
			#ifdef usebuffedVcout
				extern u8 VCountgbatods[0x100]; //(LY)      (0..227) + check overflow
				extern u8 VCountdstogba[263]; //(LY)      (0..262)
				extern u8 VCountdoit[263]; //jump in or out
				u32 offsetthisfile;
				fread((void*)&offsetthisfile,1,0x4,patchf);

				int coo = ftell(patchf);
				fseek(patchf,offsetthisfile,SEEK_SET);
				fread(VCountgbatods,1,0x100,patchf);
				fread(VCountdstogba,1,263,patchf);
				fread(VCountdoit,1,263,patchf);
				fseek(patchf,coo,SEEK_SET);
			#endif
			}
			break;
		}
	}
	char patchmsg[0x100];
	if(fread(patchmsg,1,0x100,patchf) > 0)
	{
		iprintf(patchmsg);
	}
#ifdef debugpatch
		iprintf("end (%X)",patchnum);
#endif
		fclose(patchf);
}


//VA + ( DTCMTOP - (stack_size)*4) - dtcm_reservedcode[end_of_usedDTCMstorage] (we use for the emu)  in a loop of CACHE_LINE size

// returns unary(decimal) ammount of bits using the Hamming Weight approach 

//8 bit depth Lookuptable 
//	0	1	2	3	4	5	6	7	8	9	a	b	c	d	e	f
const u8 minilut[0x10] = {
	0,	1,	1,	2,	1,	2,	2,	3,	1,	2,	2,	3,	2,	3,	3,	4,		//0n
};

u8 lutu16bitcnt(u16 x){
	return (minilut[x &0xf] + minilut[(x>>4) &0xf] + minilut[(x>>8) &0xf] + minilut[(x>>12) &0xf]);
}

u8 lutu32bitcnt(u32 x){
	return (lutu16bitcnt(x & 0xffff) + lutu16bitcnt(x >> 16));
}


//counts leading zeroes :)
u8 clzero(u32 var){
   
    u8 cnt=0;
    u32 var3;
    if (var>0xffffffff) return 0;
   
    var3=var; //copy
    var=0xFFFFFFFF-var;
    while((var>>cnt)&1){
        cnt++;
    }
    if ( (((var3&0xf0000000)>>28) >0x7) && (((var3&0xff000000)>>24)<0xf)){
        var=((var3&0xf0000000)>>28);
        var-=8; //bit 31 can't count to zero up to this point
            while(var&1) {
                cnt++; var=var>>1;
            }
    }
return cnt;
}


char* strtoupper(char* s) {
  assert(s != NULL);

  char* p = s;
  while (*p != '\0') {
    *p = toupper(*p);
    p++;
  }

  return s;
}

char* strtolower(char* s) {
  assert(s != NULL);

  char* p = s;
  while (*p != '\0') {
    *p = tolower(*p);
    p++;
  }

  return s;
}



bool pendingSaveFix = false;	//false if already saved new forked save / or game doesn't meet savefix conditions // true if pending a save that was fixed in gba core, but still has not been written/updated to file.
int  SaveSizeBeforeFix = 0;	//only valid if pendingSaveFix == true
int  SaveSizeAfterFix = 0;	//only valid if pendingSaveFix == true


bool save_deciderByTitle(char * headerTitleSource, char * headerTitleHaystack, int SizeToCheck){
	if (
			strncmp( 
			strtoupper(headerTitleSource), 
			strtoupper(headerTitleHaystack),
			SizeToCheck
			) == 0
		){
		return true;
	}
	return false;
}

//coto: if a game is defined here savetype from gamecode will be used
int save_decider(){

	//void * memcpy ( void * destination, const void * source, size_t num );
	int savetype=0;
	char gamecode[6] = {0};
	char title[13] = {0};
	title[12] = '\0';
	
	memcpy((char*)gamecode,(u8*)&GetsIPCSharedGBA()->gbaheader.gamecode,sizeof(GetsIPCSharedGBA()->gbaheader.gamecode));
	memcpy((char*)title,(u8*)&GetsIPCSharedGBA()->gbaheader.title,sizeof(GetsIPCSharedGBA()->gbaheader.title));
	
	//iprintf("GameCode is: %s \n",gamecode);
	//iprintf("GameCode is: %s \n",strtoupper(gamecode));
	//iprintf("GameCode is: %s \n",strtolower(gamecode));
	//while(1);

    if( strncmp( 
        strtoupper((char*)gamecode), 
        strtoupper((char*)"bpre01"), //firered 128K
        sizeof(GetsIPCSharedGBA()->gbaheader.gamecode)
        ) == 0 
        )
    {
        
        savetype = 3; 
    }
    
	else if( strncmp( 
        strtoupper((char*)gamecode), 
        strtoupper((char*)"bpge01"), //greenleaf 128K
        sizeof(GetsIPCSharedGBA()->gbaheader.gamecode)
        ) == 0 
        )
    {
        
        savetype = 3; 
    }
    
    else if( strncmp( 
        strtoupper((char*)gamecode), 
        strtoupper((char*)"amze01"), //mario 1 eeprom 64K
        sizeof(GetsIPCSharedGBA()->gbaheader.gamecode)
        ) == 0 
        )
    {
        
        savetype = 1; 
    }
    
    else if( strncmp( 
        strtoupper((char*)gamecode), 
        strtoupper((char*)"ax4p01"), //mario 3 128K
        sizeof(GetsIPCSharedGBA()->gbaheader.gamecode)
        ) == 0 
        )
    {
        
        savetype = 3; 
    }
	
	//all pokemon sapphire / pokemon ruby cases are also 128K Flash
	
	else if(
		(save_deciderByTitle(title, (char*)"POKEMON SAPP",sizeof(GetsIPCSharedGBA()->gbaheader.title)) == true)
		||
		(save_deciderByTitle(title, (char*)"POKEMON RUBY",sizeof(GetsIPCSharedGBA()->gbaheader.title)) == true)
	)
    {
        savetype = 3; 
    }
	

    int myflashsize = 0x10000;

    //Flash setup: 0 auto / 1 eeprom / 2 sram / 3 flashrom /4 eeprom + sensor / 5 none
    if(savetype == 3)
    {
        myflashsize = 0x20000;
        cpuSaveType = 3;
    }
    else if(savetype == 1)
    {
        myflashsize = 0x10000;
        cpuSaveType = 1; 
    }
    else
        cpuSaveType = savetype;

    //Flash format
    if(cpuSaveType == 3){
        flashSetSize(myflashsize);
        iprintf("[FLASH %d bytes]\n",myflashsize);
    }

return savetype;
}

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

uint32 getVRAMHeapStart(){
	return (uint32)0x06820000;
}

//vram linear memory allocator 
sint32 vramABlockOfst	=	0;	//offset pointer to free memory, user alloced memory is (baseAddr + (sizeAlloced - vramBlockPtr))
sint32 vramBBlockOfst	=	0;
sint32 vramCBlockOfst	=	0;
sint32 vramDBlockOfst	=	0;

sint32 HeapBlockOfst	=	0;

//if ret ptr == NULL, invalid operation  not enough space
uint32 * vramHeapAlloc(uint32 vramBlock,uint32 StartAddr,int size){
	uint32 * BlockAssign = NULL;
	bool isVram = false;
	switch(vramBlock){
		case(vramBlockA):{
			BlockAssign = (uint32 *)&vramABlockOfst;
			isVram = true;
		}
		break;
		case(vramBlockB):{
			BlockAssign = (uint32 *)&vramBBlockOfst;
			isVram = true;
		}
		break;
		case(vramBlockC):{
			BlockAssign = (uint32 *)&vramCBlockOfst;
			isVram = true;
		}
		break;
		case(vramBlockD):{
			BlockAssign = (uint32 *)&vramDBlockOfst;
			isVram = true;
		}
		break;
		case(HeapBlock):{
			BlockAssign = (uint32 *)&HeapBlockOfst;
		}
		break;
	}
	if(BlockAssign == NULL){
		return NULL;
	}
	sint32 heapDetected = (isVram == true) ? vramSize : HeapSize;
	if((StartAddr + (int)*BlockAssign + size) <= (StartAddr+heapDetected)){
		//memset((uint8*)(StartAddr + (int)*BlockAssign) , 0, size);
		*BlockAssign = (uint32)((int)*BlockAssign + size);
	}
	else{
		return NULL;
	}
	uint32 AllocBuf = (StartAddr + ((int)*BlockAssign - size));
	if(AllocBuf < StartAddr){
		AllocBuf = StartAddr;
	}
	return (uint32*)AllocBuf;
}

//if ret ptr == NULL, invalid operation  not enough space
uint32 * vramHeapFree(uint32 vramBlock,uint32 StartAddr,int size){
	uint32 * BlockAssign = NULL;
	bool isVram = false;
	switch(vramBlock){
		case(vramBlockA):{
			BlockAssign = (uint32 *)&vramABlockOfst;
			isVram = true;
		}
		break;
		case(vramBlockB):{
			BlockAssign = (uint32 *)&vramBBlockOfst;
			isVram = true;
		}
		break;
		case(vramBlockC):{
			BlockAssign = (uint32 *)&vramCBlockOfst;
			isVram = true;
		}
		break;
		case(vramBlockD):{
			BlockAssign = (uint32 *)&vramDBlockOfst;
			isVram = true;
		}
		break;
		case(HeapBlock):{
			BlockAssign = (uint32 *)&HeapBlockOfst;
		}
		break;
	}
	if(BlockAssign == NULL){
		return NULL;
	}
	if(((StartAddr + (int)*BlockAssign) - size) >= (StartAddr)){
		*BlockAssign = (uint32)((int)*BlockAssign - size);
	}
	else{
		return NULL;
	}
	return (uint32*)(StartAddr + ((int)*BlockAssign));
}

//vram alloc/free testcases

/*
int _tmain(int argc, _TCHAR* argv[])
{
	uint32 startLinearVramAddr = 0x06000000;
	uint32 startLinearHeapAddr = 0x02040100;	//fake ewram address

	sint32 size = 1024 * 32;
	printf("vram-alloc%x:%x \n",startLinearVramAddr,(uint16 *)vramHeapAlloc(vramBlockD,startLinearVramAddr,size));	//0x06000000
	printf("vram-alloc%x:%x \n",startLinearVramAddr,(uint16 *)vramHeapAlloc(vramBlockD,startLinearVramAddr,size));	//0x06008000
	printf("vram-alloc%x:%x \n",startLinearVramAddr,(uint16 *)vramHeapAlloc(vramBlockD,startLinearVramAddr,size));	//0x06010000
	printf("vram-alloc%x:%x \n",startLinearVramAddr,(uint16 *)vramHeapAlloc(vramBlockD,startLinearVramAddr,size));	//0x06018000

	printf("heap-alloc%x:%x \n",startLinearHeapAddr,(uint16 *)vramHeapAlloc(HeapBlock,startLinearHeapAddr,size));	//0x02040100
	printf("heap-alloc%x:%x \n",startLinearHeapAddr,(uint16 *)vramHeapAlloc(HeapBlock,startLinearHeapAddr,size));	//0x02048100
	printf("heap-alloc%x:%x \n",startLinearHeapAddr,(uint16 *)vramHeapAlloc(HeapBlock,startLinearHeapAddr,size));	//0x02050100
	printf("heap-alloc%x:%x \n",startLinearHeapAddr,(uint16 *)vramHeapAlloc(HeapBlock,startLinearHeapAddr,size));	//0x02058100
	
	printf("heap-alloc%x:%x \n",startLinearHeapAddr,(uint16 *)vramHeapAlloc(HeapBlock,startLinearHeapAddr,size));	//0 (invalid)
	
	printf("vram-free%x:%x \n",startLinearVramAddr,(uint16 *)vramHeapFree(vramBlockD,startLinearVramAddr,size));	//0x06018000
	printf("heap-free%x:%x \n",startLinearHeapAddr,(uint16 *)vramHeapFree(HeapBlock,startLinearHeapAddr,size));		//0x02058100
	
	printf("vram-free%x:%x \n",startLinearVramAddr,(uint16 *)vramHeapFree(vramBlockD,startLinearVramAddr,size));	//0x06010000
	printf("heap-free%x:%x \n",startLinearHeapAddr,(uint16 *)vramHeapFree(HeapBlock,startLinearHeapAddr,size));		//0x02050100
	
	printf("vram-free%x:%x \n",startLinearVramAddr,(uint16 *)vramHeapFree(vramBlockD,startLinearVramAddr,size));	//0x06008000
	printf("heap-free%x:%x \n",startLinearHeapAddr,(uint16 *)vramHeapFree(HeapBlock,startLinearHeapAddr,size));		//0x02048100
	
	printf("vram-free%x:%x \n",startLinearVramAddr,(uint16 *)vramHeapFree(vramBlockD,startLinearVramAddr,size));	//0x06000000
	printf("heap-free%x:%x \n",startLinearHeapAddr,(uint16 *)vramHeapFree(HeapBlock,startLinearHeapAddr,size));		//0x02040100
	
	printf("vram-free%x:%x \n",startLinearVramAddr,(uint16 *)vramHeapFree(vramBlockD,startLinearVramAddr,size));	//0 (invalid)
	printf("heap-free%x:%x \n",startLinearHeapAddr,(uint16 *)vramHeapFree(HeapBlock,startLinearHeapAddr,size));		//0 (invalid)
	
	while(1==1){}
	return 0;
}
*/

//sets up the environment / supports being called multiple times
__attribute__((section(".itcm")))
bool reloadGBA(char * filename, u32 manual_save_type){
	REG_IME = IME_DISABLE;
	
	int syncline = 159;
	arm9VCOUNTsyncline = syncline;
	while(REG_VCOUNT!=syncline){
	}
	SendArm7Command(set_callline,(u32)arm9VCOUNTsyncline);	//cmd
	SendArm7Command(enableWaitforVblancarmcmdirq,0);
	//sound data buff
	#ifdef arm9advsound
	arm7amr9buffer = (u32)arm7exchangefild;
	SendArm7Command((u32)setdmasoundbuff,arm7amr9buffer);	//load buffer for arm7

	#ifdef anyarmcom
	*(u32*)arm7exchangefild = (u32)&amr7sendcom;
	*(u32*)(arm7exchangefild + 4) = (u32)&amr7senddma1;
	*(u32*)(arm7exchangefild + 8) = (u32)&amr7senddma2;
	*(u32*)(arm7exchangefild + 12) = (u32)&amr7recmuell;
	*(u32*)(arm7exchangefild + 16) = (u32)&amr7directrec;
	*(u32*)(arm7exchangefild + 20) = (u32)&amr7indirectrec;
	*(u32*)(arm7exchangefild + 24) = (u32)&amr7fehlerfeld[0];
	#endif
	#endif
	
	
	DISPCNT  = 0x0080;
	if(bios){
		free(bios);
	}
	int sectortableSize16Morless = 262160;
	if((sectortabel) && (latestsectortableSize > sectortableSize16Morless)){
		free(sectortabel);
	}
	if(gbafsbuffer){
		free(gbafsbuffer);
	}
	int myflashsize = 0x10000;
	bool failed = !CPULoadRom(szFile);
	if(failed){
		printf("failed");
		while(1);
	}
	iprintf("OK\n");
	
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
	
	useBios = false;
	iprintf("CPUInit\n");
	CPUInit(biosPath, useBios);
	iprintf("CPUReset\n");
	CPUReset();
	int filepathlen = strlen(szFile);
	char  fn_noext[filepathlen] = {0};
	memcpy(fn_noext,szFile,filepathlen-3);

	//detect savefile (filename.sav)
	sprintf(fn_noext,"%ssav",fn_noext);
	FILE * frh = fopen(fn_noext,"r");

	//coto: added create new savefile
	if(!frh){
		iprintf("no savefile found, creating new one... \n");
		savePath[0] = 0;
		strcpy ((char *)savePath, (const char *)fn_noext);
		CPUWriteBatteryFile(savePath);
	}
	else{
		strcpy ((char *)savePath, (const char *)fn_noext);
		if(CPUReadBatteryFile(savePath))
		{
			if(cpuSaveType == 0)iprintf("SaveReadOK![AUTO]\n");
			if(cpuSaveType == 1)iprintf("SaveReadOK![EEPROM]\n");
			if(cpuSaveType == 2)iprintf("SaveReadOK![SRAM]\n");
			if(cpuSaveType == 3)iprintf("SaveReadOK![FLASHROM]\n");
			if(cpuSaveType == 4)iprintf("SaveReadOK![EEPROM+SENSOR]\n");
			if(cpuSaveType == 5)iprintf("SaveReadOK![NONE]\n");			
		}
		else
		{
			iprintf("failed reading: %s\n",savePath);
			while(1);
		}
		fclose(frh);
	}
	
	ndsMode();
	BIOS_RegisterRamReset(0xFF);
	REG_IME = IME_ENABLE;
	
	while(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)){	//rudimentary spinlock
		u32 src = REG_IPC_FIFO_RX;
	}
	
	anytimejmpfilter = 0;
	setGBAVectors();
	
	#ifdef capture_and_pars
	videoBgDisableSub(0);
	vramSetBankH(VRAM_H_LCD); //only sub
	vramSetBankI(VRAM_I_LCD); //only sub
	int iback = bgInitSub(3, BgType_ExRotation, BgSize_B16_256x256, 0,0);

	bgSetRotateScale(iback,0,0x0F0,0x0D6);
	bgUpdate();
	#endif
	SendArm7Command(GBAEMU4DS_SND_START,0x0);
	pu_Enable();
	gbaInit(false);
	gbaMode();
	REG_IF = IRQ_HBLANK;
	cpu_ArmJumpforstackinit((u32)rom, 0);
}

char * bufNames[entriesPerList][512];


//return value : lcdSwapS == true, must swap LCD, otherwise false
__attribute__((section(".itcm")))
bool ShowBrowser(){
	int pressed = 0;
	bool lcdSwapS = false;
	iprintf("gbaemu DS by ichfly\n");
	iprintf("press B for lcdswap A for normal\n");
	while(1) 
	{
		scanKeys();
		int isdaas = keysHeld();
		if (isdaas&KEY_A)
		{
			lcdSwapS = false;
			break;
		}
		if(isdaas&KEY_B)
		{
			lcdSwapS = true;
			break;
		}
	}
	
	struct dirent *de;  // Pointer for directory entry
	char cwPath[512] = {0};
	
	sprintf(cwPath,"%s","/gba");
	
	// opendir() returns a pointer of DIR type. 
    DIR *dr = opendir(cwPath);
	
    if (dr == NULL){  // opendir returns NULL if couldn't open directory
        iprintf("Could not open current directory. check README.md " );
        while(1==1);
    }
	int j = 0, k =0;
	
    while ((de = readdir(dr)) != NULL){
		if(j < entriesPerList){
			if( utilIsGBAImage((const char *)de->d_name) == true){
				sprintf((char*)&bufNames[j],"%s",de->d_name);
				j++;
			}
		}
	}
    closedir(dr);    
	
	//actual file lister
	iprintf("\x1b[2J");
	while(k < j ){
		iprintf("--- %s",(char*)&bufNames[k]);
		iprintf("\n");
		k++;
	}
	
	pressed = 0 ;
	k = 0;
	int lastVal = 0;
	while(1)
	{
		scanKeys();
		pressed = keysHeld();
		if (pressed&KEY_DOWN && k < (j - 1) ){
			k++;
			while(pressed&KEY_DOWN){
				scanKeys();
				pressed = keysHeld();
			}
		}
		if (pressed&KEY_UP && k != 0) {
			k--;
			while(pressed&KEY_UP){
				scanKeys();
				pressed = keysHeld();
			}
		}
		if(pressed&KEY_START){
			break;
		}
		
		// Show cursor
		iprintf ("\x1b[%d;0H*", k);
		
		if(lastVal != k){
			iprintf ("\x1b[%d;0H ", lastVal);	//clean old
		}
		
		while(!(pressed&KEY_DOWN) && !(pressed&KEY_UP) && !(pressed&KEY_START)){
			scanKeys();
			pressed = keysHeld();
		}
		
		lastVal = k;
	}
	iprintf("\x1b[2J");
	sprintf(szFile,"%s%s/%s","fat:",cwPath,(char*)&bufNames[k]);
	return lcdSwapS;
}

//little-endian GBA == little-endian NDS

__attribute__((section(".itcm")))
u16 READ16LE(u16 * x){
  return *((u16 *)x);
}

__attribute__((section(".itcm")))
u32 READ32LE(u32 * x){
  return *((u32 *)x);
}

__attribute__((section(".itcm")))
void WRITE16LE(u16 * x,u16 v){
  *((u16 *)x) = (v);
}

__attribute__((section(".itcm")))
void WRITE32LE(u32 * x, u32 v){
  *((u32 *)x) = (v);
}

__attribute__((section(".itcm")))
void UPDATE_REG(u16 address, u16 value)
{
	*(u16 *)&ioMem[address] = value;
}
