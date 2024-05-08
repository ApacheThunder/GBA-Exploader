/***********************************************************
	Arm7 Soft rest for reset.mse

		by Rudolph (çcíÈ)
***************************************************************/

#include <nds.h>
//#include <nds/registers_alt.h>	// devkitPror20
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SOUND_CR			REG_SOUNDCNT

void ret_menu7_mse() {
	u32	i;

	while(*((vu32*)0x027FFDFC) != 0x06000000) {	// Timing adjustment with ARM9
		vu32 w;
		for(w=0;w<0x100;w++);
	}

	REG_IME = IME_DISABLE;	// Disable interrupts
	REG_IF = REG_IF;	// Acknowledge interrupt


	for (i = 0x04000400; i < 0x04000500; i+=4)*((u32*)i) = 0;
	
	SOUND_CR = 0;
	
	for(i = 0x040000B0; i < (0x040000B0+0x30); i+=4)*((vu32*)i) = 0;
	for(i = 0x04000100; i < 0x04000110; i+=2)*((u16*)i) = 0;

//switch to user mode
	asm("mov r0, #0x1F");
	asm("msr cpsr, r0");


	REG_IE = 0;
	REG_IF = ~0;
	(*(vu32*)(0x04000000-4)) = 0;		//IRQ_HANDLER ARM7 version
	(*(vu32*)(0x04000000-8)) = ~0;		//VBLANK_INTR_WAIT_FLAGS, ARM7 version
	REG_POWERCNT = 1;				//turn off power to stuffs

	*((vu32*)0x027FFE34) = *((vu32*)0x027FFDFC);	// Bootloader start address
//	asm("swi 0x00");			// JUMP 0x027FFE34
    swiSoftReset();
	while(1);
}

