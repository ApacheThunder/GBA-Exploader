/*
	io_m3_common.h 

	Routines common to all version of the Super Card

 Copyright (c) 2006 Michael "Chishm" Chisholm
	
 Redistribution and use in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:

  1. Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation and/or
     other materials provided with the distribution.
  3. The name of the author may not be used to endorse or promote products derived
     from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE
 LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "io_sc_common.h"

#ifndef NULL
	#define NULL 0
#endif

/*-----------------------------------------------------------------
_SC_changeMode (was SC_Unlock)
Added by MightyMax
Modified by Chishm
Modified again by loopy
1=ram(readonly), 5=ram, 3=SD interface?
-----------------------------------------------------------------*/
void _SC_changeMode(u8 mode) {
	vu16 *unlockAddress = (vu16*)0x09FFFFFE;
	*unlockAddress = 0xA55A;
	*unlockAddress = 0xA55A;
	*unlockAddress = mode;
	*unlockAddress = mode;
} 

void _SC_changeMode16(u16 mode) {
	vu16 *unlockAddress = (vu16*)0x09FFFFFE;
	*unlockAddress = 0xA55A;
	*unlockAddress = 0xA55A;
	*unlockAddress = mode;
	*unlockAddress = mode;
} 

void twoByteCpy(u16 *dst, const u16 *src, u32 size) {
	if (size == 0 || dst == NULL || src == NULL)return;

	u32 count;
	u16 *dst16;	 // hword destination
	u8  *src8;	  // byte source

	// Ideal case: copy by 4x words. Leaves tail for later.
	if (((u32)src|(u32)dst) %4 == 0 && size >= 4) {
		u32 *src32= (u32*)src, *dst32= (u32*)dst;

		count = size/4;
		u32 tmp = count&3;
		count /= 4;

		// Duff's Device, good friend!
		switch(tmp) {
			do {	*dst32++ = *src32++;
			case 3:	 *dst32++ = *src32++;
			case 2:	 *dst32++ = *src32++;
			case 1:	 *dst32++ = *src32++;
			case 0:	 ; } while (count--);
		}

		// Check for tail
		size &= 3;
		if (size == 0)return;

		src8 = (u8*)src32;
		dst16 = (u16*)dst32;
	} else {
		// Unaligned

		u32 dstOfs = (u32)dst&1;
		src8 = (u8*)src;
		dst16 = (u16*)(dst-dstOfs);

		// Head: 1 byte.
		if (dstOfs != 0) {
			*dst16 = (*dst16 & 0xFF) | *src8++<<8;
			dst16++;
			if (--size == 0)return;
		}
	}

	// Unaligned main: copy by 2x byte.
	count = size/2;
	while (count--) {
		*dst16++ = src8[0] | src8[1]<<8;
		src8 += 2;
	}

	// Tail: 1 byte.
	if (size & 1)*dst16 = (*dst16 &~ 0xFF) | *src8;
}