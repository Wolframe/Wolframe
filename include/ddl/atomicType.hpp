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
///\file ddl/atomicType.hpp
///\brief Defines an an intrusive atomic type as basic mapping type for DDLs

#ifndef _Wolframe_DDL_ATOMICTYPE_HPP_INCLUDED
#define _Wolframe_DDL_ATOMICTYPE_HPP_INCLUDED
#include <string>
#include <stdexcept>
#include <cstddef>
#include <boost/lexical_cast.hpp>

namespace _Wolframe {
namespace ddl {

///\class AtomicType
///\brief atomic type for DDL language binding
class AtomicType
{
public:
	enum Type
	{
		double_,float_,long_,ulong_,int_,uint_,short_,ushort_,char_,uchar_,string_
	};

	///\brief Get the name of a type
	static const char* typeName( Type tp)
	{
		static const char* ar[] = {"double","float","long","ulong","int","uint","short","ushort","char","uchar","string",0};
		return ar[ (int)tp];
	}
	///\brief Get the id of a type by name, if exists
	///\return true, if the type exists
	static bool getType( const char* name, Type& tp);

	AtomicType( Type t=string_)		:m_type(t){}
	AtomicType( const AtomicType& o)	:m_type(o.m_type),m_value(o.m_value){}

	template <typename T>
	bool set( const T& val)
	{
		try
		{
			switch (m_type)
			{
				case double_:	assign<double,T>( val); break;
				case float_:	assign<float,T>( val); break;
				case long_:	assign<long,T>( val); break;
				case ulong_:	assign<unsigned long,T>( val); break;
				case int_:	assign<int,T>( val); break;
				case uint_:	assign<unsigned int,T>( val); break;
				case short_:	assign<short,T>( val); break;
				case ushort_:	assign<unsigned short,T>( val); break;
				case char_:	assign<char,T>( val); break;
				case uchar_:	assign<unsigned char,T>( val); break;
				case string_:	boost::lexical_cast<std::string>( val); break;
			}

		}
		catch ( const boost::bad_lexical_cast&)
		{
			return false;
		}
		return false;
	}

	template <typename T>
	bool get( T& val)
	{
		try
		{
			val = boost::lexical_cast<T>( m_value);
		}
		catch ( const boost::bad_lexical_cast&)
		{
			return false;
		}
	}

	Type type() const
	{
		return m_type;
	}

	const std::string& value() const
	{
		return m_value;
	}

private:
	Type m_type;
	std::string m_value;

	template <typename M, typename S>
	void assign( const S& src)
	{
		M im = boost::lexical_cast<M>( src);
		m_value = boost::lexical_cast<std::string>(im);
	}
};

}}//namespace
#endif

