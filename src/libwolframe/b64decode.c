/*
cdecoder.c - c source to a base64 decoding algorithm implementation

This is part of the libb64 project, and has been placed in the public domain.
For details, see http://sourceforge.net/projects/libb64
*/

#include <base64.h>

static const char decodeTable[] = { 62, -1, -1, -1, 63, 52, 53, 54,
				    55, 56, 57, 58, 59, 60, 61, -1,
				    -1, -1, -2, -1, -1, -1,  0,  1,
				     2,  3,  4,  5,  6,  7,  8,  9,
				    10, 11, 12, 13, 14, 15, 16, 17,
				    18, 19, 20, 21, 22, 23, 24, 25,
				    -1, -1, -1, -1, -1, -1, 26, 27,
				    28, 29, 30, 31, 32, 33, 34, 35,
				    36, 37, 38, 39, 40, 41, 42, 43,
				    44, 45, 46, 47, 48, 49, 50, 51
				  };

static const unsigned short decodeTableSize = sizeof( decodeTable );

static inline int base64_decodeValue( char value )
{
	value -= 43;
	if ( value < 0 || value > decodeTableSize )
		return -1;
	return decodeTable[ (int)value ];
}

void base64_initDecodeState( base64_DecodeState* state )
{
	state->step = STEP_A;
	state->plainchar = 0;
}

int base64_decodeBlock( base64_DecodeState* state, const char* encoded,
			size_t codeLength, unsigned char* plain, size_t plainMaxSize )
{
	const char* codeChar = encoded;
	const char* encodedEnd = encoded + codeLength;
	unsigned char* plainByte = plain;
	char fragment;

	*plainByte = state->plainchar;

	switch ( state->step )	{
		while ( 1 )	{
			case STEP_A:
				do {
					if ( codeChar == encodedEnd )	{
						state->step = STEP_A;
						state->plainchar = *plainByte;
						return plainByte - plain;
					}
					fragment = (char)base64_decodeValue( *codeChar++ );
				} while ( fragment < 0 );
				*plainByte = ( fragment & 0x03f ) << 2;
			case STEP_B:
				do {
					if ( codeChar == encodedEnd )	{
						state->step = STEP_B;
						state->plainchar = *plainByte;
						return plainByte - plain;
					}
					fragment = (char)base64_decodeValue( *codeChar++ );
				} while ( fragment < 0 );
				if ( plainMaxSize-- < 1 )
					return BUFFER_OVERFLOW;
				*plainByte++ |= ( fragment & 0x030 ) >> 4;
				*plainByte = ( fragment & 0x00f ) << 4;
			case STEP_C:
				do {
					if ( codeChar == encodedEnd )	{
						state->step = STEP_C;
						state->plainchar = *plainByte;
						return plainByte - plain;
					}
					fragment = (char)base64_decodeValue( *codeChar++ );
				} while ( fragment < 0 );
				if ( plainMaxSize-- < 1 )
					return BUFFER_OVERFLOW;
				*plainByte++ |= ( fragment & 0x03c ) >> 2;
				*plainByte = ( fragment & 0x003 ) << 6;
			case STEP_D:
				do {
					if ( codeChar == encodedEnd )	{
						state->step = STEP_D;
						state->plainchar = *plainByte;
						return plainByte - plain;
					}
					fragment = (char)base64_decodeValue( *codeChar++ );
				} while ( fragment < 0 );
				if ( plainMaxSize-- < 1 )
					return BUFFER_OVERFLOW;
				*plainByte++ |= ( fragment & 0x03f );
		}
	}
	/* control should not reach here */
	return plainByte - plain;
}
