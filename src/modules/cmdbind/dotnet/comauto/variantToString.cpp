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
//\file comauto/variantToString.hpp
//\brief Debug helper for mapping VARIANT data types to string
#include "comauto/variantToString.hpp"
#include "comauto/variantInputFilter.hpp"
#include <iostream>
#include <sstream>

using namespace _Wolframe;
using namespace _Wolframe::comauto;

std::string comauto::variantToString( const comauto::TypeLib* typelib_, const ITypeInfo* typeinfo_, const VARIANT& data_)
{
	std::ostringstream out;
	langbind::FilterBase::ElementType type;
	types::VariantConst element;
	std::vector<std::string> stk;

	VariantInputFilter itr( typelib_, typeinfo_, data_, serialize::Flags::None);
	while (itr.getNext( type, element))
	{
		switch (type)
		{
			case langbind::FilterBase::OpenTag:
				stk.push_back( element.tostring());
				out << "<" << stk.back() << ">";
				break;
			case langbind::FilterBase::CloseTag:
				if (stk.empty()) break;
				out << "</" << stk.back() << ">";
				stk.pop_back();
				break;
			case langbind::FilterBase::Attribute:
				stk.push_back( element.tostring());
				out << "<" << stk.back() << ">";
				if (!itr.getNext( type, element) || type != langbind::FilterBase::Value)
				{
					throw std::runtime_error( "error in variant filter: attribute value expected");
				}
				out << element.tostring() << "</" << stk.back() << ">";
				stk.pop_back();
				break;
			case langbind::FilterBase::Value:
				out << element.tostring();
			break;
		}
	}
	return out.str();
}




