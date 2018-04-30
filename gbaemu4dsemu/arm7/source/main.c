#include <nds.h>
#include <nds/arm7/audio.h>
#include <nds/arm7/serial.h>
#include <stdio.h> //for NULL

//Shared memory region static object
#include "../../common/clock.h"
#include "../../common/gba_ipc.h"
#include "../../common/wifi_shared.h"

#include "main.h"
#include "wireless/wifi_arm7.h"
#include "ipc/ipc_arm7.h"
#include "ipc/touch_ipc.h"

//coto: we cant use 32K shared chip because its the stack for gba. So we set arm7 to 64K
/*
//dma buffer inherited from EWRAM
u8 * arm7dmaexchange_buf;

//u16 callline = 0xFFFF;
bool autodetectdetect = false;

//sound buffers
u8* soundbuffA = 0;
u8* soundbuffB = 0;

//dma register status
u8 dmaApart = 0;
u8 dmaBpart = 0;


//GBA Virtual Sound Controller
u16 SOUNDCNT_L = 0;
u16 SOUNDCNT_H = 0;

//GBA Virtual Timer Controller
u16 TM0CNT_L = 0;
u16 TM1CNT_L = 0;

u16 TM0CNT_H = 0;
u16 TM1CNT_H = 0;

//GBA Virtual DMA Controller
u16 DMA1CNT_H = 0;
u16 DMA2CNT_H = 0;

u16 DMA1SAD_L = 0;
u16 DMA1SAD_H  = 0;
u16 DMA1DAD_L = 0;
u16 DMA1DAD_H  = 0;

u16 DMA2SAD_L = 0;
u16 DMA2SAD_H  = 0;
u16 DMA2DAD_L = 0;
u16 DMA2DAD_H  = 0;

//DMA SA Timer select : 0 = Timer0 / 1 = Timer1
u8 DMASA_TIMER_SEL = 0;

//DMA SB Timer select : 0 = Timer0 / 1 = Timer1
u8 DMASB_TIMER_SEL = 0;


//debug stuff
vu32 debugsrc1 = 0;
vu32 debugsrc2  = 0;
vu32 debugfr1 = 0;
vu32 debugfr2  = 0;
*/


//DMA SA Timer select : 0 = Timer0 / 1 = Timer1
u8 DMASA_TIMER_SEL = 0;

//DMA SB Timer select : 0 = Timer0 / 1 = Timer1
u8 DMASB_TIMER_SEL = 0;


///////////////////////////////////////// sound code (modified) from nds sseq player /////////////////////////////////////////////////
//sound buffer
volatile u8 waveram[2*0x10];

int pitch=0;
int pitch2=0;
int basefreq=0;
int volume=0;

//Use: Freq (from key2freq BIOS output) from GBA gb channels to GB channels!
int gba_to_gb_sound( int addr )
{
	static const int table [0x40] =
	{
		0xFF10,     0,0xFF11,0xFF12,0xFF13,0xFF14,     0,     0,
		0xFF16,0xFF17,     0,     0,0xFF18,0xFF19,     0,     0,
		0xFF1A,     0,0xFF1B,0xFF1C,0xFF1D,0xFF1E,     0,     0,
		0xFF20,0xFF21,     0,     0,0xFF22,0xFF23,     0,     0,
		0xFF24,0xFF25,     0,     0,0xFF26,     0,     0,     0,
		     0,     0,     0,     0,     0,     0,     0,     0,
		0xFF30,0xFF31,0xFF32,0xFF33,0xFF34,0xFF35,0xFF36,0xFF37,
		0xFF38,0xFF39,0xFF3A,0xFF3B,0xFF3C,0xFF3D,0xFF3E,0xFF3F,
	};
	if ( addr >= 0x60 && addr < 0xA0 )
		return table [addr - 0x60];
	return 0;
}


//GBA sine table
volatile s16 sineTable[256] = {
  (s16)0x0000, (s16)0x0192, (s16)0x0323, (s16)0x04B5, (s16)0x0645, (s16)0x07D5, (s16)0x0964, (s16)0x0AF1,
  (s16)0x0C7C, (s16)0x0E05, (s16)0x0F8C, (s16)0x1111, (s16)0x1294, (s16)0x1413, (s16)0x158F, (s16)0x1708,
  (s16)0x187D, (s16)0x19EF, (s16)0x1B5D, (s16)0x1CC6, (s16)0x1E2B, (s16)0x1F8B, (s16)0x20E7, (s16)0x223D,
  (s16)0x238E, (s16)0x24DA, (s16)0x261F, (s16)0x275F, (s16)0x2899, (s16)0x29CD, (s16)0x2AFA, (s16)0x2C21,
  (s16)0x2D41, (s16)0x2E5A, (s16)0x2F6B, (s16)0x3076, (s16)0x3179, (s16)0x3274, (s16)0x3367, (s16)0x3453,
  (s16)0x3536, (s16)0x3612, (s16)0x36E5, (s16)0x37AF, (s16)0x3871, (s16)0x392A, (s16)0x39DA, (s16)0x3A82,
  (s16)0x3B20, (s16)0x3BB6, (s16)0x3C42, (s16)0x3CC5, (s16)0x3D3E, (s16)0x3DAE, (s16)0x3E14, (s16)0x3E71,
  (s16)0x3EC5, (s16)0x3F0E, (s16)0x3F4E, (s16)0x3F84, (s16)0x3FB1, (s16)0x3FD3, (s16)0x3FEC, (s16)0x3FFB,
  (s16)0x4000, (s16)0x3FFB, (s16)0x3FEC, (s16)0x3FD3, (s16)0x3FB1, (s16)0x3F84, (s16)0x3F4E, (s16)0x3F0E,
  (s16)0x3EC5, (s16)0x3E71, (s16)0x3E14, (s16)0x3DAE, (s16)0x3D3E, (s16)0x3CC5, (s16)0x3C42, (s16)0x3BB6,
  (s16)0x3B20, (s16)0x3A82, (s16)0x39DA, (s16)0x392A, (s16)0x3871, (s16)0x37AF, (s16)0x36E5, (s16)0x3612,
  (s16)0x3536, (s16)0x3453, (s16)0x3367, (s16)0x3274, (s16)0x3179, (s16)0x3076, (s16)0x2F6B, (s16)0x2E5A,
  (s16)0x2D41, (s16)0x2C21, (s16)0x2AFA, (s16)0x29CD, (s16)0x2899, (s16)0x275F, (s16)0x261F, (s16)0x24DA,
  (s16)0x238E, (s16)0x223D, (s16)0x20E7, (s16)0x1F8B, (s16)0x1E2B, (s16)0x1CC6, (s16)0x1B5D, (s16)0x19EF,
  (s16)0x187D, (s16)0x1708, (s16)0x158F, (s16)0x1413, (s16)0x1294, (s16)0x1111, (s16)0x0F8C, (s16)0x0E05,
  (s16)0x0C7C, (s16)0x0AF1, (s16)0x0964, (s16)0x07D5, (s16)0x0645, (s16)0x04B5, (s16)0x0323, (s16)0x0192,
  (s16)0x0000, (s16)0xFE6E, (s16)0xFCDD, (s16)0xFB4B, (s16)0xF9BB, (s16)0xF82B, (s16)0xF69C, (s16)0xF50F,
  (s16)0xF384, (s16)0xF1FB, (s16)0xF074, (s16)0xEEEF, (s16)0xED6C, (s16)0xEBED, (s16)0xEA71, (s16)0xE8F8,
  (s16)0xE783, (s16)0xE611, (s16)0xE4A3, (s16)0xE33A, (s16)0xE1D5, (s16)0xE075, (s16)0xDF19, (s16)0xDDC3,
  (s16)0xDC72, (s16)0xDB26, (s16)0xD9E1, (s16)0xD8A1, (s16)0xD767, (s16)0xD633, (s16)0xD506, (s16)0xD3DF,
  (s16)0xD2BF, (s16)0xD1A6, (s16)0xD095, (s16)0xCF8A, (s16)0xCE87, (s16)0xCD8C, (s16)0xCC99, (s16)0xCBAD,
  (s16)0xCACA, (s16)0xC9EE, (s16)0xC91B, (s16)0xC851, (s16)0xC78F, (s16)0xC6D6, (s16)0xC626, (s16)0xC57E,
  (s16)0xC4E0, (s16)0xC44A, (s16)0xC3BE, (s16)0xC33B, (s16)0xC2C2, (s16)0xC252, (s16)0xC1EC, (s16)0xC18F,
  (s16)0xC13B, (s16)0xC0F2, (s16)0xC0B2, (s16)0xC07C, (s16)0xC04F, (s16)0xC02D, (s16)0xC014, (s16)0xC005,
  (s16)0xC000, (s16)0xC005, (s16)0xC014, (s16)0xC02D, (s16)0xC04F, (s16)0xC07C, (s16)0xC0B2, (s16)0xC0F2,
  (s16)0xC13B, (s16)0xC18F, (s16)0xC1EC, (s16)0xC252, (s16)0xC2C2, (s16)0xC33B, (s16)0xC3BE, (s16)0xC44A,
  (s16)0xC4E0, (s16)0xC57E, (s16)0xC626, (s16)0xC6D6, (s16)0xC78F, (s16)0xC851, (s16)0xC91B, (s16)0xC9EE,
  (s16)0xCACA, (s16)0xCBAD, (s16)0xCC99, (s16)0xCD8C, (s16)0xCE87, (s16)0xCF8A, (s16)0xD095, (s16)0xD1A6,
  (s16)0xD2BF, (s16)0xD3DF, (s16)0xD506, (s16)0xD633, (s16)0xD767, (s16)0xD8A1, (s16)0xD9E1, (s16)0xDB26,
  (s16)0xDC72, (s16)0xDDC3, (s16)0xDF19, (s16)0xE075, (s16)0xE1D5, (s16)0xE33A, (s16)0xE4A3, (s16)0xE611,
  (s16)0xE783, (s16)0xE8F8, (s16)0xEA71, (s16)0xEBED, (s16)0xED6C, (s16)0xEEEF, (s16)0xF074, (s16)0xF1FB,
  (s16)0xF384, (s16)0xF50F, (s16)0xF69C, (s16)0xF82B, (s16)0xF9BB, (s16)0xFB4B, (s16)0xFCDD, (s16)0xFE6E
};


