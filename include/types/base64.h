/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
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
#define INVALID_BUFFER		-2
#define INVALID_DATA		-3

/* enconding */

/**
 * @brief Encoder state structure
 */
typedef struct	{
	unsigned char	carryBytes[ 3 ];
	unsigned short	bytesLeft;
	unsigned short	lineLength;
	unsigned short	lineSize;
	unsigned char	newLinePending;
} base64_EncodeState;

/**
 * @brief Initialize a base64 encoder state structure
 *
 * @param state		pointer to the decoder state structure
 * @param lineLength	length of the line in the output string
 *			(distance between 2 consecutive \n)
 */
void base64_initEncodeState( base64_EncodeState* state, unsigned short lineLength );

/**
 * @brief Reset a base64 encoder state structure
 *
 * @param state		pointer to the decoder state structure
 * @note the structure should have been initialized before by a call
 *	 to a base64_initEncodeState(..)
 */
void base64_resetEncodeState( base64_EncodeState* state );

int base64_encodeChunk( base64_EncodeState* state, const void* data, size_t dataSize,
			char* encoded, size_t encodedMaxSize );

/**
 * @brief Encode a the final  block of binary data into base64
 *
 * @param state			pointer to the encoder state structure
 * @param encoded		pointer to the output buffer for the data
 * @param encodedMaxSize	size of the output buffer
 *
 * @return	length of the output string if successful
 *                    (not counting the zero termination)
 *		-1 if the output buffer is not large enough
 *
 * @note the string will be zero terminated, so allocate a buffer
 *       long enough to hold the 0 termination also.
 */
int base64_encodeEndChunk( base64_EncodeState* state, char* encoded, size_t encodedMaxSize );


size_t base64_encodedSize( size_t dataSize, unsigned short lineLength );

size_t base64_encodedChunkSize( base64_EncodeState* state, size_t dataSize );

/**
 * @brief Encode a block of binary data into base64
 *
 * @param data			string of the base64 encoded data
 *				(not necessary zero terminated)
 * @param dataSize		length of the string to decode
 * @param encoded		pointer to the output buffer for the data
 * @param encodedMaxSize	size of the output buffer
 * @param lineLength		size of the output buffer
 *
 * @return	length of the output string if successful
 *                    (not counting the zero termination)
 *		-1 if the output buffer is not large enough
 *
 * @note the string will be zero terminated, so allocate a buffer
 *       long enough to hold the final 0 also. A value of base64_encodedSize(..) + 1
 *       should be enough
 */
int base64_encode( const void* data, size_t dataSize,
		   char* encoded, size_t encodedMaxSize, unsigned short lineLength );

/* decoding */

typedef enum	{
	STEP_A,
	STEP_B,
	STEP_C,
	STEP_D
} base64_DecodeStep;


/**
 * @brief Decoder state structure
 */
typedef struct	{
	base64_DecodeStep	step;
	unsigned char		dataByte;
} base64_DecodeState;

/**
 * @brief Initialize a base64 decoder state structure
 *
 * @param state	pointer to the decoder state structure
 */
void base64_initDecodeState( base64_DecodeState* state );

int base64_decodeChunk( base64_DecodeState* state, const char* encoded, size_t encodedSize,
			void* data, size_t dataMaxSize );

/**
 * @brief Decode a base64 string into a data block
 *
 * @param encoded	string of the base64 encoded data
 *			(not necessary zero terminated)
 * @param encodedSize	length of the string to decode
 * @param data		pointer to the output buffer for the data
 * @param dataMaxSize	size of the output buffer
 *
 * @return	length of the output if successful
 *		-1 if the output buffer is not large enough
 */
int base64_decode( const char* encoded, size_t encodedSize, void* data, size_t dataMaxSize );

#ifdef __cplusplus
}
#endif

#endif /* _BASE64_H_INCLUDED */
