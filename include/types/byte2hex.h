/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
 All rights reserved.

 This file is part of Project Wolframe.

 Commercial Usage
    Licensees holding valid Project Wolframe Commercial licenses may
    use this file in accordance with the Project Wolframe
    Commercial License Agreement provided with the Software or,
    alternatively, in accordance with the terms contained
    in a written agreement between the licensee and Project Wolframe.

 GNU General Public License Usage
    Alternatively, you can redistribute this file and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Wolframe is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
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

/**
 * @brief	Get the ASCII hex string for a data block
 *
 * @param	data	pointer to the data block
 * @param	size	size of the data block
 * @param	outStr	pointer to the buffer for the output string
 * @param	outSize	size of the output buffer
 *
 * @return	length of the output if successful not counting the final 0
 *		-1 if the output buffer is not large enough
 */
int byte2hex( const unsigned char *data, size_t size, char *outStr, size_t outSize );

/**
 * @brief	Get the data block from the ASCII hex string
 *
 * @param	hexStr	pointer to the ASCII hex string
 * @param	outData	pointer to the buffer for the output data
 * @param	outSize	size of the output buffer
 *
 * @return	length of the output if successful
 *		-1 if the output buffer is not large enough
 *		-2 if the string has invalid hex characters
 */
int hex2byte( const char *hexStr, unsigned char *outData, size_t outSize );

#ifdef __cplusplus
}
#endif

#endif	/* _BYTE2HEX_H_INCLUDED */

