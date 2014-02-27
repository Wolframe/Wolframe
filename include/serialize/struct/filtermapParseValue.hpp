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
///\file serialize/struct/filtermapParseValue.hpp
///\brief Defines the intrusive parsing of a value in deserialization
#ifndef _Wolframe_SERIALIZE_STRUCT_FILTERMAP_PARSE_VALUE_HPP_INCLUDED
#define _Wolframe_SERIALIZE_STRUCT_FILTERMAP_PARSE_VALUE_HPP_INCLUDED
#include "types/conversions.hpp"
#include "types/datetime.hpp"
#include "types/bignumber.hpp"
#include "types/customDataType.hpp"
#include "types/integer.hpp"
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
	struct String {};		//< atomic type category tag for string value to parse
	struct Bool {};			//< atomic type category tag for a boolean value to parse
	struct Arithmetic {};		//< atomic type category tag for arithmetic value to parse
	struct DateTime {};		//< atomic type category tag for datetime value to parse
	struct BigNumber {};		//< atomic type category tag for bignumber value to parse

	///\brief get category String for a type
	///\return String if T is a std:string
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

	///\brief get category Arithmetic for a type
	///\return Bool if T is a arithmetic but not a bool
	template <typename T>
	static typename boost::enable_if_c<
		(boost::is_arithmetic<T>::value && !boost::is_same<T,bool>::value && !boost::is_same<T,std::string>::value)
		,const Arithmetic&>::type get( const T&) { static Arithmetic rt; return rt;}

	///\brief get category DateTime for a type
	///\return DateTime if T is a datetime
	template <typename T>
	static typename boost::enable_if_c<
		boost::is_same<T,types::DateTime>::value
		,const DateTime&>::type get( const T&) { static DateTime rt; return rt;}

	///\brief get category BigNumber for a type
	///\return BigNumber if T is a big number
	template <typename T>
	static typename boost::enable_if_c<
		boost::is_same<T,types::BigNumber>::value
		,const BigNumber&>::type get( const T&) { static BigNumber rt; return rt;}
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
static bool parseValue_( ValueType& val, const ParseValueType::String&, const types::VariantConst& element)
{
	try
	{
		val = element.tostring();
		return true;
	}
	catch (const boost::bad_lexical_cast&){}
	catch (const std::runtime_error&){}
	return false;
}

template <typename ValueType>
static bool parseValue_( ValueType& val, const ParseValueType::Bool&, const types::VariantConst& element)
{
	try
	{
		switch (element.type())
		{
			case types::Variant::Null:
				return false;

			case types::Variant::Bool:
				val = element.tobool();
				return true;

			case types::Variant::Double:
				return getBool( val, boost::numeric_cast<int>( element.todouble()));

			case types::Variant::Int:
				return getBool( val, boost::numeric_cast<ValueType>( element.toint()));

			case types::Variant::UInt:
				return getBool( val, boost::numeric_cast<ValueType>( element.touint()));

			case types::Variant::String:
				if (element.charsize() == 4 && std::memcmp( element.charptr(), "true", 4) == 0)
				{
					val = true;
					return true;
				}
				if (element.charsize() == 5 && std::memcmp( element.charptr(), "false", 5) == 0)
				{
					val = false;
					return true;
				}
				return false;

			case types::Variant::BigNumber:
				throw std::runtime_error( "cannot convert big number type to boolean value");
			case types::Variant::Timestamp:
				throw std::runtime_error( "cannot convert timestamp type to boolean value");
			case types::Variant::Custom:
			{
				types::Variant baseval;
				try
				{
					element.customref()->getBaseTypeValue( baseval);
					if (baseval.type() != types::Variant::Custom)
					{
						return parseValue_( val, ParseValueType::Bool(), baseval);
					}
				}
				catch (const std::runtime_error& e)
				{
					throw std::runtime_error( std::string("cannot convert custom data type to boolean value: ") + e.what());
				}
				throw std::runtime_error( "cannot convert custom data type to boolean value");
			}
		}
	}
	catch (const boost::bad_lexical_cast&){}
	return false;
}

