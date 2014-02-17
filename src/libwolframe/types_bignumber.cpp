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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
//\file types_bignumber.cpp
//\brief Big number values implementation
#include "types/bignumber.hpp"
#include "utils/conversions.hpp"
#include <limits>
#include <cstdlib>
#include <boost/cstdint.hpp>
#include <boost/lexical_cast.hpp>

using namespace _Wolframe;
using namespace _Wolframe::types;

static BigNumber* allocBigNumber( std::size_t size)
{
	if (size > std::numeric_limits<unsigned short>::max()) throw std::bad_alloc();
	BigNumber* rt = (BigNumber*)std::calloc( 1, sizeof( BigNumber) + size-1);
	if (!rt) throw std::bad_alloc();
	return rt;
}

_WOLFRAME_UINTEGER BigNumber::touint() const
{
	if (m_sign) throw std::runtime_error( "conversion error: negative bcd number to unsigned integer");
	if (m_scale) throw std::runtime_error("conversion error: bcd number with fraction to unsigned integer");
	_WOLFRAME_UINTEGER rt = 0;
	_WOLFRAME_UINTEGER prev = 0;
	unsigned short ii = 0;
	while (ii < m_size)
	{
		rt = rt * 10 + m_ar[ii];
		if (!(rt > prev))
		{
			throw std::runtime_error("conversion error: big number value to big to convert to unsigned integer");
		}
		prev = rt;
	}
	return rt;
}

_WOLFRAME_INTEGER BigNumber::toint() const
{
	_WOLFRAME_INTEGER val = (_WOLFRAME_UINTEGER)touint();
	if (val < 0) throw std::runtime_error("conversion error: big number value to big to convert to signed integer");
	return m_sign?-val:val;
}

double BigNumber::todouble() const
{
	short si = m_scale;
	double rt = 0;
	double prev = 0;
	unsigned short ii = 0;
	while (ii < m_size)
	{
		rt = rt * 10 + m_ar[ii];
		if (!(rt > prev))
		{
			//... catch NaN
			throw std::runtime_error("conversion error: bcd number value to big to convert to double");
		}
		if (si > 0)
		{
			--si;
			rt /= 10;
		}
		prev = rt;
	}
	while (si < 0)
	{
		++si;
		rt *= 10;
		if (!(rt > prev))
		{
			//... catch NaN
			throw std::runtime_error("conversion error: bcd number value out of range");
		}
		prev = rt;
	}
	return rt;
}

std::string BigNumber::tostring() const
{
	if (m_size == 0) return "0";
	std::string rt;

	if (m_sign) rt.push_back( '-');
	if (m_scale == 0)
	{
		for (int ii=0; ii<m_size; ++ii)
		{
			rt.push_back( m_ar[ii] + '0');
		}
		return rt;
	}
	else if (m_scale > 0)
	{
		if ((unsigned short)m_scale >= m_size)
		{
			unsigned short ii,nof_leading_zeros = (unsigned short)m_scale - m_size;
			rt.push_back( '0');
			rt.push_back( '.');
			for (ii=0; ii<nof_leading_zeros; ++ii)
			{
				rt.push_back( '0');
			}
			for (ii=0; ii<m_size; ++ii)
			{
				rt.push_back( m_ar[ii] + '0');
			}
		}
		else
		{
			unsigned short ii,pre_comma_digits = m_size - (unsigned short)m_scale;
			for (ii=0; ii<pre_comma_digits; ++ii)
			{
				rt.push_back( m_ar[ii] + '0');
			}
			rt.push_back( '.');
			for (ii=0; ii<(unsigned short)m_scale; ++ii)
			{
				rt.push_back( m_ar[ii] + '0');
			}
		}
	}
	else
	{
		unsigned short ii;
		for (ii=0; ii<m_size; ++ii)
		{
			rt.push_back( m_ar[ii] + '0');
		}
		for (ii=0; ii<(unsigned short)m_scale; ++ii)
		{
			rt.push_back( m_ar[ii] + '0');
		}
	}
	return rt;
}

BigNumber* BigNumber::create( double val)
{
	return create( boost::lexical_cast<std::string>( val));
}

BigNumber* BigNumber::create( _WOLFRAME_INTEGER val)
{
	return create( utils::tostring_cast( val));
}

BigNumber* BigNumber::create( _WOLFRAME_UINTEGER val)
{
	return create( utils::tostring_cast( val));
}

BigNumber* BigNumber::create( const std::string& val)
{
	struct LocalGarbageCollect
	{
		BigNumber* val;
		LocalGarbageCollect( BigNumber* val_) :val(val_){}
		~LocalGarbageCollect() {if (val) BigNumber::destroy(val);}
		void done() {val=0;}
	};
	BigNumber* rt = allocBigNumber( val.size());
	LocalGarbageCollect localGarbageCollect(rt);
	std::string::const_iterator vi = val.begin(), ve = val.end();
	enum State {NUMS,NUM1,NUM2,EXPE,EXPS,EXP1}; //< parsing states
	State state = NUMS;

	short scaleinc = 1;
	short prev_scaleinc = 1;
	bool expsign = false;

	for (; vi != ve; ++vi)
	{
		switch (state)
		{
			case NUMS:
				state = NUM1;
				if (*vi == '-')
				{
					rt->m_sign = true;
					continue;
				}
				//...no break here !
			case NUM1:
				if (*vi == '.')
				{
					state = NUM2;
					continue;
				}
				if (*vi >= '0' && *vi <= '9')
				{
					rt->m_ar[ rt->m_size++] = *vi - '0';
					continue;
				}
				state = EXPE;
				continue;
			case NUM2:
				if (*vi >= '0' && *vi <= '9')
				{
					rt->m_ar[ rt->m_size++] = *vi - '0';
					rt->m_scale++;
					continue;
				}
				state = EXPE;
				//...no break here !
			case EXPE:
				if (*vi == ' ') continue;
				if (*vi == 'E')
				{
					state = EXPS;
					continue;
				}
				throw std::runtime_error("syntax error in big number string");
			case EXPS:
				state = EXP1;
				if (*vi == '-')
				{
					expsign = true;
					continue;
				}
				//...no break here !
			case EXP1:
				if (*vi >= '0' && *vi <= '9')
				{
					scaleinc = scaleinc * 10 + *vi - '0';
					if (prev_scaleinc > scaleinc) throw std::runtime_error("conversion error: big number value in string is out of range");
					prev_scaleinc = scaleinc;
					continue;
				}
				throw std::runtime_error("syntax error in big number string");
		}
	}
	if (expsign)
	{
		short prev_scale = rt->m_scale;
		rt->m_scale += scaleinc;
		if (prev_scale > rt->m_scale) throw std::runtime_error("conversion error: big number value in string is out of range");
	}
	else
	{
		short prev_scale = rt->m_scale;
		rt->m_scale -= scaleinc;
		if (prev_scale < rt->m_scale) throw std::runtime_error("conversion error: big number value in string is out of range");
	}
	localGarbageCollect.done();
	return rt;
}

void BigNumber::destroy( BigNumber* num)
{
	std::free( num);
}


