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