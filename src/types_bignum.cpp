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
#if WITH_LIBGMP
#include "gmp.h"
#endif
#include "types/bignum.hpp"
#include <malloc.h>
#include <stdlib.h>
#include <setjmp.h>
#include "boost/thread/thread.hpp"
#include "boost/thread/mutex.hpp"
#include "boost/thread/tss.hpp"
#include "boost/scoped_ptr.hpp"
#include "boost/scoped_array.hpp"

using namespace _Wolframe::types;

#if WITH_LIBGMP
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

	ThreadMemory() :head(0){}
	~ThreadMemory()
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
	if (!tm->head || !mb) ::longjmp( tm->env, __LINE__);
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
	if (!mb) ::longjmp( tm->env, __LINE__);
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
	MemBlock* mb = (MemBlock*)std::malloc( size);
	ThreadMemory* tm = g_threadMemory.get();
	if (n >= size) ::longjmp( tm->env, __LINE__);
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
	if (n >= size) ::longjmp( tm->env, __LINE__);
	threadMemoryUnlink( tm, mb);
	MemBlock* new_mb = (MemBlock*)std::realloc( mb, size);
	if (!new_mb)
	{
		std::free( mb);
		::longjmp( tm->env, __LINE__);
	}
	mb = new_mb;
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

static void GMP_free_func_ptr( void* ptr, size_t /*old_size*/)
{
	threadMemoryFree( ptr);
}

} //namespace

struct StaticInitializations
{
	StaticInitializations()
	{
		mp_set_memory_functions( &GMP_alloc_func_ptr, &GMP_realloc_func_ptr, &GMP_free_func_ptr);
	}
};
static StaticInitializations g_staticInitializations;


static bool enter_GMP()
{
	ThreadMemory* tm = init_threadMemory();
	if (!tm) return false;
	int st = ::setjmp(tm->env);
	if (st != 0)
	{
		releaseThreadMemory();
		return false;
	}
	else
	{
		return true;
	}
}

static void leave_GMP()
{
	releaseThreadMemory();
}

static std::size_t bufsize_operation_1stOrder( std::size_t o1, std::size_t o2, std::size_t exp)
{
	return ((o1 > o2)?(o1+2):(o2+2)) + exp+1;
}

static std::size_t bufsize_operation_2ndOrder( std::size_t o1, std::size_t o2, std::size_t exp)
{
	return (o1 + o2 +2 + exp);
}

template <void (*OP)( mpz_ptr,mpz_srcptr,mpz_srcptr)>
struct IntBinOperation
{
	static void eval( std::string& dest, const std::string& a, const std::string& b, std::size_t bufsize, std::size_t rexp_=0)
	{
		boost::scoped_array<char> bufptr( new char[ bufsize]);
		char* buf = bufptr.get();
		if (!enter_GMP()) throw std::runtime_error( "GMP malloc error");

		mpz_t aa, bb, rr;
		mpz_init( aa);
		mpz_init( bb);
		mpz_set_str ( aa, a.c_str(), 10);
		mpz_set_str ( bb, b.c_str(), 10);
		OP( rr, aa, bb);
		if (rexp_) mpz_div_ui( rr, rr, rexp_*10);
		mpz_get_str( buf, 10, rr);
		mpz_clear( aa);
		mpz_clear( bb);
		mpz_clear( rr);

		leave_GMP();
		dest.clear();
		dest.append( buf);
	}
};

template <void (*OP)( mpz_ptr,mpz_srcptr,unsigned long)>
struct IntUiBinOperation
{
	static void eval( std::string& dest, const std::string& a, unsigned int bb, std::size_t bufsize, std::size_t rexp_=0)
	{
		boost::scoped_array<char> bufptr( new char[ bufsize]);
		char* buf = bufptr.get();
		if (!enter_GMP()) throw std::runtime_error( "GMP malloc error");

		mpz_t aa, rr;
		mpz_init( aa);
		mpz_set_str ( aa, a.c_str(), 10);
		OP( rr, aa, bb);
		if (rexp_) mpz_div_ui( rr, rr, rexp_*10);
		mpz_get_str( buf, 10, rr);
		mpz_clear( aa);
		mpz_clear( rr);

		leave_GMP();
		dest.clear();
		dest.append( buf);
	}
};

static void increxp( std::string& value, std::size_t exp_)
{
	for (std::size_t ii=0; ii<exp_; ++ii)
	{
		value.push_back( '0');
	}
}

