/*
cencoder.c - c source to a base64 encoding algorithm implementation

This is part of the libb64 project, and has been placed in the public domain.
For details, see http://sourceforge.net/projects/libb64
*/

#include <base64.h>

void base64_init_encodestate( base64_EncodeState* state, unsigned short lineLength )
{
	state->step = STEP_0;
	state->result = 0;
	state->stepCount = 0;
	state->lineLength = lineLength;
}

static inline char base64_encodeValue( unsigned char value )
{
	static const char* encoding = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	if ( value > 63 )
		return '=';
	return encoding[ value ];
}

int base64_encode_block(const unsigned char* plaintext_in, int length_in, char* code_out, base64_EncodeState* state )
{
	const unsigned char* plainchar = plaintext_in;
	const unsigned char* const plaintextend = plaintext_in + length_in;
	char* codechar = code_out;
	char result;
	char fragment;

	result = state ->result;

	switch (state ->step)
	{
		while (1)
		{
	case STEP_0:
			if (plainchar == plaintextend)
			{
				state ->result = result;
				state ->step = STEP_0;
				return codechar - code_out;
			}
			fragment = *plainchar++;
			result = (fragment & 0x0fc) >> 2;
			*codechar++ = base64_encodeValue(result);
			result = (fragment & 0x003) << 4;
	case STEP_1:
			if (plainchar == plaintextend)
			{
				state ->result = result;
				state ->step = STEP_1;
				return codechar - code_out;
			}
			fragment = *plainchar++;
			result |= (fragment & 0x0f0) >> 4;
			*codechar++ = base64_encodeValue(result);
			result = (fragment & 0x00f) << 2;
	case STEP_2:
			if (plainchar == plaintextend)
			{
				state ->result = result;
				state ->step = STEP_2;
				return codechar - code_out;
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
	return codechar - code_out;
}

int base64_encodeEnd( char* code_out, base64_EncodeState* state )
{
	char* codechar = code_out;

	switch (state ->step)
	{
	case STEP_1:
		*codechar++ = base64_encodeValue(state ->result);
		*codechar++ = '=';
		*codechar++ = '=';
		break;
	case STEP_2:
		*codechar++ = base64_encodeValue(state ->result);
		*codechar++ = '=';
		break;
	case STEP_0:
		break;
	}
	*codechar++ = '\n';

	return codechar - code_out;
}

