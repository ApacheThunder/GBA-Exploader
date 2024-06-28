#include <nds.h>

#include "io_sc_common.h"

u32 prefetchPatch[8] = {
	0xE59F000C,	// LDR  R0, =0x4000204
	0xE59F100C, // LDR  R1, =0x4000
	0xE4A01000, // STRT R1, [R0]
	0xE59F0008, // LDR  R0, =0x80000C0 (this changes, depending on the ROM)
	0xE1A0F000, // MOV  PC, R0
	0x04000204,
	0x00004000,
	0x080000C0
};

static const u8 sDbzLoGUPatch1[0x24] = {
	0x0A, 0x1C, 0x40, 0x0B, 0xE0, 0x21, 0x09, 0x05, 0x41, 0x18, 0x07, 0x31, 0x00, 0x23, 0x08, 0x78,
	0x10, 0x70, 0x01, 0x33, 0x01, 0x32, 0x01, 0x39, 0x07, 0x2B, 0xF8, 0xD9, 0x00, 0x20, 0x70, 0xBC,
	0x02, 0xBC, 0x08, 0x47
};

static const u8 sDbzLoGUPatch2[0x28] = {
	0x70, 0xB5, 0x00, 0x04, 0x0A, 0x1C, 0x40, 0x0B, 0xE0, 0x21, 0x09, 0x05, 0x41, 0x18, 0x07, 0x31,
	0x00, 0x23, 0x10, 0x78, 0x08, 0x70, 0x01, 0x33, 0x01, 0x32, 0x01, 0x39, 0x07, 0x2B, 0xF8, 0xD9,
	0x00, 0x20, 0x70, 0xBC, 0x02, 0xBC, 0x08, 0x47
};

static const u8 wwTwistedPatch[0xF0] = {
	0x1F, 0x24, 0x1F, 0xB4, 0x33, 0x48, 0x01, 0x21, 0x01, 0x60, 0x33, 0x48, 0x01, 0x21, 0x01, 0x60,
	0x32, 0x49, 0x0A, 0x68, 0x10, 0x23, 0x1A, 0x40, 0x1E, 0xD1, 0x30, 0x49, 0x0A, 0x68, 0x02, 0x23,
	0x1A, 0x40, 0x0D, 0xD0, 0x2E, 0x48, 0x01, 0x68, 0x01, 0x22, 0x91, 0x42, 0x02, 0xDB, 0x09, 0x19,
	0x01, 0x60, 0x38, 0xE0, 0x2A, 0x48, 0x01, 0x22, 0x02, 0x60, 0x12, 0x19, 0x02, 0x60, 0x32, 0xE0,
	0x27, 0x48, 0x01, 0x68, 0x01, 0x22, 0x91, 0x42, 0x00, 0xDB, 0x01, 0xE0, 0x02, 0x60, 0x11, 0x1C,
	0x24, 0x4B, 0xC9, 0x18, 0x01, 0x60, 0x26, 0xE0, 0x20, 0x49, 0x0A, 0x68, 0x20, 0x23, 0x1A, 0x40,
	0x1E, 0xD1, 0x1E, 0x49, 0x0A, 0x68, 0x02, 0x23, 0x1A, 0x40, 0x0D, 0xD0, 0x1C, 0x48, 0x01, 0x68,
	0x1D, 0x4A, 0x91, 0x42, 0x02, 0xDC, 0x09, 0x1B, 0x01, 0x60, 0x14, 0xE0, 0x18, 0x48, 0x1A, 0x4A,
	0x02, 0x60, 0x12, 0x1B, 0x02, 0x60, 0x0E, 0xE0, 0x15, 0x48, 0x01, 0x68, 0x16, 0x4A, 0x91, 0x42,
	0x00, 0xDC, 0x01, 0xE0, 0x02, 0x60, 0x11, 0x1C, 0x12, 0x4B, 0xC9, 0x1A, 0x01, 0x60, 0x02, 0xE0,
	0x0F, 0x48, 0x01, 0x21, 0x01, 0x60, 0x1F, 0xBC, 0x0C, 0x48, 0x00, 0x88, 0x0F, 0x4A, 0x10, 0x47,
	0x00, 0x7F, 0x00, 0x03, 0xA0, 0x7F, 0x00, 0x03, 0x30, 0x01, 0x00, 0x04, 0x4B, 0x13, 0x00, 0x08,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x20, 0x10, 0x00, 0x03, 0x98, 0x0F, 0x00, 0x03, 0x30, 0x01, 0x00, 0x04,
	0x30, 0x10, 0x00, 0x03, 0x20, 0x01, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x4B, 0x13, 0x00, 0x08
};

