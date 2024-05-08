/*---------------------------------------------------------------------------------
	$Id: template.c,v 1.4 2005/09/17 23:15:13 wntrmute Exp $

	Basic Hello World

	$Log: template.c,v $
	Revision 1.4  2005/09/17 23:15:13  wntrmute
	corrected iprintAt in templates
	
	Revision 1.3  2005/09/05 00:32:20  wntrmute
	removed references to IPC struct
	replaced with API functions
	
	Revision 1.2  2005/08/31 01:24:21  wntrmute
	updated for new stdio support

	Revision 1.1  2005/08/03 06:29:56  wntrmute
	added templates


---------------------------------------------------------------------------------*/
#include "nds.h"
#include <nds/arm9/console.h> //basic print funcionality
//#include <nds/registers_alt.h>
// #include <nds/jtypes.h>
#include <nds/ndstypes.h>

#include <fat.h>
#include <sys/dir.h>
#include <sys/iosupport.h>
// #include "fatfile.h"
#include <nds/arm9/dldi.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "disc_io.h"
//#include "gba_nds_fat.h"
//extern LPIO_INTERFACE active_interface;


#include "maindef.h"
#include "dsCard.h"
#include "GBA_ini.h"
#include "ctrl_tbl.h"

//#include "ram.h"

#include "memcleaner.h"

#include "linkreset_arm9.h"
//#include "unicode.h"
#include "skin.h"
#include "message.h"

#include "tarosa/tarosa_Graphic.h"
#include "tarosa/tarosa_Shinofont.h"
extern uint16* MainScreen;
extern uint16* SubScreen;
//uint16* MainScreen = VRAM_A;
//uint16* SubScreen = (uint16*)BG_TILE_RAM_SUB(1);

#define BG_256_COLOR   (BIT(7))

int	numFiles = 0;
int	numGames = 0;

char	curpath[256];
int	sortfile[200];
struct	GBA_File {
		char	Alias[13];
		u32	type;
		char	filename[512];
		u32	filesize;
		char	gametitle[13];
		char	gamecode[5];
};

struct	GBA_File	fs[200];
char	tbuf[512];
char	filename[512];

u8	*rwbuf;

#define IPC_CMD_GBAMODE  1
#define IPC_CMD_SLOT2	 2
#define IPC_CMD_TURNOFF  9
#define IPC_CMD_SR_R4TF 11
#define IPC_CMD_SR_DLMS 12
#define IPC_CMD_SR_GEN	13


extern	int	carttype;


void Vblank()
{
}

void FIFOInit()
{
	REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_SEND_CLEAR;
}


void FIFOSend(u32 val)
{
	REG_IPC_FIFO_TX = val;
}



u32 inp_key()
{
	u32	ky;

	while(1) {
		swiWaitForVBlank();
		scanKeys();
		ky = keysDown();
		if(ky & KEY_A)	break;
		if(ky & KEY_B)	break;
	}
	while(1) {
		swiWaitForVBlank();
		scanKeys();
		if(keysHeld() != ky)	break;
	}
	return(ky);
}


extern	void	ret_menu9_R4(void);

extern	bool	ret_menu_chk(void);
extern	bool	ret_menu9_Gen(void);
extern	void	ret_menu9_GENs(void);

void turn_off(int cmd)
{
	if(cmd == 0)			// 電源断
		FIFOSend(IPC_CMD_TURNOFF);
	if(cmd == 1) {			// R4 Soft Reset
		FIFOSend(IPC_CMD_SR_R4TF);
		REG_IME = 0;
		REG_IE = 0;
		REG_IF = REG_IF;

		REG_EXMEMCNT = 0xE880;
		REG_IPC_SYNC = 0;
		DMA0_CR = 0;
		DMA1_CR = 0;
		DMA2_CR = 0;

		ret_menu9_R4();
	}
	if(cmd == 2) {			// DSLink Soft Reset
		FIFOSend(IPC_CMD_SR_DLMS);
		LinkReset_ARM9();
	}
	if(cmd == 3) {			// General purpose Soft Reset
		ret_menu9_Gen();
		FIFOSend(IPC_CMD_SR_GEN);
		ret_menu9_GENs();
	}

	while(1);
}


void gba_frame()
{
	int	ret;
	int	x=0, y=0;
	u16	*pDstBuf1;
	u16	*pDstBuf2;

	ret = LoadSkin(2, "/gbaframe.bmp");
	if(ret)	return;

	sprintf(tbuf, "%s/gbaframe.bmp", ini.sign_dir);
	ret = LoadSkin(2, tbuf);
	if(ret)	return;

	ret = LoadSkin(2, "/_system_/gbaframe.bmp");
	if(ret)	return;

	pDstBuf1 = (u16*)0x06000000;
	pDstBuf2 = (u16*)0x06020000;
	for(y = 0; y < 192; y++) {
		for(x = 0; x < 256; x++) {
			pDstBuf1[x] = 0x0000;
			pDstBuf2[x] = 0x0000;
		}
		pDstBuf1 += 256;
		pDstBuf2 += 256;
	}
}

static void resetToSlot2()
{
	vu32	vr;

    // make arm9 loop code
	*((vu32*)0x027FFE08) = (u32)0xE59FF014;  // ldr pc, 0x027FFE24
	*((vu32*)0x027FFE24) = (u32)0x027FFE08;  // Set ARM9 Loop address
	*((vu32*)0x027FFE34) = (u32)0x080000C0;

	sysSetCartOwner(BUS_OWNER_ARM7);  // ARM7 has access to GBA cart

	FIFOSend(IPC_CMD_SLOT2);

	for(vr = 0; vr < 0x20000; vr++);	// Wait ARM7

	DC_FlushAll();
	DC_InvalidateAll();
	swiSoftReset();
}

