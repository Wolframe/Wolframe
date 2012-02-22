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
///\file textwolf_filterBase.hpp
///\brief Base functions and classes for filters based on textwolf

#ifndef _Wolframe_TEXTWOLF_FILTER_BASE_HPP_INCLUDED
#define _Wolframe_TEXTWOLF_FILTER_BASE_HPP_INCLUDED
#include "textwolf.hpp"
#include "protocol/inputfilter.hpp"
#include "protocol/outputfilter.hpp"
#include <cstddef>

namespace _Wolframe {
namespace filter {

///\class StrIterator
///\brief input iterator on a constant string returning null characters after EOF 
class StrIterator
{
public:
	///\brief Constructor
	///\param [in] src string to iterate on
	///\param [in] size number of char in the string to iterate on
	StrIterator( const char* src, unsigned int size)
		:m_src(src)
		,m_size(size)
		,m_pos(0){}

	///\brief Element access
	///\return current character  
	char operator* ()
	{
		return (m_pos < m_size)?m_src[m_pos]:0;
	}

	///\brief Preincrement
	StrIterator& operator++()
	{
		m_pos++;
		return *this;
	}

	///\brief Return current char position
	unsigned int pos() const {return m_pos;}

private:
	const char* m_src;
	unsigned int m_size;
	unsigned int m_pos;
};


///\class SrcIterator
///\brief Input iterator as source for the XML scanner (throws EndOfMessageException on EoM)
class SrcIterator
{
public:
	///\class EoM
	///\brief End of message exception
	struct EoM{};

	///\brief Empty constructor
	SrcIterator()
		:m_gen(0) {}

	///\brief Constructor
	SrcIterator( protocol::InputFilter* gen)
		:m_gen(gen) {}

	///\brief Copy constructor
	///\param [in] o iterator to copy
	SrcIterator( const SrcIterator& o)
		:m_gen(o.m_gen) {}

	///\brief access operator (required by textwolf for an input iterator)
	char operator*()
	{
		if (!m_gen->size())
		{
			if (m_gen->gotEoD()) return 0;
			throw EoM();
		}

		return *(char*)m_gen->ptr();
	}

	///\brief prefix increment operator (required by textwolf for an input iterator)
	SrcIterator& operator++()
	{
		m_gen->skip(1);
		return *this;
	}

	void setInput( protocol::InputFilter* ig)
	{
		m_gen = ig;
	}
private:
	protocol::InputFilter* m_gen;		///< input for the iterator (from network message)
};

///\class FilterBase
///\brief Filter base template
///\tparam IOCharset Character set encoding of input and output
///\tparam AppCharset Character set encoding of the application processor
///\tparam BufferType STL back insertion sequence to use for printing output
template <class IOCharset, class AppCharset>
struct FilterBase
{
	///\brief Prints a character string to an STL back insertion sequence buffer in the IO character set encoding
	///\param [in] src pointer to string to print
	///\param [in] srcsize size of src in bytes
	///\param [in,out] buf buffer to print to
	template <class BufferType>
	static void printToBuffer( const char* src, std::size_t srcsize, BufferType& buf)
	{
		StrIterator itr( src, srcsize);
		textwolf::TextScanner<StrIterator,AppCharset> ts( itr);

		textwolf::UChar ch;
		while ((ch = ts.chr()) != 0)
		{
			IOCharset::print( ch, buf);
			++ts;
		}
	}

	///\brief Prints an end of line marker (EOL) to an STL back insertion sequence buffer in the IO character set encoding
	///\param [in] ch character to print
	///\param [in,out] buf buffer to print to
	template <class BufferType>
	static void printToBufferEOL( BufferType& buf)
	{
		static const char* str =  protocol::EndOfLineMarker::value();
		static unsigned int len = protocol::EndOfLineMarker::size();
		printToBuffer( str, len, buf);
	}

	///\brief Prints a character to an STL back insertion sequence buffer in the IO character set encoding
	///\param [in] ch character to print
	///\param [in,out] buf buffer to print to
	template <class BufferType>
	static void printToBuffer( char ch, BufferType& buf)
	{
		IOCharset::print( (textwolf::UChar)(unsigned char)ch, buf);
	}
};

struct TextwolfEncoding
{
	enum Id
	{
		Unknown, IsoLatin, UTF8, UTF16, UTF16BE, UTF16LE, UCS2BE, UCS2LE, UCS4BE, UCS4LE
	};

	static const char* getName( Id e);
	static Id getId( const char* name);
};

}}//namespace
#endif


