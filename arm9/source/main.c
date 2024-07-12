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
#include <nds/ndstypes.h>
#include <nds/fifocommon.h>
#include <nds/fifomessages.h>

#include <fat.h>
#include <sys/dir.h>
#include <nds/arm9/dldi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tarosa/tarosa_Graphic.h"
#include "tarosa/tarosa_Shinofont.h"

#include "ret_menu9_gen.h"
#include "dsCard.h"

#include "GBA_ini.h"
#include "ctrl_tbl.h"
#include "skin.h"
#include "message.h"
#include "tonccpy.h"

extern uint16* MainScreen;
extern uint16* SubScreen;

#define BG_256_COLOR (BIT(7))

#define VERSTRING "v0.64"

int	numFiles = 0;
int	numGames = 0;

char curpath[256];
int	sortfile[200];

struct GBA_File fs[200];
char tbuf[512];
char filename[512];

u8	*rwbuf;

int	GBAmode;
bool softReset;

extern int carttype;
extern bool isSuperCard;
extern bool is3in1Plus;
extern bool isOmega;
extern u16 gl_ingame_RTC_open_status;
extern void SetSDControl(u16 control);

u32 inp_key() {
	u32	ky;

	while(1) {
		swiWaitForVBlank();
		scanKeys();
		ky = keysDown();
		if(ky & KEY_A)break;
		if(ky & KEY_B)break;
	}
	while(1) {
		swiWaitForVBlank();
		scanKeys();
		if(keysHeld() != ky)break;
	}
	return(ky);
}


extern bool ret_menu_chk(void);


extern void setGBAmode(void);
extern void getGBAmode(void);

void turn_off(bool softReset) {
	if (softReset) {
		if (!ret_menu9_Gen())systemShutDown();
	} else {
		systemShutDown();
	}
	while(1)swiWaitForVBlank();
}


void gba_frame() {
	int	ret;
	int mode = 3; // old mode == 2
	// int	x = 0, y = 0;
	// u16	*pDstBuf1;
	// u16	*pDstBuf2;

	if (access("/gbaframe.bmp", F_OK) == 0) {
		ret = LoadSkin(mode, "/gbaframe.bmp");
		if(ret)return;
	}

	sprintf(tbuf, "%s/gbaframe.bmp", ini.sign_dir);
	if (access(tbuf, F_OK) == 0) {
		ret = LoadSkin(mode, tbuf);
		if(ret)return;
	}

	if (access("/_system_/gbaframe.bmp", F_OK) == 0) {
		ret = LoadSkin(mode, "/_system_/gbaframe.bmp");
		if(ret)return;
	}
	
	if (access("/ttmenu/gbaframe.bmp", F_OK) == 0) {
		ret = LoadSkin(mode, "/ttmenu/gbaframe.bmp");
		if(ret)return;
	}

	/*pDstBuf1 = (u16*)0x06000000;
	pDstBuf2 = (u16*)0x06020000;
	for(y = 0; y < 192; y++) {
		for(x = 0; x < 256; x++) {
			pDstBuf1[x] = 0x0000;
			pDstBuf2[x] = 0x0000;
		}
		pDstBuf1 += 256;
		pDstBuf2 += 256;
	}*/
}

static void resetToSlot2() {
	vu32 vr;
    // make arm9 loop code
	*((vu32*)0x027FFE08) = (u32)0xE59FF014;  // ldr pc, 0x027FFE24
	*((vu32*)0x027FFE24) = (u32)0x027FFE08;  // Set ARM9 Loop address
	*((vu32*)0x027FFE34) = (u32)0x080000C0;

	sysSetCartOwner(BUS_OWNER_ARM7);  // ARM7 has access to GBA cart

	fifoSendValue32(FIFO_USER_02, 1);
	
	for(vr = 0; vr < 0x20000; vr++);	// Wait ARM7

	DC_FlushAll();
	DC_InvalidateAll();
	swiSoftReset();
}

