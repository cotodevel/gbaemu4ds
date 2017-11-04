#include "gbaemu4ds_fat_ext.h"

#include "typedefs.h"
#include "dsregs.h"
#include "dsregs_asm.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/dir.h>
#include <fcntl.h>

#include "fs.h"
#include "gui.h"
#include "nds_cp15_misc.h"

#include "fsfat_layer.h"
#include "file.h"
#include "InterruptsARMCores_h.h"
#include "specific_shared.h"
#include "ff.h"
#include "mem_handler_shared.h"
#include "reent.h"
#include "sys/types.h"
#include "console.h"
#include "toolchain_utils.h"
#include "devoptab_devices.h"
#include "posix_hook_shared.h"
#include "about.h"
#include "xenofunzip.h"

u32 *sectortabel;
void * lastopen;
void * lastopenlocked;

/*
PARTITION* partitionlocked;
FN_MEDIUM_READSECTORS	readSectorslocked;
*/
u32 current_pointer = 0;
u32 allocedfild[buffslots];
u8* greatownfilebuffer;

//part of fatfile.c
void generatefilemap(int size)
{
	FILE_STRUCT* file = (FILE_STRUCT*)(lastopen);
	lastopenlocked = lastopen; //copy
	PARTITION* partition;
	uint32_t cluster;
	int clusCount;
	partition = file->partition;
	partitionlocked = partition;

	readSectorslocked = file->partition->disc->readSectors;
	iprintf("generating file map (size %d Byte)",((size/chucksize) + 1)*8);
	sectortabel =(u8*)malloc(((size/chucksize) + 1)*8); //alloc for size every Sector has one u32
	greatownfilebuffer =(u8*)malloc(chucksize * buffslots);

	clusCount = size/partition->bytesPerCluster;
	cluster = file->startCluster;


	//setblanc
	int i = 0;
	while(i < (partition->bytesPerCluster/chucksize)*clusCount+1)
	{
		sectortabel[i*2] = 0x0;
		i++;
	}
	i = 0;
	while(i < buffslots)
	{
		allocedfild[i] = 0x1;
		i++;
	}


	int mappoffset = 0;
	i = 0;
	while(i < (partition->bytesPerCluster/chucksize))
	{
		sectortabel[mappoffset*2 + 1] = _FAT_fat_clusterToSector(partition, cluster) + i;
		mappoffset++;
		i++;
	}
	while (clusCount > 0) {
		clusCount--;
		cluster = _FAT_fat_nextCluster (partition, cluster);

		i = 0;
		while(i < (partition->bytesPerCluster/chucksize))
		{
			sectortabel[mappoffset*2 + 1] = _FAT_fat_clusterToSector(partition, cluster) + i;
			mappoffset++;
			i++;
		}
	}

}

void getandpatchmap(int offsetgba,int offsetthisfile)
{
	FILE_STRUCT* file = (FILE_STRUCT*)(lastopen);
	PARTITION* partition;
	uint32_t cluster;
	int clusCount;
	partition = file->partition;

	clusCount = offsetthisfile/partition->bytesPerCluster;
	cluster = file->startCluster;

	int offset1 = (offsetthisfile/chucksize) % partition->bytesPerCluster;

	int mappoffset = offsetthisfile/chucksize;
	while (clusCount > 0) {
		clusCount--;
		cluster = _FAT_fat_nextCluster (partition, cluster);
	}
	sectortabel[mappoffset*2 + 1] = _FAT_fat_clusterToSector(partition, cluster) + offset1;
}


//part of fatfileextract.cpp

#include <nds.h>
#include "fatfileextract.h"
#include "../GBA.h"

__attribute__((section(".itcm")))
u8 ichfly_readu8(unsigned int pos) //need lockup
{

	// Calculate the sector and byte of the current position,
	// and store them
	unsigned int sectoroffset = pos % chucksize;
	unsigned int mappoffset = pos / chucksize;
	
	u8* asd = (u8*)(sectortabel[mappoffset*2]);
	
	if(asd != (u8*)0x0)return asd[sectoroffset]; //found exit here

	sectortabel[allocedfild[current_pointer]] = 0x0; //reset

	allocedfild[current_pointer] = mappoffset*2; //set new slot
	asd = greatownfilebuffer + current_pointer * chucksize;
	sectortabel[mappoffset*2] = (u32)asd;

	readSectorslocked(sectortabel[mappoffset*2 + 1], chucksizeinsec, asd);
#ifdef countpagefalts
pagefehler++;
#endif
	current_pointer++;
	if(current_pointer == buffslots)current_pointer = 0;
	
	return asd[sectoroffset];
}

