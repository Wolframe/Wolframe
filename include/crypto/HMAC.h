/************************************************************************

 Copyright (C) 2011 - 2014 Project Wolframe.
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
/*
 *  Standard HMAC-SHA1 / HMAC-SHA256
 */

#ifndef _HMAC_H_INCLUDED
#define _HMAC_H_INCLUDED

#include "crypto/sha1.h"
#include "crypto/sha2.h"

#define	HMAC_SHA1_HASH_SIZE		SHA1_DIGEST_SIZE
#define	HMAC_SHA256_HASH_SIZE		SHA256_DIGEST_SIZE

#ifdef __cplusplus
extern "C" {
#endif

void hmac_sha1( const unsigned char* key, size_t keySize,
		const unsigned char* msg, size_t msgSize,
		unsigned char hash[] );

void hmac_sha256( const unsigned char* key, size_t keySize,
		  const unsigned char* msg, size_t msgSize,
		  unsigned char hash[] );

#ifdef __cplusplus
}
#endif

#endif // _HMAC_H_INCLUDED
