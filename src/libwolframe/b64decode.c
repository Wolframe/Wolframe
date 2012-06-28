/*
cdecoder.c - c source to a base64 decoding algorithm implementation

This is part of the libb64 project, and has been placed in the public domain.
For details, see http://sourceforge.net/projects/libb64
*/

#include <base64.h>

static const char decodeTable[] = {
	/* 0 - 31 / 0x00 - 0x1f */
		-1, -1, -1, -1, -1, -1, -1, -1,
		-1, -2, -2, -1, -1, -2, -1, -1,	/* ... '\t', '\n','\r' */
		-1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1,
	/* 32 - 63 / 0x20 - 0x3f */
		-2, -1, -1, -1, -1, -1, -1, -1, /* ' ', ...             */
		-1, -1, -1, 62, -1, -1, -1, 63,	/* ... , '+', ... '/'   */
		52, 53, 54, 55, 56, 57, 58, 59,	/* '0' - '7'            */
		60, 61, -1, -1, -1, -2, -1, -1,	/* '8', '9', ... '=' .. */
	/* 64 - 95 / 0x40 - 0x5f */
		-1, 0,  1,  2,  3,  4,  5,  6,	/* ..., 'A' - 'G'       */
		7,  8,  9,  10, 11, 12, 13, 14,	/* 'H' - 'O'            */
		15, 16, 17, 18, 19, 20, 21, 22,	/* 'P' - 'W'            */
		23, 24, 25, -1, -1, -1, -1, -1,	/* 'X', 'Y', 'Z', ...   */
	/* 96 - 127 / 0x60 - 0x7f */
		-1, 26, 27, 28, 29, 30, 31, 32,	/* ..., 'a' - 'g'       */
		33, 34, 35, 36, 37, 38, 39, 40,	/* 'h' - 'o'            */
		41, 42, 43, 44, 45, 46, 47, 48,	/* 'p' - 'w'            */
		49, 50, 51, -1, -1, -1, -1, -1,	/* 'x', 'y', 'z', ...   */

		-1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1,

		-1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1,

		-1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1,

		-1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1,
	};

static const unsigned short decodeTableSize = sizeof( decodeTable );

static inline int base64_decodeValue( char value )
{
	return decodeTable[ (int)value ];
}

void base64_initDecodeState( base64_DecodeState* state )
{
	state->step = STEP_A;
	state->dataByte = 0;
}

int base64_decodeBlock( base64_DecodeState* state, const char* encoded, size_t encodedSize,
			void* data, size_t dataMaxSize )
{
	const char* codeChar = encoded;
	const char* const encodedEnd = encoded + encodedSize;
	unsigned char* dataByte = ( unsigned char* )data;
	char fragment;

	*dataByte = state->dataByte;

	switch ( state->step )	{
		while ( 1 )	{
			case STEP_A:
				do {
					if ( codeChar == encodedEnd )	{
						state->step = STEP_A;
						state->dataByte = *dataByte;
						return dataByte - ( unsigned char* )data;
					}
					fragment = (char)base64_decodeValue( *codeChar++ );
				} while ( fragment < 0 );
				*dataByte = ( fragment & 0x03f ) << 2;
			case STEP_B:
				do {
					if ( codeChar == encodedEnd )	{
						state->step = STEP_B;
						state->dataByte = *dataByte;
						return dataByte - ( unsigned char* )data;
					}
					fragment = (char)base64_decodeValue( *codeChar++ );
				} while ( fragment < 0 );
				if ( dataMaxSize-- < 1 )
					return BUFFER_OVERFLOW;
				*dataByte++ |= ( fragment & 0x030 ) >> 4;
				*dataByte = ( fragment & 0x00f ) << 4;
			case STEP_C:
				do {
					if ( codeChar == encodedEnd )	{
						state->step = STEP_C;
						state->dataByte = *dataByte;
						return dataByte - ( unsigned char* )data;
					}
					fragment = (char)base64_decodeValue( *codeChar++ );
				} while ( fragment < 0 );
				if ( dataMaxSize-- < 1 )
					return BUFFER_OVERFLOW;
				*dataByte++ |= ( fragment & 0x03c ) >> 2;
				*dataByte = ( fragment & 0x003 ) << 6;
			case STEP_D:
				do {
					if ( codeChar == encodedEnd )	{
						state->step = STEP_D;
						state->dataByte = *dataByte;
						return dataByte - ( unsigned char* )data;
					}
					fragment = (char)base64_decodeValue( *codeChar++ );
				} while ( fragment < 0 );
				if ( dataMaxSize-- < 1 )
					return BUFFER_OVERFLOW;
				*dataByte++ |= ( fragment & 0x03f );
		}
	}
	/* it should not reach this point */
	return dataByte - ( unsigned char* )data;
}
