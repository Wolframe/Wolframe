/*
cencoder.c - c source to a base64 encoding algorithm implementation

This is part of the libb64 project, and has been placed in the public domain.
For details, see http://sourceforge.net/projects/libb64
*/

#include <base64.h>

const int CHARS_PER_LINE = 72;

void base64_init_encodestate(base64_EncodeState* state_in)
{
	state_in->step = STEP_0;
	state_in->result = 0;
	state_in->stepcount = 0;
}

static inline char base64_encode_value( unsigned char value_in )
{
	static const char* encoding = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	if (value_in > 63) return '=';
	return encoding[(int)value_in];
}

int base64_encode_block(const unsigned char* plaintext_in, int length_in, char* code_out, base64_EncodeState* state_in)
{
	const char* plainchar = plaintext_in;
	const char* const plaintextend = plaintext_in + length_in;
	char* codechar = code_out;
	char result;
	char fragment;

	result = state_in->result;

	switch (state_in->step)
	{
		while (1)
		{
	case STEP_0:
			if (plainchar == plaintextend)
			{
				state_in->result = result;
				state_in->step = STEP_0;
				return codechar - code_out;
			}
			fragment = *plainchar++;
			result = (fragment & 0x0fc) >> 2;
			*codechar++ = base64_encode_value(result);
			result = (fragment & 0x003) << 4;
	case STEP_1:
			if (plainchar == plaintextend)
			{
				state_in->result = result;
				state_in->step = STEP_1;
				return codechar - code_out;
			}
			fragment = *plainchar++;
			result |= (fragment & 0x0f0) >> 4;
			*codechar++ = base64_encode_value(result);
			result = (fragment & 0x00f) << 2;
	case STEP_2:
			if (plainchar == plaintextend)
			{
				state_in->result = result;
				state_in->step = STEP_2;
				return codechar - code_out;
			}
			fragment = *plainchar++;
			result |= (fragment & 0x0c0) >> 6;
			*codechar++ = base64_encode_value(result);
			result  = (fragment & 0x03f) >> 0;
			*codechar++ = base64_encode_value(result);

			++(state_in->stepcount);
			if (state_in->stepcount == CHARS_PER_LINE/4)
			{
				*codechar++ = '\n';
				state_in->stepcount = 0;
			}
		}
	}
	/* control should not reach here */
	return codechar - code_out;
}

int base64_encode_blockend(char* code_out, base64_EncodeState* state_in)
{
	char* codechar = code_out;

	switch (state_in->step)
	{
	case STEP_1:
		*codechar++ = base64_encode_value(state_in->result);
		*codechar++ = '=';
		*codechar++ = '=';
		break;
	case STEP_2:
		*codechar++ = base64_encode_value(state_in->result);
		*codechar++ = '=';
		break;
	case STEP_0:
		break;
	}
	*codechar++ = '\n';

	return codechar - code_out;
}

