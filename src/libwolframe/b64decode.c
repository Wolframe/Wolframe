/************************************************************************

 Copyright (C) 2011 - 2014 Project Wolframe.
 All rights reserved.

 This file is part of Project Wolframe.

 Commercial Usage
    Licensees holding valid Project Wolframe Commercial licenses may
    use this file in accordance with the Project Wolframe
    Commercial License Agreement provided with the Software or,
    alternatively, in accordance with the terms contained
    in a written agreement between the licensee and Project Wolframe.

 GNU General Public License Usage
    Alternatively, you can redistribute this file and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Wolframe is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
/*
 * b64decode.c - base64 decoding implementation
 */

#include "types/base64.h"

static const int decodeTable[] = {
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

static int base64_decodeValue( char value )
{
	return decodeTable[ (int)value ];
}

void base64_initDecodeState( base64_DecodeState* state )
{
	state->step = STEP_A;
	state->dataByte = 0;
}

int base64_decodeChunk( base64_DecodeState* state, const char* encoded, size_t encodedSize,
			void* data, size_t dataMaxSize )
{
	const char* encodedEnd;
	unsigned char* outByte;
	unsigned char dataByte;
	int fragment;

	encodedEnd = encoded + encodedSize;
	outByte = ( unsigned char *)data;
	dataByte = state->dataByte;

	switch ( state->step )	{
		for ( ; ; )	{
			case STEP_A:
				do {
					if ( encoded == encodedEnd )	{
						state->step = STEP_A;
						state->dataByte = dataByte;
						return outByte - ( unsigned char* )data;
					}
					fragment = base64_decodeValue( *encoded++ );
				} while ( fragment < 0 );
				dataByte = ( fragment & 0x03f ) << 2;
			case STEP_B:
				do {
					if ( encoded == encodedEnd )	{
						state->step = STEP_B;
						state->dataByte = dataByte;
						return outByte - ( unsigned char* )data;
					}
					fragment = base64_decodeValue( *encoded++ );
				} while ( fragment < 0 );
				if ( dataMaxSize-- < 1 )
					return BUFFER_OVERFLOW;
				dataByte |= ( fragment & 0x030 ) >> 4;
				*outByte++ = dataByte;
				dataByte = ( fragment & 0x00f ) << 4;
			case STEP_C:
				do {
					if ( encoded == encodedEnd )	{
						state->step = STEP_C;
						state->dataByte = dataByte;
						return outByte - ( unsigned char* )data;
					}
					fragment = base64_decodeValue( *encoded++ );
				} while ( fragment < 0 );
				if ( dataMaxSize-- < 1 )
					return BUFFER_OVERFLOW;
				dataByte |= ( fragment & 0x03c ) >> 2;
				*outByte++ = dataByte;
				dataByte = ( fragment & 0x003 ) << 6;
			case STEP_D:
				do {
					if ( encoded == encodedEnd )	{
						state->step = STEP_D;
						state->dataByte = dataByte;
						return outByte - ( unsigned char* )data;
					}
					fragment = base64_decodeValue( *encoded++ );
				} while ( fragment < 0 );
				if ( dataMaxSize-- < 1 )
					return BUFFER_OVERFLOW;
				dataByte |= ( fragment & 0x03f );
				*outByte++ = dataByte;
		}
	}
	/* it should not reach this point */
	return outByte - ( unsigned char* )data;
}


int base64_decode( const char* encoded, size_t encodedSize, void* data, size_t dataMaxSize )
{
	const char* encodedEnd;
	unsigned char dataByte;
	unsigned char* outByte;
	int fragment;

	encodedEnd = encoded + encodedSize;
	outByte = ( unsigned char* )data;

	for ( ; ; )	{
		do {
			if ( encoded == encodedEnd )
				return outByte - ( unsigned char* )data;
			fragment = base64_decodeValue( *encoded++ );
		} while ( fragment < 0 );
		dataByte = ( fragment & 0x03f ) << 2;

		do {
			if ( encoded == encodedEnd )
				return outByte - ( unsigned char* )data;
			fragment = base64_decodeValue( *encoded++ );
		} while ( fragment < 0 );
		if ( dataMaxSize-- < 1 )
			return BUFFER_OVERFLOW;
		dataByte |= ( fragment & 0x030 ) >> 4;
		*outByte++ = dataByte;
		dataByte = ( fragment & 0x00f ) << 4;

		do {
			if ( encoded == encodedEnd )
				return outByte - ( unsigned char* )data;
			fragment = base64_decodeValue( *encoded++ );
		} while ( fragment < 0 );
		if ( dataMaxSize-- < 1 )
			return BUFFER_OVERFLOW;
		dataByte |= ( fragment & 0x03c ) >> 2;
		*outByte++ = dataByte;
		dataByte = ( fragment & 0x003 ) << 6;

		do {
			if ( encoded == encodedEnd )
				return outByte - ( unsigned char* )data;
			fragment = base64_decodeValue( *encoded++ );
		} while ( fragment < 0 );
		if ( dataMaxSize-- < 1 )
			return BUFFER_OVERFLOW;
		dataByte |= ( fragment & 0x03f );
		*outByte++ = dataByte;
	}
}
