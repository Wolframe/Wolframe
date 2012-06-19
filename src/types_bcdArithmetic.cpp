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

using namespace _Wolframe::types;

void BigBCD::init( std::size_t nn, bool sgn)
{
	if (m_ar) delete [] m_ar;
	if (!nn) ++nn;
	m_size = nn;
	m_ar = (boost::uint32_t*)std::calloc( nn, sizeof(*m_ar));
	if (!m_ar) throw std::bad_alloc();
	m_sign = sgn;
}

void BigBCD::expand( std::size_t addsize)
{
	std::size_t newsize = m_size + addsize;
	if (newsize < m_size) throw std::bad_alloc();
	boost::uint32_t* newar = (boost::uint32_t*)std::realloc( m_ar, newsize * sizeof(*m_ar));
	if (newar == 0) throw std::bad_alloc();
	for (; m_size < newsize; m_size++) m_ar[m_size] = 0;
	m_ar = newar;
	m_size = newsize;
}

BigBCD::BigBCD( std::size_t n, bool sgn)
{
	init( n, sgn);
}

BigBCD::BigBCD( const std::string& numstr)
{
	unsigned int ii = numstr.size();
	unsigned int gg;
	if (numstr[0] == '-')
	{
		init( (numstr.size()+6)/7, true);
		gg = 1;
		m_sign = true;
	}
	else
	{
		init( (numstr.size()+6)/7, false);
		gg = 0;
	}
	unsigned int bb;
	unsigned int tt;
	for (bb=0,tt=0; ii>gg; --ii)
	{
		unsigned char digit = (unsigned char)(numstr[ ii-1] - '0');
		if (digit > '9') throw std::runtime_error( "illegal bcd number");
		m_ar[ bb] = (m_ar[ bb] << 4) + digit;
		if (++tt == 7)
		{
			++bb;
			tt = 0;
		}
	}
}

BigBCD::BigBCD( const BigBCD& o)
{
	init( m_size, m_sign);
	std::memcpy( m_ar, o.m_ar, m_size * sizeof(*m_ar));
}

BigBCD::~BigBCD()
{
	free( m_ar);
}

std::string BigBCD::tostring() const
{
	std::string rt;
	if (m_sign) rt.push_back('-');

	std::size_t bb = m_size;
	unsigned char tt = 24;
	while (bb>0)
	{
		unsigned char digit = (unsigned char)(m_ar[ bb-1] >> tt);
		if (digit != 0) break;
		if (tt == 0)
		{
			tt = 24;
			bb -= 1;
		}
		else
		{
			tt -= 4;
		}
	}
	if (bb == 0)
	{
		if (m_sign)
		{
			rt[0] = '0';
		}
		else
		{
			rt.push_back('0');
		}
	}
	else while (bb>0)
	{
		unsigned char digit = (unsigned char)(m_ar[ bb-1] >> tt);
		if (digit > 9) throw std::runtime_error( "illegal bcd number");
		rt.push_back('0'+digit);
		if (tt == 0)
		{
			tt = 24;
			bb -= 1;
		}
		else
		{
			tt -= 4;
		}
	}
	return rt;
}

static boost::uint32_t checkvalue( boost::uint32_t a)
{
	// thanks to http://www.divms.uiowa.edu/~jones/bcd/bcd.html:
	boost::uint32_t t1,t2;
	t1 = a + 0x06666666;
	t2 = t1 ^ a;
	return (t2 & 0x11111110);
}

static boost::uint32_t add( boost::uint32_t a, boost::uint32_t b)
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

#if USE_OWN_INCREMENT
static boost::uint32_t increment( boost::uint32_t a)
{
	boost::uint32_t t1,t2,t3,t4,t5;
	t1 = a + 0x06666667;
	// convert 0 bytes to 0x6 bytes
	t2 = t1 ^ 0xffffFFFF;
	t3 = t2 & (t2 >> 1);				//... t3 has now bit 0100 set 's where t1 has bits 0011 set
	t4 = t3 & 0x44444444;				//... t4 has 0x4 set for nibbles in t1 in range 0x0 to 0x3 (=0 for a valid bcd a)
	t5 = t1 | (t4 | (t4 >> 1));			//... t5 has 0x6 set for the 0 nibbles in t1
	return (t5 - 0x06666666);
}

