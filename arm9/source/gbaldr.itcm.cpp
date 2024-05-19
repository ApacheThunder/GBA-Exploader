#include <nds.h>

#include <fat.h>
#include <sys/iosupport.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <string>
#include <vector>

//#include "disc_io.h"
//#include "gba_nds_fat.h"

//#include "unicode.h"
#include "skin.h"
#include "gba_patch.h"
#include "GBA_ini.h"
#include "header_rep.h"
#include "ctrl_tbl.h"
//#include <nds/registers_alt.h>
//#include <nds/arm9/rumble.h>

#include "dsCard.h"

//#include <nds/arm9/console.h> //basic print funcionality

#define MAX_NOR		0x2000000 	// 32MByte 
#define MAX_NORPLUS		0x4000000 	// 64MByte  (3 in 1 Plus)
#define MAX_PSRAM	0x1000000 	// 16MByte
#define SRAM_PAGE_SIZE	0x1000	  	// SRAM Page Size
#define MAX_SRAM	0x80000		// 4MBit/512KByte total SRAM

#define	SRAM_ADDR	0x0A000000
#define	USE_SRAM	0x20000		// 128KByte
#define	USE_SRAM_PG		48		// 0x0A030000-0A031FFF
#define	USE_SRAM_PG_EZ4		11		// 0x0A008000-0A00FFFF
#define	USE_SRAM_PG_EWN		10		// 0x0A020000-0A02FFFF
#define	USE_SRAM_PG_EWN128	 9		// 0x0A010000-0A01FFFF
#define	USE_SRAM_PG_M3		 6


#define	USE_SRAM_NOR		16		// 0x0A010000-0A02FFFF

#define	USE_SRAM_PSR		50		// 0x0A032000-0A051FFF
#define	USE_SRAM_PSR_EZ4	16		// 0x0A010000-0A02FFFF
#define	USE_SRAM_PSR_EWN	 8		// 0x0A000000-0A01FFFF
#define	USE_SRAM_PSR_M3		 4

#define PSRAM_BUF	0x8000		// 32KB

#define poke(addr) do{(void)*(vu8*)addr;} while(0)
#define poke16(addr) do{(void)*(vu16*)addr;} while(0)

