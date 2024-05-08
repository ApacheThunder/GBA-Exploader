//************************************************************************
//
//  日本語表示関連のプログラム   2006.07.25
//
//************************************************************************
#include <nds.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <stdlib.h>
#include <unistd.h>


//東雲フォントデータ用ヘッダ
#include "shinonome_bin.h"		//全角ゴシックフォント
#include "shinonomeank_bin.h"	//半角フォント

#include "tarosa_Graphic.h"
#include "tarosa_Shinofont.h"


/**********************************************
*                                             *
*    SJISコードから恵梨沙フォントデータを     *
*           格納アドレスを計算する            *
*                                             *
**********************************************/
short Sjis2Elisa( short u, short d )
{
short c;

	if (d > 0x7f) d--;
	d -= 0x40;
	if( u>=0xe0 )	u -= 0x40;
	u -= 0x81;
	c = u * 0xbc + d;

	if(c >= 4418) {			/* 第二水準漢字*/
       	return (c - 929);
	} else if (c >= 1410) {		/* 第一水準漢字*/   
		return c + (0x20c - 1410);
	} else if (c >= 690) {  /* 禁止領域の文字 */
		return 0x6c;
	} else if (c >= 658) {		/* 罫線素 */
		return c + (0x1ec - 658);
	} else if (c >= 612) {  /* ロシア小 */
		return c + (0x1cb - 612);
	} else if (c >= 564) {  /* ロシア大 */
		return c + (0x1aa - 564);
	} else if (c >= 502) {  /* ギリシャ小 */
		return c + (0x192 - 502);
	} else if (c >= 470) {  /* ギリシャ大*/
		return c + (0x17a - 470);
	} else if (c >= 376) {  /* カタカナ */
		return c + (0x124 - 376);
	} else if (c >= 282) {  /* ひらがな */
		return c + (0xd1 - 282);
	} else if (c >= 252) {  /* 英小文字 */
		return c + (0xb7 - 252);
	} else if (c >= 220) {  /* 英大文字 */
		return c + (0x9d - 220);
	} else if (c >= 203) {  /* 数字 */
		return c + (0x93 - 203);
	} else if (c >= 187) { /* 記号(◯) */
		return 0x92;
	} else if (c >= 175) { /* 記号(Å‰♯♭♪†‡¶) */
		return c + (0xa6 - 203);
	} else if (c >= 153) { /* 記号(∠⊥⌒∂∇≡≒≪≫√∽∝∵∫∬) */
		return c + (0x7b - 153);
	} else if (c >= 135) { /* 記号(∧∨￢⇒⇔∀∃) */
		return c + (0x74 - 135);
	} else if (c >= 119) { /* 記号(∈∋⊆⊇⊂⊃∪∩) */
		return c + (0x6c - 119);
	} else return c; /* 記号(その他) */
}

