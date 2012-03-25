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

using namespace _Wolframe;
using namespace serialize;

Context::Context()
	:m_buf(new char[ bufsize+errbufsize+256])
	,m_endTagConsumed(false)
{
	m_buf[0] = 0;
	m_lasterror = m_buf+bufsize;
}

Context::~Context()
{
	delete [] m_buf;
}

void Context::setError( const char* tt, const char* msg, const char* msgparam)
{
	setMsg( tt, ':', msg, msgparam);
}

void Context::setError( const char* tt)
{
	if (!tt) return;
	setMsg( tt, '/', m_lasterror);
}

void Context::setMsg( const char* m1, char dd, const char* m2, const char* m3)
{
	std::size_t m1len = m1?std::strlen(m1):0;
	std::size_t m2len = m2?std::strlen(m2):0;
	std::size_t m3len = m3?std::strlen(m3):0;
	if (m1len >= errbufsize-1) m1len = errbufsize-2;
	if (m2len >= errbufsize-m1len) m2len = errbufsize-m1len-1;
	if (m3len >= errbufsize-m1len-m2len) m3len = errbufsize-m1len-m2len;
	std::memmove( m_lasterror+m1len+m2len+1, m3?m3:"", m3len);
	std::memmove( m_lasterror+m1len+1, m2?m2:"", m2len);
	std::memmove( m_lasterror, m1?m1:"", m1len);
	m_lasterror[ m1len] = dd;
	m_lasterror[ m1len+m2len+m3len+1] = '\0';
}

bool Context::printElem( protocol::OutputFilter::ElementType tp, const void* elem, std::size_t elemsize, protocol::OutputFilter*& out)
{
	if (!out->print( tp, elem, elemsize))
	{
		append( out->charptr(), out->pos());
		out->release();
		if (!out->print( tp, elem, elemsize))
		{
			protocol::OutputFilter* ff = out->createFollow();
			if (ff)
			{
				delete out;
				out = ff;
			}
			if (!out->print( tp, elem, elemsize))
			{
				setError( out->getError());
				return false;
			}
		}
	}
	return true;
}


