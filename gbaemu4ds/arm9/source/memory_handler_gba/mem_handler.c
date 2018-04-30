#include <nds.h>
#include <nds/memory.h>
#include <unistd.h>    //sbrk()
#include <nds/system.h>

//coto: small memory handler given POSIX / sbrk memory impl. is broken when different MPU mapping is in use.
#include "mem_handler.h"

u32 * sbrk_alloc(int size){
	DC_FlushAll();
	return (u32*)sbrk(size); // + allocs / - frees;
}

u32 * sbrk_dealloc(int size){
	DC_FlushAll();
	return (u32*)sbrk(-size); // + allocs / - frees;;
}