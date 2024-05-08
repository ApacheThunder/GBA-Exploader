/**************************************************************************************************************
 * 此文件为 dsCard.cpp 文件的第二版 
 * 日期：2006年11月27日11点33分  第一版 version 1.0
 * 作者：aladdin
 * CopyRight : EZFlash Group
 * 
 **************************************************************************************************************/
#include "dscard.h"
#include "string.h"

#ifdef __cplusplus
extern "C" {
#endif

static	u32	ID = 0x227E2218;

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
void		OpenNorWrite()
{
	*(vuint16 *)0x9fe0000 = 0xd200;
	*(vuint16 *)0x8000000 = 0x1500;
	*(vuint16 *)0x8020000 = 0xd200;
	*(vuint16 *)0x8040000 = 0x1500;
	*(vuint16 *)0x9C40000 = 0x1500;
	*(vuint16 *)0x9fc0000 = 0x1500;
}


void		CloseNorWrite()
{
	*(vuint16 *)0x9fe0000 = 0xd200;
	*(vuint16 *)0x8000000 = 0x1500;
	*(vuint16 *)0x8020000 = 0xd200;
	*(vuint16 *)0x8040000 = 0x1500;
	*(vuint16 *)0x9C40000 = 0xd200;
	*(vuint16 *)0x9fc0000 = 0x1500;
}

void SetRompage(u16 page)
{
	*(vuint16 *)0x9fe0000 = 0xd200;
	*(vuint16 *)0x8000000 = 0x1500;
	*(vuint16 *)0x8020000 = 0xd200;
	*(vuint16 *)0x8040000 = 0x1500;
	*(vuint16 *)0x9880000 = page;
	*(vuint16 *)0x9fc0000 = 0x1500;
}
void SetRampage(u16 page)
{
	*(vu16 *)0x9fe0000 = 0xd200;
	*(vu16 *)0x8000000 = 0x1500;
	*(vu16 *)0x8020000 = 0xd200;
	*(vu16 *)0x8040000 = 0x1500;
	*(vu16 *)0x9c00000 = page;
	*(vu16 *)0x9fc0000 = 0x1500;
}
void SetSerialMode()
{
	
	*(vu16 *)0x9fe0000 = 0xd200;
	*(vu16 *)0x8000000 = 0x1500;
	*(vu16 *)0x8020000 = 0xd200;
	*(vu16 *)0x8040000 = 0x1500;
	*(vu16 *)0x9A40000 = 0xe200;
	*(vu16 *)0x9fc0000 = 0x1500;
	
}
uint32   ReadNorFlashID()
{
		vuint16 id1,id2;	//,id3,id4;
		ID=0;
		*((vuint16 *)(FlashBase+0x555*2)) = 0xAA ;
		*((vuint16 *)(FlashBase+0x2AA*2)) = 0x55 ;
		*((vuint16 *)(FlashBase+0x555*2)) = 0x90 ;

		*((vuint16 *)(FlashBase+0x1555*2)) = 0xAA ;
		*((vuint16 *)(FlashBase+0x12AA*2)) = 0x55 ;
		*((vuint16 *)(FlashBase+0x1555*2)) = 0x90 ;

		id1 = *((vuint16 *)(FlashBase+0x2)) ;
		id2 = *((vuint16 *)(FlashBase+0x2002)) ;
		if( (id1!=0x227E)|| (id2!=0x227E))
			return 0;
		
		id1 = *((vuint16 *)(FlashBase+0xE*2)) ;
		id2 = *((vuint16 *)(FlashBase+0x100e*2)) ;
		if(id1==0x2218 && id2==0x2218)			//H6H6
		{
			ID = 0x227E2218;
			return 0x227E2218;
		}
			
		if(id1==0x2202 && id2==0x2202)			//VZ064
		{
			ID = 0x227E2202;
			return 0x227E2202;
		}
		if(id1==0x2202 && id2==0x2220)			//VZ064
		{
			ID = 0x227E2202;
			return 0x227E2202;
		}
		if(id1==0x2202 && id2==0x2215)			//VZ064
		{
			ID = 0x227E2202;
			return 0x227E2202;
		}
	return 0x227E2220;			
}
void chip_reset()
{
		*((vu16 *)(FlashBase)) = 0xF0 ;
		*((vu16 *)(FlashBase+0x1000*2)) = 0xF0 ;
		
		if(ID==0x227E2202)
		{
			*((vu16 *)(FlashBase+0x1000000)) = 0xF0 ;
			*((vu16 *)(FlashBase+0x1000000+0x1000*2)) = 0xF0 ;
		}
}

void Block_Erase(u32 blockAdd)
{
		vu16 page,v1,v2;  
		u32 Address;
		u32 loop;
		u32 off=0;
		if( (blockAdd>=0x1000000) &&  (ID==0x227E2202))
		{
			off=0x1000000;
			*((vu16 *)(FlashBase+off+0x555*2)) = 0xF0 ;
			*((vu16 *)(FlashBase+off+0x1555*2)) = 0xF0 ;
		}
		else
			off=0;
		Address=blockAdd;
		*((vu16 *)(FlashBase+0x555*2)) = 0xF0 ;
		*((vu16 *)(FlashBase+0x1555*2)) = 0xF0 ;
		
	
		if( (blockAdd==0) || (blockAdd==0x1FC0000) || (blockAdd==0xFC0000) || (blockAdd==0x1000000))
		{
			for(loop=0;loop<0x40000;loop+=0x8000)
			{
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
				do
				{  
					
					v1 = *((vu16 *)(FlashBase+Address+loop)) ;
					v2 = *((vu16 *)(FlashBase+Address+loop)) ;
				}while(v1!=v2);
				do
				{
					
					v1 = *((vu16 *)(FlashBase+Address+loop+0x2000)) ;
					v2 = *((vu16 *)(FlashBase+Address+loop+0x2000)) ;
				}while(v1!=v2);
				do
				{
					
					v1 = *((vu16 *)(FlashBase+Address+loop+0x4000)) ;
					v2 = *((vu16 *)(FlashBase+Address+loop+0x4000)) ;
				}while(v1!=v2);
				do
				{
					
					v1 = *((vu16 *)(FlashBase+Address+loop+0x6000)) ;
					v2 = *((vu16 *)(FlashBase+Address+loop+0x6000)) ;
				}while(v1!=v2);
			}	
		}	
		else
		{
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
			
			do
			{
				v1 = *((vu16 *)(FlashBase+Address)) ;
				v2 = *((vu16 *)(FlashBase+Address)) ;
			}while(v1!=v2);
			do
			{
				v1 = *((vu16 *)(FlashBase+Address+0x2000)) ;
				v2 = *((vu16 *)(FlashBase+Address+0x2000)) ;
			}while(v1!=v2);
			
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
		
			do
			{
				v1 = *((vu16 *)(FlashBase+Address+0x20000)) ;
				v2 = *((vu16 *)(FlashBase+Address+0x20000)) ;
			}while(v1!=v2);
			do
			{
				v1 = *((vu16 *)(FlashBase+Address+0x2000+0x20000)) ;
				v2 = *((vu16 *)(FlashBase+Address+0x2000+0x20000)) ;
			}while(v1!=v2);	
		}
}
void 		ReadNorFlash(u8* pBuf,u32 address,u16 len)
{
	vu16 *p = (vu16 *)pBuf;
	u32 loop;
	for(loop=0;loop<len/2;loop++)
	{
		p[loop]=*((vu16 *)(FlashBase+address+loop*2) );
	}	
}

void WriteNorFlash(u32 address,u8 *buffer,u32 size)
{
		vu16 v1,v2;
		register u32 loopwrite ;
		vu16* buf = (vu16*)buffer ;
		u32 size2,lop;
		u32 mapaddress;
		u32 j;
		v1=0;v2=1;
		u32 off=0;
		if( (address>=0x1000000) &&  (ID==0x227E2202))
		{
			off=0x1000000;
		}
		else
			off=0;
		if(size>0x4000)
		{
			size2 = size >>1 ;
			lop = 2; 
		}
		else 
		{
			size2 = size  ;
			lop = 1;
		}
		mapaddress = address;
		for(j=0;j<lop;j++)
		{
			if(j!=0)
			{
				mapaddress += 0x4000;
				buf = (vu16*)(buffer+0x4000);
			}
			for(loopwrite=0;loopwrite<(size2>>2);loopwrite++)
			{
				*((vu16 *)(FlashBase+off+0x555*2)) = 0xAA ;
				*((vu16 *)(FlashBase+off+0x2AA*2)) = 0x55 ;
				*((vu16 *)(FlashBase+off+0x555*2)) = 0xA0 ;
				*((vu16 *)(FlashBase+mapaddress+loopwrite*2)) = buf[loopwrite];
				
				*((vu16 *)(FlashBase+off+0x1555*2)) = 0xAA ;
				*((vu16 *)(FlashBase+off+0x12AA*2)) = 0x55 ;
				*((vu16 *)(FlashBase+off+0x1555*2)) = 0xA0 ;			
				*((vu16 *)(FlashBase+mapaddress+0x2000+loopwrite*2)) = buf[0x1000+loopwrite];
				do
				{
					v1 = *((vu16 *)(FlashBase+mapaddress+loopwrite*2)) ;
					v2 = *((vu16 *)(FlashBase+mapaddress+loopwrite*2)) ;
				}while(v1!=v2);
				do
				{
					v1 = *((vu16 *)(FlashBase+mapaddress+0x2000+loopwrite*2)) ;
					v2 = *((vu16 *)(FlashBase+mapaddress+0x2000+loopwrite*2)) ;
				}while(v1!=v2);
			}
		}	
}
void WriteSram(uint32 address, u8* data , uint32 size )
{	
	uint32 i ;
	for(i=0;i<size;i++)
		*(u8*)(address+i)=data[i];
}
void ReadSram(uint32 address, u8* data , uint32 size )
{
	uint32 i ;

	for(i=0;i<size;i++)
		data[i] = *(u8*)(address+i);

/***
	u16* pData = (u16*)data;

	for(i=0;i<size;i+=2)
	{
		pData[i>>1]=*(u8*)(address+i)+(*(u8*)(address+i+1)*0x100);
	}
**/
}
void  OpenRamWrite()
{
	*(vu16 *)0x9fe0000 = 0xd200;
	*(vu16 *)0x8000000 = 0x1500;
	*(vu16 *)0x8020000 = 0xd200;
	*(vu16 *)0x8040000 = 0x1500;
	*(vu16 *)0x9C40000 = 0xA500;
	*(vu16 *)0x9fc0000 = 0x1500;
}

void CloseRamWrite()
{
	*(vu16 *)0x9fe0000 = 0xd200;
	*(vu16 *)0x8000000 = 0x1500;
	*(vu16 *)0x8020000 = 0xd200;
	*(vu16 *)0x8040000 = 0x1500;
	*(vu16 *)0x9C40000 = 0xA200;
	*(vu16 *)0x9fc0000 = 0x1500;
}
void SetShake(u16 data)
{
	*(vuint16 *)0x9fe0000 = 0xd200;
	*(vuint16 *)0x8000000 = 0x1500;
	*(vuint16 *)0x8020000 = 0xd200;
	*(vuint16 *)0x8040000 = 0x1500;
	*(vuint16 *)0x9E20000 = data;
	*(vuint16 *)0x9fc0000 = 0x1500;
}
#ifdef __cplusplus
}
#endif
