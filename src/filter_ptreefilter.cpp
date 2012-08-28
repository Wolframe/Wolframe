/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
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
			type = TypedFilterBase::CloseTag;
			element = Element();
			return true;
		}
		switch (m_state)
		{
			case 0:
				element = Element( m_stk.back().itr->first);
				type = TypedFilterBase::OpenTag;
				m_state = 1;
				return true;
			case 1:
				if (m_stk.back().itr->second.data().size())
				{
					element = Element( m_stk.back().itr->second.data());
					type = TypedFilterBase::Value;
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

PropertyTreeOutputFilter::PropertyTreeOutputFilter()
{
	m_stk.push_back( &m_content);
}

bool PropertyTreeOutputFilter::print( ElementType type, const Element& element)
{
	std::string elem = element.tostring();
	if (std::strchr( elem.c_str(), ' '))
	{
		throw std::runtime_error( "illegal character in property tree output filter node");
	}
	switch (type)
	{
		case TypedFilterBase::OpenTag:
		{
			m_stk.back()->add_child( elem, Tree());
			Tree* cur = m_stk.back();
			Tree::assoc_iterator nodei = cur->find( elem), nexti = nodei;
			if (nodei == cur->not_found())
			{
				std::runtime_error( "illegal state in property tree output filter");
			}
			if (!m_attribute.empty())
			{
				std::runtime_error( "unspecified attribute value in property tree output filter");
			}
			for (++nexti; nexti != cur->not_found(); nodei=nexti,++nexti);
			m_stk.push_back( &nodei->second);
		}
		break;
		case TypedFilterBase::CloseTag:
			if (!m_attribute.empty())
			{
				std::runtime_error( "unspecified attribute value in property tree output filter");
			}
			m_stk.pop_back();
			if (m_stk.empty())
			{
				std::runtime_error( "tags not balanced in property tree output filter");
			}
		break;
		case TypedFilterBase::Attribute:
			m_attribute = elem;
			if (m_attribute.empty())
			{
				std::runtime_error( "empty attribute name in property tree output filter");
			}
		break;
		case TypedFilterBase::Value:
		{
			if (!m_attribute.empty())
			{
				Tree node;
				node.put_value( elem);
				m_stk.back()->add_child( m_attribute, node);
				m_attribute.clear();
			}
			else
			{
				Tree* cur = m_stk.back();
				if (!cur->get_value<std::string>().empty())
				{
					std::runtime_error( "duplicate value for a tag in property tree output filter");
				}
				cur->put_value( elem);
			}
		}
		break;
	}
	return true;
}



