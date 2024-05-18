/*---------------------------------------------------------------------------------


Copyright (C) 2007 Acekard, www.acekard.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.


---------------------------------------------------------------------------------*/

#include <nds.h>

#include "memcleaner.h"

void resetARM9Memory() {
    // DMA
    for (u8 i=0; i<4; i++) {
        DMA_CR(i) = 0;
        DMA_SRC(i) = 0;
        DMA_DEST(i) = 0;
        TIMER_CR(i) = 0;
        TIMER_DATA(i) = 0;
    }

    swiWaitForVBlank(); // wait for VBLANK to avoid screen picture break
    //PALETTE[0] = 0xFFFF;

    // VIDEO
    // trun on vram banks for clear
	VRAM_CR = 0x80808080;
	(*(vu32*)0x027FFE04) = 0;   // temporary variable
	BG_PALETTE[0] = 0xFFFF;
	dmaFillWords(0, BG_PALETTE+1, (2*1024)-2);
	dmaFillWords(0, OAM,     2*1024);
	dmaFillWords(0, (void*)0x04000000, 0x56);  //clear main display registers
	dmaFillWords(0, (void*)0x04001000, 0x56);  //clear sub  display registers

    // clear video registers
    REG_DISPCNT = 0;
    REG_DISPCNT_SUB = 0;
    VRAM_A_CR = 0;
    VRAM_B_CR = 0;
    VRAM_C_CR = 0;
    VRAM_D_CR = 0;
    VRAM_E_CR = 0;
    VRAM_F_CR = 0;
    VRAM_G_CR = 0;
    VRAM_H_CR = 0;
    VRAM_I_CR = 0;
    VRAM_CR   = 0x00000000;
    REG_POWERCNT  = 0x820F; // turn on all engines

    // Interrupt
    REG_IME = 0;
    REG_IE = 0;
    REG_IF = ~0;
}

