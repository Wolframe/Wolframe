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
#include "protocol/ioblocks.hpp"
#include "protocol.hpp"
#include <cstring>
#include <new>

namespace _Wolframe {
namespace protocol {

#ifdef _Wolframe_LOWLEVEL_DEBUG
static const char* eodStateName( EODState e)
{
	static const char* ar[]={"SRC","LF","LF_DOT","LF_DOT_CR","LF_DOT_CR_LF"};
	return ar[e-SRC];
}
#endif

MemBlock::MemBlock()							:m_ptr(0),m_size(0),m_pos(0),m_allocated(false) {}
MemBlock::MemBlock( size_type p_size)					:m_ptr(0),m_size(p_size),m_pos(0),m_allocated(false)
{
	m_ptr = new unsigned char[ m_size];
	m_allocated = true;
}

MemBlock::MemBlock( void* p_ptr, size_type p_size)			:m_ptr(p_ptr),m_size(p_size),m_pos(0),m_allocated(false){}
MemBlock::MemBlock( const MemBlock& o)					:m_ptr(0),m_size(0),m_pos(0),m_allocated(false) {*this = o;}

MemBlock::~MemBlock()
{
	if (m_allocated) delete [] (unsigned char*)m_ptr;
}

MemBlock& MemBlock::operator=( const MemBlock& o)
{
	if (m_allocated) delete [] (unsigned char*)m_ptr;
	m_size = o.m_size;
	m_pos = o.m_pos;
	m_allocated = o.m_allocated;

	if (o.m_allocated)
	{
		m_ptr = new unsigned char[ m_size];
		std::memcpy( m_ptr, o.m_ptr, m_size);
	}
	else
	{
		m_ptr = o.m_ptr;
	}
	return *this;
}

int InputBlock::getEoDpos( size_type offset)
{
	if (pos()<=offset) return -1;

	std::size_t bufsize = pos()-offset;
	char* buf = charptr()+offset;
	std::size_t bufpos=0;

	while (bufpos<bufsize)
	{
		if (m_eodState == EoD::SRC)
		{
			char* cc = (char*)std::memchr( buf+bufpos, '\n', bufsize-bufpos);
			if (cc)
			{
				bufpos = cc - buf + 1;
				m_eodState = EoD::LF;
			}
			else
			{
				bufpos = bufsize;
			}
		}
		else if (m_eodState == EoD::LF)
		{
			if (buf[bufpos] == '.')
			{
				m_eodState = EoD::LF_DOT;
				std::memmove( buf+bufpos, buf+bufpos+1, bufsize-bufpos-1);
				bufsize -= 1;
				setPos( bufsize + offset);
			}
			else
			{
				++bufpos;
				m_eodState = EoD::SRC;
			}
		}
		else if (m_eodState == EoD::LF_DOT)
		{
			if (buf[bufpos] == '\r')
			{
				m_eodState = EoD::LF_DOT_CR;
			}
			else if (buf[bufpos] == '\n')
			{
				m_eodState = EoD::LF_DOT_CR_LF;
			}
			else
			{
				m_eodState = EoD::SRC;
			}
			++bufpos;
		}
		else if (m_eodState == EoD::LF_DOT_CR)
		{
			if (buf[bufpos] == '\n')
			{
				++bufpos;
			}
			m_eodState = EoD::LF_DOT_CR_LF;
			return bufpos;
		}
	}
	return -1;
}
}}//namespace

