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
/// \file filter/envelopefilter.hpp
/// \brief Filter serving as envelope for a typed filter with some addtionional context data. Used for functions that return a result of a function called as their own result. They need to add their closure so that it not get freed while the result is still processed by the caller.
/// \remark Most scenarios where a function returns a result of an other function as its own needs an envelope of this kind to avoid data reference deadlocks.

#ifndef _Wolframe_FILTER_ENVELOPE_FILTER_INTERFACE_HPP_INCLUDED
#define _Wolframe_FILTER_ENVELOPE_FILTER_INTERFACE_HPP_INCLUDED
#include "filter/typedfilter.hpp"
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace langbind {

/// \class EnvelopeInputFilter
/// \brief Envelope for a input filter and its context
template <class Context>
class EnvelopeInputFilter
	:public TypedInputFilter
{
public:
	typedef boost::shared_ptr<Context> ContextR;

	/// \brief Constructor
	EnvelopeInputFilter( const TypedInputFilterR& i, const ContextR& c)
		:TypedInputFilter(i->name())
		,m_context(c)
		,m_input(i){}

	/// \brief Copy constructor
	/// \param[in] o typed output filter to copy
	EnvelopeInputFilter( const EnvelopeInputFilter& o)
		:TypedInputFilter(o)
		,m_context(o.m_context)
		,m_input(o.m_input){}

	/// \brief Destructor
	virtual ~EnvelopeInputFilter()
	{
		m_input.reset();
	}

	/// \brief Get a self copy
	/// \return allocated pointer to copy of this
	virtual TypedInputFilter* copy() const
	{
		return new EnvelopeInputFilter(*this);
	}

	/// \brief Get next element
	/// \param [out] type element type parsed
	/// \param [out] element reference to element returned
	/// \return true, if success, false, if not.
	/// \remark Check the state when false is returned
	/// \remark If type is OpenTag then element is either 1) a string (tag) or 2) a number convertible to an uint (array index counted from 1)
	virtual bool getNext( ElementType& type, types::VariantConst& element)
	{
		return m_input->getNext( type, element);
	}

	/// \brief Set the iterator to the start (if implemented)
	virtual void resetIterator()
	{
		m_input->resetIterator();
	}

	/// \brief Set a flag (or a set of flags)
	virtual bool setFlags( Flags f)
	{
		if (m_input->setFlags(f))
		{
			return TypedInputFilter::setFlags(f);
		}
		return false;
	}

	/// \brief Test if a flag can be set (allowed)
	virtual bool checkSetFlags( Flags f) const
	{
		return m_input->checkSetFlags(f);
	}

	/// \brief Get the last error
	virtual const char* getError() const
	{
		return m_input->getError();
	}

private:
	boost::shared_ptr<Context> m_context;
	TypedInputFilterR m_input;
};

}}//namespace
#endif
