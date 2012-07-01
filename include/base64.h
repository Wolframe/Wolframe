/************************************************************************

 Copyright (C) 2011 Project Wolframe.
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
 * base64.h - c header for base64 functions
 */

#ifndef _BASE64_H_INCLUDED
#define _BASE64_H_INCLUDED

#include <stddef.h>

#ifdef	__cplusplus
extern "C" {
#endif

#define DEFAULT_BASE64_LINE_LENGTH	76

#define BUFFER_OVERFLOW		-1
#define INVALID_CODE		-2

typedef struct	{
	unsigned char	carryBytes[ 3 ];
	unsigned short	bytesLeft;
	unsigned short	lineLength;
	unsigned short	lineSize;
	unsigned char	newLinePending;
} base64_EncodeState;

/**
 *
 */
void base64_initEncodeState( base64_EncodeState* state, unsigned short lineLength );

void base64_resetEncodeState( base64_EncodeState* state );

/**
 * Encodes a block of binary data into Base-64
 */
int base64_encode( const void* data, size_t dataSize,
		   char* encoded, size_t encodedMaxSize, unsigned short lineLength );

int base64_encodeChunk( base64_EncodeState* state, const void* data, size_t dataSize,
			char* encoded, size_t encodedMaxSize );

int base64_encodeEndChunk( base64_EncodeState* state, char* encoded, size_t encodedMaxSize );

size_t base64_encodedSize( size_t dataSize, unsigned short lineLength );


typedef enum	{
	STEP_A,
	STEP_B,
	STEP_C,
	STEP_D
} base64_DecodeStep;

typedef struct
{
	base64_DecodeStep	step;
	unsigned char		dataByte;
} base64_DecodeState;

void base64_initDecodeState( base64_DecodeState* state );

int base64_decodeChunk( base64_DecodeState* state, const char* encoded, size_t encodedSize,
			void* data, size_t dataMaxSize );

#ifdef __cplusplus
}
#endif

#endif /* _BASE64_H_INCLUDED */