static const u8 yoshiTopsyTurvyPatch[0x18C] = {
	0x0C, 0x20, 0x9F, 0xE5, 0x80, 0x30, 0xA0, 0xE3, 0x00, 0x30, 0xE2, 0xE4, 0x04, 0x30, 0x9F, 0xE5,
	0x13, 0xFF, 0x2F, 0xE1, 0xE0, 0x7F, 0x00, 0x03, 0x69, 0x51, 0x02, 0x08, 0x00, 0x00, 0x00, 0x00,
	0xFF, 0xB5, 0x01, 0x4F, 0x00, 0x00, 0x09, 0xE0, 0xE0, 0x7F, 0x00, 0x03, 0x02, 0x49, 0x09, 0x88,
	0x01, 0x23, 0x08, 0x40, 0x70, 0x47, 0x00, 0x00, 0x30, 0x01, 0x00, 0x04, 0x3D, 0x78, 0x7F, 0x1C,
	0x80, 0x26, 0x35, 0x42, 0x24, 0xD0, 0x33, 0x48, 0x00, 0x21, 0x00, 0x88, 0x88, 0x42, 0x04, 0xD1,
	0x31, 0x49, 0x20, 0x20, 0x00, 0x02, 0x02, 0x30, 0x08, 0x80, 0x30, 0x48, 0x03, 0x21, 0x00, 0x88,
	0x88, 0x42, 0x04, 0xD1, 0x2E, 0x49, 0x20, 0x20, 0x00, 0x02, 0x02, 0x30, 0x08, 0x80, 0x02, 0x20,
	0x00, 0x02, 0x00, 0x30, 0xFF, 0xF7, 0xDA, 0xFF, 0x02, 0xD1, 0x2A, 0x49, 0x00, 0x20, 0x08, 0x80,
	0x01, 0x20, 0xFF, 0x30, 0xFF, 0xF7, 0xD2, 0xFF, 0x02, 0xD1, 0x27, 0x49, 0x03, 0x20, 0x08, 0x80,
	0x76, 0x08, 0x35, 0x42, 0x02, 0xD0, 0x25, 0x49, 0x63, 0x20, 0x08, 0x70, 0x76, 0x08, 0x35, 0x42,
	0x04, 0xD0, 0x23, 0x49, 0x27, 0x20, 0x00, 0x02, 0x0F, 0x30, 0x08, 0x80, 0x76, 0x08, 0x35, 0x42,
	0x02, 0xD0, 0x20, 0x49, 0x03, 0x20, 0x08, 0x80, 0x76, 0x08, 0x35, 0x42, 0x22, 0xD0, 0x1E, 0x49,
	0xAA, 0x20, 0x00, 0x02, 0xAA, 0x30, 0x08, 0x80, 0x1C, 0x49, 0xAA, 0x20, 0x00, 0x02, 0xAA, 0x30,
	0x08, 0x80, 0x1B, 0x49, 0xAA, 0x20, 0x00, 0x02, 0xAA, 0x30, 0x08, 0x80, 0x19, 0x49, 0xAA, 0x20,
	0x00, 0x02, 0xAA, 0x30, 0x08, 0x80, 0x18, 0x49, 0xAA, 0x20, 0x00, 0x02, 0xAA, 0x30, 0x08, 0x80,
	0x16, 0x49, 0xAA, 0x20, 0x00, 0x02, 0xAA, 0x30, 0x08, 0x80, 0x15, 0x49, 0xAA, 0x20, 0x00, 0x02,
	0xAA, 0x30, 0x08, 0x80, 0x76, 0x08, 0x35, 0x42, 0x02, 0xD0, 0x12, 0x49, 0x0A, 0x20, 0x08, 0x80,
	0x00, 0x00, 0x21, 0xE0, 0xE0, 0x1D, 0x00, 0x03, 0xE0, 0x1D, 0x00, 0x03, 0xE0, 0x1D, 0x00, 0x03,
	0xE0, 0x1D, 0x00, 0x03, 0xE0, 0x1D, 0x00, 0x03, 0xE0, 0x1D, 0x00, 0x03, 0xD8, 0x03, 0x00, 0x03,
	0xF8, 0x03, 0x00, 0x03, 0x00, 0x05, 0x00, 0x03, 0xDA, 0x03, 0x00, 0x03, 0xDC, 0x03, 0x00, 0x03,
	0xDE, 0x03, 0x00, 0x03, 0xE0, 0x03, 0x00, 0x03, 0xE2, 0x03, 0x00, 0x03, 0xE4, 0x03, 0x00, 0x03,
	0xE6, 0x03, 0x00, 0x03, 0x48, 0x29, 0x00, 0x02, 0xFF, 0xBD, 0x00, 0x00, 0x00, 0xB5, 0x03, 0x48,
	0xFE, 0x46, 0x00, 0x47, 0x01, 0xBC, 0x86, 0x46, 0x01, 0xBC, 0x01, 0xE0, 0x01, 0x9C, 0x7B, 0x08,
	0x02, 0x48, 0x00, 0x88, 0xC0, 0x43, 0x80, 0x05, 0x81, 0x0D, 0x01, 0xE0, 0x30, 0x01, 0x00, 0x04,
	0x03, 0xB4, 0x01, 0x48, 0x01, 0x90, 0x01, 0xBD, 0x18, 0x1A, 0x00, 0x08
};

