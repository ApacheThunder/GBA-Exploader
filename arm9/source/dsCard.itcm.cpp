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


#ifdef __cplusplus
extern "C" {
#endif

static u32 ID = 0x227E2218;

static bool checkForSuperCard() {
	_SC_changeMode(SC_MODE_RAM);	// Try again with SuperCard
	// _SC_changeMode16(0x1510);
	*(vu16*)(0x08000000) = 0x4D54;
	if (*(vu16*)(0x08000000) == 0x4D54)return true;
	return false;
}

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
	
	if ((id1 != 0x227E) || (id2 != 0x227E)) {
		if (checkForSuperCard()) {
			ID = 0x227E2202;
			return 0x227E0000;
		}
		return 0;
	}
	
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
	return 0x227E2220;
}

void chip_reset() {
	if(ID == 0x89168916) {
		*((vu16*)(FlashBase+0)) = 0x50;
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
		OpenNorWrite();
	}
	if(ID==0x89168916) {
		*((vu16*)(FlashBase+mapAddress)) = 0x50;
		*((vu16*)(FlashBase+mapAddress+0x1000*2)) = 0x50;
		*((vu16*)(FlashBase+mapAddress)) = 0xFF;
		*((vu16*)(FlashBase+mapAddress+0x1000*2)) = 0xFF;
	}
	for(loop=0;loop<len/2;loop++)p[loop]=*((vu16*)(FlashBase+mapAddress+loop*2));
	// if (ID==0x89168916)CloseNorWrite();
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
	
	if(size>0x4000) {
		size2 = size >>1;
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
			*((vu16*)(FlashBase+mapaddress+(loopwrite>>1))) = 0x70;
			*((vu16*)(FlashBase+mapaddress+(loopwrite>>1)+0x2000)) = 0x70;
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
					WriteSram(0xA000000,(u8 *)buf,0x8000);
					while(1);
					break;
				}
			}
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

void WriteSram(uint32 address, u8* data , uint32 size ) {	
	uint32 i ;
	for(i=0;i<size;i++)*(u8*)(address+i)=data[i];
}

void ReadSram(uint32 address, u8* data , uint32 size ) {
	uint32 i;
	for(i=0;i<size;i++)data[i] = *(u8*)(address+i);
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

