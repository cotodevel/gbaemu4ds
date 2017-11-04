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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <zlib.h> //todo ichfly
#include <stdio.h>
#include <stdlib.h>
#include <nds/memory.h>//#include <memory.h> ichfly
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

#include "../../common/cpuglobal.h"
#include "../../common/gba_ipc.h"

#include <filesystem.h>
#include "GBA.h"
#include "Sound.h"
#include "Util.h"
#include "getopt.h"
#include "System.h"
#include <fat.h>
#include <dirent.h>

#include "cpumg.h"
#include "GBAinline.h"
#include "bios.h"

#include "mydebuger.h"

#include "file_browse.h"

#define MAXPATHLEN 256 

#include <nds.h>

#include "arm7sound.h"

#include "main.h"


#include <unistd.h>    // for sbrk()


#define INT_TABLE_SECTION __attribute__((section(".dtcm")))


extern struct IntTable irqTable[MAX_INTERRUPTS] INT_TABLE_SECTION;

extern "C" void __irqSet(u32 mask, IntFn handler, struct IntTable irqTable[] );



#include <fat.h>
#include "ichflysettings.h"

#if 0
#include "unrarlib.h"
#endif

#include "System.h"
#include "NLS.h"
#include "Util.h"
#include "Flash.h"
#include "GBA.h"
#include "Globals.h"
#include "RTC.h"
#include "Port.h"


#ifndef _MSC_VER
#define _stricmp strcasecmp
#endif // ! _MSC_VER

extern int systemColorDepth;
extern int systemRedShift;
extern int systemGreenShift;
extern int systemBlueShift;


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
/*
static u8 *utilLoadFromZip(const char *file,
                           bool (*accept)(const char *),
                           u8 *data,
                           int &size)
{
  char buffer[2048];
  
  unzFile unz = unzOpen(file);
    
  if(unz == NULL) {
    systemMessage(MSG_CANNOT_OPEN_FILE, N_("Cannot open file %s"), file);
    return NULL;
  }
  int r = unzGoToFirstFile(unz);
    
  if(r != UNZ_OK) {
    unzClose(unz);
    systemMessage(MSG_BAD_ZIP_FILE, N_("Bad ZIP file %s"), file);
    return NULL;
  }
    
  bool found = false;
    
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
      return NULL;
    }

    if(accept(buffer)) {
      found = true;
      break;
    }
    
    r = unzGoToNextFile(unz);
      
    if(r != UNZ_OK)
      break;
  }

  if(!found) {
    unzClose(unz);
    systemMessage(MSG_NO_IMAGE_ON_ZIP,
                  N_("No image found on ZIP file %s"), file);
    return NULL;
  }
  
  int fileSize = info.uncompressed_size;
  if(size == 0)
    size = fileSize;
  r = unzOpenCurrentFile(unz);

  if(r != UNZ_OK) {
    unzClose(unz);
    systemMessage(MSG_ERROR_OPENING_IMAGE, N_("Error opening image %s"), buffer);
    return NULL;
  }

  u8 *image = data;
  
  if(image == NULL) {
    image = (u8 *)malloc(utilGetSize(size));
    if(image == NULL) {
      unzCloseCurrentFile(unz);
      unzClose(unz);
      systemMessage(MSG_OUT_OF_MEMORY, N_("Failed to allocate memory for %s"),
                    "data");
      return NULL;
    }
    size = fileSize;
  }
  int read = fileSize <= size ? fileSize : size;
  r = unzReadCurrentFile(unz,
                         image,
                         read);

  unzCloseCurrentFile(unz);
  unzClose(unz);
  
  if(r != (int)read) {
    systemMessage(MSG_ERROR_READING_IMAGE,
                  N_("Error reading image %s"), buffer);
    if(data == NULL)
      free(image);
    return NULL;
  }

  size = fileSize;

  return image;
}

static u8 *utilLoadGzipFile(const char *file,
                            bool (*accept)(const char *),
                            u8 *data,
                            int &size)
{
  FILE *f = fopen(file, "rb");

  if(f == NULL) {
    systemMessage(MSG_ERROR_OPENING_IMAGE, N_("Error opening image %s"), file);
    return NULL;
  }

  fseek(f, -4, SEEK_END);
  int fileSize = fgetc(f) | (fgetc(f) << 8) | (fgetc(f) << 16) | (fgetc(f) << 24);
  fclose(f);
  if(size == 0)
    size = fileSize;

  gzFile gz = gzopen(file, "rb");

  if(gz == NULL) {
    // should not happen, but who knows?
    systemMessage(MSG_ERROR_OPENING_IMAGE, N_("Error opening image %s"), file);
    return NULL;
  }

  u8 *image = data;

  if(image == NULL) {
    image = (u8 *)malloc(utilGetSize(size));
    if(image == NULL) {
      systemMessage(MSG_OUT_OF_MEMORY, N_("Failed to allocate memory for %s"),
                    "data");
      fclose(f);
      return NULL;
    }
    size = fileSize;
  }
  int read = fileSize <= size ? fileSize : size;
  int r = gzread(gz, image, read);
  gzclose(gz);

  if(r != (int)read) {
    systemMessage(MSG_ERROR_READING_IMAGE,
                  N_("Error reading image %s"), file);
    if(data == NULL)
      free(image);
    return NULL;
  }  
  
  size = fileSize;

  return image;  
}
*/
#if 0
static u8 *utilLoadRarFile(const char *file,
                           bool (*accept)(const char *),
                           u8 *data,
                           int &size)
{
  char buffer[2048];

  ArchiveList_struct *rarList = NULL;
  if(urarlib_list((void *)file, (ArchiveList_struct *)&rarList)) {
    ArchiveList_struct *p = rarList;
    
    bool found = false;
    while(p) {
      if(accept(p->item.Name)) {
        strcpy(buffer, p->item.Name);
        found = true;
        break;
      }
      p = p->next;
    }
    if(found) {
      void *memory = NULL;
      unsigned long lsize = 0;
      size = p->item.UnpSize;
      int r = urarlib_get((void *)&memory, &lsize, buffer, (void *)file, "");
      if(!r) {
        systemMessage(MSG_ERROR_READING_IMAGE,
                      N_("Error reading image %s"), buffer);
        urarlib_freelist(rarList);
        return NULL;
      }
      u8 *image = (u8 *)memory;
      if(data != NULL) {
        memcpy(image, data, size);
      }
      urarlib_freelist(rarList);
      return image;
    }
    systemMessage(MSG_NO_IMAGE_ON_ZIP,
                  N_("No image found on RAR file %s"), file);
    urarlib_freelist(rarList);
    return NULL;
  }
  // nothing found
  return NULL;
}
#endif

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
			#ifdef arm7dmapluscheats
			
			fread((void*)&cheatsNumber,1,0x4,patchf);
			int offset;
			fread((void*)&offset,1,0x4,patchf);
			int coo5 = ftell(patchf);
			fseek(patchf,offset,SEEK_SET);
			fread((void*)cheatsList,1,cheatsNumber*28,patchf);
			fseek(patchf,coo5,SEEK_SET);
			__irqSet(IRQ_FIFO_NOT_EMPTY,arm7dmareqandcheat,irqTable);
