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
///\file serialize/struct/filtermapPrintValue.hpp
///\brief Defines the intrucsive printing of a value for serialization
#ifndef _Wolframe_SERIALIZE_STRUCT_FILTERMAP_PRINT_VALUE_HPP_INCLUDED
#define _Wolframe_SERIALIZE_STRUCT_FILTERMAP_PRINT_VALUE_HPP_INCLUDED
#include "types/datetime.hpp"
#include "types/bignumber.hpp"
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
	struct String {};		//< atomic type category tag for a string value to print
	struct Bool {};			//< atomic type category tag for a boolean value to print
	struct Double {};		//< atomic type category tag for a double precision value to print
	struct UInt {};			//< atomic type category tag for an unsinged integer value to print
	struct Int {};			//< atomic type category tag for an integer value to print
	struct DateTime {};		//< atomic type category tag for a datetime value to print
	struct BigNumber {};		//< atomic type category tag for a big number value to print

	///\brief get category String for a type
	///\return String if T is a std::string
	template <typename T>
	static typename boost::enable_if_c<
		boost::is_same<T,std::string>::value
		,const String&>::type get( const T&) { static String rt; return rt;}

	///\brief get category Bool for a type
	///\return Bool if T is a bool
	template <typename T>
	static typename boost::enable_if_c<
		boost::is_same<T,bool>::value
		,const Bool&>::type get( const T&) { static Bool rt; return rt;}

	///\brief get category Double for a type
	///\return Double if T is a floating point number
	template <typename T>
	static typename boost::enable_if_c<
		boost::is_floating_point<T>::value
		,const Double&>::type get( const T&) { static Double rt; return rt;}

	///\brief get category Int for a type
	///\return Int if T is a signed integer number
	template <typename T>
	static typename boost::enable_if_c<
		boost::is_arithmetic<T>::value && boost::is_signed<T>::value && !boost::is_floating_point<T>::value
		,const Int&>::type get( const T&) { static Int rt; return rt;}

	///\brief get category UInt for a type
	///\return UInt if T is an unsigned integer number
	template <typename T>
	static typename boost::enable_if_c<
		boost::is_arithmetic<T>::value && boost::is_unsigned<T>::value && !boost::is_floating_point<T>::value
		,const UInt&>::type get( const T&) { static UInt rt; return rt;}

	///\brief get category DateTime for a type
	///\return DateTime if T is a types::DateTime
	template <typename T>
	static typename boost::enable_if_c<
		boost::is_same<T,types::DateTime>::value
		,const DateTime&>::type get( const T&) { static DateTime rt; return rt;}

	///\brief get category BigNumber for a type
	///\return BigNumber if T is a types::BigNumber
	template <typename T>
	static typename boost::enable_if_c<
		boost::is_same<T,types::BigNumber>::value
		,const BigNumber&>::type get( const T&) { static BigNumber rt; return rt;}
};



template <typename ValueType>
bool printValue_( const ValueType& val, const PrintValueType::Bool&, types::VariantConst& element)
{
	element = val;
	return true;
}

template <typename ValueType>
bool printValue_( const ValueType& val, const PrintValueType::String&, types::VariantConst& element)
{
	element.init( val.c_str(), val.size());
	return true;
}

template <typename ValueType>
bool printValue_( const ValueType& val, const PrintValueType::Double&, types::VariantConst& element)
{
	try
	{
		element = boost::numeric_cast<double>( val);
		return true;
	}
	catch (boost::bad_numeric_cast&){}
	return false;
}

template <typename ValueType>
bool printValue_( const ValueType& val, const PrintValueType::Int&, types::VariantConst& element)
{
	try
	{
		element = boost::numeric_cast<int>( val);
		return true;
	}
	catch (boost::bad_numeric_cast&){}
	return false;
}


template <typename ValueType>
bool printValue_( const ValueType& val, const PrintValueType::UInt&, types::VariantConst& element)
{
	try
	{
		element = boost::numeric_cast<unsigned int>( val);
		return true;
	}
	catch (boost::bad_numeric_cast&){}
	return false;
}

template <typename ValueType>
bool printValue_( const ValueType& val, const PrintValueType::DateTime&, types::VariantConst& element)
{
	element = val;
	return true;
}

template <typename ValueType>
bool printValue_( const ValueType& val, const PrintValueType::BigNumber&, types::VariantConst& element)
{
	element = val;
	return true;
}

template <typename ValueType>
bool printValue( const ValueType& val, types::VariantConst& element)
{
	return printValue_( val, PrintValueType::get(val), element);
}

}}}//namespace
#endif