static boost::uint32_t decrement( boost::uint32_t a)
{
	boost::uint32_t t1,t2,t3,t4,t5,t6;
	t1 = a - 1;
	// convert 0xF bytes to 0x9 bytes
	t2 = t1 & 0xeeeeEEEE;				//... delete bit 0001 of each nibble
	t3 = t2 & (t2 >> 1);				//... t3 has 0010 set for nibbles in t1 having bit 0110 set
	t3 = t3 & (t3 >> 1);				//... t3 has 0010 set for nibbles in t1 having bits 1110 set
	return t1 - (t3 | (t3 << 1) | (t3 >> 1));	//... subtract 7 from each elememt in t1 equals 0xF
}
#endif

static boost::uint32_t sub( boost::uint32_t a, boost::uint32_t b)
{
	return add( a, tencomp(b));
}

static boost::uint32_t getcarry( boost::uint32_t& a)
{
	boost::uint32_t carry = (a >> 28);
	a &= 0x0fffFFFF;
	return carry;
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

void BigBCD::digits_addition( BigBCD& rt, BigBCD& this_, const BigBCD& opr, boost::uint32_t& carry)
{
	std::size_t ii = 0, nn = rt.m_size;
	carry = 0;
	for (;ii<nn; ++ii)
	{
		boost::uint32_t op1 = (ii>=this_.m_size)?0:this_.m_ar[ii];
		boost::uint32_t op2 = (ii>=opr.m_size)?0:opr.m_ar[ii];
		boost::uint32_t res = add( op1, op2);
#if USE_OWN_INCREMENT
		if (carry) rt.m_ar[ ii] = increment( res);
#else
		rt.m_ar[ ii] = add( res, carry);
#endif
		carry = getcarry( carry);
	}
	if (carry)
	{
		rt.expand( 1);
		rt.m_ar[ nn] = carry;
	}
	else
	{
		for (; ii>0; --ii)
		{
			if (rt.m_ar[ii]) break;
		}
		rt.m_size = ii+1;
	}
}

void BigBCD::digits_subtraction( BigBCD& rt, BigBCD& this_, const BigBCD& opr)
{
	std::size_t ii = 0, nn = rt.m_size;
	boost::uint32_t carry = 0;
	for (;ii<nn; ++ii)
	{
		boost::uint32_t op1 = (ii>=this_.m_size)?0:this_.m_ar[ii];
		boost::uint32_t op2 = (ii>=opr.m_size)?0:opr.m_ar[ii];
		boost::uint32_t res = sub( op1, op2);
#if USE_OWN_INCREMENT
		if (carry) rt.m_ar[ ii] = decrement( res);
#else
		rt.m_ar[ ii] = sub( rt.m_ar[ ii], carry);
#endif
	}
	if (carry)
	{
		rt.m_sign = !rt.m_sign;
	}
	else
	{
		for (; ii>0; --ii)
		{
			if (rt.m_ar[ii]) break;
		}
		rt.m_size = ii+1;
	}
}

BigBCD BigBCD::operator +( const BigBCD& opr)
{
	boost::uint32_t carry;
	std::size_t nn = (opr.m_size > m_size)?opr.m_size:m_size;
	BigBCD rt( nn, m_sign);
	if (m_sign == opr.m_sign)
	{
		digits_addition( rt, *this, opr, carry);
	}
	else
	{
		digits_subtraction( rt, *this, opr);
	}
	return rt;
}

BigBCD BigBCD::operator -( const BigBCD& opr)
{
	boost::uint32_t carry;
	std::size_t nn = (opr.m_size > m_size)?opr.m_size:m_size;
	BigBCD rt( nn, m_sign);
	if (m_sign == opr.m_sign)
	{
		digits_subtraction( rt, *this, opr);
	}
	else
	{
		digits_addition( rt, *this, opr, carry);
	}
	return rt;
}

