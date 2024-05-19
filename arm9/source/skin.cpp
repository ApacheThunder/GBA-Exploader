#include <fat.h>
#include <nds.h>
#include <sys/dir.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "skin.h"


#define BI_RGB (0)
#define BI_RLE8 (1)
#define BI_RLE4 (2)
#define BI_Bitfields (3)

typedef struct {
	u8 bfType[2];
	u32 bfSize;
	u16 bfReserved1;
	u16 bfReserved2;
	u32 bfOffset;
	u32 biSize;
	u32 biWidth;
	u32 biHeight;
	u16 biPlanes;
	u16 biBitCount;
	u32 biCopmression;
	u32 biSizeImage;
	u32 biXPixPerMeter;
	u32 biYPixPerMeter;
	u32 biClrUsed;
	u32 biCirImportant;
	u8 *pPalette;
	u8 *pBitmap;
	u32 DataWidth;
} TBMPHeader;


static u16 GetVariable16bit(void *pb) {
  u16 res;
  u8 *pb8=(u8*)pb;
  
  res=(u32)pb8[0] << 0;
  res+=(u32)pb8[1] << 8;
  
  return(res);
}

static u32 GetVariable32bit(void *pb) {
  u32 res;
  u8 *pb8=(u8*)pb;
  
  res=(u32)pb8[0] << 0;
  res+=(u32)pb8[1] << 8;
  res+=(u32)pb8[2] << 16;
  res+=(u32)pb8[3] << 24;
  
  return(res);
}


static bool GetBMPHeader(u8 *pb,TBMPHeader *pBMPHeader) {
  if(pb==NULL){
//    BMP_LoadErrorStr="SourceData Null.";
    return(false);
  }
  if(pBMPHeader==NULL){
//    BMP_LoadErrorStr="pBMPHeader Null.";
    return(false);
  }
  
  pBMPHeader->bfType[0]=pb[0];
  pBMPHeader->bfType[1]=pb[1];
  pBMPHeader->bfSize=GetVariable32bit(&pb[2]);
  pBMPHeader->bfReserved1=GetVariable16bit(&pb[6]);
  pBMPHeader->bfReserved2=GetVariable16bit(&pb[8]);
  pBMPHeader->bfOffset=GetVariable32bit(&pb[10]);
  pBMPHeader->biSize=GetVariable32bit(&pb[14+0]);
  pBMPHeader->biWidth=GetVariable32bit(&pb[14+4]);
  pBMPHeader->biHeight=GetVariable32bit(&pb[14+8]);
  pBMPHeader->biPlanes=GetVariable16bit(&pb[14+12]);
  pBMPHeader->biBitCount=GetVariable16bit(&pb[14+14]);
  pBMPHeader->biCopmression=GetVariable32bit(&pb[14+16]);
  pBMPHeader->biSizeImage=GetVariable32bit(&pb[14+20]);
  pBMPHeader->biXPixPerMeter=GetVariable32bit(&pb[14+24]);
  pBMPHeader->biYPixPerMeter=GetVariable32bit(&pb[14+28]);
  pBMPHeader->biClrUsed=GetVariable32bit(&pb[14+32]);
  pBMPHeader->biCirImportant=GetVariable32bit(&pb[14+36]);
  
  pBMPHeader->pPalette=&pb[14+40];
  pBMPHeader->pBitmap=&pb[pBMPHeader->bfOffset];
  
  pBMPHeader->DataWidth=0;
  
  if((pBMPHeader->bfType[0]!='B')||(pBMPHeader->bfType[1]!='M')){
//    BMP_LoadErrorStr="Error MagicID!=BM";
    return(false);
  }
  
  if(pBMPHeader->biCopmression!=BI_RGB){
//    BMP_LoadErrorStr="Error notsupport Compression";
    return(false);
  }
  
  if(pBMPHeader->biHeight>=0x80000000){
//    BMP_LoadErrorStr="Error notsupport OS/2 format";
    return(false);
  }
  
  if(pBMPHeader->biPlanes!=1){
//    BMP_LoadErrorStr="Error notsupport Planes!=1";
    return(false);
  }
  
  switch(pBMPHeader->biBitCount){
    case 1:
//      BMP_LoadErrorStr="Error notsupport 1bitcolor.";
      return(false);
    case 4:
//      BMP_LoadErrorStr="Error notsupport 4bitcolor.";
      return(false);
    case 8:
      pBMPHeader->DataWidth=pBMPHeader->biWidth*1;
      break;
    case 16:
//      BMP_LoadErrorStr="Error notsupport 16bitcolor.";
      return(false);
    case 24:
      pBMPHeader->DataWidth=pBMPHeader->biWidth*3;
      break;
    case 32:
      pBMPHeader->DataWidth=pBMPHeader->biWidth*4;
      break;
    default:
//      BMP_LoadErrorStr="Error Unknown xxBitColor.";
      return(false);
  }
  
  if((pBMPHeader->DataWidth&3)!=0){
    pBMPHeader->DataWidth+=4-(pBMPHeader->DataWidth&3);
  }
  
//  BMP_LoadErrorStr="";
  return(true);
}


