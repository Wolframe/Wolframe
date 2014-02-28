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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file bcdArithmetic.cpp
///\brief Implements some operations on arbitrary sized packed bcd numbers

#include "bcdArithmetic.hpp"
#include "utils/allocators.hpp"
#include <cstring>
#include <cstdlib>
#include <stdexcept>
#include <limits>
#include <cmath>
#include <boost/lexical_cast.hpp>

#ifdef _Wolframe_TYPES_BCD_USE_64BIT
#define NumMask 0x0fffFFFFffffFFFFULL
#define NumHighShift 60
#define NumDigits 15
#define MaxEstimate 100000000000000ULL
#else
#define NumMask 0x0fffFFFF
#define NumHighShift 28
#define NumDigits 7
#define MaxEstimate 1000000000
#endif

using namespace _Wolframe;
using namespace _Wolframe::types;

void BigBCD::xchg( BigBCD& a, BigBCD& b)
{
	BigBCD tmp;
	tmp.m_ar = a.m_ar;
	tmp.m_size = a.m_size;
	tmp.m_sign = a.m_sign;
	tmp.m_allocated = a.m_allocated;

	a.m_ar = b.m_ar;
	a.m_size = b.m_size;
	a.m_sign = b.m_sign;
	a.m_allocated = b.m_allocated;

	b.m_ar = tmp.m_ar;
	b.m_size = tmp.m_size;
	b.m_sign = tmp.m_sign;
	b.m_allocated = tmp.m_allocated;

	tmp.m_ar = 0;
	tmp.m_size = 0;
	tmp.m_allocated = false;
}

