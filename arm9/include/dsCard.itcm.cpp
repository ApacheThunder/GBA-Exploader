/**************************************************************************************************************
 * 此文件为 dsCard.cpp 文件的第二版 
 * 日期：2006年11月27日11点33分  第一版 version 1.0
 * 作者：aladdin
 * CopyRight : EZFlash Group
 * 
 **************************************************************************************************************/
#include <stdio.h>

#include "dscard.h"
#include "string.h"
#include "io_sc_common.h"
#include "tonccpy.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NOR_info_offset 0x7A0000
#define SET_info_offset 0x7B0000

static u32 ID = 0x227E2218;

static u32 FAT_table_buffer[0x400/4];

u16 gl_ingame_RTC_open_status = 0;

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//---------------------------------------------------
//DS 卡 基本操作
/************
void Enable_Arm9DS()
{
	WAIT_CR &= ~0x0800;
}

void Enable_Arm7DS()
{
	WAIT_CR |= 0x0800;
}
*************/
void OpenNorWrite() {
	*(vu16*)0x9fe0000 = 0xd200;
	*(vu16*)0x8000000 = 0x1500;
	*(vu16*)0x8020000 = 0xd200;
	*(vu16*)0x8040000 = 0x1500;
	*(vu16*)0x9C40000 = 0x1500;
	*(vu16*)0x9fc0000 = 0x1500;
}

void CloseNorWrite() {
	*(vu16*)0x9fe0000 = 0xd200;
	*(vu16*)0x8000000 = 0x1500;
	*(vu16*)0x8020000 = 0xd200;
	*(vu16*)0x8040000 = 0x1500;
	*(vu16*)0x9C40000 = 0xd200;
	*(vu16*)0x9fc0000 = 0x1500;
}


void SetRompage(u16 page) {
	*(vu16*)0x9fe0000 = 0xd200;
	*(vu16*)0x8000000 = 0x1500;
	*(vu16*)0x8020000 = 0xd200;
	*(vu16*)0x8040000 = 0x1500;
	*(vu16*)0x9880000 = page;
	*(vu16*)0x9fc0000 = 0x1500;
}


// EZ Flash Omega only
void SetSDControl(u16 control) {
	*(u16 *)0x9fe0000 = 0xd200;
	*(u16 *)0x8000000 = 0x1500;
	*(u16 *)0x8020000 = 0xd200;
	*(u16 *)0x8040000 = 0x1500;
	*(u16 *)0x9400000 = control;
	*(u16 *)0x9fc0000 = 0x1500;
}

void Set_AUTO_save(u16 mode) {
	*(u16*)0x9fe0000 = 0xd200;
	*(u16*)0x8000000 = 0x1500;
	*(u16*)0x8020000 = 0xd200;
	*(u16*)0x8040000 = 0x1500;
	*(u16*)0x96C0000 = mode;
	*(u16*)0x9fc0000 = 0x1500;
}

u16 Read_S71NOR_ID() {
	*((vu16*)(FlashBase)) = 0xF0;	
	*((vu16*)(FlashBase+0x555*2)) = 0xAA;
	*((vu16*)(FlashBase+0x2AA*2)) = 0x55;
	*((vu16*)(FlashBase+0x555*2)) = 0x90;
	u16 ID1 = *((vu16*)(FlashBase+0xE*2));
	*((vu16*)(FlashBase)) = 0xF0;
	return ID1;
}	

u16 Read_S98NOR_ID() {
	*((vu16*)(FlashBase_S98)) = 0xF0;	
	*((vu16*)(FlashBase_S98+0x555*2)) = 0xAA;
	*((vu16*)(FlashBase_S98+0x2AA*2)) = 0x55;
	*((vu16*)(FlashBase_S98+0x555*2)) = 0x90;
	return *((vu16*)(FlashBase_S98+0xE*2));
}

// EZFlash Omega uses this
void SetPSRampage(u16 page) {
	*(vu16*)0x9fe0000 = 0xd200;
	*(vu16*)0x8000000 = 0x1500;
	*(vu16*)0x8020000 = 0xd200;
	*(vu16*)0x8040000 = 0x1500;
	*(vu16*)0x9860000 = page; //C3
	*(vu16*)0x9fc0000 = 0x1500;
}