void gbaMode()
{

	if(strncmp(GBA_HEADER.gamecode, "PASS", 4) == 0) {
	        resetARM9Memory();
	        resetToSlot2();
	}

	videoSetMode(0);
	videoSetModeSub(0);

	// vramSetMainBanks(VRAM_A_MAIN_BG, VRAM_B_MAIN_BG, VRAM_C_MAIN_BG, VRAM_D_MAIN_BG);
	vramSetPrimaryBanks(VRAM_A_MAIN_BG, VRAM_B_MAIN_BG, VRAM_C_MAIN_BG, VRAM_D_MAIN_BG);
//	vramSetMainBanks(VRAM_A_MAIN_BG, VRAM_B_MAIN_BG, VRAM_C_ARM7, VRAM_D_ARM7);

	if(PersonalData->gbaScreen) { lcdMainOnBottom(); } else { lcdMainOnTop(); }

//	FIFOSend(IPC_CMD_GBAMODE);

	gba_frame();

	FIFOSend(IPC_CMD_GBAMODE);
	sysSetBusOwners(ARM7_OWNS_CARD, ARM7_OWNS_ROM);
	REG_IME = 0;

	while(1);
} 


void err_cnf(int n1, int n2)
{
	int	len;
	int	x1, x2;
	int	y1, y2;
	int	xi, yi;
	u16	*gback;
	int	gsiz;

	len = strlen(errmsg[n1]);
	if(len < strlen(errmsg[n2]))
		len = strlen(errmsg[n2]);
	if(len < 10)	len = 10;

	x1 = (256 - len * 6) / 2 - 4;
	y1 = 4*12-6;
	x2 = x1 + len * 6 + 9;
	y2 = 8*12+3;

	gsiz = (x2-x1+1) * (y2-y1+1);
	gback = (u16*)malloc(sizeof(u16*) * gsiz);
	for( yi=y1; yi<y2+1; yi++ ){
		for( xi=x1; xi<x2+1; xi++ ){
			gback[(xi-x1)+(yi-y1)*(x2+1-x1)] = Point_SUB( SubScreen, xi, yi );
		}
	}

	DrawBox_SUB( SubScreen, x1, y1, x2, y2, 6, 0);
	DrawBox_SUB( SubScreen, x1+1, y1+1, x2-1, y2-1, 2, 1);
	DrawBox_SUB( SubScreen, x1+2, y1+2, x2-2, y2-2, 6, 0);

	ShinoPrint_SUB(SubScreen, x1 + 6, y1 + 6, (u8 *)errmsg[n1], 0, 0, 0);
	ShinoPrint_SUB(SubScreen, x1 + 6, y1 + 20, (u8 *)errmsg[n2], 0, 0, 0);
	ShinoPrint_SUB(SubScreen, x1 + (len/2)*6 - 18, y1 + 37, (u8*)errmsg[13], 0, 0, 0);


	while(!(inp_key() & KEY_A));

	for( yi=y1; yi<y2+1; yi++ ){
		for( xi=x1; xi<x2+1; xi++ ){
			Pixel_SUB(SubScreen, xi, yi, gback[(xi-x1) + (yi-y1)*(x2+1-x1)] );
		}
	}
	free(gback);

//	turn_off(0);

}


int cnf_inp(int n1, int n2)
{
	int	len;
	int	x1, x2;
	int	y1, y2;
	int	xi, yi;
	u16	*gback;
	int	gsiz;
	u32	ky;

	len = strlen(cnfmsg[n1]);
	if(len < strlen(cnfmsg[n2]))
		len = strlen(cnfmsg[n2]);
	if(len < 20)	len = 20;

	x1 = (256 - len * 6) / 2 - 4;
	y1 = 4*12-6;
	x2 = x1 + len * 6 + 9;
	y2 = 8*12+3;

	gsiz = (x2-x1+1) * (y2-y1+1);
	gback = (u16*)malloc(sizeof(u16*) * gsiz);
	for( yi=y1; yi<y2+1; yi++ ){
		for( xi=x1; xi<x2+1; xi++ ){
			gback[(xi-x1)+(yi-y1)*(x2+1-x1)] = Point_SUB( SubScreen, xi, yi );
		}
	}

	DrawBox_SUB( SubScreen, x1, y1, x2, y2, 6, 0);
	DrawBox_SUB( SubScreen, x1+1, y1+1, x2-1, y2-1, 5, 1);
	DrawBox_SUB( SubScreen, x1+2, y1+2, x2-2, y2-2, 6, 0);

	ShinoPrint_SUB(SubScreen, x1 + 6, y1 + 6, (u8 *)cnfmsg[n1], 0, 0, 0);
	ShinoPrint_SUB(SubScreen, x1 + 6, y1 + 20, (u8 *)cnfmsg[n2], 0, 0, 0);
	ShinoPrint_SUB(SubScreen, x1 + (len/2)*6 - 50, y1 + 37, (u8*)cnfmsg[0], 0, 0, 0);


	ky = inp_key();

	for( yi=y1; yi<y2+1; yi++ ){
		for( xi=x1; xi<x2+1; xi++ ){
			Pixel_SUB(SubScreen, xi, yi, gback[(xi-x1) + (yi-y1)*(x2+1-x1)] );
		}
	}
	free(gback);
	return(ky);
}


u16	*gbar = NULL;
int	oldper;

