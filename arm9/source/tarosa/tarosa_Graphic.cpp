//************************************************************************
//
//　画像処理関係のプログラム   2007.05.04
//
//************************************************************************
#include <nds.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <stdlib.h>
#include <unistd.h>

#include "tarosa_Graphic.h"

//メインスクリーンとサブスクリーンの設定
uint16* MainScreen = VRAM_A;
uint16* SubScreen = (uint16*)BG_TILE_RAM_SUB(1);


/**********************************************
*                                             *
*         MainScreenに点を描画する            *
*                      yasuhoさんのソースより *
**********************************************/
void Pixel( u16* screen, s16 x, s16 y, u16 color )
{
	if(x < 0 || x > 256-1 || y < 0 || y > 192-1) return;
	screen[y*256+x] = color;
}

/**********************************************
*                                             *
*           SubScreenに点を描画する           *
*                                             *
**********************************************/
void Pixel_SUB(uint16* screen, s16 x, s16 y, uint16 palet )
{
uint16	bgsubAddress, h;

	if(x < 0 || x > 256-1 || y < 0 || y > 192-1) return;

	bgsubAddress = (x>>3)*32 + ((x&7)>>1) + (y>>3)*1024 + ((y&7)<<2);
	h = screen[bgsubAddress];

	if( (x&1)==0 ){
		h = palet | (h&0xff00);
	}
	else{
		h = (palet*0x100) | (h&0xff);
	}
	screen[bgsubAddress] = h;
}

/**********************************************
*                                             *
*  サブ画面をパレットコードで塗りつぶします   *
*                                             *
**********************************************/
void ClearBG_SUB(uint16* screen, uint16 palet)
{
int x=0,y=0;

	for( y=0; y<192; y++ ){
		for( x=0; x<256; x++ ){
			Pixel_SUB( screen, x, y, palet );
		}
	}
}

/**********************************************
*                                             *
*  メイン画面をカラーコードで塗りつぶします   *
*                      yasuhoさんのソースより *
**********************************************/
void ClearBG(uint16* screen,uint16 color){

int x=0,y=0;
	for( y=0; y<192; y++ ){
		for( x=0; x<256; x++ ){
			screen[(y)*256 + (x)] = color;
		}
	}
}

/**********************************************
*                                             *
*          メイン画面に線を引きます           *
*                     yasuhoさんのソースより  *
**********************************************/
void Line(u16* screen,int x0,int y0,int x1,int y1,int color)
{
int E,x,y;
int dx,dy,sx,sy,i;

	sx = ( x1 > x0 ) ? 1 : -1;
	dx = ( x1 > x0 ) ? x1 - x0 : x0 - x1;
	sy = ( y1 > y0 ) ? 1 : -1;
	dy = ( y1 > y0 ) ? y1 - y0 : y0 - y1;

	x = x0;
	y = y0;
	/* 傾きが1以下の場合 */
	if( dx >= dy ) {
		E = -dx;
		for( i = 0; i <= dx; i++ ) {
			Pixel(screen, x, y, color );
			x += sx;
			E += 2 * dy;
			if( E >= 0 ) {
				y += sy;
				E -= 2 * dx;
			}
		}
		/* 傾きが1より大きい場合 */
	}
	else {
		E = -dy;
		for( i = 0; i <= dy; i++ ) {
			Pixel(screen, x, y, color );
			y += sy;
			E += 2 * dx;
			if( E >= 0 ) {
				x += sx;
				E -= 2 * dy;
			}
		}
	}
}

/**********************************************
*                                             *
*          サブ画面に線を引きます             *
*                                             *
**********************************************/
void Line_Sub(u16* screen,int x0,int y0,int x1,int y1,u8 lpal)
{
int E,x,y;
int dx,dy,sx,sy,i;

	sx = ( x1 > x0 ) ? 1 : -1;
	dx = ( x1 > x0 ) ? x1 - x0 : x0 - x1;
	sy = ( y1 > y0 ) ? 1 : -1;
	dy = ( y1 > y0 ) ? y1 - y0 : y0 - y1;

	x = x0;
	y = y0;
	/* 傾きが1以下の場合 */
	if( dx >= dy ) {
		E = -dx;
		for( i = 0; i <= dx; i++ ) {
			Pixel_SUB(screen, x, y, lpal );
			x += sx;
			E += 2 * dy;
			if( E >= 0 ) {
				y += sy;
				E -= 2 * dx;
			}
		}
		/* 傾きが1より大きい場合 */
	}
	else {
		E = -dy;
		for( i = 0; i <= dy; i++ ) {
			Pixel_SUB(screen, x, y, lpal );
			y += sy;
			E += 2 * dx;
			if( E >= 0 ) {
				x += sx;
				E -= 2 * dy;
			}
		}
	}
}

