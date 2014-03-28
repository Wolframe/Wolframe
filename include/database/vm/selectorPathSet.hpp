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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
//\file database/vm/selectorPathSet.hpp
//\brief Defines a set selector pathes
#ifndef _DATABASE_VIRTUAL_MACHINE_SELECTOR_PATH_SET_HPP_INCLUDED
#define _DATABASE_VIRTUAL_MACHINE_SELECTOR_PATH_SET_HPP_INCLUDED
#include "database/vm/instructionSet.hpp"
#include <map>
#include <vector>
#include <string>
#include <cstdlib>
#include "transactionfunction/InputStructure.hpp"
#include "transactionfunction/TagTable.hpp"

namespace _Wolframe {
namespace db {
namespace vm {

typedef TransactionFunctionInput::Structure::NodeVisitor NodeVisitor;
typedef TransactionFunctionInput::Structure::NodeVisitor::Index NodeIndex;
typedef TransactionFunctionInput::Structure InputStructure;

class SelectorPath
{
public:
	struct Element
	{
		enum Type
		{
			Root,		//< starting '/'
			Next,		//< '/element'
			Find,		//< '//element'
			Up		//< '..'
		};
	
		static const char* typeName( Type i)
		{
			static const char* ar[] ={"Root","Next","Find","Up"};
			return ar[(int)i];
		}
		Type m_type;
		int m_tag;

		Element()
			:m_type(Root),m_tag(0){}
		explicit Element( Type type_, int tag_=0)
			:m_type(type_),m_tag(tag_){}
		Element( const Element& o)
			:m_type(o.m_type),m_tag(o.m_tag){}
	};

	SelectorPath(){}
	SelectorPath( const std::string& selector, TagTable* tagmap);
	SelectorPath( const SelectorPath& o)				:m_path(o.m_path){}

	std::string tostring( const TagTable* tagmap) const;

	void selectNodes( const InputStructure& st, const NodeVisitor& nv, std::vector<NodeIndex>& ar) const;

	std::vector<Element>::const_iterator begin() const		{return m_path.begin();}
	std::vector<Element>::const_iterator end() const		{return m_path.end();}
	std::size_t size() const					{return m_path.size();}

private:
	std::vector<Element> m_path;
};


class SelectorPathSet
{
public:
	typedef InstructionSet::ArgumentIndex Index;

public:
	explicit SelectorPathSet()
		:m_tagtab(false){}
	SelectorPathSet( const SelectorPathSet& o)
		:m_tagtab(o.m_tagtab)
		,m_pathar(o.m_pathar){}

	const SelectorPath& getPath( const Index& idx) const
	{
		if (idx == 0) throw std::runtime_error( "accessing null selector path");
		if (idx >= m_pathar.size()) throw std::runtime_error( "selector path index out of bounds");
		return m_pathar.at( idx-1);
	}

	Index add( const std::string& selector)
	{
		m_pathar.push_back( SelectorPath( selector, &m_tagtab));
		return m_pathar.size();
	}

private:
	TagTable m_tagtab;
	std::vector<SelectorPath> m_pathar;
};

}}}//namespace
#endif

