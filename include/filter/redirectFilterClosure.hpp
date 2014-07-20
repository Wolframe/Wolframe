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
/// \file filter/redirectFilterClosure.hpp
/// \brief Interface to redirect streaming from an input filter to an output filter interface
#ifndef _Wolframe_filter_REDIRECT_FILTER_CLOSURE_HPP_INCLUDED
#define _Wolframe_filter_REDIRECT_FILTER_CLOSURE_HPP_INCLUDED
#include "filter/typedfilter.hpp"
#include "types/variant.hpp"
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace langbind {

/// \class RedirectFilterClosure
/// \brief Closure of a map from an input filter to an output filter
class RedirectFilterClosure
{
public:
	/// \brief Default constructor
	explicit RedirectFilterClosure( bool doPrintFinalClose)
		:m_state(0)
		,m_taglevel(0)
		,m_do_print_final_close(doPrintFinalClose)
		,m_elemtype(InputFilter::Value)
		{}

	/// \brief Constructor
	RedirectFilterClosure( const TypedInputFilterR& i, const TypedOutputFilterR& o, bool doPrintFinalClose)
		:m_state(0)
		,m_taglevel(0)
		,m_do_print_final_close(doPrintFinalClose)
		,m_inputfilter(i)
		,m_outputfilter(o)
		,m_elemtype(InputFilter::Value)
		{
			m_inputfilter->setFlags( m_outputfilter->flags());
		}

	/// \brief Copy constructor
	RedirectFilterClosure( const RedirectFilterClosure& o)
		:m_state(o.m_state)
		,m_taglevel(o.m_taglevel)
		,m_do_print_final_close(o.m_do_print_final_close)
		,m_inputfilter(o.m_inputfilter)
		,m_outputfilter(o.m_outputfilter)
		,m_elemtype(o.m_elemtype)
		,m_elem(o.m_elem)
		{}

	/// \brief Destructor
	~RedirectFilterClosure(){}

	/// \brief Calls the fetching of input and printing it to output until end or interruption
	/// \return true when completed
	bool call()
	{
		if (!m_inputfilter.get()) throw std::runtime_error( "no input defined for redirecting filter");
		if (!m_outputfilter.get()) throw std::runtime_error( "no output defined for redirecting filter");
	
		for (;;) switch (m_state)
		{
			case 0:
				if (!m_inputfilter->getNext( m_elemtype, m_elem))
				{
					switch (m_inputfilter->state())
					{
						case InputFilter::Start:
						case InputFilter::Open:
							m_state = (m_do_print_final_close)?2:3;
							continue;
	
						case InputFilter::EndOfMessage:
							return false;
	
						case InputFilter::Error:
							throw std::runtime_error( m_inputfilter->getError());
					}
				}
				m_state = 1;
				if (m_elemtype == InputFilter::OpenTag)
				{
					++m_taglevel;
				}
				else if (m_elemtype == InputFilter::CloseTag)
				{
					--m_taglevel;
					if (m_taglevel < 0)
					{
						m_state = (m_do_print_final_close)?2:3;
						continue;
					}
				}
				/*no break here!*/
			case 1:
				if (!m_outputfilter->print( m_elemtype, m_elem))
				{
					switch (m_outputfilter->state())
					{
						case OutputFilter::Start:
						case OutputFilter::Open:
							throw std::runtime_error( "unknown error in output filter");
	
						case OutputFilter::EndOfBuffer:
							return false;
	
						case OutputFilter::Error:
							throw std::runtime_error( m_outputfilter->getError());
					}
				}
				m_state = 0;
				continue;
			case 2:
				if (!m_outputfilter->print( m_elemtype, m_elem))
				{
					switch (m_outputfilter->state())
					{
						case OutputFilter::Start:
						case OutputFilter::Open:
							throw std::runtime_error( "unknown error in output filter");
	
						case OutputFilter::EndOfBuffer:
							return false;
	
						case OutputFilter::Error:
							throw std::runtime_error( m_outputfilter->getError());
					}
				}
				m_state = 3;
				/*no break here!*/
			default:
				return true;
		}
	}

	/// \brief Initialization of call context for a new call
	/// \param[in] i call input
	/// \param[in] o call output
	void init( const TypedInputFilterR& i, const TypedOutputFilterR& o)
	{
		m_inputfilter = i;
		m_outputfilter = o;
		m_state = 0;
		m_taglevel = 0;
		m_elemtype = InputFilter::Value;
		m_inputfilter->setFlags( m_outputfilter->flags());
	}

	/// \brief Get the input filter reference
	const TypedInputFilterR& inputfilter() const		{return m_inputfilter;}
	/// \brief Get the output filter reference
	const TypedOutputFilterR& outputfilter() const		{return m_outputfilter;}

private:
	int m_state;				///< current state of call
	int m_taglevel;				///< current balance of open/close tags
	bool m_do_print_final_close;		///< true, if the final close is printed, false if it is only consumed
	TypedInputFilterR m_inputfilter;	///< input filter
	TypedOutputFilterR m_outputfilter;	///< output filter
	InputFilter::ElementType m_elemtype;	///< type of last element read from command result
	types::VariantConst m_elem;		///< last element read from command result
};

}}//namespace
#endif


