/************************************************************************
Copyright (C) 2011, 2012 Project Wolframe.
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
///\file serialize/struct/filtermapParseValue.hpp
///\brief Defines the intrusive parsing of a value in deserialization
#ifndef _Wolframe_SERIALIZE_STRUCT_FILTERMAP_PARSE_VALUE_HPP_INCLUDED
#define _Wolframe_SERIALIZE_STRUCT_FILTERMAP_PARSE_VALUE_HPP_INCLUDED
#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits.hpp>
#include <string>

namespace _Wolframe {
namespace serialize {

///\class ParseValueType
///\brief type traits for parse value types
struct ParseValueType
{
	struct string_ {};		//< atomic type category tag for string value to parse
	struct bool_ {};		//< atomic type category tag for a boolean value to parse
	struct arithmetic_ {};		//< atomic type category tag for arithmertic value to parse

	///\brief get category string_ for a type
	///\return string_ if T is a std:string
	template <typename T>
	static typename boost::enable_if_c<
		boost::is_same<T,std::string>::value
		,const string_&>::type get( const T&) { static string_ rt; return rt;}

	///\brief get category bool_ for a type
	///\return bool_ if T is a bool
	template <typename T>
	static typename boost::enable_if_c<
		boost::is_same<T,bool>::value
		,const bool_&>::type get( const T&) { static bool_ rt; return rt;}

	///\brief get category arithmetic_ for a type
	///\return bool_ if T is a arithmetic but not a bool
	template <typename T>
	static typename boost::enable_if_c<
		(boost::is_arithmetic<T>::value && !boost::is_same<T,bool>::value && !boost::is_same<T,std::string>::value)
		,const arithmetic_&>::type get( const T&) { static arithmetic_ rt; return rt;}
};


static bool getBool( bool& val, int boolnum)
{
	if (boolnum == 0)
	{
		val = false;
		return true;
	}
	else if (boolnum == 1)
	{
		val = true;
		return true;
	}
	return false;
}

template <typename ValueType>
static bool parseValue_( ValueType& val, const ParseValueType::string_&, const langbind::TypedInputFilter::Element& element)
{
	try
	{
		switch (element.type)
		{
			case langbind::TypedInputFilter::Element::bool_:
				val = (element.value.bool_)?"true":"false";
				return true;

			case langbind::TypedInputFilter::Element::double_:
				val = boost::lexical_cast<ValueType>( element.value.double_);
				return true;

			case langbind::TypedInputFilter::Element::int_:
				val = boost::lexical_cast<ValueType>( element.value.int_);
				return true;

			case langbind::TypedInputFilter::Element::uint_:
				val = boost::lexical_cast<ValueType>( element.value.uint_);
				return true;

			case langbind::TypedInputFilter::Element::string_:
				val.clear();
				val.append( element.value.string_.ptr, element.value.string_.size);
				return true;

			case langbind::TypedInputFilter::Element::blob_:
				val.clear();
				val.append( (const char*)element.value.blob_.ptr, element.value.blob_.size);
				return true;
		}
	}
	catch (const boost::bad_lexical_cast&)
	{
		return false;
	}
	return false;
}

template <typename ValueType>
static bool parseValue_( ValueType& val, const ParseValueType::bool_&, const langbind::TypedInputFilter::Element& element)
{
	try
	{
		switch (element.type)
		{
			case langbind::TypedInputFilter::Element::bool_:
				val = element.value.bool_;
				return true;

			case langbind::TypedInputFilter::Element::double_:
				return getBool( val, boost::numeric_cast<int>( element.value.double_));

			case langbind::TypedInputFilter::Element::int_:
				return getBool( val, boost::numeric_cast<ValueType>( element.value.int_));

			case langbind::TypedInputFilter::Element::uint_:
				return getBool( val, boost::numeric_cast<ValueType>( element.value.uint_));

			case langbind::TypedInputFilter::Element::blob_:
				return false;

			case langbind::TypedInputFilter::Element::string_:
				if (element.value.string_.size == 4 && std::memcmp( element.value.string_.ptr, "true", 4) == 0)
				{
					val = true;
					return true;
				}
				if (element.value.string_.size == 5 && std::memcmp( element.value.string_.ptr, "false", 5) == 0)
				{
					val = false;
					return true;
				}
				return false;
		}
	}
	catch (const boost::bad_lexical_cast&)
	{
		return false;
	}
}

template <typename ValueType>
static bool parseValue_( ValueType& val, const ParseValueType::arithmetic_&, const langbind::TypedInputFilter::Element& element)
{
	try
	{
		switch (element.type)
		{
			case langbind::TypedInputFilter::Element::bool_:
				val = boost::numeric_cast<ValueType>( element.value.bool_);
				return true;

			case langbind::TypedInputFilter::Element::double_:
				val = boost::numeric_cast<ValueType>( element.value.double_);
				return true;

			case langbind::TypedInputFilter::Element::int_:
				val = boost::numeric_cast<ValueType>( element.value.int_);
				return true;

			case langbind::TypedInputFilter::Element::uint_:
				val = boost::numeric_cast<ValueType>( element.value.uint_);
				return true;

			case langbind::TypedInputFilter::Element::string_:
				val = boost::lexical_cast<ValueType>( std::string( element.value.string_.ptr, element.value.string_.size));
				return true;

			case langbind::TypedInputFilter::Element::blob_:
				return false;
		}
	}
	catch (const boost::bad_lexical_cast&)
	{
		return false;
	}
	catch (const boost::bad_numeric_cast&)
	{
		return false;
	}
	return false;
}

template <typename ValueType>
bool parseValue( ValueType& val, const langbind::TypedInputFilter::Element& element)
{
	return parseValue_( val, ParseValueType::get(val), element);
}

}}//namespace
#endif


