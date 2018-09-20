These sources use devkitARM r43. And any dldi "carts" are supported.

Hello, after years of work (2015), and about 150 "testing builds" I managed to create at least three branches from (different) gbaemu4ds sources. Of which I managed to merge together into
a single branch.

Currently it support more or less the same games gbaemu4ds alpha2fix4 does but the compatibility is a bit better (just a bit, I got to boot a few games that didn't boot in alpha2fix4) since 
I added/merged some GBA Cpu stuff that was missing or that caused issues.

I planned originally to release gbaemu4ds sources using TGDS toolchain, but I think that will happen later. 
In fact that was the only thing really holding me back from releasing gbaemu4ds updates, but since it's been 3 years later. Yuck... it's time now.
PS: If you are a coder, use always differencing and merging tools. These really work

- Fixed hblank irqs, fixes pokemon fire red / green leaf battles
- use the MPU/Caches to speedup gba wram: the gba wram runs directly from nds ewram, but gba mirrored wram is handled by the MPU. Fixes some obscure bugs and gives speedup.
- SRAM/EEprom/Flash saves supported. Note: press Y mid-game -only- after the game saved. 
- RTC support
- cleanup gbaemu4ds source code. Definitely needs more code cleanup.
- broken mode 1 support. You will see games that use mode1 correctly or glitched.
- Soft reset works: Either in-game soft-reset method (A+B+SELECT+START) or gbaemu4ds menu (LEFT+UP+A+B) -> "reset GBA"

Instructions:

How to build: 
 - Open msys2 console: Head to gbaemu4ds folder write "make clean", then "make", then copy the gbaemu4ds.nds file to any directory in SD card.
 - Or just copy /release folder gbaemu4ds.nds in root directory in SD card, and launch it. The card must have a DLDI that allows to read/write for it to work (NTR Mode).


How to boot: 
 - copy the /release folder contents, in SD:/ root folder. (where SD is the Mounted Media according your OS), if it prompts for overwrite: Yes to All. 
   And run gbaemu4ds.nds, and a simple file directory will read the gba folder contents. Press start to choose the file and that's it.
 
 
 Coto.