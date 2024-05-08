#include <nds.h>
#include <string.h>
#include <ctype.h>
#include "unicode.h"

/*#include "unicode_u2l_bin.h"
#include "unicode_l2u_bin.h"
#include "unicode_ank_bin.h"*/

extern u16 unicode_u2l_bin[];
extern u16 unicode_l2u_bin[];
extern u16 unicode_ank_bin[];


static u16 _codePage = 0;

static const u16 * _L2UTable = NULL; // custom local to unicode

static u16 _L2UTableSize = 0;

static const u16 * _U2LTable = NULL;

static u16 _U2LTableSize = 0;

static const u8 * _ankTable = NULL;


void _FAT_unicode_init_default() // ANSI CODE PAGE
{
//	_L2UTable = NULL;
//	_U2LTable = NULL;
//	_ankTable = NULL;

	_FAT_unicode_init((const u16*)unicode_l2u_bin, (const u16*)unicode_u2l_bin, (const u8*)unicode_ank_bin);
}

void _FAT_unicode_init( const u16 * l2uTableData, const u16 * u2lTableData, const u8 * ankData )
{
	if( NULL != l2uTableData ) {
		_codePage = l2uTableData[0];
		_L2UTableSize = l2uTableData[1];
		_L2UTable = l2uTableData + 2;
	}
	if( NULL != u2lTableData ) {
		_U2LTableSize = u2lTableData[1];
		_U2LTable = u2lTableData + 2;
	}
	_ankTable = ankData;
}

static void _ank_char_to_unicode( const u8 * src, u16 * dest )
{
	while( 0 != *src )
	{
		*dest = (UNICHAR)(*src);
		dest++;
		src++;
	}
	*dest = (UNICHAR)0;
}

static void _unicode_to_ank_char( const u16 * src, u8 * dest )
{
	u16 local = 0;
	while( 0x0000 != *src )
	{
		local = (*src++);
		if( !(local >= 0x20 && local <= 0xff) )
			local = '_';

		*dest++ = local & 0xff;
	}
	*dest = 0x00;
}

void _FAT_unicode_local_to_unicode( const u8 * src, UNICHAR * dest )
{
	if( NULL == _L2UTable || NULL == _ankTable )
	{
		_ank_char_to_unicode( src, dest );
		return;
	}

	while( 0 != *src )
	{
		u16 lc = 0;

		lc=(u16)*src;
		src++;

		if( 0 == _ankTable[lc] && 0 != *src )
		{
			lc=(lc << 8)+((u16)*src);
			src++;
		}

		if( lc < _L2UTableSize )
		{
			*dest = _L2UTable[lc];
			if( *dest == (UNICHAR)0) 
				*dest = (UNICHAR)'?';
		}
		else
		{
			*dest=(UNICHAR)'?';
		}
		dest++;
	}

	*dest = (UNICHAR)0;
}

void _FAT_unicode_unicode_to_local( const UNICHAR * src, u8 * dest )
{
	//if( defCharUsed ) 
	//	*defCharUsed = false;
	//std::string ret;
	if( NULL == _U2LTable || NULL == _ankTable )
	{
		_unicode_to_ank_char( src, dest );
		return;
	}

	const UNICHAR * p = src;
	while( *p != 0x0000 /*&& p < unicode_string + length*/ )
	{
		u16 localChar = 0;
		if( *p < _U2LTableSize ) { // p's value < table size;
			localChar = _U2LTable[*p];
		}
		else {
			localChar = '?';
			// 使用 '?' 作为默认字符好处是，文件系统的文件名里不会包含 '?' 字符，错误容易发现
		}
		if( '?' == localChar && (u16)'?' != *p ) {
			//if( defCharUsed ) 
			//	*defCharUsed = true;
			//dbg_printf( "caution: uni %04x->%04x, CP%d\n", *p, localChar, _codePage );
		}

		*dest++ = (u8)(localChar & 0xFF);
		if( (localChar & 0xFF00) ) {
			*dest++ = (u8)( (localChar  & 0xFF00) >> 8 );
		}
		++p;
	}
	*dest = 0;
}

u32 _unistrnlen( const u16 * unistr, u32 maxlen )
{
	const u16 * pstr = NULL;
	u32 len = 0;
	if( NULL == unistr )
		return 0;

	if( 0 == maxlen )
		return 0;

	pstr = unistr;

	while( len < maxlen && *pstr != 0x0000 )
	{
		++len; 
		++pstr;
	}
	return len;
}

int _unistrncmp( const u16 * src, const u16 * dest, u32 maxlen )
{
	if( NULL == src || NULL == dest )
	{
		if( src == dest ) return NULL;
		return (NULL == src ) ? -1 : 1;
	}

	while( *src == *dest && maxlen && *src != 0x0000 && *dest != 0x0000 )
	{
		++src;
		++dest;
		--maxlen;
	}
	if( 0 == maxlen ) return 0;

	return *src > *dest ? 1 : -1;
}

const u16 * _unistrchr( const u16 * str, u16 unichar )
{
	if( NULL == str )
		return NULL;

	while( *str != unichar && *str != 0x0000 )
	{
		++str;
	}
	return str;
}

bool _uniisalnum( u8 ch )
{
	if( NULL == _ankTable ) 
		return isalnum( ch );

	return (0 == _ankTable[ch]) || isalnum( ch );
}