volatile struct NDSCH NDSCH_inst[NDS_HARDWARE_SOUND_DEVICES];
volatile struct SWAVINFO SWAVINFO_inst[NDS_HARDWARE_SOUND_DEVICES];
volatile struct GBAVIRTCH GBAVIRTCH_inst[GBA_VIRTUAL_SOUND_DEVICES];

//coto: returns free channel n or 0x10 if all-busy channel, or 0x11 if channel is already assigned
//you feed gba channels availables and not currently used so it is assigned to you
volatile int assigngbavirt_ch(int req_gbachan){
int i = 0;
	
	for(i=0; i < (int)NDS_HARDWARE_SOUND_DEVICES;i++){
		
        
        if (NDSCH_inst[i].assigned==false) //only queue can enable this
		{
			//need to lockdown chan input 
			switch(req_gbachan){
				case(0):
					//check if channel is not being used otherwise we dont need duplicate ch
					if (GBAVIRTCH_inst[0].assigned == true){
						return 0x11;
					}
					
					else if	(((1<<i) & GBA_VIRTUAL_CH1) > 0 ){ //channel assigned belongs to NDS_PSG
						GBAVIRTCH_inst[0].gba_channel = req_gbachan;
						GBAVIRTCH_inst[0].assigned = true;
						GBAVIRTCH_inst[0].nds_channel_assigned = i;
						
						NDSCH_inst[i].gba_channel = req_gbachan; 	//is always 1
						NDSCH_inst[i].chstat = false;				//channel is not busy but assigned
						NDSCH_inst[i].assigned = true;				//virtual nds channel is assigned
						NDSCH_inst[i].enabled = false;				//nds hw channel is by default disabled , queue enables this
						
						return i;
					}
					//otherwise this channel can't be assigned to gba channel
				break;
				
				case(1):
					//check if channel is not being used otherwise we dont need duplicate ch
					if (GBAVIRTCH_inst[1].assigned == true){
						return 0x11;
					}
					
					else if	(((1<<i) & GBA_VIRTUAL_CH2) > 0 ){ //channel assigned belongs to NDS_PSG
						GBAVIRTCH_inst[1].gba_channel = req_gbachan;
						GBAVIRTCH_inst[1].assigned = true;
						GBAVIRTCH_inst[1].nds_channel_assigned = i;
						
						NDSCH_inst[i].gba_channel = req_gbachan; 	//is always 2
						NDSCH_inst[i].chstat = false;				//channel is not busy but assigned
						NDSCH_inst[i].assigned = true;				//virtual nds channel is assigned
						NDSCH_inst[i].enabled = false;				//nds hw channel is by default disabled
						
						return i;
					}
					//otherwise this channel can't be assigned to gba channel
				break;
				
				case(2):
					//check if channel is not being used otherwise we dont need duplicate ch
					if (GBAVIRTCH_inst[2].assigned == true){
						return 0x11;
					}
					
					else if	(((1<<i) & GBA_VIRTUAL_CH3) > 0 ){ //channel assigned belongs to NDS_PSG
						GBAVIRTCH_inst[2].gba_channel = req_gbachan;
						GBAVIRTCH_inst[2].assigned = true;
						GBAVIRTCH_inst[2].nds_channel_assigned = i;
						
						NDSCH_inst[i].gba_channel = req_gbachan; 	//is always 3
						NDSCH_inst[i].chstat = false;				//channel is not busy but assigned
						NDSCH_inst[i].assigned = true;				//virtual nds channel is assigned
						NDSCH_inst[i].enabled = false;				//nds hw channel is by default disabled
						
						return i;
					}
					//otherwise this channel can't be assigned to gba channel
				break;
				
				case(3):
					//check if channel is not being used otherwise we dont need duplicate ch
					if (GBAVIRTCH_inst[3].assigned == true){
						return 0x11;
					}
					
					else if	(((1<<i) & GBA_VIRTUAL_CH4) > 0 ){ //channel assigned belongs to NDS_PSG
						GBAVIRTCH_inst[3].gba_channel = req_gbachan;
						GBAVIRTCH_inst[3].assigned = true;
						GBAVIRTCH_inst[3].nds_channel_assigned = i;
						
						NDSCH_inst[i].gba_channel = req_gbachan; 	//is always 4
						NDSCH_inst[i].chstat = false;				//channel is not busy but assigned
						NDSCH_inst[i].assigned = true;				//virtual nds channel is assigned
						NDSCH_inst[i].enabled = false;				//nds hw channel is by default disabled
						
						return i;
					}
					//otherwise this channel can't be assigned to gba channel
				break;
				
				case(4):
					//check if channel is not being used otherwise we dont need duplicate ch
					if (GBAVIRTCH_inst[4].assigned == true){
						return 0x11;
					}
					
					else if	(((1<<i) & GBA_VIRTUAL_DMASA) > 0 ){ //channel assigned belongs to NDS DMA DS A
						
						GBAVIRTCH_inst[4].gba_channel = req_gbachan;
						GBAVIRTCH_inst[4].assigned = true;
						GBAVIRTCH_inst[4].nds_channel_assigned = i;
						
						NDSCH_inst[i].gba_channel = req_gbachan; 	//is always dma sa
						NDSCH_inst[i].chstat = false;				//channel is not busy but assigned
						NDSCH_inst[i].assigned = true;				//virtual nds channel is assigned
						NDSCH_inst[i].enabled = false;				//nds hw channel is by default disabled
						
						return i;
					}
					//otherwise this channel can't be assigned to gba channel
				break;
				
				case(5):
					//check if channel is not being used otherwise we dont need duplicate ch
					if (GBAVIRTCH_inst[5].assigned == true){
						return 0x11;
					}
					
					else if	(((1<<i) & GBA_VIRTUAL_DMASB) > 0 ){ //channel assigned belongs to NDS DMA DS B
						GBAVIRTCH_inst[5].gba_channel = req_gbachan;
						GBAVIRTCH_inst[5].assigned = true;
						GBAVIRTCH_inst[5].nds_channel_assigned = i;
						
						NDSCH_inst[i].gba_channel = req_gbachan; 	//is always dma sb
						NDSCH_inst[i].chstat = false;				//channel is not busy but assigned
						NDSCH_inst[i].assigned = true;				//virtual nds channel is assigned
						NDSCH_inst[i].enabled = false;				//nds hw channel is by default disabled
						
					}
					//otherwise this channel can't be assigned to gba channel
				break;
				
			}
		}
	}
return 0x10;
}

