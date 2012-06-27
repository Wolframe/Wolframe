/*
cencoder.c - c source to a base64 encoding algorithm implementation

This is part of the libb64 project, and has been placed in the public domain.
For details, see http://sourceforge.net/projects/libb64
*/

#include <base64.h>

#define CRLF_SIZE	1

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

static inline char base64_encodeValue( unsigned char value )
{
	if ( value > 63 )
		return '=';
	return encoding[ value ];
}

int base64_encodeBlock( base64_EncodeState* state, const unsigned char* plain,
			size_t plainLength, char* encoded, size_t encodedMaxSize )
{
	const unsigned char* plainByte = plain;
	const unsigned char* const plainEnd = plain + plainLength;
	char* codeChar = encoded;
	char result;
	char fragment;

	result = state->result;

	switch ( state->step )	{
		while ( 1 )	{
			case STEP_0:
				if ( plainByte == plainEnd )	{
					state->result = result;
					state->step = STEP_0;
					return codeChar - encoded;
				}
				fragment = *plainByte++;
				result = ( fragment & 0x0fc ) >> 2;
				if ( encodedMaxSize-- <= 0 )
					return -1;
				*codeChar++ = base64_encodeValue( result );
				result = ( fragment & 0x003 ) << 4;
			case STEP_1:
				if ( plainByte == plainEnd )	{
					state->result = result;
					state->step = STEP_1;
					return codeChar - encoded;
				}
				fragment = *plainByte++;
				result |= ( fragment & 0x0f0 ) >> 4;
				if ( encodedMaxSize-- <= 0 )
					return -1;
				*codeChar++ = base64_encodeValue( result );
				result = ( fragment & 0x00f ) << 2;
			case STEP_2:
				if ( plainByte == plainEnd )	{
					state->result = result;
					state->step = STEP_2;
					return codeChar - encoded;
				}
				fragment = *plainByte++;
				result |= ( fragment & 0x0c0 ) >> 6;
				if ( encodedMaxSize-- <= 0 )
					return -1;
				*codeChar++ = base64_encodeValue( result );
				result  = ( fragment & 0x03f ) >> 0;
				if ( encodedMaxSize-- <= 0 )
					return -1;
				*codeChar++ = base64_encodeValue( result );

				++( state->stepCount );
				if ( state->stepCount == state->lineLength / 4 )	{
					if ( encodedMaxSize-- <= 0 )
						return -1;
					*codeChar++ = '\n';
					state->stepCount = 0;
				}
		}
	}
	/* control should not reach here */
	return codeChar - encoded;
}

int base64_encodeEnd( base64_EncodeState* state, char* encoded, size_t encodedMaxSize )
{
	char* codeByte = encoded;

	switch ( state->step )	{
		case STEP_1:
			if ( encodedMaxSize < 3 + CRLF_SIZE )
				return -1;
			*codeByte++ = base64_encodeValue( state->result );
			*codeByte++ = '=';
			*codeByte++ = '=';
			break;
		case STEP_2:
			if ( encodedMaxSize < 2 + CRLF_SIZE)
				return -1;
			*codeByte++ = base64_encodeValue( state->result );
			*codeByte++ = '=';
			break;
		case STEP_0:
			if ( encodedMaxSize < CRLF_SIZE )
				return -1;
			break;
	}
	*codeByte++ = '\n';

	return codeByte - encoded;
}