/**********************************************
*                                             *
*      メインスクリーンの画像(256色)を        *
*      サブスクリーンにコピーします。         *
*                                             *
**********************************************/
void	ScreenCopy( uint16* mainscrn, uint16* subscrn )
{
uint16 i, x, y, k=0;
uint16 col;
uint16 paln=0;
uint16 paFlag;
	
	for( y=0; y<192; y++ ){
		for( x=0; x<256; x++ ){
			col = mainscrn[y*256+x];
			if( k==0 ){
				BG_PALETTE_SUB[0] = col;
				Pixel_SUB( subscrn, x, y, 0 );
				k++;
			}
			else{
				paFlag=0;
				if( k!=256 ){
					for( i=0; i<k; i++ ){
						if( BG_PALETTE_SUB[i]==col ){
							paln = i;
							paFlag = 1;
							break;
						}
					}
				}
				if( paFlag==0 ){
					BG_PALETTE_SUB[k] = col;
					Pixel_SUB( subscrn, x, y, k );
					k++;
					if( k>=256 )	k=256;
				}
				else{
					Pixel_SUB( subscrn, x, y, paln );
				}
			}
		}
	}
}

/**********************************************
*                                             *
*       サブスクリーンを                      *
*              8ドット上にスクロールします    *
*                                             *
*    backpalet:スクロールした後を塗りつぶす   *
*              パレットカラーの指定           *
*                                             *
**********************************************/
void	ScRl8Dot_SUB( uint16* screen, u8 backpal )
{
uint16* strtAdd;

	strtAdd = screen;
	strtAdd += 1024;
	dmaCopy( strtAdd, screen, 47104);
	for( int j=184; j<192; j++ ){
		for( int i=0; i<256; i++ ){
			Pixel_SUB( screen, i, j, backpal );
		}
	}
}

/**********************************************
*                                             *
*       サブスクリーンを                      *
*            nドット上下にスクロールします    *
*             正で上、負で下にスクロール      *
*    backpalet:スクロールした後を塗りつぶす   *
*              パレットカラーの指定           *
*                                             *
**********************************************/
void	ScRLnDot_SUB( uint16* screen, int n, u8 backpal )
{
int		x,y;

	if( n>0 ){
		for( y=0; y<192-n; y++ ){
			for( x=0; x<256; x++ ){
				SubScreen[ (x>>3)*32 + ((x&7)>>1) + (y>>3)*1024 + ((y&7)<<2) ]
					= SubScreen[(x>>3)*32 + ((x&7)>>1) + ((y+n)>>3)*1024 + (((y+n)&7)<<2)];
			}
		}
		for( y=192-n; y<192; y++ ){
			for( int x=0; x<256; x++ ){
				Pixel_SUB( SubScreen, x, y, backpal );
			}
		}
	}
	else{
		n = -n;
		for( y=192-n; y>=0; y-- ){
			for( x=0; x<256; x++ ){
				SubScreen[ (x>>3)*32 + ((x&7)>>1) + ((y+n)>>3)*1024 + (((y+n)&7)<<2) ]
					= SubScreen[(x>>3)*32 + ((x&7)>>1) + (y>>3)*1024 + ((y&7)<<2)];
			}
		}	
		for( y=0; y<n; y++ ){
			for( x=0; x<256; x++ ){
				Pixel_SUB( SubScreen, x, y, backpal );
			}
		}
	}
}