void patchGeneralWhiteScreen(u32 romSize) {
    u32 entryPoint = *(u32*)0x08000000;
	entryPoint -= 0xEA000000;
	entryPoint += 2;
	prefetchPatch[7] = 0x08000000+(entryPoint*4);

	u32 patchOffset = 0x01FFFFDC;

	vu32 *patchAddr = (vu32*)(0x08000000+patchOffset);
	for(int i=0;i<8;i++)patchAddr[i] = prefetchPatch[i];
	

	u32 branchCode = 0xEA000000+(patchOffset/sizeof(u32))-2;
	*(vu32*)0x08000000 = branchCode;

	u32 searchRange = 0x08000000+romSize;
	if (romSize > 0x01FFFFDC) searchRange = 0x09FFFFDC;

	// General fix for white screen crash
	// Patch out wait states
	for (u32 addr = 0x080000C0; addr < searchRange; addr+=4) {
		if (*(u32*)addr == 0x04000204 &&
		  (*(u8*)(addr-1) == 0x00 || *(u8*)(addr-1) == 0x03 || *(u8*)(addr-1) == 0x04 || *(u8*)(addr+7) == 0x04
		  || *(u8*)(addr-1) == 0x08 || *(u8*)(addr-1) == 0x09
		  || *(u8*)(addr-1) == 0x47 || *(u8*)(addr-1) == 0x81 || *(u8*)(addr-1) == 0x85
		  || *(u8*)(addr-1) == 0xE0 || *(u8*)(addr-1) == 0xE7 || *(u16*)(addr-2) == 0xFFFE)) 
		{
			*(vu32*)addr = 0;
		}
	}

	// Also check at 0x410
	if (*(u32*)0x08000410 == 0x04000204)*(vu32*)0x08000410 = 0;
}

