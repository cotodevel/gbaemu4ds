#include "ipc_arm7.h"

#include <nds.h>
#include <nds/arm7/audio.h>
#include <nds/arm7/serial.h>

#include "../wireless/wifi_arm7.h"

//Shared memory region static object
#include "../../common/clock.h"
#include "../../common/gba_ipc.h"
#include "../../common/wifi_shared.h"

//sound commands
#include "../main.h"

//arm7 opcodes
#include "../arm7opcodes.h"



//arm7 callback that receives FIFO messages. Must be set on HBLANK interrupts
volatile void handlefifo(){
	
	while(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)){
		volatile u32 value0 = REG_IPC_FIFO_RX;	//we already have arg0 otherwise this wouldnt be triggered
		volatile u32 value1 = 0;
		volatile u32 value2 = 0;
		volatile u32 value3 = 0;
		
		if (!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)){
			value1 = REG_IPC_FIFO_RX;
		}
		
		if (!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)){
			value2 = REG_IPC_FIFO_RX;
		}
		
		if (!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)){
			value3 = REG_IPC_FIFO_RX;
		}
		
		//iprintf("arm9: IRQ not empty received value: %x ! \n",(unsigned int)value0);

		//if there is a cmd to be executed..
		ipc7_handler(value0,value1,value2,value3);
		continue; //required: keep reading commands from the IPC RX Vector
	}
	
	REG_IF = IRQ_FIFO_NOT_EMPTY;
}


volatile void ipc7_handler(u32 command1,u32 command2,u32 command3,u32 command4)
{

	//##command list
	//0xc070xxxx	 = GBA sound command write
	//0xc171xxxx	 = WIFI write
	//0xc272xxxx	 = machine specific commands
	
	//GBA sound cmd
	if(command1 == 0xc0700100){
	
        //execute GBA ARM7 SOUND command
		write_sound_command(command2,command3);
        //SendArm9Command(0x0,0x0,0x0,0x0); //raise an interrupt always
	}
	
	if(command1 == 0xc0700101){
		//execute NDS ARM7 WIFI command
	}
	
	//raise sleepmode from arm9 with custom IRQs
	if(command1 == 0xc0700102){ //0x1FFFFFF0
		lid_closing_handler(command2);
	}
	
	//sleepmode bouncer (ARM9 is caller, ARM7 is callee)
	if(command1 == 0xc0700103){ //0x1FFFFFF1
		SendArm9Command(0xc0700104,0x0,0x0,0x0);
	}
	
    //arm9 is safe GBA sleep mode / ARM7 is soundbias
    if(command1 == 0xc0700104){
        swi0x19((u16)command2,(u16)command3); //gba uses a fixed delay count of 8
    }
	
    //debug
    if(command1 == 0xc0700105){
        SendArm9Command(0xc0700105,(u32)getgbavirt_ch(current_instrument_play),0x0,0x0);
    }
	
    //press X button from ARM9 /value0 == 0 no auto press continue, value0 == 1 auto press continue
    if(command1 == 0xc0700106){
        SendArm9Command(0xc0700101,0x0,0x0,0x0);
    }
    
    
	//WIFI cmds
	
	//arm9 wants to WifiSync
	if(command1 == 0xc1710100){
		Wifi_Sync();
	}
	
    //problem is here
	//arm9 wants to send a WIFI context block address / userdata is always zero here
	if(command1 == 0xc1710101){
		//	wifiAddressHandler( void * address, void * userdata )
		wifiAddressHandler((Wifi_MainStruct *)(u32)command2, 0);
	}
	
	//plain FIFO interrupt on both ARM Cores
	if(command1 == 0xc3730100){
		SendArm9Command(0xc3730101,0x0,0x0,0x0);
	}
	
    //acknowledge event, does nothing but to receive the above interrupt
    if(command1 == 0xc3730101){
		
	}
    
    //raises arm7_synctoarm9(); (sync WIFI stacks with ARM9)
    if(command1 == 0xc3730102){
		SendArm9Command(0xc3730103,0x0,0x0,0x0);
	}
    
    //receives the signal for syncing WIFI stacks from the other ARM core
    if(command1 == 0xc3730103){
        arm7_synctoarm9();
	}
	
	
	//misc arm7 utils
	
	//set ppu's vcounter irq
	if(command1 == 0xc4740100){
		set_ppu_vcountline_irq(command2);	//u8 vcount_line_irq
	}
}