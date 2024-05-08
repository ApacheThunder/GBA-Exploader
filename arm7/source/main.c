#include <nds.h>
#include <stdlib.h>
#include <string.h>

#include "linkreset_arm7.h"


extern	void	ret_menu7_R4(void);
extern	void	ret_menu7_Gen(void);
extern	void	ret_menu7_mse(void);


#define IPC_CMD_GBAMODE  1
#define IPC_CMD_SLOT2	 2
#define IPC_CMD_TURNOFF  9
#define IPC_CMD_SR_R4TF 11
#define IPC_CMD_SR_DLMS 12
#define IPC_CMD_SR_GEN	13
#define IPC_CMD_SR_MSE	14


void FIFOInit()
{
	REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_SEND_CLEAR;
}


static void prepairReset()
{
	vu32	vr;
	u32	i;

	powerON(POWER_SOUND);

	for(i = 0x040000B0; i < (0x040000B0+0x30); i+=4) {
		*((vu32*)i) = 0;
	}

	REG_IME = IME_DISABLE;
	REG_IE = 0;
	REG_IF = ~0;

	for(vr = 0; vr < 0x100; vr++);	// Wait ARM9

	swiSoftReset();
}


void gbaMode() 
{
	vu32	vr;

	REG_IME = IME_DISABLE;
	for(vr = 0; vr < 0x1000; vr++);	// Wait ARM9

	if (((*(vu32*)0x027FFCE4 >> 3) & 0x01) == 0x01)
		writePowerManagement(0, PM_BACKLIGHT_BOTTOM | PM_SOUND_AMP);
	else	writePowerManagement(0, PM_BACKLIGHT_TOP | PM_SOUND_AMP);

	swiSwitchToGBAMode();
//	asm("mov    r2, #0x40");
//	asm("swi    0x1F0000");

	while(1);
} 

__attribute__((noinline)) u8 PM_GetRegister(int reg)
{

//	while(REG_SPICNT & SPI_BUSY)
//		SWI_WaitByLoop(1);
	SerialWaitBusy();
 
	REG_SPICNT = SPI_ENABLE | SPI_DEVICE_POWER |SPI_BAUD_1MHz | SPI_CONTINUOUS;
	REG_SPIDATA = reg | 0x80;
 
	SerialWaitBusy();
//	while(REG_SPICNT & SPI_BUSY)
//		SWI_WaitByLoop(1);
 
	REG_SPICNT = SPI_ENABLE | SPI_DEVICE_POWER |SPI_BAUD_1MHz ;
	REG_SPIDATA = 0;
 
	SerialWaitBusy();
//	while(REG_SPICNT & SPI_BUSY)
//		SWI_WaitByLoop(1);
 

	return REG_SPIDATA & 0xff;
}

__attribute__((noinline)) void PM_SetRegister(int reg, int control)
{

	SerialWaitBusy();
//	while(REG_SPICNT & SPI_BUSY)
//		SWI_WaitByLoop(1);
 
	REG_SPICNT = SPI_ENABLE | SPI_DEVICE_POWER | SPI_BAUD_1MHz | SPI_CONTINUOUS;
	REG_SPIDATA = reg;
 
	SerialWaitBusy();
//	while(REG_SPICNT & SPI_BUSY)
//		SWI_WaitByLoop(1);
 
	REG_SPICNT = SPI_ENABLE | SPI_DEVICE_POWER | SPI_BAUD_1MHz;
	REG_SPIDATA = control;


}

void PM_SetControl(int control)
{
	PM_SetRegister(0, PM_GetRegister(0) | control);
}


//---------------------------------------------------------------------------------
void startSound(int sampleRate, const void* data, u32 bytes, u8 channel, u8 vol,  u8 pan, u8 format) {
//---------------------------------------------------------------------------------
	SCHANNEL_TIMER(channel)  = SOUND_FREQ(sampleRate);
	SCHANNEL_SOURCE(channel) = (u32)data;
	SCHANNEL_LENGTH(channel) = bytes >> 2 ;
	SCHANNEL_CR(channel)     = SCHANNEL_ENABLE | SOUND_ONE_SHOT | SOUND_VOL(vol) | SOUND_PAN(pan) | (format==1?SOUND_8BIT:SOUND_16BIT);
}


//---------------------------------------------------------------------------------
s32 getFreeSoundChannel() {
//---------------------------------------------------------------------------------
	int i;
	for (i=0; i<16; i++) {
		if ( (SCHANNEL_CR(i) & SCHANNEL_ENABLE) == 0 ) return i;
	}
	return -1;
}

