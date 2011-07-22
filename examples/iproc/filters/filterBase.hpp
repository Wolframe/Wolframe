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
///\file filterBase.hpp
///\brief Base class for stream processing filters

#ifndef _Wolframe_FILTER_BASE_HPP_INCLUDED
#define _Wolframe_FILTER_BASE_HPP_INCLUDED
#include "textwolf.hpp"

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
	typedef std::size_t size_type;
	typedef protocol::EscapingBuffer<textwolf::StaticBuffer> BufferType;

	///\param [in] src pointer to string to print
	///\param [in] srcsize size of src in bytes
	///\param [in,out] buf buffer to print to
	static void printToBuffer( const char* src, size_type srcsize, BufferType& buf)
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

	///\brief prints a character to an STL back insertion sequence buffer in the IO character set encoding
	///\param [in] ch character to print
	///\param [in,out] buf buffer to print to
	static void printToBuffer( char ch, BufferType& buf)
	{
		IOCharset::print( (textwolf::UChar)(unsigned char)ch, buf);
	}
};

}}//namespace
#endif


