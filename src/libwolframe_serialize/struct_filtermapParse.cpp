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
///\file serialize_struct_filtermapParse.cpp
///\brief Implements some shared helpers of the intrusive implementation of the deserialization of objects interfaced as TypedInputFilter
#include "serialize/struct/filtermapParse.hpp"
#include "serialize/struct/filtermapParseStack.hpp"
#include "serialize/struct/filtermapParseValue.hpp"
#include "logger/logger-v1.hpp"

using namespace _Wolframe;
using namespace _Wolframe::serialize;

static bool emptycontent( const types::VariantConst& val)
{
	if (val.type() != types::Variant::string_) return false;
	std::size_t ii = 0, nn = val.charsize();
	const char* cc = val.charptr();

	for (; ii<nn; ++ii)
	{
		if ((unsigned char)cc[ii]> 32) return false;
	}
	return true;
}

bool serialize::parseValue_int( signed int* val, const types::VariantConst& element)
{
	return parseValue( *val, element);
}

bool serialize::parseValue_uint( unsigned int* val, const types::VariantConst& element)
{
	return parseValue( *val, element);
}

bool serialize::parseValue_ulong( unsigned long* val, const types::VariantConst& element)
{
	return parseValue( *val, element);
}

bool serialize::parseValue_long( signed long* val, const types::VariantConst& element)
{
	return parseValue( *val, element);
}

bool serialize::parseValue_short( signed short* val, const types::VariantConst& element)
{
	return parseValue( *val, element);
}

bool serialize::parseValue_ushort( unsigned short* val, const types::VariantConst& element)
{
	return parseValue( *val, element);
}

bool serialize::parseValue_char( signed char* val, const types::VariantConst& element)
{
	return parseValue( *val, element);
}

bool serialize::parseValue_uchar( unsigned char* val, const types::VariantConst& element)
{
	return parseValue( *val, element);
}

bool serialize::parseValue_float( float* val, const types::VariantConst& element)
{
	return parseValue( *val, element);
}

bool serialize::parseValue_double( double* val, const types::VariantConst& element)
{
	return parseValue( *val, element);
}

bool serialize::parseValue_string( std::string* val, const types::VariantConst& element)
{
	return parseValue( *val, element);
}

bool serialize::parseAtomicElementEndTag( langbind::TypedInputFilter& inp, Context&, FiltermapParseStateStack& stk)
{
	langbind::InputFilter::ElementType typ;
	types::VariantConst element;

	if (!inp.getNext( typ, element))
	{
		if (inp.state() == langbind::InputFilter::EndOfMessage) return false;
		throw SerializationErrorException( inp.getError(), element.tostring(), StructParser::getElementPath( stk));
	}
	LOG_DATA << "[C++ structure serialization parse] atomic element " << langbind::InputFilter::elementTypeName( typ) << " '" << element.tostring() << "'";

	if (typ == langbind::InputFilter::Value)
	{
		throw SerializationErrorException( "subsequent values assigned to atomic value in filter serialization", element.tostring(), StructParser::getElementPath( stk));
	}
	if (typ != langbind::InputFilter::CloseTag)
	{
		throw SerializationErrorException( "close tag expected after atomic element", element.tostring(), StructParser::getElementPath( stk));
	}
	stk.pop_back();
	return true;
}

