/*
 * SHA-1 in C
 * By Steve Reid <steve@edmweb.com>
 * 100% Public Domain
 *
 * Modified for Wolframe by the Wolframe Team
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _SHA1_H_INCLUDED
#define _SHA1_H_INCLUDED

#include <stddef.h>
#ifdef _WIN32
#if _MSC_VER >= 1600
#include <stdint.h>
#else
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;
#endif
#else
#include <stdint.h>
#endif

#define SHA1_DIGEST_SIZE	( 160 / 8 )
#define SHA1_BLOCK_SIZE		( 512 / 8 )

#ifdef __cplusplus
extern "C" {
#endif

typedef struct	{
	uint32_t	state[ 5 ];
	uint64_t	count;
	unsigned char	buffer[ SHA1_BLOCK_SIZE ];
} sha1_ctx;

void sha1_init( sha1_ctx *ctx );
void sha1_update( sha1_ctx *ctx, const void *data, size_t size );
void sha1_final( sha1_ctx *ctx, unsigned char *digest );
void sha1( const void *data, size_t size, unsigned char *digest );

#ifdef __cplusplus
}
#endif

#endif /* _SHA1_H_INCLUDED */

