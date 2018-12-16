
#include "fatfile.h"

#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>

#include "cache.h"
#include "file_allocation_table.h"
#include "bit_ops.h"
#include "filetime.h"
#include "lock.h"

#include "ichflysettings.h"

#include "../Globals.h"
#include "../GBA.h"

#ifdef __cplusplus
extern "C"{
#endif

extern u8 ichfly_readu8(unsigned int pos);
extern u16 ichfly_readu16(unsigned int pos);
extern u32 ichfly_readu32(unsigned int pos);
extern void ichfly_readdma_rom(u32 pos,u8 *ptr,u32 c,int readal);

extern u32 *sectortabel;
extern void * lastopen;
extern void * lastopenlocked;

extern PARTITION* partitionlocked;
extern FN_MEDIUM_READSECTORS	readSectorslocked;
extern u32 current_pointer;

extern u32* allocedfild;	//u32 allocedfild[buffslots]

extern u8* gbafsbuffer;

extern void generatefilemap(int size);
extern void getandpatchmap(int offsetgba,int offsetthisfile);

extern int latestsectortableSize;

#ifdef __cplusplus
}
#endif