u16 Read_SET_info(u32 offset) { return *((vu16 *)(FlashBase+SET_info_offset+offset*2)); }

// EZFlash Omega RTC thingy
void Set_RTC_status(u16 status) {
	*(u16*)0x9fe0000 = 0xd200;
	*(u16*)0x8000000 = 0x1500;
	*(u16*)0x8020000 = 0xd200;
	*(u16*)0x8040000 = 0x1500;
	*(u16*)0x96A0000 = status;
	*(u16*)0x9fc0000 = 0x1500;
}

void rtc_toggle(bool enable) {
	if (enable) { *RTC_ENABLE = 1; } else { *RTC_ENABLE = 0; }
}

void Omega_Bank_Switching(u8 bank) {
	*((vu8*)(SRAM_ADDR_OMEGA+0x5555)) = 0xAA; // SRAM_ADDR_OMEGA
	*((vu8*)(SRAM_ADDR_OMEGA+0x2AAA)) = 0x55;
	*((vu8*)(SRAM_ADDR_OMEGA+0x5555)) = 0xB0;
	*((vu8*)SRAM_ADDR_OMEGA)		  = bank;
}

void SetbufferControl(u16 control) {
	*(u16*)0x9fe0000 = 0xd200;
	*(u16*)0x8000000 = 0x1500;
	*(u16*)0x8020000 = 0xd200;
	*(u16*)0x8040000 = 0x1500;
	*(u16*)0x9420000 = control; //A1
	*(u16*)0x9fc0000 = 0x1500;
}

u16 SD_Response(void) { return *(vu16*)0x9E00000; }

void Omega_InitFatBuffer(BYTE saveMODE, u32 saveSize, u32 gameSize) {
	toncset((u32*)FAT_table_buffer, 0, 0x400);
	// FAT_table_buffer[1] = 0; // sector location of game rom to be copies
	FAT_table_buffer[2] = 0xFFFFFFFF;
	// FAT_table_buffer[0x1F0/4] = gameSize; // rom copy size
	FAT_table_buffer[0x1F4/4] = 0x2;  // 0x1 == rom copy to psram // 0x2 == copy mode
	FAT_table_buffer[0x1F8/4] = 0x40; // secort of cluster
	FAT_table_buffer[0x1FC/4] = ((saveMODE << 24) | saveSize);  // save mode and save file size
	// FAT_table_buffer[0x204/4] = 0x363100; // Save file sector location
	// FAT_table_buffer[0x204/4] = 0x00C07644;
	FAT_table_buffer[0x204/4] = 0xFFFFFFFF;
	FAT_table_buffer[0x208/4] = 0xFFFFFFFF;
	// FAT_table_buffer[0x308/4] = 0x100;	// RTS?
	// FAT_table_buffer[0x320/4] = 0xFFFFFFFF; // RTS?
	// FAT_table_buffer[0x210/4] = 0xFFFFFFFF;
	// FAT_table_buffer[0x210/4] = 0xFFFFFFFF;
	/*FILE *testFile = fopen("/fatTableTest.bin", "wb");
	if (testFile) {
		fwrite((void*)FAT_table_buffer, 0x400, 1, testFile);
		fclose(testFile);
	}*/
	SetbufferControl(1);
	// dmaCopy(FAT_table_buffer, (void*)0x9E00000, 0x400);
	tonccpy((void*)0x9E00000, &FAT_table_buffer, 0x400);
	SetbufferControl(3);
	SetbufferControl(0);	
	/*SetbufferControl(0);
	u16 res;
	while(1) {
		res = SD_Response();
		if(res != 0)break;
	}
	
	while(1) {
		res = SD_Response();
		if(res != 0x0001)break;
	}
	SetbufferControl(0);*/
}

u32 Omega_SetSaveSize(u8 SaveMode) {
	switch (SaveMode) {
		case OMEGA_NOSAVE: return 0;
		case OMEGA_UNKNOWN: return 0x10000;
		case OMEGA_EEPROM_512: return 0x200;
		case OMEGA_EEPROM_8K: return 0x2000;
		case OMEGA_EEPROM_V125: return 0x2000;
		case OMEGA_FLASH_64K: return 0x10000;
		case OMEGA_FLASH_512: return 0x10000;
		case OMEGA_FLASH_1M: return 0x20000;
		case OMEGA_SRAM_32K: return 0x8000;
		case OMEGA_SRAM_64K: return 0x10000;
		default: return 0;
	}
}

