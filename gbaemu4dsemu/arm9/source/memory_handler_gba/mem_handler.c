#include <nds.h>
#include <nds/memory.h>
#include <unistd.h>    //sbrk()
//coto: small memory handler given POSIX LIBNDS memory allocation is broken when different MPU mapping is in use.
#include "mem_handler.h"

//at start 0
u32 * this_heap_ptr = 0;
int free_allocable_mem = 0;

int calc_remaining_kernel_mem(){
	return (int)( (u32)this_heap_ptr - ((u32)&__end__) );
}

u32 * sbrk_init(){
	return (u32*)sbrk(0);
}

u32 * sbrk_alloc(int size){
	this_heap_ptr = (u32*)sbrk(size); // + allocs / - frees 
	free_allocable_mem = calc_remaining_kernel_mem();
	DC_FlushAll();
	return this_heap_ptr;
}

u32 * sbrk_dealloc(int size){
	this_heap_ptr = (u32*)sbrk(-size); // + allocs / - frees 
	free_allocable_mem = calc_remaining_kernel_mem();
	DC_FlushAll();
	return this_heap_ptr;
}