inline void init_gbasound_system(){
int i = 0;

	for(i=0;i < (int)GBA_VIRTUAL_SOUND_DEVICES;i++){
			
            GBAVIRTCH_inst[i].assigned = false;
            GBAVIRTCH_inst[i].gba_channel = 0;
            GBAVIRTCH_inst[i].nds_channel_assigned = 0;
	}
    
    for(i=0;i < (int)NDS_HARDWARE_SOUND_DEVICES;i++){
        NDSCH_inst[i].chstat = false;
        NDSCH_inst[i].gba_channel = 0;
        NDSCH_inst[i].mode = 0;
        NDSCH_inst[i].frequency = 0;
        NDSCH_inst[i].amplitude = 0;
        NDSCH_inst[i].panning = 0;
        NDSCH_inst[i].duty = 0;
        NDSCH_inst[i].enabled = false;
        NDSCH_inst[i].assigned = false;
    }

}


//Returns the hardware nds channel used by a GBA Virtual Sound Instance, 0x10 if not assigned
// uses the gbavirtch alloced map

volatile int getgbavirt_ch(int req_gbachan){
int i = 0;

	for(i=0;i<(int)GBA_VIRTUAL_SOUND_DEVICES;i++){
		
        if (
            (GBAVIRTCH_inst[i].gba_channel == req_gbachan)
            &&
            (GBAVIRTCH_inst[i].assigned == true) //only if was allocated previously by assigngbavirt_ch
        ){
            return GBAVIRTCH_inst[i].nds_channel_assigned;
        }
        
	}
return 0x10;
}


u16 volumetable(int volume){
	return (u64)volume*swiGetVolumeTable(volume);
}

// This function was obtained through disassembly of Ninty's sound driver
u16 AdjustFreq(u16 basefreq, int pitch){
	u64 freq;
	int shift = 0;
	pitch = -pitch;
	while (pitch < 0){
		shift --;
		pitch += 0x300;
	}
	while (pitch >= 0x300){
		shift ++;
		pitch -= 0x300;
	}
	freq = (u64)basefreq * ((u32)swiGetPitchTable(pitch) + 0x10000);
	shift -= 16;
	if (shift <= 0)
		freq >>= -shift;
	else if (shift < 32){
		if (freq & ((~0ULL) << (32-shift))) return 0xFFFF;
		freq <<= shift;
	}else
		return 0x10;
	if (freq < 0x10) return 0x10;
	if (freq > 0xFFFF) return 0xFFFF;
	return (u16)freq;
}

//any frequency into  arm7 sound freq
volatile u16 ADJUST_FREQ(u16 basefreq, int noteN, int baseN){
	return AdjustFreq(basefreq, ((noteN - baseN) * 64));
}

volatile u16 ADJUST_PITCH_BEND(u16 basefreq, int pitchb, int pitchr){
	if (!pitchb) return basefreq;
	return AdjustFreq(basefreq, (pitchb*pitchr) >> 1);
}

//coto: this driver is the one you use to reserve sound properties on your sound calls.
//GBA virtual channel assignment (one way add)
//this either creates or updates current playback
volatile u32 queue_gbavirt_instr(int new_gbach,int freq, int volume, int pan, int psgduty, int mode, int ndsch_enabled){
    
    //Returns the hardware nds channel used by a GBA Virtual Sound Instance, 0x10 if not assigned
	int ndsvirtch_assigned = getgbavirt_ch(new_gbach); 
	
	//if gba is free, continue (creates)
	if(ndsvirtch_assigned == 0x10){
		
		int new_ndsvirtch_assigned=assigngbavirt_ch(new_gbach);
		
		//allocate a new nds channel for new gba virt allocat
		if (new_ndsvirtch_assigned <= (int)NDS_HARDWARE_SOUND_DEVICES){
			
			//populate chan n objects then ..
			//but first we validate each GBA Virtual instrument to match NDS Hardware Sound Channels
			
			NDSCH_inst[new_ndsvirtch_assigned].mode = mode;				//NDS Hardware channel mode
			NDSCH_inst[new_ndsvirtch_assigned].frequency = freq;		//frequency of wave
			NDSCH_inst[new_ndsvirtch_assigned].amplitude = volume;		//volume of wave
			NDSCH_inst[new_ndsvirtch_assigned].panning = pan;			//volume balance per output
			NDSCH_inst[new_ndsvirtch_assigned].duty = psgduty;			//psg duty (only nds hardware)
			NDSCH_inst[new_ndsvirtch_assigned].enabled = ndsch_enabled;				//set to play instrument (external toggle)
			NDSCH_inst[new_ndsvirtch_assigned].chstat = false;			//only play_instr triggers this to enable (internal)
			
			//assigngbavirt_ch() only creates, does not update current gbavirtch instance, this does
			NDSCH_inst[new_ndsvirtch_assigned].gba_channel = new_gbach;	//gbachannel alloc
			GBAVIRTCH_inst[new_gbach].gba_channel = new_gbach;
			GBAVIRTCH_inst[new_gbach].assigned = true;
			GBAVIRTCH_inst[new_gbach].nds_channel_assigned = new_ndsvirtch_assigned;
			
			return 0x0;
		}
		else
			return 0x1; //failed to allocate a GBA channel
	}
	//(updates)
	else if (ndsvirtch_assigned <= (int)NDS_HARDWARE_SOUND_DEVICES){
		NDSCH_inst[ndsvirtch_assigned].mode = mode;				//NDS Hardware channel mode
		NDSCH_inst[ndsvirtch_assigned].frequency = freq;		//frequency of wave
		NDSCH_inst[ndsvirtch_assigned].amplitude = volume;		//volume of wave
		NDSCH_inst[ndsvirtch_assigned].panning = pan;			//volume balance per output
		NDSCH_inst[ndsvirtch_assigned].duty = psgduty;			//psg duty (only nds hardware)
		NDSCH_inst[ndsvirtch_assigned].enabled = ndsch_enabled;				//set to play instrument (external toggle)
		NDSCH_inst[ndsvirtch_assigned].chstat = false; 			//only play_instr triggers this to enable (internal)
		
		//assigngbavirt_ch() only creates, does not update current gbavirtch instance, this does
		NDSCH_inst[ndsvirtch_assigned].gba_channel = new_gbach;	//gbachannel alloc
		GBAVIRTCH_inst[new_gbach].gba_channel = new_gbach;
		GBAVIRTCH_inst[new_gbach].assigned = true;
		GBAVIRTCH_inst[new_gbach].nds_channel_assigned = ndsvirtch_assigned;

		return 0x2; //updated
	}
	
	else
		return 0x3; //(not gba ch record found based off gba ch input) and nds ch are full
}


volatile int deallocvirtch(int req_gbachan){
 
//only dealloc channel if was queued previously
if(
    GBAVIRTCH_inst[req_gbachan].assigned == true
    
){

//only dealloc if nds channel actually belongs to virtual gba ch
if(NDSCH_inst[GBAVIRTCH_inst[req_gbachan].nds_channel_assigned].gba_channel == req_gbachan){
    
    //cleanup virtual channels
    NDSCH_inst[GBAVIRTCH_inst[req_gbachan].nds_channel_assigned].gba_channel = 0;
    NDSCH_inst[GBAVIRTCH_inst[req_gbachan].nds_channel_assigned].enabled = false; 	    //bool
    NDSCH_inst[GBAVIRTCH_inst[req_gbachan].nds_channel_assigned].assigned = false; 	//bool
}

    GBAVIRTCH_inst[req_gbachan].gba_channel = 0;
    GBAVIRTCH_inst[req_gbachan].nds_channel_assigned = 0;
    GBAVIRTCH_inst[req_gbachan].assigned = false; //bool
}
	
return 0;
}