void gbaMode() {

	if(strncmp(GBA_HEADER.gamecode, "PASS", 4) == 0)resetToSlot2();

	// videoSetMode(0);
	// videoSetModeSub(0);

	// vramSetPrimaryBanks(VRAM_A_MAIN_BG, VRAM_B_MAIN_BG, VRAM_C_MAIN_BG, VRAM_D_MAIN_BG);

	videoSetMode(MODE_5_2D | DISPLAY_BG3_ACTIVE);
	videoSetModeSub(MODE_5_2D | DISPLAY_BG3_ACTIVE);
	vramSetBankA(VRAM_A_MAIN_BG_0x06000000);
	vramSetBankB(VRAM_B_MAIN_BG_0x06020000);
	vramSetBankC(VRAM_C_SUB_BG_0x06200000);
	vramSetBankD(VRAM_D_LCD);
	// for the main screen
	REG_BG3CNT = BG_BMP16_256x256 | BG_BMP_BASE(0) | BG_WRAP_OFF;
	REG_BG3PA = 1 << 8; //scale x
	REG_BG3PB = 0; //rotation x
	REG_BG3PC = 0; //rotation y
	REG_BG3PD = 1 << 8; //scale y
	REG_BG3X = 0; //translation x
	REG_BG3Y = 0; //translation y*/
	toncset((void*)BG_BMP_RAM(0),0,0x18000);
	toncset((void*)BG_BMP_RAM(8),0,0x18000);
	swiWaitForVBlank();

	if(PersonalData->gbaScreen) { lcdMainOnBottom(); } else { lcdMainOnTop(); }

	gba_frame();

	sysSetBusOwners(ARM7_OWNS_CARD, ARM7_OWNS_ROM);
	fifoSendValue32(FIFO_USER_01, 1);
	REG_IME = 0;
	irqDisable(IRQ_VBLANK);
	while(1)swiWaitForVBlank();
} 


