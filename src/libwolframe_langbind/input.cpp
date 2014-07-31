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
///\file input.cpp
///\brief Implementation of input for scripting language binding
#include "langbind/input.hpp"
#include <stdexcept>

using namespace _Wolframe;
using namespace langbind;

Input::Input( const Input& o)
	:m_used(o.m_used)
	,m_inputfilter(o.m_inputfilter)
	,m_docformat(o.m_docformat)
	,m_content(o.m_content)
	,m_contentsize(o.m_contentsize)
	,m_unconsumedInput(o.m_unconsumedInput)
	,m_gotEoD(o.m_gotEoD)
{}

Input::Input( const std::string& docformat_)
	:m_used(false)
	,m_docformat(docformat_)
	,m_contentsize(0)
	,m_gotEoD(false)
{}

Input::Input( const std::string& docformat_, const std::string& content_)
	:m_used(false)
	,m_docformat(docformat_)
	,m_contentsize(0)
	,m_gotEoD(false)
{
	char* mem = (char*)std::malloc( content_.size());
	if (!mem) throw std::bad_alloc();
	m_content = boost::shared_ptr<char>( mem, std::free);
	m_contentsize = content_.size();
	std::memcpy( mem, content_.c_str(), content_.size());
}

void Input::putInput( const void* data, std::size_t datasize, bool eod)
{
	m_gotEoD |= eod;
	if (m_inputfilter.get())
	{
		m_inputfilter->putInput( data, datasize, eod);
	}
	else
	{
		m_unconsumedInput.append( (const char*)data, datasize);
	}
}

void Input::setInputFilter( const InputFilterR& filter)
{
	if (m_inputfilter.get() && m_inputfilter->state() != InputFilter::Start)
	{
		throw std::runtime_error( "cannot reset input filter already used");
	}
	else
	{
		m_inputfilter.reset( filter->copy());
		if (m_unconsumedInput.size() || m_gotEoD)
		{
			m_inputfilter->putInput( m_unconsumedInput.c_str(), m_unconsumedInput.size(), m_gotEoD);
		}
		else if (isDocument())
		{
			m_inputfilter->putInput( documentptr(), documentsize(), true);
		}
	}
}

InputFilterR& Input::getIterator()
{
	if (m_used) throw std::runtime_error( "try to use iterator for input/document twice");
	m_used = true;
	return m_inputfilter;
}