//cur GBA Virtual Channel
int current_instrument_play=0;

//plays any GBA instrument that is set to enable and it's not playing
volatile u32 play_instr(){
extern int current_instrument_play; 										//cur GBA Virtual Channel

int this_nds_channel = getgbavirt_ch(current_instrument_play); //works

    //is it available and is assigned?
	if( 
        (this_nds_channel < 0x10)
    )   
    {
		
        if(GBAVIRTCH_inst[current_instrument_play].assigned == true){ //only play channels enabled through queue_gbavirt_instr
        
            //if channel 1
            if(
                (current_instrument_play == 0)
            &&
                (NDSCH_inst[this_nds_channel].chstat == false)
            )
            {
                //sound is busy playing
                NDSCH_inst[this_nds_channel].chstat = true;
                
                
                //add here math
                /*
                int freq_input_tot = NDSCH_inst[this_nds_channel].frequency; //because the frequency of playinstr() is of 33mhz 
                //C 3	36(midi)	44(gb)	65.406(hz) == 1,48 hz per gb step
                double hertz_gb_per_step = (65.406/44);	
                
                double frametime = (1/60);
                
                int hertz_counter = 0;
                int vblank_frequency = 33513982 / frametime ; //in cpu cycles : 569737,6 hz
                int gba_psg_driven_frequency_onvblank = (33513982/2) * vblank_frequency; //29,4hz on vblank: we need just 8
                
			
                //coto (psg mode)
                playsound(
                    //AdjustFreq(u16 basefreq, int pitch)
                    //ADJUST_FREQ(u16 basefreq, int noteN, int baseN)
                    //(131072/(2048-freq_input_tot)
                    //gba_to_gb_sound( int addr )
				
                	AdjustFreq((33513982)/(131072),2048-freq_input_tot), // 131072/(2048-n)
                	this_nds_channel,
                	NDSCH_inst[this_nds_channel].amplitude,
                	NDSCH_inst[this_nds_channel].panning,
                	NDSCH_inst[this_nds_channel].duty,
                	NDSCH_inst[this_nds_channel].mode,
                	1
                );
                */
                //adpcm mode
			
                //WAVE_SRC wav;
                //IMA_STATE enc, dec;
                //FILE *outfp, *codefp;
	
                //WAVOUT_SPECS specs;
                //unsigned char header[44];

                //specs.len = wav.chunk_left / wav.fmt.frame_size;
                //specs.sample_rate = wav.fmt.sample_rate;
                //specs.sample_width = 2;
                //specs.channels = 1;

                //wavout_make_header(header, &specs);
            
                //init_encode_ima9(&enc, 0);		//set to index 0
                //init_encode_ima9(&dec, enc.min_index);
			
                //clear_bins(); //cleanup
                //wav.chunk_left = 128000;
			
                //while(wav.chunk_left > 0)
                //{
                //	int size_wave_bytes_buffer = 0;
                //	int i;
                //	signed short samples[size_wave_bytes_buffer];
                //	unsigned char ima[32];
                //	signed short samples_out[size_wave_bytes_buffer];

				// read samples
                //	for(i = 0; i < size_wave_bytes_buffer; i++){
					//samples[i] = get_next_wav_sample(&wav); //grab adsr fm psg data here
                //	}
				
				// compress and decompress
				//encode_ima(&enc, ima, samples, size_wave_bytes_buffer);		//stuck here
				//ori: fwrite(ima, 1, sizeof(ima), codefp);
				//add_nibbles_to_bins(ima, 64);				//works
				//decode_ima(&dec, samples_out, ima, 64);	//stuck here
				
				
				// write samples
                //	for(i = 0; i < size_wave_bytes_buffer; i++)
                //	{
                //		int yn = samples_out[i];
					//ori: fputc(yn & 0xff, outfp);
					//ori: fputc(yn >> 8, outfp);
					
					//playsound here ima pcm format yn
                //	}
			
                //wav.chunk_left--;
                //}
  
                //write_bins();
			
                NDSCH_inst[this_nds_channel].chstat = false;
            }
		
            //else if channel 2
            else if(current_instrument_play == 1){
            
            }
		
            //else if channel 3
            else if(current_instrument_play == 2)
            {
            
            }
            //else  channel 4
            else if(current_instrument_play == 3)
            {
            
            }
            
            
            //handled by timer interrupts channel :DMA SA / DMA SB
            //deallocvirtch(current_instrument_play); 		//delete instrument already played / deletes channels ok
            
        }
        
    }

	//coto : play_instr() is executing ok
	
	//end loop
	if(current_instrument_play < (int)GBA_VIRTUAL_SOUND_DEVICES){
		current_instrument_play++;   //next instrument
	}
	else{
		current_instrument_play = 0; //reset instrument
	}
	

    return 0;
}

// PCM8  0<<29 // PCM16 1<<29 // IMA-ADPCM 2<<29 // PSGNOISE 3<<29
// starting with channel 0 at 4000400h..400040Fh, up to channel 15 at 40004F0h..40004FFh.
inline int playsound(int freq,int channel,int volume,int pan,int psgduty,int mode,int enable){
	
	
//PSG Wave Duty (channel 8..13 in PSG mode)
//Each duty cycle consists of eight HIGH or LOW samples, so the sound frequency is 1/8th of the selected sample rate. The duty cycle always starts at the begin of the LOW period when the sound gets (re-)started.
//  0  12.5% "_______-_______-_______-"
//  1  25.0% "______--______--______--"
//  2  37.5% "_____---_____---_____---"
//  3  50.0% "____----____----____----"
//  4  62.5% "___-----___-----___-----"
//  5  75.0% "__------__------__------"
//  6  87.5% "_-------_-------_-------"
//  7   0.0% "________________________"
//The Wave Duty bits exist and are read/write-able on all channels (although they are actually used only in PSG mode on channels 8-13).

//PSG Noise (channel 14..15 in PSG mode)
//Noise randomly switches between HIGH and LOW samples, the output levels are calculated, at the selected sample rate, as such:
//  X=X SHR 1, IF carry THEN Out=LOW, X=X XOR 6000h ELSE Out=HIGH
//The initial value when (re-)starting the sound is X=7FFFh. The formula is more or less same as "15bit polynomial counter" used on 8bit Gameboy and GBA.
	
	
	//40004x0h - NDS7 - SOUNDxCNT - Sound Channel X Control Register (R/W)
	//Bit0-6    Volume Mul   (0..127=silent..loud)
	//Bit7      Not used     (always zero)
	//Bit8-9    Volume Div   (0=Normal, 1=Div2, 2=Div4, 3=Div16)
	//Bit10-14  Not used     (always zero)
	//Bit15     Hold         (0=Normal, 1=Hold last sample after one-shot sound)
	//Bit16-22  Panning      (0..127=left..right) (64=half volume on both speakers)
	//Bit23     Not used     (always zero)
	//Bit24-26  Wave Duty    (0..7) ;HIGH=(N+1)*12.5%, LOW=(7-N)*12.5% (PSG only)
	//Bit27-28  Repeat Mode  (0=Manual, 1=Loop Infinite, 2=One-Shot, 3=Prohibited)
	//Bit29-30  Format       (0=PCM8, 1=PCM16, 2=IMA-ADPCM, 3=PSG/Noise)
	//Bit31     Start/Status (0=Stop, 1=Start/Busy)
	
	//ori: stru32inlasm(0x4000400+(channel<<4),0x0,volume<<0|pan<<16|psgduty<<24|mode<<29|enable<<31);
	*(u32*)(0x4000400+(channel<<4))=(volume<<0|pan<<16|psgduty<<24|mode<<29|enable<<31);
	
	//40004x4h - NDS7 - SOUNDxSAD - Sound Channel X Data Source Register (W)
	//Bit0-26  Source Address (must be word aligned, bit0-1 are always zero)
	//Bit27-31 Not used

	//40004x8h - NDS7 - SOUNDxTMR - Sound Channel X Timer Register (W)
	//Bit0-15  Timer Value, Sample frequency, timerval=-(33513982/2)/freq
	//The PSG Duty Cycles are composed of eight "samples", and so, the frequency for Rectangular Wave is 1/8th of the selected sample frequency.
	//For PSG Noise, the noise frequency is equal to the sample frequency.
	
	//int modified_freq = freq;
	
	//C 3	36(midi)	44(gb)	65.406(hz) == 1,48 hz per gb step
	//double hertz_gb_per_step = 65.406/44;
	
	/*
	//it is one of psg channels
	if( ((1 << channel) & NDS_PSG_CHANNELS) > 0){
		*(u16*)(0x4000408+(channel<<4))=(-(33513982/2)/modified_freq)/hertz_gb_per_step;
	}
	else{
		modified_freq = (freq);
	}
	*/
	
	
	//ADJUST_FREQ(u16 basefreq, int noteN, int baseN)
	//ori: *(u16*)(0x4000408+(channel<<4))=( (s16)(-(33513982/2)/modified_freq) * (-1) / 100000 ); // -16756991 / 4 (psg) = 4189247 / 100000 = 41,89
	
	
	/* //works
	if(freq > 0){
		if( ((1 << channel) & NDS_PSG_CHANNELS) > 0){
			SendArm9Command(0x4700BEEF,modified_freq,0x0,0x0); //
		}
	}
	*/
	
	//frequency validation per hardware
	//it is one of psg channels
    /*
	if	( ((1 << channel) & NDS_PSG_CHANNELS) > 0){
		*(u16*)(0x4000408+(channel<<4))=freq;
	}
	//it is one of whitenoise channels
	else if	( ((1 << channel) & NDS_WNOISE_CHANNELS) > 0){
	
	}
	//it is one of adpcm channels (reserved by driver, because we know all channels are adpcm but we cant include those because those belong to above channels)
	else if( ((1 << channel) & NDS_ADPCM_PCM_CHANNELS) > 0){
	
	}
	*/
	return 0;
}
/////////////////////////////////// sound code from nds9 seq player end /////////////////////////////////////////////////

