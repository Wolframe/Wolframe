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
///\brief Defines a non intrusive atomic type for the DDLs used for forms

#ifndef _Wolframe_DDL_ATOMICTYPE_HPP_INCLUDED
#define _Wolframe_DDL_ATOMICTYPE_HPP_INCLUDED
#include <string>
#include <stdexcept>
#include <cstddef>
#include <cstring>
#include <boost/utility/enable_if.hpp>
#include <boost/lexical_cast.hpp>

namespace _Wolframe {
namespace ddl {

///\class AtomicType
///\brief atomic type for DDL language binding
class AtomicType
{
public:
	///\enum Type
	///\brief What AtomicType can be
	enum Type
	{
		double_,float_,long_,ulong_,int_,uint_,short_,ushort_,char_,string_
	};

	///\brief Get the name of a type as string
	static const char* typeName( Type tp)
	{
		static const char* ar[] = {"double","float","long","ulong","int","uint","short","ushort","char","string",0};
		return ar[ (int)tp];
	}
	///\brief Get the id of a type by name, if exists
	///\return true, if the type exists
	static bool getType( const char* name, Type& tp);

	///\brief Constructor
	AtomicType( Type t=string_)		:m_type(t){}
	///\brief Copy constructor
	///\param[in] o element to copy
	AtomicType( const AtomicType& o)	:m_type(o.m_type),m_value(o.m_value){}

	///\brief Reset the value
	void clear()
	{
		m_type = string_;
		m_value.clear();
	}

	///\brief Setter method
	///\tparam T Type of element to assign
	///\param[in] val element to assign
	///\return true on success, false if the type check fails or on overflow
	template <typename T>
	bool set( const T& val)
	{
		try
		{
			switch (m_type)
			{
				case double_:	assign<double,T>( val); return true;
				case float_:	assign<float,T>( val); return true;
				case long_:	assign<long,T>( val); return true;
				case ulong_:	assign<unsigned long,T>( val); return true;
				case int_:	assign<int,T>( val); return true;
				case uint_:	assign<unsigned int,T>( val); return true;
				case short_:	assign<short,T>( val); return true;
				case ushort_:	assign<unsigned short,T>( val); return true;
				case char_:	assign<unsigned char,T>( val); return true;
				case string_:	boost::lexical_cast<std::string>( val); return true;
			}

		}
		catch ( const boost::bad_lexical_cast&)
		{
			return false;
		}
		return false;
	}

	///\brief Getter method
	///\tparam T Type of element to get
	///\param[out] val element retrieved
	///\return true on success, false if the type check fails or on overflow
	template <typename T>
	bool get( T& val) const
	{
		try
		{
			val = boost::lexical_cast<T>( m_value);
			return true;
		}
		catch ( const boost::bad_lexical_cast&)
		{
			return false;
		}
	}

	///\brief Get the type of this
	///\return the type of this
	Type type() const
	{
		return m_type;
	}

	///\brief Get the value of this as string
	///\return the value of this as string
	const std::string& value() const
	{
		return m_value;
	}

	///\brief Print content to out
	///\param[out] out output stream to print to
	///\param[in] indent indentiation for items to print
	void print( std::ostream& out, size_t indent=0) const;

	///\brief Initialize value (reset) depending on type
	void init();

private:
	Type m_type;						//< type of this
	std::string m_value;					//< value of this

	///\brief Internally used assingement over another type as intermediate.
	///\tparam M type serving as intermediate of the assignement (used for type checking)
	///\tparam S type of the element to assign to this. (source type of the assingement)
	///\param[in] src element to assign to this
	template <typename M, typename S>
	typename boost::disable_if_c<
		boost::is_same<M,S>::value,void>::type assign( const S& src)
	{
		M im = boost::lexical_cast<M>( src);
		m_value = boost::lexical_cast<std::string>(im);
	}

	template <typename M, typename S>
	typename boost::enable_if_c<
		boost::is_same<M,S>::value,void>::type assign( const S& src)
	{
		m_value = boost::lexical_cast<std::string>(src);
	}
};

}}//namespace
#endif

