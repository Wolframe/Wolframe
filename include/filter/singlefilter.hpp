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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file filter/singlefilter.hpp
///\brief Interface of input filter that returns one content element only
#ifndef _Wolframe_SINGLE_FILTER_HPP_INCLUDED
#define _Wolframe_SINGLE_FILTER_HPP_INCLUDED
#include "filter/typedfilter.hpp"

namespace _Wolframe {
namespace langbind {

///\class SingleElementInputFilter
///\brief Input filter returning one content element only
class SingleElementInputFilter :public TypedInputFilter
{
public:
	///\brief Constructor
	explicit SingleElementInputFilter( const types::VariantConst& e)
		:types::TypeSignature("langbind::SingleElementInputFilter", __LINE__)
		,m_element(e)
		,m_consumed(false){}

	///\brief Copy constructor
	///\param[in] o input filter to copy
	SingleElementInputFilter( const SingleElementInputFilter& o)
		:types::TypeSignature(o)
		,TypedInputFilter(o)
		,m_element(o.m_element)
		,m_consumed(o.m_consumed){}

	///\brief Destructor
	virtual ~SingleElementInputFilter(){}

	///\brief Get a self copy
	///\return allocated pointer to copy of this
	virtual SingleElementInputFilter* copy() const
	{
		return new SingleElementInputFilter(*this);
	}

	///\brief Implementation of TypedInputFilter::getNext(ElementType&,Element&)
	virtual bool getNext( ElementType& type, types::VariantConst& element)
	{
		if (m_consumed)
		{
			return false;
		}
		else
		{
			type = TypedInputFilter::Value;
			element = m_element;
			return m_consumed=true;
		}
	}

private:
	types::VariantConst m_element;
	bool m_consumed;
};

}}//namespace
#endif


