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
///\file filter/inputfilterScope.hpp
///\brief Interface to scope of an input filter

#ifndef _Wolframe_FILTER_INPUTFILTER_SCOPE_HPP_INCLUDED
#define _Wolframe_FILTER_INPUTFILTER_SCOPE_HPP_INCLUDED
#include "filter/inputfilter.hpp"
#include "types/countedReference.hpp"
#include <stdexcept>

namespace _Wolframe {
namespace langbind {

///\class InputFilterScope
///\brief InputFilter that stops fetching elements after the creation tag level has been left
class InputFilterScope :public InputFilter
{
public:
	///\brief Constructor
	InputFilterScope()
		:utils::TypeSignature("langbind::InputFilterScope", __LINE__)
		,InputFilter("scope")
		,m_taglevel(0){}

	///\brief Constructor
	explicit InputFilterScope( const InputFilterR& i)
		:utils::TypeSignature("langbind::InputFilterScope", __LINE__)
		,InputFilter(i->name())
		,m_inputfilter(i)
		,m_taglevel(1)
	{
		InputFilterScope* prev = dynamic_cast<InputFilterScope*>(i.get());
		if (prev) m_inputfilter = prev->inputfilter();
	}

	///\brief Copy constructor
	///\param[in] o typed output filter scope to copy
	InputFilterScope( const InputFilterScope& o)
		:utils::TypeSignature("langbind::InputFilterScope", __LINE__)
		,InputFilter(o)
		,m_inputfilter(o.m_inputfilter)
		,m_taglevel(o.m_taglevel){}

	///\brief Destructor
	virtual ~InputFilterScope(){}

	///\brief Get a self copy
	///\return allocated pointer to copy of this
	virtual InputFilter* copy() const
	{
		return new InputFilterScope(*this);
	}

	//\brief Implement InputFilter::initcopy()
	virtual InputFilter* initcopy() const
	{
		return new InputFilterScope( m_inputfilter);
	}

	//\brief Implement InputFilter::putInput(const void*,std::size_t,bool)
	virtual void putInput( const void*, std::size_t, bool)
	{
		throw std::logic_error( "put input to an input filter scope");
	}

	virtual const char* getEncoding() const
	{
		return m_inputfilter->getEncoding();
	}

	///\brief Get next element
	///\param [out] type element type parsed
	///\param [out] element reference to element returned
	///\param [out] elementsize size of element returned in bytes
	///\return true, if success, false, if not.
	///\remark Check the state when false is returned
	virtual bool getNext( ElementType& type, const void*& element, std::size_t& elementsize);

	const InputFilterR& inputfilter()		{return m_inputfilter;}

private:
	InputFilterR m_inputfilter;
	int m_taglevel;
};

}}//namespace
#endif


