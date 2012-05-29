/************************************************************************
Copyright (C) 2011 Project Wolframe.
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
///\file serialize/mapContext.cpp
///\brief Defines the error handling of serialization/deserialization functions and some other common stuff

#include "serialize/mapContext.hpp"
#include <cstring>
#include <stdexcept>

using namespace _Wolframe;
using namespace serialize;

Context::Context()
	:m_followTagConsumed(false)
{
	m_lasterror[0] = 0;
}

void Context::clear()
{
	m_content.clear();
	m_lasterror[0] = 0;
	m_followTagConsumed = false;
}

void Context::setError( const char* msg, const char* msgparam)
{
	setMsg( " ", ' ', msg, msgparam);
}

void Context::setTag( const char* tt)
{
	if (!tt) return;
	setMsg( tt, '/', m_lasterror);
}

void Context::setMsg( const char* m1, char dd, const char* m2, const char* m3)
{
	try
	{
		std::string msg;
		if (m1) msg.append( m1);
		if (dd) msg.push_back( dd);
		if (m2) msg.append( m2);
		if (m3)
		{
			msg.append( " (");
			msg.append( m3);
			msg.push_back(')');
		}
		std::size_t nn = msg.size();
		if (nn >=  sizeof(m_lasterror)) nn = sizeof(m_lasterror)-1;
		std::memcpy( m_lasterror, msg.c_str(), nn);
		m_lasterror[ nn] = 0;
	}
	catch (const std::bad_alloc& e)
	{
		std::size_t nn = std::strlen( e.what());
		if (nn >=  sizeof(m_lasterror)) nn = sizeof(m_lasterror)-1;
		std::memcpy( m_lasterror, e.what(), nn);
		m_lasterror[ nn] = 0;
	}
}

bool Context::printElem( langbind::OutputFilter::ElementType tp, const void* elem, std::size_t elemsize, langbind::OutputFilter& out)
{
	char buf[ 2048];

	if (out.getPosition()) throw std::runtime_error( "unconsumed output in filter serialization");
	out.setOutputBuffer( (void*)buf, sizeof(buf));

	while (!out.print( tp, elem, elemsize) && out.state() == langbind::OutputFilter::EndOfBuffer)
	{
		append( buf, out.getPosition());
		out.setOutputBuffer( (void*)buf, sizeof(buf));
	}
	if (out.state() == langbind::OutputFilter::Error)
	{
		setError( out.getError());
		return false;
	}
	append( buf, out.getPosition());
	out.setOutputBuffer( (void*)buf, sizeof(buf));
	return true;
}


