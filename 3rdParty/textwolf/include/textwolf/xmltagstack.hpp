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
/// \file textwolf/xmltagstack.hpp
/// \brief textwolf XML printer tag stack

#ifndef __TEXTWOLF_XML_TAG_STACK_HPP__
#define __TEXTWOLF_XML_TAG_STACK_HPP__
#include <cstring>
#include <cstdlib>

/// \namespace textwolf
/// \brief Toplevel namespace of the library
namespace textwolf {

/// \class TagStack
/// \brief stack of tag names
class TagStack
{
public:
	/// \brief Destructor
	~TagStack()
	{
		if (m_ptr) std::free( m_ptr);
	}

	/// \brief Default constructor
	TagStack()
		:m_ptr(0),m_pos(0),m_size(InitSize)
	{
		if ((m_ptr=(char*)std::malloc( m_size)) == 0) throw std::bad_alloc();
	}
	/// \brief Copy constructor
	TagStack( const TagStack& o)
		:m_ptr(0),m_pos(o.m_pos),m_size(o.m_size)
	{
		if ((m_ptr=(char*)std::malloc( m_size)) == 0) throw std::bad_alloc();
		std::memcpy( m_ptr, o.m_ptr, m_pos);
	}

	/// \brief Push a tag on top
	/// \param[out] pp pointer to tag value to push
	/// \param[out] nn size of tag value to push in bytes
	void push( const char* pp, std::size_t nn)
	{
		std::size_t align = getAlign( nn);
		std::size_t ofs = nn + align + sizeof( std::size_t);
		if (m_pos + ofs > m_size)
		{
			while (m_pos + ofs > m_size) m_size *= 2;
			if (m_pos + ofs > m_size) throw std::bad_alloc();
			if (nn > ofs) throw std::logic_error( "invalid tag offset");
			char* xx = (char*)std::realloc( m_ptr, m_size);
			if (!xx) throw std::bad_alloc();
			m_ptr = xx;
		}
		std::memcpy( m_ptr + m_pos, pp, nn);
		m_pos += ofs;
		void* tt = m_ptr + m_pos - sizeof( std::size_t);
		*(std::size_t*)(tt) = nn;
	}

	/// \brief Get the topmost tag
	/// \param[out] element pointer to topmost tag value
	/// \param[out] elementsize size of topmost tag value in bytes
	/// \return true on success, false if the stack is empty
	bool top( const void*& element, std::size_t& elementsize)
	{
		std::size_t ofs = topofs(elementsize);
		if (!ofs) return false;
		element = m_ptr + m_pos - ofs;
		return true;
	}

	/// \brief Pop (remove) the topmost tag
	void pop()
	{
		std::size_t elementsize=0;
		std::size_t ofs = topofs(elementsize);
		if (m_pos < ofs) throw std::runtime_error( "corrupt tag stack");
		m_pos -= ofs;
	}

	/// \brief Find out if the stack is empty
	/// \return true if yes
	bool empty() const
	{
		return (m_pos == 0);
	}

private:
	std::size_t topofs( std::size_t& elementsize)
	{
		if (m_pos < sizeof( std::size_t)) return false;
		void* tt = m_ptr + (m_pos - sizeof( std::size_t));
		elementsize = *(std::size_t*)(tt);
		std::size_t align = getAlign( elementsize);
		std::size_t ofs = elementsize + align + sizeof( std::size_t);
		if (ofs > m_pos) return 0;
		return ofs;
	}
private:
	enum {InitSize=256};
	char* m_ptr;
	std::size_t m_pos;	///< current position in the tag hierarchy stack buffer
	std::size_t m_size;	///< current position in the tag hierarchy stack buffer

	static std::size_t getAlign( std::size_t n)
	{
		return (sizeof(std::size_t) - (n & (sizeof(std::size_t)-1))) & (sizeof(std::size_t)-1);
	}
};

} //namespace
#endif
