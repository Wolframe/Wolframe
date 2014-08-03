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
 * This is an OpenSSL-compatible implementation of the RSA Data Security, Inc.
 * MD5 Message-Digest Algorithm (RFC 1321).
 *
 * Homepage:
 * http://openwall.info/wiki/people/solar/software/public-domain-source-code/md5
 *
 * Author:
 * Alexander Peslyak, better known as Solar Designer <solar at openwall.com>
 *
 * This software was written by Alexander Peslyak in 2001.  No copyright is
 * claimed, and the software is hereby placed in the public domain.
 * In case this attempt to disclaim copyright and place the software in the
 * public domain is deemed null and void, then the software is
 * Copyright (c) 2001 Alexander Peslyak and it is hereby released to the
 * general public under the following terms:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted.
 *
 * There's ABSOLUTELY NO WARRANTY, express or implied.
 *
 * See md5.c for more information.
 *
 * Modified for Wolframe by the Wolframe Team (c) 2012 - 2014
 */

#ifndef _MD5_H_INCLUDED
#define _MD5_H_INCLUDED

#define MD5_DIGEST_SIZE	( 128 / 8 )

#ifdef __cplusplus
extern "C" {
#endif

/* Any 32-bit or wider unsigned integer data type will do */
typedef unsigned int md5_u32plus;

typedef struct {
	md5_u32plus lo, hi;
	md5_u32plus a, b, c, d;
	unsigned char buffer[64];
	md5_u32plus block[16];
} md5_ctx;

void md5_init( md5_ctx *ctx );
void md5_update( md5_ctx *ctx, const void *data, unsigned long size );
void md5_final( unsigned char *digest, md5_ctx *ctx );

void md5( const void *data, unsigned long size, unsigned char *hash );

#ifdef __cplusplus
}
#endif

#endif /* _MD5_H_INCLUDED */