/**********************************************
*                                             *
*    メイン画面に東雲12フォントを表示する     *
*        Fpalet:文字色                        *
*        Bpalet:背景色                        *
*        kaki  :or書きするか and書きするか    *
*               0=or , 1=and                  *
*                                             *
**********************************************/
void ShinoPrint( uint16* screen, int x, int y, u8 *st, u16 Fcolor, u16 Bcolor, u8 kaki )
{
short i, k, p;
short su, sd;

	while( 1 ){
		su = *st;
		st++;
		if( ((su>=0x81)&&(su<=0x9f)) || ((su>=0xe0)&&(su<=0xfc)) ){
			sd = *st;
			st++;
			if( su==0 || sd==0 )	break;
			k = Sjis2Elisa( su, sd );
			for( i=0; i<6; i++ ){
				p = shinonome_bin[k*18+i*3];
				if((p&0x1)!=0) 	Pixel( screen, x+7, y+i*2, Fcolor );
				if((p&0x2)!=0) 	Pixel( screen, x+6, y+i*2, Fcolor );
				if((p&0x4)!=0) 	Pixel( screen, x+5, y+i*2, Fcolor );
				if((p&0x8)!=0) 	Pixel( screen, x+4, y+i*2, Fcolor );
				if((p&0x10)!=0) 	Pixel( screen, x+3, y+i*2, Fcolor );
				if((p&0x20)!=0) 	Pixel( screen, x+2, y+i*2, Fcolor );
				if((p&0x40)!=0) 	Pixel( screen, x+1, y+i*2, Fcolor );
				if((p&0x80)!=0) 	Pixel( screen, x+0, y+i*2, Fcolor );
				if( kaki==1 ){
					if((p&0x1)==0) 	Pixel( screen, x+7, y+i*2, Bcolor );
					if((p&0x2)==0) 	Pixel( screen, x+6, y+i*2, Bcolor );
					if((p&0x4)==0) 	Pixel( screen, x+5, y+i*2, Bcolor );
					if((p&0x8)==0) 	Pixel( screen, x+4, y+i*2, Bcolor );
					if((p&0x10)==0) 	Pixel( screen, x+3, y+i*2, Bcolor );
					if((p&0x20)==0) 	Pixel( screen, x+2, y+i*2, Bcolor );
					if((p&0x40)==0) 	Pixel( screen, x+1, y+i*2, Bcolor );
					if((p&0x80)==0) 	Pixel( screen, x+0, y+i*2, Bcolor );
				}
				p = shinonome_bin[k*18+i*3+1];
				if((p&0x1)!=0) 	Pixel( screen, x+7, y+i*2+1, Fcolor );
				if((p&0x2)!=0) 	Pixel( screen, x+6, y+i*2+1, Fcolor );
				if((p&0x4)!=0) 	Pixel( screen, x+5, y+i*2+1, Fcolor );
				if((p&0x8)!=0) 	Pixel( screen, x+4, y+i*2+1, Fcolor );
				if((p&0x10)!=0) 	Pixel( screen, x+3, y+i*2+1, Fcolor );
				if((p&0x20)!=0) 	Pixel( screen, x+2, y+i*2+1, Fcolor );
				if((p&0x40)!=0) 	Pixel( screen, x+1, y+i*2+1, Fcolor );
				if((p&0x80)!=0) 	Pixel( screen, x+0, y+i*2+1, Fcolor );
				if( kaki==1 ){
					if((p&0x1)==0) 	Pixel( screen, x+7, y+i*2+1, Bcolor );
					if((p&0x2)==0) 	Pixel( screen, x+6, y+i*2+1, Bcolor );
					if((p&0x4)==0) 	Pixel( screen, x+5, y+i*2+1, Bcolor );
					if((p&0x8)==0) 	Pixel( screen, x+4, y+i*2+1, Bcolor );
					if((p&0x10)==0) 	Pixel( screen, x+3, y+i*2+1, Bcolor );
					if((p&0x20)==0) 	Pixel( screen, x+2, y+i*2+1, Bcolor );
					if((p&0x40)==0) 	Pixel( screen, x+1, y+i*2+1, Bcolor );
					if((p&0x80)==0) 	Pixel( screen, x+0, y+i*2+1, Bcolor );
				}
				p = shinonome_bin[k*18+i*3+2];
				if((p&0x1)!=0) 	Pixel( screen, x+11, y+i*2+1, Fcolor );
				if((p&0x2)!=0) 	Pixel( screen, x+10, y+i*2+1, Fcolor );
				if((p&0x4)!=0) 	Pixel( screen, x+9, y+i*2+1, Fcolor );
				if((p&0x8)!=0) 	Pixel( screen, x+8, y+i*2+1, Fcolor );
				if((p&0x10)!=0) 	Pixel( screen, x+11, y+i*2, Fcolor );
				if((p&0x20)!=0) 	Pixel( screen, x+10, y+i*2, Fcolor );
				if((p&0x40)!=0) 	Pixel( screen, x+9, y+i*2, Fcolor );
				if((p&0x80)!=0) 	Pixel( screen, x+8, y+i*2, Fcolor );
				if( kaki==1 ){
					if((p&0x1)==0) 	Pixel( screen, x+11, y+i*2+1, Bcolor );
					if((p&0x2)==0) 	Pixel( screen, x+10, y+i*2+1, Bcolor );
					if((p&0x4)==0) 	Pixel( screen, x+9, y+i*2+1, Bcolor );
					if((p&0x8)==0) 	Pixel( screen, x+8, y+i*2+1, Bcolor );
					if((p&0x10)==0) 	Pixel( screen, x+11, y+i*2, Bcolor );
					if((p&0x20)==0) 	Pixel( screen, x+10, y+i*2, Bcolor );
					if((p&0x40)==0) 	Pixel( screen, x+9, y+i*2, Bcolor );
					if((p&0x80)==0) 	Pixel( screen, x+8, y+i*2, Bcolor );
				}
			}
			x += 12;
		}
		else{	//半角文字だよ
			if( su==0 )	break;
//			k = su * 9;
			for( i=0; i<3; i++ ){
				p = shinonomeank_bin[su*9+i*3];
				if((p&0x80)!=0)	Pixel( screen, x+0, y+i*4, Fcolor );
				if((p&0x40)!=0) 	Pixel( screen, x+1, y+i*4, Fcolor );
				if((p&0x20)!=0) 	Pixel( screen, x+2, y+i*4, Fcolor );
				if((p&0x10)!=0) 	Pixel( screen, x+3, y+i*4, Fcolor );
				if((p&0x8)!=0) 	Pixel( screen, x+4, y+i*4, Fcolor );
				if((p&0x4)!=0) 	Pixel( screen, x+5, y+i*4, Fcolor );
				if((p&0x2)!=0) 	Pixel( screen, x+0, y+i*4+1, Fcolor );
				if((p&0x1)!=0) 	Pixel( screen, x+1, y+i*4+1, Fcolor );
				if( kaki==1 ){
					if((p&0x80)==0)	Pixel( screen, x+0, y+i*4, Bcolor );
					if((p&0x40)==0) 	Pixel( screen, x+1, y+i*4, Bcolor );
					if((p&0x20)==0) 	Pixel( screen, x+2, y+i*4, Bcolor );
					if((p&0x10)==0) 	Pixel( screen, x+3, y+i*4, Bcolor );
					if((p&0x8)==0) 	Pixel( screen, x+4, y+i*4, Bcolor );
					if((p&0x4)==0) 	Pixel( screen, x+5, y+i*4, Bcolor );
					if((p&0x2)==0) 	Pixel( screen, x+0, y+i*4+1, Bcolor );
					if((p&0x1)==0) 	Pixel( screen, x+1, y+i*4+1, Bcolor );
				}
				p = shinonomeank_bin[su*9+i*3+1];
				if((p&0x80)!=0)	Pixel( screen, x+2, y+i*4+1, Fcolor );
				if((p&0x40)!=0) 	Pixel( screen, x+3, y+i*4+1, Fcolor );
				if((p&0x20)!=0) 	Pixel( screen, x+4, y+i*4+1, Fcolor );
				if((p&0x10)!=0) 	Pixel( screen, x+5, y+i*4+1, Fcolor );
				if((p&0x8)!=0) 	Pixel( screen, x+0, y+i*4+2, Fcolor );
				if((p&0x4)!=0) 	Pixel( screen, x+1, y+i*4+2, Fcolor );
				if((p&0x2)!=0) 	Pixel( screen, x+2, y+i*4+2, Fcolor );
				if((p&0x1)!=0) 	Pixel( screen, x+3, y+i*4+2, Fcolor );
				if( kaki==1 ){
					if((p&0x80)==0)	Pixel( screen, x+2, y+i*4+1, Bcolor );
					if((p&0x40)==0) 	Pixel( screen, x+3, y+i*4+1, Bcolor );
					if((p&0x20)==0) 	Pixel( screen, x+4, y+i*4+1, Bcolor );
					if((p&0x10)==0) 	Pixel( screen, x+5, y+i*4+1, Bcolor );
					if((p&0x8)==0) 	Pixel( screen, x+0, y+i*4+2, Bcolor );
					if((p&0x4)==0) 	Pixel( screen, x+1, y+i*4+2, Bcolor );
					if((p&0x2)==0) 	Pixel( screen, x+2, y+i*4+2, Bcolor );
					if((p&0x1)==0) 	Pixel( screen, x+3, y+i*4+2, Bcolor );
				}
				p = shinonomeank_bin[su*9+i*3+2];
				if((p&0x80)!=0)	Pixel( screen, x+4, y+i*4+2, Fcolor );
				if((p&0x40)!=0) 	Pixel( screen, x+5, y+i*4+2, Fcolor );
				if((p&0x20)!=0) 	Pixel( screen, x+0, y+i*4+3, Fcolor );
				if((p&0x10)!=0) 	Pixel( screen, x+1, y+i*4+3, Fcolor );
				if((p&0x8)!=0) 	Pixel( screen, x+2, y+i*4+3, Fcolor );
				if((p&0x4)!=0) 	Pixel( screen, x+3, y+i*4+3, Fcolor );
				if((p&0x2)!=0) 	Pixel( screen, x+4, y+i*4+3, Fcolor );
				if((p&0x1)!=0) 	Pixel( screen, x+5, y+i*4+3, Fcolor );
				if( kaki==1 ){
					if((p&0x80)==0)	Pixel( screen, x+4, y+i*4+2, Bcolor );
					if((p&0x40)==0) 	Pixel( screen, x+5, y+i*4+2, Bcolor );
					if((p&0x20)==0) 	Pixel( screen, x+0, y+i*4+3, Bcolor );
					if((p&0x10)==0) 	Pixel( screen, x+1, y+i*4+3, Bcolor );
					if((p&0x8)==0) 	Pixel( screen, x+2, y+i*4+3, Bcolor );
					if((p&0x4)==0) 	Pixel( screen, x+3, y+i*4+3, Bcolor );
					if((p&0x2)==0) 	Pixel( screen, x+4, y+i*4+3, Bcolor );
					if((p&0x1)==0) 	Pixel( screen, x+5, y+i*4+3, Bcolor );
				}
			}
			x += 6;
		}	
	}

}

