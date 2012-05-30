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
///\file serialize/struct/filtermapPrintValue.hpp
///\brief Defines the intrucsive printing of a typed filter element for serialization
#ifndef _Wolframe_SERIALIZE_STRUCT_FILTERMAP_PARSE_VALUE_HPP_INCLUDED
#define _Wolframe_SERIALIZE_STRUCT_FILTERMAP_PARSE_VALUE_HPP_INCLUDED
#include <boost/lexical_cast.hpp>
#include <boost/numeric_cast.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits.hpp>

namespace _Wolframe {
namespace serialize {
namespace traits {

template <typename ValueType>
typename boost::enable_if_c<boost::is_same<bool,ValueType>::value,bool>::type
valuePush( const ValueType& val, TypedInputFilter::Element& element) const
{
	element.type = TypedInputFilter::Element::bool_;
	element.value.bool_ = val;
	return true;
}

template <typename ValueType>
typename boost::enable_if_c<boost::is_same<std::string,ValueType>::value,bool>::type
valuePush( const ValueType& val, TypedInputFilter::Element& element) const
{
	element.type = TypedInputFilter::Element::string_;
	element.value.string_.ptr = val.c_str();
	element.value.string_.size = val.size();
	return true;
}

template <typename ValueType>
typename boost::enable_if_c<boost::is_floating_point<ValueType>::value,bool>::type
valuePush( const ValueType& val, TypedInputFilter::Element& element) const
{
	element.type = TypedInputFilter::Element::double_;
	element.value.double_ = val;
	return true;
}

template <typename ValueType>
typename boost::enable_if_c<boost::is_arithmetic<ValueType>::value && boost::is_signed<ValueType>::value && !boost::is_floating_point<ValueType>::value,bool>::type
valuePush( const ValueType& val, TypedInputFilter::Element& element) const
{
	try
	{
		element.type = TypedInputFilter::Element::int_;
		element.value.int_ = boost::numeric_cast<int>( val);
		return true;
	}
	catch (boost::bad_numeric_cast&){}
	return false;
}


template <typename ValueType>
typename boost::enable_if_c<boost::is_arithmetic<ValueType>::value && boost::is_unsigned<ValueType>::value && !boost::is_floating_point<ValueType>::value,bool>::type
valuePush( const ValueType& val, TypedInputFilter::Element& element) const
{
	try
	{
		element.type = TypedInputFilter::Element::uint_;
		element.value.uint_ = boost::numeric_cast<unsigned int>( val);
		return true;
	}
	catch (boost::bad_numeric_cast&){}
	return false;
}

}}}//namespace
#endif