Bignum& Bignum::operator+( const Bignum& arg)
{
	if (m_exp < arg.m_exp)
	{
		increxp( m_value, arg.m_exp - m_exp);
		m_exp = arg.m_exp;
	}
	if (m_exp == arg.m_exp)
	{
		std::size_t bufsize = bufsize_operation_1stOrder( m_value.size(), arg.m_value.size(), m_exp);
		IntBinOperation<mpz_add>::eval( m_value, (const std::string&)m_value, arg.m_value, bufsize);
	}
	else
	{
		std::string argval( arg.m_value);
		increxp( argval, m_exp);
		std::size_t bufsize = bufsize_operation_1stOrder( m_value.size(), argval.size(), m_exp);
		IntBinOperation<mpz_add>::eval( m_value, (const std::string&)m_value, argval, bufsize);
	}
	return *this;
}

Bignum& Bignum::operator-( const Bignum& arg)
{
	if (m_exp < arg.m_exp)
	{
		increxp( m_value, arg.m_exp - m_exp);
		m_exp = arg.m_exp;
	}
	if (m_exp == arg.m_exp)
	{
		std::size_t bufsize = bufsize_operation_1stOrder( m_value.size(), arg.m_value.size(), m_exp);
		IntBinOperation<mpz_sub>::eval( m_value, (const std::string&)m_value, arg.m_value, bufsize);
	}
	else
	{
		std::string argval( arg.m_value);
		increxp( argval, m_exp-arg.m_exp);
		std::size_t bufsize = bufsize_operation_1stOrder( m_value.size(), argval.size(), m_exp);
		IntBinOperation<mpz_sub>::eval( m_value, (const std::string&)m_value, argval, bufsize);
	}
	return *this;
}

Bignum& Bignum::operator*( const Bignum& arg)
{
	if (m_exp < arg.m_exp)
	{
		increxp( m_value, arg.m_exp - m_exp);
		m_exp = arg.m_exp;
	}
	std::size_t bufsize = bufsize_operation_2ndOrder( m_value.size(), arg.m_value.size(), m_exp);
	IntBinOperation<mpz_mul>::eval( m_value, (const std::string&)m_value, arg.m_value, bufsize, m_exp);
	return *this;
}

Bignum& Bignum::operator/( const Bignum& arg)
{
	if (m_exp < arg.m_exp)
	{
		increxp( m_value, arg.m_exp - m_exp);
		m_exp = arg.m_exp;
	}
	increxp( m_value, arg.m_exp);
	std::size_t bufsize = bufsize_operation_1stOrder( m_value.size(), arg.m_value.size(), m_exp);
	IntBinOperation<mpz_div>::eval( m_value, (const std::string&)m_value, arg.m_value, bufsize);
	return *this;
}

Bignum& Bignum::pow( const unsigned int& arg)
{
	std::size_t bufsize = m_value.size() * arg + 2;
	IntUiBinOperation<mpz_pow_ui>::eval( m_value, (const std::string&)m_value, (unsigned long)arg, bufsize, m_exp*arg);
	return *this;
}

#endif

Bignum& Bignum::neg()
{
	if (m_value[0] == '-')
	{
		std::string val( m_value.c_str()+1, m_value.size()-1);
		m_value = val;
	}
	else
	{
		std::string val("-");
		val.append( m_value);
		m_value = val;
	}
	return *this;
}

bool Bignum::set( const std::string& val)
{
	m_exp = 0;
	m_value.clear();

	std::string::const_iterator itr = val.begin();
	if (*itr == '-')
	{
		m_value.push_back( '-');
		++itr;
	}
	for (; itr!=val.end(); ++itr)
	{
		if (*itr < '0' || *itr > '9')
		{
			if (*itr == '.') break;
			return false;
		}
		m_value.push_back( *itr);
	}
	if (itr != val.end())
	{
		if (*itr != '.')
		{
			return false;
		}
		for (++itr; itr!=val.end(); ++itr)
		{
			if (*itr < '0' || *itr > '9')
			{
				return false;
			}
			++m_exp;
			m_value.push_back( *itr);
		}
	}
	return true;
}

void Bignum::get( std::string& val)
{
	val.clear();
	std::size_t gg = (m_value.size()>m_exp)?(m_value.size()-m_exp):0;
	val.append( m_value.c_str(), gg);
	val.push_back( '.');
	for (std::size_t ll=gg; ll < m_exp; ++ll)
	{
		val.push_back( '0');
	}
	while (gg < m_value.size())
	{
		val.push_back( m_value[gg]);
		++gg;
	}
}