void dsp_bar(int mod, int per)
{
	int	x1, x2;
	int	y1, y2;
	int	xi, yi;
	int	gsiz;

	x1 = 49;
	y1 = 142;	//70;
	x2 = 205;
	y2 = 187;	//115;

	if(per < 0) {
		gsiz = (x2-x1+1) * (y2-y1+1);
		gbar = (u16*)malloc(sizeof(u16*) * gsiz);
		for( yi=y1; yi<y2+1; yi++ ){
			for( xi=x1; xi<x2+1; xi++ ){
				gbar[(xi-x1)+(yi-y1)*(x2+1-x1)] = Point(MainScreen, xi, yi );
			}
		}

		DrawBox(MainScreen, x1, y1, x2, y2, RGB15(31,31,0), 0);
		DrawBox(MainScreen, x1+1, y1+1, x2-1, y2-1, RGB15(0,0,31), 1);
		DrawBox(MainScreen, x1+2, y1+2, x2-2, y2-2, RGB15(31,31,0), 0);

		if(per != -2)
			DrawBox(MainScreen, x1 + 28, y1 + 20, x1 + 129, y1 + 40, RGB15(31,31,31), 0);
		ShinoPrint(MainScreen, x1 + 26, y1 + 6, (u8 *)barmsg[mod], RGB15(31,31,31), RGB15(31,31,31), 0);
		oldper = -1;
		return;
	}

	if(gbar == NULL)	return;

	if(per != oldper) {
		oldper = per;
		if(per > 0)
			DrawBox(MainScreen, x1 + 29, y1 + 21, x1 + 28 + per, y1 + 39, RGB15(30,0,0), 1);
		if(per < 100)
			DrawBox(MainScreen, x1 + 28 + per + 1, y1 + 21, x1 + 128, y1 + 39, RGB15(0,30,0), 1);
		sprintf(tbuf, "%2d%%", per);
		ShinoPrint(MainScreen, x1 + 73, y1 + 24, (u8 *)tbuf, RGB15(31,31,31), RGB15(31,31,31), 0);
	}

	if(mod == -1) {
		for( yi=y1; yi<y2+1; yi++ ){
			for( xi=x1; xi<x2+1; xi++ ){
				Pixel(MainScreen, xi, yi, gbar[(xi-x1) + (yi-y1)*(x2+1-x1)] );
			}
		}
		free(gbar);
		gbar = NULL;
	}
	return;
}



void RamClear()
{
	u32	*a8;	//, *a9;
	int	i;

	a8 = (u32*)0x8000000;
//	a9 = (u32*)0x9000000;
	for(i = 0; i < 0x100; i++) {
		a8[i] = 0xFFFFFFFF;
//		a9[i] = 0xFFFFFFFF;
	}

	*(vu32*)0x80000B4 = 0x24242400;		// "$$$"
	*(vu32*)0x80000BC = 0x7FFFFFFF;
	*(vu32*)0x801FFFC = 0x7FFFFFFF;
	*(vu32*)0x8240000 = 0x00000000;
}


extern void setGBAmode(void);
extern void getGBAmode(void);

int	GBAmode;
int	r4tf;


void _dsp_clear()
{
	DrawBox_SUB(SubScreen, 0, 28, 255, 114, 0, 1);

}


int rumble_cmd()
{
	int	cmd = 0;
	u32	ky, repky;
	int	i;
	int	len;
	int	x1, x2;
	int	y1, y2;
//	int	xi, yi;
//	u16	*gback;
//	int	gsiz;

	len = strlen(cmd_m[0]);

	x1 = (256 - len * 6) / 2 - 4;
	y1 = 4*12-6;
	x2 = x1 + len * 6 + 5;
	y2 = 8*12+2;

//	gsiz = (x2-x1+1) * (y2-y1+1);
//	gback = (u16*)malloc(sizeof(u16*) * gsiz);
//	for( yi=y1; yi<y2+1; yi++ ){
//		for( xi=x1; xi<x2+1; xi++ ){
//			gback[(xi-x1)+(yi-y1)*(x2+1-x1)] = Point_SUB( SubScreen, xi, yi );
//		}
//	}

	ColorSwap_SUB(SubScreen, 0, 0, 255, 192, 3, 5);
	_dsp_clear();
	DrawBox_SUB(SubScreen, 9, 137, 246, 187, 0, 1);

	DrawBox_SUB(SubScreen, 75, 115, 181, 136, 1, 0);
	DrawBox_SUB(SubScreen, 76, 116, 180, 135, 5, 1);
	DrawBox_SUB(SubScreen, 77, 117, 179, 134, 0, 0);

	ShinoPrint_SUB( SubScreen, 15*6, 10*12, (u8 *)t_msg[17], 0, 5, 1);
	ShinoPrint_SUB( SubScreen, 2*6, 12*12+6, (u8 *)t_msg[18], 1, 0, 0);
	ShinoPrint_SUB( SubScreen, 2*6, 14*12+6, (u8 *)t_msg[19], 1, 0, 0);


	DrawBox_SUB( SubScreen, x1, y1, x2, y2, 5, 1);
	DrawBox_SUB( SubScreen, x1+1, y1+1, x2-1, y2-1, 0, 1);
	DrawBox_SUB( SubScreen, x1+2, y1+2, x2-2, y2-2, 5, 0);

	ShinoPrint_SUB(SubScreen, x1 + 3, y1 + 3, (u8 *)cmd_m[0], 2, 3, 1);
	for(i = 1; i < 4; i++) {
		ShinoPrint_SUB(SubScreen, x1 + 3, y1 + 3 + i*13, (u8 *)cmd_m[i], 1, 0, 0);
	}


	while(1) {
		swiWaitForVBlank();
		scanKeys();
		repky = keysDownRepeat();
		if((repky & KEY_UP) || (repky & KEY_DOWN)) {
			if(repky & KEY_UP){
				if(cmd > 0) {
					ShinoPrint_SUB(SubScreen, x1 + 3, y1 + 3 + cmd*13, (u8 *)cmd_m[cmd], 1, 0, 1);
					cmd--;
					ShinoPrint_SUB(SubScreen, x1 + 3, y1 + 3 + cmd*13, (u8 *)cmd_m[cmd], 2, 3, 1);
				}
			}
			if(repky & KEY_DOWN){
				if(cmd < 3) {
					ShinoPrint_SUB(SubScreen, x1 + 3, y1 + 3 + cmd*13, (u8 *)cmd_m[cmd], 1, 0, 1);
					cmd++;
					ShinoPrint_SUB(SubScreen, x1 + 3, y1 + 3 + cmd*13, (u8 *)cmd_m[cmd], 2, 3, 1);
				}
			}
			continue;
		}

		ky = keysDown();

		if(ky & KEY_A)	break;
		if(ky & KEY_L) {
			GBAmode = 1;
			setGBAmode();
			cmd = -1;
			break;
		}
		if(ky & KEY_START) {
			if(r4tf) {
				cmd = 99;
				SetRompage(0);
				SetRampage(16);
				break;
			}
		}
	}

/*
	while(1) {
		swiWaitForVBlank();
		scanKeys();
		if(keysHeld() != ky)	break;
	}
*/

	if(cmd != -1)	return(cmd);

//	for( yi=y1; yi<y2+1; yi++ ){
//		for( xi=x1; xi<x2+1; xi++ ){
//			Pixel_SUB(SubScreen, xi, yi, gback[(xi-x1) + (yi-y1)*(x2+1-x1)] );
//		}
//	}
//	free(gback);

	return(-1);
}


