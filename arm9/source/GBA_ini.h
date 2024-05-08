
struct	ini_file {
	char	save_dir[64];
	char	sign_dir[64];
	u32	multi;
};

extern	struct	ini_file	ini;


#ifdef __cplusplus
extern "C" {
#endif

extern	void	GBA_ini();

#ifdef __cplusplus
}
#endif

