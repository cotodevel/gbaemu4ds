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

#include "GBA.h"
#include "gui.h"
#include "nds_cp15_misc.h"
#include "dldi.h"
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
#include "ichflysettings.h"

u32 *sectortabel;
void * lastopen;
void * lastopenlocked;
int PosixStructFDLastLoadFile;	//Coto: a StructFD non-posix file descriptor having the handle for the FILE * open at the time the sector table map was generated.
								//TGDS uses this to access the FSFAT level file attributes
u32 current_pointer = 0;
u32 allocedfild[buffslots];
u8* greatownfilebuffer;

//DLDI Interface @ io_dldi_data->ioInterface.readSectors(sector, count, buff)

//part of fatfile.c
void generatefilemap(FILE * f, int size)	//FILE * f is already open at this point.
{
	clrscr();
	int clusCount;
	printf("generating file map (size %d Byte)",((size/chucksize) + 1)*8);
	sectortabel = (u32*)malloc(((size/chucksize) + 1)*8); //alloc for size every Sector has one u32
	greatownfilebuffer = (u8*)malloc(chucksize * buffslots);
	clusCount = size/getDiskClusterSizeBytes();
	
	sint32 fd = -1;
	struct fd * fdinst = NULL;
	if(f){
		fd = fileno(f);
		fdinst = fd_struct_get(fd);
		
		if(fdinst->filPtr){	//File open OK.
			PosixStructFDLastLoadFile = fd;		//use fd_struct_get(PosixStructFDLastLoadFile) to access the File from now on!
		}
		else{	//File open ERROR.
			clrscr();
			printf("this is not a FIL * !!");
			while(1==1){}
		}
	}
	else{
		clrscr();
		printf("the FILE * was closed.");
		while(1==1){}
	}
	
	//Coto Todo: chucksize replace by sectorSize so its SD dependant and implementation does not break on cards that aren't 512 bytes per sector lol
	//setblanc
	int i = 0;
	while(i < (getDiskClusterSizeBytes()/chucksize)*clusCount+1)
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

	//this actual Cluster
	int mappoffset = 0;
	i = 0;
	while(i < (getDiskClusterSizeBytes()/chucksize))
	{
		sectortabel[mappoffset*2 + 1] = clust2sect(fdinst->filPtr->obj.fs, fdinst->filPtr->obj.sclust) + i;
		mappoffset++;
		i++;
	}
	
	//Iterate over several Clusters
	int ClusterOffset = 1;	//start from next Cluster please
	while (clusCount > 0) {
		clusCount--;
		i = 0;
		while(i < (getDiskClusterSizeBytes()/chucksize))
		{
			sectortabel[mappoffset*2 + 1] = clust2sect(fdinst->filPtr->obj.fs, fdinst->filPtr->obj.sclust + ClusterOffset) + i;
			mappoffset++;
			i++;
		}
		ClusterOffset++;
	}
	
	//debug
	/*
	//read sector table from original gbaemu4ds
	uint8 * sectortabel_read = (uint8*)malloc(((size/chucksize) + 1)*8);
	//compare the sector tables from original GBAEMU4DS against TGDS version
	FILE * fcompare = fopen_fs(getfatfsPath((char*)"gbaemu4dsori_sectortabel.bin"),"r");
	//size_t fread ( void * ptr, size_t size, size_t count, FILE * stream )
	int sizeRead = fread_fs((uint8*)sectortabel_read,1,(((size/chucksize) + 1)*8),fcompare);
	fclose_fs(fcompare);
	
	//clrscr();
	printf("Read %d bytes!",sizeRead);
	//save TGDS sector table
	FILE * fsave = fopen_fs(getfatfsPath((char*)"TGDS-sectortabel.bin"),"w+");
	fwrite_fs((uint8*)sectortabel,1,(((size/chucksize) + 1)*8),fsave);
	fclose_fs(fsave);
	
	uint32 * sectorPtr1 = (uint32*)sectortabel_read;
	uint32 * sectorPtr2 = (uint32*)sectortabel;
	
	//use sector offset 1 (uint32) because the sector 0 is always zero 
	if ((uint32)(sectorPtr1[1]) == (uint32)(sectorPtr2[1])){
		printf("sector table parse OK!");
	}
	else{
		printf("sector table TGDS malformed!");
	}
	printf("sectorTblTGDS:%x",sectorPtr2[1]);
	printf("sectorTblLIBFAT:%x",sectorPtr1[1]);
	printf("building sectortable end.");
	*/
	//debug end
}

void getandpatchmap(int offsetgba,int offsetthisfile,FILE * f)	//FILE * f is already open at this point.
{
	int clusCount;
	sint32 fd = -1;
	struct fd * fdinst = NULL;
	clusCount = offsetthisfile/getDiskClusterSizeBytes();
	int offset1 = (offsetthisfile/chucksize) % getDiskClusterSizeBytes();
	int mappoffset = offsetthisfile/chucksize;
	if(f){
		fd = fileno(f);
		fdinst = fd_struct_get(fd);
		if(fdinst->filPtr){	//File open OK.
			sectortabel[mappoffset*2 + 1] = clust2sect(fdinst->filPtr->obj.fs, fdinst->filPtr->obj.sclust + clusCount) + offset1;
		}
		else{	//File open ERROR.
			clrscr();
			printf("this is not a FIL * !!");
			while(1==1){}
		}
	}
}


//part of gbaemu4ds_fat_ext.cpp

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

	io_dldi_data->ioInterface.readSectors(sectortabel[mappoffset*2 + 1], chucksizeinsec, asd);	//readSectorslocked(sectortabel[mappoffset*2 + 1], chucksizeinsec, asd);
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
	
	io_dldi_data->ioInterface.readSectors(sectortabel[mappoffset*2 + 1], chucksizeinsec, asd);	//readSectorslocked(sectortabel[mappoffset*2 + 1], chucksizeinsec, asd);
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

	io_dldi_data->ioInterface.readSectors(sectortabel[mappoffset*2 + 1], chucksizeinsec, asd);	//readSectorslocked(sectortabel[mappoffset*2 + 1], chucksizeinsec, asd);
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

			io_dldi_data->ioInterface.readSectors(sectortabel[mappoffset*2 + 1], chucksizeinsec, asd);	//readSectorslocked(sectortabel[mappoffset*2 + 1], chucksizeinsec, asd);
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
			//printf("%X %X %X %X %X %X\n\r",sectoroffset,mappoffset,currsize,pos,c,chucksize);
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

			io_dldi_data->ioInterface.readSectors(sectortabel[mappoffset*2 + 1], chucksizeinsec, asd);	//readSectorslocked(sectortabel[mappoffset*2 + 1], chucksizeinsec, asd);
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
