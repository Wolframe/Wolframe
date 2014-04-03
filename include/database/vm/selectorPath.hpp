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
//\file database/vm/selectorPath.hpp
//\brief Defines an input selector path
#ifndef _DATABASE_VIRTUAL_MACHINE_SELECTOR_PATH_HPP_INCLUDED
#define _DATABASE_VIRTUAL_MACHINE_SELECTOR_PATH_HPP_INCLUDED
#include <map>
#include <vector>
#include <string>
#include <cstdlib>
#include <iostream>

namespace _Wolframe {
namespace db {

namespace tf {
//\brief Forward declaration
class InputNodeVisitor;
//\brief Forward declaration
class InputStructure;
//\brief Forward declaration
typedef int InputNodeIndex;
//\brief Forward declaration
class TagTable;
}//namespace

namespace vm {

class SelectorPath
{
public:
	struct Element
	{
		enum Type
		{
			Root,		//< starting '/'
			Current,	//< starting './' (optional, e.g. not Root)
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
	SelectorPath( const std::string& selector, tf::TagTable* tagmap);
	SelectorPath( const SelectorPath& o)				:m_path(o.m_path){}

	std::string tostring( const tf::TagTable* tagmap) const;
	static std::string normalize( const std::string& pathstr);

	void selectNodes( const tf::InputStructure& st, const tf::InputNodeVisitor& nv, std::vector<tf::InputNodeIndex>& ar) const;

	typedef std::vector<Element>::const_iterator const_iterator;
	typedef std::vector<Element>::iterator iterator;

	std::vector<Element>::iterator begin()				{return m_path.begin();}
	std::vector<Element>::iterator end()				{return m_path.end();}

	std::vector<Element>::const_iterator begin() const		{return m_path.begin();}
	std::vector<Element>::const_iterator end() const		{return m_path.end();}
	std::size_t size() const					{return m_path.size();}

	void print( std::ostream& out, const tf::TagTable* tagmap) const;

private:
	std::vector<Element> m_path;
};

}}}//namespace
#endif

