#ifndef mem_handler_gbaemu4ds
#define mem_handler_gbaemu4ds

#include <nds.h>

#endif

#ifdef __cplusplus
extern "C"{
#endif

extern u32 * sbrk_alloc(int size);
extern u32 * sbrk_dealloc(int size);

//ld variables
//linker script hardware address setup (get map addresses from linker file)
extern int __ewram_start;
extern int __dtcm_start;

extern int __gbarom_start;
extern int __gbarom_end;

extern int __gbaew_start;
extern int __gbaew_end;

extern int __itcm_start;

//top NDS EWRAM used memory by libnds
extern int __end__;

#ifdef __cplusplus
}
#endif