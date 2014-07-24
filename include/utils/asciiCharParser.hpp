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
/// \file utils/asciiCharParser.hpp
/// \brief Ascii character by characer parser in a unicode source feeded chunkwise.
///	Used for document format guessing and doctype extraction

#ifndef _WOLFRAME_UTILS_ASCII_CHAR_PARSER_HPP_INCLUDED
#define _WOLFRAME_UTILS_ASCII_CHAR_PARSER_HPP_INCLUDED
#include <cstddef>

namespace _Wolframe {
namespace utils {

/// \class AsciiCharParser
/// \brief Class for parsing ascii character by character from a source in an unicode based encoding
class AsciiCharParser
{
public:
	enum Encoding
	{
		NONE,UCS1,UCS2BE,UCS2LE,UCS4BE,UCS4LE
	};
	/// \brief Constructor
	AsciiCharParser()
		:m_src(0),m_itr(0),m_end(0),m_lastError(0),m_encoding(NONE),m_bufsize(0){}

	/// \brief Feed next input block
	/// \remark The previous block has been consumed completely
	void putInput( const char* src, std::size_t srcsize);

	/// \brief Get the next ASCII character or 0, if at the end of the current input block or an error occurred (to check with lastError()const)
	unsigned char getNext();

	/// \brief Get the last error occurred
	const char* lastError() const		{return m_lastError;}

private:
	bool prepareChar( unsigned int chrsize);
	void consumeChar( unsigned int chrsize);

private:
	const char* m_src;
	std::size_t m_itr;
	std::size_t m_end;
	const char* m_lastError;
	Encoding m_encoding;
	enum {BufSize=8};
	unsigned char m_buf[ BufSize];
	std::size_t m_bufsize;
};

}}
#endif

