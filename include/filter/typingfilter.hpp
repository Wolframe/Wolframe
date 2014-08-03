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
/// \file filter/typingfilter.hpp
/// \brief Interface of filters that wrap untyped to typed input/output filters
#ifndef _Wolframe_TYPING_FILTER_HPP_INCLUDED
#define _Wolframe_TYPING_FILTER_HPP_INCLUDED
#include "filter/typedfilter.hpp"
#include <vector>

namespace _Wolframe {
namespace langbind {

/// \class TypingInputFilter
/// \brief Typed fascade for input filter
class TypingInputFilter :public TypedInputFilter
{
public:
	/// \brief Constructor
	explicit TypingInputFilter( const InputFilterR& inp)
		:TypedInputFilter(inp->name())
		,m_inputfilter(inp)
	{
		setFlags( inp->flags());
	}

	/// \brief Copy constructor
	/// \param[in] o input filter to copy
	TypingInputFilter( const TypingInputFilter& o)
		:TypedInputFilter(o)
		,m_inputfilter(o.m_inputfilter)
		,m_stack(o.m_stack){}

	/// \brief Destructor
	virtual ~TypingInputFilter(){}

	/// \brief Get a self copy
	/// \return allocated pointer to copy of this
	virtual TypedInputFilter* copy() const		{return new TypingInputFilter(*this);}

	/// \brief Implementation of TypedInputFilter::getNext(ElementType&,types::VariantConst&)
	virtual bool getNext( ElementType& type, types::VariantConst& element);

	/// \brief Implementation of TypedInputFilter::setFlags(Flags)
	virtual bool setFlags( Flags f);

	/// \brief Implements FilterBase::checkSetFlags()const
	virtual bool checkSetFlags( Flags f) const;

	/// \brief Get the last error
	virtual const char* getError() const;
	
private:
	/// \brief Stack element of the input filter state stack
	struct StackElement
	{
		bool isArrayElem;
		unsigned int cnt;

		StackElement( const StackElement& o)
			:isArrayElem(o.isArrayElem),cnt(o.cnt){}
		StackElement()
			:isArrayElem(false),cnt(0){}
	};
	InputFilterR m_inputfilter;
	std::vector<StackElement> m_stack;
};


/// \class TypingOutputFilter
/// \brief Typed fascade for output filter
class TypingOutputFilter :public TypedOutputFilter
{
public:
	/// \brief Constructor
	explicit TypingOutputFilter( const OutputFilterR& outp)
		:TypedOutputFilter(outp->name())
		,m_outputfilter(outp)
	{
		setFlags( outp->flags());
	}

	/// \brief Copy constructor
	/// \param[in] o typed output filter to copy
	TypingOutputFilter( const TypingOutputFilter& o)
		:TypedOutputFilter(o)
		,m_outputfilter(o.m_outputfilter){}

	/// \brief Destructor
	virtual ~TypingOutputFilter(){}

	/// \brief Get a self copy
	/// \return allocated pointer to copy of this
	virtual TypedOutputFilter* copy() const		{return new TypingOutputFilter(*this);}

	/// \brief Implementation of TypedOutputFilter::print( ElementType,const types::VariantConst&)
	virtual bool print( ElementType type, const types::VariantConst& element);

private:
	OutputFilterR m_outputfilter;
};

}}//namespace
#endif


