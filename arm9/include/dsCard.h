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
#define	_Ez5PsRAM 	0x08000000
		void		OpenNorWrite();
		void		CloseNorWrite();
		void      SetRompage(u16 page);
		void 		SetRampage(u16 page);
		void  	OpenRamWrite();
		void 		CloseRamWrite();
		void      SetSerialMode();
		uint32   ReadNorFlashID();
		void 		chip_reset();
		void 		Block_Erase(u32 blockAdd);
		void 		ReadNorFlash(u8* pBuf,u32 address,u16 len);
		void 		WriteNorFlash(u32 address,u8 *buffer,u32 size);
		void 		WriteSram(uint32 address, u8* data , uint32 size );
		void 		ReadSram(uint32 address, u8* data , uint32 size );
		void 		SetShake(u16 data);
	#ifdef __cplusplus
	}
	#endif
#endif
