#include <nds.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fat.h>
#include <sys/iosupport.h>

#include "dsCard.h"
#include "GBA_ini.h"
#include "ctrl_tbl.h"

struct	ctrl_tbl	ctrl;

#define	SRAM_ADDR	0x0A000000


extern	int	carttype;

#ifdef __cplusplus
extern "C" {
#endif

extern	void	_RamPG(void);
extern	void	_RamSave(int bnk);

#ifdef __cplusplus
}
#endif


void	ctrl_get()
{
	FILE	*exp;
	char	expfile[64];

	memset((u8*)&ctrl, 0, sizeof(struct ctrl_tbl));

	if(carttype != 5) {
		_RamPG();
		ReadSram(SRAM_ADDR, (u8*)&ctrl, sizeof(struct ctrl_tbl));
		_RamSave(0);
		return;
	}

	sprintf(expfile, "%s/EXP128K.dat", ini.sign_dir);
	exp = fopen(expfile, "rb");
	if(exp != NULL) {
		fread(&ctrl, 1, sizeof(struct ctrl_tbl), exp);
		fclose(exp);
	}
}

void	ctrl_set()
{
	FILE	*exp;
	char	expfile[64];


	if(carttype != 5) {
		_RamPG();
		WriteSram(SRAM_ADDR, (u8*)&ctrl, sizeof(struct ctrl_tbl));
		_RamSave(0);
		return;
	}

	sprintf(expfile, "%s/EXP128K.dat", ini.sign_dir);
	exp = fopen(expfile, "wb");
	if(exp != NULL) {
		fwrite(&ctrl, 1, sizeof(struct ctrl_tbl), exp);
		fclose(exp);
	}
}