#ifdef __cplusplus
extern "C" {
#endif


extern char curpath[];
extern int sortfile[];

extern struct GBA_File fs[];
extern char tbuf[];
extern u8 *rwbuf;

extern int numFiles;
extern int numGames;

extern int GBAmode;

static u32 savesize;

int	carttype = 0;
bool isSuperCard = false;
bool is3in1Plus = false;

extern int save_sel(int mod, char *name);

using namespace std;

void SetEWINRam(u8 page) {
	vu32 wait;
	// vu8	a;


	poke(0x0A005555);
	poke(0x0A002AAA);
	poke(0x0A00B055);

	*(vu8*)0x0A000000 = page;

	for(wait = 0; wait < 15000; wait++);	// 1ms

	poke(0x0A000000);
	poke(0x0A000000);
	poke(0x0A000000);
}


int cehck_EWIN() {
	vu32	wait;
	vu8	a, a8, a9, aa, org;


	for(wait = 0; wait < 15000; wait++) {
		a = *(vu8*)(0x0A000000 + wait);
	}

	org = *(vu8*)0x0A000000;

	SetEWINRam(0);

	a = *(vu8*)0x0A005555;
	a = *(vu8*)0x0A002AAA;
	a = *(vu8*)0x0A009055;

	a = *(vu8*)0x0A000000;

	SetEWINRam(0x08);

	a = *(vu8*)0x0A005555;
	a = *(vu8*)0x0A002AAA;
	a = *(vu8*)0x0A009055;

	a = *(vu8*)0x0A000000;
	if(a != 0xB8) {
		*(vu8*)0x0A000000 = org;
		return(0);
	}

	SetEWINRam(0x08);
	a8 = *(vu8*)0x0A000600;

	SetEWINRam(0x09);
	a9 = *(vu8*)0x0A000600;

	*(vu8*)0x0A000600 = a8 ^ 0xFF;

	SetEWINRam(0x08);
	a = *(vu8*)0x0A000600;
	*(vu8*)0x0A000600 = a8;

	if(a != a8) {
		*(vu8*)0x0A000000 = org;
		return(0);
	}

	SetEWINRam(0x09);
	*(vu8*)0x0A000600 = a9;


	SetEWINRam(0x0A);
	aa = *(vu8*)0x0A000600;

	*(vu8*)0x0A000600 = a8 ^ 0xFF;

	SetEWINRam(0x08);
	a = *(vu8*)0x0A000600;
	*(vu8*)0x0A000600 = a8;
	if(a != a8)
		return(2);

	SetEWINRam(0x0A);
	*(vu8*)0x0A000600 = aa;

	SetEWINRam(0x08);

	return(1);
}

bool Close_EWIN() {
	vu8	a;


	SetEWINRam(0x0C);

	a = *(vu8*)0x0A005555;
	a = *(vu8*)0x0A002AAA;
	a = *(vu8*)0x0A009055;

	a = *(vu8*)0x0A000000;

	if(a != 0xBC)	return false;
	a = *(vu8*)0x0A000000;

	return true;
}


void SetM3Ram(u8 page) {
	u32	mode;
	// vu16 tmp;

	mode = (u32)page << 5;

// M3

	poke16(0x08000000);
	poke16(0x08E00002);
	poke16(0x0800000E);
	poke16(0x08801FFC);
	poke16(0x0800104A);
	poke16(0x08800612);
	poke16(0x08000000);
	poke16(0x08801B66);
	poke16((0x08000000 + mode));
	poke16(0x0800080E);
	poke16(0x08000000);

// G6
	poke16(0x09000000);
	poke16(0x09FFFFE0);
	poke16(0x09FFFFEC);
	poke16(0x09FFFFEC);
	poke16(0x09FFFFEC);
	poke16(0x09FFFFFC);
	poke16(0x09FFFFFC);
	poke16(0x09FFFFFC);
	poke16(0x09FFFF4A);
	poke16(0x09FFFF4A);
	poke16(0x09FFFF4A);

	poke16((0x09800000 + (mode << 12)));
//	poke16(0x09FFFFF0);
	poke16(0x09FFFFE8);
}

bool _set_M3(int sw) {
	vu32 wait;
	// vu16 tmp;
	// vu8	a;

// M3
	poke16(0x08000000);
	poke16(0x08E00002);
	poke16(0x0800000E);
	poke16(0x08801FFC);
	poke16(0x0800104A);
	poke16(0x08800612);
	poke16(0x08000000);
	poke16(0x08801B66);
	if(sw == 0)poke16(0x08800004);
	if(sw == 1)poke16(0x0880000C);
	if(sw == 2)poke16(0x08800008);
	poke16(0x0800080E);
	poke16(0x08000000);

	if(sw == 2) {
		poke16(0x080001E4);
		poke16(0x080001E4);
		poke16(0x08000188);
		poke16(0x08000188);
	}

// G6
	poke16(0x09000000);
	poke16(0x09FFFFE0);
	poke16(0x09FFFFEC);
	poke16(0x09FFFFEC);
	poke16(0x09FFFFEC);
	poke16(0x09FFFFFC);
	poke16(0x09FFFFFC);
	poke16(0x09FFFFFC);
	poke16(0x09FFFF4A);
	poke16(0x09FFFF4A);
	poke16(0x09FFFF4A);

	if(sw == 0)poke16(0x09200004);
	if(sw == 1)poke16(0x0920000C);
	if(sw == 2)poke16(0x09200008);
	poke16(0x09FFFFF0);
	poke16(0x09FFFFE8);

// GBA Pack
	poke(0x0A000000);
	poke(0x0A009999);
	poke(0x0A009999);
	poke(0x0A006666);
	poke(0x0A006666);
	poke(0x0A000001);
	poke16(0x08000000); // tmp

	poke(0x0A000000);
	poke(0x0A009999);
	poke(0x0A009999);
	poke(0x0A006666);
	poke(0x0A006666);
	poke(0x0A000006);
	poke16(0x08000800); // tmp

	poke(0x0A000000);
	poke(0x0A009999);
	poke(0x0A009999);
	poke(0x0A006666);
	poke(0x0A006666);
	poke(0x0A000008);
	poke16(0x09000040); // tmp

	poke(0x0A000000);
	poke(0x0A009999);
	poke(0x0A009999);
	poke(0x0A006666);
	poke(0x0A006666);
	poke(0x0A000008);
	poke16(0x09000060); // tmp

	poke(0x0A000000);
	poke(0x0A009999);
	poke(0x0A009999);
	poke(0x0A006666);
	poke(0x0A006666);
	poke(0x0A000006);
	poke16(0x08000800); // tmp

	poke(0x0A000000);
	poke(0x0A009999);
	poke(0x0A009999);
	poke(0x0A006666);
	poke(0x0A006666);
	poke(0x0A000007);
	poke16(0x08000000); // tmp

	poke(0x0A000000);
	poke(0x0A009999);
	poke(0x0A009999);
	poke(0x0A006666);
	poke(0x0A006666);
	poke(0x0A000003);

	poke(0x0A000000);
	poke(0x0A009999);
	poke(0x0A009999);
	poke(0x0A006666);
	poke(0x0A006666);
	poke(0x0A000005);
	poke16(0x08000000); // tmp

	for(wait = 0; wait < 15000; wait++);	// 1ms

	poke(0x0A000000);
	poke(0x0A009999);
	poke(0x0A009999);
	poke(0x0A006666);
	poke(0x0A006666);
	poke(0x0A00000A);
	poke16(0x08000000); // tmp

	poke(0x0A000000);
	poke(0x0A009999);
	poke(0x0A009999);
	poke(0x0A006666);
	poke(0x0A006666);
	poke(0x0A000003);

	return true;
}

bool Close_M3() {
	_set_M3(0);
	return true;
}

int cehck_M3() {
	vu16 tmp;

	_set_M3(1);
	tmp = *(vu16*)0x08000000;

	*(vu16*)0x08000000 = 0x0123;
	if(*(vu16*)0x08000000 == 0x0123) {
		Close_M3();
		*(vu16*)0x08000000 = 0xABCD;
		if(*(vu16*)0x08000000 != 0xABCD) {
			_set_M3(1);
			*(vu16*)0x08000000 = tmp;
			return true;
		}
	}

	*(vu16*)0x08000000 = tmp;
	return false;
}


void _RamPG() {
	switch (carttype) {
		case 3:
			SetRampage(USE_SRAM_PG_EZ4);
			return;
		case 4:
			SetEWINRam(USE_SRAM_PG_EWN);
			return;
		case 5:
			SetEWINRam(USE_SRAM_PG_EWN128);
			return;
		case 6:
			SetM3Ram(USE_SRAM_PG_M3);
			return;
	}
	SetRampage(USE_SRAM_PG);
	return;
}

void _RamSave(int bnk) {
	switch (carttype) {
		case 3:
			SetRampage(USE_SRAM_PSR_EZ4 + bnk * 16);
			return;
		case 6:
			SetM3Ram(USE_SRAM_PSR_M3 + bnk);
			return;
	}
	
	if(carttype >= 4) {
		SetEWINRam(USE_SRAM_PSR_EWN + bnk);
		return;
	}

	if(GBAmode == 0) { SetRampage(USE_SRAM_PSR + bnk * 16); } else { SetRampage(USE_SRAM_NOR + bnk * 16); }
	return;
}

int checkFlashID() {
	int	ewin;
	u32	id;

	OpenNorWrite();
	id = ReadNorFlashID();
	chip_reset();
	if(id == 0) {
		id = ReadNorFlashID();
		chip_reset();
	}
	CloseNorWrite();

	carttype = id;
	
	switch (id) {
		case 0:
			if (cehck_M3()) { 
				carttype = 6; // M3/G6
				return carttype; 
			}
			ewin = cehck_EWIN();
			if(ewin > 0)carttype = 3 + ewin; // EWIN
			return carttype;
		case 0x227E2218: carttype = 1; return carttype; // 3in1
		case 0x227E2202: carttype = 2; return carttype; // New3in1
		case 0x227E2220: carttype = 3; return carttype; // EZ4
		case 0x227E0000: // SuperCard
			carttype = 6; 
			is3in1Plus = false;
			isSuperCard = true;
			return carttype;
		case 0x89168916: // 3in1 Plus
			is3in1Plus = true;
			isSuperCard = false;
			carttype = 1; 
			return carttype;
		default: return 0; // Unsupported/Unimplemented Cart ID
	}
}



char const *Rudolph = "GBA ExpLoader by Rudolph (LocalCode v0.1)";

bool checkSRAM_cnf() {
	int	i;

	ctrl_get();

	for(i = 0; ctrl.sign[i] != 0; i++) {
		if(ctrl.sign[i] != Rudolph[i])break;
	}

//	_RamSave(0);
	if(Rudolph[i] != 0)
		return false;
	return true;
}


int checkSRAM(char *name) {
	int	i, ln;


	ctrl_get();

	for(i = 0; ctrl.sign[i] != 0; i++) {
		if(ctrl.sign[i] != Rudolph[i])
			break;
	}

	if(carttype < 4)OpenNorWrite();

	if(Rudolph[i] != 0) {
		strcpy((char *)ctrl.sign, Rudolph);
		ctrl_set();
		if(carttype < 4)
			CloseNorWrite();
		return false;
	}

	savesize = ctrl.save_siz[GBAmode];
	if((savesize < 0x2000) || (savesize > USE_SRAM)) {
		savesize = 0x10000;
		ctrl.save_siz[GBAmode] = savesize;
	}

	strcpy(name, (char *)ctrl.sav_nam[GBAmode]);
	ln = strlen(name) - 3;
	if((name[ln-1] != '.') || (name[ln] != 's' && name[ln] != 'S')) {
//	if((name[ln] != 's' && name[ln] != 'S') || (name[ln+1] != 'a' && name[ln+1] != 'A') || (name[ln+2] != 'v' && name[ln+2] != 'V')) {
		memset(ctrl.sav_nam[GBAmode], 0, 512);
		name[0] = 0;
		savesize = 0x10000;
		ctrl.save_siz[GBAmode] = savesize;
		ctrl_set();
		if(carttype < 4)CloseNorWrite();
		return false;
	}

	if(carttype < 4)CloseNorWrite();
	return true;
}

void setGBAmode() {
	if(ctrl.mode != (char)GBAmode) {
		ctrl.mode = (char)GBAmode;
//		if(carttype < 4)
//			OpenNorWrite();
//		ctrl_set();
//		if(carttype < 4)
//			CloseNorWrite();
	}
}

void getGBAmode() {
//	ctrl_get();
	GBAmode = ctrl.mode;
	if((GBAmode < 0) || (GBAmode > 2)) {
		GBAmode = 0;
		ctrl.mode = 0;
	}
}

void setcurpath() {
	memset(ctrl.path, 0, 256);
	strcpy((char *)ctrl.path, curpath);
	ctrl_set();

}

void getcurpath() {
	if(ctrl.path[0] != '/') {
		ctrl.path[0] = '/';
		ctrl.path[1] = 0;
	}
	strcpy(curpath, (char *)ctrl.path);
}

extern	void err_cnf(int n1, int n2);

/*************
bool getSaveFilename(int sel, char *savename)
{
	DIR_ITER	*dir;
	struct stat	st;
	char	fname[256];
	char	lfnname[512];
	int	len, ln;
	bool	exist;


	len = strlen(fs[sel].filename) - 3;
	exist = false;

	dir = diropen("/GBA_SAVE");
        while((dir != NULL) && (dirnextl(dir, fname, lfnname, &st) == 0)) {
		if(st.st_mode & S_IFDIR)
			continue;

		ln = strlen(fname);
		if(ln < 5)	continue;
		if((fname[ln - 4] != '.') || (fname[ln - 3] != 'S') || (fname[ln - 2] != 'A') || (fname[ln - 1] != 'V'))
			continue;

		if(strncmp(fs[sel].filename, lfnname, len) == 0) {
			exist = true;
			break;
		}
	}

	if(exist) {
		strcpy(savename, lfnname);
		return true;
	}

	strncpy(savename, fs[sel].filename, len);
	savename[len] = 0;
	strcat(savename, "sav");
	return false;

}
*******************/

bool checkBackup() {
	if(GBAmode == 1)return true;

	if(ctrl.save_flg[GBAmode] == 0xFF)return false;

	return true;
}


void writeSramToFile(char *savename) {
	FILE	*saver;
	u32	len;

	if(savename[0] == 0) return;

//	if((GBAmode == 0) && (checkBackup() == false))
//		return;

//	getSaveFilename(filename, tbuf);
	sprintf(tbuf, "%s/%s", ini.save_dir, savename);
	saver = fopen(tbuf, "wb");
//	memset(rwbuf, 0, USE_SRAM / 2);
//	OpenNorWrite();

	_RamSave(0);

	ReadSram(SRAM_ADDR, rwbuf, USE_SRAM / 2);
	if(saver != NULL) {
		len = USE_SRAM / 2;
		if(len > savesize)len = savesize;
		fwrite(rwbuf, 1, len, saver);
	}
	if(savesize > USE_SRAM / 2) {
		_RamSave(1);
		ReadSram(SRAM_ADDR, rwbuf, USE_SRAM / 2);
		if(saver != NULL)fwrite(rwbuf, 1, USE_SRAM / 2, saver);
	}
	fclose(saver);

	ctrl.save_flg[GBAmode] = 0xFF;

	if(carttype < 4)OpenNorWrite();

	ctrl_set();

	if(carttype < 4)CloseNorWrite();
}

void _WritePSram(uint32 address, u8* data , uint32 size );

void SRAMdump(int cmd) {
	FILE	*dmp;
	int	i;
	int	mx;
	char	name[256];

	mx = 8;
	if(carttype == 4)
		mx = 4;
	if(carttype == 5)
		mx = 2;
	if(carttype == 6)
		mx = 16;


	sprintf(name, "%s/SRAM.BIN", ini.save_dir);
	if(cmd == 0) {
		dmp = fopen(name, "wb");
		for(i = 0; i < mx; i++) {
			if(carttype == 6) {
				SetM3Ram(i);
			} else {
				if(carttype >= 4)
					SetEWINRam(8 + i);
				else	SetRampage(i * 16);
			}
			ReadSram(SRAM_ADDR, rwbuf, USE_SRAM / 2);
			if(dmp != NULL)
				fwrite(rwbuf, 1, USE_SRAM / 2, dmp);
		}
	} else {
		dmp = fopen(name, "rb");
		if(carttype < 4)
			OpenNorWrite();

		for(i = 0; i < mx; i++) {
			memset(rwbuf, 0, USE_SRAM / 2);
			if(dmp != NULL)
				fread(rwbuf, 1, USE_SRAM / 2, dmp);
			if(carttype == 6) {
				SetM3Ram(i);
			} else {
				if(carttype >= 4)
					SetEWINRam(8 + i);
				else	SetRampage(i * 16);
			}
			WriteSram(SRAM_ADDR, rwbuf, USE_SRAM / 2);
		}

		if(carttype < 4)
			CloseNorWrite();
	}
	fclose(dmp);
	_RamSave(0);
}

void blankSRAM(char *savename) {

	memset(rwbuf, 0xFF, USE_SRAM / 2);

	if(carttype < 4)
		OpenNorWrite();

	_RamSave(0);
	WriteSram(SRAM_ADDR, rwbuf, USE_SRAM / 2);

//	if(carttype != 5) {
		_RamSave(1);
		WriteSram(SRAM_ADDR, rwbuf, USE_SRAM / 2);
//	}

	ctrl.save_siz[GBAmode] = savesize;
	ctrl.save_flg[GBAmode] = 0x00;
	memset(ctrl.sav_nam[GBAmode], 0, 512);
	strcpy((char *)ctrl.sav_nam[GBAmode], savename);
	ctrl_set();

	if(carttype < 4)
		CloseNorWrite();
}

void writeSramFromFile(char *savename) {	
	FILE	*saver;

	if(savename[0] == 0) return;

	sprintf(tbuf, "%s/%s", ini.save_dir, savename);
//	getSaveFilename(filename, tbuf);

	saver = fopen(tbuf, "rb");
	if(saver == NULL) {
		blankSRAM(savename);
//		writeSramToFile(uniName);
		return;
	}


	if(carttype < 4)
		OpenNorWrite();

	ctrl.save_siz[GBAmode] = savesize;
	ctrl.save_flg[GBAmode] = 0x00;
	memset(ctrl.sav_nam[GBAmode], 0, 512);
	strcpy((char *)ctrl.sav_nam[GBAmode], savename);
	ctrl_set();

//	OpenNorWrite();

	_RamSave(0);

	memset(rwbuf, 0xFF, USE_SRAM / 2);
	fread(rwbuf, 1, USE_SRAM / 2, saver);
	WriteSram(SRAM_ADDR, rwbuf, USE_SRAM / 2);

//	if(carttype != 5) {
		if(savesize > USE_SRAM / 2) {
			_RamSave(1);
			memset(rwbuf, 0xFF, USE_SRAM / 2);
			fread(rwbuf, 1, USE_SRAM / 2, saver);
			WriteSram(SRAM_ADDR, rwbuf, USE_SRAM / 2);
			_RamSave(0);
		}
//	}

	if(carttype < 4)
		CloseNorWrite();

	fclose(saver);
}


void _ReadPSram(uint32 address, u8* data , uint32 size ) {
	u32	i;
	u16* pData = (u16*)data;
	u16* sData = (u16*)address;

	for(i = 0; i < size / 2; i++)
		pData[i] = sData[i];
}

void _WritePSram(uint32 address, u8* data , uint32 size ) {
	u32	i;
	u16* sData = (u16*)data;
	u16* pData = (u16*)address;

	for(i = 0; i < size / 2; i++)pData[i] = sData[i];
}


extern	void turn_off(int cmd);
extern	void dsp_bar(int mod, int per);


int writeFileToNor(int sel) {
	FILE	*gbaFile;
	char	savName[512];
	u32	siz, wsz;
	u32	exp, ofs;
	u32	fsz;
	int	cmd;
	bool	gba;

	if (is3in1Plus) { fsz = MAX_NORPLUS; } else { fsz = MAX_NOR; }
	
	exp = 0x00000000;

	if(fs[sel].filesize > fsz)return 1;

	if(checkSRAM(savName) == false) {
		err_cnf(4, 5);
	} else {
		if(save_sel(1, savName) >= 0)writeSramToFile(savName);
	}

	// if((fs[sel].Alias[strlen(fs[sel].Alias) - 3] != 'G') || (fs[sel].Alias[strlen(fs[sel].Alias) - 3] != 'g')) {
	if((fs[sel].filename[strlen(fs[sel].filename) - 3] != 'G') || (fs[sel].filename[strlen(fs[sel].filename) - 3] != 'g')) {
		gba = false;
	} else {
		gba = true;
	}

	sprintf(tbuf, "%s%s", curpath, fs[sel].filename);

	gbaFile = fopen(tbuf, "rb");
	if(gbaFile == NULL)	return(0);

	strcpy(savName, fs[sel].filename);
	cmd = save_sel(0, savName);

	SetRompage(0);
	OpenNorWrite();
	SetSerialMode();
	dsp_bar(0, -1);

	for(siz = 0; siz < fs[sel].filesize; siz += 0x40000) {
		dsp_bar(0, siz * 100 / fs[sel].filesize);
		if (is3in1Plus) { Block_EraseIntel(siz); } else { Block_Erase(siz); }
	}
	
	dsp_bar(0, 100);

	chip_reset();
	CloseNorWrite();
	
	SetRampage(USE_SRAM_NOR);
	OpenNorWrite();
	SetSerialMode();

	gba_check_int(fs[sel].filename);
	dsp_bar(-1, 100);

	savesize = gba_check(gbaFile, fs[sel].filesize, rwbuf, PSRAM_BUF*32);
	dsp_bar(1, -1);
	
	for(siz = 0; siz < fs[sel].filesize; siz += PSRAM_BUF*32) {
		fread(rwbuf, 1, PSRAM_BUF*32, gbaFile);

//		dsp_bar(1, siz * 100 / fs[sel].filesize);

		if(siz == 0 && gba)header_rep(rwbuf);

		wsz = fs[sel].filesize - siz;
		if(wsz > PSRAM_BUF*32)	wsz = PSRAM_BUF*32;

		gba_patch(rwbuf, siz, wsz, GBAmode, fs[sel].filename);

		for(ofs = 0; ofs < wsz; ofs += PSRAM_BUF) {
			dsp_bar(1, (siz + ofs) * 100 / fs[sel].filesize);
			WriteNorFlash(exp, rwbuf + ofs, PSRAM_BUF);
			exp += PSRAM_BUF;
		}
	}
	
	dsp_bar(1, 100);
	fclose(gbaFile);
	CloseNorWrite();

//	getSaveFilename(sel, savName);
	if(cmd >= 0) { writeSramFromFile(savName); } else {	blankSRAM(savName);	}

	dsp_bar(-1, 100);
//	SetRampage(USE_SRAM_NOR);
	return(0);
}


int writeFileToRam(int sel) {
	FILE *gbaFile;
	char savName[512];
	u32	siz;
	u32	exp, exps;
	u32	fsz;
	int	cmd;
	bool	gba;

	if(carttype >= 3) {	fsz = MAX_NOR; } else { fsz = MAX_PSRAM; }

	if(carttype >= 4) {	exp = 0x08000000; } else { exp = 0x08060000; }
	exps = exp;

	if(fs[sel].filesize > fsz)return 1;


//	if(checkSRAM(savName) == false) {
//		err_cnf(4, 5);
//	} else	writeSramToFile(savName);

	// if((fs[sel].Alias[strlen(fs[sel].Alias) - 3] != 'G') || (fs[sel].Alias[strlen(fs[sel].Alias) - 3] != 'g')) {
	if((fs[sel].filename[strlen(fs[sel].filename) - 3] != 'G') || (fs[sel].filename[strlen(fs[sel].filename) - 3] != 'g')) { gba = false; } else { gba = true; }

	sprintf(tbuf, "%s%s", curpath, fs[sel].filename);

	gbaFile = fopen(tbuf, "rb");
	if(gbaFile == NULL)	return(0);

	strcpy(savName, fs[sel].filename);
	cmd = save_sel(0, savName);

//	SetRampage(USE_SRAM_PSR);
	if(carttype < 4) {
		if(carttype == 3) { SetRompage(0x300 - 3); } else { SetRompage(384 - 3); }
		OpenNorWrite();
	}
//	savesize = gba_check(gbaFile, fs[sel].filesize, rwbuf, 0x100000);
//	if(savesize == 0)savesize = 0x8000;


	dsp_bar(2, -1);
	gba_check_int(fs[sel].filename);

	for(siz = 0; siz < fs[sel].filesize; siz += 0x100000, exp += 0x100000) {
		fseek(gbaFile, siz, SEEK_SET);
		fread(rwbuf, 1, 0x100000+0x400, gbaFile);

		dsp_bar(2, siz * 100 / fs[sel].filesize);

		if(siz == 0 && gba)header_rep(rwbuf);

		savesize = gba_check_Ram1(rwbuf, 0x100000, fs[sel].filesize, siz);

		_WritePSram(exp, rwbuf, 0x100000);
//		dmaCopy(rwbuf, (void *)exp, 0x100000);
//		dmaCopyWords(3, rwbuf, (void *)exp, 0x100000);
	}

	dsp_bar(2, 100);
	gba_check_Ram2(exps, rwbuf, 0x100000, fs[sel].filesize);
	gba_patch_Ram(exps, fs[sel].filename, carttype);

	fclose(gbaFile);

//	if(carttype == 5 && savesize == 0x20000) {
//		dsp_bar(-1, 100);
//		return(2);
//	}

//	getSaveFilename(sel, savName);
	if(cmd >= 0) { writeSramFromFile(savName); } else { blankSRAM(savName); }

	dsp_bar(-1, 100);

	if(carttype < 4)CloseNorWrite();

	_RamSave(0);
	if(carttype >= 4 && !isSuperCard) {
		if(carttype == 6) { Close_M3(); } else { Close_EWIN(); }
	}
	if(carttype == 3)SetRompage(0x300);
	if(carttype <= 2)SetRompage(384);

	return(0);
}


void QSort(int left, int right) {
	int	i, j;
	int	p;
	int	tmp;

	i = left;
	j = right;

	p = sortfile[(i + j) / 2];


	while(1) {
		while(strcmp((char *)&(fs[sortfile[i]].type), (char *)&(fs[p].type)) < 0)
			i++;
		while(strcmp((char *)&(fs[sortfile[j]].type), (char *)&(fs[p].type)) > 0)
			j--;
//		while(strcmp(fs[sortfile[i]].filename, fs[p].filename) < 0)
//			i++;
//		while(strcmp(fs[sortfile[j]].filename, fs[p].filename) > 0)
//			j--;
		if(i >= j)	break;

		tmp = sortfile[i];
		sortfile[i] = sortfile[j];
		sortfile[j] = tmp;
		i++;
		j--;
	}

	if(left < i - 1)
	        QSort(left, i - 1);
	if(right > j + 1)
		QSort(j + 1, right);
}


static void _sort_file() {
	int	i, j;
	int	no;

//	for(i = 0; i < numFiles; i++)
//		sortfile[i] = i;
	
	for(i = 0; i < numFiles; i++ ) {
		for (j = 1; j < numFiles; j++) {
			if((fs[sortfile[j]].type & S_IFMT) && (fs[sortfile[j]].type & S_IFDIR)) {
				no = sortfile[j - 1];
				sortfile[j - 1] = sortfile[j];
				sortfile[j] = no;
			}
		}
	}

	for (i = 0; i < numFiles; i++) {
		for (j = 0; j < numFiles - 1; j++) {
			if(fs[sortfile[j]].type != fs[sortfile[j + 1]].type)
				continue;
			if(strcmp(fs[sortfile[j]].filename, "..") == 0)
				continue;
			if(strcmp(fs[sortfile[j]].filename, fs[sortfile[j + 1]].filename) > 0 ) {
				no = sortfile[j + 1];
				sortfile[j + 1] = sortfile[j];
				sortfile[j] = no;
			}
		}
	}
}

bool nameEndsWith (const string& name, const string& extension) {
	if (name.size() == 0) return false;
	if (name.front() == '.') return false;

	const string ext = extension;
	if (strcasecmp(name.c_str() + name.size() - ext.size(), ext.c_str()) == 0)return true;
	return false;
}

// TODO: Fix Filelist to use new code based on port of NDS_Backup_Tool
void FileListGBA() {
	DIR	*dir;
	struct stat	st;
	FILE *gbaFile;
	int	i;

	numFiles = 0;
	numGames = 0;

	chdir (curpath);
	dir = opendir(curpath);
	
	if(dir == NULL) {
		strcpy(curpath, "/");
		dir = opendir(curpath);
	}

	const char* GBAEXT = ".GBA";
	const char* BINEXT = ".BIN";

	if (dir != NULL) {
		while(true) {
			dirent* pent = readdir(dir);
			if(pent == NULL)break;
			stat(pent->d_name, &st);
			if (((st.st_mode & S_IFMT) == S_IFDIR) || nameEndsWith(pent->d_name, GBAEXT) || nameEndsWith(pent->d_name, BINEXT)) {
				strcpy(fs[numFiles].filename, pent->d_name);
				// strcpy(fs[numFiles].Alias, pent->d_name);
				fs[numFiles].type = st.st_mode;
				if ((((string)pent->d_name).compare(".") != 0) && ((st.st_mode & S_IFMT) != S_IFDIR)) {
					FILE *file = fopen(pent->d_name, "rb");
					if (file) {
						fseek(file, 0, SEEK_END);
						fs[numFiles].filesize = ftell(file);
						fclose(file);
					}
				}
				numFiles++;
				if (numFiles > 199)break;
			}
		}
		closedir(dir);
	}

	for(i = 0; i < numFiles; i++) {
		sortfile[i] = i;
		if(fs[i].type & S_IFDIR) {
			fs[i].gamecode[0] = 0;
			fs[i].gametitle[0] = 0;
		} else {
			sprintf(tbuf, "%s%s", curpath, fs[i].filename);
			gbaFile = fopen(tbuf, "rb");
			memset(tbuf, 0, 256);
			if(gbaFile != NULL)fread(tbuf, 1, 256, gbaFile);
			tbuf[0xB0] = 0;
			strcpy(fs[i].gamecode, tbuf + 0xAC);
			tbuf[0xAC] = 0;
			strcpy(fs[i].gametitle, tbuf + 0xA0);
			fclose(gbaFile);
			numGames++;
		}
	}

	if(numFiles > 1)_sort_file();
}


#ifdef __cplusplus
}
#endif
