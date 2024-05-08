/***********************************************************

	by Rudolph (çcíÈ)
		2007/05/24 First release
		2007/05/27 Timing adjustment with ARM9

------------------------------------------------------------
  SoftwareReset Routines for R4DS or M3SimplyDS.

 Redistribution and use in source and binary forms,
 with or without modification, are permitted provided 
 that the following conditions are met:

  Only the Homebrew application can be used. 
  It is not possible to use it by a business purpose. 

  This software is made based on information obtained 
  by Reverse engineering. 

  Please use that at once when a source code that is 
  more formal than the official is open to the public.
***************************************************************/

#include <nds.h>
//#include <nds/registers_alt.h>	// devkitPror20
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CARD_COMMAND	REG_CARD_COMMAND
#define	CARD_CR1H		REG_AUXSPICNTH
#define	CARD_CR2		REG_ROMCTRL
#define	CARD_DATA_RD	REG_CARD_DATA_RD

static int _set_r4menu()
{
	u32	add;

	add = (*(vu32*)0x027FFE18);

	while(CARD_CR2 & CARD_BUSY);

	CARD_CR1H = 0xC0;
	CARD_COMMAND[0] = 0xB4;
	CARD_COMMAND[1] = (add >> 24) & 0xFF;
	CARD_COMMAND[2] = (add >> 16) & 0xFF;
	CARD_COMMAND[3] = (add >> 8) & 0xFF;
	CARD_COMMAND[4] = add & 0xFF;
//	CARD_COMMAND[5] = 0x00;
//	CARD_COMMAND[6] = 0x00;
//	CARD_COMMAND[7] = 0x00;

	CARD_CR2 = 0xA7586000;
	while(!(CARD_CR2 & CARD_DATA_READY));

	return(CARD_DATA_RD);
}


static int _read_r4menu(char *buf, u32 blk)
{
	int	s = 0;
	u32	*buf32;

	buf32 = (u32*)buf;
	blk *= 2;
	do {
		while(CARD_CR2 & CARD_BUSY);
		CARD_CR1H = 0xC0;
		CARD_COMMAND[0] = 0xB6;
		CARD_COMMAND[1] = (blk >> 16) & 0xFF;
		CARD_COMMAND[2] = (blk >> 8) & 0xFF;
		CARD_COMMAND[3] = blk & 0xFF;
		CARD_COMMAND[4] = 0x00;
//		CARD_COMMAND[5] = 0x00;
//		CARD_COMMAND[6] = 0x00;
//		CARD_COMMAND[7] = 0x00;
		CARD_CR2 = 0xA7586000;
		while(!(CARD_CR2 & CARD_DATA_READY));
	} while(CARD_DATA_RD);

	while(CARD_CR2 & CARD_BUSY);
	CARD_CR1H = 0xC0;
	CARD_COMMAND[0] = 0xBF;
	CARD_COMMAND[1] = (blk >> 16) & 0xFF;
	CARD_COMMAND[2] = (blk >> 8) & 0xFF;
	CARD_COMMAND[3] = blk & 0xFF;
	CARD_COMMAND[4] = 0x00;
//	CARD_COMMAND[5] = 0x00;
//	CARD_COMMAND[6] = 0x00;
//	CARD_COMMAND[7] = 0x00;
	CARD_CR2 = 0xA1586000;

	do {
		while(!(CARD_CR2 & CARD_DATA_READY));
		*buf32 = CARD_DATA_RD;
		buf32++;
		s += 4;
	} while(CARD_CR2 & CARD_BUSY);

	return(s);
}


void ret_menu7_R4()
{
	char	*adr;
	u32	blk, siz;
	u32	i;
	u32	*mem;

	REG_IME = 0;
	REG_IE = 0;
	REG_IF = REG_IF;

	REG_IPC_SYNC = 0;
	DMA0_CR = 0;
	DMA1_CR = 0;
	DMA2_CR = 0;
	DMA3_CR = 0;

	while((*(vu32*)0x027FFDFC) != 0x027FFDF8);	// Timing adjustment with ARM9

	mem = (u32*)0x02000000;
	for(i = 0; i < 0x3FF800/4; i++) {
		*mem = 0x00000000;
		mem++;
	}
//	memset((u8*)0x2000000, 0x00, 0x3FF800);

	while(_set_r4menu());

	adr = (char*)0x027FFE00;
	_read_r4menu(adr, 0);			// Header


	blk = (*(vu32*)0x027FFE20) / 512;
	adr = (char*)(*(vu32*)0x027FFE28);
	siz = (*(vu32*)0x027FFE2C);
	for(i = 0; i < siz; i += 512) {		// ARM9
		_read_r4menu(adr, blk);
		blk++;
		adr += 512;
	}

	blk = (*(vu32*)0x027FFE30) / 512;
	adr = (char*)(*(vu32*)0x027FFE38);
	siz = (*(vu32*)0x027FFE3C);
	for(i = 0; i < siz; i += 512) {		// ARM7
		_read_r4menu(adr, blk);
		blk++;
		adr += 512;
	}

	*(vu32*)0x027FFDFC = *(vu32*)0x027FFE24;
	asm("swi 0x00");			// JUMP 0x027FFE34

	while(1);

}
