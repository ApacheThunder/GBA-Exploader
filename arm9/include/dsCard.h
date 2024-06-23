/**************************************************************************************************************
 * ���ļ�Ϊ dsCard.h �ļ��ĵڶ��� 
 * ���ڣ�2006��11��27��11��33��  ��һ�� version 1.0
 * ���ߣ�aladdin
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
typedef bool BOOL ;
#endif
// export interface




//---------------------------------------------------
//DS �� ��������
		//Arm9 ���������������ARM7����slot1
		void		Enable_Arm7DS(void);

		//Arm9 ���������������ARM9����slot1
		void		Enable_Arm9DS(void);

		
		//�����Ƿ����𶯿��ĺ���
#define FlashBase		0x08000000
#define PSRAMBase_S98	0x08800000
#define FlashBase_S98	0x09000000
#define	_Ez5PsRAM 		0x08000000
#define SAVE_sram_base	0x0E000000
		void		OpenNorWrite();
		void		CloseNorWrite();
		void      	SetRompage(u16 page);
		void 		SetRampage(u16 page);
		void		SetSDControl(u16 control);
		void		Set_AUTO_save(u16 mode);
		u16			Read_S71NOR_ID(); // For Reading EZFlash Omega ID
		u16			Read_S98NOR_ID(); // For Reading EZFlash Omega ID
		void		SetPSRampage(u16 page); // EZFlash Omega uses this instead of regular SetRamPage
		void 		Set_RTC_status(u16 status); // EZFlash Omega RTC thingy
		u16 		Read_SET_info(u32 offset);
		void  		OpenRamWrite();
		void 		CloseRamWrite();
		void      	SetSerialMode();
		uint32   	ReadNorFlashID();
		void 		chip_reset();
		void 		Block_EraseIntel(u32 blockAdd);
		void 		Block_Erase(u32 blockAdd);
		// void 		ReadNorFlash(u8* pBuf,u32 address,u16 len);
		void 		WriteNorFlashINTEL(u32 address,u8 *buffer,u32 size);
		void 		WriteNorFlash(u32 address,u8 *buffer,u32 size);
		void 		WriteSram(uint32 address, u8* data , uint32 size );
		void 		ReadSram(uint32 address, u8* data , uint32 size );
		void 		SetShake(u16 data);
		void		Omega_Bank_Switching(u8 bank);
	#ifdef __cplusplus
	}
	#endif
#endif

