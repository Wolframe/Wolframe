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
 * b64encode.c - base64 encoding implementation
 */

#include <assert.h>
#include "types/base64.h"

static const char* base64Table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static void encodeBytes( const unsigned char *data, char *str )
{
	*str++ = base64Table[ data[ 0 ] >> 2 ];
	*str++ = base64Table[(( data[ 0 ] << 4 ) | ( data[ 1 ] >> 4 )) & 0x3f ];
	*str++ = base64Table[(( data[ 1 ] << 2 ) | ( data[ 2 ] >> 6 )) & 0x3f ];
	*str = base64Table[ data[ 2 ] & 0x3f ];
}

static int encodeEndBytes( const unsigned char *data, size_t dataSize,
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
	return 0; //PF: should never happen
}

static unsigned short correctedLineLength( unsigned short length )
{
	unsigned short corrected;

	if ( length == 0 )
		corrected = 0;
	else	{
		corrected = (( length + 2 ) / 4 ) * 4;
		if ( !corrected )
			corrected = 4;
	}
	return corrected;
}

void base64_initEncodeState( base64_EncodeState* state, unsigned short lineLength )
{
	state->bytesLeft = 0;
	state->lineLength = correctedLineLength( lineLength );
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
	size_t encodedSize;

	encodedSize = (( dataSize + 2 ) / 3 ) * 4;
	lineLength = correctedLineLength( lineLength );
	if ( lineLength && encodedSize )
		encodedSize += ( encodedSize + lineLength - 1 ) / lineLength - 1;

	return encodedSize;
}


size_t base64_encodedChunkSize( base64_EncodeState* state, size_t dataSize )
{
	size_t encodedSize;
	size_t	bytesToEncode;

	encodedSize = 0;
	bytesToEncode = dataSize;

	/* Start with data remaining from the previous chunk ignoring the pending new line
	 */
	if ( state->bytesLeft )	{
		if ( bytesToEncode + state->bytesLeft >= 3 )	{
			bytesToEncode -= 3 - state->bytesLeft;
			encodedSize += 4;
		}
		else
			bytesToEncode = 0;
	}


	/* Count the triplets in this chunk
	 */
	encodedSize += ( bytesToEncode / 3 ) * 4;

	/* Count the new lines and take into account the pending new line
	 * For a row which ends on the border of the line, the '\n' is not
	 * counted unless there is data left for the next chunk
	 */
	if ( state->lineLength )	{
		if ( encodedSize )	{
			if ( state->newLinePending )
				encodedSize += ( encodedSize - ( bytesToEncode % 3 ? 0 : 1 ))
						/ state->lineLength + 1;
			else
				encodedSize += ( state->lineSize + encodedSize - ( bytesToEncode % 3 ? 0 : 1 ))
						/ state->lineLength;
		}
		else	{
			if ( state->newLinePending && bytesToEncode )
				encodedSize++;
		}
	}

	return encodedSize;
}

int base64_encodeChunk( base64_EncodeState* state, const void* data, size_t dataSize,
			char* encoded, size_t encodedMaxSize )
{
	const unsigned char *bytes;
	char *output;
	size_t	encodedSize;

	encodedSize = base64_encodedChunkSize( state, dataSize );
	if ( encodedMaxSize < encodedSize )
		return BASE64_BUFFER_OVERFLOW;

	bytes = ( const unsigned char *)data;
	output = encoded;

	if ( state->newLinePending && dataSize )	{
		*output++ = '\n';
		state->newLinePending = 0;
		state->lineSize = 0;
	}

	switch ( state->bytesLeft )	{
		case 0:
			break;
		case 1:
			if ( dataSize >= 2 )	{
				state->carryBytes[ 1 ] = *bytes++;
				state->carryBytes[ 2 ] = *bytes++;
				dataSize -= 2;
				encodeBytes( state->carryBytes, output );
				state->bytesLeft = 0;
				output += 4;
				if ( state->lineLength )	{
					state->lineSize += 4;
					if ( state->lineSize >= state->lineLength )	{
						if ( dataSize )	{
							*output++ = '\n';
							state->lineSize = 0;
						}
						else
							state->newLinePending = 1;
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
				state->carryBytes[ 2 ] = *bytes++;
				dataSize--;
				encodeBytes( state->carryBytes, output );
				state->bytesLeft = 0;
				output += 4;
				if ( state->lineLength )	{
					state->lineSize += 4;
					if ( state->lineSize >= state->lineLength )	{
						if ( dataSize )	{
							*output++ = '\n';
							state->lineSize = 0;
						}
						else
							state->newLinePending = 1;
					}
				}
			}
	}

	while ( dataSize >= 3 )	{
		encodeBytes( bytes, output );
		bytes += 3;
		output += 4;
		dataSize -= 3;
		if ( state->lineLength )	{
			state->lineSize += 4;
			if ( state->lineSize >= state->lineLength )	{
				if ( dataSize )	{
					*output++ = '\n';
					state->lineSize = 0;
				}
				else
					state->newLinePending = 1;
			}
		}
	}

	switch( dataSize )	{
		case 0:
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
	assert( encodedSize == ( size_t )( output - encoded ));
	return output - encoded;
}

int base64_encodeEndChunk( base64_EncodeState* state, char* encoded, size_t encodedMaxSize )
{
	if ( state->bytesLeft )	{
		if ( state->newLinePending )	{
			if ( encodedMaxSize < 6 )
				return BASE64_BUFFER_OVERFLOW;
			*encoded++ = '\n';
			encodeEndBytes( state->carryBytes, state->bytesLeft, encoded );
			encoded[ 4 ] = 0;
			return 5;
		}
		else	{
			if ( encodedMaxSize < 5 )
				return BASE64_BUFFER_OVERFLOW;
			encodeEndBytes( state->carryBytes, state->bytesLeft, encoded );
			encoded[ 4 ] = 0;
			return 4;
		}
	}
	else	{
		if ( encodedMaxSize < 1 )
			return BASE64_BUFFER_OVERFLOW;
		*encoded = 0;
		return 0;
	}
}


int base64_encode( const void* data, size_t dataSize,
		   char* encoded, size_t encodedMaxSize, unsigned short lineLength )
{
	const unsigned char *bytes;
	char *output;
	unsigned short lineSize;

	size_t encodedSize;
	encodedSize = base64_encodedSize( dataSize, lineLength );
	if ( encodedMaxSize <= encodedSize )
		return BASE64_BUFFER_OVERFLOW;

	bytes = ( const unsigned char *)data;
	output = encoded;
	lineSize = 0;

	lineLength = correctedLineLength( lineLength );

	while ( dataSize >= 3 )	{
		encodeBytes( bytes, output );
		bytes += 3;
		output += 4;
		dataSize -= 3;
		if ( lineLength )	{
			lineSize += 4;
			if ( lineSize >= lineLength && dataSize )	{
				*output++ = '\n';
				lineSize = 0;
			}
		}
	}

	switch( dataSize )	{
		case 0:
			break;
		case 1:
		case 2:
			encodeEndBytes( bytes, dataSize, output );
			output += 4;
	}
	*output = 0;
	assert( encodedSize == ( size_t )( output - encoded ));
	return output - encoded;
}

