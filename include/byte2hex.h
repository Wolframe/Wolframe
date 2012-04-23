/**
 * @file byte2hex.h
 * @brief Header file for conversion between ASCII hex string and data block
 * @author Mihai Barbos <mihai.barbos@gmail.com>
 */

/*
 * hex string to data block and back
 *
 */

#ifndef _BYTE2HEX_H_INCLUDED
#define _BYTE2HEX_H_INCLUDED


#include <stdlib.h>			/* for size_t */


#ifdef	__cplusplus
extern "C" {
#endif


char *byte2hex( const unsigned char *data, size_t size, char *outStr, size_t outSize );

unsigned char *hex2byte( const char *hexStr, unsigned char *outData, size_t outSize );


#ifdef __cplusplus
}
#endif

#endif	/* _BYTE2HEX_H_INCLUDED */

