/*
cencoder.c - c source to a base64 encoding algorithm implementation

This is part of the libb64 project, and has been placed in the public domain.
For details, see http://sourceforge.net/projects/libb64
*/

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

static inline char base64_encodeValue( unsigned char value )
{
	if ( value > 63 )
		return '=';
	return encoding[ value ];
}

int base64_encodeBlock( base64_EncodeState* state, const unsigned char* plain,
			size_t plainLength, char* encoded )
{
	const unsigned char* plainchar = plain;
	const unsigned char* const plaintextend = plain + plainLength;
	char* codechar = encoded;
	char result;
	char fragment;

	result = state->result;

	switch ( state->step )	{
		while ( 1 )	{
			case STEP_0:
				if (plainchar == dataEnd)	{
					state ->result = result;
					state ->step = STEP_0;
					return codechar - encoded;
				}
				fragment = *plainchar++;
				result = (fragment & 0x0fc) >> 2;
				*codechar++ = base64_encodeValue(result);
				result = (fragment & 0x003) << 4;
			case STEP_1:
				if (plainchar == dataEnd)	{
					state ->result = result;
					state ->step = STEP_1;
					return codechar - encoded;
				}
				fragment = *plainchar++;
				result |= (fragment & 0x0f0) >> 4;
				*codechar++ = base64_encodeValue(result);
				result = (fragment & 0x00f) << 2;
			case STEP_2:
				if (plainchar == dataEnd)	{
					state ->result = result;
					state ->step = STEP_2;
					return codechar - encoded;
				}
				fragment = *plainchar++;
				result |= (fragment & 0x0c0) >> 6;
				*codechar++ = base64_encodeValue(result);
				result  = (fragment & 0x03f) >> 0;
				*codechar++ = base64_encodeValue(result);

				++( state->stepCount );
				if ( state->stepCount == state->lineLength / 4 )	{
					*codechar++ = '\n';
					state->stepCount = 0;
				}
		}
	}
	/* control should not reach here */
	return codechar - encoded;
}

int base64_encodeEnd(base64_EncodeState* state, char* encodedEnd )
{
	char* encoded = encodedEnd;

	switch ( state->step )	{
		case STEP_1:
			*encoded++ = base64_encodeValue( state->result );
			*encoded++ = '=';
			*encoded++ = '=';
			break;
		case STEP_2:
			*encoded++ = base64_encodeValue( state->result );
			*encoded++ = '=';
			break;
		case STEP_0:
			break;
	}
	*encoded++ = '\n';

	return encoded - encodedEnd;
}

