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
#include <cmath>
#include <limits>
#include <boost/utility/enable_if.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/is_float.hpp>
#include <boost/cstdint.hpp>

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
		float_,int_,uint_,string_
	};
	typedef boost::int64_t IntDomainType;
	typedef boost::uint64_t UintDomainType;

	///\brief Get the name of a type as string
	static const char* typeName( Type tp)
	{
		static const char* ar[] = {"float","int","uint","string",0};
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
	///\return true on success, false if the type check fails
	template <typename T>
	typename boost::enable_if_c<boost::is_float<T>::value,bool>::type
	set( const T& val)
	{
		return assign_double( val, std::numeric_limits<T>::epsilon());
	}

	template <typename T>
	typename boost::enable_if_c<boost::is_integral<T>::value && !boost::is_unsigned<T>::value,bool>::type
	set( const T& val)
	{
		return assign_int( val);
	}

	template <typename T>
	typename boost::enable_if_c<boost::is_integral<T>::value && boost::is_unsigned<T>::value,bool>::type
	set( const T& val)
	{
		return assign_uint( val);
	}

	template <typename T>
	typename boost::enable_if_c<!boost::is_integral<T>::value && !boost::is_float<T>::value && !boost::is_same<std::string,T>::value,bool>::type
	set( const T& val)
	{
		try
		{
			return assign_string( boost::lexical_cast<std::string>(val));
		}
		catch ( const boost::bad_lexical_cast&)
		{
			return false;
		}
	}

	template <typename T>
	typename boost::enable_if_c<boost::is_same<std::string,T>::value,bool>::type
	set( const T& val)
	{
		return assign_string( val);
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

	bool assign_double( double val, double epsilon);
	bool assign_int( IntDomainType val);
	bool assign_uint( UintDomainType val);
	bool assign_string( const std::string& val);
};

}}//namespace
#endif

