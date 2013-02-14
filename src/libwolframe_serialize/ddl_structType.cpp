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
///\file ddl_structType.cpp
///\brief implementation of the form DDL generic type
#include "ddl/structType.hpp"
#include "utils/miscUtils.hpp"

using namespace _Wolframe;
using namespace ddl;

void StructType::REQUIRE( ContentType t) const
{
	if (m_contentType != t)
	{
		switch(t)
		{
			case Vector: throw std::logic_error( "not defined as vector");
			case Struct: throw std::logic_error( "not defined as structure");
			case Indirection: throw std::logic_error( "not defined as indirection");
			case Atomic: throw std::logic_error( "not defined as atomic");
		}
	}
}

StructType::Map::const_iterator StructType::find( const char* name) const
{
	REQUIRE(Struct);
	for (Map::const_iterator itr = m_elem.begin(); itr!=m_elem.end(); ++itr)
	{
		if (std::strcmp( itr->first.c_str(), name) == 0) return itr;
	}
	return m_elem.end();
}

StructType::Map::const_iterator StructType::find( const std::string& name) const
{
	return find( name.c_str());
}

StructType::Map::iterator StructType::find( const char* name)
{
	REQUIRE(Struct);
	for (Map::iterator itr = m_elem.begin(); itr!=m_elem.end(); ++itr)
	{
		if (std::strcmp( itr->first.c_str(), name) == 0) return itr;
	}
	return m_elem.end();
}

StructType::Map::iterator StructType::find( const std::string& name)
{
	return find( name.c_str());
}

const StructType* StructType::select( const char* name) const
{
	StructType::Map::const_iterator si = this->find( name);
	if (si == this->end()) throw std::runtime_error( std::string( "addressed form structure element in path not found: '") + (name?name:"") + "'");
	return &si->second;
}

StructType* StructType::select( const char* name)
{
	StructType::Map::iterator si = this->find( name);
	if (si == this->end()) throw std::runtime_error( std::string( "addressed form structure element in path not found: '") + (name?name:"") + "'");
	initialized(true);
	si->second.initialized(true);
	return &si->second;
}

StructType::Map::const_iterator StructType::begin() const
{
	if (m_contentType == Atomic) throw std::logic_error( "defined as atomic");
	return (m_contentType == Vector)?(m_elem.begin()+1):m_elem.begin();
}

StructType::Map::const_iterator StructType::end() const
{
	if (m_contentType == Atomic) throw std::logic_error( "defined as atomic");
	return m_elem.end();
}

std::size_t StructType::nof_elements() const
{
	return (m_contentType==Vector)?(m_elem.size()-1):m_elem.size();
}

StructType::Map::iterator StructType::begin()
{
	if (m_contentType == Atomic) throw std::logic_error( "defined as atomic");
	return (m_contentType == Vector)?(m_elem.begin()+1):m_elem.begin();
}

StructType::Map::iterator StructType::end()
{
	if (m_contentType == Atomic) throw std::logic_error( "defined as atomic");
	return m_elem.end();
}

AtomicType& StructType::value()
{
	REQUIRE(Atomic);
	return m_value;
}

const AtomicType& StructType::value() const
{
	REQUIRE(Atomic);
	return m_value;
}

void StructType::defineContent( const char* name, const StructType& dd)
{
	REQUIRE(Struct);
	m_elem.push_back( Element( name, dd));
}

void StructType::inheritContent( const StructType& val)
{
	REQUIRE(Struct);
	val.REQUIRE(Struct);
	Map::const_iterator vi,fi;
	std::size_t idx;
	for (vi = val.m_elem.begin(),idx=0; vi != val.m_elem.end(); ++vi,++idx)
	{
		fi = find( vi->first);
		if (fi != m_elem.end()) throw std::runtime_error( std::string("in structure inherited duplicate definition of element '") + vi->first + "'");
		if (idx < val.m_nof_attributes)
		{
			defineAttribute( vi->first, vi->second);
		}
		else
		{
			defineContent( vi->first, vi->second);
		}
	}
}

void StructType::defineContent( const std::string& name, const StructType& dd)
{
	defineContent( name.c_str(), dd);
}

void StructType::defineAttribute( const char* name, const StructType& dd)
{
	REQUIRE(Struct);
	dd.REQUIRE(Atomic);
	m_elem.insert( m_elem.begin()+m_nof_attributes, Element( name, dd));
	m_nof_attributes += 1;
}

void StructType::defineAttribute( const std::string& name, const StructType& dd)
{
	defineAttribute( name.c_str(), dd);
}

void StructType::defineAsVector( const StructType& prototype_)
{
	if (m_contentType == Vector) throw std::logic_error( "prototype of vector defined");
	if (m_contentType == Atomic) throw std::logic_error( "defined as atomic");
	if (m_elem.size()) throw std::logic_error( "defined as structure");
	m_contentType = Vector;
	m_elem.push_back( Element( std::string(), prototype_));
}

void StructType::defineAsAtomic( const AtomicType& tp)
{
	if (m_contentType == Vector) throw std::logic_error( "element already defined as vector");
	if (m_elem.size()) throw std::logic_error( "element already initialized as non empty structure. cannot redefine type anymore");
	m_value = tp;
	m_contentType = Atomic;
}

void StructType::defineAsIndirection( const IndirectionConstructorR& ref)
{
	if (m_contentType == Vector) throw std::logic_error( "element already defined as vector");
	if (m_elem.size()) throw std::logic_error( "element already initialized as non empty structure. cannot redefine type anymore");
	m_indirection = ref;
	m_contentType = Indirection;
}

