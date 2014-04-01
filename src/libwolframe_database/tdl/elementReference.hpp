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
///\brief Definition of parsing results of atomic element references in subroutine calls or embedded database statements
///\file tdl/elementReference.hpp
#ifndef _DATABASE_TDL_ELEMENT_REFERENCE_HPP_INCLUDED
#define _DATABASE_TDL_ELEMENT_REFERENCE_HPP_INCLUDED
#include "database/databaseLanguage.hpp"
#include <string>
#include <vector>
#include <cstdlib>

namespace _Wolframe {
namespace db {
namespace tdl {

struct ElementReference
{
	enum Type 
	{
		SelectorPath,
		LoopCounter,
		Constant,
		NamedSetElement,
		IndexSetElement
	};
	Type type;
	std::string selector;
	std::string name;
	std::size_t index;

	ElementReference()
		:type(SelectorPath){}
	explicit ElementReference( Type t)
		:type(t),index(0){}
	ElementReference( Type t, const std::string& s)
		:type(t),selector(s){}
	ElementReference( Type t, const std::string& s, std::size_t i)
		:type(t),selector(s),index(i){}
	ElementReference( Type t, const std::string& s, const std::string& n)
		:type(t),selector(s),name(n){}
	ElementReference( const ElementReference& o)
		:type(o.type),selector(o.selector),name(o.name),index(o.index){}

	static ElementReference parseEmbeddedReference( const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se);
	static ElementReference parsePlainReference( const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se);
};

}}}//namespace
#endif

