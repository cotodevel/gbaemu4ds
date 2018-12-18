//functions
#define arm9advsound			//gbaemu4ds sound

//extra settings for gbaemu4ds fs driver (gbafsbuffer) buffer + addresses higher than romsize (in ewram). (stream from slot 1)
#define chucksizeinsec 1 //1,2,4,8
#define buffslots 255
#define chucksize 0x200*chucksizeinsec

//#define fullsync				//trap more addresses-ranges of IO MAP (should raise compatibility)
//#define capture_and_pars
//#define antyflicker //only work with capture_and_pars
//#define skipper //only work with capture_and_pars
//#define skipperval 5 //only work with capture_and_pars and skipper

//settings
#define loadindirect	//load homebrew(disabled) or stream from slot 1 (enabled)
#define ichflytestkeypossibillity	// disallow Left+Right or Up+Down of being pressed at the same time
//#define powerpatches //do some hacky things that give the emulator more Vb in some cases.
//#define HBlankdma //only if hblanc irq is on
//#define forceHBlankirqs
#define advanced_irq_check


//(updated)settings
#define uppern_read_emulation

//(updated) debug
//#define usebuffedVcout		//preset line counter or hardware vcounter
#define unsecamr7com		//allows arm7 to either reach all gba map (unsecure,defined) or nds-hardware mapped mem (undefined)
//#define printsaveread
//#define printsavewrite
//#define countpagefalts
//#define DEV_VERSION	//printf load/stores
//#define checkclearaddr	//enabled: 0x03007ffc user IRQ handler / disabled: (mirror) 3ffffffc user IRQ handler

//#define checkclearaddr20 //break clock and some other things only work with checkclearaddr
//#define checkclearaddrrw
//#define printreads
//#define printsoundwrites
//#define dmawriteprint
//#define soundwriteprint
//#define printsoundtimer
//#define loaddirect
//#define noichflydebugger
#define ichflyDebugdumpon	//dump file to fat:/gbadump.bin on ARM / Thumb undefined exception
//#define lastdebug
//#define showdebug
//#define wifidebuger	//Coto: not used but kept for historical reasons
//#define debugpatch
//#define print_uppern_read_emulation
#define gba_handel_IRQ_correct	//proper GBA IRQ emulation (maybe)
