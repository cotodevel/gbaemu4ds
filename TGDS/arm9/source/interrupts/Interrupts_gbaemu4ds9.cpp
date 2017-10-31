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