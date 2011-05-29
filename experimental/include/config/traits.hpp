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
/// \file config/traits.hpp
/// \brief defines the traits for the type categories for dispatching parse functions deoending on the value type

#ifndef _Wolframe_CONFIG_TRAITS_HPP_INCLUDED
#define _Wolframe_CONFIG_TRAITS_HPP_INCLUDED
#include <boost/lexical_cast.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits.hpp>
#include <boost/detail/select_type.hpp>
#include "config/descriptionBase.hpp"

namespace _Wolframe {
namespace config {
namespace traits
{
struct struct_ {};	///< category tag for a structure with named elements
struct vector_ {};	///< category tag for a std::vector of any type
struct atom_ {};	///< category tag for a type that is convertible from a string through boost::lexical_cast
struct pointer_ {};	///< category tag for a poiner type

/// \brief conditional template for detecting if a type is a class with a static/member method description() returning a const pointer to a structure description as defined in config/descriptionBase.hpp
/// see http://drdobbs.com/article/print?articleId=227500449&siteSectionName= "Checking Concept Without Concepts in C++"
template<typename T,bool is_class_type=boost::is_class<T>::value>
struct has_description_method: boost::false_type {};

template<typename T>
struct has_description_method_noprm
{
	typedef char small_type;
	struct large_type {small_type dummy[2];};

	template<const DescriptionBase* (T::*)()> struct tester_member_signature;
	template<const DescriptionBase* (*)()> struct tester_static_signature;

	template<typename U>
	static small_type has_matching_member(tester_member_signature<&U::description>*);
	template<typename U>
	static small_type has_matching_member(tester_static_signature<&U::description>*);
	template<typename U>
	static large_type has_matching_member(...);

	/// \brief value with the boolean property corresponding has description static/member method without paramater
	static const bool value=sizeof(has_matching_member<T>(0))==sizeof(small_type);
};

template<typename T>
struct has_description_method<T,true>:
	boost::integral_constant<bool, has_description_method_noprm<T>::value>
{};


/// \brief get category vector_ for a type
/// returns vector_ if  std::vector<T::value_type> EQUALS T. This is true, when T is a std::vector of any kind
template <typename T>
typename boost::enable_if_c<
	boost::is_same< std::vector< typename T::value_type> ,T>::value && !boost::is_same<std::string,T>::value
	,vector_>::type getCategory( const T&) { return vector_();}


/// \brief get category struct_ for a type
/// returns struct_ if T fulfills has a method description with no params returning a const pointer to a config::DescriptionBase
template <typename T>
typename boost::enable_if_c<
	has_description_method<T>::value
	,struct_>::type getCategory( const T&) { return struct_();}


/// \brief get category atom_ for a type
/// returns atom_ if T T fulfills the is_arithmetic condition or is a string
template <typename T>
typename boost::enable_if_c<
	boost::is_arithmetic<T>::value || boost::is_same<std::string,T>::value
	,atom_>::type getCategory( const T&) { return atom_();}

/// \brief get category pointer_ for a type
/// returns pointer_ if T T fulfills the is_pointer
template <typename T>
typename boost::enable_if_c<
	boost::is_pointer<T>::value
	,pointer_>::type getCategory( const T&) { return pointer_();}

}}}// end namespace
#endif
