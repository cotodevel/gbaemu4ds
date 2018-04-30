//shared static object across both ARM7 and ARM9 cpu (copied to shared region memory)

#ifndef SHARED_IPC_defs
#define SHARED_IPC_defs

#include <nds/ndstypes.h>

#define SHAREDIPC_MAX_ARGS 4
#define ARM7ARM9SLOTCMDS 16

#define KEY_XARM7 (1<<0)
#define KEY_YARM7 (1<<1)
#define KEY_HINGE (1<<7)

#define VCOUNT_LINE_INTERRUPT 159

//processor ipc read/writes flags
#define ARM7_BUSYFLAGRD (u8)(0x08)
#define ARM7_BUSYFLAGWR (u8)(0x0f)
#define ARM9_BUSYFLAGRD (u8)(0x80)
#define ARM9_BUSYFLAGWR (u8)(0xf0)


//gba dma fifo
#define INTERNAL_FIFO_SIZE 	(16)	//each DMA
#define FIFO_BUFFER_SIZE	(4)		//FIFO_A/FIFO_B = 4 Bytes


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
typedef struct sMyIPC {
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
  
	
	//not yet!
	/*
    //GBA IRQS
    
    
	//GBA IO MAP
    //bool ioReadable[0x400];     //GBA IO MAP Permissions (offset[n] true == GBA IO MAP Write / false == cant write GBA IO MAP)
    */
	
	u16 IE;

    u16 IF;

    u16 IME;
	
	//The next hardware mapped here: DMA 1,2 and Timers 0,1 since they belong to GBA sound system

    //These two registers may receive 32bit (4 bytes) of audio data (Data 0-3, Data 0 being located in least significant byte which is replayed first).
    //Internally, the capacity of the FIFO is 8 x 32bit (32 bytes), allowing to buffer a small amount of samples. As the name says (First In First Out), oldest data is replayed first.
    
    //40000A0h - FIFO_A_L - Sound A FIFO, Data 0 and Data 1 (W)
    //u8 FIFO_A_L[2];
    //40000A2h - FIFO_A_H - Sound A FIFO, Data 2 and Data 3 (W)
    //u8 FIFO_A_H[2];

    //40000A4h - FIFO_B_L - Sound B FIFO, Data 0 and Data 1 (W)
    //u8 FIFO_B_L[2];
    //40000A6h - FIFO_B_H - Sound B FIFO, Data 2 and Data 3 (W)
    //u8 FIFO_B_H[2];
    
    //DMA
    u32 dma0Source;
    u32 dma0Dest;
    u16 DM0SAD_L;
    u16 DM0SAD_H;
    u16 DM0DAD_L;
    u16 DM0DAD_H;
    u16 DM0CNT_L;
    u16 DM0CNT_H;
	
	//for ARM7 <-> ARM9 DMA Sync Process
	bool dma1_donearm9;
    bool dma2_donearm9;
    
    u32 dma1Source;
    u32 dma1Dest;
    u16 DM1SAD_L;
    u16 DM1SAD_H;
    u16 DM1DAD_L;
    u16 DM1DAD_H;
    u16 DM1CNT_L;
    u16 DM1CNT_H;
    
    u32 dma2Source;
    u32 dma2Dest;
    u16 DM2SAD_L;
    u16 DM2SAD_H;
    u16 DM2DAD_L;
    u16 DM2DAD_H;
    u16 DM2CNT_L;
    u16 DM2CNT_H;
    
    u32 dma3Source;
    u32 dma3Dest;
    u16 DM3SAD_L;
    u16 DM3SAD_H;
    u16 DM3DAD_L;
    u16 DM3DAD_H;
    u16 DM3CNT_L;
    u16 DM3CNT_H;
    
    //Timers
    
	//4000102h - TM0CNT_H - Timer 0 Control (R/W)
	//u16 timer0Value;
	u16 TM0CNT_H;
	
	//4000100h - TM0CNT_L - Timer 0 Counter/Reload (R/W)
	//int timer0Reload;
	u16 TM0CNT_L;
	
    bool timer0On;
    int timer0Ticks;
	int timer0ClockReload;
    
	
	//4000106h - TM1CNT_H - Timer 1 Control (R/W)
	//u16 timer1Value;
	u16 TM1CNT_H;
	
	//4000104h - TM1CNT_L - Timer 1 Counter/Reload (R/W)
	//int timer1Reload;
    u16 TM1CNT_L;
	
    bool timer1On;
    int timer1Ticks;
    int timer1ClockReload;
    
	
	//400010Ah - TM2CNT_H - Timer 2 Control (R/W)
	//u16 timer2Value;
    u16 TM2CNT_H;
	
	//4000108h - TM2CNT_L - Timer 2 Counter/Reload (R/W)
	//int timer2Reload;
    u16 TM2CNT_L;
	
	bool timer2On;
    int timer2Ticks;
    int timer2ClockReload;
    
	
	//400010Eh - TM3CNT_H - Timer 3 Control (R/W)
	//u16 timer3Value;
	u16 TM3CNT_H;
	
	//400010Ch - TM3CNT_L - Timer 3 Counter/Reload (R/W)
	//int timer3Reload;
    u16 TM3CNT_L;
	
    bool timer3On;
    int timer3Ticks;
    int timer3ClockReload;   
    
	//new
	u8 fifodmasA[INTERNAL_FIFO_SIZE];
	u8 fifodmasB[INTERNAL_FIFO_SIZE];

	//since FIFO is 16x2 bytes (DMA SA/DMASB) and FIFO stream registers are 4 bytes 16/4 = 4 steps, little endian.
	u8 fifoA_offset;
	u8 fifoB_offset;
	
    //GBA Sound Controller
    u16 SOUNDCNT_L;
    u16 SOUNDCNT_H;
	
	//SoundBias
	u16 SOUNDBIAS;
	
	//misc
	char charbuf[0x100];
	
} tMyIPC;

//Shared Work     027FF000h 4KB    -     -    -    R/W
//IPC Struct
#define SHARED_ADDR_REGION 0x027FF000
#define GBAEMU4DS_IPC ((tMyIPC volatile *)(SHARED_ADDR_REGION))

//4000136h - NDS7 - EXTKEYIN - Key X/Y Input (R)
//  0      Button X     (0=Pressed, 1=Released)
//  1      Button Y     (0=Pressed, 1=Released)
//  3      DEBUG button (0=Pressed, 1=Released/None such)
//  6      Pen down     (0=Pressed, 1=Released/Disabled) (always 0 in DSi mode)
//  7      Hinge/folded (0=Open, 1=Closed)
//  2,4,5  Unknown / set
//  8..15  Unknown / zero


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

extern void arm7printdebug(char * charbufptr,int charbufsize);
#ifdef __cplusplus
}
#endif