void BigBCD::allocate( std::size_t nn, Allocator* allocator)
{
	if (m_ar && m_allocated) free( m_ar);
	m_size = nn;
	if (m_size)
	{
		std::size_t mm = nn * sizeof(*m_ar);
		if (mm < nn) throw std::bad_alloc();
		if (allocator)
		{
			m_ar = (BCDElement*)allocator->alloc( mm);
			m_allocated = false;
		}
		else
		{
			m_ar = (BCDElement*)std::malloc( mm);
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
	m_sign = false;
}

BigBCD::BigBCD()
	:m_size(0)
	,m_ar(0)
	,m_sign(false)
	,m_allocated(false)
{
	allocate( 0, 0);
}

void BigBCD::init( const BigNumber& num)
{
	unsigned int ii = 0, nn = num.size(), nofDigits = num.size();
	const unsigned char* digits = num.digits();
	const unsigned char* digits_zero = {0};
	if (nn == 0)
	{
		digits = digits_zero;
		nofDigits = nn = 1;
	}
	if (num.scale() > 0)
	{
		if ((unsigned int)num.scale() > nn)
		{
			digits = digits_zero;
			nn = nofDigits = 1;
		}
		else
		{
			nn -= (unsigned int)num.scale();
			nofDigits = nn;
		}
	}
	else
	{
		nn += (unsigned int)-num.scale();
	}
	unsigned int bb = ((nn+NumDigits-1) / NumDigits);
	unsigned int tt = ((nn+NumDigits-1) % NumDigits) * 4;

	allocate( bb, 0);
	for (; ii<nn; ++ii)
	{
		BCDElement digit;
		if (ii > nofDigits)
		{
			digit = 0;
		}
		else
		{
			digit = digits[ ii];
			if (digit > 9) throw std::runtime_error( "illegal bcd number");
		}
		m_ar[ bb-1] += (digit << tt);

		if (tt == 0)
		{
			--bb;
			if (!bb && (ii+1) != nn) throw std::runtime_error( "illegal state in bcd number constructor");
			tt = (NumHighShift-4);
		}
		else
		{
			tt -= 4;
		}
	}
	m_sign = num.sign();
	normalize();
}

void BigBCD::init( const std::string& str)
{
	BigNumber num( str);
	init( num);
}

void BigBCD::init( const char* str, std::size_t strsize)
{
	BigNumber num( str, strsize);
	init( num);
}

void BigBCD::init( _WOLFRAME_INTEGER num)
{
	bool ng = false;
	if (num < 0)
	{
		ng = true;
		num = -num;
	}
	BigBCD th = estimate_as_bcd( (FactorType)num, 0, 0);
	th.m_sign ^= ng;
	copy( th, 0);
}

void BigBCD::init( _WOLFRAME_UINTEGER num)
{
	BigBCD th = estimate_as_bcd( (FactorType)num, 0, 0);
	copy( th, 0);
}

BigBCD::BigBCD( const std::string& numstr)
	:m_size(0)
	,m_ar(0)
	,m_sign(false)
	,m_allocated(false)
{
	init( numstr);
}

BigBCD::BigBCD( const BigNumber& num)
	:m_size(0)
	,m_ar(0)
	,m_sign(false)
	,m_allocated(false)
{
	BigBCD::init( num);
}

BigBCD::BigBCD( _WOLFRAME_INTEGER num)
	:m_size(0)
	,m_ar(0)
	,m_sign(false)
	,m_allocated(false)
{
	BigBCD::init( num);
}

BigBCD::BigBCD( _WOLFRAME_UINTEGER num)
	:m_size(0)
	,m_ar(0)
	,m_sign(false)
	,m_allocated(false)
{
	BigBCD::init( num);
}

BigBCD::BigBCD( const BigBCD& o)
	:m_size(o.m_size)
	,m_ar(0)
	,m_sign(o.m_sign)
	,m_allocated(false)
{
	allocate( m_size, 0);
	m_sign = o.m_sign;
	std::memcpy( m_ar, o.m_ar, m_size * sizeof(*m_ar));
}

void BigBCD::copy( const BigBCD& o, Allocator* allocator)
{
	allocate( o.m_size, allocator);
	m_sign = o.m_sign;
	std::memcpy( m_ar, o.m_ar, m_size * sizeof(*m_ar));
}

BigBCD::~BigBCD()
{
	if (m_ar && m_allocated) free( m_ar);
}

std::string BigBCD::tostring() const
{
	std::string rt;
	const_iterator ii = begin(), ee = end();
	if (ii == ee) return "0";

	if (m_sign)
	{
		rt.push_back('-');
	}
	for (; ii != ee; ++ii)
	{
		rt.push_back( ii->ascii());
	}
	return rt;
}

_WOLFRAME_INTEGER BigBCD::toint() const
{
	_WOLFRAME_INTEGER rt = 0;
	int cnt = 0;

	const_iterator ii = begin(), ee = end();
	if (ii == ee) return 0;

	for (; cnt < _WOLFRAME_INTEGER_DIGITS && ii != ee; ++ii)
	{
		rt = rt * 10 + *ii;
	}
	if (m_sign)
	{
		rt = -rt;
	}
	if (ii != ee)
	{
		throw std::runtime_error("number out of range to convert it to an int");
	}
	return rt;
}

double BigBCD::todouble() const
{
	double rt = 0.0;

	const_iterator ii = begin(), ee = end();
	if (ii == ee) return 0;

	for (; ii != ee; ++ii)
	{
		rt = rt * 10 + *ii;
	}
	if (m_sign)
	{
		rt = -rt;
	}
	return rt;
}

BigBCD::const_iterator::const_iterator()
	:m_idx(0),m_shf(NumHighShift-4),m_ar(0)
{}

BigBCD::const_iterator::const_iterator( const BigBCD& bcd)
	:m_idx(bcd.m_size),m_shf(NumHighShift-4),m_ar(bcd.m_ar)
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
	return (m_idx == 0)?0:((m_idx-1)*NumDigits + m_shf/4 + 1);
}

void BigBCD::const_iterator::increment()
{
	if (m_shf == 0)
	{
		m_shf = NumHighShift-4;
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

#ifdef _Wolframe_TYPES_BCD_USE_64BIT
static BCDElement checkvalue( boost::uint64_t a)
{
	// thanks to http://www.divms.uiowa.edu/~jones/bcd/bcd.html:
	boost::uint64_t t1,t2;
	t1 = a + 0x0666666666666666ULL;
	t2 = t1 ^ a;
	return (t2 & 0x1111111111111110ULL);
}

static boost::uint64_t add_bcd( boost::uint64_t a, boost::uint64_t b)
{
	// thanks to http://www.divms.uiowa.edu/~jones/bcd/bcd.html:
	boost::uint64_t t1,t2,t3,t4,t5,t6;
	t1 = a + 0x0666666666666666ULL;
	t2 = t1 + b;
	t3 = t1 ^ b;
	t4 = t2 ^ t3;
	t5 = ~t4 & 0x1111111111111110ULL;
	t6 = (t5 >> 2) | (t5 >> 3);
	return t2 - t6;
}

static boost::uint64_t tencomp( boost::uint64_t a)
{
	// thanks to http://www.divms.uiowa.edu/~jones/bcd/bcd.html:
	boost::uint64_t t1,t2,t3,t4,t5,t6;
	t1 = 0xffffFFFFffffFFFFULL - a;
	t2 = (boost::uint64_t) (- (boost::int64_t)a);
	t3 = t1 ^  0x0000000000000001ULL;
	t4 = t2 ^ t3;
	t5 = ~t4 & 0x1111111111111110ULL;
	t6 = (t5 >> 2) | (t5 >> 3);
	return t2 - t6;
}

static boost::uint64_t getcarry( boost::uint64_t& a)
{
	boost::uint64_t carry = (a >> 60);
	a &= 0x0fffFFFFffffFFFFULL;
	return carry;
}
#else
static BCDElement checkvalue( boost::uint32_t a)
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

static boost::uint32_t getcarry( boost::uint32_t& a)
{
	boost::uint32_t carry = (a >> 28);
	a &= 0x0fffFFFF;
	return carry;
}
#endif

static BCDElement sub_bcd( BCDElement a, BCDElement b)
{
	BCDElement rt = add_bcd( a, tencomp(b));
	return rt;
}


static BCDElement increment( BCDElement a)
{
	return add_bcd( a, 1);
}

static BCDElement decrement( BCDElement a)
{
	return sub_bcd( a, 1);
}

bool BigBCD::isValid() const
{
	std::size_t ii;
	BCDElement chkval = 0;
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

void BigBCD::normalize()
{
	if (!isValid()) throw std::logic_error( "bad bcd calculation");
	std::size_t ii = 0, nn = m_size;

	for (ii=nn; ii>0; --ii)
	{
		if (m_ar[ii-1]) break;
	}
	if (ii > 0)
	{
		m_size = ii;
	}
	else
	{
		m_sign = false;
		m_size = 0;
	}
}

void BigBCD::digits_addition( BigBCD& rt, const BigBCD& this_, const BigBCD& opr, Allocator* allocator)
{
	BCDElement carry;
	std::size_t ii=0, nn = (opr.m_size > this_.m_size)?opr.m_size:this_.m_size;
	if (nn == 0) return;
	rt.allocate( nn+1, allocator);
	rt.m_sign = this_.m_sign;
	carry = 0;
	for (;ii<nn; ++ii)
	{
		BCDElement op1 = (ii>=this_.m_size)?0:this_.m_ar[ii];
		BCDElement op2 = (ii>=opr.m_size)?0:opr.m_ar[ii];
		BCDElement res = add_bcd( op1, op2);
		if (carry) res = increment( res);
		carry = getcarry( res);
		rt.m_ar[ ii] = res;
	}
	if (carry)
	{
		rt.m_ar[ nn++] = carry;
		if (!rt.isValid()) throw std::logic_error( "bad bcd calculation");
	}
	else
	{
		rt.normalize();
	}
}

void BigBCD::digits_subtraction( BigBCD& rt, const BigBCD& this_, const BigBCD& opr, Allocator* allocator)
{
	std::size_t ii = 0, mm = 0, nn = (opr.m_size > this_.m_size)?opr.m_size:this_.m_size;
	if (nn == 0) return;
	rt.allocate( nn, allocator);
	rt.m_sign = this_.m_sign;
	BCDElement carry = 0;
	for (;ii<nn; ++ii)
	{
		BCDElement op1 = (ii>=this_.m_size)?0:this_.m_ar[ii];
		BCDElement op2 = (ii>=opr.m_size)?0:opr.m_ar[ii];
		BCDElement res = add_bcd( op1, tencomp(op2));
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
			BCDElement res = rt.m_ar[ mm-1];
			if (mm>1) res = increment( res);
			res = tencomp(res) & NumMask;
			rt.m_ar[ mm-1] = res;
		}
		rt.m_sign = !rt.m_sign;
	}
	else
	{
		for (mm=nn; mm>0; mm--) rt.m_ar[ mm-1] &= NumMask;
	}
	rt.normalize();
}

void BigBCD::digits_shift( BigBCD& rt, const BigBCD& this_, int nof_digits, Allocator* allocator)
{
	if (nof_digits > 0)
	{
		unsigned int ofs = (unsigned int)nof_digits / NumDigits;
		unsigned int sfh = (unsigned int)nof_digits % NumDigits;
		std::size_t ii,nn;

		rt.allocate( this_.m_size + ofs + 1, allocator);
		rt.m_sign = this_.m_sign;
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
			unsigned char upshift=NumHighShift-(sfh*4),doshift=sfh*4;
			rt.m_ar[ ofs++] = (this_.m_ar[ 0] << doshift) & NumMask;
			for (ii=0,nn=this_.m_size-1; ii<nn; ++ii)
			{
				BCDElement aa = this_.m_ar[ ii] >> upshift;
				BCDElement bb = (this_.m_ar[ ii+1] << doshift) & NumMask;
				rt.m_ar[ ii + ofs] = aa | bb;
			}
			rt.m_ar[ ii + ofs] = this_.m_ar[ ii] >> upshift;
		}
	}
	else if (nof_digits < 0)
	{
		nof_digits = -nof_digits;
		unsigned int ofs = (unsigned int)nof_digits / NumDigits;
		unsigned int sfh = (unsigned int)nof_digits % NumDigits;
		std::size_t ii,nn;

		rt.allocate( this_.m_size - ofs + 1, allocator);
		rt.m_sign = this_.m_sign;
		if (sfh == 0)
		{
			for (ii=ofs,nn=this_.m_size; ii<nn; ++ii)
			{
				rt.m_ar[ ii-ofs] = this_.m_ar[ ii];
			}
		}
		else if (this_.m_size)
		{
			unsigned char upshift=NumHighShift-(sfh*4),doshift=sfh*4;
			for (ii=ofs,nn=this_.m_size-1; ii<nn; ++ii)
			{
				BCDElement aa = this_.m_ar[ ii] >> doshift;
				BCDElement bb = (this_.m_ar[ ii+1] << upshift) & NumMask;
				rt.m_ar[ ii - ofs] = aa | bb;
			}
			rt.m_ar[ ii - ofs] = this_.m_ar[ ii] >> doshift;
		}
	}
	else
	{
		rt.copy( this_, allocator);
	}
	rt.normalize();
}

void BigBCD::digits_cut( BigBCD& rt, const BigBCD& this_, unsigned int nof_digits, Allocator* allocator)
{
	unsigned int ofs = (unsigned int)nof_digits / NumDigits;
	unsigned char sfh = (unsigned char)nof_digits % NumDigits;
	std::size_t ii,nn;

	rt.allocate( ofs + 1, allocator);
	rt.m_sign = this_.m_sign;
	for (ii=0,nn=ofs; ii<nn; ++ii)
	{
		rt.m_ar[ ii] = this_.m_ar[ ii];
	}
	unsigned int mask = NumMask >> ((NumDigits - sfh) * 4);
	rt.m_ar[ ii] = this_.m_ar[ ii] & mask;
	rt.normalize();
}

BigBCD BigBCD::shift( int digits) const
{
	BigBCD rt;
	digits_shift( rt, *this, digits, 0);
	return rt;
}

BigBCD BigBCD::cut( unsigned int digits) const
{
	BigBCD rt;
	digits_cut( rt, *this, digits, 0);
	return rt;
}

BigBCD BigBCD::round( const BigBCD& gran) const
{
	unsigned int nn = gran.nof_digits();
	if (gran.m_sign || !nn) throw std::runtime_error( "rounding granularity must be a positive number");

	Allocator allocator;
	BigBCD aa;
	aa.copy( *this, &allocator);
	BigBCD ct;
	digits_cut( ct, aa, nn, &allocator);
	ct.m_sign = false;

	unsigned int ft = 0;
	BigBCD zt;
	for (;;)
	{
		++ft;
		digits_subtraction( zt, ct, gran, &allocator);
		if (zt.m_sign) break;
		ct.copy( zt, &allocator);
	}
	if (m_sign)
	{
		return *this + zt;
	}
	else
	{
		return *this - zt;
	}
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

void BigBCD::digits_multiplication( BigBCD& rt, const BigBCD& this_, FactorType factor, Allocator* allocator)
{
	if (factor == 0)
	{
		rt.allocate( 0, 0);
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
	int rt = (int)(this_.nof_digits() - match.nof_digits());
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
	reminder.m_sign = false;

	if (opr.isNull()) throw std::runtime_error( "division by zero");

	while (!reminder.isNull() && reminder.compare( opr) >= 0)
	{
		FactorType estimate = division_estimate( reminder, opr);
		if (estimate == 0) throw std::runtime_error( "illegal state calculating division estimate");
		BigBCD part;
		digits_multiplication( part, opr, estimate, allocator);
		part.m_sign = false;
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
			part.m_sign = false;
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
		rt.m_sign = true;
		rt.normalize();
	}
}

BigBCD BigBCD::add( const BigBCD& opr) const
{
	BigBCD rt;
	if (m_sign == opr.m_sign)
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
	if (m_sign == opr.m_sign)
	{
		digits_subtraction( rt, *this, opr, 0);
	}
	else
	{
		digits_addition( rt, *this, opr, 0);
	}
	return rt;
}

BigBCD BigBCD::mul( FactorType opr) const
{
	BigBCD val;
	Allocator allocator;
	digits_multiplication( val, *this, opr, &allocator);
	return BigBCD( val);
}

BigBCD BigBCD::mul( _WOLFRAME_INTEGER opr) const
{
	bool ng = false;
	if (opr < 0)
	{
		opr = -opr;
		ng = true;
	}
	BigBCD val;
	Allocator allocator;
	digits_multiplication( val, *this, opr, &allocator);
	val.m_sign ^= ng;
	return val;
}

BigBCD BigBCD::mul( const BigBCD& opr) const
{
	BigBCD val;
	Allocator allocator;
	digits_multiplication( val, *this, opr, &allocator);
	return BigBCD( val);
}

int BigBCD::compare( const BigBCD& o) const
{
	if (sign() != o.sign())
	{
		return (sign() == '-')?-1:+1;
	}
	else
	{
		int resOtherSmaller = (sign() == '-')?-1:+1;
		BigBCD::const_iterator ii = begin(), ee = end(), oo = o.begin();
		if (ii.size() > oo.size()) return resOtherSmaller;
		if (ii.size() < oo.size()) return -resOtherSmaller;
		for (; ii != ee; ++ii,++oo)
		{
			if (*ii > *oo) return resOtherSmaller;
			if (*ii < *oo) return -resOtherSmaller;
		}
		return 0;
	}
}

static FactorType estimate_to_uint( double val)
{
	boost::uint64_t rt = static_cast<boost::uint64_t>( std::floor( val * MaxEstimate));
	while (rt >= (std::numeric_limits<FactorType>::max() * 1000)) rt /= 1000;
	while (rt >= std::numeric_limits<FactorType>::max()) rt /= 10;
	return (FactorType)rt;
}

FactorType BigBCD::division_estimate( const BigBCD& this_, const BigBCD& opr)
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

BigBCD BigBCD::estimate_as_bcd( FactorType estimate, int estshift, Allocator* allocator)
{
	unsigned int mm = (estshift>0)?(3+estshift/4):(3-estshift/4);
	BigBCD rt;
	rt.allocate( mm, allocator);

	if (estimate >= std::numeric_limits<FactorType>::max())
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
	unsigned int bb = estshift/NumDigits, tt = 4*(estshift%NumDigits);
	while (estimate > 0)
	{
		BCDElement dd = estimate % 10;
		estimate /= 10;
		rt.m_ar[ bb] |= dd << tt;

		if (tt == (NumHighShift-4))
		{
			tt = 0;
			bb += 1;
		}
		else
		{
			tt += 4;
		}
	}
	rt.normalize();
	return rt;
}

BigBCD BigBCD::div( const BigBCD& opr) const
{
	BigBCD val;
	Allocator allocator;
	digits_division( val, *this, opr, &allocator);
	val.normalize();
	return BigBCD( val);
}

BigBCD BigBCD::neg() const
{
	BigBCD rt(*this);
	rt.m_sign = !rt.m_sign;
	rt.normalize();
	return rt;
}

BigFxpBCD::BigFxpBCD( const types::BigNumber& num)
{
	initFromNumber( num);
}

BigFxpBCD::BigFxpBCD( const std::string& numstr)
{
	initFromString( numstr);
}

BigFxpBCD::BigFxpBCD( const std::string& numstr, unsigned int sp, unsigned int cp)
{
	initFromString( numstr);
	format( sp, cp);
}

BigFxpBCD::BigFxpBCD( const BigBCD& o, unsigned int sp, unsigned int cp)
	:BigBCD(o)
	,m_show_precision(0)
	,m_calc_precision(0)
{
	format( sp, cp);
}

BigFxpBCD::BigFxpBCD( const std::string& numstr, unsigned int p)
{
	initFromString( numstr);
	format( p, p);
}

void BigFxpBCD::initFromString( const std::string& numstr)
{
	initFromString( numstr, numstr.size());
}

BigFxpBCD& BigFxpBCD::operator=( const std::string& o)
{
	unsigned int cp = m_calc_precision;
	unsigned int sp = m_show_precision;
	initFromString( o, cp);
	format( sp, cp);
	return *this;
}

static double multiplyPower10( double o, unsigned int p)
{
	while (p > 8) {p -= 8; o *= 10000000;}
	while (p > 3) {p -= 3; o *= 1000;}
	while (p > 1) {p -= 1; o *= 10;}
	return o;
}

BigFxpBCD& BigFxpBCD::operator=( double o)
{
	BigBCD::init( boost::numeric_cast<_WOLFRAME_INTEGER>( multiplyPower10( o, m_calc_precision)));
	return *this;
}

BigFxpBCD& BigFxpBCD::operator=( _WOLFRAME_INTEGER o)
{
	BigBCD::init( o);
	BigBCD::shift( m_calc_precision);
	return *this;
}

BigFxpBCD& BigFxpBCD::operator=( _WOLFRAME_UINTEGER o)
{
	BigBCD::init( o);
	BigBCD::shift( m_calc_precision);
	return *this;
}

void BigFxpBCD::initFromNumber( const BigNumber& num)
{
	if (num.scale() > 0)
	{
		BigNumberConst num2( num.sign(), num.precision(), 0, num.digits());
		init( num2);
		m_calc_precision = m_show_precision = num.scale();
	}
	else
	{
		init( num);
		m_calc_precision = m_show_precision = 0;
	}
}

void BigFxpBCD::initFromString( const std::string& numstr, unsigned int maxPrecision)
{
	std::string val;
	unsigned int cpn = 0;
	int state = 0;

	std::string::const_iterator ii=numstr.begin(), ee=numstr.end();
	if (*ii == '-')
	{
		val.push_back('-');
		++ii;
	}
	for (; ii != ee; ++ii)
	{
		if (*ii == '.')
		{
			if (state != 0) throw std::runtime_error( "illegal big number syntax");
			state = 1;
		}
		else if (*ii >= '0' && *ii <= '9')
		{
			if (state == 1)
			{
				if (cpn < maxPrecision)
				{
					val.push_back( *ii);
					cpn++;
				}
			}
			else
			{
				val.push_back( *ii);
			}
		}
		else
		{
			throw std::runtime_error( std::string("illegal big number value '") + numstr + "'");
		}
	}
	m_calc_precision = m_show_precision = cpn;
	init( val);
}

std::string BigFxpBCD::tostring() const
{
	BigBCD::const_iterator ii=begin(), ee=end();
	unsigned int kk = ii.size();
	if (!kk) return "0";

	std::string rt;
	if (sign() == '-')
	{
		rt.push_back('-');
	}
	if (kk == m_calc_precision)
	{
		rt.push_back('0');
	}
	else
	{
		for (; kk > m_calc_precision && ii != ee; --kk,++ii)
		{
			rt.push_back( ii.ascii());
		}
	}
	if (m_show_precision > 0)
	{
		rt.push_back('.');
	}
	for (kk=0; ii != ee && kk < m_show_precision; ++kk,++ii)
	{
		rt.push_back( ii.ascii());
	}
	for (; kk < m_show_precision; ++kk)
	{
		rt.push_back( '0');
	}
	return rt;
}

double BigFxpBCD::todouble() const
{
	double rt = 0.0;
	if (m_calc_precision)
	{
		rt = 1.0;
		unsigned int kk = 0;
		for (; kk < m_calc_precision; ++kk)
		{
			rt /= 10;
		}
	}
	const_iterator ii = begin(), ee = end();
	if (ii == ee) return 0.0;

	for (; ii != ee; ++ii)
	{
		rt = rt * 10 + *ii;
	}
	if (sign())
	{
		rt = -rt;
	}
	return rt;
}

int BigFxpBCD::compare( const BigFxpBCD& o) const
{
	if (o.m_calc_precision == m_calc_precision)
	{
		return BigBCD::compare( o);
	}
	else if (o.m_calc_precision < m_calc_precision)
	{
		return BigBCD::compare( o.shift( m_calc_precision-o.m_calc_precision));
	}
	else
	{
		BigBCD val( shift( o.m_calc_precision-m_calc_precision));
		return val.compare(o);
	}
}

BigFxpBCD BigFxpBCD::operator /( const BigFxpBCD& o) const
{
	BigBCD val( shift( (int)o.m_calc_precision));
	BigBCD res( val / o);
	BigFxpBCD rt( res, 0, 0);
	rt.m_calc_precision = m_calc_precision;
	rt.m_show_precision = m_show_precision;
	return rt;
}

BigFxpBCD BigFxpBCD::operator /( _WOLFRAME_INTEGER opr) const
{
	BigBCD res( this->BigBCD::operator/(opr));
	BigFxpBCD rt( res, 0, 0);
	rt.m_calc_precision = m_calc_precision;
	rt.m_show_precision = m_show_precision;
	return rt;
}

BigFxpBCD BigFxpBCD::operator *( const BigFxpBCD& o) const
{
	BigBCD val( this->BigBCD::operator*( o));
	BigFxpBCD rt( val.shift( -(int)o.m_calc_precision), 0, 0);
	rt.m_calc_precision = m_calc_precision;
	rt.m_show_precision = m_show_precision;
	return rt;
}

BigFxpBCD BigFxpBCD::operator *( _WOLFRAME_INTEGER opr) const
{
	BigBCD val( this->BigBCD::operator*( opr));
	BigFxpBCD rt( val, m_show_precision, m_calc_precision);
	return rt;
}

BigFxpBCD BigFxpBCD::operator +( const BigFxpBCD& o) const
{
	if (o.m_calc_precision == m_calc_precision)
	{
		BigFxpBCD rt( BigBCD::operator + ( o), 0, 0);
		rt.m_calc_precision = m_calc_precision;
		rt.m_show_precision = m_show_precision;
		return rt;
	}
	else if (o.m_calc_precision < m_calc_precision)
	{
		BigFxpBCD rt( BigBCD::operator + ( o.shift( m_calc_precision-o.m_calc_precision)), 0, 0);
		rt.m_calc_precision = m_calc_precision;
		rt.m_show_precision = m_show_precision;
		return rt;
	}
	else
	{
		BigBCD val( shift( o.m_calc_precision-m_calc_precision) + o);
		BigFxpBCD rt( val.shift( -(int)(o.m_calc_precision-m_calc_precision)), 0, 0);
		rt.m_calc_precision = m_calc_precision;
		rt.m_show_precision = m_show_precision;
		return rt;
	}
}

BigFxpBCD BigFxpBCD::operator +( _WOLFRAME_INTEGER opr) const
{
	BigBCD arg( opr);
	BigFxpBCD rt( *this);
	rt.add( arg.shift( m_calc_precision));
	return rt;
}

BigFxpBCD BigFxpBCD::operator -( const BigFxpBCD& o) const
{
	if (o.m_calc_precision == m_calc_precision)
	{
		BigFxpBCD rt( BigBCD::operator - ( o), 0, 0);
		rt.m_calc_precision = m_calc_precision;
		rt.m_show_precision = m_show_precision;
		return rt;
	}
	else if (o.m_calc_precision < m_calc_precision)
	{
		BigFxpBCD rt( BigBCD::operator - ( o.shift( m_calc_precision-o.m_calc_precision)), 0, 0);
		rt.m_calc_precision = m_calc_precision;
		rt.m_show_precision = m_show_precision;
		return rt;
	}
	else
	{
		BigBCD val( shift( o.m_calc_precision-m_calc_precision) - o);
		BigFxpBCD rt( val.shift( -(int)(o.m_calc_precision-m_calc_precision)), 0, 0);
		rt.m_calc_precision = m_calc_precision;
		rt.m_show_precision = m_show_precision;
		return rt;
	}
}

BigFxpBCD BigFxpBCD::operator -( _WOLFRAME_INTEGER opr) const
{
	BigBCD arg( opr);
	BigFxpBCD rt( *this);
	rt.sub( arg.shift( m_calc_precision));
	return rt;
}

BigFxpBCD BigFxpBCD::operator -() const
{
	BigFxpBCD rt( *this);
	rt.invert_sign();
	return rt;
}

void BigFxpBCD::format( unsigned int show_prec, unsigned int calc_prec)
{
	if (calc_prec < show_prec)
	{
		throw std::runtime_error("illegal big bcd number format description (show precision > calc precision)");
	}
	if (calc_prec != m_calc_precision)
	{
		init( shift( (int)calc_prec - (int)m_calc_precision));
	}
	m_calc_precision = calc_prec;
	m_show_precision = show_prec;
}

BigFxpBCD BigFxpBCD::round( const BigFxpBCD& gran)
{
	BigFxpBCD aa = *this;
	aa.format( gran.m_show_precision, gran.m_show_precision);
	BigFxpBCD rt( aa.BigBCD::round( gran), 0, 0);
	rt.m_calc_precision = gran.m_show_precision;
	rt.m_show_precision = gran.m_show_precision;
	return rt;
}

