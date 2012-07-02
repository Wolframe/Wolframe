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
 * b64encode.c - base64 encoding implementation
 */

#include <assert.h>
#include "base64.h"

static const char* base64Table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

void base64_initEncodeState( base64_EncodeState* state, unsigned short lineLength )
{
	state->bytesLeft = 0;
	state->lineLength = (( lineLength + 2 ) / 4 ) * 4;
	state->lineSize = 0;
	state->newLinePending = 0;
}

void base64_resetEncodeState( base64_EncodeState* state )
{
	state->bytesLeft = 0;
	state->lineSize = 0;
	state->newLinePending = 0;
}

size_t base64_encodedSize( size_t dataSize, unsigned short lineLength )
{
	size_t encodedSize = (( dataSize + 2 ) / 3 ) * 4;
	lineLength = (( lineLength + 2 ) / 4 ) * 4;
	if ( lineLength )
		encodedSize += ( encodedSize + lineLength - 1 ) / lineLength - 1;

	return encodedSize;
}

static inline void encodeBytes( const unsigned char *data, char *str )
{
	*str++ = base64Table[ data[ 0 ] >> 2 ];
	*str++ = base64Table[(( data[ 0 ] << 4 ) | ( data[ 1 ] >> 4 )) & 0x3f ];
	*str++ = base64Table[(( data[ 1 ] << 2 ) | ( data[ 2 ] >> 6 )) & 0x3f ];
	*str = base64Table[ data[ 2 ] & 0x3f ];
}

static inline int encodeEndBytes( const unsigned char *data, size_t dataSize,
				  char *str )
{
	assert( dataSize < 3 );

	switch( dataSize )	{
		case 0:
			return 0;
		case 1:
			*str++ = base64Table[ data[ 0 ] >> 2 ];
			*str++ = base64Table[( data[ 0 ] << 4 ) & 0x3f ];
			*str++ = '=';
			*str = '=';
			return 4;
		case 2:
			*str++ = base64Table[ data[ 0 ] >> 2 ];
			*str++ = base64Table[(( data[ 0 ] << 4 ) | ( data[ 1 ] >> 4 )) & 0x3f ];
			*str++ = base64Table[( data[ 1 ] << 2 ) & 0x3f ];
			*str = '=';
			return 4;
	}
}

int base64_encodeChunk( base64_EncodeState* state, const void* data, size_t dataSize,
			char* encoded, size_t encodedMaxSize )
{
	const unsigned char *bytes;
	char *output;

	bytes = ( const unsigned char *)data;
	output = encoded;

	if ( state->newLinePending )	{
		if ( encodedMaxSize < 1 )
			return BUFFER_OVERFLOW;
		*output++ = '\n';
		encodedMaxSize--;
		state->newLinePending = 0;
	}

	switch ( state->bytesLeft )	{
		case 0:
			break;
		case 1:
			if ( dataSize >= 2 )	{
				if ( encodedMaxSize < 4 )
					return BUFFER_OVERFLOW;
				state->carryBytes[ 1 ] = *bytes++;
				state->carryBytes[ 2 ] = *bytes++;
				dataSize -= 2;
				encodeBytes( state->carryBytes, output );
				state->bytesLeft = 0;
				output += 4;
				encodedMaxSize -= 4;
				if ( state->lineLength )	{
					state->lineSize += 4;
					if ( state->lineSize >= state->lineLength )	{
						if ( encodedMaxSize < 1 )
							return BUFFER_OVERFLOW;
						*output++ = '\n';
						encodedMaxSize--;
						state->lineSize = 0;
					}
				}
			}
			else if ( dataSize == 1 )	{
				state->carryBytes[ 1 ] = *bytes++;
				state->bytesLeft++;
				dataSize--;
			}
			break;
		case 2:
			if ( dataSize >= 1 )	{
				if ( encodedMaxSize < 4 )
					return BUFFER_OVERFLOW;
				state->carryBytes[ 1 ] = *bytes++;
				dataSize--;
				encodeBytes( state->carryBytes, output );
				state->bytesLeft = 0;
				output += 4;
				encodedMaxSize -= 4;
				if ( state->lineLength )	{
					state->lineSize += 4;
					if ( state->lineSize >= state->lineLength )	{
						if ( encodedMaxSize < 1 )
							return BUFFER_OVERFLOW;
						*output++ = '\n';
						encodedMaxSize--;
						state->lineSize = 0;
					}
				}
			}
	}

	while ( dataSize >= 3 )	{
		if ( encodedMaxSize < 4 )
			return BUFFER_OVERFLOW;
		encodeBytes( bytes, output );
		bytes += 3;
		output += 4;
		dataSize -= 3;
		encodedMaxSize -= 4;
		if ( state->lineLength )	{
			state->lineSize += 4;
			if ( state->lineSize >= state->lineLength )	{
				if ( dataSize )	{
					if ( encodedMaxSize < 1 )
						return BUFFER_OVERFLOW;
					*output++ = '\n';
					encodedMaxSize--;
					state->lineSize = 0;
				}
				else
					state->newLinePending = 1;
			}
		}
	}

	switch( dataSize )	{
		case 0:
			state->bytesLeft = 0;
			break;
		case 1:
			state->carryBytes[ 0 ] = *bytes;
			state->bytesLeft = 1;
			break;
		case 2:
			state->carryBytes[ 0 ] = *bytes++;
			state->carryBytes[ 1 ] = *bytes;
			state->bytesLeft = 2;
	}
	return output - encoded;
}

int base64_encodeEndChunk( base64_EncodeState* state, char* encoded, size_t encodedMaxSize )
{
	if ( state->bytesLeft && encodedMaxSize < 4 )
		return BUFFER_OVERFLOW;
	return encodeEndBytes( state->carryBytes, state->bytesLeft, encoded );
}


int base64_encode( const void* data, size_t dataSize,
		   char* encoded, size_t encodedMaxSize, unsigned short lineLength )
{
	const unsigned char *bytes;
	char *output;
	unsigned short lineSize;

	bytes = ( const unsigned char *)data;
	output = encoded;
	lineSize = 0;

	lineLength = (( lineLength + 2 ) / 4 ) * 4;

	while ( dataSize >= 3 )	{
		if ( encodedMaxSize < 4 )
			return BUFFER_OVERFLOW;
		encodeBytes( bytes, output );
		bytes += 3;
		output += 4;
		dataSize -= 3;
		encodedMaxSize -= 4;
		if ( lineLength )	{
			lineSize += 4;
			if ( lineSize >= lineLength && dataSize )	{
				if ( encodedMaxSize < 1 )
					return BUFFER_OVERFLOW;
				*output++ = '\n';
				encodedMaxSize--;
				lineSize = 0;
			}
		}
	}

	switch( dataSize )	{
		case 0:
			break;
		case 1:
		case 2:
			if ( encodedMaxSize < 4 )
				return BUFFER_OVERFLOW;
			encodeEndBytes( bytes, dataSize, output );
			output += 4;
	}
	return output - encoded;
}