//ARM7 Sound IO <- GBA Sound IO
inline void write_sound_command(u32 addr,u32 val)
{
    /*
	switch(addr)
	{	  
        case 0xBC:
            DMA1SAD_L = val;
		break;
		case 0xBE:
            DMA1SAD_H = val & 0x0FFF;
		break;
		case 0xC0:
            DMA1DAD_L = val;
			checkstart();
		break;
		case 0xC2:
            DMA1DAD_H = val & 0x07FF;
			checkstart();
		break;
		case 0xC4:
            //DMA1CNT_L = val & 0x3FFF;
		break;
		case 0xC6:
            DMA1CNT_H = val;
			checkstart();
		break;
		case 0xC8:
            DMA2SAD_L = val;
		break;
		case 0xCA:
            DMA2SAD_H = val & 0x0FFF;
		break;
		case 0xCC:
            DMA2DAD_L = val;
			checkstart();
		break;
		case 0xCE:
            DMA2DAD_H = val & 0x07FF;
			checkstart();
		case 0xD0:
            //DMA2CNT_L = val & 0x3FFF;
		break;
		case 0xD2:
            DMA2CNT_H = val;
			checkstart();
		break;
        
		case 0x100:
            TM0CNT_L = val;
			updatetakt();
		case 0x102:
            TM0CNT_H = val;
			updatetakt();
			checkstart();
		case 0x104:
            TM1CNT_L = val;
			updatetakt();
		case 0x106:
            TM1CNT_H = val;
			updatetakt();
			checkstart();
            
        //4000080h - SOUNDCNT_L (NR50, NR51) - Channel L/R Volume/Enable (R/W)
        //Bit   Expl.
        //0-2   Sound 1-4 Master Volume RIGHT (0-7)
        //3     Not used
        //4-6   Sound 1-4 Master Volume LEFT (0-7)
        //7     Not used
        //8-11  Sound 1-4 Enable Flags RIGHT (each Bit 8-11, 0=Disable, 1=Enable)
        //12-15 Sound 1-4 Enable Flags LEFT (each Bit 12-15, 0=Disable, 1=Enable)
		case 0x80:
            SOUNDCNT_L = val;
			updatevol();
		break;
        
        //4000082h - SOUNDCNT_H (GBA only) - DMA Sound Control/Mixing (R/W)
        //  Bit   Expl.
        //  0-1   Sound # 1-4 Volume   (0=25%, 1=50%, 2=100%, 3=Prohibited)
        //  2     DMA Sound A Volume   (0=50%, 1=100%)
        //  3     DMA Sound B Volume   (0=50%, 1=100%)
        //  4-7   Not used
        //  8     DMA Sound A Enable RIGHT (0=Disable, 1=Enable)
        //  9     DMA Sound A Enable LEFT  (0=Disable, 1=Enable)
        //  10    DMA Sound A Timer Select (0=Timer 0, 1=Timer 1)
        //  11    DMA Sound A Reset FIFO   (1=Reset)
        //  12    DMA Sound B Enable RIGHT (0=Disable, 1=Enable)
        //  13    DMA Sound B Enable LEFT  (0=Disable, 1=Enable)
        //  14    DMA Sound B Timer Select (0=Timer 0, 1=Timer 1)
        //  15    DMA Sound B Reset FIFO   (1=Reset)
		case 0x82:
            SOUNDCNT_H = val; //Reset FIFO is not needed because we don't have direct streaming yet so don't need that
			updatevol();
            
			if(val & BIT(10))
			{
                DMASA_TIMER_SEL = 1;
			}
			else
			{
                DMASA_TIMER_SEL = 0;
			}
			if(val & BIT(14))
			{
                DMASB_TIMER_SEL = 1;
			}
			else
			{
                DMASB_TIMER_SEL = 0;
			}
			updatetakt();
			checkstart();
		break;
		case 0x84:
            if(val & 0x80)REG_SOUNDCNT |= 0x8000;
			else
			{
                REG_SOUNDCNT &= ~0x8000;
			}
		break;
		case 0x88:
            //Amplitude Resolution/Sampling Cycle is not supported so only Bias
			//it is better on the DS any way
			REG_SOUNDBIAS = val;
		break;
        
        //installs dmabuffer between ARM9 & ARM7
		case 0x1FFFFFFA:
        {
            arm7dmaexchange_buf = (u8*)(u32)val;                          
			soundbuffA = (u8*)(u32)(arm7dmaexchange_buf + 0x0);               //ori: (u8*)(u32*)(dmabuffer);
			SCHANNEL_SOURCE(4) = (u32)soundbuffA;
			soundbuffB = (u8*)(u32)(arm7dmaexchange_buf + 0x50);               //ori: (u8*)(u32*)(dmabuffer + 0x50);
			SCHANNEL_SOURCE(5) = (u32)soundbuffB;
            
        }   
		break;
		case 0x1FFFFFFB: //wait
			{
                //REG_IPC_FIFO_TX = 0xc0700106; //0x4100BEEF; //trigger a frameasync() manually from BIOS call
                //SendArm9Command(0xc0700106,0x0,0x0,0x0);     //causes to jump to entrypoint sometimes
            }
		break;
		case 0x1FFFFFFC: //setauto
            autodetectdetect = true;
		break;
		case 0x1FFFFFFD: //getkeys
		{
            
		}
		break;
		case 0x1FFFFFFF: //set callline
            callline = val;
		break;
		default:
            //senddebug32(0x7FFFFFFF); //error
			
            //if(addr > 0x400)  //arm9 -> arm7 con crushed try to get at last some infos
			//{
            //    swiWaitForVBlank();
			//	swiWaitForVBlank();
			//	senddebug32(addr);
			//	senddebug32(addr);
			//	senddebug32(addr);
			//	senddebug32(addr);
			//	senddebug32(addr);
			//	senddebug32(addr);
			//	senddebug32(addr);
			//	senddebug32(addr);
			//	senddebug32(addr);
			//	while(1);
			//}
			//break;
        */
        
    //coto
    
    switch(addr)
	{	    
		//channel 1 sweep
		case 0x60:{
			val&=0x7f;
			
			
		}
		break;
		
		//u32 queue_gbavirt_instr(int new_gbach,int freq, int volume, int pan, int psgduty, int mode, int ndsch_enabled);
		
		//channel 1 tone SOUND1CNT_H (NR11, NR12) - Channel 1 Duty/Len/Envelope (R/W)
		case 0x62:{
			//15    W    Initial      (1=Restart Sound)
			//12-15 R/W  Initial Volume of envelope          (1-15, 0=No Sound)
					val&=0xffff;
					int this_gbachannel=0;	//put the GBA Channel nº here
					int alloced_ndsch_ind=0; 	//virtual gbach index (aka the nds hw ch reserved) (that is allocated by assigngbavirt_ch)
	
					//if new ch get nds ch indx
					alloced_ndsch_ind = getgbavirt_ch(this_gbachannel); //fixed function
					if(alloced_ndsch_ind == 0x10){
						alloced_ndsch_ind = assigngbavirt_ch(this_gbachannel); //fixed function
					}
			
					//this must happen always
					if (alloced_ndsch_ind <= (int)NDS_HARDWARE_SOUND_DEVICES){
						
						//waveduty gba #2 -> nds #3
						
						//gba duty table
						//Wave Duty:
						//0: 12.5% ( -_______-_______-_______ )
						//1: 25%   ( --______--______--______ )
						//2: 50%   ( ----____----____----____ ) (normal)
						//3: 75%   ( ------__------__------__ )
						
						//nds channel 8--13 duty table
						//0  12.5% "_______-_______-_______-"
						//1  25.0% "______--______--______--"
						//2  37.5% "_____---_____---_____---"
						//3  50.0% "____----____----____----"
						//4  62.5% "___-----___-----___-----"
						//5  75.0% "__------__------__------"
						//6  87.5% "_-------_-------_-------"
						//7   0.0% "________________________"
						
						//define duty
						u8 gbaduty = ((val >> 6) & 0x3);
						
						if(gbaduty == 0){
							gbaduty = 0;
						}
						else if(gbaduty == 1){
							gbaduty = 1;
						}
						else if(gbaduty == 2){
							gbaduty = 3;
						}
						else if(gbaduty == 3){
							gbaduty = 5;
						}
						else{
							gbaduty =0;
						}
						
						u8 panning = 64;
						
						//gbaio writes -> ndsio writes
						NDSCH_inst[alloced_ndsch_ind].amplitude = (val&0x7ff);
						NDSCH_inst[alloced_ndsch_ind].panning = panning;
						NDSCH_inst[alloced_ndsch_ind].duty = gbaduty;
						NDSCH_inst[alloced_ndsch_ind].mode = 3;
						NDSCH_inst[alloced_ndsch_ind].enabled = 1;
						
                        
						queue_gbavirt_instr(
							this_gbachannel,						// new or existing (nds created) gbachannel
							NDSCH_inst[alloced_ndsch_ind].frequency, 	//freq
							NDSCH_inst[alloced_ndsch_ind].amplitude, 	//volume
							NDSCH_inst[alloced_ndsch_ind].panning, 		//pan
							NDSCH_inst[alloced_ndsch_ind].duty, 			//psgduty
							NDSCH_inst[alloced_ndsch_ind].mode, 			//mode
							NDSCH_inst[alloced_ndsch_ind].enabled		//nds ch enabled 1==yes / 0 == no
						);
						
						//debug: SendArm9Command(0x4700BEEF,queue_value,0x0,0x0); //update ok
						
					}
					
					//channel required is not allocated yet
					else{
						//break;
					}
					
		}
		break;
		
		//channel 1 tone : SOUND1CNT_X (NR13, NR14) - Channel 1 Frequency/Control (R/W)
		case 0x64:{
		
		}
		break;
		
        
        case 0x82:{
            GBAEMU4DS_IPC->SOUNDCNT_H = val;
			
			if(!(val & (1<<10)))
			{
                DMASA_TIMER_SEL = 0;
			}
			else
			{
                DMASA_TIMER_SEL = 1;
			}
			
            if(!(val & (1<<14)))
			{
                DMASB_TIMER_SEL = 0;
			}
			else
			{
                DMASB_TIMER_SEL = 1;
			}
            
            //Reset FIFO
            if(val & (1<<11))
			{
                GBAEMU4DS_IPC->FIFO_A_L[0]=0;
                GBAEMU4DS_IPC->FIFO_A_L[1]=0;
                GBAEMU4DS_IPC->FIFO_A_H[0]=0;
                GBAEMU4DS_IPC->FIFO_A_H[1]=0;
                
            }
            
            if(val & (1<<15))
			{
                GBAEMU4DS_IPC->FIFO_B_L[0]=0;
                GBAEMU4DS_IPC->FIFO_B_L[1]=0;
                GBAEMU4DS_IPC->FIFO_B_H[0]=0;
                GBAEMU4DS_IPC->FIFO_B_H[1]=0;
                
            }
		}
		break;
		case 0x84:{
            
            if(val & 0x80)
                REG_SOUNDCNT |= 0x8000;
			else
                REG_SOUNDCNT &= ~0x8000;
        }
		break;
		
        
        //SOUND FIFO IO
        case 0xa0:{
            GBAEMU4DS_IPC->FIFO_A_L[0] = (val)&0xff;
            GBAEMU4DS_IPC->FIFO_A_L[1] = (val>>8)&0xff;
		}
		break;
		case 0xa2:{
            GBAEMU4DS_IPC->FIFO_A_H[0] = (val)&0xff;
            GBAEMU4DS_IPC->FIFO_A_H[1] = (val>>8)&0xff;
		}
		break;
		case 0xa4:{
            GBAEMU4DS_IPC->FIFO_B_L[0] = (val)&0xff;
            GBAEMU4DS_IPC->FIFO_B_L[1] = (val>>8)&0xff;
            
		}
		break;
		case 0xa6:{
            GBAEMU4DS_IPC->FIFO_B_H[0] = (val)&0xff;
            GBAEMU4DS_IPC->FIFO_B_H[1] = (val>>8)&0xff;
		}
		break;
		
		
        //Timers
        
        case 0x100:{
            //Reload/Counter val
            
            GBAEMU4DS_IPC->timer0Reload = (u16)val;
            
            if(DMASA_TIMER_SEL==0)
                SCHANNEL_TIMER(4) = GBAEMU4DS_IPC->timer0Reload;
            if(DMASB_TIMER_SEL==0)
                SCHANNEL_TIMER(5) = GBAEMU4DS_IPC->timer0Reload;
            
        }
        break;
        
        case 0x102:{
            //Timer Controller
            GBAEMU4DS_IPC->TM0CNT = (u16)val;
            
            if(GBAEMU4DS_IPC->TM0CNT & 0x80) //started timer
			{
                if(!(TIMER0_CR & TIMER_ENABLE))
                    timerStart(0, ClockDivider_1,GBAEMU4DS_IPC->TM0CNT << 5, timer0interrupt_thread);
				
			}
            
            //timerOnOffDelay|=1;
            //cpuNextEvent = cpuTotalTicks;            
        }
        break;
        case 0x104:{
            //Reload/Counter val
            GBAEMU4DS_IPC->timer1Reload = (u16)val;
            
            if(DMASA_TIMER_SEL==1)
                SCHANNEL_TIMER(4) = GBAEMU4DS_IPC->timer1Reload;
            if(DMASB_TIMER_SEL==1)
                SCHANNEL_TIMER(5) = GBAEMU4DS_IPC->timer1Reload;
            
        }
        break;
        case 0x106:
        {   
            //Timer Controller
            GBAEMU4DS_IPC->TM1CNT = (u16)val;
            
            //GBAEMU4DS_IPC->timer1Value = value;
            //timerOnOffDelay|=2;
            //cpuNextEvent = cpuTotalTicks;
            
            /*if(timer1Reload & 0x8000)
            {
                if((value & 0x3) == 0)
                {
                    *(u16 *)(0x4000104) = timer1Reload >> 5;
                    *(u16 *)(0x4000106) = value + 1;
                    break;
                }
                if((value & 0x3) == 1)
                {
                    *(u16 *)(0x4000104) = timer1Reload >> 1;
                    *(u16 *)(0x4000106) = value + 1;
                    break;
                }
                if((value & 3) == 2)
                {
                    *(u16 *)(0x4000104) = timer1Reload >> 1;
                    *(u16 *)(0x4000106) = value + 1;
                    break;
                }
                *(u16 *)(0x4000106) = value;
                iprintf("big reload1\r\n");//todo 
            }
            else*/
            
            //*(u16*)0x04000106 = (u16)val;
            
        }    
        break;
        case 0x108:
        {
            //Reload/Counter val
            //*(u16*)0x04000108 = (u16)val;
            //GBAEMU4DS_IPC->timer2Reload= (u16)val;
        }    
        break;
        case 0x10A:
        {
            //Timer Controller
            //*(u16*)0x0400010A = (u16)val;
            //GBAEMU4DS_IPC->TM2CNT = (u16)val;
            
            //timerOnOffDelay|=4;
            //cpuNextEvent = cpuTotalTicks;
            
            /*if(GBAEMU4DS_IPC->timer2Reload & 0x8000)
            {
                if((value & 0x3) == 0)
                {
                    *(u16 *)(0x4000108) = GBAEMU4DS_IPC->timer2Reload >> 5;
                    *(u16 *)(0x400010A) = value + 1;
                    break;
                }
                if((value & 0x3) == 1)
                {
                    *(u16 *)(0x4000108) = GBAEMU4DS_IPC->timer2Reload >> 1;
                    *(u16 *)(0x400010A) = value + 1;
                    break;
                }
                if((value & 3) == 2)
                {
                    *(u16 *)(0x4000108) = GBAEMU4DS_IPC->timer2Reload >> 1;
                    *(u16 *)(0x400010A) = value + 1;
                    break;
                }
                iprintf("big reload2\r\n");//todo 
                *(u16 *)(0x400010A) = value;
            }
            else*/
            
            //*(u16*)0x0400010A = (u16)val;
        }
        break;
        case 0x10C:
        {
            //Reload/Counter val
            //*(u16*)0x0400010C = (u16)val;
            //GBAEMU4DS_IPC->timer3Reload=(u16)val;
        }
        break;
        case 0x10E:
        {
            //Timer Controller
            //*(u16*)0x0400010E = (u16)val;
            //GBAEMU4DS_IPC->TM3CNT = (u16)val;
            
            //GBAEMU4DS_IPC->timer3Value = value;
            //timerOnOffDelay|=8;
            //cpuNextEvent = cpuTotalTicks;
            
            /*if(GBAEMU4DS_IPC->timer3Reload & 0x8000)
            {
                if((value & 0x3) == 0)
                {
                    *(u16 *)(0x400010C) = GBAEMU4DS_IPC->timer3Reload >> 5;
                    *(u16 *)(0x400010E) = value + 1;
                    break;
                }
                if((value & 0x3) == 1)
                {
                    *(u16 *)(0x400010C) = GBAEMU4DS_IPC->timer3Reload >> 1;
                    *(u16 *)(0x400010E) = value + 1;
                    break;
                }
                if((value & 3) == 2)
                {
                    *(u16 *)(0x400010C) = GBAEMU4DS_IPC->timer3Reload >> 1;
                    *(u16 *)(0x400010E) = value + 1;
                    break;
                }
                iprintf("big reload3\r\n");//todo 
                *(u16 *)(0x400010E) = value;
            }
            else*/
        }
        break;
        
        
        //bias is handled at IPC level (ipc_arm7.cpp)
		case 0x88:
			swiChangeSoundBias(1,val);
		break;
	}
}

