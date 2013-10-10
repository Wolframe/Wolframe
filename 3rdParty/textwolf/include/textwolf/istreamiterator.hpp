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
///\file textwolf/istreamiterator.hpp
///\brief textwolf iterator for std::istream

#ifndef __TEXTWOLF_ISTREAM_ITERATOR_HPP__
#define __TEXTWOLF_ISTREAM_ITERATOR_HPP__
#include <iostream>

///\namespace textwolf
///\brief Toplevel namespace of the library
namespace textwolf {

///\class CStringIterator
///\brief Input iterator on an STL input stream
class IStreamIterator
{
public:
	///\brief Default constructor
	IStreamIterator(){}

	///\brief Constructor
	///\param [in] input input to iterate on
	IStreamIterator( std::istream& input)
		:m_itr(input)
	{
		input.unsetf( std::ios::skipws);
	}

	///\brief Copy constructor
	///\param [in] o iterator to copy
	IStreamIterator( const IStreamIterator& o)
		:m_itr(o.m_itr)
		,m_end(o.m_end){}

	///\brief Element access
	///\return current character
	char operator* ()
	{
		return (m_itr != m_end)?*m_itr:0;
	}

	///\brief Pre increment
	IStreamIterator& operator++()
	{
		++m_itr;
		return *this;
	}

private:
	std::istream_iterator<unsigned char> m_itr;
	std::istream_iterator<unsigned char> m_end;
};

}//namespace
#endif
