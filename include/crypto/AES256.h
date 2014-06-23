/*
*   Byte-oriented AES-256 implementation.
*   All lookup tables replaced with 'on the fly' calculations.
*
*   Copyright (c) 2007-2009 Ilya O. Levin, http://www.literatecode.com
*   Other contributors: Hal Finney
*
*   Permission to use, copy, modify, and distribute this software for any
*   purpose with or without fee is hereby granted, provided that the above
*   copyright notice and this permission notice appear in all copies.
*
*   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
*   WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
*   MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
*   ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
*   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
*   ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
*   OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*
*   Modified for Wolframe by the Wolframe Team (c) 2012 - 2014
*   The original code is available from http://www.literatecode.com/aes256
*/

#ifdef __cplusplus
extern "C" {
#endif

typedef struct	{
	unsigned char	key[ 32 ];
	unsigned char	enckey[ 32 ];
	unsigned char	deckey[ 32 ];
} AES256_context;


void AES256_init( AES256_context *ctx, unsigned char *key );
void AES256_done( AES256_context *ctx );
void AES256_encrypt_ECB( AES256_context *ctx, unsigned char *plaintext );
void AES256_decrypt_ECB( AES256_context *ctx, unsigned char *ciphertext );

#ifdef __cplusplus
}
#endif