/**********************************************
*                                             *
*       メインスクリーンを                    *
*              8ドット上にスクロールします    *
*                                             *
*    backcolor:スクロールした後を塗りつぶす   *
*              バックカラーの指定             *
*                                             *
**********************************************/
void	ScRl8Dot( uint16* screen, uint16 backcolor )
{
uint16* strtAdd;

	strtAdd = screen;
	strtAdd += 2048;
	dmaCopy( strtAdd, screen, 47104*2);
	for( int j=184; j<192; j++ ){
		for( int i=0; i<256; i++ ){
			Pixel( screen, i, j, backcolor );
		}
	}
}

/**********************************************
*                                             *
*       メインスクリーンを                    *
*            nドット上下にスクロールします    *
*             正で上、負で下にスクロール      *
*    backcolor:スクロールした後を塗りつぶす   *
*              バックカラーの指定             *
*                                             *
**********************************************/
void	ScRLnDot( uint16* screen, int n, uint16 backcolor )
{
uint16* strtAdd;
int		x, y;

	strtAdd = MainScreen;
	if( n>0 ){
		strtAdd += 256*n;
		dmaCopy( strtAdd, MainScreen, 256*(192-n)*2);
		for( y=192-n; y<192; y++ ){
			for( x=0; x<256; x++ ){
				Pixel( MainScreen, x, y, backcolor );
			}
		}
	}
	else{
		n=-n;
		for( y=192-n; y>=0; y-- ){
			for( x=0; x<256; x++ ){
				MainScreen[(y+n)*256+x] = MainScreen[y*256+x];
			}
		}	
		for( y=0; y<n; y++ ){
			for( x=0; x<256; x++ ){
				Pixel( MainScreen, x, y, backcolor );
			}
		}	
	}
}

/**********************************************
*                                             *
*               画像データを                  *
*     メイン画面の任意の位置にロードする      *
*                                             *
**********************************************/
void	PicLoadMS( uint16 *screen, uint16 *sourceData, uint16 x, uint16 y, uint16 picwide, uint16 pichight )
{
uint16 i;
uint16 j;

	for( j=y; j<(y+pichight); j++ ){
		for( i=x; i<(x+picwide); i++ ){
			Pixel( screen, i, j, sourceData[i-x+(j-y)*picwide] | BIT(15) );
		}
	}
}

/**********************************************
*                                             *
*               画像データを                  *
*     メイン画面の任意の位置にロードする      *
*     透過色を指定できる                      *
* tcf: 0:透過無し、1:元の色とANDをとる　　　　*
*      2:元の色とORをとる                     *
*      3:元の色とXORをとる                    *
*      4:透過色tcは表示しない                 *
**********************************************/
void	PicLoadMSo( uint16 *screen, uint16 *sourceData, uint16 x, uint16 y, uint16 picwide, uint16 pichight
					, int tcf, uint16 tc )
{
uint16	i;
uint16	j;
uint16	col;

	switch( tcf ){
	case 1:
		for( j=y; j<(y+pichight); j++ ){
			for( i=x; i<(x+picwide); i++ ){
				Pixel( screen, i, j, (sourceData[i-x+(j-y)*picwide] | BIT(15))&Point( screen, i, j ) );
			}
		}
		break;
	case 2:
		for( j=y; j<(y+pichight); j++ ){
			for( i=x; i<(x+picwide); i++ ){
				Pixel( screen, i, j, (sourceData[i-x+(j-y)*picwide] | BIT(15))|Point( screen, i, j ) );
			}
		}
		break;
	case 3:
		for( j=y; j<(y+pichight); j++ ){
			for( i=x; i<(x+picwide); i++ ){
				Pixel( screen, i, j, (sourceData[i-x+(j-y)*picwide] | BIT(15))^Point( screen, i, j ) );
			}
		}
		break;
	case 4:
		for( j=y; j<(y+pichight); j++ ){
			for( i=x; i<(x+picwide); i++ ){
				col = sourceData[i-x+(j-y)*picwide];
				if( tc!=col ){
					Pixel( screen, i, j, col | BIT(15) );
				}
			}
		}
		break;
	case 0:
	default:
		PicLoadMS( screen, sourceData, x, y, picwide, pichight );
		break;
	}
}