void _gba_dsp(int no, int mod, int x, int y)
{
	char	dsp[40];
	int	sn;

	sn = sortfile[no];
//	Unicode2Local(fs[no].uniname, (u8*)dsp, 31);
	if(fs[sn].type & S_IFDIR) {
		jstrncpy(tbuf, fs[sn].filename, 33);
//		tbuf[33] = 0;
		sprintf(dsp, "<%s>", tbuf);
		sprintf(tbuf, " %-35s <DIR>", dsp);
	} else {
		jstrncpy(dsp, fs[sn].filename, 31);
//		dsp[31] = 0;
		sprintf(tbuf, " %-31s %6.2f MB", dsp, (float)fs[sn].filesize / (1024*1024));
	}

	if(mod == 1) {
		ShinoPrint( MainScreen, x*6, y*12, (u8 *)tbuf, RGB15(31,0,0), RGB15(0,0,31), 1);

		if(GBAmode == 0) {
			DrawBox_SUB(SubScreen, 6, 32, 249, 76, 5, 0);
			DrawBox_SUB(SubScreen, 8, 34, 247, 74, 5, 0);
		} else {
			DrawBox_SUB(SubScreen, 6, 32, 249, 76, 3, 0);
			DrawBox_SUB(SubScreen, 8, 34, 247, 74, 3, 0);
		}
		DrawBox_SUB(SubScreen, 9, 4*12, 246, 6*12-1, 0, 1);

		ShinoPrint_SUB( SubScreen, 2*6, 3*12, (u8 *)t_msg[0], 1, 0, 0 );

//		Unicode2Local(fs[no].uniname, (u8*)tbuf, 38);
		if(!(fs[sn].type & S_IFDIR)) {
			jstrncpy(tbuf, fs[sn].filename, 38);
//			tbuf[38] = 0;
			ShinoPrint_SUB( SubScreen, 3*6, 4*12, (u8 *)tbuf, 1, 0, 0 );
			sprintf(tbuf, "Size: %dKB (%s %s)", (int)fs[sn].filesize / 1024, fs[sn].gametitle, fs[sn].gamecode);
			ShinoPrint_SUB( SubScreen, 4*6, 5*12, (u8 *)tbuf, 1, 0, 0 );
		}
	} else	ShinoPrint( MainScreen, x*6, y*12, (u8 *)tbuf, RGB15(0,0,0), RGB15(31,31,31), 1);
}


/**************
void _gba_dsp2(char *name)
{
	int	i;

	for(i = 0; i < 32; i++)
		tbuf[i] = name[i];
	tbuf[i] = 0;

	ShinoPrint_SUB( SubScreen, 1*6, 5*12, (u8 *)tbuf, 1, 0, 0 );
}
***************/

extern	bool checkSRAM_cnf();
extern	int checkSRAM(char *name);


