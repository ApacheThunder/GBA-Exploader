#ifdef __cplusplus
extern "C" {
#endif

extern	void	gba_check_int(char *name);
extern	u32	gba_check_Ram1(u8 *buf, u32 bufsize, u32 size, u32 ofs);
extern	void	gba_check_Ram2(u32 exp, u8 *buf, u32 bufsize, u32 size);
extern	u32	gba_check(FILE *gbaFile, u32 size, u8 *buf, u32 bufsize);
extern	void	gba_patch_Ram(u32 exp, char *name, int cart);
extern	void	gba_patch(u8 *buf, u32 add, u32 bufsize, int GBAmode, char *name);

#ifdef __cplusplus
}
#endif
