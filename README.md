These sources use devkitARM r43. And any dldi "carts" are supported.

Hello, after years of work (2015), and about 150 "testing builds" I managed to create at least three branches from (different) gbaemu4ds sources. Of which I managed to merge together into
a single branch.

Branch: wormsgbacompatible. devkitARM version: r45, libnds at commit 4c3bf52 and libfat at commit 3375e8f. (both on github)

This branch allows to boot Worms World Party, Worms Armageddon and possibly other Worms games using the same game engine.



Instructions:

How to build: 
 - Open msys2 console: Head to gbaemu4ds folder write "make clean", then "make", then copy the gbaemu4ds.nds file to any directory in SD card.
 - Or just copy /release folder gbaemu4ds.nds in root directory in SD card, and launch it. The card must have a DLDI that allows to read/write for it to work (NTR Mode).


How to boot: 
 - copy the /release folder contents, in SD:/ root folder. (where SD is the Mounted Media according your OS), if it prompts for overwrite: Yes to All. 
   And run gbaemu4ds.nds, and a simple file directory will read the gba folder contents. Press start to choose the file and that's it.
 
 
 Coto.