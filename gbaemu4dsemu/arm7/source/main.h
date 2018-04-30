
//sound
#ifndef ARM7_GBASOUND
#define ARM7_GBASOUND

#include <nds.h>
#include "../../common/wifi_shared.h"

#define NDS_HARDWARE_SOUND_DEVICES 	0xF		//0--15 nds ch
#define NDS_PSG_CHANNELS 			0x3F00
#define NDS_WNOISE_CHANNELS			0xC000
#define NDS_ADPCM_PCM_CHANNELS		0x00FF 	//well, DS can use all 16 CH as ADpcm/PCM but we can't take the others critically needed by gba ch

#define GBA_VIRTUAL_SOUND_DEVICES 0x6 //ch 1,2,3,4 + DSOUND A + B

//this stores the NDS sound channel hardware that is used by the virtual GBA sound channel
#define GBA_VIRTUAL_CH1 	NDS_PSG_CHANNELS
#define GBA_VIRTUAL_CH2 	NDS_PSG_CHANNELS
#define GBA_VIRTUAL_CH3 	NDS_ADPCM_PCM_CHANNELS //any ADPCM/PCM
#define GBA_VIRTUAL_CH4 	NDS_WNOISE_CHANNELS
#define GBA_VIRTUAL_DMASA	NDS_ADPCM_PCM_CHANNELS
#define GBA_VIRTUAL_DMASB	NDS_ADPCM_PCM_CHANNELS

//addresses
#define GBA_VIRTUAL_NR10			1 	//sweep / ch1
#define GBA_VIRTUAL_NR11_NR12		1 	//ch1 DUTY-LEN-ENV
#define GBA_VIRTUAL_NR13_NR14		1	//ch1 freq

#define GBA_VIRTUAL_NR21_NR22		2	//ch2 tone
#define GBA_VIRTUAL_NR23_NR24		2	//ch2 freq

#define GBA_VIRTUAL_NR30			3	//ch3 stop / wave ram select
#define GBA_VIRTUAL_NR31_NR32		3	//ch3 length/volume
#define GBA_VIRTUAL_NR33_NR34		3	//ch3 frequency control

#define GBA_VIRTUAL_NR41_NR42		4	//ch4 len/env
#define GBA_VIRTUAL_NR43_NR44		4	//ch4 freq/control


//sample frequency: 22050 11025 44100
//nds hardware channel setup

struct NDSCH{
	bool chstat; 			//false free - true busy channel
	int gba_channel; 		//gba channel listing (channel 1, channel 2, channel 3, channel 4, channel 5 (DMA A), channel 6 (DMA B) ) -> nds 0xf channels
	int mode; 				//bit30-31 & 0x60000000 -> (0=PCM8, 1=PCM16, 2=IMA-ADPCM, 3=PSG/Noise) (NDS hardware channel mode)
	int frequency; 			//22050 11025 44100
	int amplitude;			//volume of wave
	int panning;			//volume amplitude balance per side
	int duty;				//psg duty (only nds psg hardware)
	bool enabled; 			//nds sound channel can be used : true yes | false no (hardware)
	bool assigned;			//nds channel assigned (virtual)
};

//gbavirtual channel listing
struct GBAVIRTCH{
	int  gba_channel;
	bool assigned;
	int  nds_channel_assigned;
};

// info about the sample
struct SWAVINFO
{
	u8  nWaveType;    // 0 = PCM8, 1 = PCM16, 2 = (IMA-)ADPCM
	u8  bLoop;        // Loop flag = TRUE|FALSE
	u16 nSampleRate;  // Sampling Rate
	u16 nTime;        // (ARM7_CLOCK / nSampleRate) [ARM7_CLOCK: 33.513982MHz / 2 = 1.6756991 E +7]
	u16 nLoopOffset;  // Loop Offset (expressed in words (32-bits))
	u32 nNonLoopLen;  // Non Loop Length (expressed in words (32-bits))
};



#endif


#ifdef __cplusplus
extern "C" {
#endif

extern u8 * arm7dmaexchange_buf;

void checkstart();
void updatevol();
void updatetakt();
void enableSound();

extern bool ykeypp;
extern bool isincallline;

extern void vcount_handler();
extern void vblank_handler();
extern void hblank_handler();

//these must not be optimized
extern void lid_open_irq_handler();
extern void lid_closing_handler(u32 WAKEUP_IRQS);

//so we can setup irqs from ARM9
extern u32 interrupts_to_wait_arm7;

//process that makes ARM9 wifi_sync();
extern int arm7_synctoarm9_process();

extern void ipcsynchandler();

extern void write_sound_command(u32 addr,u32 val);

extern u32 arm7ticks;
extern u32 power;
extern u32 ie_save;

extern bool autodetectdetect;
extern u16 callline;

extern u8 DMASA_TIMER_SEL;
extern u8 DMASB_TIMER_SEL;
extern int gba_to_gb_sound( int addr );

extern volatile u8 waveram[2*0x10];
extern int pitch;
extern int pitch2;
extern int basefreq;
extern int volume;
extern volatile s16 sineTable[256];

//Init gba sound system
extern void init_gbasound_system();


//allocates a GBA Channel instance (taking one NDS hw channel)
//returns 0x10 if all-busy NDS CH
//returns 0x11 if already assigned GBA CH
//return 0 -- 0xf with the NDS CH assigned to your new GBA Virtual Channel Instance
extern volatile int assigngbavirt_ch(int req_gbachan);

//returns 0 -- 0xf with the NDS hw channel used by a GBA Virtual channel instance
//otherwise 0x10 is free (not assigned GBA Virtual channel)
extern volatile  int getgbavirt_ch(int req_gbachan);


extern int updatechannel(int channel, int toggle,struct NDSCH * snd);
extern u16 volumetable(int volume);
extern u16 AdjustFreq(u16 basefreq, int pitch);

extern volatile u16 ADJUST_FREQ(u16 basefreq, int noteN, int baseN);
extern volatile u16 ADJUST_PITCH_BEND(u16 basefreq, int pitchb, int pitchr);
extern int playsound(int freq,int channel,int volume,int pan,int psgduty,int mode,int enable);

//index of cur GBA instrument playing
extern int current_instrument_play;	//1--6 ch 1,2,3,4 dmasa 1 dmasb 2
extern volatile u32 play_instr();

//queues go here 
extern volatile u32 queue_gbavirt_instr(int new_gbach,int freq, int volume, int pan, int psgduty, int mode, int ndsch_enabled);

//delete virtual channels
extern volatile int deallocvirtch(int req_gbachan);

//NDS7 sound instance is global
extern volatile struct NDSCH NDSCH_inst[NDS_HARDWARE_SOUND_DEVICES];
extern volatile struct SWAVINFO SWAVINFO_inst[NDS_HARDWARE_SOUND_DEVICES];
extern volatile struct GBAVIRTCH GBAVIRTCH_inst[GBA_VIRTUAL_SOUND_DEVICES];

//gba timer/dma process
extern void timer0interrupt_thread();
extern void timer1interrupt_thread();

extern u8 dmasa_buffer[0x4];
extern u8 dmasb_buffer[0x4];

extern u8 DMASA_TIMER_SEL;
extern u8 DMASB_TIMER_SEL;

extern bool dmasa_play;
extern bool dmasb_play;

extern void playdmafifo();

#ifdef __cplusplus
}
#endif
