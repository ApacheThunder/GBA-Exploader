/*---------------------------------------------------------------------------------

	default ARM7 core

		Copyright (C) 2005 - 2010
		Michael Noland (joat)
		Jason Rogers (dovoto)
		Dave Murphy (WinterMute)

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any
	damages arising from the use of this software.

	Permission is granted to anyone to use this software for any
	purpose, including commercial applications, and to alter it and
	redistribute it freely, subject to the following restrictions:

	1.	The origin of this software must not be misrepresented; you
		must not claim that you wrote the original software. If you use
		this software in a product, an acknowledgment in the product
		documentation would be appreciated but is not required.

	2.	Altered source versions must be plainly marked as such, and
		must not be misrepresented as being the original software.

	3.	This notice may not be removed or altered from any source
		distribution.

---------------------------------------------------------------------------------*/
#include <nds.h>
#include <stdlib.h>
#include <string.h>

#include "linkreset_arm7.h"

volatile bool switchedMode = false;

extern	void	ret_menu7_R4(void);
extern	void	ret_menu7_Gen(void);
extern	void	ret_menu7_mse(void);

void gbaMode() {
	vu32	vr;

	REG_IME = IME_DISABLE;
	for(vr = 0; vr < 0x1000; vr++);	// Wait ARM9

	if (((*(vu32*)0x027FFCE4 >> 3) & 0x01) == 0x01) {
		writePowerManagement(0, PM_BACKLIGHT_BOTTOM | PM_SOUND_AMP);
	} else { 
		writePowerManagement(0, PM_BACKLIGHT_TOP | PM_SOUND_AMP);
	}

	swiSwitchToGBAMode();
//	asm("mov    r2, #0x40");
//	asm("swi    0x1F0000");

	while(1);
}

static void prepairReset() {
	vu32	vr;
	u32	i;

	powerOn(POWER_SOUND);

	for(i = 0x040000B0; i < (0x040000B0+0x30); i+=4)*((vu32*)i) = 0;

	REG_IME = IME_DISABLE;
	REG_IE = 0;
	REG_IF = ~0;

	for(vr = 0; vr < 0x100; vr++);	// Wait ARM9

	swiSoftReset();
}

volatile bool exitflag = false;

void powerButtonCB() { exitflag = true; }

void fifoCheckHandler() {
	if (!switchedMode) {
		if(fifoCheckValue32(FIFO_USER_01)) {
			switchedMode = true;
			gbaMode();
		} else if (fifoCheckValue32(FIFO_USER_02)) {
			switchedMode = true;
			ret_menu7_R4();
		} else if (fifoCheckValue32(FIFO_USER_03)) {
			switchedMode = true;
			LinkReset_ARM7();
		} else if (fifoCheckValue32(FIFO_USER_04)) {
			switchedMode = true;
			ret_menu7_Gen();
		} else if (fifoCheckValue32(FIFO_USER_05)) {
			switchedMode = true;
			prepairReset();
		}			
	}
	
	
	/*u32	fifo;
	if(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)) {
		fifo = REG_IPC_FIFO_RX;

		if(fifo == IPC_CMD_GBAMODE)gbaMode();
		if(fifo == IPC_CMD_SLOT2)prepairReset();
		if(fifo == IPC_CMD_TURNOFF) {
			PM_SetControl(1<<6);
			while(1);
		}
		if(fifo == IPC_CMD_SR_R4TF)ret_menu7_R4();
		if(fifo == IPC_CMD_SR_DLMS)LinkReset_ARM7();
		if(fifo == IPC_CMD_SR_GEN)ret_menu7_Gen();
		if(fifo == IPC_CMD_SR_MSE)ret_menu7_mse();
	}*/
}

void VblankHandler(void) {
	fifoCheckHandler();
	// Wifi_Update();
}


void VcountHandler() { inputGetAndSend(); }

int main() {
	readUserSettings();
	ledBlink(0);
		
	irqInit();
	// Start the RTC tracking IRQ
	initClockIRQ();
	fifoInit();
	touchInit();

	SetYtrigger(80);

	installSystemFIFO();
	
	irqSet(IRQ_VCOUNT, VcountHandler);
	irqSet(IRQ_VBLANK, VblankHandler);
	
	irqEnable(IRQ_VBLANK | IRQ_VCOUNT);
	
	/*if (REG_SNDEXTCNT != 0) {
		i2cWriteRegister(0x4A, 0x12, 0x00);	// Press power-button for auto-reset
		i2cWriteRegister(0x4A, 0x70, 0x01);	// Bootflag = Warmboot/SkipHealthSafety
	}*/

	setPowerButtonCB(powerButtonCB);

	// Keep the ARM7 mostly idle
	while(1)swiWaitForVBlank();
	return 0;
}

