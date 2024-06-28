#include <nds.h>
#include <stddef.h> // NULL
//#include <string.h> // memcmp
//#include <limits.h>

// (memcmp is slower)
//#define memcmp __builtin_memcmp

//#define TABLE_SIZE (UCHAR_MAX + 1) // 256

/*
*   Look for @find and return the position of it.
*   Brute Force algorithm
*/
u32* memsearch32(const u32* start, u32 dataSize, const u32* find, u32 findSize, bool forward) {
	u32 dataLen = dataSize/sizeof(u32);
	u32 findLen = findSize/sizeof(u32);

	const u32* end = forward ? (start + dataLen) : (start - dataLen);
	for (u32* addr = (u32*)start; addr != end; forward ? ++addr : --addr) {
		bool found = true;
		for (u32 j = 0; j < findLen; ++j) {
			if (addr[j] != find[j]) {
				found = false;
				break;
			}
		}
		if (found) {
			return (u32*)addr;
		}
	}
	return NULL;
}
u16* memsearch16(const u16* start, u32 dataSize, const u16* find, u32 findSize, bool forward) {
	u32 dataLen = dataSize/sizeof(u16);
	u32 findLen = findSize/sizeof(u16);

	const u16* end = forward ? (start + dataLen) : (start - dataLen);
	for (u16* addr = (u16*)start; addr != end; forward ? ++addr : --addr) {
		bool found = true;
		for (u32 j = 0; j < findLen; ++j) {
			if (addr[j] != find[j]) {
				found = false;
				break;
			}
		}
		if (found) {
			return (u16*)addr;
		}
	}
	return NULL;
}
u8* memsearch8(const u8* start, u32 dataSize, const u8* find, u32 findSize, bool forward) {
	u32 dataLen = dataSize/sizeof(u8);
	u32 findLen = findSize/sizeof(u8);

	const u8* end = forward ? (start + dataLen) : (start - dataLen);
	for (u8* addr = (u8*)start; addr != end; forward ? ++addr : --addr) {
		bool found = true;
		for (u32 j = 0; j < findLen; ++j) {
			if (addr[j] != find[j]) {
				found = false;
				break;
			}
		}
		if (found) {
			return (u8*)addr;
		}
	}
	return NULL;
}

