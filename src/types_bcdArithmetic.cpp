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
///\file types_bcdArithmetic.cpp
///\brief Implements some operations on arbitrary sized packed bcd numbers

#include "types/bcdArithmetic.hpp"
#include <cstring>
#include <cstdlib>
#include <stdexcept>
#include <limits>
#include <cmath>

using namespace _Wolframe::types;

void BigBCD::xchg( BigBCD& a, BigBCD& b)
{
	BigBCD tmp;
	tmp.m_ar = a.m_ar;
	tmp.m_size = a.m_size;
	tmp.m_neg = a.m_neg;
	tmp.m_allocated = a.m_allocated;

	a.m_ar = b.m_ar;
	a.m_size = b.m_size;
	a.m_neg = b.m_neg;
	a.m_allocated = b.m_allocated;

	b.m_ar = tmp.m_ar;
	b.m_size = tmp.m_size;
	b.m_neg = tmp.m_neg;
	b.m_allocated = tmp.m_allocated;

	tmp.m_ar = 0;
	tmp.m_size = 0;
	tmp.m_allocated = false;
}

namespace {
class MemChunk
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
		free( m_buf);
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
}//anonymous namespace

class BigBCD::Allocator
{
public:
	Allocator() :m_chunk(0){}

	~Allocator()
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

	void* alloc( std::size_t nn)
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
private:
	MemChunk* m_chunk;
};

void BigBCD::init( std::size_t nn, bool ng, Allocator* allocator)
{
	if (m_ar && m_allocated) free( m_ar);
	m_size = nn;
	if (m_size)
	{
		std::size_t mm = nn * sizeof(*m_ar);
		if (mm < nn) throw std::bad_alloc();
		if (allocator)
		{
			m_ar = (boost::uint32_t*)allocator->alloc( mm);
			m_allocated = false;
		}
		else
		{
			m_ar = (boost::uint32_t*)std::malloc( mm);
			if (!m_ar) throw std::bad_alloc();
			std::memset( m_ar, 0, mm);
			m_allocated = true;
		}
	}
	else
	{
		m_ar = 0;
		m_allocated = false;
	}
	m_neg = ng;
}

BigBCD::BigBCD()
	:m_size(0)
	,m_ar(0)
	,m_neg(false)
	,m_allocated(false)
{
	init( 0, false);
}

BigBCD::BigBCD( std::size_t n, bool ng)
	:m_size(0)
	,m_ar(0)
	,m_neg(ng)
	,m_allocated(false)
{
	init( n, ng);
}

BigBCD::BigBCD( const std::string& numstr)
	:m_size(0)
	,m_ar(0)
	,m_neg(false)
	,m_allocated(false)
{
	unsigned int ii = 0, nn = numstr.size();
	if (numstr[ ii] == '-')
	{
		++ii;
		m_neg = true;
	}
	unsigned int bb = (((nn-ii)+6) / 7);
	unsigned int tt = (((nn-ii)+6) % 7) * 4;
	if (!bb) throw std::runtime_error( "illegal bcd number string");

	init( bb, m_neg);
	for (; ii<nn; ++ii)
	{
		unsigned int digit = (unsigned char)(numstr[ ii] - '0');
		if (digit > 9) throw std::runtime_error( "illegal bcd number");
		m_ar[ bb-1] += (digit << tt);

		if (tt == 0)
		{
			--bb;
			if (!bb && (ii+1) != nn) throw std::runtime_error( "illegal state in bcd number constructor");
			tt = 24;
		}
		else
		{
			tt -= 4;
		}
	}
}

BigBCD::BigBCD( const BigBCD& o)
	:m_size(o.m_size)
	,m_ar(0)
	,m_neg(o.m_neg)
	,m_allocated(false)
{
	init( m_size, m_neg);
	std::memcpy( m_ar, o.m_ar, m_size * sizeof(*m_ar));
}

void BigBCD::copy( const BigBCD& o, Allocator* allocator)
{
	init( o.m_size, o.m_neg, allocator);
	std::memcpy( m_ar, o.m_ar, m_size * sizeof(*m_ar));
}

BigBCD::~BigBCD()
{
	if (m_ar && m_allocated) free( m_ar);
}

