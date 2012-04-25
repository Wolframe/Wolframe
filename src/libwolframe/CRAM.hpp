/************************************************************************

 Copyright (C) 2011 Project Wolframe.
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
//
// Challenge Response Authentication Mechanism
//

#ifndef _CRAM_HPP_INCLUDED
#define _CRAM_HPP_INCLUDED

static const size_t HASH_BLOCK_SIZE = 1024 / 8;
static const size_t HASH_DIGEST_SIZE = 512 / 8;
static const size_t PASSWORD_DIGEST_SIZE = 224 / 8;
static const size_t CRAM_CHALLENGE_SIZE = HASH_BLOCK_SIZE;
static const size_t CRAM_RESPONSE_SIZE = HASH_DIGEST_SIZE;

#define	PASSWORD_HASH_STRING_SIZE	( 2 * PASSWORD_HASH_SIZE + 1 )
#define	CRAM_CHALLENGE_STRING_SIZE	( 2 * CRAM_CHALLENGE_SIZE + 1 )
#define	CRAM_RESPONSE_STRING_SIZE	( 2 * CRAM_RESPONSE_SIZE + 1 )

#include <stdlib.h>

#ifdef	__cplusplus
extern "C" {
#endif

char *passwordHash( char *password, char *buffer, size_t size );

char *CRAMchallenge( char *buffer, size_t size );

char *CRAMresponse( const unsigned char *challenge, const char *passwordHash, char *buffer, size_t size );

#ifdef __cplusplus
}
#endif

#endif	/* _CRAM_HPP_INCLUDED */

