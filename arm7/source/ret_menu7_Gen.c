/***********************************************************
	Arm7 Soft rest for General purpose

		by Rudolph (çcíÈ)
***************************************************************/

#include <nds.h>
//#include <nds/registers_alt.h>	// devkitPror20
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define	ARM7_PROG	(0x03810000 - 0xA00)
typedef void (* FN_MEDIUM_ARM7)(void);
FN_MEDIUM_ARM7	_menu7_Gen;

extern	void	_menu7_Gen_s();

void ret_menu7_Gen()
{
	u32	*adr;
	u32	*buf;
	u32	i;

	while((*(vu32*)0x027FFDFC) != 0x027FFDF8) {	// Timing adjustment with ARM9
		vu32 w;
		for(w=0;w<0x100;w++){
		}
	}

	REG_IME = IME_DISABLE;	// Disable interrupts
	REG_IF = REG_IF;	// Acknowledge interrupt
	
//	REG_IME = 0;

	for (i = 0x04000400; i < 0x04000500; i+=4) {
		*((u32*)i) = 0;
	}
	SOUND_CR = 0;

	for(i = 0x040000B0; i < (0x040000B0+0x30); i+=4) {
		*((vu32*)i) = 0;
	}
	for(i = 0x04000100; i < 0x04000110; i+=2) {
		*((u16*)i) = 0;
	}

//switch to user mode
	asm("mov	r0, #0x1F");
	asm("msr	cpsr, r0");


	adr = (u32*)ARM7_PROG;
	buf = (u32*)_menu7_Gen_s;
	for(i = 0; i < 0x200/4; i++) {
		*adr = *buf;
		adr++;
		buf++;
	}

	_menu7_Gen = (FN_MEDIUM_ARM7)ARM7_PROG;
	_menu7_Gen();

	while(1);

}


void _menu7_Gen_s()
{
	u32	*adr;
	u32	*bufh, *buf7, *buf9;
	u32	siz;
	u32	i;
	u32	*arm9s, *arm9e;
	u32	*arm7s, *arm7e;


	bufh = (u32*)(*(vu32*)0x027FFDF4);

	adr = (u32*)0x027FFE00;
	for(i = 0; i < 512/4; i++) {		// Header
		*adr = *bufh;
		adr++;
		bufh++;
	}

	buf9 = bufh;
	buf7 = buf9 + ((*(vu32*)0x027FFE2C) / 4);


	adr = (u32*)(*(vu32*)0x027FFE38);
	siz = (*(vu32*)0x027FFE3C);
	for(i = 0; i < siz/4; i++) {		// ARM7
		*adr = *buf7;
		adr++;
		buf7++;
	}
	arm7e = adr;


	adr = (u32*)(*(vu32*)0x027FFE28);
	siz = (*(vu32*)0x027FFE2C);
	if(adr < buf9) {			// ARM9
		for(i = 0; i < siz/4; i++) {
			*adr = *buf9;
			adr++;
			buf9++;
		}
		arm9e = adr;
	} else {
		adr += (siz/4 - 1);
		buf9 += (siz/4 - 1);
		arm9e = adr + 1;
		for(i = 0; i < siz/4; i++) {
			*adr = *buf9;
			adr--;
			buf9--;
		}
	}


	arm7s = (u32*)(*(vu32*)0x027FFE38);
	if(arm7s > (u32*)0x023FF800)
		arm7s = (u32*)0x023FF800;
	arm9s = (u32*)(*(vu32*)0x027FFE28);
	if(arm9s > arm7s) {
		adr = arm9s;
		arm9s = arm7s;
		arm7s = adr;
		adr = arm9e;
		arm9e = arm7e;
		arm7e = adr;
	}

	adr = (u32*)0x02000000;
	while(adr < arm9s) {
		*adr = 0x00000000;
		adr++;
	}

	while(arm9e < arm7s) {
		*arm9e = 0x00000000;
		arm9e++;
	}

	while(arm7e < (u32*)0x023FF800) {
		*arm7e = 0x00000000;
		arm7e++;
	}

	REG_IE = 0;
	REG_IF = ~0;
	(*(vu32*)(0x04000000-4)) = 0;  //IRQ_HANDLER ARM7 version
	(*(vu32*)(0x04000000-8)) = ~0; //VBLANK_INTR_WAIT_FLAGS, ARM7 version
	REG_POWERCNT = 1;  //turn off power to stuffs

	*(vu32*)0x027FFDFC = *(vu32*)0x027FFE24;
	asm("swi 0x00");			// JUMP 0x027FFE34

	while(1);
//	swiSoftReset();
}
