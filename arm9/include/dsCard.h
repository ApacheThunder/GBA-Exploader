/**************************************************************************************************************
 * 此文件为 dsCard.h 文件的第二版 
 * 日期：2006年11月27日11点33分  第一版 version 1.0
 * 作者：aladdin
 * CopyRight : EZFlash Group
 * 
 **************************************************************************************************************/

#ifndef NDS_DSCARD_V2_INCLUDE
#define NDS_DSCARD_V2_INCLUDE

#include "nds.h"

	#ifdef __cplusplus
	extern "C" {
	#endif

#ifndef BYTE
typedef unsigned char BYTE;
#endif

#ifndef WORD
typedef unsigned short WORD;
#endif

#ifndef DWORD
typedef unsigned long DWORD;
#endif

#ifndef BOOL
typedef bool BOOL;
#endif
// export interface


//---------------------------------------------------
//DS 卡 基本操作
	//Arm9 方面基本操作容许ARM7访问slot1
	// void		Enable_Arm7DS(void);

	//Arm9 方面基本操作容许ARM9访问slot1
	// void		Enable_Arm9DS(void);

	
	//下面是访问震动卡的函数
#define FlashBase		0x08000000
#define PSRAMBase_S98	0x08800000 // EZ Flash Omega PSRAM location while in Kernal mode. (only writable in Kernal mode)
#define FlashBase_S98	0x09000000
#define	SRAM_ADDR		0x0A000000
// #define	SRAM_ADDR_OMEGA	0x0E000000 // SRAM address changes in DS mode. Use 0x0A000000 instead.
#define	SRAM_ADDR_OMEGA	0x0A000000
#define RTC_ENABLE		((vu16*)0x080000C8)

#define OMEGA_NOSAVE		0
#define OMEGA_UNKNOWN		0xFF
#define OMEGA_EEPROM_512	0x21
#define OMEGA_EEPROM_8K		0x22
#define OMEGA_EEPROM_V125	0x23
#define OMEGA_FLASH_64K		0x32
#define OMEGA_FLASH_512		0x33
#define OMEGA_FLASH_1M		0x31
#define OMEGA_SRAM_32K		0x11
#define OMEGA_SRAM_64K		0x10
// #define OMEGA_SRAM_64K		0xEE

	void		OpenNorWrite();
	void		CloseNorWrite();
	void      	SetRompage(u16 page);
	void 		SetRampage(u16 page);
	void		SetSDControl(u16 control);
	void		Set_AUTO_save(u16 mode);
	u16			Read_S71NOR_ID(); // For Reading EZFlash Omega ID
	u16			Read_S98NOR_ID(); // For Reading EZFlash Omega ID
	void		SetPSRampage(u16 page); // EZFlash Omega uses this to change what 8MB of 32MB PSRAM is accessible at 0x08800000
	void 		Set_RTC_status(u16 status); // EZFlash Omega RTC thingy
	u16 		Read_SET_info(u32 offset);
	void		rtc_toggle(bool enable);
	void		SetbufferControl(u16 control);
	void		Omega_Bank_Switching(u8 bank);
	void		Omega_InitFatBuffer(BYTE saveMODE, u32 saveSize, u32 gameSize);
	u32			Omega_SetSaveSize(u8 SaveMode);
	void		SetSPIWrite(u16 control);
	void		SetSPIControl(u16 control);
	void		SPI_Enable();
	void		SPI_Disable();
	void		SPI_Write_Enable();
	void		SPI_Write_Disable();
	u16			Read_FPGA_ver();
	void  		OpenRamWrite();
	void 		CloseRamWrite();
	void      	SetSerialMode();
	u32			CheckSuperCardID();
	u32			CheckOmegaID();
	uint32   	ReadNorFlashID();
	void 		chip_reset();
	void 		Block_EraseIntel(u32 blockAdd);
	void 		Block_Erase(u32 blockAdd);
	// void 	ReadNorFlash(u8* pBuf,u32 address,u16 len); // Not used anywhere right now. Disabled out for now.
	void 		WriteNorFlashINTEL(u32 address,u8 *buffer,u32 size);
	void 		WriteNorFlash(u32 address,u8 *buffer,u32 size);
	void 		WriteSram(uint32 address, u8* data , uint32 size );
	void 		ReadSram(uint32 address, u8* data , uint32 size );
	void 		SetShake(u16 data);
	
	#ifdef __cplusplus
	}
	#endif
#endif