/**********************************************
*                                             *
*               画像データを                  *
*      サブ画面の任意の位置にロードする       *
*       (パレットは現状のものを使う)          *
**********************************************/
void	PicLoadSS( uint16 *screen, uint16 *sourceData, uint16 subx, uint16 suby,
							uint16 soux, uint16 souy, uint16 picwide, uint16 pichight )
{
uint16 i;
uint16 j;

	for( j=suby; j<(suby+pichight); j++ ){
		for( i=subx; i<(subx+picwide); i++ ){
			Pixel_SUB( screen, i, j, sourceData[(i-subx+soux)+(j-suby+souy)*picwide] );
		}
	}
}

/**********************************************
*                                             *
*               画像データを                  *
*      サブ画面の任意の位置にロードする       *
*       (パレットは現状のものを使う)          *
*     透過色を指定できる                      *
* tcf: 0:透過無し、1:元の色とANDをとる　　　　*
*      2:元の色とORをとる                     *
*      3:元の色とXORをとる                    *
*      4:透過色cpaletは表示しない             *
**********************************************/
void	PicLoadSSo( uint16 *screen, uint16 *sourceData, uint16 subx, uint16 suby, uint16 picwide, uint16 pichight,
					int tcf, uint16 cpalet )
{
uint16	i;
uint16	j;
uint16	col;

	switch( tcf ){
	case 1:
		for( j=suby; j<(suby+pichight); j++ ){
			for( i=subx; i<(subx+picwide); i++ ){
				Pixel_SUB( screen, i, j, sourceData[(i-subx)+(j-suby)*picwide]&Point_SUB( screen, i, j ) );
			}
		}
		break;
	case 2:
		for( j=suby; j<(suby+pichight); j++ ){
			for( i=subx; i<(subx+picwide); i++ ){
				Pixel_SUB( screen, i, j, sourceData[(i-subx)+(j-suby)*picwide]|Point_SUB( screen, i, j ) );
			}
		}
		break;
	case 3:
		for( j=suby; j<(suby+pichight); j++ ){
			for( i=subx; i<(subx+picwide); i++ ){
				Pixel_SUB( screen, i, j, sourceData[(i-subx)+(j-suby)*picwide]^Point_SUB( screen, i, j ) );
			}
		}
		break;
	case 4:
		for( j=suby; j<(suby+pichight); j++ ){
			for( i=subx; i<(subx+picwide); i++ ){
				col = sourceData[(i-subx)+(j-suby)*picwide];
				if( cpalet!=col ){
					Pixel_SUB( screen, i, j, col );
				}
			}
		}
		break;
	case 0:
	default:
		PicLoadSS( screen, sourceData, subx, suby, 0, 0, picwide, pichight );
		break;
	}
}


/**********************************************
*                                             *
*     メインスクリーンにボックスを書きます    *
*              full:0 枠のみ                  *
*                   1 塗りつぶす              *
*                     yasuhoさんのソースより  *
**********************************************/
void DrawBox(u16* screen, int x0, int y0, int x1, int y1,  uint16 color, int full)
{
	if( full==0 ){
		Line( screen, x0, y0, x1, y0, color );
		Line( screen, x0, y1, x1, y1, color );
		Line( screen, x0, y0, x0, y1, color );
		Line( screen, x1, y0, x1, y1, color );
	}
	else{
		if( y0>y1 ){
			for( int i=y1; i<=y0; i++ ){
				Line( screen, x0, i, x1, i, color );
			}
		}
		else{
			for( int i=y0; i<=y1; i++ ){
				Line( screen, x0, i, x1, i, color );
			}
		}
	}
}

/**********************************************
*                                             *
*     サブスクリーンにボックスを書きます      *
*              full:0 枠のみ                  *
*                   1 塗りつぶす              *
*                     2007.5.4                *
**********************************************/
void DrawBox_SUB( u16* screen, int x0, int y0, int x1, int y1, uint16 bpalet, int full)
{
	if( full==0 ){
		Line_Sub( screen, x0, y0, x1, y0, bpalet );
		Line_Sub( screen, x0, y1, x1, y1, bpalet );
		Line_Sub( screen, x0, y0, x0, y1, bpalet );
		Line_Sub( screen, x1, y0, x1, y1, bpalet );
	}
	else{
		if( y0>y1 ){
			for( int i=y1; i<=y0; i++ ){
				Line_Sub( screen, x0, i, x1, i, bpalet );
			}
		}
		else{
			for( int i=y0; i<=y1; i++ ){
				Line_Sub( screen, x0, i, x1, i, bpalet );
			}
		}
	}
}

