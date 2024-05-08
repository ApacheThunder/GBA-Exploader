#ifdef __cplusplus
extern "C" {
#endif

//************************************************************************
//
//  日本語表示関連のプログラムのヘッダ   2006.07.25
//
//************************************************************************
// SJISコードから恵梨沙フォントデータを格納アドレスを計算する
short Sjis2Elisa( short u, short d );
// メイン画面に東雲12フォントを表示する
void ShinoPrint( uint16* screen, int x, int y, u8 *st, u16 Fcolor, u16 Bcolor, u8 kaki );
// サブ画面に東雲12フォントを表示する
void ShinoPrint_SUB( uint16* screen, int x, int y, u8 *st, u16 Fpalet, u16 Bpalet, u8 kaki );

#ifdef __cplusplus
}
#endif
