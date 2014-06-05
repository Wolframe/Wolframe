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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
/// \file utils/asciiCharParser.cpp
/// \brief Utility functions for guessing encoding and parsing ascii character in different encodings 
#include "utils/asciiCharParser.hpp"
#include <cstring>

using namespace _Wolframe;
using namespace _Wolframe::utils;

static AsciiCharParser::Encoding guessEncoding( const unsigned char* src, std::size_t& itr, std::size_t end)
{
	if (itr + 2 > end) return AsciiCharParser::NONE;
	if (src[itr+0] == 0xFF && src[itr+1] == 0xFE)
	{
		if (itr + 4 > end) return AsciiCharParser::NONE;
		if (!src[itr+2] && !src[itr+3])
		{
			itr += 4;
			return AsciiCharParser::UCS4LE;		//... BOM for UTF-32LE
		}
		else
		{
			itr += 2;
			return AsciiCharParser::UCS2LE;		//... BOM for UTF-16LE
		}
	}
	else if (src[itr+0] == 0xEF && src[itr+1] == 0xBB)
	{
		if (itr + 3 > end) return AsciiCharParser::NONE;
		if (src[itr+2] == 0xBF)
		{
			itr += 3;
			return AsciiCharParser::UCS1;		//... BOM for UTF-8
		}
	}
	else if (src[itr+0] == 0xFE && src[itr+1] == 0xFF)
	{
		itr += 2;
		return AsciiCharParser::UCS2BE;			//... BOM for UTF-16BE
	}
	else if (src[itr+0] && !src[itr+1])
	{
		if (itr + 4 > end) return AsciiCharParser::NONE;
		if (!src[itr+2])
		{
			return AsciiCharParser::UCS4LE;		//... First character is Ascii UTF-32LE
		}
		else
		{
			return AsciiCharParser::UCS2LE;		//... First character is Ascii UCS-2LE
		}
	}
	else if (!src[itr+0] && src[itr+1])
	{
		return AsciiCharParser::UCS2BE;			//... First character is Ascii UCS-2BE
	}
	else if (!src[itr+0] && !src[itr+1])
	{
		if (itr + 4 > end) return AsciiCharParser::NONE;
		if (!src[itr+2])
		{
			return AsciiCharParser::UCS4BE;		//... First character is Ascii UTF-32BE
		}
		else if (src[itr+2] == 0xFE && src[itr+2] == 0xFF)
		{
			itr += 4;
			return AsciiCharParser::UCS4BE;		//... BOM for UTF-32BE
		}
	}
	else
	{
		return AsciiCharParser::UCS1;
	}
	return AsciiCharParser::NONE;
}

void AsciiCharParser::putInput( const char* src, std::size_t srcsize)
{
	m_src = src;
	m_itr = 0;
	m_end = srcsize;

	if (m_encoding == NONE)
	{
		while (m_bufsize < BufSize && m_itr < m_end)
		{
			m_buf[ m_bufsize++] = (unsigned char)m_src[ m_itr++];
		}
		std::size_t ii = 0;
		m_encoding = guessEncoding( m_buf, ii, m_end);
		if (m_encoding != NONE && ii > 0)
		{
			std::memmove( m_buf, m_buf + ii, m_bufsize - ii);
		}
	}
}

bool AsciiCharParser::prepareChar( unsigned int chrsize)
{
	while (m_bufsize < chrsize && m_itr < m_end)
	{
		m_buf[ m_bufsize++] = (unsigned char)m_src[ m_itr++];
	}
	return (m_bufsize >= chrsize);
}

void AsciiCharParser::consumeChar( unsigned int chrsize)
{
	if (m_bufsize > chrsize)
	{
		std::memmove( m_buf, m_buf + chrsize, m_bufsize - chrsize);
	}
	m_bufsize -= chrsize;
}

unsigned char AsciiCharParser::getNext()
{
	unsigned char rt = 0;
	switch (m_encoding)
	{
		case NONE:
			if (m_bufsize == BufSize)
			{
				m_lastError = "could not guess character set encoding";
				return 0;
			}
			else
			{
				return 0;
			}
		case UCS1:
			if (!prepareChar( 1)) return 0;
			rt = (m_buf[0] > 127)?0xFF:m_buf[0];
			consumeChar( 1);
			break;
		case UCS2BE:
			if (!prepareChar( 2)) return 0;
			rt = (m_buf[0] != 0 || m_buf[1] > 127)?0xFF:m_buf[1];
			consumeChar( 2);
			break;
		case UCS2LE:
			if (!prepareChar( 2)) return 0;
			rt = (m_buf[1] != 0 || m_buf[0] > 127)?0xFF:m_buf[0];
			consumeChar( 2);
			break;
		case UCS4BE:
			if (!prepareChar( 4)) return 0;
			rt = (m_buf[0] != 0 || m_buf[1] != 0 || m_buf[2] != 0 || m_buf[3] > 127)?0xFF:m_buf[3];
			consumeChar( 4);
			break;
		case UCS4LE:
			if (!prepareChar( 4)) return 0;
			rt = (m_buf[1] != 0 || m_buf[2] != 0 || m_buf[3] != 0 || m_buf[0] > 127)?0xFF:m_buf[0];
			consumeChar( 4);
			break;
	}
	return rt;
}