void err_cnf(int n1, int n2) {
	int	len;
	int	x1, x2;
	int	y1, y2;
	int	xi, yi;
	u16	*gback;
	int	gsiz;

	len = strlen(errmsg[n1]);
	if(len < strlen(errmsg[n2]))len = strlen(errmsg[n2]);
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


int cnf_inp(int n1, int n2) {
	int	len;
	int	x1, x2;
	int	y1, y2;
	int	xi, yi;
	u16	*gback;
	int	gsiz;
	u32	ky;

	len = strlen(cnfmsg[n1]);
	if(len < strlen(cnfmsg[n2]))len = strlen(cnfmsg[n2]);
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

void dsp_bar(int mod, int per) {
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

		if(per != -2)DrawBox(MainScreen, x1 + 28, y1 + 20, x1 + 129, y1 + 40, RGB15(31,31,31), 0);
		ShinoPrint(MainScreen, x1 + 26, y1 + 6, (u8 *)barmsg[mod], RGB15(31,31,31), RGB15(31,31,31), 0);
		oldper = -1;
		return;
	}

	if(gbar == NULL)return;

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

void RamClear() {
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



void _dsp_clear() {	DrawBox_SUB(SubScreen, 0, 28, 255, 114, 0, 1); }


int rumble_cmd() {
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
			if (isOmega)GBAmode = 0;
			setGBAmode();
			cmd = -1;
			break;
		}
		if(ky & KEY_START) {
			if(softReset) {
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


void _gba_dsp(int no, int mod, int x, int y) {
	char dsp[40];
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
	} else {
		ShinoPrint( MainScreen, x*6, y*12, (u8 *)tbuf, RGB15(0,0,0), RGB15(31,31,31), 1);
	}
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


void _gba_sel_dsp(int no, int yc, int mod) {
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

			if(carttype < 4) {
				ShinoPrint_SUB( SubScreen, 15*6, 10*12, (u8 *)t_msg[1], 0, 5, 1);
			} else {
				ShinoPrint_SUB( SubScreen, 15*6, 10*12, (u8 *)t_msg[21], 0, 5, 1);
			}

			ShinoPrint_SUB( SubScreen, 2*6, 11*12+6, (u8 *)t_msg[2], 1, 0, 1);
			ShinoPrint_SUB( SubScreen, 2*6, 12*12+6, (u8 *)t_msg[3], 1, 0, 1);
			ShinoPrint_SUB( SubScreen, 2*6, 13*12+6, (u8 *)t_msg[4], 1, 0, 1);
			if(carttype < 3) {
				ShinoPrint_SUB( SubScreen, 2*6, 14*12+6, (u8 *)t_msg[5], 1, 0, 1);
			} else {
				if(softReset) {
					ShinoPrint_SUB( SubScreen, 2*6, 14*12+6, (u8 *)t_msg[20], 1, 0, 1);
				} else {
					ShinoPrint_SUB( SubScreen, 2*6, 14*12+6, (u8 *)"                          ", 1, 0, 1);
				}
			}
		} else {
//			DrawBox_SUB(SubScreen, 75, 115, 181, 136, 1, 0);
//			DrawBox_SUB(SubScreen, 76, 116, 180, 135, 6, 1);
//			DrawBox_SUB(SubScreen, 77, 117, 179, 134, 5, 0);

			if(softReset) {
				ShinoPrint_SUB( SubScreen, 2*6, 14*12+6, (u8 *)t_msg[6], 1, 0, 1);
			} else {
				ShinoPrint_SUB( SubScreen, 2*6, 14*12+6, (u8 *)t_msg[7], 1, 0, 1);
			}
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
			if(i == yc) { _gba_dsp(i + st, 1, x, y + i); } else { _gba_dsp(i + st, 0, x, y + i); }
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
	if(softReset)	cn++;

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
			if(softReset) {
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


int gba_sel() {
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
	if(softReset)	cn++;

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


		if((ky & KEY_L) && !isSuperCard) {
			if(GBAmode > 0) {
				GBAmode--;
				if ((GBAmode == 1) && isOmega)GBAmode--;
				setGBAmode();
				_gba_sel_dsp(sel, yc, 0);
//				cmd = -1;
//				break;
			}
		}
		if((ky & KEY_R) && !isSuperCard) {
			if(softReset && (carttype > 2)) {
				cmd = 3;
				break;
			} else if(GBAmode < cn && carttype <= 2) {
				GBAmode++;
				if ((GBAmode == 1) && isOmega)GBAmode++;
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
			if(softReset && !isOmega) {
				cmd = 99;
				if(carttype == 1) {
					SetRompage(0);
					SetRampage(16);
				}
				break;
			} else if (isOmega && (GBAmode == 0)) {
				SetRompage(0x8002);
				gbaMode();
			}
		}
		if(ky & KEY_SELECT) {
			if(softReset && (GBAmode == 0)) {
				if(!(fs[sortfile[sel]].type & S_IFDIR))
					ret = writeFileToRam(sortfile[sel]);
				if(ret != 0) {
					_gba_sel_dsp(sel, yc, 0);
					err_cnf(7, 8);
				} else {
//					if(carttype == 3)
//						SetRompage(0x300);
//					else	SetRompage(384);
					turn_off(softReset);
				}
			}
		}

		if(ky & KEY_X) {
			if(GBAmode == 1) {
				// if (!is3in1Plus)SetRompage(0);
				SetRompage(0);
				SetRampage(16);
				gbaMode();
			} else {
				if(cnf_inp(7, 8) & KEY_A)SRAMdump(0);
			}
		}

		if(ky & KEY_Y) {
			if(GBAmode == 1) {
				if(checkSRAM(filename)) {
//					if(cnf_inp(3, 4) & KEY_A)
					if(save_sel(0, filename) >= 0) { 
						dsp_bar(5, -1);
						swiWaitForVBlank();
						dsp_bar(5, 50);
						writeSramFromFile(filename);
						dsp_bar(5, 100);
						for (int I = 0; I < 50; I++)swiWaitForVBlank();
						dsp_bar(-1, 100);
					}
					_gba_sel_dsp(sel, yc, 0);
				} else {
					err_cnf(4, 5);
				}
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
			if(GBAmode == 0) { ret = writeFileToRam(sortfile[sel]); } else { ret = writeFileToNor(sortfile[sel]); }
			if(ret != 0) {
				if(ret == 2) {
					err_cnf(9, 10);
				} else {
					if(GBAmode == 0 && carttype < 3) { err_cnf(7, 8); } else { err_cnf(7, 6); }
				}
			} else {
				if(GBAmode == 0) {
					// if (is3in1Plus)SetRompage(0x100);
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
					dsp_bar(4, -1);
					swiWaitForVBlank();
					dsp_bar(4, 50);
					writeSramToFile(filename);
					dsp_bar(4, 100);
					for (int I = 0; I < 50; I++)swiWaitForVBlank();
					dsp_bar(-1, 100);
				}
				_gba_sel_dsp(sel, yc, 0);
			} else {
				err_cnf(4, 5);
			}
		}
	}
	return(cmd);
}


// extern u32	_io_dldi;

extern void setLang(void);


void mainloop(void) {
//	vu16	reg;

	// FILE	*r4dt;
	// __handle *handle;
	// _FILE_STRUCT *file;
	// FILE *file;
	// PARTITION *part;

	int	cmd;

	keysSetRepeat(20, 6);		// def. 60, 30 (delay, repeat)

	DrawBox_SUB(SubScreen, 20, 3, 235, 27, 1, 0);
	DrawBox_SUB(SubScreen, 21, 4, 234, 26, 5, 1);
	DrawBox_SUB(SubScreen, 22, 5, 233, 25, 0, 0);
	ShinoPrint_SUB( SubScreen, 9*6, 1*12-2, (u8*)"GBA ExpLoader", 0, 0, 0);
	// ShinoPrint_SUB( SubScreen, 33*6-2, 12, (u8*)VERSTRING, 0, 0, 0);
	ShinoPrint_SUB( SubScreen, 34*6-2, 12, (u8*)VERSTRING, 0, 0, 0);


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

	setLangMsg();
	
	if(isDSiMode()) { err_cnf(14, 15); turn_off(0); }

	/*CloseNorWrite();
	SetRompage(0);
	SetRampage(16);
	SetShake(0x08);*/


/********
	ram_init(DETECT_RAM);
	sprintf(tbuf, "%s %dKB", ram_type_string(), ram_size());
	ShinoPrint_SUB( SubScreen, 9*6, 5*12, tbuf, 1, 0, 0 );
	inp_key();
********/
	if(!fatInitDefault()) { err_cnf(0, 1); turn_off(0); }
		
	checkFlashID();
	
	switch (carttype) {
		default: 
			err_cnf(2, 3);
			turn_off(softReset);
			break;
		case 0: 
			err_cnf(2, 3);
			turn_off(softReset);
			break;
		case 1: 
			if (is3in1Plus) {
				ShinoPrint_SUB( SubScreen, 23*6, 1*12-2, (u8*)"[3in1Pls]", 0, 0, 0 );
			} else if (isOmega) {
				ShinoPrint_SUB( SubScreen, 23*6, 1*12-2, (u8*)"[ Ω DE ]", 0, 0, 0 );
			} else {
				ShinoPrint_SUB( SubScreen, 23*6, 1*12-2, (u8*)" [ 3in1 ]", 0, 0, 0 );
			}
			break; // SetRampage(16); // SetShake(0x08);
		case 2: 
			ShinoPrint_SUB( SubScreen, 23*6, 1*12-2, (u8*)"[New3in1]", 0, 0, 0 ); break;
		case 3:
			SetRompage(0x300);
			ShinoPrint_SUB( SubScreen, 23*6, 1*12-2, (u8*)"  [ EZ4 ]", 0, 0, 0 );
			break;
		case 4: ShinoPrint_SUB( SubScreen, 23*6, 1*12-2, (u8*)"[EXP256K]", 0, 0, 0 ); break;
		case 5: ShinoPrint_SUB( SubScreen, 23*6, 1*12-2, (u8*)"[EXP128K]", 0, 0, 0 ); break;
		case 6: 
			if (isSuperCard) {
				ShinoPrint_SUB( SubScreen, 23*6, 1*12-2, (u8*)"[ SC ]", 0, 0, 0 );
			} else {
				ShinoPrint_SUB( SubScreen, 23*6, 1*12-2, (u8*)"[ M3/G6 ]", 0, 0, 0 ); 
			}
			break;
	}
	ShinoPrint_SUB( SubScreen, 9*6, 5*12, (u8 *)t_msg[16], 1, 0, 0 );
	// if(!fatInitDefault()) { err_cnf(0, 1); turn_off(0); }

//ShinoPrint_SUB( SubScreen, 6*6, 6*12, "FAT OK", 1, 0, 0 );

/*********************
	sprintf(tbuf, "0x27FFE18 = %08X", (*(vu32*)0x027FFE18));
	ShinoPrint_SUB( SubScreen, 8*6, 5*12, (u8*)tbuf, 3, 0, 1);
**********************/

	// SuperCard and EZFlash Omega does not support 3in1's Rumble commands. :P
	/*if (isOmega) {
		softReset = false;
	} else */ if (isSuperCard) {
		softReset = false;
	} else {
		softReset = ret_menu_chk();
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

	if(!checkSRAM_cnf() && (carttype != 5) && (cnf_inp(9, 10) & KEY_B))turn_off(softReset);

//ShinoPrint_SUB( SubScreen, 6*6, 7*12, "FILE LIST", 1, 0, 0 );
//	mkdir("/GBA_SAVE", 0777);
//	mkdir("/GBA_SIGN", 0777);
	getcurpath();
	FileListGBA();
//ShinoPrint_SUB( SubScreen, 6*6, 7*12, "FILE LIST -- OK", 1, 0, 0 );

	_dsp_clear();

	GBAmode = 0;
	if(checkSRAM(filename) && checkBackup()) {
		dsp_bar(4, -1);
		dsp_bar(4, 0);
		for (int I = 0; I < 30; I++)swiWaitForVBlank();
		if(save_sel(1, filename) >= 0) { 
			writeSramToFile(filename);
			dsp_bar(4, 50);
			for (int I = 0; I < 30; I++)swiWaitForVBlank();
			dsp_bar(4, 100);
			for (int I = 0; I < 30; I++)swiWaitForVBlank();
			dsp_bar(-1, 100);
		} else {
			dsp_bar(-1, 100);
		}
	}

	getGBAmode();
	if((GBAmode == 2) && !softReset)GBAmode = 0;
	if(carttype > 2)GBAmode = 0;

	cmd = -1;
	while(cmd == -1) {
		if(GBAmode == 2) {
			cmd = rumble_cmd();
		} else {
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
			if (!isSuperCard)SetShake(0xF0);
			break;
		case 1:
			if (!isSuperCard)SetShake(0xF1);
			break;
		case 2:
			if (!isSuperCard)SetShake(0xF2);
			break;
		case 3:
			if((carttype != 4) && !isSuperCard && !isOmega) {
				if (isOmega) {
					SetRompage(0x8002);
				} else {
					if (is3in1Plus) { SetRompage(0x100); } else { SetRompage(0x300); }
					OpenNorWrite();
				}
			}
			if(!isSuperCard && !isOmega)RamClear();
			break;
	}

	turn_off(softReset);
}


//---------------------------------------------------------------------------------
int main(void) {
//---------------------------------------------------------------------------------
	extern u64 *fake_heap_end;
	*fake_heap_end = 0;
	
	defaultExceptionHandler();
	
	int	i;

	vramSetPrimaryBanks(VRAM_A_LCD, VRAM_B_LCD, VRAM_C_SUB_BG, VRAM_D_MAIN_BG);
	powerOn(POWER_ALL);

	videoSetMode(MODE_FB0 | DISPLAY_BG2_ACTIVE);
	videoSetModeSub(MODE_0_2D | DISPLAY_BG0_ACTIVE );
	REG_BG0CNT_SUB = BG_256_COLOR | BG_MAP_BASE(0) | BG_TILE_BASE(1);
	uint16* map1 = (uint16*)BG_MAP_RAM_SUB(0);
	for(i=0;i<(256*192/8/8);i++)map1[i]=i;
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

	swiWaitForVBlank();
		
	sysSetBusOwners(BUS_OWNER_ARM9,BUS_OWNER_ARM9);

	mainloop();

	return 0;
}

