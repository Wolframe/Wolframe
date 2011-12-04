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
///\file serialize/luamapError.hpp
///\brief Defines the error handling of the intrusive lua serialization/deserialization

#ifndef _Wolframe_LUAMAP_ERROR_HPP_INCLUDED
#define _Wolframe_LUAMAP_ERROR_HPP_INCLUDED
#include <cstring>

namespace _Wolframe {
namespace serialize {

struct Context
{
	enum {bufsize=4096,errbufsize=256};

	Context()
		:m_buf(new char[ bufsize+errbufsize])
	{
		m_lasterror = m_buf+bufsize;
	}

	~Context()
	{
		delete [] m_buf;
	}

	const char* getLastError() const	{return m_lasterror;}
	char* buf() const			{return m_buf;}
	const std::string content() const {return m_content;}
	void endTagConsumed( bool v)	{m_endtagConsumed=v;}
	bool endTagConsumed()		{return m_endtagConsumed;}
	char* buf() const		{return m_buf;}

	void setError( const char* tt, const char* msg)
	{
		setMsg( tt, ':', msg);
	}

	void setError( const char* tt)
	{
		if (!tt) return;
		setMsg( tt, '/', m_lasterror);
	}

private:
	friend class DescriptionBase;
	char* m_lasterror;
	char* m_buf;
	std::string m_content;
	bool m_endtagConsumed;

	void setMsg( const char* m1, char dd, const char* m2)
	{
		std::size_t m1len = m1?std::strlen(m1):0;
		std::size_t m2len = m2?std::strlen(m2):0;
		if (m1len >= errbufsize) m1len = errbufsize-1;
		if (m2len >= errbufsize-m1len) m2len = errbufsize-m1len-1;
		std::memmove( m_lasterror, m1?m1:"", m1len);
		m_lasterror[ m1len] = dd;
		std::memmove( m_lasterror+m1len+1, m2?m2:"", m2len);
		m_lasterror[ m1len+m2len+1] = '\0';
	}
};

}}
#endif

