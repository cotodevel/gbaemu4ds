1)

patchpack alpha 2 folder:

Q:
ichfly, i want to ask, could game compatibility be increased with patches?

A:
yes that is the reason I made this patches

Q:
if possible(It does look like it is), would you please release release some patches for (some) popular games like the megaman bn series?

A:
I don't have many games but there is a patchkit but it is not easy to use
edit: The patchkit http://www.mediafire.com/?m6rk7pl40ak3k1u but the documentation is not ready yet and all buttons are in German 

This folder has the patcher from alpha2fix4 (older gbaemu4ds patchKit), and the latest one (newest gbaemu4ds patchKit).

-

2)

gbaemu4ds pre-patcher folder:

patchpat1old folder: Before running hypervisor mode (within gbaemu4ds, not hbmenu launcher), prepares the DS hardware to enter hypervisor mode. 
						Just the MPU GBA hypervisor mode is set.

patchpat1new: Before running hypervisor mode (within gbaemu4ds, not hbmenu launcher), prepares the DS hardware to enter hypervisor mode.
						Disables IRQs, then the MPU GBA hypervisor mode is set.
						
-

3)

newest gbaemu4ds patchKit folder:

has the patcher program that increases compatibility with games. Currently the binary only works, the source was extracted, but cannot compile yet.

-

4) 

Coto:
todo: restore original patcher code inside gbaemu4ds (the one that merges the PC patches into the actual hypervisor) part. I removed it since I had no idea what it
did. Now I know what it does.. hah

--------------------------------

Ichfly tutorial: ( https://gbatemp.net/threads/gbaemu4ds-gba-emulator-for-ds-i-dsi-entry.281715/page-82#post-4496242 )

the TuT

Step 1:
Run the modified Version of VisualBoyAdvance.exe and run the game you want to patch. The modified VisualBoyAdvance.exe will now collect data while running the game.
Step 2:
Press the Capture Button(normally F12) the VisualBoyAdvance.exe will generate 2 Files (the results.ichfly is the important on)
Step 3:
Start WindowsFormsApplication1.exe and open the gba File and open the results.ichfly as IPS file
Step 4:
Enable Sp wechseln if there was no error while opening the gba file
Step 5:
Click on Start Anzeige
Step 6:
Check if the max PC is to high (the emulator can handle a max PC of about 8300000(hex Val) depending on some other settings in the rom)

Below is a list of found I/O and card accesses

first Val is the place where the instruction that accessed the I/O or card (you can ignore entrys that start with a Val below 2000000).
The second is the Address of the register that was accessed when this function was executed the first time (4XXXXXX = I/O 8000000 and greater = card).
The third is the number of times the instruction was executed.

The patcher is not yet full working only code that is placed in WRAM and only ARM code can be patched. (http://www.daftcode.net/gbatek/cpu#cpuoverview)

kein IRQ is needed if irqs are not disabled
wechsle stack is not working
sichere CPSR if the CPSR is needed to be stored (http://www.daftcode.net/gbatek/cpu#cpuflags)
Used 1 and 2 should always be set to a Register that is not used by the patched instruction
If the instruction is a load instruction it must be marked as zerstörbares Register if it is in the list.

setze pu nds <-- sets the CPU into nds mode at entering the patch (all instructions are executed as if they are nds code (faster)) (required if not already in this mode)
setze pu gba <-- sets the CPU into gba mode after the patch routine
Direct read offset <-- just copy the instruction and adds setze pu nds and/or setze pu gba


Last step create the patch as usual/you can use the hb file that sometimes also fix some bugs

normal.ccc emulates the HBirqs optimized for graphics (only works if the game is patched else the game gets graphic bugs by the lake of CPU or crush(of cause only is used if HBirqs are used (YA)))
half.ccc <-- only every second HBirq is executed
1by4.ccc etc <-- only every forth HBirq is executed
min.ccc only two HBirqs are executed
non.ccc no HBirqs are executed 