std::string BigBCD::tostring() const
{
	std::string rt;
	if (m_neg) rt.push_back('-');

	const_iterator ii = begin(), ee = end();
	if (ii == ee) return "0";

	for (; ii != ee; ++ii)
	{
		rt.push_back( ii->ascii());
	}
	return rt;
}

BigBCD::const_iterator::const_iterator()
	:m_idx(0),m_shf(24),m_ar(0)
{}

BigBCD::const_iterator::const_iterator( const BigBCD& bcd)
	:m_idx(bcd.m_size),m_shf(24),m_ar(bcd.m_ar)
{
	while (m_idx>0)
	{
		unsigned char digit = (unsigned char)((m_ar[ m_idx-1] >> m_shf) & 0xf);
		if (digit != 0) break;
		increment();
	}
}

std::size_t BigBCD::const_iterator::size() const
{
	return (m_idx == 0)?0:((m_idx-1)*7 + m_shf/4);
}

void BigBCD::const_iterator::increment()
{
	if (m_shf == 0)
	{
		m_shf = 24;
		m_idx -= 1;
	}
	else
	{
		m_shf -= 4;
	}
}

unsigned char BigBCD::const_iterator::operator*() const
{
	unsigned char rt = (unsigned char)(m_ar[ m_idx-1] >> m_shf) & 0xf;
	if (rt > 9) throw std::runtime_error( "illegal bcd number");
	return rt;
}

bool BigBCD::const_iterator::isequal( const const_iterator& other) const
{
	return m_idx==other.m_idx && m_shf==other.m_shf;
}

bool BigBCD::const_iterator::islt( const const_iterator& other) const
{
	return m_idx>other.m_idx || m_shf>other.m_shf;
}

bool BigBCD::const_iterator::isle( const const_iterator& other) const
{
	return m_idx>=other.m_idx || m_shf>=other.m_shf;
}


static boost::uint32_t checkvalue( boost::uint32_t a)
{
	// thanks to http://www.divms.uiowa.edu/~jones/bcd/bcd.html:
	boost::uint32_t t1,t2;
	t1 = a + 0x06666666;
	t2 = t1 ^ a;
	return (t2 & 0x11111110);
}

static boost::uint32_t add_bcd( boost::uint32_t a, boost::uint32_t b)
{
	// thanks to http://www.divms.uiowa.edu/~jones/bcd/bcd.html:
	boost::uint32_t t1,t2,t3,t4,t5,t6;
	t1 = a + 0x06666666;
	t2 = t1 + b;
	t3 = t1 ^ b;
	t4 = t2 ^ t3;
	t5 = ~t4 & 0x11111110;
	t6 = (t5 >> 2) | (t5 >> 3);
	return t2 - t6;
}

static boost::uint32_t tencomp( boost::uint32_t a)
{
	// thanks to http://www.divms.uiowa.edu/~jones/bcd/bcd.html:
	boost::uint32_t t1,t2,t3,t4,t5,t6;
	t1 = 0xffffFFFF - a;
	t2 = (boost::uint32_t) (- (boost::int32_t)a);
	t3 = t1 ^ 0x00000001;
	t4 = t2 ^ t3;
	t5 = ~t4 & 0x11111110;
	t6 = (t5 >> 2) | (t5 >> 3);
	return t2 - t6;
}

static boost::uint32_t sub_bcd( boost::uint32_t a, boost::uint32_t b)
{
	boost::uint32_t rt = add_bcd( a, tencomp(b));
	return rt;
}

static boost::uint32_t getcarry( boost::uint32_t& a)
{
	boost::uint32_t carry = (a >> 28);
	a &= 0x0fffFFFF;
	return carry;
}

static boost::uint32_t increment( boost::uint32_t a)
{
	return add_bcd( a, 1);
}

static boost::uint32_t decrement( boost::uint32_t a)
{
	return sub_bcd( a, 1);
}

bool BigBCD::isValid() const
{
	std::size_t ii;
	boost::uint32_t chkval = 0;
	for (ii=0; ii<m_size; ++ii)
	{
		chkval |= checkvalue( m_ar[ii]);
	}
	return (chkval == 0);
}