void _gba_sel_dsp(int no, int yc, int mod)
{
	int	x, y;
	int	st, i;
	int	len;
	y = 1;
	x = 0;

	if(mod == 0) {
		_dsp_clear();

			DrawBox_SUB(SubScreen, 75, 115, 181, 136, 1, 0);
			DrawBox_SUB(SubScreen, 76, 116, 180, 135, 5, 1);
			DrawBox_SUB(SubScreen, 77, 117, 179, 134, 0, 0);

		if(GBAmode == 0) {
//			DrawBox_SUB(SubScreen, 75, 115, 181, 136, 1, 0);
//			DrawBox_SUB(SubScreen, 76, 116, 180, 135, 5, 1);
//			DrawBox_SUB(SubScreen, 77, 117, 179, 134, 0, 0);

			if(carttype < 4)
				ShinoPrint_SUB( SubScreen, 15*6, 10*12, (u8 *)t_msg[1], 0, 5, 1);
			else	ShinoPrint_SUB( SubScreen, 15*6, 10*12, (u8 *)t_msg[21], 0, 5, 1);

			ShinoPrint_SUB( SubScreen, 2*6, 11*12+6, (u8 *)t_msg[2], 1, 0, 1);
			ShinoPrint_SUB( SubScreen, 2*6, 12*12+6, (u8 *)t_msg[3], 1, 0, 1);
			ShinoPrint_SUB( SubScreen, 2*6, 13*12+6, (u8 *)t_msg[4], 1, 0, 1);
			if(carttype < 3) {
				ShinoPrint_SUB( SubScreen, 2*6, 14*12+6, (u8 *)t_msg[5], 1, 0, 1);
			} else {
				if(r4tf)
					ShinoPrint_SUB( SubScreen, 2*6, 14*12+6, (u8 *)t_msg[20], 1, 0, 1);
				else	ShinoPrint_SUB( SubScreen, 2*6, 14*12+6, (u8 *)"                          ", 1, 0, 1);
			}
		} else {
//			DrawBox_SUB(SubScreen, 75, 115, 181, 136, 1, 0);
//			DrawBox_SUB(SubScreen, 76, 116, 180, 135, 6, 1);
//			DrawBox_SUB(SubScreen, 77, 117, 179, 134, 5, 0);

			if(r4tf)
				ShinoPrint_SUB( SubScreen, 2*6, 14*12+6, (u8 *)t_msg[6], 1, 0, 1);
			else
				ShinoPrint_SUB( SubScreen, 2*6, 14*12+6, (u8 *)t_msg[7], 1, 0, 1);
//		ShinoPrint_SUB( SubScreen, 15*6, 10*12, (u8 *)t_msg[8], 5, 6, 1);
			ShinoPrint_SUB( SubScreen, 15*6, 10*12, (u8 *)t_msg[8], 0, 5, 1);
			ShinoPrint_SUB( SubScreen, 2*6, 11*12+6, (u8 *)t_msg[9], 1, 0, 1);
			ShinoPrint_SUB( SubScreen, 2*6, 12*12+6, (u8 *)t_msg[10], 1, 0, 1);
			ShinoPrint_SUB( SubScreen, 2*6, 13*12+6, (u8 *)t_msg[11], 1, 0, 1);
		}


		ClearBG( MainScreen, RGB15(31,31,31) );
		DrawBox(MainScreen, 0, 0, 255, 11, RGB15(0,0,0), 1);
		sprintf(tbuf, t_msg[12], curpath, numGames);
		len = strlen(tbuf);
		if(len > 40)	len -= 40;
		else		len = 0;
		ShinoPrint(MainScreen, 0, 0, (u8 *)(tbuf + len), RGB15(31,31,31), RGB15(0,0,0), 0);

		DrawBox_SUB(SubScreen, 6, 80, 249, 111, 5, 0);
		DrawBox_SUB(SubScreen, 8, 82, 247, 109, 5, 0);


	if(GBAmode == 0) {
		ColorSwap_SUB(SubScreen, 0, 0, 255, 192, 3, 5);
	} else {
		ColorSwap_SUB(SubScreen, 0, 0, 255, 192, 5, 3);
	}


		checkSRAM(filename);
//		Unicode2Local(uniname, (u8*)savName, 34);
		filename[35] = 0;
		len = strlen(filename);
		if(len == 0) {
			sprintf(filename, t_msg[13]);
			len = 20;
		}

		len = (42 - len - 4) * 6 / 2;
		sprintf(tbuf, "< %s >", filename);
		ShinoPrint_SUB( SubScreen, 2*6, 7*12, (u8 *)t_msg[14], 1, 0, 0);
		ShinoPrint_SUB( SubScreen, len+1, 8*12, (u8 *)tbuf, 1, 0, 0);
	}

	st = no - yc;
	for(i = 0; i < 15; i++) {
		if(i + st < numFiles) {
			if(i == yc)
				_gba_dsp(i + st, 1, x, y + i);
			else	_gba_dsp(i + st, 0, x, y + i);
		}
	}
}

extern	int writeFileToNor(int sel);
extern	int writeFileToRam(int sel);
extern	void writeSramToFile(char *name);
extern	void writeSramFromFile(char *name);
extern	void SRAMdump(int cmd);
extern	bool checkBackup(void);
extern	bool checkFlashID(void);

/***************************
int gba_sel0()
{
	int	cmd;
	u32	ky;
	int	cn;

	cn = 1;
	if(r4tf)	cn++;

	_gba_sel_dsp(0, 0, 0);

	ShinoPrint(MainScreen, 35, 60, (u8 *)t_msg[15], RGB15(31,0,0), RGB15(0,0,31), 1);
	while(1) {
		swiWaitForVBlank();
		scanKeys();
		ky = keysDown();

		if(ky & KEY_L) {
			if(GBAmode > 0) {
				GBAmode--;
				setGBAmode();
				cmd = -1;
				break;
			}
		}
		if(ky & KEY_R) {
			if(carttype > 2) {
				cmd = 3;
				break;
			}
			if(GBAmode < cn) {
				GBAmode++;
				setGBAmode();
				cmd = -1;
				break;
			}
		}
		if(ky & KEY_START) {
			if(r4tf) {
				cmd = 99;
				SetRompage(0);
				SetRampage(16);
				break;
			}
		}
	}


//	while(1) {
//		swiWaitForVBlank();
//		scanKeys();
//		if(keysHeld() != ky)	break;
//	}

	return(cmd);
}
****************/


extern	u32	SaveType;
extern	u32	SaveSize;
extern	u8	SaveVer[];
extern	int	PatchCnt;
extern	u32	PatchType[];
extern	u32	PatchAddr[];

extern	void setcurpath(void);
extern	void getcurpath(void);
extern	void FileListGBA(void);

extern	int save_sel(int mod, char *name);


