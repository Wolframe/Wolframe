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
///\file serialize/struct/filtermapProperty.hpp
///\brief Defines the intrusive implementation of some parsing element properties
#ifndef _Wolframe_SERIALIZE_STRUCT_FILTERMAP_PROPERTY_HPP_INCLUDED
#define _Wolframe_SERIALIZE_STRUCT_FILTERMAP_PROPERTY_HPP_INCLUDED
#include "serialize/struct/filtermapTraits.hpp"

namespace _Wolframe {
namespace serialize {

template <typename T>
struct FiltermapIntrusiveProperty
{
private:
	static FiltermapDescriptionBase::ElementType type_( const traits::struct_&)
		{return FiltermapDescriptionBase::Struct;}
	static FiltermapDescriptionBase::ElementType type_( const traits::vector_&)
		{return FiltermapDescriptionBase::Vector;}
	static FiltermapDescriptionBase::ElementType type_( const traits::atomic_&)
		{return FiltermapDescriptionBase::Atomic;}
public:
	static FiltermapDescriptionBase::ElementType type()
	{
		const T* obj = 0;
		return type_( traits::getFiltermapCategory(*obj));
	}
};

}}//namespace
#endif