void SetSPIWrite(u16 control) {
	*(u16*)0x9fe0000 = 0xd200;
	*(u16*)0x8000000 = 0x1500;
	*(u16*)0x8020000 = 0xd200;
	*(u16*)0x8040000 = 0x1500;
	*(u16*)0x9680000 = control;
	*(u16*)0x9fc0000 = 0x1500;
}

void SetSPIControl(u16 control) {
	*(u16*)0x9fe0000 = 0xd200;
	*(u16*)0x8000000 = 0x1500;
	*(u16*)0x8020000 = 0xd200;
	*(u16*)0x8040000 = 0x1500;
	*(u16*)0x9660000 = control;
	*(u16*)0x9fc0000 = 0x1500;
}

void SPI_Enable() { SetSPIControl(1); }

void SPI_Disable() { SetSPIControl(0); }

void SPI_Write_Enable() { SetSPIWrite(1); }

void SPI_Write_Disable() { SetSPIWrite(0); }

u16 Read_FPGA_ver() {
	u16 Read_SPI;
	SPI_Enable();	
	Read_SPI = *(vu16*)0x9E00000;
	SPI_Disable();
	return Read_SPI;
}



void SetRampage(u16 page) {
	*(vu16*)0x9fe0000 = 0xd200;
	*(vu16*)0x8000000 = 0x1500;
	*(vu16*)0x8020000 = 0xd200;
	*(vu16*)0x8040000 = 0x1500;
	*(vu16*)0x9c00000 = page;
	*(vu16*)0x9fc0000 = 0x1500;
}

void SetSerialMode() {
	*(vu16*)0x9fe0000 = 0xd200;
	*(vu16*)0x8000000 = 0x1500;
	*(vu16*)0x8020000 = 0xd200;
	*(vu16*)0x8040000 = 0x1500;
	*(vu16*)0x9A40000 = 0xe200;
	*(vu16*)0x9fc0000 = 0x1500;
}

u32 CheckSuperCardID() {
	SetRompage(0); // Prevent possible chain boot issue where previously booted app put card into similar mode to SuperCard if it's a card that responds to SetRompage.
	_SC_changeMode(SC_MODE_RAM);	// Try again with SuperCard
	// _SC_changeMode16(0x1510);
	*(vu16*)(FlashBase) = 0x4D54;
	if (*(vu16*)(FlashBase) == 0x4D54) {
		ID = 0x227E2202;
		return 0x227E0000;
	}
	return 0;
}


u32 CheckOmegaID() {
	// Check For EZ Flash Omega
	if (Read_S98NOR_ID() == 0x223D) {
		ID = 0x227EEA00;
		return 0x227EEA00;
	}
	return 0;
}


