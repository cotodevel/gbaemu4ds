//shared static object across both ARM7 and ARM9 cpu (copied to shared region memory)

#ifndef SHARED_IPC_defs
#define SHARED_IPC_defs

#include <nds/ndstypes.h>

#define SHAREDIPC_MAX_ARGS 4
#define ARM7ARM9SLOTCMDS 16

#define FIFO_SWI_SLEEPMODE_PHASE1 (u32)(0xff1100aa)
#define FIFO_SWI_SLEEPMODE_PHASE2 (u32)(0xff1100ab)
#define FIFO_SWIGBA_FROM_ARM7 	(u32)(0xff1100ac)
#define WIFI_SYNC_GBAEMU4DS 	(u32)(0xff1100ad)
#define WIFI9_SETUP_GBAEMU4DS 	(u32)(0xff1100ae)
#define DSWIFI_DEINIT_WIFI 	(u32)(0xff1100af)
#define FIFO_DEBUG 	(u32)(0xff1100b0)

//ARM7 <- ARM9 update GBA IRQ Regs
#define ARM9_DOGBASHADOWCOPY 	(u32)(0xff1100b1)

//ARM9 <- ARM7 update GBA IRQ Regs
#define ARM7_DOGBASHADOWUPDATE 	(u32)(0xff1100b2)

//gba dma fifo
#define INTERNAL_FIFO_SIZE 	(32)	//internal DMA FIFO memory size (per channel A/B)

//GBA hardware interrupts emulated in ARM7
#define NDSGBAHARDWARE_HELD_ARM7 (u32)(IRQ_TIMER0 | IRQ_TIMER1 | IRQ_DMA1 | IRQ_DMA2)

//so when shadow update (ARM7-> ARM9) = (GBAIE & NDSGBAHARDWARE_HELD_ARM7) & value


//DMA FIFO
#define ARM7_CPUCHECKDMA1FIFO 	(u32)(0xff1100b3)	//CPUCheckDMA(3, 2);	//Serve DMA1 FIFO
#define ARM7_CPUCHECKDMA2FIFO 	(u32)(0xff1100b4)	//CPUCheckDMA(3, 4);	//Serve DMA2 FIFO

//emulator direct DMAFIFOWRITE (handles specific logic)
#define WRITEWORD_DMAFIFO_A 	(u32)(0xff1100b5)
#define WRITEWORD_DMAFIFO_B 	(u32)(0xff1100b6)

//if 
//   dma fifo copy: update internal buffer
#define WRITEWORD_UPDATEDMAFIFO_8 	(u32)(0xff1100b7)
#define WRITEWORD_UPDATEDMAFIFO_16 	(u32)(0xff1100b8)
#define WRITEWORD_UPDATEDMAFIFO_32 	(u32)(0xff1100b9)

//else
//   otherwise plain write
#define WRITEWORD_PLAINDMAFIFO_8 	(u32)(0xff1100ba)
#define WRITEWORD_PLAINDMAFIFO_16 	(u32)(0xff1100bb)
#define WRITEWORD_PLAINDMAFIFO_32 	(u32)(0xff1100bc)


typedef struct
{
	u32 entryPoint;
	u8 logo[156];
	char title[0xC];
	char gamecode[0x4];
	u16 makercode;
	u8 is96h;
	u8 unitcode;
	u8 devicecode;
	u8 unused[7];
	u8 version;
	u8 complement;
	u16 checksum;
} __attribute__ ((__packed__)) gbaHeader_t;


//---------------------------------------------------------------------------------
struct sIPCSharedGBA{
//---------------------------------------------------------------------------------
    int16 touchX,   touchY;   // raw x/y
	int16 touchXpx, touchYpx; // TFT x/y pixel

	int16 touchZ1,  touchZ2;  // TSC x-panel measurements
	uint16 tdiode1,  tdiode2;  // TSC temperature diodes
	uint32 temperature;        // TSC computed temperature

	uint16 buttons;            // keypad buttons
    uint16 buttons_xy_folding;  // X, Y, /PENIRQ buttons

    //unused
	union {
		uint8 curtime[8];        // current time response from RTC

        struct {
                u8 rtc_command;
                u8 rtc_year;           //add 2000 to get 4 digit year
                u8 rtc_month;          //1 to 12
                u8 rtc_day;            //1 to (days in month)
        
                u8 rtc_incr;
                u8 rtc_hours;          //0 to 11 for AM, 52 to 63 for PM
                u8 rtc_minutes;        //0 to 59
                u8 rtc_seconds;        //0 to 59
        };
    };
    
