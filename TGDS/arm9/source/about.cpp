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


//Coto: add support title + compile version for easier debugging.

#include "about.h"
#include "toolchain_utils.h"

#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "main.h"
#include "specific_shared.h"

sint8*  GBAEMU4DSTITLE[] = {
	"-= GBAEMU4DS",
	"by ichfly =-\n"
};

//fullpath	/rompath	/gamedir	/dirgame	/dirpath
sint8*  READ_GAME_DIR[] = {
	"gba"	//"0:/gba"	//correct format for opendir(); /each folder must not have an ending "/"
};

volatile char versionBuf[0x100];
sint8 * RetTitleCompiledVersion(){
	addAppVersiontoCompiledCode((VERSION_DESCRIPTOR *)&Version,(char *)appver,64);
	Version[0].app_version[63] = '0';
	//Update info + toolchain version
	updateVersionfromCompiledCode((VERSION_DESCRIPTOR *)&Version);
	sprintf((char*)&versionBuf[0],"%s %s %s",(sint8*)GBAEMU4DSTITLE[0],(sint8*)Version[0].app_version,(sint8*)GBAEMU4DSTITLE[1]);
	return (char*)&versionBuf[0];
}