bool serialize::parseObjectStruct( const StructDescriptionBase* descr, langbind::TypedInputFilter& inp, Context& ctx, FiltermapParseStateStack& stk)
{
	langbind::InputFilter::ElementType typ;
	types::VariantConst element;

	if (stk.hasbufvalue())
	{
		throw SerializationErrorException( "error in structure definition: structure without name");
	}
	if (!inp.getNext( typ, element))
	{
		if (inp.state() != langbind::InputFilter::Error) return false;
		throw SerializationErrorException( inp.getError(), element.tostring(), StructParser::getElementPath( stk));
	}

	LOG_DATA << "[C++ structure serialization parse] structure element " << langbind::InputFilter::elementTypeName( typ) << " '" << element.tostring() << "'";
	switch (typ)
	{
		case langbind::InputFilter::OpenTag:
		{
			StructDescriptionBase::Map::const_iterator itr;
			if (ctx.flag( Context::CaseInsensitiveCompare))
			{
				itr = descr->find_cis( element.tostring());
			}
			else
			{
				itr = descr->find( element.tostring());
			}
			if (itr == descr->end())
			{
				throw SerializationErrorException( "unknown element", element.tostring(), StructParser::getElementPath( stk), std::string(".. candidates are {") + descr->names(", ") + "}");
			}
			std::size_t idx = itr - descr->begin();
			if (idx < descr->nof_attributes())
			{
				if (ctx.flag( Context::ValidateAttributes))
				{
					throw SerializationErrorException( "attribute element expected", element.tostring(), StructParser::getElementPath( stk));
				}
			}
			if (itr->second.type() != StructDescriptionBase::Vector)
			{
				if (stk.back().selectElement( idx, descr->size()))
				{
					throw SerializationErrorException( "duplicate definition", element.tostring(), StructParser::getElementPath( stk));
				}
			}
			void* value = (char*)stk.back().value() + itr->second.ofs();
			if (itr->second.type() == StructDescriptionBase::Atomic)
			{
				stk.push_back( FiltermapParseState( 0, &parseAtomicElementEndTag, value));
			}
			stk.push_back( FiltermapParseState( itr->first.c_str(), itr->second.parse(), value));
			return true;
		}

		case langbind::InputFilter::Attribute:
		{
			StructDescriptionBase::Map::const_iterator itr;
			if (ctx.flag( Context::CaseInsensitiveCompare))
			{
				itr = descr->find_cis( element.tostring());
			}
			else
			{
				itr = descr->find( element.tostring());
			}
			if (itr == descr->end())
			{
				throw SerializationErrorException( "unknown attribute", element.tostring(), StructParser::getElementPath( stk));
			}
			std::size_t idx = itr - descr->begin();
			if (idx >= descr->nof_attributes())
			{
				if (ctx.flag( Context::ValidateAttributes))
				{
					throw SerializationErrorException( "content element expected", element.tostring(), StructParser::getElementPath( stk));
				}
			}
			if (itr->second.type() != StructDescriptionBase::Atomic)
			{
				throw SerializationErrorException( "atomic value expected for attribute", element.tostring(), StructParser::getElementPath( stk));
			}
			if (stk.back().selectElement( idx, descr->size()))
			{
				throw SerializationErrorException( "duplicate definition", element.tostring(), StructParser::getElementPath( stk));
			}
			stk.push_back( FiltermapParseState( itr->first.c_str(), itr->second.parse(), (char*)stk.back().value() + itr->second.ofs()));
			return true;
		}

		case langbind::InputFilter::Value:
		{
			StructDescriptionBase::Map::const_iterator itr = descr->find( "");
			if (itr == descr->end())
			{
				if (emptycontent( element)) return true;
				throw SerializationErrorException( "structure instead of value expected or definition of untagged value in structure must be available", element.tostring(), StructParser::getElementPath( stk));
			}
			std::size_t idx = itr - descr->begin();
			if (idx < descr->nof_attributes())
			{
				throw SerializationErrorException( "error in structure definition: defined untagged value as attribute in structure", StructParser::getElementPath( stk));
			}
			if (itr->second.type() != StructDescriptionBase::Vector)
			{
				if (stk.back().selectElement( idx, descr->size()))
				{
					throw SerializationErrorException( "duplicate definition", element.tostring(), StructParser::getElementPath( stk));
				}
			}
			void* value = (char*)stk.back().value() + itr->second.ofs();
			stk.push_back( FiltermapParseState( "", itr->second.parse(), value));
			stk.bufvalue( element);
			return true;
		}

		case langbind::InputFilter::CloseTag:
		{
			StructDescriptionBase::Map::const_iterator itr = descr->begin(),end=descr->end();
			for (;itr != end; ++itr)
			{
				if (itr->second.mandatory() && !stk.back().initCount( itr-descr->begin()))
				{
					throw SerializationErrorException( "undefined structure element", itr->first, StructParser::getElementPath( stk));
				}
			}
			if (ctx.flag( Context::ValidateInitialization))
			{
				for (;itr != end; ++itr)
				{
					if (!itr->second.optional() && !stk.back().initCount( itr-descr->begin()))
					{
						throw SerializationErrorException( "undefined structure element", itr->first, StructParser::getElementPath( stk));
					}
				}
			}
			stk.pop_back();
			return true;
		}
	}
	throw SerializationErrorException( "illegal state in parse structure", StructParser::getElementPath( stk));
}

bool serialize::parseObjectAtomic( ParseValue parseVal, langbind::TypedInputFilter& inp, Context&, FiltermapParseStateStack& stk)
{
	langbind::InputFilter::ElementType typ;
	types::VariantConst element;

	if (stk.hasbufvalue())
	{
		typ = langbind::InputFilter::Value;
		element = stk.valuebuf();
		stk.clearbuf();
	}
	else if (!inp.getNext( typ, element))
	{
		if (inp.state() != langbind::InputFilter::Error) return false;
		throw SerializationErrorException( inp.getError(), element.tostring(), StructParser::getElementPath( stk));
	}
	LOG_DATA << "[C++ structure serialization parse] atomic element " << langbind::InputFilter::elementTypeName( typ) << " '" << element.tostring() << "'";
	switch (typ)
	{
		case langbind::InputFilter::OpenTag:
		case langbind::InputFilter::Attribute:
			throw SerializationErrorException( "atomic value expected", StructParser::getElementPath( stk));

		case langbind::InputFilter::Value:
			if (!parseVal( stk.back().value(), element))
			{
				throw SerializationErrorException( "illegal type of atomic value", StructParser::getElementPath( stk));
			}
			stk.pop_back();
			return true;

		case langbind::InputFilter::CloseTag:
			element.init();
			if (!parseVal( stk.back().value(), element))
			{
				throw SerializationErrorException( "cannot convert empty value to expected atomic type", StructParser::getElementPath( stk));
			}
			stk.pop_back();
			if (stk.back().parse() == &parseAtomicElementEndTag)
			{
				stk.pop_back();
			}
			else
			{
				throw SerializationErrorException( "value expected after attribute", StructParser::getElementPath( stk));
			}
			return true;
	}
	throw SerializationErrorException( "illegal state in parse atomic value", StructParser::getElementPath( stk));
}