u32 ReadNorFlashID() {
	vu16 id1, id2, id3, id4;
	ID = 0;
	//check intel 512M 3in1 card
	*((vu16*)(FlashBase+0)) = 0xFF ;
	*((vu16*)(FlashBase+0x1000*2)) = 0xFF ;
	*((vu16*)(FlashBase+0)) = 0x90 ;
	*((vu16*)(FlashBase+0x1000*2)) = 0x90 ;
	id1 = *((vu16*)(FlashBase+0)) ;
	id2 = *((vu16*)(FlashBase+0x1000*2)) ;
	id3 = *((vu16*)(FlashBase+1*2)) ;
	id4 = *((vu16*)(FlashBase+0x1001*2)) ;
	if((id1==0x89) && (id2==0x89)) {
		if(((id3==0x8816)||(id3 ==0x8810))&&((id4==0x8816)||(id4==0x8810))) {
			ID = 0x89168916;
			return 0x89168916;
		}
	}
	// check original version chips	
	*((vu16*)(FlashBase+0x555*2)) = 0xAA;
	*((vu16*)(FlashBase+0x2AA*2)) = 0x55;
	*((vu16*)(FlashBase+0x555*2)) = 0x90;
	
	*((vu16*)(FlashBase+0x1555*2)) = 0xAA;
	*((vu16*)(FlashBase+0x12AA*2)) = 0x55;
	*((vu16*)(FlashBase+0x1555*2)) = 0x90;
	
	id1 = *((vu16*)(FlashBase+0x2));
	id2 = *((vu16*)(FlashBase+0x2002));
	
	/*FILE *testFile = fopen("/gbacardID.bin", "wb");
	if (testFile) {
		fwrite((void*)(FlashBase+0x2), 2, 1, testFile);
		fwrite((void*)(FlashBase+0x2002), 2, 1, testFile);
		fclose(testFile);
	}*/
		
	if ((id1 != 0x227E) || (id2 != 0x227E))return 0;
	
	id1 = *((vu16*)(FlashBase+0xE*2));
	id2 = *((vu16*)(FlashBase+0x100e*2));
	
	/*FILE *testFile = fopen("/gbacardID.bin", "wb");
	if (testFile) {
		fwrite((void*)FlashBase+0xE*2, 2, 1, testFile);
		fwrite((void*)FlashBase+0x100e*2, 2, 1, testFile);
		fclose(testFile);
	}*/
	
	if(id1 == 0x2202) {
		if ((id2 == 0x2202) || (id2 == 0x2220) || (id2 == 0x2215)) { // VZ064
			ID = 0x227E2202;
			return 0x227E2202;
		}
	} else if(id1 == 0x2218) {
		if (id2 == 0x2218) { // H6H6
			ID = 0x227E2218;
			return 0x227E2218;
		}
	}
	return 0x227E2220; // EZFlash IV or EWIN
}

void chip_reset() {
	if (ID == 0x227EEA00) {
		*((vu16*)0x08600000) = 0xF0;
		return;
	} else if(ID == 0x89168916) {
		// *((vu16*)(FlashBase+0)) = 0x50;
		*((vu16*)(FlashBase+0x1000*2)) = 0x50;
		*((vu16*)(FlashBase+0)) = 0xFF;
		*((vu16*)(FlashBase+0x1000*2)) = 0xFF;
	} else {
		*((vu16*)(FlashBase)) = 0xF0;
		*((vu16*)(FlashBase+0x1000*2)) = 0xF0;
		if(ID==0x227E2202) {
			*((vu16*)(FlashBase+0x1000000)) = 0xF0;
			*((vu16*)(FlashBase+0x1000000+0x1000*2)) = 0xF0;
		}
	}
}

void Block_EraseIntel(u32 blockAdd) {
	u32 loop;
	vu16 v1,v2;
	bool b512 = false;
	vu32 blockAddress = blockAdd;
	if(blockAddress >= 0x2000000) {
		// blockAdd-=0x2000000;
		blockAddress -= 0x2000000;
		CloseNorWrite();
		SetRompage(768);
		OpenNorWrite();	
		b512=true;
	}
	if(blockAddress == 0) {
		for(loop=0;loop<0x40000;loop+=0x10000) {
			*((vu16 *)(FlashBase+loop)) = 0x50;
			*((vu16 *)(FlashBase+loop+0x2000)) = 0x50;
			*((vu16 *)(FlashBase+loop)) = 0xFF;
			*((vu16 *)(FlashBase+loop+0x2000)) = 0xFF;
			*((vu16 *)(FlashBase+loop)) = 0x60;
			*((vu16 *)(FlashBase+loop+0x2000)) = 0x60;
			*((vu16 *)(FlashBase+loop)) = 0xD0;
			*((vu16 *)(FlashBase+loop+0x2000)) = 0xD0;
			*((vu16 *)(FlashBase+loop)) = 0x20;
			*((vu16 *)(FlashBase+loop+0x2000)) = 0x20;
			*((vu16 *)(FlashBase+loop)) = 0xD0;
			*((vu16 *)(FlashBase+loop+0x2000)) = 0xD0;
			do {
				v1 = *((vu16 *)(FlashBase+loop));
				v2 = *((vu16 *)(FlashBase+loop+0x2000));
			}
			while((v1!=0x80)||(v2!=0x80));
		}
	} else {
		*((vu16 *)(FlashBase+blockAddress)) = 0xFF;
		*((vu16 *)(FlashBase+blockAddress+0x2000)) = 0xFF;
		*((vu16 *)(FlashBase+blockAddress)) = 0x60;
		*((vu16 *)(FlashBase+blockAddress+0x2000)) = 0x60;
		*((vu16 *)(FlashBase+blockAddress)) = 0xD0;
		*((vu16 *)(FlashBase+blockAddress+0x2000)) = 0xD0;
		*((vu16 *)(FlashBase+blockAddress)) = 0x20;
		*((vu16 *)(FlashBase+blockAddress+0x2000)) = 0x20;
		*((vu16 *)(FlashBase+blockAddress)) = 0xD0 ;
		*((vu16 *)(FlashBase+blockAddress+0x2000)) = 0xD0;
		do {
			v1 = *((vu16 *)(FlashBase+blockAddress));
			v2 = *((vu16 *)(FlashBase+blockAddress+0x2000));
		}
		while((v1!=0x80)||(v2!=0x80));
	}
	if(b512) {
		CloseNorWrite();
		SetRompage(0);
		OpenNorWrite();	
		b512= true;
	}
}