#ifdef debugpatch
			iprintf("patch cheats %08X from %08X (%08X)\n\r",cheatsNumber,cheatsList,ftell(patchf));
#endif
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

u8 *utilLoad(const char *file, //ichfly todo
             u8 *data,
             int size,bool extram)
{
  u8 *image = data;


  FILE *f = fopen(file, "rb");

  if(!f) {
    systemMessage(MSG_ERROR_OPENING_IMAGE, N_("Error opening image %s"), file);
    return NULL;
  }

  fseek(f,0,SEEK_END);
  int fileSize = ftell(f);
  fseek(f,0,SEEK_SET);

  generatefilemap(fileSize);

  if(data == NULL)
  {
	  romSize = 0x02400000 - ((u32)sbrk(0) + 0x5000 + 0x2000);
	  rom = (u8 *)(sbrk(0) + 0x2000/*8K for futur alloc*/);              //rom = (u8 *)0x02180000; //old
	  image = data = rom;
	  size = romSize;
  }
  size_t read = fileSize <= size ? fileSize : size;


  size_t r= 0x80000;

  //workaround read
  /*int seek = 0;
  while(r == 0x80000)
  {
	fseek(f,seek,SEEK_SET);
	if(read > 0x80000)r = fread(image, 1, 0x80000, f); //512 KByte chucks
	else r = fread(image, 1, read, f);
	read -= r;
	image += r;
	seek += r;
	fclose(f);
	f = fopen(file, "rb"); //close and open
  }*/

	r = fread(image, 1, read, f);
	
	//set up header
    memcpy((u8*)&GBAEMU4DS_IPC->gbaheader,(u8*)rom,sizeof(gbaHeader_t));
    
#ifndef uppern_read_emulation
  fclose(f);
#else
  ichflyfilestream = f;
#endif

  if(r != read) {
    systemMessage(MSG_ERROR_READING_IMAGE,
                  N_("Error reading image %s"), file);
	while(1);
  }  
#ifdef uppern_read_emulation
  ichflyfilestreamsize = fileSize;
#endif
  //size = fileSize;
  
  if(patchPath[0] != 0)patchit(romSize);


  return image;
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


//coto: if a game is defined here savetype from gamecode will be used
int save_decider(){

//void * memcpy ( void * destination, const void * source, size_t num );
int savetype=0;
char gamecode[6]; 
memcpy((char*)gamecode,(u8*)&GBAEMU4DS_IPC->gbaheader.gamecode,6);

//iprintf("GameCode is: %s \n",gamecode);
//iprintf("GameCode is: %s \n",strtoupper(gamecode));
//iprintf("GameCode is: %s \n",strtolower(gamecode));
//while(1);

    if( strncmp( 
        strtoupper((char*)gamecode), 
        strtoupper((char*)"bpre01"), //firered 128K
        6
        ) == 0 
        )
    {
        
        //iprintf("firered detected!!!! \n");
        //while(1);
        savetype = 3; 
    }
    
	else if( strncmp( 
        strtoupper((char*)gamecode), 
        strtoupper((char*)"bpge01"), //greenleaf 128K
        6
        ) == 0 
        )
    {
        
        //iprintf("greenleaf detected!!!! \n");
        //while(1);
        savetype = 3; 
    }
    
    else if( strncmp( 
        strtoupper((char*)gamecode), 
        strtoupper((char*)"amze01"), //mario 1 eeprom 64K
        6
        ) == 0 
        )
    {
        
        //iprintf("smb1 detected!!!! \n");
        //while(1);
        savetype = 1; 
    }
    
    else if( strncmp( 
        strtoupper((char*)gamecode), 
        strtoupper((char*)"ax4p01"), //mario 3 128K
        6
        ) == 0 
        )
    {
        
        //iprintf("smb3 detected!!!! \n");
        //while(1);
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