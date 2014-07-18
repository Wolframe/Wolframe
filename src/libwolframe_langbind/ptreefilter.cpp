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
///\file filter_ptreefilter.cpp
///\brief Implements a filter for serialization/deserialization of a property tree
#include "filter/ptreefilter.hpp"
#include <stdexcept>
#include <sstream>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

bool PropertyTreeInputFilter::checkSetFlags( Flags f) const
{
	return (0==((int)f & (int)langbind::FilterBase::SerializeWithIndices));
}

bool PropertyTreeInputFilter::setFlags( Flags f)
{
	langbind::TypedInputFilter::setFlags( f);
	if (flag( langbind::TypedInputFilter::SerializeWithIndices))
	{
		return false;
	}
	return true;
}

bool PropertyTreeInputFilter::getNext( ElementType& type, types::VariantConst& element)
{
	while (m_stk.size())
	{
		State st;
		if (m_stk.back().itr == m_stk.back().end)
		{
			m_stk.pop_back();
			m_state = 0;
			type = FilterBase::CloseTag;
			element.init();
			m_position = types::PropertyTree::Position();
			return true;
		}
		switch (m_state)
		{
			case 0:
				if (m_stk.back().itr != m_stk.back().end)
				{
					m_position = m_stk.back().itr->second.position();
				}
				element.init( m_stk.back().itr->first);
				type = FilterBase::OpenTag;
				m_state = 1;
				return true;
			case 1:
				if (m_stk.back().itr->second.data().size())
				{
					m_position = m_stk.back().itr->second.position();
					element.init( m_stk.back().itr->second.data());
					type = FilterBase::Value;
					m_state = 2;
					return true;
				}
			case 2:
				st.itr = m_stk.back().itr->second.begin();
				st.end = m_stk.back().itr->second.end();
				if (st.itr != st.end)
				{
					m_position = m_stk.back().itr->second.position();
				}
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

PropertyTreeOutputFilter::PropertyTreeOutputFilter()
	:TypedOutputFilter("ptree")
{
	m_stk.push_back( State());
}

bool PropertyTreeOutputFilter::print( ElementType type, const types::VariantConst& element)
{
	std::string elem = element.tostring();
	switch (type)
	{
		case FilterBase::OpenTag:
		{
			m_stk.push_back( State( elem));
			if (!m_attribute.empty())
			{
				std::runtime_error( "unspecified attribute value in property tree output filter");
			}
		}
		break;
		case FilterBase::CloseTag:
			if (!m_attribute.empty())
			{
				std::runtime_error( "unspecified attribute value in property tree output filter");
			}
			if (m_stk.size() <= 1)
			{
				std::runtime_error( "tags not balanced in property tree output filter");
			}
			if (!m_stk.back().m_node.empty() || !m_stk.back().m_node.data().empty())
			{
				types::PropertyTree::Node* parent = &m_stk[ m_stk.size()-2].m_node;
				parent->add_child( m_stk.back().m_name, m_stk.back().m_node);
			}
			m_stk.pop_back();
		break;
		case FilterBase::Attribute:
			m_attribute = elem;
			if (m_attribute.empty())
			{
				std::runtime_error( "empty attribute name in property tree output filter");
			}
		break;
		case FilterBase::Value:
		{
			if (!m_attribute.empty())
			{
				types::PropertyTree::Node node;
				node.setValue( elem);
				m_stk.back().m_node.add_child( m_attribute, node);
				m_attribute.clear();
			}
			else
			{
				types::PropertyTree::Node* cur = &m_stk.back().m_node;
				if (!cur->data().empty())
				{
					std::runtime_error( "duplicate value for a tag in property tree output filter");
				}
				cur->setValue( elem);
			}
		}
		break;
	}
	return true;
}



