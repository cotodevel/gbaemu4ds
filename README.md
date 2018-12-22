These sources use devkitARM r43. And any dldi "carts" are supported.

Hello, after years of work (2015), and about 150 "testing builds" I managed to create at least three branches from (different) gbaemu4ds sources. Of which I managed to merge together into
a single branch.

Currently it support more or less the same games gbaemu4ds alpha2fix4 does but the compatibility is a bit better (just a bit, I got to boot a few games that didn't boot in alpha2fix4) since 
I added/merged some GBA Cpu stuff that was missing or that caused issues.

I planned originally to release gbaemu4ds sources using TGDS toolchain, but I think that will happen later. 
In fact that was the only thing really holding me back from releasing gbaemu4ds updates, but since it's been 3 years later. Yuck... it's time now.
PS: If you are a coder, use always differencing and merging tools. These really work

- Fixed hblank irqs, fixes pokemon fire red / green leaf battles
- SRAM/EEprom/Flash saves supported. Note: press Y mid-game -only- after the game saved. 
- RTC support
- cleanup gbaemu4ds source code. 
- broken mode 0 support. You will see games that use mode1 correctly or glitched. Other modes might work correctly.
- In-game soft-reset method (A+B+SELECT+START)

Instructions:

How to build: 
 - Open msys2 console: Head to hbmenu folder write "make clean", then "make", then copy the hbmenu.nds file to any directory in SD card.
 - Head to hyperspeedup, write "make clean", then "make", then copy the advirqsound.loader file to any SDRoot/gbaemu4ds folder. Create it if missing.
 

How to boot: 
 - Just copy /release folder contents in root directory in SD card, and launch hbmenu.nds. hbmenu.nds must be DLDI-patched (NTR Mode). If prompts for overwrite: Yes to All. 
   Now pick your file. Press A consecutively if you don't know what to do.
 
 
 Coto.