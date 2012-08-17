/************************************************************************
Copyright (C) 2011, 2012 Project Wolframe.
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
#include <cstring>
#include <cstdlib>

namespace _Wolframe {
namespace protocol {

MemBlock::MemBlock()							:m_ptr(0),m_size(0),m_pos(0),m_allocated(false) {}
MemBlock::MemBlock( std::size_t p_size)					:m_ptr(0),m_size(p_size),m_pos(0),m_allocated(false)
{
	m_ptr = new unsigned char[ m_size];
	m_allocated = true;
}

MemBlock::MemBlock( void* p_ptr, std::size_t p_size, std::size_t p_pos)	:m_ptr(p_ptr),m_size(p_size),m_pos(p_pos),m_allocated(false){}
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
	char ecb[8];
	unsigned int eci=0;

	while (bufpos<bufsize)
	{
		switch (m_eodState)
		{
			case EoD::SRC:
			{
				srcendpos = bufsize;
				while (bufpos<bufsize)
				{
					if (buf[bufpos] == '\r')
					{
						m_eodState = EoD::CR;
						srcendpos = bufpos;
						ecb[ 0] = '\r';
						eci = 1;
						++bufpos;
						break;
					}
					else if (buf[bufpos] == '\n')
					{
						m_eodState = EoD::CR_LF;
						srcendpos = bufpos;
						ecb[ 0] = '\n';
						eci = 1;
						++bufpos;
						break;
					}
					++bufpos;
				}
				break;
			}

			case EoD::CR:
				if (buf[bufpos] == '\n')
				{
					m_eodState = EoD::CR_LF;
					ecb[ eci++] = '\n';
					++bufpos;
				}
				else if (buf[bufpos] == '\r')
				{

					m_eodState = EoD::CR;
					srcendpos = bufpos;
					ecb[ 0] = '\r';
					eci = 1;
					++bufpos;
				}
				else
				{
					m_eodState = EoD::SRC;
					srcendpos = bufpos;
					eci = 0;
				}
				break;

			case EoD::CR_LF:
				if (buf[bufpos] == '.')
				{
					m_eodState = EoD::CR_LF_DOT;
					ecb[ eci++] = '.';
					std::memmove( buf+bufpos, buf+bufpos+1, bufsize-bufpos-1);
					bufsize -= 1;
					setPos( bufsize + offset);
				}
				else if (buf[bufpos] == '\n')
				{
					m_eodState = EoD::CR_LF;
					srcendpos = bufpos;
					ecb[ 0] = '\n';
					eci = 1;
					++bufpos;
				}
				else if (buf[bufpos] == '\r')
				{
					m_eodState = EoD::CR;
					srcendpos = bufpos;
					ecb[ 0] = '\r';
					eci = 1;
					++bufpos;
				}
				else
				{
					m_eodState = EoD::SRC;
					srcendpos = bufpos;
					eci = 0;
				}
				break;

			case EoD::CR_LF_DOT:
				if (buf[bufpos] == '\r')
				{
					m_eodState = EoD::CR_LF_DOT_CR;
					ecb[ eci++] = '\r';
					++bufpos;
				}
				else if (buf[bufpos] == '\n')
				{
					m_eodState = EoD::CR_LF_DOT_CR_LF;
					++bufpos;
					return srcendpos;
				}
				else
				{
					m_eodState = EoD::SRC;
					srcendpos = bufpos;
					eci = 0;
				}
				break;

			case EoD::CR_LF_DOT_CR:
				if (buf[bufpos] == '\n')
				{
					m_eodState = EoD::CR_LF_DOT_CR_LF;
					++bufpos;
					return srcendpos;
				}
				else if (buf[bufpos] == '\r')
				{
					m_eodState = EoD::CR;
					srcendpos = bufpos;
					ecb[ 0] = '\r';
					eci = 1;
					++bufpos;
				}
				else
				{
					m_eodState = EoD::SRC;
					srcendpos = bufpos;
					eci = 0;
				}
				break;

			case EoD::CR_LF_DOT_CR_LF:
				return srcendpos;
		}
	}
	m_eodcharbuf.clear();
	m_eodcharbuf.append( ecb, eci);
	m_eodState = EoD::SRC;
	setPos( srcendpos + offset);
	return -1;
}

bool OutputBlock::printNumber( unsigned int num)
{
	char nn[64];
	unsigned int ii= sizeof(nn);
	const char digits[] = "0123456789";
	char sg = '\0';

	if (num == 0) nn[--ii] = '0';
	if ((int)num < 0)
	{
		num = (unsigned int)(-(int)num);
		sg = '-';
	}
	while (num > 0)
	{
		nn[ --ii] = digits[num%10];
		num /= 10;
	}
	if (sg)
	{
		nn[ --ii] = sg;
	}
	return print( nn+ii, sizeof(nn)-ii);
}

EscapeBuffer::EscapeBuffer()
	:m_data( (char*)std::calloc( InitDataSize, sizeof(char))),m_itr(0),m_end(0),m_size(InitDataSize),m_state(SRC){}

EscapeBuffer::~EscapeBuffer()
{
	std::free( m_data);
}

void EscapeBuffer::push( char ch)
{
	if (((m_end +1) & (m_size-1)) == m_itr)
	{
		std::size_t nn = m_size*2;
		char* dd = (char*)std::calloc( nn, sizeof(char));
		if (nn <= m_size || !dd) throw std::bad_alloc();
		std::size_t ii,ee;
		for (ii=0; (ee=(m_itr+ii)&(m_size-1)) != m_end; ii++)
		{
			dd[ ii] = m_data[ ee];
		}
		m_itr = 0;
		m_end = ii;
		m_size = nn;
		std::free( m_data);
		m_data = dd;
	}
	m_data[ m_end] = ch;
	m_end = (m_end+1)&(m_size-1);
}

void EscapeBuffer::process( char* aa, std::size_t aasize, std::size_t& aapos)
{
	std::size_t ii = 0;
	for (; ii < aapos; ++ii)
	{
		if (aa[ii] == '\n')
		{
			m_state = LF;
		}
		else if (aa[ii] == '.')
		{
			if (m_state == LF)
			{
				push( '.');
			}
			m_state = SRC;
		}
		else
		{
			m_state = SRC;
		}
		if (m_itr != m_end)
		{
			push( aa[ii]);
			aa[ii] = m_data[ m_itr];
			m_itr = (m_itr+1)&(m_size-1);
		}
	}
	while (aapos < aasize && m_itr != m_end)
	{
		aa[ aapos++] = m_data[ m_itr];
		m_itr = (m_itr+1)&(m_size-1);
	}
}

}}//namespace

