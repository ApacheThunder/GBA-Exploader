/***********************************************************
	Arm9 Soft rest for General purpose

		by Rudolph (�c��)
***************************************************************/

#include <nds.h>
#include <nds/arm9/dldi.h>

#include <fat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *menu_nam;
static char name[32];

static ALIGN(4) u32 hed[16];
static ALIGN(4) u8	*ldrBuf;

bool ret_menu_chk() {
	FILE	*fp;
	char	buf[5];

	buf[0] = io_dldi_data->ioInterface.ioType & 0xFF;
	buf[1] = (io_dldi_data->ioInterface.ioType >> 8) & 0xFF;
	buf[2] = (io_dldi_data->ioInterface.ioType >> 16) & 0xFF;
	buf[3] = (io_dldi_data->ioInterface.ioType >> 24) & 0xFF;
	buf[4] = 0;
	sprintf(name, "/SoftReset.%s", buf);
	fp = fopen(name, "rb");
	if(fp != NULL) {
		fclose(fp);
		menu_nam = name;
		return true;
	}

	menu_nam = NULL;

	switch (io_dldi_data->ioInterface.ioType) {
		case 0x53444353: menu_nam = "/MSFORSC.NDS"; break; // SCDS		
		case 0x4F49524E: menu_nam = "/udisk.dat"; break; // N-Card and Clones		
		case 0x4E475052: menu_nam = "/akmenu4.nds"; break; // AK.R.P.G NAND		
		case 0x53475052: menu_nam = "/akmenu4.nds"; break; // AK.R.P.G SD		
		case 0x44533958: menu_nam = "/loader.nds"; break; // X9 SD
		case 0x4F495454: menu_nam = "/TTMENU.DAT"; break; // DSTT
	}

//	if(_io_dldi == 0x58585858) {		// AK+(XXXX)
//		menu_nam = "/system/akmenu2_fat.nds";
//	}
	
	if(menu_nam != NULL) {
		fp = fopen(menu_nam, "rb");
		if(fp != NULL) {
			fclose(fp);
			return true;
		}
	}

	return false;
}

ITCM_CODE bool ret_menu9_Gen() {
	FILE *ldr;
	u32	siz;

	ldr = fopen(menu_nam, "rb");
	if(ldr == NULL)	return false;

	fread((u8*)hed, 1, 16*4, ldr);
	siz = 512 + hed[11] + hed[15];
	ldrBuf = (u8*)malloc(siz);
	if(ldrBuf == NULL) {
		fclose(ldr);
		return false;
	}

	fseek(ldr, 0, SEEK_SET);
	fread(ldrBuf, 1, 512, ldr);

	fseek(ldr, hed[8], SEEK_SET);
	fread(ldrBuf + 512, 1, hed[11], ldr);

	fseek(ldr, hed[12], SEEK_SET);
	fread(ldrBuf + 512 + hed[11], 1, hed[15], ldr);

	fclose(ldr);

	(*(vu32*)0x027FFDF4) = (u32)ldrBuf;

	return true;
}