touchPosition first,tempPos;

//---------------------------------------------------------------------------------
void VcountHandler() {
//---------------------------------------------------------------------------------
	static int lastbut = -1;
	
	uint16 but=0, x=0, y=0, xpx=0, ypx=0, z1=0, z2=0;

	but = REG_KEYXY;

	if (!( (but ^ lastbut) & (1<<6))) {
 
		tempPos = touchReadXY();

		if ( tempPos.x == 0 || tempPos.y == 0 ) {
			but |= (1 <<6);
			lastbut = but;
		} else {
			x = tempPos.x;
			y = tempPos.y;
			xpx = tempPos.px;
			ypx = tempPos.py;
			z1 = tempPos.z1;
			z2 = tempPos.z2;
		}
		
	} else {
		lastbut = but;
		but |= (1 <<6);
	}

	IPC->touchX			= x;
	IPC->touchY			= y;
	IPC->touchXpx		= xpx;
	IPC->touchYpx		= ypx;
	IPC->touchZ1		= z1;
	IPC->touchZ2		= z2;
	IPC->buttons		= but;

}

/***
#define READ_TIME_AND_DATE	0x65
//---------------------------------------------------------------------------------
void rtcGetTimeAndDate(uint8 * time) {
//---------------------------------------------------------------------------------
	uint8 command, status;

	command = READ_TIME_AND_DATE;
	rtcTransaction(&command, 1, time, 7);

	command = READ_STATUS_REG1;
	rtcTransaction(&command, 1, &status, 1);

	if ( status & STATUS_24HRS ) {
		time[4] &= 0x3f;
	} else {

	}
	BCDToInteger(time,7);
}
**/

//---------------------------------------------------------------------------------
void VblankHandler(void) {
//---------------------------------------------------------------------------------

	u32	fifo;

	if(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)) {
		fifo = REG_IPC_FIFO_RX;

		if(fifo == IPC_CMD_GBAMODE)
			gbaMode();
		if(fifo == IPC_CMD_SLOT2) {
			prepairReset();
		}
		if(fifo == IPC_CMD_TURNOFF) {
			PM_SetControl(1<<6);
			while(1);
		}

		if(fifo == IPC_CMD_SR_R4TF)
			ret_menu7_R4();
		if(fifo == IPC_CMD_SR_DLMS)
			LinkReset_ARM7();
		if(fifo == IPC_CMD_SR_GEN)
			ret_menu7_Gen();
		if(fifo == IPC_CMD_SR_MSE)
			ret_menu7_mse();
	}


	u32 i;

	//sound code  :)
	TransferSound *snd = IPC->soundData;
	IPC->soundData = 0;

	if (0 != snd) {

		for (i=0; i<snd->count; i++) {
			s32 chan = getFreeSoundChannel();

			if (chan >= 0) {
				startSound(snd->data[i].rate, snd->data[i].data, snd->data[i].len, chan, snd->data[i].vol, snd->data[i].pan, snd->data[i].format);
			}
		}
	}


	uint8 ct[sizeof(IPC->time.curtime)];
	int t1, t2;

	rtcGetTimeAndDate((uint8 *)ct);
	IPC->time.rtc.year = ct[0];
	IPC->time.rtc.month = ct[1];
	IPC->time.rtc.day = ct[2];
	IPC->time.rtc.weekday = ct[3];
	IPC->time.rtc.hours = (ct[4]<12) ? ct[4] : ct[4]+40;
	IPC->time.rtc.minutes = ct[5];
	IPC->time.rtc.seconds = ct[6];

	IPC->temperature = touchReadTemperature(&t1, &t2);
	IPC->tdiode1 = t1;
	IPC->tdiode2 = t2;

}

//---------------------------------------------------------------------------------
int main(int argc, char ** argv) {
//---------------------------------------------------------------------------------

//	memset((void*)IPC, 0, sizeof(IPC));
	
//	rtcReset();
	FIFOInit();

	//enable sound
	powerON(POWER_SOUND);
	SOUND_CR = SOUND_ENABLE | SOUND_VOL(0x7F);
	IPC->soundData = 0;

	irqInit();

	// Start the RTC tracking IRQ
	initClockIRQ();

	SetYtrigger(80);
	irqSet(IRQ_VBLANK, VblankHandler);
	irqSet(IRQ_VCOUNT, VcountHandler);

	irqEnable(IRQ_VBLANK | IRQ_VCOUNT);

	while(1) {
		swiWaitForVBlank();
	}
}