int gba_sel()
{
//	u32	i;

	int	cmd = -1;
	int	sel;
	u32	ky, repky;
	int	yc;
	int	x, y;
	int	cn;
	int	ret = 0;
	int	st0, st1;

	y = 1;
	x = 0;
	sel = 0;
	yc = 0;

	int	ii;

	cn = 1;
	if(r4tf)	cn++;

	_gba_sel_dsp(sel, yc, 0);

	while(1) {
		swiWaitForVBlank();
		scanKeys();
		repky = keysDownRepeat();
		if((repky & KEY_UP) || (repky & KEY_DOWN)) {
			if(repky & KEY_UP) {
				if(sel > 0) {
					if(yc == 0) {
						sel--;
						_gba_sel_dsp(sel, yc, 1);
					} else {
						_gba_dsp(sel, 0, x, y+yc);
						yc--;
						sel--;
						_gba_dsp(sel, 1, x, y+yc);
					}
				}
			}
			if(repky & KEY_DOWN) {
				if(sel < numFiles - 1) {
					if(yc == 14) {
						sel++;
						_gba_sel_dsp(sel, yc, 1);
					} else {
						_gba_dsp(sel, 0, x, y+yc);
						yc++;
						sel++;
						_gba_dsp(sel, 1, x, y+yc);
					}
				}
			}
			continue;
		}

		ky = keysDown();

		if(ky & KEY_LEFT) {
			if(sel > 0) {
				st0 = sel - yc;
				st1 = st0 - 15;
				if(st1 < 0)	st1 = 0;
				if(st1 == st0) {
					_gba_dsp(sel, 0, x, y+yc);
					yc = 0;
					sel = 0;
					_gba_dsp(sel, 1, x, y+yc);
				} else {
					sel = st1 + yc;
					_gba_sel_dsp(sel, yc, 1);
				}
			}
		}
		if(ky & KEY_RIGHT) {
			if(sel < numFiles -1) {
				st0 = sel - yc;
				st1 = st0 + 15;
				if(st1 >= numFiles - 15) {
					st1 = numFiles - 15;
					if(st1 < 0)	st1 = 0;
				}
				if(st1 == st0) {
					_gba_dsp(sel, 0, x, y+yc);
					yc = 14;
					if(yc >= numFiles)	// BUG
						yc = numFiles - 1;
					sel = st1 + yc;
					_gba_dsp(sel, 1, x, y+yc);
				} else {
					sel = st1 + yc;
					_gba_sel_dsp(sel, yc, 1);
				}
			}
		}


		if(ky & KEY_L) {
			if(GBAmode > 0) {
				GBAmode--;
				setGBAmode();
				_gba_sel_dsp(sel, yc, 0);
//				cmd = -1;
//				break;
			}
		}
		if(ky & KEY_R) {
			if(r4tf && carttype > 2) {
				cmd = 3;
				break;
			}
			if(GBAmode < cn && carttype <= 2) {
				GBAmode++;
				setGBAmode();
				if(GBAmode == 2) {
					_gba_dsp(sel, 0, x, y+yc);
					cmd = -1;
					break;
				} else {
					_gba_sel_dsp(sel, yc, 0);
				}
			}
		}


		if(ky & KEY_START) {
			if(r4tf) {
				cmd = 99;
				if(carttype == 1) {
					SetRompage(0);
					SetRampage(16);
				}
				break;
			}
		}
		if(ky & KEY_SELECT) {
			if(r4tf && (GBAmode == 0)) {
				if(!(fs[sortfile[sel]].type & S_IFDIR))
					ret = writeFileToRam(sortfile[sel]);
				if(ret != 0) {
					_gba_sel_dsp(sel, yc, 0);
					err_cnf(7, 8);
				} else {
//					if(carttype == 3)
//						SetRompage(0x300);
//					else	SetRompage(384);
					turn_off(r4tf);
				}
			}
		}

		if(ky & KEY_X) {
			if(GBAmode == 1) {
				SetRompage(0);
				SetRampage(16);
				gbaMode();
			} else {
				if(cnf_inp(7, 8) & KEY_A)
					SRAMdump(0);
			}
		}

		if(ky & KEY_Y) {
			if(GBAmode == 1) {
				if(checkSRAM(filename)) {
//					if(cnf_inp(3, 4) & KEY_A)
					if(save_sel(0, filename) >= 0)
						writeSramFromFile(filename);
					_gba_sel_dsp(sel, yc, 0);
				} else	err_cnf(4, 5);
			} else {
				if(cnf_inp(5, 6) & KEY_A) {
					SRAMdump(1);
					_gba_sel_dsp(sel, yc, 0);
				}
			}
		}

		if(ky & KEY_A) {
			if(fs[sortfile[sel]].type & S_IFDIR) {
				if(!strcmp(fs[sortfile[sel]].filename, "..")) {
					for(ii = strlen(curpath) - 2; ii >= 0; ii--) {
						if(curpath[ii] == '/' ) {
							curpath[ii + 1] = 0;
							break;
						}
					}
				} else {
					strcat(curpath, fs[sortfile[sel]].filename);
					strcat(curpath, "/");
				}
				FileListGBA();
				setcurpath();
				cmd = -1;
				break;
			}
			if(GBAmode == 0)
				ret = writeFileToRam(sortfile[sel]);
			else	ret = writeFileToNor(sortfile[sel]);
			if(ret != 0) {
				if(ret == 2) {
					err_cnf(9, 10);
				} else {
					if(GBAmode == 0 && carttype < 3)
						err_cnf(7, 8);
					else	err_cnf(7, 6);
				}
			} else {


/******************
sprintf(tbuf, " %d <%s> ", (int)PatchType[0], SaveVer);
ShinoPrint_SUB( SubScreen, 8*6, 4*12, (u8*)tbuf, 3, 0, 1);
for(i = 0; i < PatchCnt; i++) {
	sprintf(tbuf, "  %02X %02X %08X  ", (int)SaveType, (int)PatchType[i], (int)PatchAddr[i]);
	ShinoPrint_SUB( SubScreen, 8*6, (5+i)*12, (u8*)tbuf, 3, 0, 1);
}
inp_key();
****************/


				if(GBAmode == 0) {
//					SetRompage(384);
					gbaMode();
				}
			}
			_gba_sel_dsp(sel, yc, 0);
		}
		if(ky & KEY_B) {
			if(checkSRAM(filename)) {
//				if(cnf_inp(1, 2) & KEY_A) {
				if(save_sel(1, filename) >= 0) {
					writeSramToFile(filename);
				}
				_gba_sel_dsp(sel, yc, 0);
			} else {
				err_cnf(4, 5);
			}
		}

	}

/**
	while(1) {
		swiWaitForVBlank();
		scanKeys();
		if(keysDown() != repky)	break;
	}
**/
	return(cmd);
}


// extern u32	_io_dldi;

extern	void	setLang(void);


