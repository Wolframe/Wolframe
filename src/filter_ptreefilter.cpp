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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\file filter_ptreefilter.cpp
///\brief Implements a filter for serialization/deserialization of a property tree
#include "filter/ptreefilter.hpp"
#include <stdexcept>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

bool PropertyTreeInputFilter::getNext( ElementType& type, Element& element)
{
	while (m_stk.size())
	{
		State st;
		if (m_stk.back().itr == m_stk.back().end)
		{
			m_stk.pop_back();
			m_state = 0;
			type = TypedInputFilter::CloseTag;
			element = Element();
			return true;
		}
		switch (m_state)
		{
			case 0:
				element = Element( m_stk.back().itr->first);
				type = TypedInputFilter::OpenTag;
				m_state = 1;
				return true;
			case 1:
				if (m_stk.back().itr->second.data().size())
				{
					element = Element( m_stk.back().itr->second.data());
					type = TypedInputFilter::Value;
					m_state = 2;
					return true;
				}
			case 2:
				st.itr = m_stk.back().itr->second.begin();
				st.end = m_stk.back().itr->second.end();
				++m_stk.back().itr;
				m_stk.push_back( st);
				m_state = 0;
				continue;

			default:
				throw std::runtime_error( "illegal state in ptree filter");
		}
	}
	return false;
}