bool BigBCD::isNull() const
{
	const_iterator ii=begin(),ee=end();
	return (ii==ee);
}

void BigBCD::digits_addition( BigBCD& rt, const BigBCD& this_, const BigBCD& opr, Allocator* allocator)
{
	boost::uint32_t carry;
	std::size_t ii=0, nn = (opr.m_size > this_.m_size)?opr.m_size:this_.m_size;
	if (nn == 0) return;
	rt.init( nn+1, this_.m_neg, allocator);
	carry = 0;
	for (;ii<nn; ++ii)
	{
		boost::uint32_t op1 = (ii>=this_.m_size)?0:this_.m_ar[ii];
		boost::uint32_t op2 = (ii>=opr.m_size)?0:opr.m_ar[ii];
		boost::uint32_t res = add_bcd( op1, op2);
		if (carry) res = increment( res);
		carry = getcarry( res);
		rt.m_ar[ ii] = res;
	}
	if (carry)
	{
		rt.m_ar[ nn++] = carry;
	}
	else
	{
		for (ii=nn; ii>0; --ii)
		{
			if (rt.m_ar[ii-1]) break;
		}
		rt.m_size = (ii>0)?ii:1;
	}
}

void BigBCD::digits_subtraction( BigBCD& rt, const BigBCD& this_, const BigBCD& opr, Allocator* allocator)
{
	std::size_t ii = 0, mm = 0, nn = (opr.m_size > this_.m_size)?opr.m_size:this_.m_size;
	if (nn == 0) return;
	rt.init( nn, this_.m_neg, allocator);
	boost::uint32_t carry = 0;
	for (;ii<nn; ++ii)
	{
		boost::uint32_t op1 = (ii>=this_.m_size)?0:this_.m_ar[ii];
		boost::uint32_t op2 = (ii>=opr.m_size)?0:opr.m_ar[ii];
		boost::uint32_t res = add_bcd( op1, tencomp(op2));
		if (carry)
		{
			res = decrement( res);
			carry = (op1 <= op2);
		}
		else
		{
			carry = (op1 < op2);
		}
		rt.m_ar[ ii] = res;
	}
	if (carry)
	{
		for (mm=nn; mm>0; mm--)
		{
			boost::uint32_t res = rt.m_ar[ mm-1];
			if (mm>1) res = increment( res);
			res = tencomp(res) & 0x0fffFFFF;
			rt.m_ar[ mm-1] = res;
		}
		rt.m_neg = !rt.m_neg;
	}
	else
	{
		for (mm=nn; mm>0; mm--) rt.m_ar[ mm-1] &= 0x0fffFFFF;
	}
	for (ii=nn; ii>0; --ii)
	{
		if (rt.m_ar[ii-1]) break;
	}
	rt.m_size = (ii>0)?ii:1;
}