__attribute__((section(".itcm")))
u16 ichfly_readu16(unsigned int pos) //need lockup
{

	// Calculate the sector and byte of the current position,
	// and store them
	unsigned int sectoroffset = pos % chucksize;
	unsigned int mappoffset = pos / chucksize;
	
	u8* asd = (u8*)(sectortabel[mappoffset*2]);
	
	if(asd != (u8*)0x0)return *(u16*)(&asd[sectoroffset]); //found exit here

	sectortabel[allocedfild[current_pointer]] = 0x0; //clear old slot

	allocedfild[current_pointer] = mappoffset*2; //set new slot
	asd = greatownfilebuffer + current_pointer * chucksize;
	sectortabel[mappoffset*2] = (u32)asd;
	
	readSectorslocked(sectortabel[mappoffset*2 + 1], chucksizeinsec, asd);
#ifdef countpagefalts
pagefehler++;
#endif
	current_pointer++;
	if(current_pointer == buffslots)current_pointer = 0;
	
	return *(u16*)(&asd[sectoroffset]);
}

__attribute__((section(".itcm")))
u32 ichfly_readu32(unsigned int pos) //need lockup
{

	// Calculate the sector and byte of the current position,
	// and store them
	unsigned int sectoroffset = pos % chucksize;
	unsigned int mappoffset = pos / chucksize;
	
	u8* asd = (u8*)(sectortabel[mappoffset*2]);
	
	if(asd != (u8*)0x0)return *(u32*)(&asd[sectoroffset]); //found exit here

	sectortabel[allocedfild[current_pointer]] = 0x0;

	allocedfild[current_pointer] = mappoffset*2; //set new slot
	asd = greatownfilebuffer + current_pointer * chucksize;
	sectortabel[mappoffset*2] = (u32)asd;

	readSectorslocked(sectortabel[mappoffset*2 + 1], chucksizeinsec, asd);
#ifdef countpagefalts
pagefehler++;
#endif
	current_pointer++;
	if(current_pointer == buffslots)current_pointer = 0;
	
	return *(u32*)(&asd[sectoroffset]);
}

__attribute__((section(".itcm")))
void ichfly_readdma_rom(u32 pos,u8 *ptr,u32 c,int readal) //need lockup only alined is not working 
{

	// Calculate the sector and byte of the current position,
	// and store them
	int sectoroffset = 0;
	int mappoffset = 0;

	int currsize = 0;

	if(readal == 4) //32 Bit
	{
		while(c > 0)
		{
			sectoroffset = (pos % chucksize) /4;
			mappoffset = pos / chucksize;
			currsize = (chucksize / 4) - sectoroffset;
			if(currsize == 0)currsize = chucksize / 4;
			if(currsize > c) currsize = c;
			

			u32* asd = (u32*)(sectortabel[mappoffset*2]);
			
			if(asd != (u32*)0x0)//found exit here
			{
				int i = 0; //copy
				while(currsize > i)
				{
					*(u32*)(&ptr[i*4]) = asd[sectoroffset + i];
					i++;
				}
				c -= currsize;
				pos += (currsize * 4);
				ptr += (currsize * 4);
				continue;
			}

			sectortabel[allocedfild[current_pointer]] = 0x0;

			allocedfild[current_pointer] = mappoffset*2; //set new slot
			asd = (u32*)(greatownfilebuffer + current_pointer * chucksize);
			sectortabel[mappoffset*2] = (u32)asd;

			readSectorslocked(sectortabel[mappoffset*2 + 1], chucksizeinsec, asd);
			#ifdef countpagefalts
pagefehler++;
#endif
			current_pointer++;
			if(current_pointer == buffslots)current_pointer = 0;

			int i = 0; //copy
			while(currsize > i)
			{
				*(u32*)(&ptr[i*4]) = asd[sectoroffset + i];
				i++;
			}
			c -= currsize;
			pos += (currsize * 4);
			ptr += (currsize * 4);
		}
	}
	else //16 Bit
	{
		while(c > 0)
		{
			sectoroffset = (pos % chucksize) / 2;
			mappoffset = pos / chucksize;
			currsize = (chucksize / 2) - sectoroffset;
			if(currsize == 0)currsize = chucksize / 2;
			if(currsize > c) currsize = c;

			u16* asd = (u16*)(sectortabel[mappoffset*2]);
			//iprintf("%X %X %X %X %X %X\n\r",sectoroffset,mappoffset,currsize,pos,c,chucksize);
			if(asd != (u16*)0x0)//found exit here
			{
				int i = 0; //copy
				while(currsize > i)
				{
					*(u16*)(&ptr[i*2]) = asd[sectoroffset + i];
					i++;
				}
				c -= currsize;
				ptr += (currsize * 2);
				pos += (currsize * 2);
				continue;
			}

			sectortabel[allocedfild[current_pointer]] = 0x0;

			allocedfild[current_pointer] = mappoffset*2; //set new slot
			asd = (u16*)(greatownfilebuffer + current_pointer * chucksize);
			sectortabel[mappoffset*2] = (u32)asd;

			readSectorslocked(sectortabel[mappoffset*2 + 1], chucksizeinsec, asd);
			#ifdef countpagefalts
pagefehler++;
#endif
			current_pointer++;
			if(current_pointer == buffslots)current_pointer = 0;

			int i = 0; //copy
			while(currsize > i)
			{
				*(u16*)(&ptr[i*2]) = asd[sectoroffset + i];
				i++;
			}
			c -= currsize;
			ptr += (currsize * 2);
			pos += (currsize * 2);
		}

	}
}
