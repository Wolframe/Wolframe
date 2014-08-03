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
/// \file textwolf/sourceiterator.hpp
/// \brief textwolf byte source iterator template

#ifndef __TEXTWOLF_SOURCE_ITERATOR_HPP__
#define __TEXTWOLF_SOURCE_ITERATOR_HPP__
#include <cstdlib>
#include <stdexcept>
#include <setjmp.h>

/// \namespace textwolf
/// \brief Toplevel namespace of the library
namespace textwolf {

/// \class SrcIterator
/// \brief Input iterator as source for the XML scanner with the possibility of being fed chunk by chunk
class SrcIterator
{
public:
	/// \brief Empty constructor
	SrcIterator()
		:m_itr(0)
		,m_end(0)
		,m_eom(0){}

	/// \brief Copy constructor
	/// \param [in] o iterator to copy
	SrcIterator( const SrcIterator& o)
		:m_itr(o.m_itr)
		,m_end(o.m_end)
		,m_eom(o.m_eom){}

	/// \brief Constructor
	/// \param [in] buf source chunk to iterate on
	/// \param [in] size size of source chunk to iterate on in bytes
	/// \param [in] eom_ trigger to activate if end of data has been reached (no next chunk anymore)
	SrcIterator( const char* buf, std::size_t size, jmp_buf* eom_=0)
		:m_itr(const_cast<char*>(buf))
		,m_end(m_itr+size)
		,m_eom(eom_){}

	/// \brief Assingment operator
	SrcIterator& operator=( const SrcIterator& o)
	{
		m_itr = o.m_itr;
		m_end = o.m_end;
		m_eom = o.m_eom;
		return *this;
	}

	/// \brief Element access operator (required by textwolf for an input iterator)
	inline char operator*()
	{
		if (m_itr >= m_end)
		{
			if (m_eom) longjmp(*m_eom,1);
			return 0;
		}
		return *m_itr;
	}

	/// \brief Prefix increment operator (required by textwolf for an input iterator)
	inline SrcIterator& operator++()
	{
		++m_itr;
		return *this;
	}

	/// \brief Get the iterator difference in bytes
	inline std::size_t operator-( const SrcIterator& b) const
	{
		if (b.m_end != m_end || m_itr < b.m_itr) throw std::logic_error( "illegal operation");
		return m_itr - b.m_itr;
	}

	/// \brief Feed input to the source iterator
	/// \param[in] buf poiner to start of input
	/// \param[in] size size of input passed in bytes
	/// \param[in] eom longjmp to call with parameter 1, if the end of data has been reached before EOF (null termination), eom=null, if the chunk passed contains the complete reset of the input and eof (null) can be returned if we reach the end
	void putInput( const char* buf, std::size_t size, jmp_buf* eom=0)
	{
		m_itr = const_cast<char*>(buf);
		m_end = m_itr+size;
		m_eom = eom;
	}

	/// \brief Get the current position in the current chunk parsed
	/// \remark Does not return the absolute position in the source parsed but the position in the chunk
	std::size_t getPosition() const
	{
		return (m_end >= m_itr)?(m_end-m_itr):0;
	}

private:
	char* m_itr;
	char* m_end;
	jmp_buf* m_eom;
};

}//namespace
#endif


