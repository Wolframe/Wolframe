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
///\file serialize_ddl_filtermapParse.cpp

#include "serialize/ddl/filtermapDDLParse.hpp"
#include "filter/typedfilter.hpp"
#include "logger-v1.hpp"
#include <cstring>
#include <sstream>

using namespace _Wolframe;
using namespace serialize;

static std::string getElementPath( const FiltermapDDLParseStateStack& stk)
{
	std::string rt;
	FiltermapDDLParseStateStack::const_iterator itr=stk.begin(), end=stk.end();
	for (; itr != end; ++itr)
	{
		if (itr->name())
		{
			rt.append( "/");
			rt.append( itr->name());
		}
	}
	return rt;
}

// forward declarations
static bool parseObject( langbind::TypedInputFilter& inp, Context& ctx, std::vector<FiltermapDDLParseState>& stk);

enum AtomicValueState
{
	TagValueOpen,
	TagValueParsed,
	AttributeValueOpen
};

static void setAtomValue( ddl::AtomicType& val, const langbind::TypedFilterBase::Element element)
{
	switch (element.type)
	{
		case langbind::TypedFilterBase::Element::bool_:
			val.set( boost::lexical_cast<std::string>( element.value.bool_));
			break;
		case langbind::TypedFilterBase::Element::double_:
			val.set( boost::lexical_cast<std::string>( element.value.double_));
			break;
		case langbind::TypedFilterBase::Element::int_:
			val.set( boost::lexical_cast<std::string>( element.value.int_));
			break;
		case langbind::TypedFilterBase::Element::uint_:
			val.set( boost::lexical_cast<std::string>( element.value.uint_));
			break;
		case langbind::TypedFilterBase::Element::string_:
			val.set( std::string( element.value.string_.ptr, element.value.string_.size));
			break;
		case langbind::TypedFilterBase::Element::blob_:
			val.set( std::string( (const char*)element.value.blob_.ptr, element.value.blob_.size));
			break;
	}
}

static bool parseAtom( ddl::AtomicType& val, langbind::TypedInputFilter& inp, Context&, std::vector<FiltermapDDLParseState>& stk)
{
	langbind::InputFilter::ElementType typ;
	langbind::TypedFilterBase::Element element;

	if (!inp.getNext( typ, element))
	{
		if (inp.state() != langbind::InputFilter::Error) return false;
		throw SerializationErrorException( inp.getError(), element.tostring(), getElementPath( stk));
	}
	LOG_DATA << "[DDL structure serialization parse] atomic element " << langbind::InputFilter::elementTypeName( typ) << " '" << element.tostring() << "'";
	switch (typ)
	{
		case langbind::InputFilter::OpenTag:
			throw SerializationErrorException( "atomic value expected instead of tag", element.tostring(), getElementPath( stk));

		case langbind::InputFilter::Attribute:
			throw SerializationErrorException( "atomic value expected instead of attribute", element.tostring(), getElementPath( stk));

		case langbind::InputFilter::Value:
			if (stk.back().state() == TagValueParsed)
			{
				throw SerializationErrorException( "two subsequent values for atomic value", element.tostring(), getElementPath( stk));
			}
			setAtomValue( val, element);

			if (stk.back().state() == AttributeValueOpen)
			{
				stk.pop_back();
			}
			else
			{
				stk.back().state( TagValueParsed);
			}
			return true;

		case langbind::InputFilter::CloseTag:
			if (stk.back().state() == AttributeValueOpen)
			{
				throw SerializationErrorException( "missing attribute value", element.tostring(), getElementPath( stk));
			}
			else if (stk.back().state() == TagValueOpen)
			{
				val.set( std::string( ""));
			}
			stk.pop_back();
			return true;
	}
	throw SerializationErrorException( "illegal state in parse DDL form atomic value", getElementPath( stk));
}

