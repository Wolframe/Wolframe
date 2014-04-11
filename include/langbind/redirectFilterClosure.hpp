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
///\file langbind/redirectFilterClosure.hpp
///\brief Interface to redirect streaming from an input filter to an output filter interface
#ifndef _Wolframe_langbind_REDIRECT_FILTER_CLOSURE_HPP_INCLUDED
#define _Wolframe_langbind_REDIRECT_FILTER_CLOSURE_HPP_INCLUDED
#include "filter/typedfilter.hpp"
#include "utils/typeSignature.hpp"
#include "types/variant.hpp"
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace langbind {

///\class RedirectFilterClosure
class RedirectFilterClosure
	:public virtual utils::TypeSignature
{
public:
	RedirectFilterClosure();
	RedirectFilterClosure( const TypedInputFilterR& i, const TypedOutputFilterR& o);
	RedirectFilterClosure( const RedirectFilterClosure& o);
	~RedirectFilterClosure(){}

	///\brief Calls the fetching of input and printing it to output until end or interruption
	///\return true when completed
	bool call();

	///\brief Initialization of call context for a new call
	///\param[in] i call input
	///\param[in] o call output
	void init( const TypedInputFilterR& i, const TypedOutputFilterR& o);

	const TypedInputFilterR& inputfilter() const		{return m_inputfilter;}
	const TypedOutputFilterR& outputfilter() const		{return m_outputfilter;}

private:
	int m_state;				//< current state of call
	int m_taglevel;				//< current balance of open/close tags
	TypedInputFilterR m_inputfilter;	//< input filter
	TypedOutputFilterR m_outputfilter;	//< output filter
	InputFilter::ElementType m_elemtype;	//< type of last element read from command result
	types::VariantConst m_elem;		//< last element read from command result
};

}}//namespace
#endif


