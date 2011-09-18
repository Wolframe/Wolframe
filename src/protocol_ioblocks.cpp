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

MemBlock::MemBlock()							:m_ptr(0),m_size(0),m_pos(0),m_allocated(false) {}
MemBlock::MemBlock( std::size_t p_size)					:m_ptr(0),m_size(p_size),m_pos(0),m_allocated(false)
{
	m_ptr = new unsigned char[ m_size];
	m_allocated = true;
}

MemBlock::MemBlock( void* p_ptr, std::size_t p_size)			:m_ptr(p_ptr),m_size(p_size),m_pos(0),m_allocated(false){}
MemBlock::MemBlock( const MemBlock& o)					:m_ptr(0),m_size(0),m_pos(0),m_allocated(false) {*this = o;}

MemBlock::~MemBlock()
{
	if (m_allocated) delete [] (unsigned char*)m_ptr;
}

void MemBlock::set( void* p_ptr, std::size_t p_size)
{
	if (m_allocated) delete [] (unsigned char*)m_ptr;
	m_size = p_size;
	m_pos = 0;
	m_ptr = p_ptr;
	m_allocated = false;
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
		std::memcpy( m_ptr, o.m_ptr, m_pos);
	}
	else
	{
		m_ptr = o.m_ptr;
	}
	return *this;
}

int InputBlock::getEoDpos( std::size_t offset)
{
	if (pos()<=offset) return -1;

	std::size_t bufsize = pos()-offset;
	char* buf = charptr()+offset;
	std::size_t bufpos = 0;
	std::size_t srcendpos = bufsize;

	while (bufpos<bufsize)
	{
		switch (m_eodState)
		{
			case EoD::SRC:
			{
				char* cc = buf+bufpos;
				std::size_t ii=0, nn=bufsize-bufpos;

				for (;ii<nn; ii++)
				{
					if (cc[ii] == '\r')
					{
						srcendpos = bufpos + ii;
						bufpos = srcendpos + 1;
						m_eodState = EoD::CR;
						break;
					}
					else if (cc[ii] == '\n')
					{
						srcendpos = bufpos + ii;
						bufpos = srcendpos + 1;
						m_eodState = EoD::CR_LF;
						break;
					}
				}
				if (ii == nn)
				{
					bufpos = bufsize;
				}
				break;
			}

			case EoD::CR:
				if (buf[bufpos] == '\n')
				{
					m_eodState = EoD::CR_LF;
					++bufpos;
				}
				else
				{
					m_eodState = EoD::SRC;
				}
				break;

			case EoD::CR_LF:
				if (buf[bufpos] == '.')
				{
					m_eodState = EoD::CR_LF_DOT;
					std::memmove( buf+bufpos, buf+bufpos+1, bufsize-bufpos-1);
					bufsize -= 1;
					setPos( bufsize + offset);
				}
				else
				{
					m_eodState = EoD::SRC;
				}
				break;

			case EoD::CR_LF_DOT:
				if (buf[bufpos] == '\r')
				{
					m_eodState = EoD::CR_LF_DOT_CR;
					++bufpos;
				}
				else if (buf[bufpos] == '\n')
				{
					m_eodState = EoD::CR_LF_DOT_CR_LF;
					++bufpos;
				}
				else
				{
					m_eodState = EoD::SRC;
				}
				break;

			case EoD::CR_LF_DOT_CR:
				if (buf[bufpos] == '\n')
				{
					++bufpos;
				}
				m_eodState = EoD::CR_LF_DOT_CR_LF;
				return srcendpos;

			case EoD::CR_LF_DOT_CR_LF:
				return srcendpos;
		}
	}
	m_eodcharbuf.clear();
	if (m_eodState != EoD::SRC)
	{
		if (m_eodState == EoD::CR_LF_DOT)
		{
			std::size_t bb = srcendpos;
			for (; bb<bufsize; bb++)
			{
				m_eodcharbuf.push_back( buf[ bb]);
			}
			m_eodcharbuf.push_back( '.');
		}
		else if (m_eodState == EoD::CR_LF_DOT_CR)
		{
			std::size_t bb = srcendpos;
			for (; (bb+1)<bufsize; bb++)
			{
				m_eodcharbuf.push_back( buf[ bb]);
			}
			m_eodcharbuf.push_back( '.');
			m_eodcharbuf.push_back( '\r');
		}
		else
		{
			std::size_t bb = srcendpos;
			for (; bb<bufsize; bb++)
			{
				m_eodcharbuf.push_back( buf[ bb]);
			}
		}
		m_eodState = EoD::SRC;
		setPos( srcendpos + offset);
	}
	return -1;
}
}}//namespace

