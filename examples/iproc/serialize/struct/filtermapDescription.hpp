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
///\file serialize/struct/filtermapDescription.hpp
///\brief Defines the bricks for the SDK to describe structures for mapping filters to forms in a readable way.
///\remark This module uses intrusive building blocks to build the serialization/deserialization of the direct map objects.
#ifndef _Wolframe_SERIALIZE_STRUCT_FILTERMAP_DESCRIPTION_HPP_INCLUDED
#define _Wolframe_SERIALIZE_STRUCT_FILTERMAP_DESCRIPTION_HPP_INCLUDED
#include "serialize/struct/filtermapBase.hpp"
#include "serialize/struct/filtermapTraits.hpp"
#include "serialize/struct/filtermapParse.hpp"
#include "serialize/struct/filtermapPrint.hpp"
#include "logger-v1.hpp"
#include <typeinfo>
#include <exception>

namespace _Wolframe {
namespace serialize {

template <class Element>
static const char* getTypename()
{
	const char* typ = 0;
	try
	{
		typ = typeid(Element).name();
	}
	catch (std::bad_typeid)
	{}
	return typ;
}

///\class Description
///\brief Intrusive configuration description
///\tparam Structure structure that is represented by this description
template <class Structure>
struct Description :public DescriptionBase
{
	///\brief Operator to build the configuration structure element by element
	///\tparam Element element type
	///\param[in] name name of the element
	///\param[in] eptr pointer to member of the element
	template <typename Element>
	Description& operator()( const char* name, Element Structure::*eptr)
	{
		DescriptionBase::Parse parse_ = &IntrusiveParser<Element>::parse;
		DescriptionBase::Print print_ = &IntrusivePrinter<Element>::print;
		DescriptionBase::IsAtomic isAtomic_ = &IntrusiveParser<Element>::isAtomic;

		std::size_t pp = (std::size_t)&(((Structure*)0)->*eptr);
		DescriptionBase e( getTypename<Element>(), pp, sizeof(Element), isAtomic_, parse_, print_);
		if (find( name) != end())
		{
			LOG_ERROR << "duplicate definition of " << name << " in structure";
		}
		define( name, e);
		return *this;
	}

	Description& operator--(int)
	{
		defineEndOfAttributes();
		return *this;
	}

	Description()
		:DescriptionBase( getTypename<Structure>(), 0, sizeof(Structure), &IntrusiveParser<Structure>::isAtomic, &IntrusiveParser<Structure>::parse, &IntrusivePrinter<Structure>::print){}
};

}}// end namespace
#endif