void BigBCD::digits_shift( BigBCD& rt, const BigBCD& this_, int nof_digits, Allocator* allocator)
{
	if (nof_digits > 0)
	{
		unsigned int ofs = (unsigned int)nof_digits / 7;
		unsigned int sfh = (unsigned int)nof_digits % 7;
		std::size_t ii,nn;
		const boost::uint32_t MASK = (1<<28)-1;

		rt.init( this_.m_size + ofs + 1, this_.m_neg, allocator);
		for (ii=0,nn=ofs; ii<nn; ++ii)
		{
			rt.m_ar[ ii] = 0;
		}
		if (sfh == 0)
		{
			for (ii=0,nn=this_.m_size; ii<nn; ++ii)
			{
				rt.m_ar[ ii+ofs] = this_.m_ar[ ii];
			}
		}
		else if (this_.m_size)
		{
			unsigned char upshift=28-(sfh*4),doshift=sfh*4;
			rt.m_ar[ ofs++] = (this_.m_ar[ 0] << doshift) & MASK;
			for (ii=0,nn=this_.m_size-1; ii<nn; ++ii)
			{
				boost::uint32_t aa = this_.m_ar[ ii] >> upshift;
				boost::uint32_t bb = (this_.m_ar[ ii+1] << doshift) & MASK;
				rt.m_ar[ ii + ofs] = aa | bb;
			}
			rt.m_ar[ ii + ofs] = this_.m_ar[ ii] >> upshift;
		}
	}
	else if (nof_digits < 0)
	{
		nof_digits = -nof_digits;
		unsigned int ofs = (unsigned int)nof_digits / 7;
		unsigned int sfh = (unsigned int)nof_digits % 7;
		std::size_t ii,nn;
		const boost::uint32_t MASK = (1<<28)-1;

		rt.init( this_.m_size - ofs + 1, this_.m_neg, allocator);
		if (sfh == 0)
		{
			for (ii=ofs,nn=this_.m_size; ii<nn; ++ii)
			{
				rt.m_ar[ ii-ofs] = this_.m_ar[ ii];
			}
		}
		else if (this_.m_size)
		{
			unsigned char upshift=28-(sfh*4),doshift=sfh*4;
			for (ii=ofs,nn=this_.m_size-1; ii<nn; ++ii)
			{
				boost::uint32_t aa = this_.m_ar[ ii] >> doshift;
				boost::uint32_t bb = (this_.m_ar[ ii+1] << upshift) & MASK;
				rt.m_ar[ ii - ofs] = aa | bb;
			}
			rt.m_ar[ ii - ofs] = this_.m_ar[ ii] >> doshift;
		}
	}
	else
	{
		rt.copy( this_, allocator);
	}
	std::size_t ii;
	for (ii=rt.m_size; ii>0; --ii)
	{
		if (rt.m_ar[ii-1]) break;
	}
	rt.m_size = ii;
}

BigBCD BigBCD::shift( int digits) const
{
	BigBCD rt;
	digits_shift( rt, *this, digits, 0);
	return rt;
}

void BigBCD::digits_16_multiplication( BigBCD& rt, const BigBCD& this_, Allocator* allocator)
{
	BigBCD x2,x4,x8;
	digits_addition( x2, this_, this_, allocator);
	digits_addition( x4, x2, x2, allocator);
	digits_addition( x8, x4, x4, allocator);
	digits_addition( rt, x8, x8, allocator);
}

void BigBCD::digits_nibble_multiplication( BigBCD& rt, const BigBCD& this_, unsigned char factor, Allocator* allocator)
{
	BigBCD x2,x4,x8;
	if ((factor & 0xE) != 0)
	{
		digits_addition( x2, this_, this_, allocator);
		if ((factor & 0xC) != 0)
		{
			digits_addition( x4, x2, x2, allocator);
			if ((factor & 0x8) != 0)
			{
				digits_addition( x8, x4, x4, allocator);
			}
		}
	}
	switch (factor)
	{
		case 0:
		break;
		case 1:
			rt.copy( this_, allocator);
		break;
		case 2:
			rt.copy( x2, allocator);
		break;
		case 3:
			digits_addition( rt, x2, this_, allocator);
		break;
		case 4:
			rt.copy( x4, allocator);
		break;
		case 5:
			digits_addition( rt, x4, this_, allocator);
		break;
		case 6:
			digits_addition( rt, x4, x2, allocator);
		break;
		case 7:
		{
			BigBCD x6;
			digits_addition( x6, x4, x2, allocator);
			digits_addition( rt, x6, this_, allocator);
		}
		break;
		case 8:
			rt.copy( x8, allocator);
		break;
		case 9:
			digits_addition( rt, x8, this_, allocator);
		break;
		case 10:
			digits_addition( rt, x8, x2, allocator);
		break;
		case 11:
		{
			BigBCD x10;
			digits_addition( x10, x8, x2, allocator);
			digits_addition( rt, x10, this_, allocator);
		}
		break;
		case 12:
			digits_addition( rt, x8, x4, allocator);
		break;
		case 13:
		{
			BigBCD x12;
			digits_addition( x12, x8, x4, allocator);
			digits_addition( rt, x12, this_, allocator);
		}
		break;
		case 14:
		{
			BigBCD x12;
			digits_addition( x12, x8, x4, allocator);
			digits_addition( rt, x12, x2, allocator);
		}
		break;
		case 15:
		{
			BigBCD x12,x14;
			digits_addition( x12, x8, x4, allocator);
			digits_addition( x14, x12, x2, allocator);
			digits_addition( rt, x14, this_, allocator);
		}
		break;
		default:
			throw std::logic_error( "multiplication nibble out of range");
	}
}