void StructType::push()
{
	REQUIRE(Vector);
	m_elem.push_back( m_elem[0]);
}

StructType& StructType::back()
{
	if (m_contentType == Atomic) throw std::logic_error( "not defined as vector or structure");
	if (m_contentType == Vector && m_elem.size() <= 1) throw std::runtime_error( "no last element defined");
	return m_elem.back().second;
}

const StructType& StructType::back() const
{
	if (m_contentType == Atomic) throw std::logic_error( "not defined as vector or structure");
	if (m_contentType == Vector && m_elem.size() <= 1) throw std::runtime_error( "no last element defined");
	return m_elem.back().second;
}

StructType& StructType::prototype()
{
	REQUIRE(Vector);
	return m_elem[0].second;
}

const StructType& StructType::prototype() const
{
	REQUIRE(Vector);
	return m_elem[0].second;
}

std::size_t StructType::nof_attributes() const
{
	return m_nof_attributes;
}

void StructType::clear()
{
	m_contentType = Struct;
	m_value.clear();
	m_elem.clear();
	m_nof_attributes = 0;
}

void StructIndirectionConstructor::substituteSelf( StructType* st, const IndirectionConstructorR& self)
{
	switch (st->contentType())
	{
		case StructType::Atomic:
			break;
		case StructType::Vector:
			substituteSelf( &st->prototype(), self);
			/* no break here! */
		case StructType::Struct:
		{
			StructType::Map::iterator ii = st->begin(),ee=st->end();
			for (; ii != ee; ++ii)
			{
				substituteSelf( &ii->second, self);
			}
			break;
		}
		case StructType::Indirection:
			if (!st->indirection().get())
			{
				st->indirection() = self;
			}
	}
}

StructType* StructIndirectionConstructor::create( const IndirectionConstructorR& self) const
{
	StructType* rt = new StructType( m_prototype);
	try
	{
		substituteSelf( rt, self);
	}
	catch (const std::exception& e)
	{
		delete rt;
		throw e;
	}
	return rt;
}

IndirectionConstructorR& StructType::indirection()
{
	REQUIRE(Indirection);
	return m_indirection;
}

const IndirectionConstructorR& StructType::indirection() const
{
	REQUIRE(Indirection);
	return m_indirection;
}

void StructType::expandIndirection()
{
	REQUIRE(Indirection);
	types::CountedReference<StructType> st( m_indirection->create( m_indirection));
	*this = *st.get();
}

void StructType::print( std::ostream& out, size_t level) const
{
	std::string indent( level, '\t');
	switch (m_contentType)
	{
		case Atomic:
		{
			out << indent << "'" << m_value.value() << "'" << std::endl;
			break;
		}
		case Vector:
		{
			StructType::Map::const_iterator ii = begin(),ee=end();
			int idx = 0;
			for (; ii != ee; ++ii)
			{
				if (ii->second.m_contentType == Atomic)
				{
					out << indent << (idx++) << ":";
					out << "'" << ii->second.value().value() << "'" << std::endl;
				}
				else
				{
					out << indent << (idx++) << ":" << std::endl << indent << "{" << std::endl;
					ii->second.print( out, level+1);
					out << indent << "}" << std::endl;
				}
			}
			break;
		}
		case Struct:
		{
			StructType::Map::const_iterator ii = begin(),ee=end();
			for (; ii != ee; ++ii)
			{
				if (ii->second.m_contentType == Atomic)
				{
					out << indent << ii->first << "= ";
					out << "'" << ii->second.value().value() << "'" << std::endl;
				}
				else if (ii->second.m_contentType == Vector)
				{
					out << indent << ii->first << "[]" << std::endl;
					out << indent << "prototype:" << std::endl;
					ii->second.prototype().print( out, level+1);
					out << indent << "{" << std::endl;
					ii->second.print( out, level+1);
					out << indent << "}" << std::endl;
				}
				else
				{
					out << indent << ii->first << std::endl << indent << "{" << std::endl;
					ii->second.print( out, level+1);
					out << indent << "}" << std::endl;
				}
			}
			break;
		}
		case Indirection:
			out << "[indirection]" << std::endl;
	}
}

void StructType::printDescriptionUI( std::ostream& out) const
{
	bool cm = true;
	switch (m_contentType)
	{
		case Indirection:
		case Atomic:
		{
			break;
		}
		case Vector:
		{
			prototype().printDescriptionUI( out);
			break;
		}
		case Struct:
		{
			StructType::Map::const_iterator ii = begin(),ee=end();
			std::size_t idx;
			for (idx=0; ii != ee; ++ii,++idx)
			{
				if (!ii->first.empty())
				{
					if (!cm) out << ",";
					if (idx < m_nof_attributes) out << "@";
					out << ii->first;
					cm = true;
				}
				if (ii->second.m_contentType == Struct)
				{
					if (!cm) out << ",";
					out << "{";
					ii->second.printDescriptionUI( out);
					out << "}";
				}
				cm = false;
			}
			break;
		}
	}
}

void Form::print( std::ostream& out, size_t level) const
{
	std::string indent( level, '\t');
	out << indent << "FORM " << m_name << std::endl;
	StructType::print( out, level);
}

void Form::printDescriptionUI( std::ostream& out) const
{
	out << m_name << "." << m_ddlname << ": ";
	StructType::printDescriptionUI( out);
}