/**********************************************
*                                             *
*  サブ画面の一番明るいパレットコードを検索   *
*      return: パレットコード                 *
*                                             *
**********************************************/
uint16	maxPaletCode( uint16* subpalet )
{
int i;
int	max=0;
int r, g, b;
int	goukei;
int	maxpalet=0;

	for( i=0; i<256; i++ ){
		r = (subpalet[i]>>10)&0x1f;
		g = (subpalet[i]>>5)&0x1f;
		b = subpalet[i]&0x1f;
		goukei = r + g + b;
		if( max<goukei ){
			max = goukei;
			maxpalet = i;
		}
	}
	return( maxpalet );
}

/**********************************************
*                                             *
*   サブ画面の一番暗いパレットコードを検索    *
*      return: パレットコード                 *
*                                             *
**********************************************/
uint16	minPaletCode( uint16 *subpalet )
{
int i;
int	min;
int r, g, b;
int	goukei;
int	minpalet=0;

	r = (subpalet[0]>>10)&0x1f;
	g = (subpalet[0]>>5)&0x1f;
	b = subpalet[0]&0x1f;
	min = r + g + b;
	for( i=1; i<256; i++ ){
		r = (subpalet[i]>>10)&0x1f;
		g = (subpalet[i]>>5)&0x1f;
		b = subpalet[i]&0x1f;
		goukei = r + g + b;
		if( min>goukei ){
			min = goukei;
			minpalet = i;
		}
	}
	return( minpalet );
}

/**********************************************
*                                             *
*   サブ画面の(x0,y0)-(x1,y1)範囲の           *
*    パレット(bpal)をcpalパレット番号に       *
*    入れ替えます                             *
*                                             *
**********************************************/
void	ColorSwap_SUB( u16* screen, s16 x0, s16 y0, s16 x1, s16 y1, uint16 bpalet, uint16 cpalet )
{
s16		x, y;

	for( y=y0+1; y<y1; y++ ){
		for( x=x0+1; x<x1; x++ ){
			if( Point_SUB( screen, x, y )==bpalet )	Pixel_SUB( screen, x, y, cpalet );
		}
	}
}

/**********************************************
*                                             *
*     サブ画面上の点のパレット番号を得る      *
*                                             *
**********************************************/
uint16	Point_SUB( u16* screen, s16 x, s16 y )
{
uint16	bgsubAddress, h;

	if(x < 0 || x > 256-1 || y < 0 || y > 192-1) return( 0 );

	bgsubAddress = (x>>3)*32 + ((x&7)>>1) + (y>>3)*1024 + ((y&7)<<2);
	h = screen[bgsubAddress];

	if( (x&1)==0 ){
		return( h&0xff );
	}
	else{
		return( h>>8 );
	}
}

// 2006.09.09追加分
/**********************************************
*                                             *
*           メイン画面上の色を得る            *
*                                             *
**********************************************/
int	Point( u16* screen, s16 x, s16 y )
{
	if(x < 0 || x > 256-1 || y < 0 || y > 192-1) return( 0 );

	return( (int)screen[y*256+x] );
}

// 2006.09.18追加分
/**********************************************
*                                             *
*   メイン画面の(x0,y0)-(x1,y1)範囲の         *
*    色(bcolor)をccolor色に入れ替えます       *
*                                             *
**********************************************/
void	ColorSwap( u16* screen, s16 x0, s16 y0, s16 x1, s16 y1, uint16 bcolor, uint16 ccolor )
{
s16		x, y;

	for( y=y0+1; y<y1; y++ ){
		for( x=x0+1; x<x1; x++ ){
			if( Point( screen, x, y )==bcolor )	Pixel( screen, x, y, ccolor );
		}
	}
}
