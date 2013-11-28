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
///\file serialize/mapContext.hpp
///\brief Defines the data structure holding the global serialization state variables (without the stack)

#ifndef _Wolframe_SERIALIZE_MAPCONTEXT_HPP_INCLUDED
#define _Wolframe_SERIALIZE_MAPCONTEXT_HPP_INCLUDED
#include <string>
#include "filter/typedfilter.hpp"

namespace _Wolframe {
namespace serialize {

///\class Context
///\brief Global state variables of a running serialization/deserialization procedure (without the stack)
class Context
{
public:
	struct ElementBuffer
	{
		langbind::FilterBase::ElementType m_type;
		types::VariantConst m_value;

		ElementBuffer()
			:m_type(langbind::FilterBase::Value)
			{}
		ElementBuffer( const ElementBuffer& o)
			:m_type(o.m_type)
			,m_value(o.m_value)
			{}
	};
	enum Flags
	{
		None=0x00,
		ValidateAttributes=0x01,
		ValidateInitialization=0x02,
		SerializeWithIndices=0x04,
		CaseInsensitiveCompare=0x08
	};

	Context( Flags f=None);
	Context( const Context& o);
	~Context(){}

	void clear();

	bool flag( Flags f) const					{return ((int)f & (int)m_flags) == (int)f;}
	void setFlags( Flags f)						{int ff=(int)m_flags | (int)f; m_flags=(Flags)ff;}
	void unsetFlags( Flags f)					{int ff=(int)m_flags - ((int)m_flags & (int)f); m_flags=(Flags)ff;}

	void setElem( const ElementBuffer& e)
	{
		m_elem = e;
		m_has_elem = true;
	}

	void setElem( langbind::FilterBase::ElementType t)
	{
		m_elem.m_type = t;
		m_elem.m_value.init();
		m_has_elem = true;
	}

	void setElem( langbind::FilterBase::ElementType t, const types::VariantConst& v)
	{
		m_elem.m_type = t;
		m_elem.m_value = v;
		m_has_elem = true;
	}

	template <typename VALTYPE>
	void setElem_( langbind::FilterBase::ElementType t, const VALTYPE& val)
	{
		setElem( t, types::VariantConst( val));
	}

	const ElementBuffer* getElem()
	{
		if (m_has_elem)
		{
			m_has_elem = false;
			return &m_elem;
		}
		return 0;
	}

	void setElementUnconsumed()
	{
		m_has_elem = true;
	}

private:
	Flags m_flags;
	ElementBuffer m_elem;
	bool m_has_elem;
};


}}
#endif

