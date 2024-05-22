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

volatile bool switchedMode = false;

// libnds messed up the original SWI bios call ASM.
// They used r0 instead of r2. This reimplementation fixes that issue for now.
extern void swiSwitchToGBAModeFixed();

static void prepareReset() {
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


void gbaMode() {
	vu32	vr;

	REG_IME = IME_DISABLE;
	for(vr = 0; vr < 0x1000; vr++);	// Wait ARM9

	if (PersonalData->gbaScreen) {
		writePowerManagement(PM_CONTROL_REG, PM_BACKLIGHT_BOTTOM | PM_SOUND_AMP);
	} else {
		writePowerManagement(PM_CONTROL_REG, PM_BACKLIGHT_TOP | PM_SOUND_AMP);
	}
	swiSwitchToGBAModeFixed();
	while(1);
}

volatile bool exitflag = false;

void powerButtonCB() { exitflag = true; }

void fifoCheckHandler() {
	if (!switchedMode) {
		if (fifoCheckValue32(FIFO_USER_01)) {
			switchedMode = true;
			gbaMode();
		} else if (fifoCheckValue32(FIFO_USER_02)) {
			switchedMode = true;
			prepareReset();
		}
	}
}

void VblankHandler() { fifoCheckHandler(); }
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
	
	setPowerButtonCB(powerButtonCB);

	// Keep the ARM7 mostly idle
	while(1)swiWaitForVBlank();
	return 0;
}

