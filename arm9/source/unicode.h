#ifndef _LIBFAT_UNICODE_H_
#define _LIBFAT_UNICODE_H_

typedef u16 UNICHAR;

void _FAT_unicode_init_default(); // ANSI CODE PAGE

// l2u/u2l table is a map, key and value are both in range 0x0000 - 0xffff
void _FAT_unicode_init( const u16 * l2uTableData, const u16 * u2lTableData, const u8 * ankData );

void _FAT_unicode_uninit(); // do we really need this???

void _FAT_unicode_local_to_unicode( const u8 * src, UNICHAR * dest );

void _FAT_unicode_unicode_to_local( const UNICHAR * src, u8 * dest );

u32 _unistrnlen( const u16 * unistr, u32 maxlen );

int _unistrncmp( const u16 * src, const u16 * dest, u32 maxlen );

const u16 * _unistrchr( const u16 * str, u16 unichar );

bool _uniisalnum( u8 ch );



#endif//_LIBFAT_UNICODE_H_