static bool intLoadBM(const char *bmpfn,u16 *pbm,const u32 bmw,const u32 bmh) {
	FILE *fh;


//  bmerrstr1[0]=0;
//  bmerrstr2[0]=0;
  
	if(pbm == NULL) {
//		snprintf(bmerrstr1,256,"BitmapMemory is NULL.");
//		snprintf(bmerrstr2,256,"The memory is insufficient?");
//		_consolePrintf("%s\n",bmerrstr1);
//		_consolePrintf("%s\n",bmerrstr2);
		return(false);
	}
  
  u8 *bmdata = NULL;
  u32 bmsize;

    fh=fopen(bmpfn, "rb");

  
	if(fh != NULL) {
		fseek(fh, 0, SEEK_END);
		bmsize=ftell(fh);
		fseek(fh, 0, SEEK_SET);
		bmdata = (u8*)malloc(bmsize);
		
		fread(bmdata, 1, bmsize, fh);
		fclose(fh);
	}

	if(bmdata==NULL){
		return(false);
	}/* else {
		_consolePrintf("loadskin /shell/%s\n",bmpfn);
	}*/
  
	TBMPHeader BMPHeader;
  
	if(!GetBMPHeader(bmdata,&BMPHeader)) {
//		snprintf(bmerrstr1,256,"Request /shell/%s WindowsBitmapFormat",bmpfn);
//		snprintf(bmerrstr2,256,"%s",BMP_LoadErrorStr);
//		_consolePrintf("%s\n",bmerrstr1);
//		_consolePrintf("%s\n",bmerrstr2);
		free(bmdata); bmdata=NULL;
		return(false);
	}
  
	if((BMPHeader.biWidth==1)&&(BMPHeader.biHeight==1)) {
		free(bmdata);
		bmdata=NULL;
		return false;
	}
  
	if(BMPHeader.biBitCount==32) {
		// _consolePrintf("Error. not support 32bit color.");
		free(bmdata);
		bmdata=NULL;
		return false;
	}
  
	if((BMPHeader.biWidth<bmw)||(BMPHeader.biHeight<bmh)) {
//		snprintf(bmerrstr1,256,"Request /shell/%s WindowsBitmapFormat",bmpfn);
//		snprintf(bmerrstr2,256,"%d x %dpixel 8 or 24bitcolor NoCompression.",bmw,bmh);
//		_consolePrintf("%s\n",bmerrstr1);
//		_consolePrintf("%s\n",bmerrstr2);
		free(bmdata);
		bmdata=NULL;
		return false;
	}
  
	u32 gr=0,gg=0,gb=0;
  
#define Gravity(c,cg) { \
	c+=cg; \
	cg=c&7; \
	c=c>>3; \
	if((c&(~0x1f))!=0) c=(c<0) ? 0x00 : 0x1f; \
}

	for(u32 y=0;y<bmh;y++) {
		u8 *pSrcBM=&BMPHeader.pBitmap[(BMPHeader.biHeight-1-y)*BMPHeader.DataWidth];
		u16 *pDstBM=&pbm[y*bmw];		
		switch(BMPHeader.biBitCount) {
			case 8: {
				u8 *PaletteTable=BMPHeader.pPalette;
				for(u32 x=0;x<bmw;x++){
					u8 *pal;
					u32 r,g,b;
					
					pal=&PaletteTable[*pSrcBM*4];
					pSrcBM+=1;
					
					b=pal[0];
					g=pal[1];
					r=pal[2];
					
					Gravity(b,gb);
					Gravity(g,gg);
					Gravity(r,gr);
					
					pDstBM[x]=RGB15(r,g,b) | BIT(15);
				}
				break;
			}
			case 24: {
				for(u32 x=0;x<bmw;x++) {
					u32 r,g,b;
					
					b=pSrcBM[0];
					g=pSrcBM[1];
					r=pSrcBM[2];
					pSrcBM+=3;
					
					Gravity(b,gb);
					Gravity(g,gg);
					Gravity(r,gr);
					
					pDstBM[x]=RGB15(r,g,b) | BIT(15);
				}
				break;
			}
		}
	}
  
#undef Gravity

	free(bmdata);
	bmdata=NULL;
  
	return true;
}


ALIGN(4) static u16 *pBuf;

bool LoadSkin(int mod, char *Name) {
	
	u16	*pDstBuf1;
	u16	*pDstBuf2;

	pBuf = (u16*)malloc(256*192*2);
	if(!intLoadBM(Name, pBuf, 256, 192)) { free(pBuf); return false; }

	switch (mod) {
		case 0: pDstBuf1 = (u16*)0x06020000; break;
		case 1: pDstBuf1 = (u16*)0x06220000; break;
		case 2:
			pDstBuf1 = (u16*)0x06000000;
			pDstBuf2 = (u16*)0x06020000;
			// pDstBuf1 = (u16*)0x06800000;
			// pDstBuf2 = (u16*)0x06820000;
			break;
		default: pDstBuf1 = (u16*)0x06020000; break;
	}

	for(s32 y=0;y<192;y++) {
		for(s32 x=0;x<256;x++) {
			pDstBuf1[x]=pBuf[x];
			if(mod == 2)pDstBuf2[x]=pBuf[x];
		}
		pDstBuf1+=256;
		if(mod == 2)pDstBuf2+=256;
		pBuf+=256;
	}

	// free(pBuf);
	return true;
}