void Block_Erase(u32 blockAdd) {
	vu16 v1,v2;  
	u32 Address;
	u32 loop;
	u32 off=0;
	if((blockAdd>=0x1000000) && (ID==0x227E2202)) {
		off=0x1000000;
		*((vu16 *)(FlashBase+off+0x555*2)) = 0xF0 ;
		*((vu16 *)(FlashBase+off+0x1555*2)) = 0xF0 ;
	} else {
		off=0;
	}
	Address=blockAdd;
	*((vu16 *)(FlashBase+0x555*2)) = 0xF0 ;
	*((vu16 *)(FlashBase+0x1555*2)) = 0xF0 ;
	
	
	if( (blockAdd==0) || (blockAdd==0x1FC0000) || (blockAdd==0xFC0000) || (blockAdd==0x1000000)) {
		for(loop=0;loop<0x40000;loop+=0x8000) {
			*((vu16 *)(FlashBase+off+0x555*2)) = 0xAA ;
			*((vu16 *)(FlashBase+off+0x2AA*2)) = 0x55 ;
			*((vu16 *)(FlashBase+off+0x555*2)) = 0x80 ;
			*((vu16 *)(FlashBase+off+0x555*2)) = 0xAA ;
			*((vu16 *)(FlashBase+off+0x2AA*2)) = 0x55 ;
			*((vu16 *)(FlashBase+Address+loop)) = 0x30 ;
			
			*((vu16 *)(FlashBase+off+0x1555*2)) = 0xAA ;
			*((vu16 *)(FlashBase+off+0x12AA*2)) = 0x55 ;
			*((vu16 *)(FlashBase+off+0x1555*2)) = 0x80 ;
			*((vu16 *)(FlashBase+off+0x1555*2)) = 0xAA ;
			*((vu16 *)(FlashBase+off+0x12AA*2)) = 0x55 ;
			*((vu16 *)(FlashBase+Address+loop+0x2000)) = 0x30 ;
			
			*((vu16 *)(FlashBase+off+0x2555*2)) = 0xAA ;
			*((vu16 *)(FlashBase+off+0x22AA*2)) = 0x55 ;
			*((vu16 *)(FlashBase+off+0x2555*2)) = 0x80 ;
			*((vu16 *)(FlashBase+off+0x2555*2)) = 0xAA ;
			*((vu16 *)(FlashBase+off+0x22AA*2)) = 0x55 ;
			*((vu16 *)(FlashBase+Address+loop+0x4000)) = 0x30 ;
			
			*((vu16 *)(FlashBase+off+0x3555*2)) = 0xAA ;
			*((vu16 *)(FlashBase+off+0x32AA*2)) = 0x55 ; 
			*((vu16 *)(FlashBase+off+0x3555*2)) = 0x80 ;
			*((vu16 *)(FlashBase+off+0x3555*2)) = 0xAA ;
			*((vu16 *)(FlashBase+off+0x32AA*2)) = 0x55 ;
			*((vu16 *)(FlashBase+Address+loop+0x6000)) = 0x30 ;
			do {
				v1 = *((vu16 *)(FlashBase+Address+loop)) ;
				v2 = *((vu16 *)(FlashBase+Address+loop)) ;
			} while(v1!=v2);
			do {
				v1 = *((vu16 *)(FlashBase+Address+loop+0x2000)) ;
				v2 = *((vu16 *)(FlashBase+Address+loop+0x2000)) ;
			} while(v1!=v2);
			do {
				v1 = *((vu16 *)(FlashBase+Address+loop+0x4000)) ;
				v2 = *((vu16 *)(FlashBase+Address+loop+0x4000)) ;
			} while(v1!=v2);
			do {
				v1 = *((vu16 *)(FlashBase+Address+loop+0x6000)) ;
				v2 = *((vu16 *)(FlashBase+Address+loop+0x6000)) ;
			} while(v1!=v2);
		}	
	} else {
		*((vu16 *)(FlashBase+off+0x555*2)) = 0xAA ;
		*((vu16 *)(FlashBase+off+0x2AA*2)) = 0x55 ;
		*((vu16 *)(FlashBase+off+0x555*2)) = 0x80 ;
		*((vu16 *)(FlashBase+off+0x555*2)) = 0xAA ;
		*((vu16 *)(FlashBase+off+0x2AA*2)) = 0x55;
		*((vu16 *)(FlashBase+Address)) = 0x30 ;
		
		*((vu16 *)(FlashBase+off+0x1555*2)) = 0xAA ;
		*((vu16 *)(FlashBase+off+0x12AA*2)) = 0x55 ;
		*((vu16 *)(FlashBase+off+0x1555*2)) = 0x80 ;
		*((vu16 *)(FlashBase+off+0x1555*2)) = 0xAA ;
		*((vu16 *)(FlashBase+off+0x12AA*2)) = 0x55 ;
		*((vu16 *)(FlashBase+Address+0x2000)) = 0x30 ;
		
		do {
			v1 = *((vu16 *)(FlashBase+Address)) ;
			v2 = *((vu16 *)(FlashBase+Address)) ;
		} while(v1!=v2);
		do {
			v1 = *((vu16 *)(FlashBase+Address+0x2000)) ;
			v2 = *((vu16 *)(FlashBase+Address+0x2000)) ;
		} while(v1!=v2);
		
		*((vu16 *)(FlashBase+off+0x555*2)) = 0xAA ;
		*((vu16 *)(FlashBase+off+0x2AA*2)) = 0x55 ;
		*((vu16 *)(FlashBase+off+0x555*2)) = 0x80 ;
		*((vu16 *)(FlashBase+off+0x555*2)) = 0xAA ;
		*((vu16 *)(FlashBase+off+0x2AA*2)) = 0x55;
		*((vu16 *)(FlashBase+Address+0x20000)) = 0x30 ;
		
		*((vu16 *)(FlashBase+off+0x1555*2)) = 0xAA ;
		*((vu16 *)(FlashBase+off+0x12AA*2)) = 0x55 ;
		*((vu16 *)(FlashBase+off+0x1555*2)) = 0x80 ;
		*((vu16 *)(FlashBase+off+0x1555*2)) = 0xAA ;
		*((vu16 *)(FlashBase+off+0x12AA*2)) = 0x55 ;
		*((vu16 *)(FlashBase+Address+0x2000+0x20000)) = 0x30 ;
	
		do {
			v1 = *((vu16 *)(FlashBase+Address+0x20000)) ;
			v2 = *((vu16 *)(FlashBase+Address+0x20000)) ;
		} while(v1!=v2);
		do {
			v1 = *((vu16 *)(FlashBase+Address+0x2000+0x20000)) ;
			v2 = *((vu16 *)(FlashBase+Address+0x2000+0x20000)) ;
		} while(v1!=v2);	
	}
}

