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
///\file serialize/directmapDescription.hpp
///\brief Defines the bricks for the SDK to describe the direct map structures in a readable way.
///\remark This module uses intrusive building blocks to build the serialization/deserialization of the direct map objects.
#ifndef _Wolframe_SERIALIZE_DIRECTMAP_DESCRIPTION_HPP_INCLUDED
#define _Wolframe_SERIALIZE_DIRECTMAP_DESCRIPTION_HPP_INCLUDED
#include "serialize/directmapBase.hpp"
#include "serialize/directmapTraits.hpp"
#include "serialize/directmapParse.hpp"
#include "serialize/directmapPrint.hpp"
#include <typeinfo>
#include <exception>

namespace _Wolframe {
namespace serialize {

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
    const char* type = 0;
    try
    {
      type = typeid(Element).name();
    }
    catch (std::bad_typeid)
    {}
    DescriptionBase::Parse parse_ = &_Wolframe::serialize::Description<Element>::parse;
    DescriptionBase::Print print_ = _Wolframe::serialize::Description<Element>::print;
    DescriptionBase::IsAtomic isAtomic_ = &_Wolframe::serialize::Description<Element>::isAtomic;

    std::size_t pp = (std::size_t)&(((Structure*)0)->*eptr);
    Item e( pp, std::string(name), type, parse_, print_, isAtomic_);
    m_ar.push_back( e);
    return *this;
  }
  Description(){}
};

}}// end namespace
#endif