/**********************************************
*                                             *
*     サブ画面に東雲12フォントを表示する      *
*        Fpalet:文字色                        *
*        Bpalet:背景色                        *
*        kaki  :or書きするか and書きするか    *
*               0=or , 1=and                  *
*                                             *
**********************************************/
void ShinoPrint_SUB( uint16* screen, int x, int y, u8 *st, u16 Fpalet, u16 Bpalet, u8 kaki )
{
short i, k, p;
short su, sd;

	while( 1 ){
		su = *st;
		st++;
		if( ((su>=0x81)&&(su<=0x9f)) || ((su>=0xe0)&&(su<=0xfc)) ){
			sd = *st;
			st++;
			if( su==0 || sd==0 )	break;
			k = Sjis2Elisa( su, sd );
			for( i=0; i<6; i++ ){
				p = shinonome_bin[k*18+i*3];
				if((p&0x1)!=0) 	Pixel_SUB( screen, x+7, y+i*2, Fpalet );
				if((p&0x2)!=0) 	Pixel_SUB( screen, x+6, y+i*2, Fpalet );
				if((p&0x4)!=0) 	Pixel_SUB( screen, x+5, y+i*2, Fpalet );
				if((p&0x8)!=0) 	Pixel_SUB( screen, x+4, y+i*2, Fpalet );
				if((p&0x10)!=0) 	Pixel_SUB( screen, x+3, y+i*2, Fpalet );
				if((p&0x20)!=0) 	Pixel_SUB( screen, x+2, y+i*2, Fpalet );
				if((p&0x40)!=0) 	Pixel_SUB( screen, x+1, y+i*2, Fpalet );
				if((p&0x80)!=0) 	Pixel_SUB( screen, x+0, y+i*2, Fpalet );
				if( kaki==1 ){
					if((p&0x1)==0) 	Pixel_SUB( screen, x+7, y+i*2, Bpalet );
					if((p&0x2)==0) 	Pixel_SUB( screen, x+6, y+i*2, Bpalet );
					if((p&0x4)==0) 	Pixel_SUB( screen, x+5, y+i*2, Bpalet );
					if((p&0x8)==0) 	Pixel_SUB( screen, x+4, y+i*2, Bpalet );
					if((p&0x10)==0) 	Pixel_SUB( screen, x+3, y+i*2, Bpalet );
					if((p&0x20)==0) 	Pixel_SUB( screen, x+2, y+i*2, Bpalet );
					if((p&0x40)==0) 	Pixel_SUB( screen, x+1, y+i*2, Bpalet );
					if((p&0x80)==0) 	Pixel_SUB( screen, x+0, y+i*2, Bpalet );
				}
				p = shinonome_bin[k*18+i*3+1];
				if((p&0x1)!=0) 	Pixel_SUB( screen, x+7, y+i*2+1, Fpalet );
				if((p&0x2)!=0) 	Pixel_SUB( screen, x+6, y+i*2+1, Fpalet );
				if((p&0x4)!=0) 	Pixel_SUB( screen, x+5, y+i*2+1, Fpalet );
				if((p&0x8)!=0) 	Pixel_SUB( screen, x+4, y+i*2+1, Fpalet );
				if((p&0x10)!=0) 	Pixel_SUB( screen, x+3, y+i*2+1, Fpalet );
				if((p&0x20)!=0) 	Pixel_SUB( screen, x+2, y+i*2+1, Fpalet );
				if((p&0x40)!=0) 	Pixel_SUB( screen, x+1, y+i*2+1, Fpalet );
				if((p&0x80)!=0) 	Pixel_SUB( screen, x+0, y+i*2+1, Fpalet );
				if( kaki==1 ){
					if((p&0x1)==0) 	Pixel_SUB( screen, x+7, y+i*2+1, Bpalet );
					if((p&0x2)==0) 	Pixel_SUB( screen, x+6, y+i*2+1, Bpalet );
					if((p&0x4)==0) 	Pixel_SUB( screen, x+5, y+i*2+1, Bpalet );
					if((p&0x8)==0) 	Pixel_SUB( screen, x+4, y+i*2+1, Bpalet );
					if((p&0x10)==0) 	Pixel_SUB( screen, x+3, y+i*2+1, Bpalet );
					if((p&0x20)==0) 	Pixel_SUB( screen, x+2, y+i*2+1, Bpalet );
					if((p&0x40)==0) 	Pixel_SUB( screen, x+1, y+i*2+1, Bpalet );
					if((p&0x80)==0) 	Pixel_SUB( screen, x+0, y+i*2+1, Bpalet );
				}
				p = shinonome_bin[k*18+i*3+2];
				if((p&0x1)!=0) 	Pixel_SUB( screen, x+11, y+i*2+1, Fpalet );
				if((p&0x2)!=0) 	Pixel_SUB( screen, x+10, y+i*2+1, Fpalet );
				if((p&0x4)!=0) 	Pixel_SUB( screen, x+9, y+i*2+1, Fpalet );
				if((p&0x8)!=0) 	Pixel_SUB( screen, x+8, y+i*2+1, Fpalet );
				if((p&0x10)!=0) 	Pixel_SUB( screen, x+11, y+i*2, Fpalet );
				if((p&0x20)!=0) 	Pixel_SUB( screen, x+10, y+i*2, Fpalet );
				if((p&0x40)!=0) 	Pixel_SUB( screen, x+9, y+i*2, Fpalet );
				if((p&0x80)!=0) 	Pixel_SUB( screen, x+8, y+i*2, Fpalet );
				if( kaki==1 ){
					if((p&0x1)==0) 	Pixel_SUB( screen, x+11, y+i*2+1, Bpalet );
					if((p&0x2)==0) 	Pixel_SUB( screen, x+10, y+i*2+1, Bpalet );
					if((p&0x4)==0) 	Pixel_SUB( screen, x+9, y+i*2+1, Bpalet );
					if((p&0x8)==0) 	Pixel_SUB( screen, x+8, y+i*2+1, Bpalet );
					if((p&0x10)==0) 	Pixel_SUB( screen, x+11, y+i*2, Bpalet );
					if((p&0x20)==0) 	Pixel_SUB( screen, x+10, y+i*2, Bpalet );
					if((p&0x40)==0) 	Pixel_SUB( screen, x+9, y+i*2, Bpalet );
					if((p&0x80)==0) 	Pixel_SUB( screen, x+8, y+i*2, Bpalet );
				}
			}
			x += 12;
		}
		else{	//半角文字だよ
			if( su==0 )	break;
//			k = su * 9;
			for( i=0; i<3; i++ ){
				p = shinonomeank_bin[su*9+i*3];
				if((p&0x80)!=0)	Pixel_SUB( screen, x+0, y+i*4, Fpalet );
				if((p&0x40)!=0) 	Pixel_SUB( screen, x+1, y+i*4, Fpalet );
				if((p&0x20)!=0) 	Pixel_SUB( screen, x+2, y+i*4, Fpalet );
				if((p&0x10)!=0) 	Pixel_SUB( screen, x+3, y+i*4, Fpalet );
				if((p&0x8)!=0) 	Pixel_SUB( screen, x+4, y+i*4, Fpalet );
				if((p&0x4)!=0) 	Pixel_SUB( screen, x+5, y+i*4, Fpalet );
				if((p&0x2)!=0) 	Pixel_SUB( screen, x+0, y+i*4+1, Fpalet );
				if((p&0x1)!=0) 	Pixel_SUB( screen, x+1, y+i*4+1, Fpalet );
				if( kaki==1 ){
					if((p&0x80)==0)	Pixel_SUB( screen, x+0, y+i*4, Bpalet );
					if((p&0x40)==0) 	Pixel_SUB( screen, x+1, y+i*4, Bpalet );
					if((p&0x20)==0) 	Pixel_SUB( screen, x+2, y+i*4, Bpalet );
					if((p&0x10)==0) 	Pixel_SUB( screen, x+3, y+i*4, Bpalet );
					if((p&0x8)==0) 	Pixel_SUB( screen, x+4, y+i*4, Bpalet );
					if((p&0x4)==0) 	Pixel_SUB( screen, x+5, y+i*4, Bpalet );
					if((p&0x2)==0) 	Pixel_SUB( screen, x+0, y+i*4+1, Bpalet );
					if((p&0x1)==0) 	Pixel_SUB( screen, x+1, y+i*4+1, Bpalet );
				}
				p = shinonomeank_bin[su*9+i*3+1];
				if((p&0x80)!=0)	Pixel_SUB( screen, x+2, y+i*4+1, Fpalet );
				if((p&0x40)!=0) 	Pixel_SUB( screen, x+3, y+i*4+1, Fpalet );
				if((p&0x20)!=0) 	Pixel_SUB( screen, x+4, y+i*4+1, Fpalet );
				if((p&0x10)!=0) 	Pixel_SUB( screen, x+5, y+i*4+1, Fpalet );
				if((p&0x8)!=0) 	Pixel_SUB( screen, x+0, y+i*4+2, Fpalet );
				if((p&0x4)!=0) 	Pixel_SUB( screen, x+1, y+i*4+2, Fpalet );
				if((p&0x2)!=0) 	Pixel_SUB( screen, x+2, y+i*4+2, Fpalet );
				if((p&0x1)!=0) 	Pixel_SUB( screen, x+3, y+i*4+2, Fpalet );
				if( kaki==1 ){
					if((p&0x80)==0)	Pixel_SUB( screen, x+2, y+i*4+1, Bpalet );
					if((p&0x40)==0) 	Pixel_SUB( screen, x+3, y+i*4+1, Bpalet );
					if((p&0x20)==0) 	Pixel_SUB( screen, x+4, y+i*4+1, Bpalet );
					if((p&0x10)==0) 	Pixel_SUB( screen, x+5, y+i*4+1, Bpalet );
					if((p&0x8)==0) 	Pixel_SUB( screen, x+0, y+i*4+2, Bpalet );
					if((p&0x4)==0) 	Pixel_SUB( screen, x+1, y+i*4+2, Bpalet );
					if((p&0x2)==0) 	Pixel_SUB( screen, x+2, y+i*4+2, Bpalet );
					if((p&0x1)==0) 	Pixel_SUB( screen, x+3, y+i*4+2, Bpalet );
				}
				p = shinonomeank_bin[su*9+i*3+2];
				if((p&0x80)!=0)	Pixel_SUB( screen, x+4, y+i*4+2, Fpalet );
				if((p&0x40)!=0) 	Pixel_SUB( screen, x+5, y+i*4+2, Fpalet );
				if((p&0x20)!=0) 	Pixel_SUB( screen, x+0, y+i*4+3, Fpalet );
				if((p&0x10)!=0) 	Pixel_SUB( screen, x+1, y+i*4+3, Fpalet );
				if((p&0x8)!=0) 	Pixel_SUB( screen, x+2, y+i*4+3, Fpalet );
				if((p&0x4)!=0) 	Pixel_SUB( screen, x+3, y+i*4+3, Fpalet );
				if((p&0x2)!=0) 	Pixel_SUB( screen, x+4, y+i*4+3, Fpalet );
				if((p&0x1)!=0) 	Pixel_SUB( screen, x+5, y+i*4+3, Fpalet );
				if( kaki==1 ){
					if((p&0x80)==0)	Pixel_SUB( screen, x+4, y+i*4+2, Bpalet );
					if((p&0x40)==0) 	Pixel_SUB( screen, x+5, y+i*4+2, Bpalet );
					if((p&0x20)==0) 	Pixel_SUB( screen, x+0, y+i*4+3, Bpalet );
					if((p&0x10)==0) 	Pixel_SUB( screen, x+1, y+i*4+3, Bpalet );
					if((p&0x8)==0) 	Pixel_SUB( screen, x+2, y+i*4+3, Bpalet );
					if((p&0x4)==0) 	Pixel_SUB( screen, x+3, y+i*4+3, Bpalet );
					if((p&0x2)==0) 	Pixel_SUB( screen, x+4, y+i*4+3, Bpalet );
					if((p&0x1)==0) 	Pixel_SUB( screen, x+5, y+i*4+3, Bpalet );
				}
			}
			x += 6;
		}	
	}

}
