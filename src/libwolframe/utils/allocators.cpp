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
///\brief Implementation for special purpose memory allocators
///\file types/allocators.cpp
#include "utils/allocators.hpp"
#include <stdexcept>
#include <cstring>
#include <cstdlib>

using namespace _Wolframe;
using namespace _Wolframe::utils;

class GreedySmallChunkAllocator::MemChunk
{
public:
	enum {bufsize=(1<<15)};

	explicit MemChunk( std::size_t nn=bufsize) :m_next(0),m_pos(0),m_bufsize(nn<(std::size_t)bufsize?(std::size_t)bufsize:nn)
	{
		m_buf = (char*)std::calloc( m_bufsize, sizeof(char));
		if (!m_buf) throw std::bad_alloc();
	}

	~MemChunk()
	{
		std::free( m_buf);
	}

	void* alloc( std::size_t nn)
	{
		if (m_pos + nn < m_bufsize)
		{
			char* rt;
			rt = m_buf + m_pos;
			m_pos += nn;
			return (void*)rt;
		}
		else
		{
			return 0;
		}
	}

	MemChunk* next() const
	{
		return m_next;
	}

	void link( MemChunk* nxt)
	{
		if (m_next) throw std::logic_error( "lost memory chunk");
		m_next = nxt;
	}

private:
	MemChunk* m_next;
	std::size_t m_pos;
	std::size_t m_bufsize;
	char* m_buf;
};


GreedySmallChunkAllocator::GreedySmallChunkAllocator()
	:m_chunk(0){}

GreedySmallChunkAllocator::~GreedySmallChunkAllocator()
{
	if (m_chunk)
	{
		MemChunk* curr = m_chunk;
		MemChunk* next;
		do
		{
			next = curr->next();
			delete curr;
			curr = next;
		}
		while (next);
	}
}

void* GreedySmallChunkAllocator::alloc( std::size_t nn)
{
	void* rt;
	if (!m_chunk || !(rt = m_chunk->alloc( nn)))
	{
		MemChunk* chk = new MemChunk( nn);
		rt = chk->alloc( nn);
		chk->link( m_chunk);
		m_chunk = chk;
	}
	return rt;
}


ArrayDoublingAllocator::ArrayDoublingAllocator()
	:m_ar(0)
	,m_size(InitBlockSize)
	,m_pos(0)
{
	m_ar = (char*)std::calloc( m_size, sizeof(char));
	if (!m_ar) throw std::bad_alloc();
}

ArrayDoublingAllocator::~ArrayDoublingAllocator()
{
	std::free( m_ar);
}

std::size_t ArrayDoublingAllocator::alloc( std::size_t nof)
{
	std::size_t rt = m_pos;
	if (m_pos + nof > m_size)
	{
		std::size_t mm = m_size;
		do
		{
			mm *= 2;
			if (m_pos + nof < m_pos) throw std::bad_alloc();
			if (mm <= m_size) throw std::bad_alloc();
		}
		while (m_pos + nof > mm);

		char* pp = (char*)std::realloc( m_ar, mm);
		if (!pp) throw std::bad_alloc();
		m_ar = pp;
		std::memset( m_ar + m_size, 0, m_size * sizeof(char));
		m_size = mm;
	}
	m_pos += nof;
	return rt;
}