template <typename ValueType>
static bool parseValue_( ValueType& val, const ParseValueType::Arithmetic&, const types::VariantConst& element)
{
	try
	{
		switch (element.type())
		{
			case types::Variant::Null:
				return false;

			case types::Variant::Bool:
				val = boost::numeric_cast<ValueType>( element.tobool());
				return true;

			case types::Variant::Double:
				val = boost::numeric_cast<ValueType>( element.todouble());
				return true;

			case types::Variant::Int:
				val = boost::numeric_cast<ValueType>( element.toint());
				return true;

			case types::Variant::UInt:
				val = boost::numeric_cast<ValueType>( element.touint());
				return true;

			case types::Variant::Timestamp:
				throw std::runtime_error( "cannot convert timestamp to arithmetic type");

			case types::Variant::BigNumber:
				val = boost::lexical_cast<ValueType>( element.tostring());
				return true;

			case types::Variant::String:
				val = boost::lexical_cast<ValueType>( element.tostring());
				return true;

			case types::Variant::Custom:
			{
				types::Variant baseval;
				try
				{
					element.customref()->getBaseTypeValue( baseval);
					if (baseval.type() != types::Variant::Custom)
					{
						return parseValue_( val, ParseValueType::Arithmetic(), baseval);
					}
				}
				catch (const std::runtime_error& e)
				{
					throw std::runtime_error( std::string("cannot convert custom data type to boolean value: ") + e.what());
				}
				throw std::runtime_error( "cannot convert custom data type to boolean value");
			}
		}
	}
	catch (const boost::bad_lexical_cast&){}
	catch (const boost::bad_numeric_cast&){}
	return false;
}

template <typename ValueType>
static bool parseValue_( ValueType& val, const ParseValueType::BigNumber&, const types::VariantConst& element)
{
	try
	{
		switch (element.type())
		{
			case types::Variant::Null:
				return false;

			case types::Variant::Bool:
				val = types::BigNumber( (_WOLFRAME_UINTEGER)(element.tobool()?1:0));
				return true;

			case types::Variant::Double:
				val = types::BigNumber( element.todouble());
				return true;

			case types::Variant::Int:
				val = types::BigNumber( element.toint());
				return true;

			case types::Variant::UInt:
				val = types::BigNumber( element.touint());
				return true;

			case types::Variant::Timestamp:
				throw std::runtime_error( "cannot convert timestamp to big number");

			case types::Variant::BigNumber:
				val = *element.bignumref();
				return true;

			case types::Variant::String:
				val = types::BigNumber( element.charptr(), element.charsize());
				return true;

			case types::Variant::Custom:
			{
				types::Variant baseval;
				try
				{
					element.customref()->getBaseTypeValue( baseval);
					if (baseval.type() != types::Variant::Custom)
					{
						return parseValue_( val, ParseValueType::BigNumber(), baseval);
					}
				}
				catch (const std::runtime_error& e)
				{
					throw std::runtime_error( std::string("cannot convert custom data type to boolean value: ") + e.what());
				}
				throw std::runtime_error( "cannot convert custom data type to boolean value");
			}
		}
	}
	catch (const boost::bad_lexical_cast&){}
	catch (const boost::bad_numeric_cast&){}
	return false;
}

template <typename ValueType>
static bool parseValue_( ValueType& val, const ParseValueType::DateTime&, const types::VariantConst& element)
{
	try
	{
		switch (element.type())
		{
			case types::Variant::Null:
				return false;

			case types::Variant::Bool:
				throw std::runtime_error( "cannot convert boolean to datetime");

			case types::Variant::Double:
				throw std::runtime_error( "cannot convert double precision floating point number to datetime");

			case types::Variant::Int:
				throw std::runtime_error( "cannot convert integer to datetime");

			case types::Variant::UInt:
				throw std::runtime_error( "cannot convert unsigned integer to datetime");

			case types::Variant::Timestamp:
				val = element.totimestamp();

			case types::Variant::BigNumber:
				throw std::runtime_error( "cannot convert big number to datetime");

			case types::Variant::String:
				val = types::DateTime( element.charptr(), element.charsize());
				return true;

			case types::Variant::Custom:
			{
				types::Variant baseval;
				try
				{
					element.customref()->getBaseTypeValue( baseval);
					if (baseval.type() != types::Variant::Custom)
					{
						return parseValue_( val, ParseValueType::DateTime(), baseval);
					}
				}
				catch (const std::runtime_error& e)
				{
					throw std::runtime_error( std::string("cannot convert custom data type to boolean value: ") + e.what());
				}
				throw std::runtime_error( "cannot convert custom data type to boolean value");
			}
		}
	}
	catch (const boost::bad_lexical_cast&){}
	catch (const boost::bad_numeric_cast&){}
	return false;
}

template <typename ValueType>
bool parseValue( ValueType& val, const types::VariantConst& element)
{
	return parseValue_( val, ParseValueType::get(val), element);
}

}}//namespace
#endif


