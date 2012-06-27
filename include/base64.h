/*
 base64.h - c header for base64 functions

 Adapted from the libb64 project (http://sourceforge.net/projects/libb64)
 for the Wolframe project
*/

#ifndef _BASE64_H_INCLUDED
#define _BASE64_H_INCLUDED

#include <stddef.h>

#ifdef	__cplusplus
extern "C" {
#endif

#define DEFAULT_BASE64_LINE_LENGTH	76

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

void base64_initEncodeState( base64_EncodeState* state, unsigned short lineLength );

void base64_resetEncodeState( base64_EncodeState* state );

int base64_encodeBlock( base64_EncodeState* state, const unsigned char* plain,
			size_t plainLength, char* encoded );

int base64_encodeEnd( base64_EncodeState* state, char* encoded );


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

void base64_initDecodeState( base64_DecodeState* state );

int base64_decodeBlock( base64_DecodeState* state, const char* encoded,
			size_t codeLength, unsigned char* data );

#ifdef __cplusplus
}
#endif

#endif /* _BASE64_H_INCLUDED */
