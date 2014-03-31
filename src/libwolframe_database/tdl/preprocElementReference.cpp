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
///\brief Implementation of the preprocessing function call parameter parsing
///\file tdl/preprocElementReference.cpp
#include "tdl/preprocElementReference.hpp"
#include "tdl/embeddedStatement.hpp"
#include "tdl/elementReference.hpp"
#include "tdl/parseUtils.hpp"
#include <string>
#include <vector>
#include <stdexcept>

using namespace _Wolframe;
using namespace _Wolframe::db;
using namespace _Wolframe::db::tdl;

PreProcElementReference PreProcElementReference::parse( const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	PreProcElementReference rt;
	std::string tok;
	char ch;
	std::string::const_iterator start = si;

	ch = gotoNextToken( langdescr, si, se);
	if (isAlphaNumeric( ch) || ch == '.')
	{
		//... try to parse assignment first
		parseNextToken( langdescr, tok, si, se);
		ch = gotoNextToken( langdescr, si, se);
		if (ch == '=')
		{
			rt.name = tok;
			++si;
			ch = gotoNextToken( langdescr, si, se);
		}
		else
		{
			//... no assignment found -> fallback
			si = start;
		}
	}
	ElementReference elem = ElementReference::parsePlainReference( langdescr, si, se);
	switch (elem.type)
	{
		case ElementReference::SelectorPath:
			rt.value = elem.selector;
			rt.type = PreProcElementReference::SelectorPath;
			break;
		case ElementReference::LoopCounter:
			rt.type = PreProcElementReference::LoopCounter;
			break;
		case ElementReference::Constant:
			rt.value = elem.selector;
			rt.type = PreProcElementReference::Constant;
			break;
		case ElementReference::NamedSetElement:
		case ElementReference::IndexSetElement:
			throw std::runtime_error("illegal element reference in preprocessing statement");
	}
	return rt;
}



