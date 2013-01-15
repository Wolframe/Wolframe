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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\brief Type traits
///\file types/traits.hpp
#ifndef _TYPES_TRAITS_HPP_INCLUDED
#define _TYPES_TRAITS_HPP_INCLUDED

namespace _Wolframe {
namespace types {
namespace traits {

template<typename T>
struct is_back_insertion_sequence
{
	typedef char small_type;
	struct large_type {small_type dummy[2];};

	template<typename V,void (T::*)( const V&)> struct tester_member_signature;

	template<typename U>
	static small_type has_matching_member(tester_member_signature<typename U::value_type, &U::push_back>*);
	template<typename U>
	static large_type has_matching_member(...);

	///\brief value with the boolean property corresponding
	static const bool value=sizeof(has_matching_member<T>(0))==sizeof(small_type);
};

}}} //namespace
#endif
