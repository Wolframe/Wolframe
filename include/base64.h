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

#define BUFFER_OVERFLOW		-1
#define INVALID_CODE		-2

typedef enum	{
	STEP_0,
	STEP_1,
	STEP_2
} base64_EncodeStep;

typedef struct	{
	base64_EncodeStep	step;
	unsigned char		result;
	int			stepCount;
	unsigned short		lineLength;
} base64_EncodeState;

/**
 *
 */
void base64_initEncodeState( base64_EncodeState* state, unsigned short lineLength );

void base64_resetEncodeState( base64_EncodeState* state );

/**
 * Encodes a block of binary data into Base-64
 */
int base64_encodeBlock( base64_EncodeState* state, const void* data, size_t dataSize,
			char* encoded, size_t encodedMaxSize );

int base64_encodeEnd( base64_EncodeState* state, char* encoded, size_t encodedMaxSize );


typedef enum	{
	STEP_A,
	STEP_B,
	STEP_C,
	STEP_D
} base64_DecodeStep;

typedef struct
{
	base64_DecodeStep	step;
	unsigned char		dataByte;
} base64_DecodeState;

void base64_initDecodeState( base64_DecodeState* state );

int base64_decodeBlock( base64_DecodeState* state, const char* encoded, size_t encodedSize,
			void* data, size_t dataMaxSize );

#ifdef __cplusplus
}
#endif

#endif /* _BASE64_H_INCLUDED */