/*void ReadNorFlash(u8* pBuf,u32 address,u16 len) {
	vu16 *p = (vu16 *)pBuf;
	u32 loop;
	u32 mapAddress = address;
	bool b512 = false;
	if(mapAddress >= 0x2000000) { //256M
		CloseNorWrite();
		SetRompage(768);
		// address-=0x2000000;
		mapAddress -= 0x2000000;
		b512 = true;
	}
	OpenNorWrite();
	if(ID==0x89168916) {
		*((vu16*)(FlashBase+mapAddress)) = 0x50;
		*((vu16*)(FlashBase+mapAddress+0x1000*2)) = 0x50;
		*((vu16*)(FlashBase+mapAddress)) = 0xFF;
		*((vu16*)(FlashBase+mapAddress+0x1000*2)) = 0xFF;
	}
	for(loop=0;loop<len/2;loop++)p[loop]=*((vu16*)(FlashBase+mapAddress+loop*2));
	if (ID==0x89168916)CloseNorWrite();
	if(b512)SetRompage(0);
}*/

void WriteNorFlashINTEL(u32 address,u8 *buffer,u32 size) {
	u32 mapaddress;
	u32 size2,lop,j;
	vu16* buf = (vu16*)buffer;
	u32 loopwrite,i;
	vu16 v1,v2;
	vu32 offset = 0;
	
	bool b512 = false;
	
	if(address >= 0x2000000) {
		// address-=0x2000000;
		offset = 0x2000000;
		CloseNorWrite();
		SetRompage(768);
		OpenNorWrite();	
		b512 = true;
	} else {
		CloseNorWrite();
		SetRompage(0);
		OpenNorWrite();
	}
	
	if(size > 0x4000) {
		size2 = size >> 1;
		lop = 2; 
	} else {
		size2 = size;
		lop = 1;
	}
	
	mapaddress = (address - offset);

	for(j=0;j<lop;j++) {
		if(j!=0) {
			mapaddress += 0x4000;
			buf = (vu16*)(buffer+0x4000);
		}
		for(loopwrite=0;loopwrite<(size2);loopwrite+=64) {
			*((vu16*)(FlashBase+mapaddress+(loopwrite>>1))) = 0x50;
			*((vu16*)(FlashBase+mapaddress+(loopwrite>>1)+0x2000)) = 0x50;
			*((vu16*)(FlashBase+mapaddress+(loopwrite>>1))) = 0xFF;
			*((vu16*)(FlashBase+mapaddress+(loopwrite>>1)+0x2000)) = 0xFF;
			*((vu16*)(FlashBase+mapaddress+(loopwrite>>1))) = 0xE8;
			*((vu16*)(FlashBase+mapaddress+(loopwrite>>1)+0x2000)) = 0xE8;
			// Disabling this fixes X button reset bug with 3in1 Plus.
			// *((vu16*)(FlashBase+mapaddress+(loopwrite>>1))) = 0x70;
			// *((vu16*)(FlashBase+mapaddress+(loopwrite>>1)+0x2000)) = 0x70;
			v1=v2=0;
			while((v1!= 0x80) || (v2 != 0x80)) {
				v1 = *((vu16 *)(FlashBase+mapaddress+(loopwrite>>1)));
				v2 = *((vu16 *)(FlashBase+mapaddress+(loopwrite>>1)+0x2000));
			}
			*((vu16 *)(FlashBase+mapaddress+(loopwrite>>1))) = 0x0F;
			*((vu16 *)(FlashBase+mapaddress+(loopwrite>>1)+0x2000)) = 0x0F;
			for(i=0;i<0x10;i++) {
				*((vu16 *)(FlashBase+mapaddress+(loopwrite>>1)+i*2)) = buf[(loopwrite>>2)+i];
				*((vu16 *)(FlashBase+mapaddress+0x2000+(loopwrite>>1)+i*2)) = buf[0x1000+(loopwrite>>2)+i];
			}
			*((vu16*)(FlashBase+mapaddress+(loopwrite>>1))) = 0xD0;
			*((vu16*)(FlashBase+mapaddress+(loopwrite>>1)+0x2000)) = 0xD0;
			v1=v2=0;
			while((v1!= 0x80) || (v2 != 0x80)) {
				v1 = *((vu16*)(FlashBase+mapaddress+(loopwrite>>1)));
				v2 = *((vu16*)(FlashBase+mapaddress+(loopwrite>>1)+0x2000));
				if((v1==0x90) || (v2==0x90)) { // error response, hopefully this never actually happens?
					WriteSram(SRAM_ADDR,(u8 *)buf,0x8000);
					while(1);
					break;
				}
			}
			// Adding this fixes 3in1 Plus reset bug with X button.
			*((vu16*)(FlashBase+mapaddress+(loopwrite>>1))) = 0xFF;
			*((vu16*)(FlashBase+mapaddress+(loopwrite>>1)+0x2000)) = 0xFF;
		}
	}
	if(b512) {
		CloseNorWrite();
		SetRompage(0);
		OpenNorWrite();
	}
}