	u8 touched;				    //TSC touched?
    u8 touch_pendown;           //TSC already held before?
    
	uint16 battery;             // battery life status
	uint16 aux;                 // SPI AUX port status

    vuint8 mailBusy;              //checks if IPC between ARM Cores is in use (1), 0 otherwise.
  
    u32 rom_entrypoint;
    
    //IPC Clock
    //[0]; //yy
    //[1]; //mth
    //[2]; //dd
    //[3]; //wat - day of week?
    //[4]; //hh
    //[5]; //mm
    //[6]; //ss
    u8 clockdata[0x20];
    
	//GBA Header
    gbaHeader_t gbaheader;
	
	//transfer queue
    u8 status; //see processor ipc read/writes flags
    u32 buf_queue[0x10];
  
	bool arm7asleep;
	
	//The next hardware mapped here: DMA 1,2 and Timers 0,1 since they belong to GBA sound system
    
    //DMA
    u16 DM0SAD_L;
    u16 DM0SAD_H;
    u16 DM0DAD_L;
    u16 DM0DAD_H;
    u16 DM0CNT_L;
    u16 DM0CNT_H;
    
    u16 DM1SAD_L;
    u16 DM1SAD_H;
    u16 DM1DAD_L;
    u16 DM1DAD_H;
    u16 DM1CNT_L;
    u16 DM1CNT_H;
    
    u16 DM2SAD_L;
    u16 DM2SAD_H;
    u16 DM2DAD_L;
    u16 DM2DAD_H;
    u16 DM2CNT_L;
    u16 DM2CNT_H;
    
    u16 DM3SAD_L;
    u16 DM3SAD_H;
    u16 DM3DAD_L;
    u16 DM3DAD_H;
    u16 DM3CNT_L;
    u16 DM3CNT_H;
    
    //Timers
    u16 TM0CNT_L;
	u16 TM0CNT_H;
	u16 TM1CNT_L;
	u16 TM1CNT_H;
	u16 TM2CNT_L;
	u16 TM2CNT_H;
	u16 TM3CNT_L;
	u16 TM3CNT_H;

    //GBA Virtual Sound Controller
	
	//SOUND1CNT_L (NR10) - Channel 1 Sweep register (R/W)
	u16 SOUND1CNT_L;
	
	//SOUND1CNT_H (NR11, NR12) - Channel 1 Duty/Len/Envelope (R/W)
	u16 SOUND1CNT_H;
	
	//SOUND1CNT_X (NR13, NR14) - Channel 1 Frequency/Control (R/W)
	u16 SOUND1CNT_X;
	
	//SOUND2CNT_L (NR21, NR22) - Channel 2 Duty/Length/Envelope (R/W)
	u16 SOUND2CNT_L;
	
	//SOUND2CNT_H (NR23, NR24) - Channel 2 Frequency/Control (R/W)
	u16 SOUND2CNT_H;
	
	//SOUND3CNT_L (NR30) - Channel 3 Stop/Wave RAM select (R/W)
	u16 SOUND3CNT_L;
	
	//SOUND3CNT_H (NR31, NR32) - Channel 3 Length/Volume (R/W)
	u16 SOUND3CNT_H;
	
	//SOUND3CNT_X (NR33, NR34) - Channel 3 Frequency/Control (R/W)
	u16 SOUND3CNT_X;
	
	//WAVE_RAM0_L - Channel 3 Wave Pattern RAM (W/R)
	u16 WAVE_RAM0_L;
	
	//WAVE_RAM0_H - Channel 3 Wave Pattern RAM (W/R)
	u16 WAVE_RAM0_H;
	
	//WAVE_RAM1_L - Channel 3 Wave Pattern RAM (W/R)
	u16 WAVE_RAM1_L;
	
	//WAVE_RAM1_H - Channel 3 Wave Pattern RAM (W/R)
	u16 WAVE_RAM1_H;
	
	//WAVE_RAM2_L - Channel 3 Wave Pattern RAM (W/R)
	u16 WAVE_RAM2_L;
	
	//WAVE_RAM2_H - Channel 3 Wave Pattern RAM (W/R)
	u16 WAVE_RAM2_H;
	
	//WAVE_RAM3_L - Channel 3 Wave Pattern RAM (W/R)
	u16 WAVE_RAM3_L;
	
	//WAVE_RAM3_H - Channel 3 Wave Pattern RAM (W/R)
	u16 WAVE_RAM3_H;
	
	//SOUND4CNT_L (NR41, NR42) - Channel 4 Length/Envelope (R/W)
	u16 SOUND4CNT_L;
	