bool patchSpecificGame() {
    u32 nop = 0xE1A00000;

	u32 gameCode = *(u32*)(0x080000AC);
			
	if (gameCode == 0x50584C42) {
		//Astreix & Obelix XXL (Europe)
		//Fix white screen crash
		if (*(u16*)(0x08000000 + 0x50118) == 0x4014)*(u16*)(0x08000000 + 0x50118) = 0x4000;
	} else if (gameCode == 0x454D4441) {
		//Doom (USA)
		//Fix black screen crash
		if (*(u16*)(0x08000000 + 0x51C) == 0x45B6)*(u16*)(0x08000000 + 0x51C) = 0x4002;
	} else if (gameCode == 0x45443941 || gameCode == 0x50443941) {
		//Doom II (USA/Europe)
		//Fix black screen crash
		if (*(u16*)(0x08000000 + 0x2856) == 0x5281)*(u16*)(0x08000000 + 0x2856) = 0x46C0;
	} else if (gameCode == 0x45474C41) {
		//Dragon Ball Z - The Legacy of Goku (USA)
		//Fix white screen crash
		if (*(u16*)(0x08000000 + 0x96E8) == 0x80A8)	*(u16*)(0x08000000 + 0x96E8) = 0x46C0;

		//Fix "game cannot be played on hardware found" error
		if (*(u16*)(0x08000000 + 0x356) == 0x7002)*(u16*)(0x08000000 + 0x356) = 0;
		if (*(u16*)(0x08000000 + 0x35E) == 0x7043)*(u16*)(0x08000000 + 0x35E) = 0;
		if (*(u16*)(0x08000000 + 0x37E) == 0x7001)*(u16*)(0x08000000 + 0x37E) = 0;
		if (*(u16*)(0x08000000 + 0x382) == 0x7041)*(u16*)(0x08000000 + 0x382) = 0;

		if (*(u16*)(0x08000000 + 0xE27E) == 0xB0A2) {
			*(u16*)(0x08000000 + 0xE27E) = 0x400;
			for (int i = 0; i < (int)sizeof(sDbzLoGUPatch1); i += 2)*(u16*)(0x08000000 + 0xE280 + i) = *(u16*)&sDbzLoGUPatch1[i];
			for (int i = 0; i < (int)sizeof(sDbzLoGUPatch2); i += 2)*(u16*)(0x08000000 + 0xE32C + i) = *(u16*)&sDbzLoGUPatch2[i];
		}
	} else if (gameCode == 0x50474C41) {
		//Dragon Ball Z - The Legacy of Goku (Europe)
		//Fix white screen crash
		if (*(u16*)(0x08000000 + 0x9948) == 0x80B0)*(u16*)(0x08000000 + 0x9948) = 0x46C0;

		//Fix "game cannot be played on hardware found" error
		if (*(u16*)(0x08000000 + 0x33C) == 0x7119)*(u16*)(0x08000000 + 0x33C) = 0x46C0;
		if (*(u16*)(0x08000000 + 0x340) == 0x7159)*(u16*)(0x08000000 + 0x340) = 0x46C0;
		if (*(u16*)(0x08000000 + 0x356) == 0x705A)*(u16*)(0x08000000 + 0x356) = 0x46C0;
		if (*(u16*)(0x08000000 + 0x35A) == 0x7002)*(u16*)(0x08000000 + 0x35A) = 0x46C0;
		if (*(u16*)(0x08000000 + 0x35E) == 0x7042)*(u16*)(0x08000000 + 0x35E) = 0x46C0;
		if (*(u16*)(0x08000000 + 0x384) == 0x7001)*(u16*)(0x08000000 + 0x384) = 0x46C0;
		if (*(u16*)(0x08000000 + 0x388) == 0x7041)*(u16*)(0x08000000 + 0x388) = 0x46C0;
		if (*(u16*)(0x08000000 + 0x494C) == 0x7002)*(u16*)(0x08000000 + 0x494C) = 0x46C0;
		if (*(u16*)(0x08000000 + 0x4950) == 0x7042)*(u16*)(0x08000000 + 0x4950) = 0x46C0;
		if (*(u16*)(0x08000000 + 0x4978) == 0x7001)*(u16*)(0x08000000 + 0x4978) = 0x46C0;
		if (*(u16*)(0x08000000 + 0x497C) == 0x7041)*(u16*)(0x08000000 + 0x497C) = 0x46C0;
		if (*(u16*)(0x08000000 + 0x988E) == 0x7028)*(u16*)(0x08000000 + 0x988E) = 0x46C0;
		if (*(u16*)(0x08000000 + 0x9992) == 0x7068)*(u16*)(0x08000000 + 0x9992) = 0x46C0;
	} else if (gameCode == 0x45464C41) {
		//Dragon Ball Z - The Legacy of Goku II (USA)
		*((u32*)0x080000E0) = nop;
//tonccpy((u16*)0x080000E0, &nop, sizeof(u32));	// Fix white screen crash
        
		//Fix "game will not run on the hardware found" error
		if (*(u16*)(0x08000000 + 0x3B8E9E) == 0x1102)*(u16*)(0x08000000 + 0x3B8E9E) = 0x1001;
		if (*(u16*)(0x08000000 + 0x3B8EAE) == 0x0003)*(u16*)(0x08000000 + 0x3B8EAE) = 0;
	} else if (gameCode == 0x4A464C41) {
		//Dragon Ball Z - The Legacy of Goku II International (Japan)
		*((u32*)0x080000E0) = nop;
//tonccpy((u16*)0x080000E0, &nop, sizeof(u32));	// Fix white screen crash

		//Fix "game will not run on the hardware found" error
		if (*(u16*)(0x08000000 + 0x3FC8F6) == 0x1102)*(u16*)(0x08000000 + 0x3FC8F6) = 0x1001;
		if (*(u16*)(0x08000000 + 0x3FC906) == 0x0003)*(u16*)(0x08000000 + 0x3FC906) = 0;
	} else if (gameCode == 0x50464C41) {
		//Dragon Ball Z - The Legacy of Goku II (Europe)
		*((u32*)0x080000E0) = nop;
//tonccpy((u16*)0x080000E0, &nop, sizeof(u32));	// Fix white screen crash

		//Fix "game will not run on the hardware found" error
		if (*(u16*)(0x08000000 + 0x6F42B2) == 0x1102)*(u16*)(0x08000000 + 0x6F42B2) = 0x1001;
		if (*(u16*)(0x08000000 + 0x6F42C2) == 0x0003)*(u16*)(0x08000000 + 0x6F42C2) = 0;
	} else if (gameCode == 0x45464C42) {
		//2 Games in 1 - Dragon Ball Z - The Legacy of Goku I & II (USA)
		*((u32*)0x080000E0) = nop;
//tonccpy((u16*)0x080000E0, &nop, sizeof(u32));	// Fix white screen crash

		if (*(u16*)(0x08000000 + 0x49840) == 0x80A8)*(u16*)(0x08000000 + 0x49840) = 0x46C0;

		// tonccpy((u16*)0x088000E0, &nop, sizeof(u32));
        *((u32*)0x080000E0) = nop;

		//LoG1: Fix "game cannot be played on hardware found" error
		if (*(u16*)(0x08000000 + 0x40356) == 0x7002)*(u16*)(0x08000000 + 0x40356) = 0;
		if (*(u16*)(0x08000000 + 0x4035E) == 0x7043)*(u16*)(0x08000000 + 0x4035E) = 0;
		if (*(u16*)(0x08000000 + 0x4037E) == 0x7001)*(u16*)(0x08000000 + 0x4037E) = 0;
		if (*(u16*)(0x08000000 + 0x40382) == 0x7041)*(u16*)(0x08000000 + 0x40382) = 0;

		//Do we need this?
		/*if (*(u16*)(0x08000000 + 0x4E316) == 0xB0A2) {
			*(u16*)(0x08000000 + 0x4E316) = 0x400;
			for (int i = 0; i < sizeof(sDbzLoGUPatch1); i += 2)*(u16*)(0x08000000 + 0x4E318 + i) = *(u16*)&sDbzLoGUPatch1[i];
			for (int i = 0; i < sizeof(sDbzLoGUPatch2); i += 2)*(u16*)(0x08000000 + 0x????? + i) = *(u16*)&sDbzLoGUPatch2[i];
		}*/

		//LoG2: Fix "game will not run on the hardware found" error
		if (*(u16*)(0x08000000 + 0xBB9016) == 0x1102)*(u16*)(0x08000000 + 0xBB9016) = 0x1001;
		if (*(u16*)(0x08000000 + 0xBB9026) == 0x0003)*(u16*)(0x08000000 + 0xBB9026) = 0;
	} else if (gameCode == 0x45424442) {
		//Dragon Ball Z - Taiketsu (USA)
		//Fix "game cannot be played on this hardware" error
		if (*(u16*)(0x08000000 + 0x2BD54) == 0x7818)*(u16*)(0x08000000 + 0x2BD54) = 0x2000;
		if (*(u16*)(0x08000000 + 0x2BD60) == 0x7810)*(u16*)(0x08000000 + 0x2BD60) = 0x2000;
		if (*(u16*)(0x08000000 + 0x2BD80) == 0x703A)*(u16*)(0x08000000 + 0x2BD80) = 0x1C00;
		if (*(u16*)(0x08000000 + 0x2BD82) == 0x7839)*(u16*)(0x08000000 + 0x2BD82) = 0x2100;
		if (*(u16*)(0x08000000 + 0x2BD8C) == 0x7030)*(u16*)(0x08000000 + 0x2BD8C) = 0x1C00;
		if (*(u16*)(0x08000000 + 0x2BD8E) == 0x7830)*(u16*)(0x08000000 + 0x2BD8E) = 0x2000;
		if (*(u16*)(0x08000000 + 0x2BDAC) == 0x7008)*(u16*)(0x08000000 + 0x2BDAC) = 0x1C00;
		if (*(u16*)(0x08000000 + 0x2BDB2) == 0x7008)*(u16*)(0x08000000 + 0x2BDB2) = 0x1C00;
	} else if (gameCode == 0x50424442) {
		//Dragon Ball Z - Taiketsu (Europe)
		//Fix "game cannot be played on this hardware" error
		if (*(u16*)(0x08000000 + 0x3FE08) == 0x7818)*(u16*)(0x08000000 + 0x3FE08) = 0x2000;
		if (*(u16*)(0x08000000 + 0x3FE14) == 0x7810)*(u16*)(0x08000000 + 0x3FE14) = 0x2000;
		if (*(u16*)(0x08000000 + 0x3FE34) == 0x703A)*(u16*)(0x08000000 + 0x3FE34) = 0x1C00;
		if (*(u16*)(0x08000000 + 0x3FE36) == 0x7839)*(u16*)(0x08000000 + 0x3FE36) = 0x2100;
		if (*(u16*)(0x08000000 + 0x3FE40) == 0x7030)*(u16*)(0x08000000 + 0x3FE40) = 0x1C00;
		if (*(u16*)(0x08000000 + 0x3FE42) == 0x7830)*(u16*)(0x08000000 + 0x3FE42) = 0x2000;
		if (*(u16*)(0x08000000 + 0x3FE58) == 0x7008)*(u16*)(0x08000000 + 0x3FE58) = 0x1C00;
		if (*(u16*)(0x08000000 + 0x3FE66) == 0x7008)*(u16*)(0x08000000 + 0x3FE66) = 0x1C00;
	} else if (gameCode == 0x45334742) {
		//Dragon Ball Z - Buu's Fury (USA)
		*((u32*)0x080000E0) = nop;
		//tonccpy((u16*)0x080000E0, &nop, sizeof(u32));	// Fix white screen crash

		//Fix "game will not run on this hardware" error
		if (*(u16*)(0x08000000 + 0x8B66) == 0x7032)*(u16*)(0x08000000 + 0x8B66) = 0;
		if (*(u16*)(0x08000000 + 0x8B6A) == 0x7072)*(u16*)(0x08000000 + 0x8B6A) = 0;
		if (*(u16*)(0x08000000 + 0x8B86) == 0x7008)*(u16*)(0x08000000 + 0x8B86) = 0;
		if (*(u16*)(0x08000000 + 0x8B8C) == 0x7031)*(u16*)(0x08000000 + 0x8B8C) = 0;
		if (*(u16*)(0x08000000 + 0x8B90) == 0x7071)*(u16*)(0x08000000 + 0x8B90) = 0;
	} else if (gameCode == 0x45345442) {
		//Dragon Ball GT - Transformation (USA)
		*((u32*)0x080000E0) = nop;
        //tonccpy((u16*)0x080000E0, &nop, sizeof(u32));	// Fix white screen crash
	} else if (gameCode == 0x45465542) {
		//2 Games in 1 - Dragon Ball Z - Buu's Fury & Dragon Ball GT - Transformation (USA)
		*((u32*)0x080000E0) = nop;
        //tonccpy((u16*)0x080000E0, &nop, sizeof(u32));	// Fix white screen crash
        *((u32*)0x080300E0) = nop;
        *((u32*)0x088000E0) = nop;
		// tonccpy((u16*)0x080300E0, &nop, sizeof(u32));
		// tonccpy((u16*)0x088000E0, &nop, sizeof(u32));

		//DBZ BF: Fix "game will not run on this hardware" error
		if (*(u16*)(0x08000000 + 0x38B66) == 0x7032)*(u16*)(0x08000000 + 0x38B66) = 0;
		if (*(u16*)(0x08000000 + 0x38B6A) == 0x7072)*(u16*)(0x08000000 + 0x38B6A) = 0;
		if (*(u16*)(0x08000000 + 0x38B86) == 0x7008)*(u16*)(0x08000000 + 0x38B86) = 0;
		if (*(u16*)(0x08000000 + 0x38B8C) == 0x7031)*(u16*)(0x08000000 + 0x38B8C) = 0;
		if (*(u16*)(0x08000000 + 0x38B90) == 0x7071)*(u16*)(0x08000000 + 0x38B90) = 0;
	} else if (gameCode == 0x45564442) {
		//Dragon Ball - Advanced Adventure (USA)
		//Fix white screen crash
		if (*(u16*)(0x08000000 + 0x10C240) == 0x8008)*(u16*)(0x08000000 + 0x10C240) = 0x46C0;
	} else if (gameCode == 0x50564442) {
		//Dragon Ball - Advanced Adventure (Europe)
		//Fix white screen crash
		if (*(u16*)(0x08000000 + 0x10CE3C) == 0x8008)*(u16*)(0x08000000 + 0x10CE3C) = 0x46C0;
	} else if (gameCode == 0x4A564442) {
		//Dragon Ball - Advanced Adventure (Japan)
		//Fix white screen crash
		if (*(u16*)(0x08000000 + 0x10B078) == 0x8008)*(u16*)(0x08000000 + 0x10B078) = 0x46C0;
	} else if (gameCode == 0x454B3842) {
		//Kirby and the Amazing Mirror (USA)
		//Fix white screen crash
		if (*(u16*)(0x08000000 + 0x1515A4) == 0x8008)*(u16*)(0x08000000 + 0x1515A4) = 0x46C0;
	} else if (gameCode == 0x504B3842) {
		//Kirby and the Amazing Mirror (Europe)
		//Fix white screen crash
		if (*(u16*)(0x08000000 + 0x151EE0) == 0x8008)*(u16*)(0x08000000 + 0x151EE0) = 0x46C0;
	} else if (gameCode == 0x4A4B3842) {
		//Hoshi no Kirby - Kagami no Daimeikyuu (Japan) (V1.1)
		//Fix white screen crash
		if (*(u16*)(0x08000000 + 0x151564) == 0x8008)*(u16*)(0x08000000 + 0x151564) = 0x46C0;
	} else if (gameCode == 0x45533342) {
		//Sonic Advance 3 (USA)
		//Fix white screen crash
		if (*(u16*)(0x08000000 + 0xBB67C) == 0x8008)*(u16*)(0x08000000 + 0xBB67C) = 0x46C0;
	} else if (gameCode == 0x50533342) {
		//Sonic Advance 3 (Europe)
		//Fix white screen crash
		if (*(u16*)(0x08000000 + 0xBBA04) == 0x8008)*(u16*)(0x08000000 + 0xBBA04) = 0x46C0;
	} else if (gameCode == 0x4A533342) {
		//Sonic Advance 3 (Japan)
		//Fix white screen crash
		if (*(u16*)(0x08000000 + 0xBB9F8) == 0x8008)*(u16*)(0x08000000 + 0xBB9F8) = 0x46C0;
	} else if (gameCode == 0x45593241) {
		//Top Gun - Combat Zones (USA)
		//Fix softlock when attempting to save (original cartridge does not have a save chip)
		if (*(u16*)(0x08000000 + 0x88816) == 0x3501)*(u16*)(0x08000000 + 0x88816) = 0x3401;

		// savingAllowed = false;
		return false;
	} else if (gameCode == 0x45415741 || gameCode == 0x4A415741) {
		//Wario Land 4/Advance (USA/Europe/Japan)
		//Fix white screen crash
		if (*(u16*)(0x08000000 + 0x726) == 0x8008)*(u16*)(0x08000000 + 0x726) = 0x46C0;
	} else if (gameCode == 0x43415741) {
		//Wario Land Advance (iQue)
		//Fix white screen crash
		if (*(u16*)(0x08000000 + 0xE92) == 0x8008)*(u16*)(0x08000000 + 0xE92) = 0x46C0;
	} else if (gameCode == 0x45575A52) {
		//WarioWare: Twisted! (USA)
		//Patch out tilt controls
		if (*(u16*)(0x08000000 + 0x1348) == 0x8800)*(u16*)(0x08000000 + 0x1348) = 0x4700;

		if (*(u16*)(0x08000000 + 0x1376) == 0x0400 && *(u16*)(0x08000000 + 0x1374) == 0x0130) {
			*(u16*)(0x08000000 + 0x1376) = 0x08E9;
			*(u16*)(0x08000000 + 0x1374) = 0x3C6D;

			// tonccpy((u8*)0x08E93C6C, &wwTwistedPatch, 0xF0);
            twoByteCpy((u16*)0x08E93C6C,(const u16*)wwTwistedPatch,0xF0);
		}
	} else if (gameCode == 0x4547594B) {
		//Yoshi Topsy-Turvy (USA)
		//Fix white screen crash
		if (*(u16*)(0x08000000 + 0x16E4) == 0x8008)*(u16*)(0x08000000 + 0x16E4) = 0x46C0;

		//Patch out tilt controls
		if (*(u16*)(0x08000000 + 0x1F2) == 0x0802 && *(u16*)(0x08000000 + 0x1F0) == 0x5169) {
			*(u16*)(0x08000000 + 0x1F2) = 0x087B;
			*(u16*)(0x08000000 + 0x1F0) = 0x9BE0;

			// tonccpy((u8*)0x087B9BE0, &yoshiTopsyTurvyPatch, 0x18C);
            twoByteCpy((u16*)0x087B9BE0,(const u16*)yoshiTopsyTurvyPatch,0x18C);
		}

		if (*(u16*)(0x08000000 + 0x1A0E) == 0x4808)*(u16*)(0x08000000 + 0x1A0E) = 0xB401;
		if (*(u16*)(0x08000000 + 0x1A10) == 0x8800)*(u16*)(0x08000000 + 0x1A10) = 0x4800;
		if (*(u16*)(0x08000000 + 0x1A12) == 0x43C0)*(u16*)(0x08000000 + 0x1A12) = 0x4700;
		if (*(u16*)(0x08000000 + 0x1A14) == 0x0580)*(u16*)(0x08000000 + 0x1A14) = 0x9D3D;
		if (*(u16*)(0x08000000 + 0x1A16) == 0x0D81)*(u16*)(0x08000000 + 0x1A16) = 0x087B;
	}
	return true;
}

