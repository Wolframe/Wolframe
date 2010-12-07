/**
 * @defgroup hex2byte Convertion between ASCII hex string and data block
 * @ingroup miscFunctions
 * @{
 */

/**
 * @file byte2hex.c
 * @brief Implementation of conversion between ASCII hex string and block of data
 * @author Mihai Barbos <mihai.barbos@gmail.com>
 */

#include "byte2hex.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>


static char	hexDigit[16] = { '0', '1', '2', '3', '4', '5', '6', '7',
				'8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };


static unsigned char hDigit2byte( int hex )
{
	switch( hex )	{
	    case '0':
		    return( 0x0 );
	    case '1':
		    return( 0x1 );
	    case '2':
		    return( 0x2 );
	    case '3':
		    return( 0x3 );
	    case '4':
		    return( 0x4 );
	    case '5':
		    return( 0x5 );
	    case '6':
		    return( 0x6 );
	    case '7':
		    return( 0x7 );
	    case '8':
		    return( 0x8 );
	    case '9':
		    return( 0x9 );
	    case 'A':
		    return( 0xA );
	    case 'B':
		    return( 0xB );
	    case 'C':
		    return( 0xC );
	    case 'D':
		    return( 0xD );
	    case 'E':
		    return( 0xE );
	    case 'F':
		    return( 0xF );
	    default:
		    return( 0 );
	}
}


/**
 * @brief	Get the ASCII hex string for a data block
 *
 * @param	data	pointer to the data block
 * @param	size	size of the data block
 * @param	outStr	pointer to the buffer for the output string
 * @param	outSize	size of the output buffer
 *
 * @return	pointer to the output string if successful
 *		or NULL if the output buffer is not large enough
 */

char *byte2hex( const unsigned char *data, size_t size, char *outStr, size_t outSize )
{
	size_t	i;
	char	*p;

	if ( outSize < 2 * size + 1 )
		return( NULL );

	p = outStr;
	for ( i = 0; i < size; i++ )	{
		*p++ = hexDigit[( *data ) >> 4 ];
		*p++ = hexDigit[( *data ) & 0x0f];
		data++;
	}
	*p = 0;
	return( outStr );
}


/**
 * @brief	Get the data block from the ASCII hex string
 *
 * @param	hexStr	pointer to the ASCII hex string
 * @param	outData	pointer to the buffer for the output data
 * @param	outSize	size of the output buffer
 *
 * @return	pointer to the output buffer if successful
 *		or NULL if the output buffer is not large enough
 */
unsigned char *hex2byte( const char *hexStr, unsigned char *outData, size_t outSize )
{
	size_t		len;
	unsigned char	*data = outData;

	if (( len = strlen( hexStr )) > outSize * 2 )
		return( NULL );
	if ( len % 2 )	{
		*data = hDigit2byte( toupper( *hexStr ));
		data++, hexStr++;
	}
	while( *hexStr )	{
		*data = (unsigned char)( hDigit2byte( toupper( *hexStr )) << 4 );
		hexStr++;
		*data |= hDigit2byte( toupper( *hexStr ));
		data++, hexStr++;
	}
	return( outData );
}


/** @} *//* hex2byte */

