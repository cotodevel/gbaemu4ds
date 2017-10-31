#include "InterruptsARMCores_h.h"
#include "specific_shared.h"
#include "dsregs_asm.h"
#include "main.h"
#include "keypad.h"
#include "interrupts.h"
#include "dswnifi_lib.h"
#include "dswnifi.h"

__attribute__((section(".itcm")))
//---------------------------------------------------------------------------------
void Vcounter(){
//---------------------------------------------------------------------------------

}


//---------------------------------------------------------------------------------
__attribute__((section(".itcm")))
void Vblank() {
//---------------------------------------------------------------------------------
	//handles DS-DS Comms
	if(doMULTIDaemon() >=0){
	}
	
	//key event between frames
	do_keys();
	
}


//---------------------------------------------------------------------------------
__attribute__((section(".itcm")))
void Hblank() {
//---------------------------------------------------------------------------------
	
}