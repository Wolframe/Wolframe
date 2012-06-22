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


typedef enum	{
	STEP_A,
	STEP_B,
	STEP_C,
	STEP_D
} base64_DecodeStep;

typedef struct
{
	base64_DecodeStep step;
	char plainchar;
} base64_DecodeState;

void base64_init_decodestate( base64_DecodeState* state );

int base64_decode_block(const char* code_in, const int length_in, unsigned char* plaintext_out, base64_DecodeState* state );


typedef enum	{
	STEP_0,
	STEP_1,
	STEP_2
} base64_EncodeStep;

typedef struct	{
	base64_EncodeStep	step;
	char			result;
	int			stepCount;
	unsigned short		lineLength;
} base64_EncodeState;

void base64_init_encodestate( base64_EncodeState* state, unsigned short lineLength );

int base64_encode_block(const unsigned char* plaintext_in, int length_in, char* code_out, base64_EncodeState* state );

int base64_encodeEnd( char* code_out, base64_EncodeState* state );


#ifdef __cplusplus
}
#endif

#endif /* _BASE64_H_INCLUDED */
