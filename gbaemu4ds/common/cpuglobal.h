#define neu_sound_16fifo

#define setdmasoundbuff 0x1FFFFFFA					//sets DMA NDS Channels to a desired sound buffer source
#define WaitforVblancarmcmd 0x1FFFFFFB				//frameasync on ARM9 (render async image from vmem)
#define enableWaitforVblancarmcmdirq 0x1FFFFFFC		//enable the above frame async render
//unused: #define getarm7keys 0x1FFFFFFD	//read XY Touch from IPC
#define set_callline 0x1FFFFFFF	//set vcounter irq line

//#define anyarmcom
//#define arm7dmapluscheats		//for dma fifo sound + cheats (old implementation)