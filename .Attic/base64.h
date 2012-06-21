/*
 base64.h - c header for base64 functions

 Adapted from the libb64 project (http://sourceforge.net/projects/libb64)
 for the Wolframe project
*/

#ifndef _BASE64_H_INCLUDED
#define _BASE64_H_INCLUDED

#ifdef	__cplusplus
extern "C" {
#endif


typedef enum
{
	step_a, step_b, step_c, step_d
} base64_decodestep;

typedef struct
{
	base64_decodestep step;
	char plainchar;
} base64_decodestate;

void base64_init_decodestate(base64_decodestate* state_in);

int base64_decode_value(char value_in);

int base64_decode_block(const char* code_in, const int length_in, char* plaintext_out, base64_decodestate* state_in);


typedef enum
{
	step_A, step_B, step_C
} base64_encodestep;

typedef struct
{
	base64_encodestep step;
	char result;
	int stepcount;
} base64_encodestate;

void base64_init_encodestate(base64_encodestate* state_in);

char base64_encode_value(char value_in);

int base64_encode_block(const char* plaintext_in, int length_in, char* code_out, base64_encodestate* state_in);

int base64_encode_blockend(char* code_out, base64_encodestate* state_in);


#ifdef __cplusplus
}
#endif

#endif /* _BASE64_H_INCLUDED */