void BigBCD::digits_multiplication( BigBCD& rt, const BigBCD& this_, unsigned int factor, Allocator* allocator)
{
	if (factor == 0)
	{
		rt.init( 0, false);
		return;
	}
	BigBCD part,fac;
	digits_nibble_multiplication( rt, this_, factor & 0x0f, allocator);
	fac.copy( this_, allocator);
	factor >>= 4;
	while (factor > 0)
	{
		BigBCD newfac;
		digits_16_multiplication( newfac, fac, allocator);
		xchg( fac, newfac);
		digits_nibble_multiplication( part, fac, factor & 0x0f, allocator);
		BigBCD sum;
		digits_addition( sum, part, rt, allocator);
		xchg( sum, rt);
		factor >>= 4;
	}
}

void BigBCD::digits_multiplication( BigBCD& rt, const BigBCD& this_, const BigBCD& opr, Allocator* allocator)
{
	const_iterator ii = opr.begin(), ee = opr.end();
	if (ii == ee) return;

	digits_nibble_multiplication( rt, this_, *ii, allocator);
	++ii;
	while (ii != ee)
	{
		BigBCD sum;
		digits_shift( sum, rt, 1, allocator);
		BigBCD part;
		digits_nibble_multiplication( part, this_, *ii, allocator);
		digits_addition( rt, sum, part, allocator);
		++ii;
	}
}

static int estimate_shifts( const BigBCD& this_, const BigBCD& match)
{
	int rt = (int)(this_.begin().size() - match.begin().size());
	if (*this_.begin() == *match.begin())
	{
		return rt;
	}
	else
	{
		return rt - 1;
	}
}

void BigBCD::digits_division( BigBCD& rt, const BigBCD& this_, const BigBCD& opr, Allocator* allocator)
{
	BigBCD reminder;
	reminder.copy( this_, allocator);
	reminder.m_neg = false;

	if (opr.isNull()) throw std::runtime_error( "division by zero");

	while (!reminder.isNull() && !reminder.isabslt( opr))
	{
		unsigned int estimate = division_estimate( reminder, opr);
		if (estimate == 0) throw std::runtime_error( "illegal state calculating division estimate");
		BigBCD part;
		digits_multiplication( part, opr, estimate, allocator);
		part.m_neg = false;
		int estshift = estimate_shifts( reminder, part);
		BigBCD corr;
		digits_shift( corr, part, estshift, allocator);

		while (reminder < corr)
		{
			if (estimate < 16)
			{
				estimate--;
				if (estimate == 0) throw std::logic_error( "division estimate got zero");
			}
			else
			{
				estimate -= estimate >> 4;
			}
			digits_multiplication( part, opr, estimate, allocator);
			part.m_neg = false;
			digits_shift( corr, part, estshift, allocator);
		}
		BigBCD bcdest;
		bcdest.copy( estimate_as_bcd( estimate, estshift, allocator), allocator);

		digits_multiplication( part, opr, bcdest, allocator);

		BigBCD newrt;
		digits_addition( newrt, rt, bcdest, allocator);
		xchg( rt, newrt);
		BigBCD newreminder;
		digits_subtraction( newreminder, reminder, part, allocator);
		xchg( reminder, newreminder);
	}
	if (opr.sign() != this_.sign())
	{
		rt.m_neg = true;
	}
}

BigBCD BigBCD::add( const BigBCD& opr) const
{
	BigBCD rt;
	if (m_neg == opr.m_neg)
	{
		digits_addition( rt, *this, opr, 0);
	}
	else
	{
		digits_subtraction( rt, *this, opr, 0);
	}
	return rt;
}

BigBCD BigBCD::sub( const BigBCD& opr) const
{
	BigBCD rt;
	if (m_neg == opr.m_neg)
	{
		digits_subtraction( rt, *this, opr, 0);
	}
	else
	{
		digits_addition( rt, *this, opr, 0);
	}
	return rt;
}

BigBCD BigBCD::mul( unsigned int opr) const
{
	BigBCD val;
	Allocator allocator;
	digits_multiplication( val, *this, opr, &allocator);
	return BigBCD( val);
}