///////////////////////////////////////////SOUND END////////////////////////////////////////////////////////////////////////////////////

bool ykeypp;
u32 interrupts_to_wait_arm7 = 0;

//---------------------------------------------------------------------------------
int main() {
//---------------------------------------------------------------------------------
	ledBlink(0);
	readUserSettings();
	interrupts_to_wait_arm7 = 0;
	
	extern bool ykeypp;
	
	ykeypp = false;
	
	// Start the RTC tracking IRQ / not needed now
	//ORI: initClockIRQ();
	
	enableSound();
    init_gbasound_system();
    
	irqInit();
	fifoInit();
    
	REG_IPC_SYNC = 0;
    REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_SEND_CLEAR | IPC_FIFO_RECV_IRQ;
    REG_IPC_FIFO_CR |= (1<<2);  //send empty irq
    REG_IPC_FIFO_CR |= (1<<10); //recv empty irq
	
	//NDS DMA SA CH
    SCHANNEL_REPEAT_POINT(4) = 0;
	SCHANNEL_LENGTH(4) = FIFO_DMA_SIZE;
    //NDS DMA SB CH
	SCHANNEL_REPEAT_POINT(5) = 0;
	SCHANNEL_LENGTH(5) = FIFO_DMA_SIZE;
    
    //4000500h - NDS7 - SOUNDCNT - Sound Control Register (R/W)
    //Bit0-6   Master Volume       (0..127=silent..loud)
    //Bit7     Not used            (always zero)
    //Bit15    Master Enable       (0=Disable, 1=Enable)
    u8 master_vol = 90;
    *(u16*)0x04000500 = (master_vol<<0) | (1<<15);
    
	//ORI: irqSet
	irqSet(IRQ_HBLANK, 			(void*)hblank_handler);					//when HBLANK time
	irqSet(IRQ_VBLANK, 			(void*)vblank_handler);					//when VBLANK time
	irqSet(IRQ_VCOUNT, 			(void*)vcount_handler);					//when VCOUNTER time
	irqSet(IRQ_FIFO_NOT_EMPTY, 	(void*)handlefifo);						//when FIFO IS NOT EMPTY time
	irqSet(IRQ_LID, 			(void*)lid_open_irq_handler);			//when opening the LID of DS time
    irqSet(IRQ_TIMER0, 			(void*)timer0interrupt_thread);	    
    
	//IRQ_NETWORK is because we use a modified DSWIFI
    interrupts_to_wait_arm7 = (IRQ_HBLANK | IRQ_VBLANK | IRQ_VCOUNT | IRQ_LID | IRQ_FIFO_NOT_EMPTY | IRQ_NETWORK);
	
	installWifiFIFO(); //rewritten a bit
	
    irqEnable(interrupts_to_wait_arm7);
	
	//enable fifo send recv
	*(u16*)0x04000184 = *(u16*)0x04000184 | (1<<15); 

    //set up ppu: do irq on hblank/vblank/vcount/and vcount line is programable (later)
    REG_DISPSTAT = REG_DISPSTAT | DISP_HBLANK_IRQ |  DISP_VBLANK_IRQ | DISP_YTRIGGER_IRQ; //| (VCOUNT_LINE_INTERRUPT << 15);
	
	REG_IF = ~0;
    REG_IME = 1;
    
    while (1) {
		swiIntrWait(1,interrupts_to_wait_arm7);
	}
    
	return 0;
}


