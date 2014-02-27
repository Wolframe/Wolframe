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
//\brief malloc wrapper that checks for memory corruption (only for testing)
//\file malloc.cpp
#include "utils/malloc.hpp"
#include <cstdlib>
#include <cstring>
#include <iostream>

#define SYSTEM_MALLOC(size)		std::malloc(size)
#define SYSTEM_FREE(ptr)		std::free(ptr)
#define SYSTEM_REALLOC(ptr,size)	std::realloc(ptr,size)

struct MemChunkHeader
{
	size_t _;	//< padding for satisfying context dependent memory alignment requirements (64bit integers on Solaris 32bit)
	size_t size;	//< size of memory block
	size_t ref;	//< number of references for detecting double frees
	size_t chk;	//< checksum
};

static void wolframe_deref_memhdr( MemChunkHeader* hdr)
{
	if (hdr->chk != (hdr->size + sizeof(MemChunkHeader)) * 2654435761U/*knuth's integer hash*/)
	{
		std::cout << "MEMCHECK ERROR (free): check of memory block checksum failed" << std::endl;
	}
	if (--hdr->ref != 0)
	{
		std::cout << "MEMCHECK ERROR (free): invalid or double free detected" << std::endl;
	}
	hdr->chk--;
}

static void wolframe_init_memhdr( MemChunkHeader* hdr, size_t size)
{
	hdr->_ = 0;
	hdr->size = size;
	hdr->ref = 1;
	hdr->chk = (size + sizeof(MemChunkHeader)) * 2654435761U/*knuth's integer hash*/;
}


void* wolframe_malloc( size_t size)
{
	if (!size) return 0;
	MemChunkHeader* hdr;
	hdr = (MemChunkHeader*)SYSTEM_MALLOC( size + sizeof(MemChunkHeader));
	if (!hdr) return 0;
	wolframe_init_memhdr( hdr, size);
	return (void*)(hdr+1);
}

void* wolframe_calloc( size_t size, size_t esize)
{
	if (!size) return 0;
	std::size_t mm = size * esize;
	if (mm < size || mm < esize) return 0;
	void* rt = wolframe_malloc( mm);
	std::memset( rt, 0, mm);
	return rt;
}

void wolframe_free( void* ptr)
{
	if (!ptr) return;
	MemChunkHeader* hdr = (MemChunkHeader*)ptr - 1;
	wolframe_deref_memhdr( hdr);
	SYSTEM_FREE( hdr);
}

void* wolframe_realloc( void* oldptr, size_t size)
{
	MemChunkHeader* hdr = (MemChunkHeader*)SYSTEM_MALLOC( size + sizeof(MemChunkHeader));
	if (!hdr) return 0;
	wolframe_init_memhdr( hdr, size);
	void* ptr = (void*)(hdr+1);
	if (oldptr)
	{
		MemChunkHeader* oldhdr = (MemChunkHeader*)oldptr - 1;
		std::size_t cpsize = (oldhdr->size > hdr->size)?hdr->size:oldhdr->size;
		std::memcpy( ptr, oldptr, cpsize);
		wolframe_deref_memhdr( oldhdr);
		SYSTEM_FREE( oldhdr);
	}
	return ptr;
}

