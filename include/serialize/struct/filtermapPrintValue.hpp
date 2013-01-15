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
///\file serialize/struct/filtermapPrintValue.hpp
///\brief Defines the intrucsive printing of a value for serialization
#ifndef _Wolframe_SERIALIZE_STRUCT_FILTERMAP_PRINT_VALUE_HPP_INCLUDED
#define _Wolframe_SERIALIZE_STRUCT_FILTERMAP_PRINT_VALUE_HPP_INCLUDED
#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits.hpp>
#include <string>

namespace _Wolframe {
namespace serialize {
namespace traits {

///\class PrintValueType
///\brief type traits for print value types
struct PrintValueType
{
	struct string_ {};		//< atomic type category tag for string value to print
	struct bool_ {};		//< atomic type category tag for a boolean value to print
	struct double_ {};		//< atomic type category tag for double precision value to print
	struct uint_ {};		//< atomic type category tag for unsinged integer value to print
	struct int_ {};			//< atomic type category tag for integer value to print

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

	///\brief get category double_ for a type
	///\return double_ if T is a floating point number
	template <typename T>
	static typename boost::enable_if_c<
		boost::is_floating_point<T>::value
		,const double_&>::type get( const T&) { static double_ rt; return rt;}

	///\brief get category int_ for a type
	///\return int_ if T is a signed integer number
	template <typename T>
	static typename boost::enable_if_c<
		boost::is_arithmetic<T>::value && boost::is_signed<T>::value && !boost::is_floating_point<T>::value
		,const int_&>::type get( const T&) { static int_ rt; return rt;}

	///\brief get category uint_ for a type
	///\return uint_ if T is an unsigned integer number
	template <typename T>
	static typename boost::enable_if_c<
		boost::is_arithmetic<T>::value && boost::is_unsigned<T>::value && !boost::is_floating_point<T>::value
		,const uint_&>::type get( const T&) { static uint_ rt; return rt;}
};



template <typename ValueType>
bool printValue_( const ValueType& val, const PrintValueType::bool_&, langbind::TypedInputFilter::Element& element)
{
	element.type = langbind::TypedInputFilter::Element::bool_;
	element.value.bool_ = val;
	return true;
}

template <typename ValueType>
bool printValue_( const ValueType& val, const PrintValueType::string_&, langbind::TypedInputFilter::Element& element)
{
	element.type = langbind::TypedInputFilter::Element::string_;
	element.value.string_.ptr = val.c_str();
	element.value.string_.size = val.size();
	return true;
}

template <typename ValueType>
bool printValue_( const ValueType& val, const PrintValueType::double_&, langbind::TypedInputFilter::Element& element)
{
	element.type = langbind::TypedInputFilter::Element::double_;
	element.value.double_ = val;
	return true;
}

template <typename ValueType>
bool printValue_( const ValueType& val, const PrintValueType::int_&, langbind::TypedInputFilter::Element& element)
{
	try
	{
		element.type = langbind::TypedInputFilter::Element::int_;
		element.value.int_ = boost::numeric_cast<int>( val);
		return true;
	}
	catch (boost::bad_numeric_cast&){}
	return false;
}


template <typename ValueType>
bool printValue_( const ValueType& val, const PrintValueType::uint_&, langbind::TypedInputFilter::Element& element)
{
	try
	{
		element.type = langbind::TypedInputFilter::Element::uint_;
		element.value.uint_ = boost::numeric_cast<unsigned int>( val);
		return true;
	}
	catch (boost::bad_numeric_cast&){}
	return false;
}

template <typename ValueType>
bool printValue( const ValueType& val, langbind::TypedInputFilter::Element& element)
{
	return printValue_( val, PrintValueType::get(val), element);
}

}}}//namespace
#endif