//Saving battery power...
inline void hblank_handler(){
extern bool ykeypp;

    //hblank
    SendArm9Command(0xc0700107,0x0,0x0,0x0);
    
	//REG_IF = IRQ_HBLANK;
}


inline void vblank_handler(){
	extern bool ykeypp;
    
	//our driver that executes queued sounds (vblank)
	//play_instr(); //causes freezes

	//NDS9 VBLANK & Wifi_Sync();
	SendArm9Command(0xc0700103,0x0,0x0,0x0); ////0x4800BEEF
	
	Wifi_Update();	//can cause lockups
	
	//REG_IF = IRQ_VBLANK;
}

u32 power=0;
u32 ie_save=0;

//ok
inline void vcount_handler(){

extern bool ykeypp;
	
	//save battery
	if(!(REG_KEYXY & 0x2))
	{
		if(!ykeypp)
		{
			SendArm9Command(0xc0700100,0x0,0x0,0x0);
			ykeypp = true;
		}
	}
	else
	{
		ykeypp = false;
	}
    
	//Press X (pause menu)
	if (!(REG_KEYXY & KEY_XARM7)){
		//REG_IPC_FIFO_TX = 0x4300BEEF;
		SendArm9Command(0xc0700101,0x0,0x0,0x0); //0x4300BEEF pause menu
	}
	
	//Press Y (save battery func)
	if (!(REG_KEYXY & KEY_YARM7)){
		//REG_IPC_FIFO_TX = 0x4200BEEF;
		SendArm9Command(0xc0700100,0x0,0x0,0x0);
	}
	
	//Normally Close HINGE circuit is pressed (folded)
	if(REG_KEYXY & KEY_HINGE)
	{
		//Safe sleepmode GBA ARM9 & ARM7
		SendArm9Command(0xc0700104,0x0,0x0,0x0);
	}
	
    /*
	//read input only once per frame
    if( (REG_VCOUNT&0xff) & callline)
	{
		SendArm9Command(0xc0700102,REG_KEYINPUT,REG_KEYXY,0x0);
        
        //then frame async
        //SendArm9Command(0xc0700110,0x0,0x0,0x0);
	}
    */
	
//IPC ARM7/ARM9 process: handles touchscreen,time,etc
gbaemu4ds_ipc();

//vcount arm7
SendArm9Command(0xc0700106,0x0,0x0,0x0);

//REG_IF = IRQ_VCOUNT;
}

