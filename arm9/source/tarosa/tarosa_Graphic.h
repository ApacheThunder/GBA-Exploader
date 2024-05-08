#ifdef __cplusplus
extern "C" {
#endif
//************************************************************************
//
//　画像処理関係のプログラムのヘッダ   2007.05.03
//
//************************************************************************

//MainScreenに点を描画する
void Pixel( u16* screen, s16 x, s16 y, u16 color );

//SubScreenに点を描画する
void Pixel_SUB(uint16* screen, s16 x, s16 y, uint16 palet );

//サブ画面をパレットコードで塗りつぶします
void ClearBG_SUB(uint16* screen, uint16 palet);

//メイン画面をカラーコードで塗りつぶします
void ClearBG(uint16* screen,uint16 color);

//メイン画面に線を引きます
void Line(u16* screen,int x0,int y0,int x1,int y1,int color);

//サブ画面に線を引きます
void Line_Sub(u16* screen,int x0,int y0,int x1,int y1,u8 lpal);

//メインスクリーンの画像(256色)を
// サブスクリーンにコピーします。
void	ScreenCopy( uint16* mainscrn, uint16* subscrn );

//サブスクリーンを8ドット上にスクロールします
void	ScRl8Dot_SUB( uint16* screen, u8 backpal );

//サブスクリーンをnドット上下にスクロールします
void	ScRLnDot_SUB( uint16* screen, int n, u8 backpal );

//メインスクリーンを8ドット上にスクロールします
void	ScRl8Dot( uint16* screen, uint16 backcolor );

//メインスクリーンをnドット上下にスクロールします
void	ScRLnDot( uint16* screen, int n, uint16 backcolor );

//画像データをメイン画面の任意の位置にロードする
void	PicLoadMS( uint16 *screen, uint16 *sourceData, uint16 x, uint16 y, uint16 picwide, uint16 pichight );

//画像データをサブ画面の任意の位置にロードする
void	PicLoadSS( uint16 *screen, uint16 *sourceData, uint16 subx, uint16 suby, uint16 soux, uint16 souy, uint16 picwide, uint16 pichight );

//メインスクリーンにボックスを書きます
void DrawBox(u16* screen, int x0, int y0, int x1, int y1,  uint16 color, int full);

//サブスクリーンにボックスを書きます
void DrawBox_SUB(u16* screen, int x0, int y0, int x1, int y1, uint16 bpalet, int full);

//サブ画面の一番明るいパレットコードを検索
uint16	maxPaletCode( uint16* subpalet );

//サブ画面の一番暗いパレットコードを検索
uint16	minPaletCode( uint16 *subpalet );

//サブ画面の(x0,y0)-(x1,y1)範囲のパレット(bpal)をcpalパレット番号に
void	ColorSwap_SUB( u16* screen, s16 x0, s16 y0, s16 x1, s16 y1, uint16 bpalet, uint16 cpalet );

//サブ画面上の点のパレット番号を得る
uint16	Point_SUB( u16* screen, s16 x, s16 y );

// 2006.09.09追加分
//メイン画面上の色を得る
int	Point( u16* screen, s16 x, s16 y );

// 2006.09.18追加分
//メイン画面の(x0,y0)-(x1,y1)範囲の色bcolorをccolor色に
void	ColorSwap( u16* screen, s16 x0, s16 y0, s16 x1, s16 y1, uint16 bcolor, uint16 ccolor );

//2007.05.03追加
//メイン画面の任意の位置にロードする。透過色などを指定できる
void	PicLoadMSo( uint16 *screen, uint16 *sourceData, uint16 x, uint16 y, uint16 picwide, uint16 pichight, int tcf, uint16 tc );
//サブ画面の任意の位置にロードする。透過色などを指定できる
void	PicLoadSSo( uint16 *screen, uint16 *sourceData, uint16 subx, uint16 suby, uint16 picwide, uint16 pichight,	int tcf, uint16 cpalet );

#ifdef __cplusplus
}
#endif