void mainloop(void)
{
//	vu16	reg;

	FILE	*r4dt;
	__handle *handle;
	// FILE_STRUCT *file;
	FILE *file;
	// PARTITION *part;

	int	cmd;

	keysSetRepeat(20, 6);		// def. 60, 30 (delay, repeat)

	DrawBox_SUB(SubScreen, 20, 3, 235, 27, 1, 0);
	DrawBox_SUB(SubScreen, 21, 4, 234, 26, 5, 1);
	DrawBox_SUB(SubScreen, 22, 5, 233, 25, 0, 0);
	ShinoPrint_SUB( SubScreen, 9*6, 1*12-2, (u8*)"GBA ExpLoader", 0, 0, 0 );
//	ShinoPrint_SUB( SubScreen, 33*6+1, 12, (u8 *)"v0.5β", 0, 0, 0 );
	ShinoPrint_SUB( SubScreen, 34*6-2, 12, (u8 *)"v0.57", 0, 0, 0 );


	DrawBox_SUB(SubScreen, 6, 125, 249, 190, 5, 0);
	DrawBox_SUB(SubScreen, 8, 127, 247, 188, 5, 0);
/*
	DrawBox_SUB(SubScreen, 75, 115, 181, 136, 1, 0);
	DrawBox_SUB(SubScreen, 76, 116, 180, 135, 5, 1);
	DrawBox_SUB(SubScreen, 77, 117, 179, 134, 0, 0);
*/
/********
reg = REG_EXMEMCNT;
//REG_EXMEMCNT = (reg & 0xFF80) | (1 << 5) | (1 << 4) | (1 << 2) | 1;
REG_EXMEMCNT = (reg & 0xFFE0) | (1 << 4) | (1 << 2) | 1;
	sprintf(tbuf, "OLD = %04X, NEW = %04X", reg, REG_EXMEMCNT);
	ShinoPrint_SUB( SubScreen, 9*6, 5*12, tbuf, 1, 0, 0 );
	inp_key();
**********/
//	OpenNorWrite();
//	chip_reset();
	CloseNorWrite();
	SetRompage(0);
	SetRampage(16);
	SetShake(0x08);

	setLangMsg();

/********
	ram_init(DETECT_RAM);
	sprintf(tbuf, "%s %dKB", ram_type_string(), ram_size());
	ShinoPrint_SUB( SubScreen, 9*6, 5*12, tbuf, 1, 0, 0 );
	inp_key();
********/

	checkFlashID();
	if(carttype == 0 || carttype > 6) {
//		if(carttype != 5)
			err_cnf(2, 3);
//		else	err_cnf(9, 10);
		turn_off(r4tf);
	}
	if(carttype <= 2) {
//		SetRampage(16);
//		SetShake(0x08);
		if(carttype == 1)
			ShinoPrint_SUB( SubScreen, 23*6, 1*12-2, (u8*)" [ 3in1 ]", 0, 0, 0 );
		else
			ShinoPrint_SUB( SubScreen, 23*6, 1*12-2, (u8*)"[New3in1]", 0, 0, 0 );
	}
	if(carttype == 3) {
		SetRompage(0x300);
		ShinoPrint_SUB( SubScreen, 23*6, 1*12-2, (u8*)"  [ EZ4 ]", 0, 0, 0 );
	}
	if(carttype == 4) {
		ShinoPrint_SUB( SubScreen, 23*6, 1*12-2, (u8*)"[EXP256K]", 0, 0, 0 );
	}
	if(carttype == 5) {
		ShinoPrint_SUB( SubScreen, 23*6, 1*12-2, (u8*)"[EXP128K]", 0, 0, 0 );
	}
	if(carttype == 6) {
		ShinoPrint_SUB( SubScreen, 23*6, 1*12-2, (u8*)"[ M3/G6 ]", 0, 0, 0 );
	}



	ShinoPrint_SUB( SubScreen, 9*6, 5*12, (u8 *)t_msg[16], 1, 0, 0 );
	if(fatInitDefault() == false) {
		err_cnf(0, 1);
		turn_off(0);
	}

//ShinoPrint_SUB( SubScreen, 6*6, 6*12, "FAT OK", 1, 0, 0 );

/*********************
	sprintf(tbuf, "0x27FFE18 = %08X", (*(vu32*)0x027FFE18));
	ShinoPrint_SUB( SubScreen, 8*6, 5*12, (u8*)tbuf, 3, 0, 1);
**********************/

	if(ret_menu_chk()) {
		r4tf = 3;
	} else {
		r4tf = 0;
		if(io_dldi_data->ioInterface.ioType == 0x46543452) {		// R4TF
			if((*(vu32*)0x027FFE18) == 0x00000000) {
				r4dt = fopen("/_DS_MENU.DAT", "rb");
				if(r4dt != NULL) {
					handle = (__handle *)r4dt->_file;
					// file = (FILE_STRUCT *)handle->fileStruct;
					file = (FILE*)handle->fileStruct;
					// part = file->partition;
					// (*(vu32*)0x027FFE18) = (part->rootDirStart + file->dirEntryStart.sector) * 512 + file->dirEntryStart.offset * 32;
					fclose(r4dt);
					r4tf = 1;
				}
			} else {
				r4tf = 1;
			}
		}

		if(io_dldi_data->ioInterface.ioType == 0x534D4C44)r4tf = 2; // DLMS
	}

/******************************
	sprintf(tbuf, "0x27FFE18 = %08X", (*(vu32*)0x027FFE18));
	ShinoPrint_SUB( SubScreen, 2*6, 6*12, (u8*)tbuf, 3, 0, 1);
	sprintf(tbuf, "rootDS = %08X, dirESo = %08X", part->rootDirStart,  part->dataStart);
	ShinoPrint_SUB( SubScreen, 2*6, 7*12, (u8*)tbuf, 3, 0, 1);

//	sprintf(tbuf, "byte/sec = %08X, byte/clu = %08X", part->bytesPerSector,  part->bytesPerSector);
//	ShinoPrint_SUB( SubScreen, 2*6, 8*12, (u8*)tbuf, 3, 0, 1);
	sprintf(tbuf, "cluster = %08X, sector = %08X", file->dirEntryEnd.sector,  file->dirEntryEnd.offset);
	ShinoPrint_SUB( SubScreen, 2*6, 9*12, (u8*)tbuf, 3, 0, 1);

	sprintf(tbuf, "sector = %08X, offset = %08X", file->dirEntryStart.sector,  file->dirEntryStart.offset);
	ShinoPrint_SUB( SubScreen, 2*6, 10*12, (u8*)tbuf, 3, 0, 1);
//	sprintf(tbuf, "rwPsec = %08X, rwByte = %08X",file->rwPosition.sector,  file->rwPosition.byte);
//	ShinoPrint_SUB( SubScreen, 2*6, 10*12, (u8*)tbuf, 3, 0, 1);
inp_key();
*************************/

	*(vu8*)0x027FFC35 = 0x01;	// GBA

	rwbuf = (u8*)malloc(0x100000 + 1024);

	GBA_ini();

	if(checkSRAM_cnf() == false) {
		if(carttype != 5) {
			if(cnf_inp(9, 10) & KEY_B)
				turn_off(r4tf);
		}
	}


//ShinoPrint_SUB( SubScreen, 6*6, 7*12, "FILE LIST", 1, 0, 0 );
//	mkdir("/GBA_SAVE", 0777);
//	mkdir("/GBA_SIGN", 0777);
	getcurpath();
	FileListGBA();
//ShinoPrint_SUB( SubScreen, 6*6, 7*12, "FILE LIST -- OK", 1, 0, 0 );

	_dsp_clear();

	GBAmode = 0;
	if(checkSRAM(filename) && checkBackup()) {
		if(save_sel(1, filename) >= 0)
			writeSramToFile(filename);
	}

	getGBAmode();
	if((GBAmode == 2) && (r4tf == 0))
		GBAmode = 0;
	if(carttype > 2)
		GBAmode = 0;

	cmd = -1;
	while(cmd == -1) {
		if(GBAmode == 2)
			cmd = rumble_cmd();
		else {
//			FileListGBA();
//			setcurpath();
//			if(numFiles == 0)
//				cmd = gba_sel0();
//			else	cmd = gba_sel();
			cmd = gba_sel();
		}
	}

	*(vu8*)0x027FFC35 = 0x00;	// 拡張
	switch(cmd) {
		case 0:
			SetShake(0xF0);
			break;
		case 1:
			SetShake(0xF1);
			break;
		case 2:
			SetShake(0xF2);
			break;
		case 3:
			if(carttype != 4) {
				SetRompage(0x300);
				OpenNorWrite();
			}
			RamClear();
			break;
	}

	turn_off(r4tf);

}


