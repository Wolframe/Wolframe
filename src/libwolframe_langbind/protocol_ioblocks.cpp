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
#include "protocol/ioblocks.hpp"
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>

using namespace _Wolframe;
using namespace _Wolframe::protocol;

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

static void remove_chars_from_buf( char* buf, std::size_t bufsize, const std::vector<std::size_t>& delchrs)
{
	if (!delchrs.empty())
	{
		char* ibuf = buf;
		char* cpbuf = buf;
		std::size_t dpos = 0;
		std::vector<std::size_t>::const_iterator di = delchrs.begin(), de = delchrs.end();
		for (; di != de; ++di)
		{
			if (*di <= dpos && di != delchrs.begin())
			{
				throw std::logic_error( "illegal state in getEoDpos (params remove_chars_from_buf)");
			}
			for (std::size_t ii = 0, nn = *di - dpos; ii < nn; ++ii) *cpbuf++ = *ibuf++;
			dpos = *di + 1;
			++ibuf;
		}
		for (std::size_t ii = 0, nn = bufsize - dpos; ii < nn; ++ii) *cpbuf++ = *ibuf++;
		if ((std::size_t)(ibuf - cpbuf) != delchrs.size())
		{
			throw std::logic_error( "illegal state in getEoDpos (consistency remove_chars_from_buf)");
		}
	}
}

int InputBlock::getEoDpos( std::size_t offset)
{
	if (pos()<=offset) return -1;

	std::size_t bufsize = pos();
	char* buf = charptr();
	std::vector<std::size_t> delchrs;
	std::size_t bufpos = offset;
	std::size_t srcendpos = bufsize;
	char ecb[8];
	unsigned int eci=0;

	for (; bufpos<bufsize; ++bufpos)
	{
		switch (m_eodState)
		{
			case EoD::SRC:
				if (buf[bufpos] == '\r')
				{
					m_eodState = EoD::CR;
					srcendpos = bufpos;
					ecb[ 0] = '\r';
					eci = 1;
				}
				else if (buf[bufpos] == '\n')
				{
					m_eodState = EoD::CR_LF;
					srcendpos = bufpos;
					ecb[ 0] = '\n';
					eci = 1;
				}
				break;

			case EoD::CR:
				if (buf[bufpos] == '\n')
				{
					m_eodState = EoD::CR_LF;
					ecb[ eci++] = '\n';
				}
				else if (buf[bufpos] == '\r')
				{

					m_eodState = EoD::CR;
					srcendpos = bufpos;
					ecb[ 0] = '\r';
					eci = 1;
				}
				else
				{
					m_eodState = EoD::SRC;
					eci = 0;
				}
				break;

			case EoD::CR_LF:
				if (buf[bufpos] == '.')
				{
					m_eodState = EoD::CR_LF_DOT;
					ecb[ eci++] = '.';
					delchrs.push_back( bufpos);
				}
				else if (buf[bufpos] == '\n')
				{
					m_eodState = EoD::CR_LF;
					srcendpos = bufpos;
					ecb[ 0] = '\n';
					eci = 1;
				}
				else if (buf[bufpos] == '\r')
				{
					m_eodState = EoD::CR;
					srcendpos = bufpos;
					ecb[ 0] = '\r';
					eci = 1;
				}
				else
				{
					m_eodState = EoD::SRC;
					eci = 0;
				}
				break;

			case EoD::CR_LF_DOT:
				if (buf[bufpos] == '\r')
				{
					m_eodState = EoD::CR_LF_DOT_CR;
					ecb[ eci++] = '\r';
				}
				else if (buf[bufpos] == '\n')
				{
					m_eodState = EoD::CR_LF_DOT_CR_LF;
					remove_chars_from_buf( buf, bufsize, delchrs);
					m_nextpos = bufpos - delchrs.size() + 1;
					setPos( bufsize - delchrs.size());
					return srcendpos;
				}
				else
				{
					m_eodState = EoD::SRC;
					eci = 0;
				}
				break;

			case EoD::CR_LF_DOT_CR:
				if (buf[bufpos] == '\n')
				{
					m_eodState = EoD::CR_LF_DOT_CR_LF;
					remove_chars_from_buf( buf, bufsize, delchrs);
					m_nextpos = bufpos - delchrs.size() + 1;
					setPos( bufsize - delchrs.size());
					return srcendpos;
				}
				else if (buf[bufpos] == '\r')
				{
					m_eodState = EoD::CR;
					srcendpos = bufpos;
					ecb[ 0] = '\r';
					eci = 1;
				}
				else
				{
					m_eodState = EoD::SRC;
					eci = 0;
				}
				break;

			case EoD::CR_LF_DOT_CR_LF:
				throw std::logic_error( "getEoDpos called after getting eoD");
		}
	}
	if (m_eodState == EoD::SRC)
	{
		srcendpos = bufpos;
	}
	else
	{
		m_eodcharbuf.clear();
		m_eodcharbuf.append( ecb, eci);
		m_eodState = EoD::SRC;
	}
	remove_chars_from_buf( buf, bufsize, delchrs);
	if (delchrs.empty())
	{
		setPos( srcendpos);
	}
	else
	{
		if (delchrs.back() >= srcendpos)
		{
			setPos( srcendpos - delchrs.size() + 1);
		}
		else
		{
			setPos( srcendpos - delchrs.size());
		}
	}
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


std::string _Wolframe::protocol::escapeStringDLF( const std::string& src)
{
	std::string rt;
	std::string::const_iterator si = src.begin(), se = src.end();
	for (; si != se; ++si)
	{
		rt.push_back( *si);
		if (*si == '\n')
		{
			++si;
			if (si != se)
			{
				rt.push_back( *si);
				if (*si == '.') rt.push_back( '.');
			}
		}
	}
	return rt;
}

std::string::const_iterator _Wolframe::protocol::unescapeStringDLF( const std::string& src, std::string& buf)
{
	std::string::const_iterator si = src.begin(), se = src.end();
	for (; si != se; ++si)
	{
		buf.push_back( *si);
		if (*si == '\n')
		{
			++si;
			if (si != se)
			{
				if (*si == '.')
				{
					std::size_t idx = buf.size();
					++si;
					if (*si == '\r')
					{
						buf.push_back( '\r');
						++si;
					}
					if (*si == '\n')
					{
						buf.append( std::string( si, se));
						return buf.begin() + idx;
					}
					else
					{
						buf.push_back( *si);
					}
				}
				else
				{
					buf.push_back( *si);
				}
			}
		}
	}
	return buf.end();
}



