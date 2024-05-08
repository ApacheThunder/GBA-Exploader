/***********************************************************
	Arm9 Soft rest for General purpose

		by Rudolph (çcíÈ)
***************************************************************/

#include <nds.h>
//#include <nds/registers_alt.h>	// devkitPror20

#include <fat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


extern u32	_io_dldi;

static	char	*menu_nam;
static	char	name[32];

bool ret_menu_chk()
{
	FILE	*fp;
	char	buf[5];

	buf[0] = _io_dldi & 0xFF;
	buf[1] = (_io_dldi >> 8) & 0xFF;
	buf[2] = (_io_dldi >> 16) & 0xFF;
	buf[3] = (_io_dldi >> 24) & 0xFF;
	buf[4] = 0;
	sprintf(name, "/SoftReset.%s", buf);
	fp = fopen(name, "rb");
	if(fp != NULL) {
		fclose(fp);
		menu_nam = name;
		return true;
	}

	menu_nam = NULL;

	if(_io_dldi == 0x53444353) {		// SCDS
		menu_nam = "/MSFORSC.NDS";
	}

//	if(_io_dldi == 0x58585858) {		// AK+(XXXX)
//		menu_nam = "/system/akmenu2_fat.nds";
//	}

	if(_io_dldi == 0x4F49524E) {		// N-Card
		menu_nam = "/udisk.nds";	// ébíË
	}

	if(_io_dldi == 0x4E475052) {		// AK.R.P.G NAND
		menu_nam = "/akmenu4.nds";
	}

	if(_io_dldi == 0x53475052) {		// AK.R.P.G SD
		menu_nam = "/akmenu4.nds";
	}

	if(_io_dldi == 0x44533958) {		// X9 SD
		menu_nam = "/loader.nds";
	}

	if(_io_dldi == 0x4F495454) {		// DSTT
		menu_nam = "/TTMENU.DAT";
	}

	if(menu_nam != NULL) {
		fp = fopen(menu_nam, "rb");
		if(fp != NULL) {
			fclose(fp);
			return true;
		}
	}

	return false;
}


bool ret_menu9_Gen()
{
	u32	hed[16];
	u8	*ldrBuf;
	FILE	*ldr;
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