static bool parseStruct( ddl::StructType& st, langbind::TypedInputFilter& inp, Context& ctx, std::vector<FiltermapDDLParseState>& stk)
{
	langbind::InputFilter::ElementType typ;
	langbind::TypedFilterBase::Element element;

	if (!inp.getNext( typ, element))
	{
		if (inp.state() != langbind::InputFilter::Error) return false;
		throw SerializationErrorException( inp.getError(), element.tostring(), getElementPath( stk));
	}
	LOG_DATA << "[DDL structure serialization parse] structure element " << langbind::InputFilter::elementTypeName( typ) << " '" << element.tostring() << "'";
	switch (typ)
	{
		case langbind::InputFilter::OpenTag:
		{
			ddl::StructType::Map::iterator itr = st.find( element.tostring());
			if (itr == st.end())
			{
				throw SerializationErrorException( "unknown tag ", element.tostring(), getElementPath( stk));
			}
			std::size_t idx = itr - st.begin();
			if (idx < st.nof_attributes())
			{
				if (ctx.flag( Context::ValidateAttributes))
				{
					throw SerializationErrorException( "attribute element expected for ", element.tostring(), getElementPath( stk));
				}
			}
			if (itr->second.contentType() != ddl::StructType::Vector && itr->second.initialized())
			{
				throw SerializationErrorException( "duplicate structure element definition", element.tostring(), getElementPath( stk));
			}
			itr->second.initialized(true);
			stk.push_back( FiltermapDDLParseState( itr->first.c_str(), &itr->second));
			if (itr->second.contentType() == ddl::StructType::Atomic)
			{
				stk.back().state( TagValueOpen);
			}
			return true;
		}

		case langbind::InputFilter::Attribute:
		{
			ddl::StructType::Map::iterator itr = st.find( element.tostring());
			if (itr == st.end())
			{
				throw SerializationErrorException( "unknown attribute ", element.tostring(), getElementPath( stk));
			}
			std::size_t idx = itr - st.begin();
			if (idx >= st.nof_attributes())
			{
				if (ctx.flag( Context::ValidateAttributes))
				{
					throw SerializationErrorException( "content element expected", element.tostring(), getElementPath( stk));
				}
			}
			if (itr->second.contentType() != ddl::StructType::Atomic)
			{
				throw SerializationErrorException( "atomic element expected", element.tostring(), getElementPath( stk));
			}
			if (itr->second.initialized(true))
			{
				throw SerializationErrorException( "duplicate structure attribute definition", element.tostring(), getElementPath( stk));
			}
			stk.push_back( FiltermapDDLParseState( itr->first.c_str(), &itr->second));
			stk.back().state( AttributeValueOpen);
			return true;
		}

		case langbind::InputFilter::Value:
		{
			ddl::StructType::Map::iterator itr = st.find( "");
			if (itr == st.end())
			{
				if (element.emptycontent()) return true;
				throw SerializationErrorException( "parsed untagged value, but no untagged value is defined for this structure", element.tostring(), getElementPath( stk));
			}
			std::size_t idx = itr - st.begin();
			if (idx < st.nof_attributes())
			{
				throw SerializationErrorException( "error in structure definition: defined untagged value as attribute in structure", element.tostring(), getElementPath( stk));
			}
			itr->second.initialized( true);
			switch (itr->second.contentType())
			{
				case ddl::StructType::Atomic:
					setAtomValue( itr->second.value(), element);
					return true;

				case ddl::StructType::Struct:
					throw SerializationErrorException( "atomic element or vector of atomic elements expected for untagged value in structure", element.tostring(), getElementPath( stk));

				case ddl::StructType::Indirection:
					throw SerializationErrorException( "atomic element or vector of atomic elements expected for untagged value in structure", element.tostring(), getElementPath( stk));

				case ddl::StructType::Vector:
					if (itr->second.prototype().contentType() == ddl::StructType::Atomic)
					{
						itr->second.push();
						setAtomValue( itr->second.back().value(), element);
						return true;
					}
					else
					{
						throw SerializationErrorException( "atomic element or vector of atomic elements expected for untagged value in structure", element.tostring(), getElementPath( stk));
					}
			}
		}

		case langbind::InputFilter::CloseTag:
		{
			ddl::StructType::Map::iterator itr = st.begin(), end = st.end();
			for (;itr != end; ++itr)
			{
				if (itr->second.mandatory() && !itr->second.initialized())
				{
					throw SerializationErrorException( "undefined mandatory structure element", itr->first, getElementPath( stk));
				}
				if (ctx.flag( Context::ValidateInitialization))
				{
					if (!itr->second.optional() && !itr->second.initialized())
					{
						throw SerializationErrorException( "schema validation failed: undefined non optional structure element", itr->first, getElementPath( stk));
					}
				}
			}
			stk.pop_back();
			return true;
		}
	}
	throw SerializationErrorException( "illegal state in parse DDL form structure", getElementPath( stk));
}


static bool parseObject( langbind::TypedInputFilter& inp, Context& ctx, std::vector<FiltermapDDLParseState>& stk)
{
	switch (stk.back().value()->contentType())
	{
		case ddl::StructType::Atomic:
		{
			return parseAtom( stk.back().value()->value(), inp, ctx, stk);
		}
		case ddl::StructType::Vector:
		{
			stk.back().value()->push();
			ddl::StructType* velem = &stk.back().value()->back();
			const char* velemname = stk.back().name();
			stk.pop_back();
			stk.push_back( FiltermapDDLParseState( velemname, velem));
			return true;
		}
		case ddl::StructType::Struct:
		{
			return parseStruct( *stk.back().value(), inp, ctx, stk);
		}
		case ddl::StructType::Indirection:
		{
			stk.back().value()->expandIndirection();
			if (stk.back().value()->contentType() == ddl::StructType::Indirection)
			{
				throw SerializationErrorException( "indirection expanding to inderection", getElementPath( stk));
			}
			return parseObject( inp, ctx, stk);
		}
	}
	throw SerializationErrorException( "illegal state in parse DDL form object", getElementPath( stk));
}

DDLStructParser::DDLStructParser( ddl::StructType* st)
	:m_st(st)
{
	m_stk.push_back( FiltermapDDLParseState( 0, st));
}

DDLStructParser::DDLStructParser( const DDLStructParser& o)
	:m_st(o.m_st)
	,m_ctx(o.m_ctx)
	,m_inp(o.m_inp)
	,m_stk(o.m_stk)
	{}

DDLStructParser& DDLStructParser::operator=( const DDLStructParser& o)
{
	m_st = o.m_st;
	m_ctx = o.m_ctx;
	m_inp = o.m_inp;
	m_stk = o.m_stk;
	return *this;
}

void DDLStructParser::init( const langbind::TypedInputFilterR& i, Context::Flags flags)
{
	m_inp = i;
	m_ctx.clear();
	m_ctx.setFlags(flags);
	m_stk.clear();
	m_stk.push_back( FiltermapDDLParseState( 0, m_st));
}

bool DDLStructParser::call()
{
	bool rt = true;
	if (!m_inp.get()) throw std::runtime_error( "no input for parse");

	while (rt && m_stk.size())
	{
		rt = parseObject( *m_inp, m_ctx, m_stk);
	}
	return rt;
}

