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

Step 1):
- Download latest gbaemu4ds-alive sources from: https://bitbucket.org/Coto88/gbaemu4ds/get/gbaemu4ds-alive-stable.zip

How to Boot:
- Follow Step 1)
- Copy the /release folder contents, to SD:/ root folder. (where SD is the Mounted Media according your OS), if it prompts for overwrite: Yes to All.
- Create a /gba folder in SD:/ root folder. (where SD is the Mounted Media according your OS). Copy your homebrew , etc here.
- Now run hbmenu.nds, and press A to browse through the /gba directory. Press A to choose the file a lot of times and that's it.

How to build:
- Follow Step 1)
- Unzip gbaemu4ds sources. Write down that dir path.
- Open msys2 console: Head to same directory path written earlier, write "make clean", then "make", wait for the build to happen. After building is done, copy the /build folder contents to SD:/ root folder. (where SD is the Mounted Media according your OS). 



Force Save 100% compatibility mode:
By default almost everything will work if you press A for detecting save chip hardware (be it games or homebrew). 
Otherwise if you can´t save and you know the save chip used, if you press B, the hbmenu save chip hardware settings will override the current save hardware.
This way, you get 100% save compatibility working.
 

Coto.