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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file types_bignum.cpp
///\brief Implements a bignum type for the DDLs used for forms
#include "types/bignum.hpp"
#include <ctypes>
#include <malloc.h>
#include <stdlib.h>
#include <setjmp.h>
#include "boost/thread/thread.hpp"
//#include "boost/thread/mutex.hpp"
//#include "boost/thread/tss.hpp"
//#include "boost/scoped_ptr.hpp"

extern "C"
{
#include "gmp.h"
}

namespace {

struct MemBlock
{
	MemBlock* prev;
	MemBlock* next;
};

struct ThreadMemory
{
	MemBlock* head;
	jmp_buf env;

	Memory() :head(0){}
	~Memory()
	{
		MemBlock* pp = head;
		while (pp)
		{
			pp = head->next;
			std::free( head);
			head = pp;
		}
	}
};

static boost::thread_specific_ptr<ThreadMemory> g_threadMemory;

static ThreadMemory* init_threadMemory()
{
	try
	{
		g_threadMemory.reset( new ThreadMemory());
		return g_threadMemory.get();
	}
	catch (const std::exception& e)
	{
		return 0;
	}
}

static void releaseThreadMemory()
{
	g_threadMemory.reset();
}

static void threadMemoryUnlink( ThreadMemory* tm, MemBlock* mb)
{
	if (!tm->head || !mb) longjump( tm->env, __LINE__);
	if (tm->head == mb)
	{
		tm->head = mb->next;
	}
	if (mb->next)
	{
		mb->next->prev = mb->prev;
	}
	if (mb->prev)
	{
		mb->prev->next = mb->next;
	}
}

static void threadMemoryInsert( ThreadMemory* tm, MemBlock* mb)
{
	if (!mb) ::longjump( tm->env, __LINE__);
	mb->next = tm->head;
	mb->prev = 0;
	if (tm->head)
	{
		tm->head->prev = mb;
	}
	tm->head = mb;
}

static void* threadMemoryMalloc( std::size_t n)
{
	std::size_t size = n+sizeof(MemBlock);
	MemBlock* mb = std::malloc( size);
	ThreadMemory* tm = g_threadMemory.get();
	if (n >= size) ::longjump( tm->env, __LINE__);
	threadMemoryInsert( tm, mb);
	return (void*)(mb+1);
}

static void threadMemoryFree( void* ptr)
{
	ThreadMemory* tm = g_threadMemory.get();
	MemBlock* mb = (MemBlock*) ptr - 1;
	threadMemoryUnlink( tm, mb);
	std::free( mb);
}

static void* threadMemoryRealloc( void* ptr, std::size_t n)
{
	if (!ptr) return threadMemoryMalloc( n);
	std::size_t size = n+sizeof(MemBlock);
	MemBlock* mb = (MemBlock*) ptr - 1;
	ThreadMemory* tm = g_threadMemory.get();
	threadMemoryUnlink( tm, mb);
	if (n >= size) longjump( tm->env, __LINE__);
	MemBlock* mb = (MemBlock*)std::realloc( mb, size);
	if (!mb) longjump( tm->env, __LINE__);
	threadMemoryInsert( tm, mb);
	return (void*)(mb+1);
}

static void* GMP_alloc_func_ptr( size_t n)
{
	return threadMemoryMalloc( n);
}

static void* GMP_realloc_func_ptr( void* ptr, size_t /*old_size*/, size_t new_size)
{
	return threadMemoryRealloc( ptr, new_size);
}

static void* GMP_free_func_ptr( void* ptr, size_t /*old_size*/)
{
	return threadMemoryFree( ptr);
}

} //namespace

using namespace _Wolframe::types;

struct StaticInitializations
{
	StaticInitializations()
	{
		mp_set_memory_functions( &GMP_alloc_func_ptr, &GMP_realloc_func_ptr, &GMP_free_func_ptr);
	}
};
static StaticInitializations g_staticInitializations;


bool Bignum::enter()
{
	ThreadMemory* tm = init_threadMemory();
	if (!tm) return false;
	int st = setjump(tm->env);
	if (st)
	{
		releaseThreadMemory();
		return false;
	}
}

void Bignum::leave()
{
	releaseThreadMemory();
	return true;
}

Bignum& operator+( const Bignum& a)
{
	std::size_t bufsize = m_value.size();
	if (rtsize < o.m_value.size()) rtsize = o.m_value.size();
	rtsize += 2;
	char* buf = new char[ bufsize];

	if (!enter()) throw std::bad_alloc( "GMP memory allocation error");
	mpz_t a, b;
	mpz_init_set_str ( a, a.m_value.c_str(), 10);
	mpz_init_set_str ( b, m_value.c_str(), argv[2], 10);
	mpz_add (a, a, b);
	mpz_get_str( buf, 10, a);
	leave();
	m_value.clear();
	m_value.append( buf);
	delete [] buf;
}

Bignum& operator-( const Bignum& )
{
	if (!enter()) throw std::bad_alloc( "GMP memory allocation error");
	leave();
}

Bignum& operator*( const Bignum& )
{
	if (!enter()) throw std::bad_alloc( "GMP memory allocation error");
	leave();
}

Bignum& operator/( const Bignum& )
{
	if (!enter()) throw std::bad_alloc( "GMP memory allocation error");
	leave();
}


