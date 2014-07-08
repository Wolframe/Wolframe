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
///\brief Implementation of the audit function call parameter parsing
///\file tdl/auditElementReference.cpp
#include "tdl/auditElementReference.hpp"
#include "tdl/elementReference.hpp"
#include "tdl/parseUtils.hpp"
#include <string>
#include <vector>
#include <stdexcept>

using namespace _Wolframe;
using namespace _Wolframe::db;
using namespace _Wolframe::db::tdl;

AuditElementReference AuditElementReference::parse( const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	std::string name;

	bool nameDefined = parseNameAssignment( langdescr, name, si, se);
	ElementReference elem = ElementReference::parsePlainReference( langdescr, si, se);

	if (!nameDefined)
	{
		if (elem.type == ElementReference::SelectorPath)
		{
			name = extractImplicitNameFromSelector( elem.selector);
		}
		else if (elem.type == ElementReference::NamedSetElement)
		{
			name = elem.name;
		}
	}
	return AuditElementReference( name, elem);
}

