/*
cencoder.c - c source to a base64 encoding algorithm implementation

This is part of the libb64 project, and has been placed in the public domain.
For details, see http://sourceforge.net/projects/libb64
*/

#include <assert.h>
#include <base64.h>

static const char* encoding = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

void base64_initEncodeState( base64_EncodeState* state, unsigned short lineLength )
{
	state->step = STEP_0;
	state->result = 0;
	state->stepCount = 0;
	state->lineLength = lineLength;
}

void base64_resetEncodeState( base64_EncodeState* state )
{
	state->step = STEP_0;
	state->result = 0;
	state->stepCount = 0;
}

size_t base64_encodedSize( size_t dataSize, unsigned short lineLength )
{
	size_t encodedSize = (( dataSize + 2 ) / 3 ) * 4;
	if ( lineLength / 4 )	{
		lineLength = ( lineLength / 4 ) * 4;
		if ( encodedSize && ( encodedSize % lineLength ))
			encodedSize += encodedSize / lineLength;
		else	{
			if ( dataSize % 3 )
				encodedSize += encodedSize / lineLength - 1;
			else
				encodedSize += encodedSize / lineLength;
		}
	}
	return encodedSize;
}

static inline char base64_encodeValue( unsigned char value )
{
	if ( value > 63 )
		return '=';
	return encoding[ value ];
}

int base64_encodeBlock( base64_EncodeState* state, const void* data, size_t dataSize,
			char* encoded, size_t encodedMaxSize )
{
	const unsigned char* dataByte;
	const unsigned char* dataEnd;
	char* codeChar;
	unsigned char result;
	unsigned char fragment;

	// check if the output buffer is big enough
	size_t encodedSize = ( dataSize * 4 ) / 3;
	if ( state->lineLength / 4 )
		encodedSize += encodedSize / (( state->lineLength / 4 ) * 4 );

	if ( encodedMaxSize < encodedSize )
		return BUFFER_OVERFLOW;

	dataByte = ( const unsigned char* )data;
	dataEnd = ( const unsigned char* )data + dataSize;
	codeChar = encoded;

	result = state->result;

	switch ( state->step )	{
		while ( 1 )	{
			case STEP_0:
				if ( dataByte == dataEnd )	{
					state->result = result;
					state->step = STEP_0;
					break;
				}
				fragment = *dataByte++;
				result = ( fragment & 0x0fc ) >> 2;
				*codeChar++ = base64_encodeValue( result );
				result = ( fragment & 0x003 ) << 4;
			case STEP_1:
				if ( dataByte == dataEnd )	{
					state->result = result;
					state->step = STEP_1;
					break;
				}
				fragment = *dataByte++;
				result |= ( fragment & 0x0f0 ) >> 4;
				*codeChar++ = base64_encodeValue( result );
				result = ( fragment & 0x00f ) << 2;
			case STEP_2:
				if ( dataByte == dataEnd )	{
					state->result = result;
					state->step = STEP_2;
					break;
				}
				fragment = *dataByte++;
				result |= ( fragment & 0x0c0 ) >> 6;
				*codeChar++ = base64_encodeValue( result );
				result  = ( fragment & 0x03f ) >> 0;
				*codeChar++ = base64_encodeValue( result );

				++( state->stepCount );
				if ( state->lineLength && state->stepCount == state->lineLength / 4 )	{
					*codeChar++ = '\n';
					state->stepCount = 0;
				}
		}
	}
	assert( encodedSize == (size_t)( codeChar - encoded ));
	return codeChar - encoded;
}

int base64_encodeEnd( base64_EncodeState* state, char* encoded, size_t encodedMaxSize )
{
	char* codeByte = encoded;

	switch ( state->step )	{
		case STEP_1:
			if ( encodedMaxSize < 3 )
				return BUFFER_OVERFLOW;
			*codeByte++ = base64_encodeValue( state->result );
			*codeByte++ = '=';
			*codeByte++ = '=';
			break;
		case STEP_2:
			if ( encodedMaxSize < 2 )
				return BUFFER_OVERFLOW;
			*codeByte++ = base64_encodeValue( state->result );
			*codeByte++ = '=';
			break;
		case STEP_0:
			break;
	}

	return codeByte - encoded;
}

