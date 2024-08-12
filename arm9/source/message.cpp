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
	"FAT�̏������Ɏ��s���܂���",				// 0
	"�K�؂�DLDI�p�b�`���s���Ă�������",			// 1
	"Slot2�g���p�b�N���Z�b�g����Ă��܂���",		// 2
	"�ŏ������蒼���Ă�������",				// 3
	"SRAM��SAVE�ް����A�������Ă��܂���",		// 4
	"SAV̧�ق̏����͍s���܂���",				// 5
	"32MB�ȏ�̃t�@�C���͈����܂���",			// 6
	"�w�肵���t�@�C���͑傫�����܂�",			// 7
	"16MB�ȏ�̃t�@�C���͈����܂���",			// 8
	"Flash 1Mb��SAVE�^�C�v�́A",			// 9
	"[EXP128K]�Ŏ�舵�������ł��܂���",		// 10
	"SAV̧�ق�����܂���I",				// 11
	"SAV̧�ق��폜���Ă��܂����H",			// 12
	"(A):�m�F",							// 13
	"DSi/3DS�ɂ͑Ή����Ă���܂���I",			// 14
	"DS/DS Lite�݂̂ɑΉ����Ă��܂��B"			// 15
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
	"(A):���s, (B):���",				// 0
	"���݂�SRAM�ɂ���SAVE�f�[�^��",			// 1
	"SAV�t�@�C���ɕۑ����܂�",			// 2
	"SAV�t�@�C���̃f�[�^��",			// 3
	"SRAM��SAVE�Ƀ��[�h���܂�",			// 4
	"�SSRAM�̈���o�b�N�A�b�v�t�@�C��",		// 5
	"(SRAM.BIN)���畜�����܂�",			// 6
	"�SSRAM�̈���o�b�N�A�b�v�Ƃ���",		// 7
	"SRAM.BIN�ɑޔ����܂�",				// 8
	"����Slot2�g���p�b�N�� GBA ExpLoader�p��",	// 9
	"�ݒ�(SRAM�͎����܂�)���Ă����ł����H"	// 10
};



static const char *cnfmsg2_j[3] = {
	"(A):�͂�, (B):������",		// 0
	"EZFlash Omega �����o���܂����B",	// 1
	"����͌���łł����H"			// 2
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
	"  NOR��������...  ",				// 0
	" NOR�ɃR�s�[��... ",				// 1
	" RAM�Ƀ��[�h��... ",				// 2
	"  ROM����͒�...  ",				// 3
	"  SRAM�̓ǂݎ��  ",				// 4
	"  SRAM�ւ̏�������  "				// 5
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
	"  �U�����x�� �F (��)  ",
	"  �U�����x�� �F (��)  ",
	"  �U�����x�� �F (��)  ",
	" �u���E�U�p�g�������� "
};

static const char *cmd_m_e[4] = {
	" Rumble level: Weak   ",
	" Rumble level: Medium ",
	" Rumble level: Strong ",
	" Expansion RAM        "
};


static const char *t_msg_j[22] = {
	"�I�𒆂̃Q�[��",
	" PSRAM���[�h",
	"(A):RAM�ɃQ�[�������[�h���Ď��s    ",
	"(B):SRAM��SAVE�ް��� SAV̧�قɕۑ� ",
	"  �SSRAM�ް��̈� (X):�ޔ�, (Y):����",
	"(R):���[�h�ύX                    ",
	"(L)/(R):���[�h�ύX                ",
	"(L):���[�h�ύX                    ",
	"  NOR���[�h ",
	"(A):�Q�[���̃R�s�[  (X):NOR�����s  ",
	"(B):SRAM��SAVE�ް��� SAV̧�قɕۑ� ",		// 10
	"(Y):SAV̧�ق� SRAM��SAVE�Ƀ��[�h   ",
	"[%s] %d �Q�[��",
	"�����������͐V�K���",
	"���݂�SRAM��SAVE",
	" == GBA�t�@�C��������܂��� == ",
	"��������....",
	"  �g�����[�h",
	"(A):3in1��ݒ肵�ă��Z�b�g",
	"(L):���[�h�ύX                   ",
	"(R):�u���E�U�p�g��������         ",
	" SDRAM���[�h",
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
	" SRAM��SAVE�f�[�^�����[�h",		// 0
	"(A):�I�������t�@�C�������[�h",		// 1
	"(B):���[�h���Ȃ�(�V�K�Q�[��)",		// 2
	" SRAM��SAVE�f�[�^��ۑ�  ",		// 3
	"(A):�I�������t�@�C���ɕۑ�",		// 4
	"(B):�ۑ����Ȃ�(���)",			// 5
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
	if((ch >= 0xFA) && (ch <= 0xFB))return true; // JIS X 0218�� IBM�g������ (0xFA40-0xFC4B)
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

