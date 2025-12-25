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

#include "nds_loader_arm9.h"

static char *menu_nam;
static char name[32];

bool ret_menu_chk() {
	char	buf[5];
	menu_nam = NULL;

	buf[0] = io_dldi_data->ioInterface.ioType & 0xFF;
	buf[1] = (io_dldi_data->ioInterface.ioType >> 8) & 0xFF;
	buf[2] = (io_dldi_data->ioInterface.ioType >> 16) & 0xFF;
	buf[3] = (io_dldi_data->ioInterface.ioType >> 24) & 0xFF;
	buf[4] = 0;
	sprintf(name, "/%s.nds", buf);
	
	if(access(name, F_OK) == 0) {
		menu_nam = name;
		return true;
	}

	switch (io_dldi_data->ioInterface.ioType) {
		case 0x53444353: menu_nam = "/MSFORSC.NDS"; break; // Moonshel for SCDS		
		case 0x4F49524E: menu_nam = "/udisk.nds"; break; // N-Card and Clones		
		case 0x4E475052: menu_nam = "/akmenu4.nds"; break; // AK.R.P.G NAND		
		case 0x53475052: menu_nam = "/akmenu4.nds"; break; // AK.R.P.G SD		
		case 0x44533958: menu_nam = "/loader.nds"; break; // X9 SD
		case 0x4F495454: menu_nam = "/TTMENU.DAT"; break; // DSTT
		case 0x4F4D4544: menu_nam = "/R4.DAT"; break; // Timebombed DSTTi clones like R4 SDHC Gold Pro
		case 0x4F434950: menu_nam = "/_picoboot.nds"; break; // DSPico
	}
	
	if((menu_nam != NULL) && (access(menu_nam, F_OK) == 0)) {
		return true;
	} else if (access("/boot.nds", F_OK) == 0) {
		menu_nam = "/boot.nds";
		return true;
	}
	return false;
}

bool ret_menu9_Gen() {
	runNdsFile(menu_nam, 0, NULL);
	return false;
}

