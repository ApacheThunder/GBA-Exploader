#include <nds.h>

#include <fat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "GBA_ini.h"
#include "header_rep.h"

u8 _comple(u8 *buf, u32 st, u32 en){
	u8	crc;
	u32	i;
	crc = 0;
	for(i = st; i <= en; i++)crc -= buf[i];
	crc = (crc - 0x19) & 0xff;
	return(crc);
}

void header_rep(u8 *rwbuf) {
	char buf[256];
	FILE *hed;
	int	i;

	rwbuf[0xBD] = _comple(rwbuf, 0xA0, 0xBC);
//	rwbuf[0xBE] = _comple(rwbuf, 0x04, 0x9F);

	if(_comple(rwbuf, 0x04, 0x9F) == 0xCC)return;

	sprintf(buf, "%s/header.dat", ini.sign_dir);
	hed = fopen(buf, "rb");
	if(hed == NULL)return;

	fread(buf, 1, 256, hed);
	if(_comple((u8*)buf, 0x04, 0x9F) != 0xCC)return;

	for(i = 0x04; i < 0xA0; i++)rwbuf[i] = buf[i];

	fclose(hed);
}