//ok
inline void lid_open_irq_handler(){
	extern u32 power;
	extern u32 ie_save;
	
	//100ms
	swiDelay(838000);
	// Restore the interrupt state.
	REG_IE = ie_save;
	// Restore power state.
	writePowerManagement(PM_CONTROL_REG, power);
	// Turn the speaker up.
	if (REG_POWERCNT & 1) swiChangeSoundBias(1,0x400);
    
	//REG_IF = IRQ_LID //is it hw toggled? (physical lid)
}

inline void lid_closing_handler(u32 WAKEUP_IRQS){
	extern u32 power;
	extern u32 ie_save;
	
	ie_save = REG_IE;
	// Turn the speaker down.
	if (REG_POWERCNT & 1) swiChangeSoundBias(0,0x400);
	// Save current power state.
	power = readPowerManagement(PM_CONTROL_REG);
	// Set sleep LED.
	writePowerManagement(PM_CONTROL_REG, PM_LED_CONTROL(1));
	// Enable IRQ_LID on interrupt vectors
	REG_IE = WAKEUP_IRQS;
	// Power down till we get our interrupt.
	swiSleep(); //waits for PM (lid open) interrupt
	
	//REG_IF = IRQ_LID //is it hw toggled? (physical lid)
}



//wifihander in wireless/wifi_arm7.c


//there are updated automatically by timers
u8 dmasa_buffer[0x4];
u8 dmasb_buffer[0x4];

bool dmasa_play;
bool dmasb_play;


void playdmafifo(){
    
    if((dmasa_play == true) && !(SCHANNEL_CR(4) & 0x80000000)){
        dmasa_buffer[0]=GBAEMU4DS_IPC->FIFO_A_L[0];
        dmasa_buffer[1]=GBAEMU4DS_IPC->FIFO_A_L[1];
        dmasa_buffer[2]=GBAEMU4DS_IPC->FIFO_A_H[0];
        dmasa_buffer[3]=GBAEMU4DS_IPC->FIFO_A_H[1];
        
        SCHANNEL_SOURCE(4) = (u32)&dmasa_buffer[0];
        SCHANNEL_CR(4) |= 0x80000000;   //play
        
    }
    
    
    //Updates async DMA FIFOs (good!)
    if(!(SCHANNEL_CR(4) & 0x80000000)){
        dmasa_play = false;
    }
}

//Timer 0 will drive DMA Play
//TIMER0 IRQ: //40000C6h - DMA1CNT_H - DMA 1 Control (R/W)
void timer0interrupt_thread(){

    //Timer0 for DMASoundA?
    if(DMASA_TIMER_SEL == 0){

        if((GBAEMU4DS_IPC->IF & IRQ_DMA1)  && ((GBAEMU4DS_IPC->IE & IRQ_DMA1))){
            
            //if DMA1 enabled: play
            if(((GBAEMU4DS_IPC->DM1CNT_H>>15)&1)==1){
                
                //PLAY HERE FIFO
                //DMA Fifo
                dmasa_play = true;
                playdmafifo();  
                
                //Repeat Bit
                if(((GBAEMU4DS_IPC->DM1CNT_H>>9)&1)==1){
                    GBAEMU4DS_IPC->DM1CNT_H |= (1<<15); //DMA1 Enabled!
                    SCHANNEL_CR(4) |= SOUND_REPEAT;
                }
                else{
                    GBAEMU4DS_IPC->DM1CNT_H |= ~(1<<15); //DMA1 Disabled!
                    SCHANNEL_CR(4) &= ~SOUND_REPEAT;
                }
                
                //Serve DMA1
                GBAEMU4DS_IPC->IF ^= IRQ_DMA1;
                SendArm9Command(0xc4740005,0x202,GBAEMU4DS_IPC->IF,0x0);
            }
        }
        else{
                //if DMA1 enabled: execute
                if(((GBAEMU4DS_IPC->DM1CNT_H>>15)&1)==1){
                    //Start Timing: FIFO
                    if(((GBAEMU4DS_IPC->DM1CNT_H>>12)&3)==3){
                        //Values should be written in TIMER CNT / DMA so far
                        SendArm9Command(0xc4740004,0x00000003,0x00000002,0x0);  //CPUCheckDMA(3, 2);     //Serve DMA1 FIFO
                    }
                }
        }
    }
    
    
    //Timer0 for DMASoundB?
    if(DMASB_TIMER_SEL == 0){
    
        if((GBAEMU4DS_IPC->IF & IRQ_DMA2)  && ((GBAEMU4DS_IPC->IE & IRQ_DMA2))){
            
            //if DMA2 enabled:play
            if(((GBAEMU4DS_IPC->DM2CNT_H>>15)&1)==1){        
                
                //PLAY HERE FIFO
                SCHANNEL_CR(5) = (SCHANNEL_CR(5) & ~0xFF) | ((( 1 + ((GBAEMU4DS_IPC->SOUNDCNT_H & 0x8) >> 3))*((GBAEMU4DS_IPC->SOUNDCNT_H & BIT(12)) >> 12) + ( 1 + ((GBAEMU4DS_IPC->SOUNDCNT_H & 0x8) >> 3))*((GBAEMU4DS_IPC->SOUNDCNT_H & BIT(13)) >> 13))*31); //max:124
                
                if(!(SCHANNEL_CR(5) & 0x80000000))
                    SCHANNEL_CR(5) |= 0x80000000 |SOUND_REPEAT; //start now
                    
                //Repeat Bit
                if(((GBAEMU4DS_IPC->DM2CNT_H>>9)&1)==1){
                    GBAEMU4DS_IPC->DM2CNT_H |= (1<<15); //DMA2 Enabled!
                }
                else{
                    GBAEMU4DS_IPC->DM2CNT_H |= ~(1<<15); //DMA2 Disabled!
                }
                
                //Serve DMA2
                GBAEMU4DS_IPC->IF ^= IRQ_DMA2;
                SendArm9Command(0xc4740005,0x202,GBAEMU4DS_IPC->IF,0x0);
            }
        }
        else{
            //if DMA2 enabled: execute
            if(((GBAEMU4DS_IPC->DM2CNT_H>>15)&1)==1){
                //Start Timing: FIFO
                if(((GBAEMU4DS_IPC->DM2CNT_H>>12)&3)==3){
                    //Values should be written in TIMER CNT / DMA so far
                    SendArm9Command(0xc4740004,0x00000003,0x00000004,0x0);  //CPUCheckDMA(3, 4)l   //Serve DMA2 FIFO
                }
            }
        }
    }
    
    //SendArm9Command(0xc4740006,0x0,0x0,0x0); //timer0 
    
    //REG_IF = IRQ_TIMER0;    ////timer0 irqs work! not needed since the tick irq is timed
}

void timer1interrupt_thread(){
    
    //Timer1 for DMASoundA?
    if(DMASA_TIMER_SEL == 1){
    }
    
    //Timer1 for DMASoundB?
    if(DMASB_TIMER_SEL == 1){
    }
    
}