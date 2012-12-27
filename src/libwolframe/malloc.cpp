/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
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
#include <cstddef>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <boost/thread/mutex.hpp>
#include <boost/lexical_cast.hpp>

struct MemChunkHeader
{
	std::size_t size;
	std::size_t idx;
	std::size_t adr;
};

struct MemPool
{
	enum {NofChunks=10000};

	void* alloc( std::size_t size)
	{
		MemChunkHeader* hdr;
		boost::mutex::scoped_lock lock(m_mutex);
		if (size <= 64)
		{
			if (size64B == 0) return 0;
			--size64B;
			hdr = ar64B[ size64B];
			hdr->size = size;
			hdr->idx = size64B;
			hdr->adr = 0;
			return hdr+1;
		}
		else if (size <= 1024)
		{
			if (size1K == 0) return 0;
			--size1K;
			hdr = ar1K[ size1K];
			hdr->size = size;
			hdr->idx = size1K;
			hdr->adr = 1;
			return hdr+1;
		}
		else if (size <= (32*1024))
		{
			if (size32K == 0) return 0;
			--size32K;
			hdr = ar32K[ size32K];
			hdr->size = size;
			hdr->idx = size32K;
			hdr->adr = 2;
			return hdr+1;
		}
		return 0;
	}

	void throwMemoryCorruptionException( int line, MemChunkHeader* hdr)
	{
		std::ostringstream rt;
		std::size_t idxrange = 0;
		MemChunkHeader* exhdr = 0;
		if (hdr->adr == 0)
		{
			idxrange = size64B;
			exhdr = ar64B[ hdr->idx];
		}
		else if (hdr->adr == 1)
		{
			idxrange = size1K;
			exhdr = ar1K[ hdr->idx];
		}
		else if (hdr->adr == 2)
		{
			idxrange = size32K;
			exhdr = ar32K[ hdr->idx];
		}
		rt << " memory header: size=" << hdr->size << " idx=" << hdr->idx << " adr=" << hdr->adr << " idxrange=" << idxrange << " ptr=" << (uintptr_t)hdr << " exhdr=" << (uintptr_t)exhdr << std::endl;
		throw std::runtime_error( std::string("memory corruption at line ") + boost::lexical_cast<std::string>(line) + rt.str());
	}

	void checkMemChunk( MemChunkHeader* hdr)
	{
		if (hdr->adr == 0)
		{
			if (hdr->size > 64 || hdr->idx < size64B) throwMemoryCorruptionException( __LINE__, hdr);
			if (hdr->idx > (sizeof(ar64B)/sizeof(ar64B[0]))) throwMemoryCorruptionException( __LINE__, hdr);
			if (hdr != ar64B[ hdr->idx]) throwMemoryCorruptionException( __LINE__, hdr);
		}
		else if (hdr->adr == 1)
		{
			if (hdr->size > (1024) || hdr->idx < size1K) throwMemoryCorruptionException( __LINE__, hdr);
			if (hdr->idx > (sizeof(ar1K)/sizeof(ar1K[0]))) throwMemoryCorruptionException( __LINE__, hdr);
			if (hdr != ar1K[ hdr->idx]) throwMemoryCorruptionException( __LINE__, hdr);
		}
		else if (hdr->adr == 2)
		{
			if (hdr->size > (32*1024) || hdr->idx < size32K) throwMemoryCorruptionException( __LINE__, hdr);
			if (hdr->idx > (sizeof(ar32K)/sizeof(ar32K[0]))) throwMemoryCorruptionException( __LINE__, hdr);
			if (hdr != ar32K[ hdr->idx]) throwMemoryCorruptionException( __LINE__, hdr);
		}
	}

	void free( void* ptr)
	{
		MemChunkHeader* hdr = (MemChunkHeader*)ptr - 1;
		boost::mutex::scoped_lock lock(m_mutex);
		checkMemChunk( hdr);

		if (hdr->adr == 0)
		{
			if (hdr->idx == size64B)
			{
				ar64B[ size64B++] = hdr;
			}
			else
			{
				//swap element sizeXX with hdr->idx:
				MemChunkHeader* tmp = ar64B[ size64B];
				ar64B[ size64B] = ar64B[ hdr->idx];
				ar64B[ hdr->idx] = tmp;
				tmp->idx = hdr->idx;
				//free element:
				size64B++;
			}
			hdr->idx = std::numeric_limits<std::size_t>::max();
		}
		else if (hdr->adr == 1)
		{
			if (hdr->idx == size1K)
			{
				ar1K[ size1K++] = hdr;
			}
			else
			{
				//swap element sizeXX with hdr->idx:
				MemChunkHeader* tmp = ar1K[ size1K];
				ar1K[ size1K] = ar1K[ hdr->idx];
				ar1K[ hdr->idx] = tmp;
				tmp->idx = hdr->idx;
				//free element:
				size1K++;
			}
			hdr->idx = std::numeric_limits<std::size_t>::max();
		}
		else if (hdr->adr == 2)
		{
			if (hdr->idx == size32K)
			{
				ar32K[ size32K++] = hdr;
			}
			else
			{
				//swap element sizeXX with hdr->idx:
				MemChunkHeader* tmp = ar32K[ size32K];
				ar32K[ size32K] = ar32K[ hdr->idx];
				ar32K[ hdr->idx] = tmp;
				tmp->idx = hdr->idx;
				//free element:
				size32K++;
			}
			hdr->idx = std::numeric_limits<std::size_t>::max();
		}
	}

	MemPool()
	{
		std::size_t ii;
		size64B = (sizeof(ar64B)/sizeof(ar64B[0]));
		for (ii=0; ii<size64B; ++ii)
		{
			ar64B[ii] = (MemChunkHeader*)std::malloc( sizeof(MemChunkHeader)+64);
		}
		size1K = (sizeof(ar1K)/sizeof(ar1K[0]));
		for (ii=0; ii<size1K; ++ii)
		{
			ar1K[ii] = (MemChunkHeader*)std::malloc( sizeof(MemChunkHeader)+1024);
		}
		size32K = (sizeof(ar32K)/sizeof(ar32K[0]));
		for (ii=0; ii<size32K; ++ii)
		{
			ar32K[ii] = (MemChunkHeader*)std::malloc( sizeof(MemChunkHeader)+32*1024);
		}
	}

	~MemPool()
	{
		std::size_t ii=0;
		for (ii=(sizeof(ar64B)/sizeof(ar64B[0])); ii>size64B; --ii)
		{
			checkMemChunk( ar64B[ii-1]);
		}
		for (ii=(sizeof(ar1K)/sizeof(ar1K[0])); ii>size1K; --ii)
		{
			checkMemChunk( ar1K[ii-1]);
		}
		for (ii=(sizeof(ar32K)/sizeof(ar32K[0])); ii>size32K; --ii)
		{
			checkMemChunk( ar32K[ii-1]);
		}
	}

private:
	MemChunkHeader* ar64B[NofChunks];
	std::size_t size64B;
	MemChunkHeader* ar1K[NofChunks];
	std::size_t size1K;
	MemChunkHeader* ar32K[NofChunks];
	std::size_t size32K;
	boost::mutex m_mutex;
};

static MemPool g_mempool;

void* _Wolframe::types::malloc( std::size_t size)
{
	return g_mempool.alloc( size);
}

void _Wolframe::types::free( void* ptr)
{
	g_mempool.free( ptr);
}