void WriteNorFlash(u32 address,u8 *buffer,u32 size) { 
	if(ID == 0x89168916) {
		WriteNorFlashINTEL(address,buffer,size);
		return;
	}
	vu16 v1,v2;
	u32 loopwrite;
	vu16* buf = (vu16*)buffer;
	u32 size2,lop;
	u32 mapaddress;
	u32 j;
	v1=0;v2=1;
	u32 off=0;
	
	if((address >= 0x1000000) && (ID == 0x227E2202)) { off = 0x1000000; } else{ off = 0; }
	if(size>0x4000) {
		size2 = size >>1;
		lop = 2; 
	} else {
		size2 = size;
		lop = 1;
	}
	mapaddress = address;
	for(j=0;j<lop;j++) {
		if(j!=0) {
			mapaddress += 0x4000;
			buf = (vu16*)(buffer+0x4000);
		}
		for(loopwrite=0;loopwrite<(size2>>2);loopwrite++) {
			*((vu16 *)(FlashBase+off+0x555*2)) = 0xAA ;
			*((vu16 *)(FlashBase+off+0x2AA*2)) = 0x55 ;
			*((vu16 *)(FlashBase+off+0x555*2)) = 0xA0 ;
			*((vu16 *)(FlashBase+mapaddress+loopwrite*2)) = buf[loopwrite];
			
			*((vu16 *)(FlashBase+off+0x1555*2)) = 0xAA ;
			*((vu16 *)(FlashBase+off+0x12AA*2)) = 0x55 ;
			*((vu16 *)(FlashBase+off+0x1555*2)) = 0xA0 ;			
			*((vu16 *)(FlashBase+mapaddress+0x2000+loopwrite*2)) = buf[0x1000+loopwrite];
			do {
				v1 = *((vu16 *)(FlashBase+mapaddress+loopwrite*2)) ;
				v2 = *((vu16 *)(FlashBase+mapaddress+loopwrite*2)) ;
			} while(v1!=v2);
			do {
				v1 = *((vu16 *)(FlashBase+mapaddress+0x2000+loopwrite*2)) ;
				v2 = *((vu16 *)(FlashBase+mapaddress+0x2000+loopwrite*2)) ;
			} while(v1!=v2);
		}
	}	
}

