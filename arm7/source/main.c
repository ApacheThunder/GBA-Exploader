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


volatile bool exitflag = false;

void powerButtonCB() { exitflag = true; }

void VblankHandler(void) { 
	// Wifi_Update();
}


void VcountHandler() { inputGetAndSend(); }

int main() {
	readUserSettings();
	ledBlink(0);
	bool switchedMode = false;
	
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
	while(1) {
		if(fifoCheckValue32(FIFO_USER_01) && !switchedMode) {
			switchedMode = true;
			gbaMode();
		}
		swiWaitForVBlank();
	}
	
	return 0;
}

