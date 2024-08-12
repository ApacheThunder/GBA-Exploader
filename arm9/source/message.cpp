#include <stdio.h>
#include <NDS.h>

#include "message.h"

char *errmsg[16];
char *cnfmsg[11];
char *cnfmsg2[3];
char *barmsg[6];
char *cmd_m[4];
char *t_msg[22];
char *savmsg[6];

static const char *errmsg_j[16] = {
	"FATの初期化に失敗しました",				// 0
	"適切なDLDIパッチを行ってください",			// 1
	"Slot2拡張パックがセットされていません",		// 2
	"最初からやり直してください",				// 3
	"SRAMのSAVEﾃﾞｰﾀが、消失していました",		// 4
	"SAVﾌｧｲﾙの処理は行えません",				// 5
	"32MB以上のファイルは扱えません",			// 6
	"指定したファイルは大きすぎます",			// 7
	"16MB以上のファイルは扱えません",			// 8
	"Flash 1MbのSAVEタイプは、",			// 9
	"[EXP128K]で取り扱う事ができません",		// 10
	"SAVﾌｧｲﾙがありません！",				// 11
	"SAVﾌｧｲﾙを削除していませか？",			// 12
	"(A):確認",							// 13
	"DSi/3DSには対応しておりません！",			// 14
	"DS/DS Liteのみに対応しています。"			// 15
};

static const char *errmsg_e[16] = {
	"FAT initialization failed ",				// 0
	"Please apply the appropriate DLDI Patch.",	// 1
	"Slot2 expansion pack not found ",			// 2
	"Please redo from start.",					// 3
	"SRAM save data not found ",				// 4
	"Can't  process to SAV file.",				// 5
	"Can't load a file above 32MB",				// 6
	"Selected file too big.",					// 7
	"Can't load a file above 16MB",				// 8
	"The SAVE type of Flash 1Mb",				// 9
	"Can't be treated with [EXP128K].",			// 10
	"SAV file not found!",						// 11
	" delete the SAV file?",					// 12
	"(A):Confirm",								// 13
	"DSi/3DS Consoles not supported!", 			// 14
	"Only compatible with DS/DS Lite." 			// 15
};


static const char *cnfmsg_j[11] = {
	"(A):実行, (B):取消",				// 0
	"現在のSRAMにあるSAVEデータを",			// 1
	"SAVファイルに保存します",			// 2
	"SAVファイルのデータを",			// 3
	"SRAMのSAVEにロードします",			// 4
	"全SRAM領域をバックアップファイル",		// 5
	"(SRAM.BIN)から復旧します",			// 6
	"全SRAM領域をバックアップとして",		// 7
	"SRAM.BINに退避します",				// 8
	"このSlot2拡張パックを GBA ExpLoader用に",	// 9
	"設定(SRAMは失われます)していいですか？"	// 10
};



static const char *cnfmsg2_j[3] = {
	"(A):はい, (B):いいえ",		// 0
	"EZFlash Omega を検出しました。",	// 1
	"これは決定版ですか？"			// 2
};

static const char *cnfmsg2_e[3] = {
	"(A):Yes, (B):No",				// 0
	"Detected EZFlash Omega.",		// 1
	"Is this Definitive Edition?"	// 2
};

static const char *cnfmsg_e[11] = {
	"(A):Run, (B):Cancel",				// 0
	"Write save data in SRAM",			// 1
	" to SAV file",					// 2
	"Load save data in SRAM",			// 3
	" from SAV file",				// 4
	"Restore all data in SRAM",			// 5
	" from file SRAM.BIN",				// 6
	"Backup all data in SRAM",			// 7
	" to file SRAM.BIN",				// 8
	"May I set this Slot2 expansion pack",		// 9
	"for GBA ExpLoader?"				// 10
};


static const char *barmsg_j[6] = {
	"  NORを消去中...  ",				// 0
	" NORにコピー中... ",				// 1
	" RAMにロード中... ",				// 2
	"  ROMを解析中...  ",				// 3
	"  SRAMの読み取り  ",				// 4
	"  SRAMへの書き込み  "				// 5
};

static const char *barmsg_e[6] = {
	"  Erasing NOR...   ",				// 0
	" Copying to NOR... ",				// 1
	" Loading to RAM... ",				// 2
	" Analyzing ROM...  ",				// 3
	" Reading SRAM...  ",				// 4
	" Writing SRAM...  "				// 5
};


static const char *cmd_m_j[4] = {
	"  振動レベル ： (小)  ",
	"  振動レベル ： (中)  ",
	"  振動レベル ： (大)  ",
	" ブラウザ用拡張メモリ "
};

static const char *cmd_m_e[4] = {
	" Rumble level: Weak   ",
	" Rumble level: Medium ",
	" Rumble level: Strong ",
	" Expansion RAM        "
};


