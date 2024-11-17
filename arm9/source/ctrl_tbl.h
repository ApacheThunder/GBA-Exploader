struct GBA_File {
	char filename[512];
	char gametitle[13];
	char gamecode[5];
	u32	type;
	u32	filesize;
	int isNDSFile;
	int isHomebrewNDS;
	char ndssavfilename[512];
};


struct ctrl_tbl {
	u8 sign[128];
	u16 mode;
	u8 save_flg[2];
	u32 save_siz[2];
	char filer[116];
	u8 sav_nam[2][512];
	u8 path[256];
};

extern struct ctrl_tbl ctrl;

#ifdef __cplusplus
extern "C" {
#endif

extern void ctrl_get();
extern void ctrl_set();

#ifdef __cplusplus
}
#endif

