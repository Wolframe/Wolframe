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
///\file filter/ptreefilter.hpp
///\brief Filter for serialization/deserialization of a property tree
#ifndef _Wolframe_FILTER_PTREE_FILTER_HPP_INCLUDED
#define _Wolframe_FILTER_PTREE_FILTER_HPP_INCLUDED
#include "filter/typedfilter.hpp"
#include <vector>
#include <string>
#include <boost/property_tree/ptree.hpp>

namespace _Wolframe {
namespace langbind {

class PropertyTreeInputFilter :public TypedInputFilter
{
public:
	///\brief Constructor
	PropertyTreeInputFilter( const boost::property_tree::ptree& pt)
		:m_state(0)
	{
		State st;
		st.itr = pt.begin();
		st.end = pt.end();
		m_stk.push_back(st);
	}

	///\brief Copy constructor
	///\param[in] o input filter to copy
	PropertyTreeInputFilter( const PropertyTreeInputFilter& o)
		:TypedInputFilter(o)
		,m_stk(o.m_stk)
		,m_state(o.m_state){}

	///\brief Destructor
	virtual ~PropertyTreeInputFilter(){}

	///\brief Implementation of TypedInputFilter::getNext(ElementType&,Element&)
	virtual bool getNext( ElementType& type, Element& element);

private:
	struct State
	{
		boost::property_tree::ptree::const_iterator itr;
		boost::property_tree::ptree::const_iterator end;
	};

private:
	std::vector<State> m_stk;	//< stack of iterator states
	int m_state;			//< fetching state of current element
};

}}//namespace
#endif



