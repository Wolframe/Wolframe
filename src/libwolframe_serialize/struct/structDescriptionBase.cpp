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
///\file struct/structDescriptionBase.cpp
///\brief Implements the non intrusive base class of serialization/deserialization
#include "serialize/struct/structDescriptionBase.hpp"
#include "serialize/struct/structDescription.hpp"
#include "filter/typingfilter.hpp"
#include <cstring>
#include <stdexcept>

using namespace _Wolframe;
using namespace serialize;

class OneElementTypedInputFilter
	:public langbind::TypedInputFilter
{
public:
	OneElementTypedInputFilter( const std::string& value)
		:utils::TypeSignature("serialize::OneElementTypedInputFilter", __LINE__)
		,langbind::TypedInputFilter("singlevalue")
		,m_value(value)
		,m_consumed(false){}
	OneElementTypedInputFilter( const OneElementTypedInputFilter& o)
		:utils::TypeSignature(o)
		,langbind::TypedInputFilter(o)
		,m_value(o.m_value)
		,m_consumed(o.m_consumed){}

	virtual TypedInputFilter* copy() const	{return new OneElementTypedInputFilter(*this);}

	virtual bool getNext( ElementType& type, types::VariantConst& element)
	{
		if (m_consumed) return false;
		type = TypedInputFilter::Value;
		element = m_value;
		m_consumed = true;
		return true;
	}
private:
	std::string m_value;
	bool m_consumed;
};

StructDescriptionBase::StructDescriptionBase( Constructor c, Destructor d, const char* tn, std::size_t os, std::size_t sz, ElementType t, Parse pa, Fetch pr, ElementRequirement requirement_)
	:m_constructor(c)
	,m_destructor(d)
	,m_typename(tn)
	,m_ofs(os)
	,m_size(sz)
	,m_nof_attributes(0)
	,m_type(t)
	,m_parse(pa)
	,m_fetch(pr)
	,m_requirement(requirement_){}

StructDescriptionBase::StructDescriptionBase( const char* tn, std::size_t os, std::size_t sz, ElementType t, Parse pa, Fetch pr, ElementRequirement requirement_)
	:m_constructor(0)
	,m_destructor(0)
	,m_typename(tn)
	,m_ofs(os)
	,m_size(sz)
	,m_nof_attributes(0)
	,m_type(t)
	,m_parse(pa)
	,m_fetch(pr)
	,m_requirement(requirement_){}

StructDescriptionBase::StructDescriptionBase( const StructDescriptionBase& o)
	:m_constructor(o.m_constructor)
	,m_destructor(o.m_destructor)
	,m_typename(o.m_typename)
	,m_ofs(o.m_ofs)
	,m_size(o.m_size)
	,m_nof_attributes(o.m_nof_attributes)
	,m_elem(o.m_elem)
	,m_type(o.m_type)
	,m_parse(o.m_parse)
	,m_fetch(o.m_fetch)
	,m_requirement(o.m_requirement){}

StructDescriptionBase::StructDescriptionBase()
	:m_typename(0)
	,m_ofs(0)
	,m_size(0)
	,m_nof_attributes(0)
	,m_type(Atomic)
	,m_parse(0)
	,m_fetch(0)
	,m_requirement(NoRequirement){}

StructDescriptionBase::Map::const_iterator StructDescriptionBase::find( const std::string& name) const
{
	for (Map::const_iterator itr = m_elem.begin(); itr!=m_elem.end(); ++itr)
	{
		if (itr->first == name) return itr;
	}
	return m_elem.end();
}

static bool stringcmp_cis( const std::string& ths, const std::string& oth)
{
	std::size_t nn = ths.size();
	if (nn != oth.size()) return false;

	const char* aa = ths.c_str();
	const char* bb = oth.c_str();
	std::size_t kk = 0;
	for (; kk<nn; kk++)
	{
		if ((unsigned char)aa[kk] <= 127)
		{
			if ((aa[kk]|32) != (bb[kk]|32)) break;
		}
		else
		{
			if (aa[kk] != bb[kk]) break;
		}
	}
	return (kk == nn);
}

StructDescriptionBase::Map::const_iterator StructDescriptionBase::find_cis( const std::string& name) const
{
	for (Map::const_iterator itr = m_elem.begin(); itr!=m_elem.end(); ++itr)
	{
		if (stringcmp_cis( itr->first, name)) return itr;
	}
	return m_elem.end();
}

std::string StructDescriptionBase::names( const char* sep) const
{
	std::string rt;
	for (Map::const_iterator itr = m_elem.begin(); itr!=m_elem.end(); ++itr)
	{
		if (itr != m_elem.begin()) rt.append( sep);
		rt.append( itr->first);
	}
	return rt;
}

bool StructDescriptionBase::setAtomicValue( void* obj, std::size_t idx, const std::string& value) const
{
	if (idx >= nof_elements()) throw std::runtime_error( "structure element index out of range");

	Map::const_iterator itr = begin() + idx;
	void* objelemptr = (char*)obj + itr->second.ofs();
	if (itr->second.type() != StructDescriptionBase::Atomic)
	{
		throw std::runtime_error( "atomic value expected for call of set atomic value");
	}
	Context ctx;
	ParseStateStack stk;
	OneElementTypedInputFilter inp( value);
	stk.push_back( ParseState( itr->first.c_str(), itr->second.parse(), objelemptr));
	return stk.back().parse()( inp, ctx, stk);
}

const serialize::StructDescriptionBase* EmptyStruct::getStructDescription( )
{
	static serialize::StructDescription<EmptyStruct> rt;
	return &rt;
}

