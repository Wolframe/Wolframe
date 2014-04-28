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
///\file fxpArithmetic.cpp
///\brief Implements some operations on arbitrary sized packed bcd fixed point numbers
#include "fxpArithmetic.hpp"
#include <boost/lexical_cast.hpp>

using namespace _Wolframe;
using namespace _Wolframe::types;

BigFxpBCD::BigFxpBCD( const types::BigBCD& o, int scale_)
	:m_bcd(o),m_scale(scale_)
{}

BigFxpBCD::BigFxpBCD( const types::BigNumber& num)
{
	initFromNumber( num);
}

BigFxpBCD::BigFxpBCD( const std::string& numstr)
{
	initFromString( numstr);
}

BigFxpBCD& BigFxpBCD::operator=( const std::string& o)
{
	initFromString( o);
	return *this;
}

BigFxpBCD& BigFxpBCD::operator=( double o)
{
	initFromString( boost::lexical_cast<std::string>( o));
	return *this;
}

BigFxpBCD& BigFxpBCD::operator=( _WOLFRAME_INTEGER o)
{
	m_bcd.init( o);
	m_scale = 0;
	return *this;
}

BigFxpBCD& BigFxpBCD::operator=( _WOLFRAME_UINTEGER o)
{
	m_bcd.init( o);
	m_scale = 0;
	return *this;
}

BigFxpBCD& BigFxpBCD::operator=( const BigNumber& num)
{
	initFromNumber( num);
	return *this;
}

BigFxpBCD& BigFxpBCD::operator=( const BigFxpBCD& num)
{
	m_bcd.init( num.m_bcd);
	m_scale = num.m_scale;
	return *this;
}

void BigFxpBCD::initFromNumber( const BigNumber& num)
{
	if (num.scale() > 0)
	{
		BigNumberConst num2( num.sign(), num.precision(), 0, num.digits());
		m_bcd.init( num2);
		if (num.precision() < num.scale())
		{
			m_bcd.shift( num.scale() - num.precision());
		}
		m_scale = num.scale();
	}
	else if (num.scale() < 0)
	{
		m_bcd.init( num);
		m_bcd.shift( -num.scale());
		m_scale = 0;
	}
	else
	{
		m_bcd.init( num);
		m_scale = 0;
	}
}

void BigFxpBCD::initFromString( const std::string& numstr)
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
				val.push_back( *ii);
				cpn++;
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
	m_bcd.init( val);
	m_scale = cpn;
}

std::string BigFxpBCD::tostring() const
{
	BigBCD::const_iterator ii=m_bcd.begin(), ee=m_bcd.end();
	unsigned int kk = ii.size();
	std::string rt;
	if (!kk)
	{
		rt.push_back('0');
		return rt;
	}
	if (m_bcd.sign() == '-')
	{
		rt.push_back('-');
	}
	if (kk == m_scale)
	{
		rt.push_back('0');
	}
	else
	{
		for (; kk > m_scale && ii != ee; --kk,++ii)
		{
			rt.push_back( ii.ascii());
		}
	}
	if (m_scale > 0)
	{
		rt.push_back('.');
	}
	for (kk=0; ii != ee && kk < m_scale; ++kk,++ii)
	{
		rt.push_back( ii.ascii());
	}
	for (; kk < m_scale; ++kk)
	{
		rt.push_back( '0');
	}
	return rt;
}

double BigFxpBCD::todouble() const
{
	return boost::lexical_cast<double>( tostring());
}

void BigFxpBCD::setScale( unsigned int scale_)
{
	if (scale_ != m_scale)
	{
		m_bcd = m_bcd.shift( (int)scale_ - (int)m_scale);
		m_scale = scale_;
	}
}

int BigFxpBCD::compare( const BigFxpBCD& o) const
{
	if (o.m_scale == m_scale)
	{
		return m_bcd.compare( o.m_bcd);
	}
	else if (o.m_scale < m_scale)
	{
		return m_bcd.compare( o.m_bcd.shift( m_scale-o.m_scale));
	}
	else
	{
		BigBCD val( m_bcd.shift( o.m_scale-m_scale));
		return val.compare( o.m_bcd);
	}
}

