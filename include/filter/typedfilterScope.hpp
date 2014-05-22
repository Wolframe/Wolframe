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
/// \file filter/typedfilterScope.hpp
/// \brief Interface to scope of typed filter

#ifndef _Wolframe_FILTER_TYPEDFILTER_SCOPE_HPP_INCLUDED
#define _Wolframe_FILTER_TYPEDFILTER_SCOPE_HPP_INCLUDED
#include "filter/typedfilter.hpp"
#include "filter/inputfilter.hpp"

namespace _Wolframe {
namespace langbind {

/// \class TypedInputFilterScope
/// \brief TypedInputFilter that stops fetching elements after the creation tag level has been left
class TypedInputFilterScope :public TypedInputFilter
{
public:
	/// \brief Constructor
	TypedInputFilterScope()
		:utils::TypeSignature("langbind::TypedInputFilterScope", __LINE__)
		,TypedInputFilter("scope")
		,m_taglevel(0){}

	explicit TypedInputFilterScope( const TypedInputFilterR& i)
		:utils::TypeSignature("langbind::TypedInputFilterScope", __LINE__)
		,TypedInputFilter(i->name())
		,m_inputfilter(i)
		,m_taglevel(1)
	{
		TypedInputFilterScope* prev = dynamic_cast<TypedInputFilterScope*>(i.get());
		if (prev) m_inputfilter = prev->inputfilter();
	}

	/// \brief Copy constructor
	/// \param[in] o typed output filter scope to copy
	TypedInputFilterScope( const TypedInputFilterScope& o)
		:utils::TypeSignature("langbind::TypedInputFilterScope", __LINE__)
		,TypedInputFilter(o)
		,m_inputfilter(o.m_inputfilter)
		,m_taglevel(o.m_taglevel){}

	/// \brief Destructor
	virtual ~TypedInputFilterScope(){}

	/// \brief Get a self copy
	/// \return allocated pointer to copy of this
	virtual TypedInputFilter* copy() const
	{
		return new TypedInputFilterScope(*this);
	}

	/// \brief Get next element
	/// \param [out] type element type parsed
	/// \param [out] element reference to element returned
	/// \return true, if success, false, if not.
	/// \remark Check the state when false is returned
	virtual bool getNext( TypedInputFilter::ElementType& type, types::VariantConst& element);

	/// \brief Set the iterator to the start (if implemented)
	virtual void resetIterator();

	const TypedInputFilterR& inputfilter()		{return m_inputfilter;}

private:
	TypedInputFilterR m_inputfilter;
	int m_taglevel;
};

}}//namespace
#endif