void WriteSram(uint32 address, u8* data, uint32 size) {	
	uint32 i;
	for(i = 0; i < size; i++)*(u8*)(address+i) = data[i];
}

void ReadSram(uint32 address, u8* data, uint32 size) {
	uint32 i;
	for(i = 0; i < size; i++)data[i] = *(u8*)(address + i);
}

void OpenRamWrite() {
	*(vu16*)0x9fe0000 = 0xd200;
	*(vu16*)0x8000000 = 0x1500;
	*(vu16*)0x8020000 = 0xd200;
	*(vu16*)0x8040000 = 0x1500;
	*(vu16*)0x9C40000 = 0xA500;
	*(vu16*)0x9fc0000 = 0x1500;
}

void CloseRamWrite() {
	*(vu16*)0x9fe0000 = 0xd200;
	*(vu16*)0x8000000 = 0x1500;
	*(vu16*)0x8020000 = 0xd200;
	*(vu16*)0x8040000 = 0x1500;
	*(vu16*)0x9C40000 = 0xA200;
	*(vu16*)0x9fc0000 = 0x1500;
}

void SetShake(u16 data) {
	*(vu16*)0x9fe0000 = 0xd200;
	*(vu16*)0x8000000 = 0x1500;
	*(vu16*)0x8020000 = 0xd200;
	*(vu16*)0x8040000 = 0x1500;
	*(vu16*)0x9E20000 = data;
	*(vu16*)0x9fc0000 = 0x1500;
}
#ifdef __cplusplus
}
#endif

