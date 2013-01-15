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
///\file serialize/struct/filtermapBase.hpp
///\brief Defines the non intrusive base class of serialization/deserialization of objects interfaced as TypedInputFilter/TypedOutputFilter.
#ifndef _Wolframe_SERIALIZE_STRUCT_FILTERMAP_BASE_HPP_INCLUDED
#define _Wolframe_SERIALIZE_STRUCT_FILTERMAP_BASE_HPP_INCLUDED
#include "filter/typedfilter.hpp"
#include "filter/bufferingfilter.hpp"
#include "serialize/mapContext.hpp"
#include "serialize/struct/filtermapParseStack.hpp"
#include "serialize/struct/filtermapSerializeStack.hpp"
#include <cstddef>
#include <string>
#include <vector>
#include <limits>

namespace _Wolframe {
namespace serialize {

class StructDescriptionBase
{
public:
	enum ElementType
	{
		Atomic,
		Struct,
		Vector
	};

	typedef std::vector<std::pair<std::string,StructDescriptionBase> > Map;
	typedef bool (*Parse)( langbind::TypedInputFilter& flt, Context& ctx, FiltermapParseStateStack& stk);
	typedef bool (*Fetch)( Context& ctx, FiltermapSerializeStateStack& stk);
	typedef bool (*Constructor)( void* obj);
	typedef void (*Destructor)( void* obj);

	Parse parse() const		{return m_parse;}
	Fetch fetch() const		{return m_fetch;}

	StructDescriptionBase( Constructor c, Destructor d, const char* tn, std::size_t os, std::size_t sz, ElementType t, Parse pa, Fetch pr, bool mandatory_)
		:m_constructor(c)
		,m_destructor(d)
		,m_typename(tn)
		,m_ofs(os)
		,m_size(sz)
		,m_nof_attributes(0)
		,m_type(t)
		,m_parse(pa)
		,m_fetch(pr)
		,m_mandatory(mandatory_){}

	StructDescriptionBase( const char* tn, std::size_t os, std::size_t sz, ElementType t, Parse pa, Fetch pr, bool mandatory_)
		:m_constructor(0)
		,m_destructor(0)
		,m_typename(tn)
		,m_ofs(os)
		,m_size(sz)
		,m_nof_attributes(0)
		,m_type(t)
		,m_parse(pa)
		,m_fetch(pr)
		,m_mandatory(mandatory_){}

	StructDescriptionBase( const StructDescriptionBase& o)
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
		,m_mandatory(o.m_mandatory){}

	StructDescriptionBase()
		:m_typename(0)
		,m_ofs(0)
		,m_size(0)
		,m_nof_attributes(0)
		,m_type(Atomic)
		,m_parse(0)
		,m_fetch(0)
		,m_mandatory(false){}

	bool parse( void* obj, langbind::TypedInputFilter& in, Context& ctx, FiltermapParseStateStack& stk) const;

	///\brief Initializes an atomic element in a structure
	///\remark Not very efficient implementation
	bool setAtomicValue( void* obj, std::size_t idx, const std::string& value) const;

	bool init( void* obj) const
	{
		return (m_constructor)?m_constructor( obj):true;
	}

	void done( void* obj) const
	{
		if (m_destructor) m_destructor( obj);
	}

	std::size_t size() const
	{
		return m_size;
	}

	std::size_t ofs() const
	{
		return m_ofs;
	}

	ElementType type() const
	{
		return m_type;
	}

	Map::const_iterator find( const std::string& name) const
	{
		for (Map::const_iterator itr = m_elem.begin(); itr!=m_elem.end(); ++itr)
		{
			if (itr->first == name) return itr;
		}
		return m_elem.end();
	}

	Map::const_iterator begin() const {return m_elem.begin();}
	Map::const_iterator end() const {return m_elem.end();}

	void define( const std::string& name, const StructDescriptionBase& dd)
	{
		m_elem.push_back( Map::value_type(name,dd));
	}

	///\brief Get the number of attributes of a struct
	///\remark returns 0 this is not of type (ContentType) Struct
	///\return the number of attributes or 0
	std::size_t nof_attributes() const
	{
		return m_nof_attributes;
	}

	///\brief Define the number of attributes of a struct
	///\return the number of attributes or 0
	///\remark returns 0 this is not of type Struct
	void defineEndOfAttributes()
	{
		m_nof_attributes = m_elem.size();
	}

	///\brief Get the number of elements in the structure or array
	///\return the number of elements or 0 in case of an atomic value
	std::size_t nof_elements() const
	{
		return m_elem.size();
	}

	const char* typeName() const
	{
		return m_typename;
	}

	///\brief Find out if the element in the structure is mandatory
	///\return true, if yes
	bool mandatory() const				{return m_mandatory;}

private:
	Constructor m_constructor;
	Destructor m_destructor;
	const char* m_typename;
	std::size_t m_ofs;
	std::size_t m_size;
	std::size_t m_nof_attributes;
	Map m_elem;
	ElementType m_type;
	Parse m_parse;
	Fetch m_fetch;
	bool m_mandatory;
};


class StructSerializer :public langbind::TypedInputFilter
{
public:
	typedef boost::shared_ptr<void> ObjectReference;

	StructSerializer( const ObjectReference& obj, const StructDescriptionBase* descr);
	StructSerializer( const void* obj, const StructDescriptionBase* descr);

	StructSerializer( const StructSerializer& o);
	virtual ~StructSerializer(){}

	static std::string getElementPath( const FiltermapSerializeStateStack& stk);

	void init( const langbind::TypedOutputFilterR& out, Context::Flags flags=Context::None);
	void reset();

	bool call();

	///\brief Get a self copy
	///\return allocated pointer to copy of this
	virtual langbind::TypedInputFilter* copy() const;

	///\brief Implements langbind::TypedInputFilter::getNext(langbind::FilterBase::ElementType&,langbind::TypedFilterBase::Element&)
	virtual bool getNext( langbind::FilterBase::ElementType& type, langbind::TypedFilterBase::Element& value);

private:
	const void* m_ptr;
	const ObjectReference m_obj;
	const StructDescriptionBase* m_descr;
	Context m_ctx;
	langbind::TypedOutputFilterR m_out;
	FiltermapSerializeStateStack m_stk;
};


class StructParser
{
public:
	typedef boost::shared_ptr<void> ObjectReference;

	StructParser( void* obj, const StructDescriptionBase* descr);
	StructParser( const ObjectReference& obj, const StructDescriptionBase* descr);
	StructParser( const StructParser& o);
	virtual ~StructParser(){}

	static std::string getElementPath( const FiltermapParseStateStack& stk);

	void init( const langbind::TypedInputFilterR& i, Context::Flags flags=Context::None);

	const ObjectReference& object() const					{return m_obj;}
	const StructDescriptionBase* descr() const				{return m_descr;}

	bool call();

private:
	void* m_ptr;
	ObjectReference m_obj;
	const StructDescriptionBase* m_descr;
	Context m_ctx;
	langbind::TypedInputFilterR m_inp;
	FiltermapParseStateStack m_stk;
};

}}//namespace
#endif