	//SOUND4CNT_H (NR43, NR44) - Channel 4 Frequency/Control (R/W)
	u16 SOUND4CNT_H;
	
	//FIFO DMA Emulation
    
    //40000A0h - FIFO_A_L - Sound A FIFO, Data 0 and Data 1 (W)
    u8 FIFO_A_L[2];
    //40000A2h - FIFO_A_H - Sound A FIFO, Data 2 and Data 3 (W)
    u8 FIFO_A_H[2];
    
    //These two registers may receive 32bit (4 bytes) of audio data (Data 0-3, Data 0 being located in least significant byte which is replayed first).
    //Internally, the capacity of the FIFO is 8 x 32bit (32 bytes), allowing to buffer a small amount of samples. As the name says (First In First Out), oldest data is replayed first.

    //40000A4h - FIFO_B_L - Sound B FIFO, Data 0 and Data 1 (W)
    u8 FIFO_B_L[2];
    //40000A6h - FIFO_B_H - Sound B FIFO, Data 2 and Data 3 (W)
    u8 FIFO_B_H[2];
	
    //u8 fifo_processed;  //0 idle / 1 ARM7/ARM9 process IO FIFO DMA ports
    
	u8 fifodmasA[INTERNAL_FIFO_SIZE];
	u8 fifodmasB[INTERNAL_FIFO_SIZE];

	//since FIFO is 16x2 bytes (DMA SA/DMASB) and FIFO stream registers are 4 bytes 16/4 = 4 steps, little endian.
	u8 fifoA_offset;
	u8 fifoB_offset;
	
	bool dma1_donearm9;
	bool dma2_donearm9;
	
	//SOUNDCNT_L (NR50, NR51) - Channel L/R Volume/Enable (R/W)
	u16 SOUNDCNT_L;
    
	//SOUNDCNT_H (GBA only) - DMA Sound Control/Mixing (R/W)
	u16 SOUNDCNT_H;
	
	//SOUNDCNT_X (NR52) - Sound on/off (R/W)
	u16 SOUNDCNT_X;
	
	//SOUNDBIAS
	u16 SOUNDBIAS;
	
	u32 GBAIE;
	u32 GBAIF;
	u32 GBAIME;
	
	
    //GBA Header
    gbaHeader_t gbaheaderInst;
	
	//Fast update GBAMap ARM9->ARM7 mechanism
	bool ShadowCopyStatus;
};

//4000136h - NDS7 - EXTKEYIN - Key X/Y Input (R)
//  0      Button X     (0=Pressed, 1=Released)
//  1      Button Y     (0=Pressed, 1=Released)
//  3      DEBUG button (0=Pressed, 1=Released/None such)
//  6      Pen down     (0=Pressed, 1=Released/Disabled) (always 0 in DSi mode)
//  7      Hinge/folded (0=Open, 1=Closed)
//  2,4,5  Unknown / set
//  8..15  Unknown / zero

#define KEY_XARM7 (1<<0)
#define KEY_YARM7 (1<<1)
#define KEY_HINGE (1<<7)

#define FIFO_DMA_SIZE (4)

//processor ipc read/writes flags
#define ARM7_BUSYFLAGRD (u8)(0x08)
#define ARM7_BUSYFLAGWR (u8)(0x0f)
#define ARM9_BUSYFLAGRD (u8)(0x80)
#define ARM9_BUSYFLAGWR (u8)(0xf0)

#endif

#ifdef __cplusplus
extern "C" {
#endif

/////////////////////hardware IPC /////////////////////////////////

//ARM7 only
#ifdef ARM7
extern void SendArm9Command(u32 command1, u32 command2, u32 command3, u32 command4);	//hardware IPC
#endif

//ARM9 only
#ifdef ARM9
extern void SendArm7Command(u32 command1, u32 command2, u32 command3, u32 command4); //hardware IPC
#endif

//gbaemu4ds clock opcodes
extern u8 gba_get_yearbytertc();
extern u8 gba_get_monthrtc();
extern u8 gba_get_dayrtc();
extern u8 gba_get_dayofweekrtc();
extern u8 gba_get_hourrtc();
extern u8 gba_get_minrtc();
extern u8 gba_get_secrtc();

//sleepmode
extern void enterGBASleepMode();
extern void Setarm7Sleep(bool value);
extern bool Getarm7Sleep();

//IPC
extern struct sIPCSharedGBA* GetsIPCSharedGBA();

extern void FIFO_DRAINWRITE();


extern void setShadowCopyStatus(bool value);
extern bool getShadowCopyStatus();


#ifdef __cplusplus
}
#endif