//---------------------------------------------------------------------------------
int main(void) {
//---------------------------------------------------------------------------------

	int	i;

	vramSetPrimaryBanks(VRAM_A_LCD ,  VRAM_B_LCD  , VRAM_C_SUB_BG, VRAM_D_MAIN_BG  );
	powerOn(POWER_ALL);

	irqInit();
	irqSet(IRQ_VBLANK, Vblank);
	irqEnable(IRQ_VBLANK);
	FIFOInit();

 videoSetMode(MODE_FB0 | DISPLAY_BG2_ACTIVE);
 videoSetModeSub(MODE_0_2D | DISPLAY_BG0_ACTIVE );
 // SUB_BG0_CR = BG_256_COLOR | BG_MAP_BASE(0) | BG_TILE_BASE(1);
 REG_BG0CNT_SUB = BG_256_COLOR | BG_MAP_BASE(0) | BG_TILE_BASE(1);
 uint16* map1 = (uint16*)BG_MAP_RAM_SUB(0);
 for(i=0;i<(256*192/8/8);i++)	map1[i]=i;
 lcdMainOnTop();
	//メイン画面を白で塗りつぶします
	ClearBG( MainScreen, RGB15(31,31,31) );

	//サブ画面の表示
	BG_PALETTE_SUB[0] = RGB15(31,31,31);		//(白)サブ画面のバックカラー
	BG_PALETTE_SUB[1] = RGB15(0,0,0);			//(黒)サブ画面のフォアカラー
	BG_PALETTE_SUB[2] = RGB15(29,0,0);			//(赤)
	BG_PALETTE_SUB[3] = RGB15(0,20,0);			//(緑)
	BG_PALETTE_SUB[4] = RGB15(0,31,31);			//(水色)
	BG_PALETTE_SUB[5] = RGB15(0,0,31);			//(青)
	BG_PALETTE_SUB[6] = RGB15(31,31,0);			//(黄)

	ClearBG_SUB( SubScreen, 0 );				//バックを白に



//	videoSetMode(0);	//not using the main screen
//	videoSetModeSub(MODE_0_2D | DISPLAY_BG0_ACTIVE);	//sub bg 0 will be used to print text
//	vramSetBankC(VRAM_C_SUB_BG);

//	SUB_BG0_CR = BG_MAP_BASE(31);

//	BG_PALETTE_SUB[255] = RGB15(31,31,31);	//by default font will be rendered with color 255

	//consoleInit() is a lot more flexible but this gets you up and running quick
//	consoleInitDefault((u16*)SCREEN_BASE_BLOCK_SUB(31), (u16*)CHAR_BASE_BLOCK_SUB(0), 16);
	swiWaitForVBlank();
	swiWaitForVBlank();
	sysSetBusOwners(BUS_OWNER_ARM9,BUS_OWNER_ARM9);


//	iprintf("%s\n%s\n%s\n\n", ROMTITLE, ROMVERSION, ROMDATE);

	mainloop();

	return 0;
}


