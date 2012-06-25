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
///\file ddl_atomicType.cpp
///\brief implementation of the form DDL atomic type
#include "ddl/atomicType.hpp"
#include <limits>

using namespace _Wolframe;
using namespace ddl;

bool AtomicType::getType( const char* name, Type& tp)
{
	const char* rt;
	unsigned int ii;
	for (ii=0,rt=typeName((Type)(ii)); rt!=0; ii++,rt=typeName((Type)(ii)))
	{
		if (std::strcmp( rt, name) == 0)
		{
			tp = (Type)ii;
			return true;
		}
	}
	return false;
}

void AtomicType::print( std::ostream& out, size_t indent) const
{
	while (indent--) out << "\t";
	out << typeName( m_type) << " '" << m_value << "'" << std::endl;
}

void AtomicType::init()
{
	switch (m_type)
	{
		case float_:
		case int_:
		case uint_:
		case string_:	m_value.clear();
	}
}

bool AtomicType::assign_double( double val, double epsilon)
{
	try
	{
		switch (m_type)
		{
			case float_:
			{
				m_value = boost::lexical_cast<std::string>( val);
				return true;
			}
			case int_:
			{
				double vf = std::floor(val);
				if (val-vf < epsilon)
				{
					if (vf < 0.0)
					{
						m_value = "-";
						vf = -vf - 1.0;
					}
					IntDomainType conv = boost::numeric_cast<IntDomainType>( vf);
					m_value.append( boost::lexical_cast<std::string>( conv));
					return true;
				}
				else
				{
					throw boost::bad_numeric_cast();
				}
			}
			case uint_:
			{
				double vf = std::floor(val);
				if (val-vf < epsilon)
				{
					if (vf < 0.0)
					{
						throw boost::bad_numeric_cast();
					}
					IntDomainType conv = boost::numeric_cast<IntDomainType>( vf);
					m_value.append( boost::lexical_cast<std::string>( conv));
					return true;
				}
				else
				{
					throw boost::bad_numeric_cast();
				}
			}
			case string_:
			{
				m_value = boost::lexical_cast<std::string>( val);
				return true;
			}
		}
	}
	catch ( const boost::bad_lexical_cast&)
	{
		return false;
	}
	catch ( const boost::bad_numeric_cast&)
	{
		return false;
	}
	return false;
}

bool AtomicType::assign_int( boost::int64_t val)
{
	try
	{
		switch (m_type)
		{
			case float_:
			{
				m_value = boost::lexical_cast<std::string>( val);
				return true;
			}
			case int_:
			{
				m_value = boost::lexical_cast<std::string>( val);
			}
			case uint_:
			{
				if (val < 0)
				{
					throw boost::bad_numeric_cast();
				}
				m_value = boost::lexical_cast<std::string>( val);
			}
			case string_:
			{
				m_value = boost::lexical_cast<std::string>( val);
			}
		}
	}
	catch ( const boost::bad_lexical_cast&)
	{
		return false;
	}
	catch ( const boost::bad_numeric_cast&)
	{
		return false;
	}
	return true;
}

bool AtomicType::assign_uint( boost::uint64_t val)
{
	try
	{
		switch (m_type)
		{
			case float_:
			{
				m_value = boost::lexical_cast<std::string>( val);
				return true;
			}
			case int_:
			case uint_:
			{
				m_value = boost::lexical_cast<std::string>( val);
			}
			case string_:
			{
				m_value = boost::lexical_cast<std::string>( val);
			}
		}
	}
	catch ( const boost::bad_lexical_cast&)
	{
		return false;
	}
	catch ( const boost::bad_numeric_cast&)
	{
		return false;
	}
	return true;
}

bool AtomicType::assign_string( const std::string& val)
{
	try
	{
		std::size_t digits;
		std::string::const_iterator itr = val.begin();

		switch (m_type)
		{
			case float_:
			{
				if (*itr == '-') ++itr;
				while (*itr >= '0' && *itr <= '9')
				{
					++itr;
				}
				if (*itr == '.') ++itr;
				while (*itr >= '0' && *itr <= '9')
				{
					++itr;
				}
				if (*itr == 'E')
				{
					++itr;
					if (*itr == '-') ++itr;
					while (*itr >= '0' && *itr <= '9')
					{
						++itr;
						++digits;
					}
					if (digits == 0) throw boost::bad_lexical_cast();
				}
				m_value = val;
				break;
			}
			case int_:
				if (*itr == '-') ++itr;
				/*no break here!*/
			case uint_:
				digits=0;
				while (*itr >= '0' && *itr <= '9')
				{
					++itr;
					++digits;
				}
				if (digits == 0) throw boost::bad_lexical_cast();
				/*no break here!*/
			case string_:
			{
				m_value = val;
			}
		}
	}
	catch ( const boost::bad_lexical_cast&)
	{
		return false;
	}
	catch ( const boost::bad_numeric_cast&)
	{
		return false;
	}
	return true;
}

