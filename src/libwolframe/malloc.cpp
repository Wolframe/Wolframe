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
///\brief Alternative malloc that checks for memory corruption (only for testing)
///\file malloc.cpp
#include "types/malloc.hpp"
#ifndef _WIN32
#include <stdexcept>
#include <cstdlib>
#include <cstring>
#include <features.h>
#endif

#undef WOLFRAME_DEBUG_MALLOC
#ifdef WOLFRAME_DEBUG_MALLOC
#ifdef __GLIBC__
#include <glib.h>

///\brief Installs the malloc wrapper
///\remark Must be created before any malloc call
void init_wolframe_malloc()
{
	static GMemVTable memvtable;
	memset( &memvtable, 0, sizeof(memvtable));
	memvtable.malloc = &wolframe_malloc;
	memvtable.free = &wolframe_free;
	memvtable.realloc = &wolframe_realloc;
	g_mem_set_vtable( &memvtable);
}
#define W_MALLOC_N(N,E) g_malloc_n(N,E)
#define W_FREE(P) g_free(P)
#else
void init_wolframe_malloc()
{
	throw std::logic_error( "wolframe malloc not implemented: not supported on this platform (only with gcc/glibc)");
}
#define W_MALLOC_N(N,E) std::malloc(N*E)
#define W_FREE(P) std::free(P)
#endif
#else
void init_wolframe_malloc()
{
	throw std::logic_error( "wolframe malloc not implemented: define WOLFRAME_DEBUG_MALLOC not set");
}
#define W_MALLOC_N(N,E) std::malloc(N*E)
#define W_FREE(P) std::free(P)
#endif
#include <stdexcept>

struct MemChunkHeader
{
	size_t size;
	size_t ref;
	size_t chk;
};

void* wolframe_malloc( size_t size)
{
	MemChunkHeader* hdr;
	hdr = (MemChunkHeader*)W_MALLOC_N( size + sizeof(MemChunkHeader), 1);
	if (!hdr) return 0;
	hdr->size = size;
	hdr->ref = 1;
	hdr->chk = (size + sizeof(MemChunkHeader)) * 2654435761/*knuth's integer hash*/;
	return (void*)(hdr+1);
}

void* wolframe_calloc( size_t size, size_t esize)
{
	std::size_t mm = size * esize;
	if (mm < size || mm < esize) return 0;
	void* rt = wolframe_malloc( mm);
	std::memset( rt, 0, mm);
	return rt;
}

static void wolframe_deref_memhdr( MemChunkHeader* hdr)
{
	if (--hdr->ref != 0)
	{
		throw std::runtime_error( "free: invalid or double free detected");
	}
	if (hdr->chk != (hdr->size + sizeof(MemChunkHeader)) * 2654435761/*knuth's integer hash*/)
	{
		throw std::runtime_error( "free: check of memory block checksum failed");
	}
	hdr->chk--;
}

void wolframe_free( void* ptr)
{
	MemChunkHeader* hdr = (MemChunkHeader*)ptr - 1;
	wolframe_deref_memhdr( hdr);
	W_FREE( hdr);
}

void* wolframe_realloc( void* oldptr, size_t size)
{
	MemChunkHeader* hdr = (MemChunkHeader*)W_MALLOC_N( size + sizeof(MemChunkHeader), 1);
	if (!hdr) return 0;
	hdr->size = size;
	hdr->ref = 1;
	hdr->chk = (size + sizeof(MemChunkHeader)) * 2654435761/*knuth's integer hash*/;
	void* ptr = (void*)(hdr+1);
	if (oldptr)
	{
		MemChunkHeader* oldhdr = (MemChunkHeader*)oldptr - 1;
		std::size_t cpsize = (oldhdr->size > hdr->size)?hdr->size:oldhdr->size;
		std::memmove( ptr, oldptr, cpsize);
		wolframe_deref_memhdr( oldhdr);
		W_FREE( oldhdr);
	}
	return ptr;
}

