#include <nds.h>

#include <sys/types.h>

#include <sys/stat.h>
#include <fat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/dir.h>
#include <dirent.h>

// #include "fatdir_ex.h"
#include "message.h"
#include "GBA_ini.h"

#include "tarosa/tarosa_Graphic.h"
#include "tarosa/tarosa_Shinofont.h"
extern uint16* MainScreen;
extern uint16* SubScreen;


char	savnam[6][26];
char	savext[6][4];
bool	savexist[6];

void _save_list(char *name) {
	struct tm *ptime;
	struct stat st;
	char	fname[512];
	char	path[512];
	int	i;

	strcpy(fname, name);
	fname[strlen(name) - 3] = 0;

	strcpy(savext[0], "sav");
	sprintf(path, "%s/%s%s", ini.save_dir, fname, savext[0]);
	if(stat(path, &st) == 0) {
		savexist[0] = true;
		ptime = gmtime(&st.st_mtime);
		sprintf(savnam[0], " SAV %04d-%02d-%02d %02d:%02d:%02d ",
				ptime->tm_year + 1900, 
				ptime->tm_mon + 1, 
				ptime->tm_mday, 
				ptime->tm_hour, 
				ptime->tm_min, 
				ptime->tm_sec);
	} else {
		savexist[0] = false;
		strcpy(savnam[0], " SAV  -- No File --      ");
	}

	for(i = 1; i < 6; i++) {
		sprintf(savext[i], "sv%d", i);
		sprintf(path, "%s/%s%s", ini.save_dir, fname, savext[i]);
		if(stat(path, &st) == 0) {
			savexist[i] = true;
			ptime = gmtime(&st.st_mtime);
			sprintf(savnam[i], " <%d> %04d-%02d-%02d %02d:%02d:%02d ",
					i,
					ptime->tm_year + 1900, 
					ptime->tm_mon + 1, 
					ptime->tm_mday, 
					ptime->tm_hour, 
					ptime->tm_min, 
					ptime->tm_sec);
		} else {
			savexist[i] = false;
			sprintf(savnam[i], " <%d>  -- No File --      ", i);
		}
	}

}


int save_sel(int mod, char *name)
{
	int	cmd = 0;
	u32	ky, repky;
	int	i;
	int	len;
	int	x1, x2;
	int	y1, y2;
	int	xi, yi;
	u16	*gback;
	int	gsiz;


	_save_list(name);

	if(ini.multi == 0) {
		name[strlen(name) - 3] = 0;
		strcat(name, savext[0]);
		if(mod == 0 && savexist[cmd] == 0)
			return(-1);
		return(0);
	}

	len = 25;

	x1 = (256 - len * 6) / 2 - 4;
	y1 = 4*12;
	x2 = x1 + len * 6 + 5;
	y2 = y1 + 7*13 + 8;

	DrawBox_SUB(SubScreen, 9, 137, 246, 187, 0, 1);

	gsiz = (x2-x1+1) * (y2-y1+1);
	gback = (u16*)malloc(sizeof(u16*) * gsiz);
	for( yi=y1; yi<y2+1; yi++ ){
		for( xi=x1; xi<x2+1; xi++ ){
			gback[(xi-x1)+(yi-y1)*(x2+1-x1)] = Point_SUB( SubScreen, xi, yi );
		}
	}


	DrawBox_SUB( SubScreen, x1, y1, x2, y2, 5, 0);
	DrawBox_SUB( SubScreen, x1+1, y1+1, x2-1, y2-1, 0, 1);

	DrawBox_SUB( SubScreen, x1+2, y1+2, x2-2, y1+17, 5, 1);
	DrawBox_SUB( SubScreen, x1+2, y1+19, x2-2, y2-2, 5, 0);

	ShinoPrint_SUB( SubScreen, x1 + 3, y1 + 4, (u8 *)savmsg[mod*3 + 0], 0, 0, 0);
	ShinoPrint_SUB( SubScreen, 7*6, 13*12-2, (u8 *)savmsg[mod*3 + 1], 1, 0, 1);
	ShinoPrint_SUB( SubScreen, 7*6, 14*12, (u8 *)savmsg[mod*3 + 2], 1, 0, 1);


	ShinoPrint_SUB(SubScreen, x1 + 3, y1 + 20, (u8 *)savnam[0], 2, 3, 1);
	for(i = 1; i < 6; i++) {
		ShinoPrint_SUB(SubScreen, x1 + 3, y1 + 20 + i*13, (u8 *)savnam[i], 1, 0, 0);
	}


	while(1) {
		swiWaitForVBlank();
		scanKeys();
		repky = keysDownRepeat();
		if((repky & KEY_UP) || (repky & KEY_DOWN)) {
			if(repky & KEY_UP){
				if(cmd > 0) {
					ShinoPrint_SUB(SubScreen, x1 + 3, y1 + 20 + cmd*13, (u8 *)savnam[cmd], 1, 0, 1);
					cmd--;
					ShinoPrint_SUB(SubScreen, x1 + 3, y1 + 20 + cmd*13, (u8 *)savnam[cmd], 2, 3, 1);
				}
			}
			if(repky & KEY_DOWN){
				if(cmd < 5) {
					ShinoPrint_SUB(SubScreen, x1 + 3, y1 + 20 + cmd*13, (u8 *)savnam[cmd], 1, 0, 1);
					cmd++;
					ShinoPrint_SUB(SubScreen, x1 + 3, y1 + 20 + cmd*13, (u8 *)savnam[cmd], 2, 3, 1);
				}
			}
			continue;
		}

		ky = keysDown();

		if(ky & KEY_A) {
			if(mod == 1 || savexist[cmd]) {
				name[strlen(name) - 3] = 0;
				strcat(name, savext[cmd]);
				break;
			}
		}
		if(ky & KEY_B) {
			cmd = -1;
			name[strlen(name) - 3] = 0;
			strcat(name, savext[0]);
			break;
		}
	}

	for( yi=y1; yi<y2+1; yi++ ){
		for( xi=x1; xi<x2+1; xi++ ){
			Pixel_SUB(SubScreen, xi, yi, gback[(xi-x1) + (yi-y1)*(x2+1-x1)] );
		}
	}
	free(gback);

	return(cmd);
}