BigBCD BigBCD::mul( const BigBCD& opr) const
{
	BigBCD val;
	Allocator allocator;
	digits_multiplication( val, *this, opr, &allocator);
	return BigBCD( val);
}

bool BigBCD::isequal( const BigBCD& o) const
{
	if (sign() != o.sign()) return false;
	BigBCD::const_iterator ii = begin(), ee = end(), oo = o.begin();
	if (ii.size() != oo.size()) return false;
	for (; ii != ee; ++ii,++oo) if (*ii != *oo) return false;
	return true;
}

bool BigBCD::isabslt( const BigBCD& o) const
{
	BigBCD::const_iterator ii = begin(), ee = end(), oo = o.begin();
	if (ii.size() > oo.size()) return false;
	if (ii.size() < oo.size()) return true;
	for (; ii != ee; ++ii,++oo)
	{
		if (*ii > *oo) return false;
		if (*ii < *oo) return true;
	}
	return false;
}

bool BigBCD::isabsle( const BigBCD& o) const
{
	BigBCD::const_iterator ii = begin(), ee = end(), oo = o.begin();
	if (ii.size() > oo.size()) return false;
	if (ii.size() < oo.size()) return true;
	for (; ii != ee; ++ii,++oo)
	{
		if (*ii > *oo) return false;
		if (*ii < *oo) return true;
	}
	return true;
}

bool BigBCD::islt( const BigBCD& o) const
{
	if (sign() != o.sign())
	{
		return (sign() == '-');
	}
	return isabslt( o);
}

bool BigBCD::isle( const BigBCD& o) const
{
	if (sign() != o.sign())
	{
		return (sign() == '-');
	}
	return isabsle( o);
}

static unsigned int estimate_to_uint( double val)
{
	boost::uint64_t rt = static_cast<boost::uint64_t>( std::floor( val * 1000000000));
	while (rt > 1000000000000ULL) rt /= 1000;
	while (rt >= std::numeric_limits<unsigned int>::max()) rt /= 10;
	return (unsigned int)rt;
}

unsigned int BigBCD::division_estimate( const BigBCD& this_, const BigBCD& opr)
{
	double est = 0;
	double div = 0;

	BigBCD::const_iterator ic = this_.begin(), ie = this_.end(), oc = opr.begin(), oe = opr.end();
	for (unsigned int kk=0; kk<24; ++kk)
	{
		est *= 10.0;
		if (ic < ie)
		{
			est += *ic;
			++ic;
		}
		div *= 10.0;
		if (oc < oe)
		{
			div += *oc;
			++oc;
		}
	}
	return estimate_to_uint( est / (div+1));
}

BigBCD BigBCD::estimate_as_bcd( unsigned int estimate, int estshift, Allocator* allocator)
{
	unsigned int mm = (estshift>0)?(3+estshift/4):(3-estshift/4);
	BigBCD rt;
	rt.init( mm, false, allocator);

	if (estimate >= std::numeric_limits<unsigned int>::max())
	{
		throw std::logic_error( "division estimate out of range");
	}
	while (estshift < -6) { estimate /= 1000000; estshift += 6;}
	while (estshift < -3) { estimate /= 1000; estshift += 3;}
	while (estshift < -1) { estimate /= 100; estshift += 2;}
	while (estshift <  0) { estimate /= 10; estshift += 1;}
	if (estimate == 0)
	{
		estimate = 1;
	}
	unsigned int bb = estshift/7, tt = 4*(estshift%7);
	while (estimate > 0)
	{
		unsigned int dd = estimate % 10;
		estimate /= 10;
		rt.m_ar[ bb] |= dd << tt;

		if (tt == 24)
		{
			tt = 0;
			bb += 1;
		}
		else
		{
			tt += 4;
		}
	}
	std::size_t ii;
	for (ii=rt.m_size; ii>0; --ii)
	{
		if (rt.m_ar[ii-1]) break;
	}
	rt.m_size = ii;
	return rt;
}

BigBCD BigBCD::div( const BigBCD& opr) const
{
	BigBCD val;
	Allocator allocator;
	digits_division( val, *this, opr, &allocator);
	return BigBCD( val);
}

