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
///\file redirectFilterClosure.cpp
///\brief Implementation of redirect streaming from an input filter to an output filter interface
#include "langbind/redirectFilterClosure.hpp"
#include "filter/filter.hpp"
#include <stdexcept>

using namespace _Wolframe;
using namespace langbind;

RedirectFilterClosure::RedirectFilterClosure()
	:utils::TypeSignature("langbind::RedirectFilterClosure", __LINE__)
	,m_state(0)
	,m_taglevel(0)
	,m_elemtype(InputFilter::Value){}

RedirectFilterClosure::RedirectFilterClosure( const TypedInputFilterR& i, const TypedOutputFilterR& o)
	:utils::TypeSignature("langbind::RedirectFilterClosure", __LINE__)
	,m_state(0)
	,m_taglevel(0)
	,m_inputfilter(i)
	,m_outputfilter(o)
	,m_elemtype(InputFilter::Value)
	{
		m_inputfilter->setFlags( m_outputfilter->flags());
	}

RedirectFilterClosure::RedirectFilterClosure( const RedirectFilterClosure& o)
	:utils::TypeSignature(o)
	,m_state(o.m_state)
	,m_taglevel(o.m_taglevel)
	,m_inputfilter(o.m_inputfilter)
	,m_outputfilter(o.m_outputfilter)
	,m_elemtype(o.m_elemtype)
	,m_elem(o.m_elem)
	{}

void RedirectFilterClosure::init( const TypedInputFilterR& i, const TypedOutputFilterR& o)
{
	m_inputfilter = i;
	m_outputfilter = o;
	m_state = 0;
	m_taglevel = 0;
	m_elemtype = InputFilter::Value;
	m_inputfilter->setFlags( m_outputfilter->flags());
}


bool RedirectFilterClosure::call()
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
					case InputFilter::Open:
						m_state = 2;
						return true;

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
					m_state = 2;
					return true;
				}
			}
			/*no break here!*/
		case 1:
			if (!m_outputfilter->print( m_elemtype, m_elem))
			{
				switch (m_outputfilter->state())
				{
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
		default:
			return true;
	}
}


