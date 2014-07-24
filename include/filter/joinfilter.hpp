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
/// \file filter/joinfilter.hpp
/// \brief Interface for a filter created from the join of two filters
#ifndef _Wolframe_JOIN_FILTER_HPP_INCLUDED
#define _Wolframe_JOIN_FILTER_HPP_INCLUDED
#include "filter/typedfilter.hpp"

namespace _Wolframe {
namespace langbind {

/// \class JoinInputFilter
/// \brief Input filter as join of two input filters
class JoinInputFilter
	:public TypedInputFilter
{
public:
	/// \brief Constructor
	explicit JoinInputFilter( const char* name_, const TypedInputFilterR& f1, const TypedInputFilterR& f2)
		:TypedInputFilter(name_)
		,m_inputfilter1(f1)
		,m_inputfilter2(f2)
		,m_joinstate(Init)
		,m_taglevel(0)
	{
		TypedInputFilter::setFlags( f1->flags());
		TypedInputFilter::setFlags( f2->flags());
	}

	/// \brief Copy constructor
	/// \param[in] o input filter to copy
	JoinInputFilter( const JoinInputFilter& o)
		:TypedInputFilter(o)
		,m_inputfilter1(o.m_inputfilter1)
		,m_inputfilter2(o.m_inputfilter2)
		,m_joinstate(o.m_joinstate)
		,m_taglevel(o.m_taglevel)
	{}

	/// \brief Destructor
	virtual ~JoinInputFilter(){}

	/// \brief Get a self copy
	/// \return allocated pointer to copy of this
	virtual TypedInputFilter* copy() const		{return new JoinInputFilter(*this);}

	/// \brief Implementation of TypedInputFilter::getNext(ElementType&,types::VariantConst&)
	virtual bool getNext( ElementType& type, types::VariantConst& element);

	/// \brief Implementation of TypedInputFilter::setFlags(Flags)
	virtual bool setFlags( Flags f);

	/// \brief Implements FilterBase::checkSetFlags()const
	virtual bool checkSetFlags( Flags f) const;

private:
	bool getNextImpl( TypedInputFilter* flt, ElementType& type, types::VariantConst& element);

	TypedInputFilterR m_inputfilter1;
	TypedInputFilterR m_inputfilter2;

	enum JoinState
	{
		Init,
		ProcessFilter1,
		ProcessFilter2,
		FinalClose,
		Done
	};
	JoinState m_joinstate;
	int m_taglevel;
};

}}//namespace
#endif


