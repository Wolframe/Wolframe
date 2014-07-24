/*
---------------------------------------------------------------------
    The template library textwolf implements an input iterator on
    a set of XML path expressions without backward references on an
    STL conforming input iterator as source. It does no buffering
    or read ahead and is dedicated for stream processing of XML
    for a small set of XML queries.
    Stream processing in this Object refers to processing the
    document without buffering anything but the current result token
    processed with its tag hierarchy information.

    Copyright (C) 2010,2011,2012,2013,2014 Patrick Frey

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
/// \file textwolf/cstringiterator.hpp
/// \brief textwolf iterator on strings

#ifndef __TEXTWOLF_CSTRING_ITERATOR_HPP__
#define __TEXTWOLF_CSTRING_ITERATOR_HPP__
#include <string>
#include <cstring>
#include <cstdlib>

/// \namespace textwolf
/// \brief Toplevel namespace of the library
namespace textwolf {

/// \class CStringIterator
/// \brief Input iterator on a constant string returning null characters after EOF as required by textwolf scanners
class CStringIterator
{
public:
	/// \brief Default constructor
	CStringIterator()
		:m_src(0)
		,m_size(0)
		,m_pos(0){}

	/// \brief Constructor
	/// \param [in] src null terminated C string to iterate on
	/// \param [in] size number of bytes in the string to iterate on
	CStringIterator( const char* src, unsigned int size)
		:m_src(src)
		,m_size(size)
		,m_pos(0){}

	/// \brief Constructor
	/// \param [in] src string to iterate on
	CStringIterator( const char* src)
		:m_src(src)
		,m_size(std::strlen(src))
		,m_pos(0){}

	/// \brief Constructor
	/// \param [in] src string to iterate on
	CStringIterator( const std::string& src)
		:m_src(src.c_str())
		,m_size(src.size())
		,m_pos(0){}

	/// \brief Copy constructor
	/// \param [in] o iterator to copy
	CStringIterator( const CStringIterator& o)
		:m_src(o.m_src)
		,m_size(o.m_size)
		,m_pos(o.m_pos){}

	/// \brief Element access
	/// \return current character
	inline char operator* ()
	{
		return (m_pos < m_size)?m_src[m_pos]:0;
	}

	/// \brief Preincrement
	inline CStringIterator& operator++()
	{
		m_pos++;
		return *this;
	}

	/// \brief Return current char position
	inline unsigned int pos() const	{return m_pos;}

	/// \brief Set current char position
	inline void pos( unsigned int i)	{m_pos=(i<m_size)?i:m_size;}

	inline int operator - (const CStringIterator& o) const
	{
		if (m_src != o.m_src) return 0;
		return (int)(m_pos - o.m_pos);
	}

private:
	const char* m_src;
	unsigned int m_size;
	unsigned int m_pos;
};

}//namespace
#endif
