/*
---------------------------------------------------------------------
    The template library textwolf implements an input iterator on
    a set of XML path expressions without backward references on an
    STL conforming input iterator as source. It does no buffering
    or read ahead and is dedicated for stream processing of XML
    for a small set of XML queries.
    Stream processing in this context refers to processing the
    document without buffering anything but the current result token
    processed with its tag hierarchy information.

    Copyright (C) 2010,2011,2012 Patrick Frey

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 3.0 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

--------------------------------------------------------------------

	The latest version of textwolf can be found at 'http://github.com/patrickfrey/textwolf'
	For documentation see 'http://patrickfrey.github.com/textwolf'

--------------------------------------------------------------------
*/
#ifndef __TEXTWOLF_STATIC_BUFFER_HPP__
#define __TEXTWOLF_STATIC_BUFFER_HPP__
#include "textwolf/exception.hpp"
#include <cstddef>
#include <cstring>
#include <cstdlib>
#include <stdexcept>

namespace textwolf {

///\class StaticBuffer
///\brief Simple back insertion sequence for storing the outputs of textwolf in a contant size buffer
class StaticBuffer :public throws_exception
{
public:
	///\brief Constructor
	explicit StaticBuffer( std::size_t n)
		:m_pos(0),m_size(n),m_ar(0),m_allocated(true)
	{
		m_ar = (char*)std::calloc( n, sizeof(char));
		if (!m_ar) throw std::bad_alloc();
	}

	///\brief Constructor
	StaticBuffer( char* p, std::size_t n, std::size_t i=0)
		:m_pos(i)
		,m_size(n)
		,m_ar(p)
		,m_allocated(false)
		,m_overflow(false) {}

	///\brief Copy constructor
	StaticBuffer( const StaticBuffer& o)
		:m_pos(o.m_pos)
		,m_size(o.m_size)
		,m_ar(0)
		,m_allocated(o.m_allocated)
		,m_overflow(o.m_overflow)
	{
		m_ar = (char*)std::malloc( m_size * sizeof(char));
		if (!m_ar) throw std::bad_alloc();
		std::memcpy( m_ar, o.m_ar, m_size);
	}

	///\brief Destructor
	~StaticBuffer()
	{
		if (m_allocated && m_ar) std::free(m_ar);
	}

	///\brief Clear the buffer content
	void clear()
	{
		m_pos = 0;
		m_overflow = false;
	}

	///\brief Append one character
	///\param[in] ch the character to append
	void push_back( char ch)
	{
		if (m_pos < m_size)
		{
			m_ar[m_pos++] = ch;
		}
		else
		{
			m_overflow = true;
		}
	}

	///\brief Append an array of characters
	///\param[in] cc the characters to append
	///\param[in] ccsize the number of characters to append
	void append( const char* cc, std::size_t ccsize)
	{
		if (m_pos+ccsize > m_size)
		{
			m_overflow = true;
			ccsize = m_size - m_pos;
		}
		std::memcpy( m_ar+m_pos, cc, ccsize);
		m_pos += ccsize;
	}

	///\brief Return the number of characters in the buffer
	///\return the number of characters (bytes)
	std::size_t size() const		{return m_pos;}

	///\brief Return the buffer content as 0-terminated string
	///\return the C-string
	const char* ptr() const			{return m_ar;}

	///\brief Shrinks the size of the buffer or expands it with c
	///\param [in] n new size of the buffer
	///\param [in] c fill character if n bigger than the current fill size
	void resize( std::size_t n, char c=0)
	{
		if (m_pos>n)
		{
			m_pos=n;
		}
		else
		{
			if (m_size<n) n=m_size;
			while (n>m_pos) push_back(c);
		}
	}

	///\brief random access of element
	///\param [in] ii
	///\return the character at this position
	char operator []( std::size_t ii) const
	{
		if (ii > m_pos) throw exception( DimOutOfRange);
		return m_ar[ii];
	}

	///\brief random access of element reference
	///\param [in] ii
	///\return the reference to the character at this position
	char& at( std::size_t ii) const
	{
		if (ii > m_pos) throw exception( DimOutOfRange);
		return m_ar[ii];
	}

	///\brief check for array bounds write
	///\return true if a push_back would have caused an array bounds write
	bool overflow() const			{return m_overflow;}
private:
	StaticBuffer(){}
private:
	std::size_t m_pos;			//< current cursor position of the buffer (number of added characters)
	std::size_t m_size;			//< allocation size of the buffer in bytes
	char* m_ar;				//< buffer content
	bool m_allocated;			//< true, if the buffer is allocated by this class and not passed by constructor
	bool m_overflow;			//< true, if an array bounds write would have happened with push_back
};

}//namespace
#endif
