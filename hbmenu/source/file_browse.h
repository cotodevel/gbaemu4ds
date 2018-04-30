/*-----------------------------------------------------------------
 Copyright (C) 2005 - 2010
	Michael "Chishm" Chisholm
	Dave "WinterMute" Murphy

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

------------------------------------------------------------------*/

#ifndef FILE_BROWSE_H
#define FILE_BROWSE_H

#ifdef __cplusplus

#include <string>
#include <vector>

extern void browseForFile (const std::vector<std::string> extensionList);

#endif

#define MAXPATHLEN 256 

#endif //FILE_BROWSE_H

#ifdef __cplusplus
extern "C" {
#endif

extern char biosPath[MAXPATHLEN * 2];
extern char patchPath[MAXPATHLEN * 2];
extern char savePath[MAXPATHLEN * 2];
extern char szFile[MAXPATHLEN * 2];
extern char temppath[MAXPATHLEN * 2];

#ifdef __cplusplus
}
#endif