static const char *t_msg_j[22] = {
	"選択中のゲーム",
	" PSRAMモード",
	"(A):RAMにゲームをロードして実行    ",
	"(B):SRAMのSAVEﾃﾞｰﾀを SAVﾌｧｲﾙに保存 ",
	"  全SRAMﾃﾞｰﾀ領域 (X):退避, (Y):復旧",
	"(R):モード変更                    ",
	"(L)/(R):モード変更                ",
	"(L):モード変更                    ",
	"  NORモード ",
	"(A):ゲームのコピー  (X):NORを実行  ",
	"(B):SRAMのSAVEﾃﾞｰﾀを SAVﾌｧｲﾙに保存 ",		// 10
	"(Y):SAVﾌｧｲﾙを SRAMのSAVEにロード   ",
	"[%s] %d ゲーム",
	"消失もしくは新規状態",
	"現在のSRAMのSAVE",
	" == GBAファイルがありません == ",
	"初期化中....",
	"  拡張モード",
	"(A):3in1を設定してリセット",
	"(L):モード変更                   ",
	"(R):ブラウザ用拡張メモリ         ",
	" SDRAMモード",
};

static const char *t_msg_e[22] = {
	"Selected game",
	"  PSRAM Mode ",
	"(A):Run (B):Write SRAM to SAV file",
	"(X):Backup whole SRAM to SRAM.BIN ",
	"(Y):Restore SRAM.BIN to SRAM      ",
	"(R):Change mode                 ",
	"(L)/(R):Change mode             ",
	"(L):Change mode                 ",
	"   NOR mode  ",
	"(A):Copy game (X):Run game in NOR ",
	"(B):Write SRAM to SAV file        ",		// 10
	"(Y):Load SAV file to SRAM         ",
	"[%s] %d game",
	"Empty or new state",
	"Current SRAM Save",
	" == GBA file not found == ",
	"Initialize....",
	"Expansion mode",
	"(A):Set mode and soft reset",
	"(L):Change mode                ",
	"(R):Expansion RAM              ",
	"  SDRAM Mode ",
};


static const char *savmsg_j[6] = {
	" SRAMにSAVEデータをロード",		// 0
	"(A):選択したファイルをロード",		// 1
	"(B):ロードしない(新規ゲーム)",		// 2
	" SRAMのSAVEデータを保存  ",		// 3
	"(A):選択したファイルに保存",		// 4
	"(B):保存しない(取消)",			// 5
};

static const char *savmsg_e[6] = {
	" Load SRAM from SAV file ",		// 0
	"(A):Load from selected file",		// 1
	"(B):No load(New Game)",		// 2
	" Write SRAM to SAV file  ",		// 3
	"(A):write to selected file",		// 4
	"(B):No write(Cancel)",			// 5
};


void setLangMsg() {
	u32	UserLang = 0;
	int	i;

	UserLang = PersonalData->language;

	if(UserLang != 0) {
		for(i = 0; i < 16; i++)errmsg[i] = (char*)errmsg_e[i];
		for(i = 0; i < 11; i++)cnfmsg[i] = (char*)cnfmsg_e[i];
		for(i = 0; i < 3; i++)cnfmsg2[i] = (char*)cnfmsg2_e[i];
		for(i = 0; i < 6; i++)barmsg[i] = (char*)barmsg_e[i];
		for(i = 0; i < 4; i++)cmd_m[i] = (char*)cmd_m_e[i];
		for(i = 0; i < 22; i++)t_msg[i] = (char*)t_msg_e[i];
		for(i = 0; i < 6; i++)savmsg[i] = (char*)savmsg_e[i];
		return;
	}

	for(i = 0; i < 16; i++)errmsg[i] = (char*)errmsg_j[i];
	for(i = 0; i < 11; i++)cnfmsg[i] = (char*)cnfmsg_j[i];
	for(i = 0; i < 3; i++)cnfmsg2[i] = (char*)cnfmsg2_j[i];
	for(i = 0; i < 6; i++)barmsg[i] = (char*)barmsg_j[i];
	for(i = 0; i < 4; i++)cmd_m[i] = (char*)cmd_m_j[i];
	for(i = 0; i < 22; i++)t_msg[i] = (char*)t_msg_j[i];
	for(i = 0; i < 6; i++)savmsg[i] = (char*)savmsg_j[i];
}

static bool _isKanji1(u8 ch) {
	if((ch >= 0x81) && (ch <= 0x9F))return true;
	if((ch >= 0xE0) && (ch <= 0xEF))return true;
	if((ch >= 0xFA) && (ch <= 0xFB))return true; // JIS X 0218他 IBM拡張文字 (0xFA40-0xFC4B)
	return false;
}

char *jstrncpy(char *s1, char *s2, size_t n) {
	bool kan = false;

	char *p = s1;

	while(n) {
		n--;
		kan = _isKanji1((u8)*s2);
		if(!(*s1++ = *s2++))break;
	}

	if(kan)*(s1 - 1) = '\0';

	while(n--)*s1++ = '\0';

	*s1 = '\0';

	return(p);
}

