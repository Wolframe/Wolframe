
/************************************************************************
Copyright (C) 2011 Project Wolframe.
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
///\file src/filter_tostringfilter.cpp
///\brief Implementation of tostring methods in language bindings
#include "filter/tostringfilter.hpp"

using namespace _Wolframe;
using namespace langbind;

bool ToStringFilter::print( ElementType type, const Element& element)
{
	switch (type)
	{
		case OpenTag:
			m_content.append( element.tostring());
			m_content.append( " { ");
			m_lasttype = type;
		return true;
		case CloseTag:
			m_content.append( " } ");
			m_lasttype = type;
		return true;
		case Attribute:
			m_lasttype = type;
			m_content.append( element.tostring());
		return true;
		case Value:
			if (m_lasttype == Attribute)
			{
				m_content.append( "=(");
				m_content.append( element.tostring());
				m_content.append( "(");
				m_lasttype = OpenTag;
			}
			else
			{
				m_content.append( "(");
				m_content.append( element.tostring());
				m_content.append( ")");
				m_lasttype = type;
			}
		return true;
	}
	throw std::runtime_error( "illegal type printed");
}


