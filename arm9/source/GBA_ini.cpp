#include <nds.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fat.h>
#include <sys/iosupport.h>

#include "GBA_ini.h"

struct	ini_file	ini;

extern	u8	*rwbuf;

void GBA_ini()
{
	FILE	*fini;
	int	len, p, s;
	char	key[20];

	ini.multi = 1;
	strcpy(ini.save_dir, "/GBA_SAVE");
	strcpy(ini.sign_dir, "/GBA_SIGN");


	fini = fopen("/GBA_ExpLoader.ini", "rb");
	if(fini == NULL) {
		mkdir(ini.save_dir, 0777);
		mkdir(ini.sign_dir, 0777);
		return;
	}

	len = fread(rwbuf, 1, 0x1000, fini);

	p = 0;
	while(p < len) {
		if(rwbuf[p] == '#' || rwbuf[p] == '!') {
			while(p < len) {
				if(rwbuf[p] == 0x0A)
					break;
				p++;
			}
			p++;
		}

		s = 0;
		while(rwbuf[p] >= 0x20 && rwbuf[p] < 0x7F) {
			key[s] = rwbuf[p];
			s++;
			p++;
		}
		key[s] = 0;

		if(strcmp(key, "SaveDir") == 0) {
			s = 0;
			while(p < len) {
				if(rwbuf[p] == 0x20 || rwbuf[p] == '\t')
					p++;
				else	break;
			}
			if(rwbuf[p] == 0x22) {
				p++;
				while((p < len) && (rwbuf[p] != 0x22)) {
					if(s < 63)
						ini.save_dir[s] = rwbuf[p];
					s++;
					p++;
				}
			} else {
				while(p < len) {
					if(rwbuf[p] <= 0x20 || rwbuf[p] >= 0x7F)
						break;
					if(s < 63)
						ini.save_dir[s] = rwbuf[p];
					s++;
					p++;
				}
			}
			ini.save_dir[s] = 0;
		}

		if(strcmp(key, "SignDir") == 0) {
			s = 0;
			while(p < len) {
				if(rwbuf[p] == 0x20 || rwbuf[p] == '\t')
					p++;
				else	break;
			}
			if(rwbuf[p] == 0x22) {
				p++;
				while((p < len) && (rwbuf[p] != 0x22)) {
					if(s < 63)
						ini.sign_dir[s] = rwbuf[p];
					s++;
					p++;
				}
			} else {
				while(p < len) {
					if(rwbuf[p] <= 0x20 || rwbuf[p] >= 0x7F)
						break;
					if(s < 63)
						ini.sign_dir[s] = rwbuf[p];
					s++;
					p++;
				}
			}
			ini.sign_dir[s] = 0;
		}
		if(strcmp(key, "No_MultiSave") == 0) {
			ini.multi = 0;
		}

		while(p < len) {
			p++;
			if(rwbuf[p - 1] == 0x0A)
				break;
		}
	}

	fclose(fini);

	if(ini.save_dir[0] != '/')
		strcpy(ini.save_dir, "/GBA_SAVE");
	if(ini.sign_dir[0] != '/')
		strcpy(ini.sign_dir, "/GBA_SIGN");

	mkdir(ini.save_dir, 0777);
	mkdir(ini.sign_dir, 0777);
}