BigFxpBCD BigFxpBCD::divide( const BigFxpBCD& o, unsigned int addscale) const
{
	BigBCD val( m_bcd.shift( (int)o.m_scale + addscale));
	BigBCD res( val / o.m_bcd);
	BigFxpBCD rt( res, m_scale + addscale);
	return rt;
}

BigFxpBCD BigFxpBCD::divide( _WOLFRAME_INTEGER opr, unsigned int addscale) const
{
	BigBCD val( m_bcd.shift( addscale));
	BigBCD res( m_bcd.operator/(opr));
	BigFxpBCD rt( res, m_scale + addscale);
	return rt;
}

BigFxpBCD BigFxpBCD::operator *( const BigFxpBCD& o) const
{
	BigBCD val( m_bcd.operator*( o.m_bcd));
	BigFxpBCD rt( val, m_scale + o.m_scale);
	return rt;
}

BigFxpBCD BigFxpBCD::operator *( _WOLFRAME_INTEGER opr) const
{
	BigBCD val( m_bcd.operator*( opr));
	BigFxpBCD rt( val, m_scale);
	return rt;
}

BigFxpBCD BigFxpBCD::operator +( const BigFxpBCD& o) const
{
	if (o.m_scale == m_scale)
	{
		BigFxpBCD rt( m_bcd.operator + ( o.m_bcd), m_scale);
		return rt;
	}
	else if (o.m_scale < m_scale)
	{
		BigFxpBCD rt( m_bcd.operator + ( o.m_bcd.shift( m_scale-o.m_scale)), m_scale);
		return rt;
	}
	else
	{
		BigFxpBCD rt( m_bcd.shift( o.m_scale-m_scale) + o.m_bcd, o.m_scale);
		return rt;
	}
}

BigFxpBCD BigFxpBCD::operator +( _WOLFRAME_INTEGER opr) const
{
	BigBCD arg( opr);
	BigFxpBCD rt( *this);
	rt.m_bcd.add( arg.shift( m_scale));
	return rt;
}

BigFxpBCD BigFxpBCD::operator -( const BigFxpBCD& o) const
{
	if (o.m_scale == m_scale)
	{
		BigFxpBCD rt( m_bcd.operator - ( o.m_bcd), m_scale);
		return rt;
	}
	else if (o.m_scale < m_scale)
	{
		BigFxpBCD rt( m_bcd.operator - ( o.m_bcd.shift( m_scale-o.m_scale)), m_scale);
		return rt;
	}
	else
	{
		BigFxpBCD rt( m_bcd.shift( o.m_scale-m_scale) - o.m_bcd, o.m_scale);
		return rt;
	}
}

BigFxpBCD BigFxpBCD::operator -( _WOLFRAME_INTEGER opr) const
{
	BigBCD arg( opr);
	BigFxpBCD rt( *this);
	rt.m_bcd.sub( arg.shift( m_scale));
	return rt;
}

BigFxpBCD BigFxpBCD::operator -() const
{
	BigFxpBCD rt( *this);
	rt.m_bcd.invert_sign();
	return rt;
}

BigFxpBCD BigFxpBCD::format( unsigned int scale_)
{
	if (scale_ != m_scale)
	{
		BigFxpBCD rt;
		rt.m_bcd = m_bcd.shift( (int)scale_ - (int)m_scale);
		rt.m_scale = scale_;
		return rt;
	}
	else
	{
		return *this;
	}
}

BigFxpBCD BigFxpBCD::round( const BigFxpBCD& gran)
{
	BigFxpBCD aa = *this;
	if (gran.m_scale != aa.m_scale)
	{
		aa.m_bcd.init( m_bcd.shift( (int)gran.m_scale - (int)aa.m_scale));
	}
	BigFxpBCD rt( aa.m_bcd.round( gran.m_bcd), gran.m_scale);
	return rt;
}

