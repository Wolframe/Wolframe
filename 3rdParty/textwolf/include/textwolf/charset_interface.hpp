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
#ifndef __TEXTWOLF_CHARSET_INTERFACE_HPP__
#define __TEXTWOLF_CHARSET_INTERFACE_HPP__
#include <cstddef>
#include "textwolf/staticbuffer.hpp"

namespace textwolf {
namespace charset {

struct Encoder
{
	///\brief Write the character 'chr' in encoded form  as nul-terminated string to a buffer
	///\param[in] chr unicode character to encode
	///\param[out] bufptr buffer to write to
	///\param[in] bufsize allocation size of buffer pointer by 'bufptr'
	static bool encode( UChar chr, char* bufptr, std::size_t bufsize)
	{
		static const char* HEX = "0123456789abcdef";
		StaticBuffer buf( bufptr, bufsize);
		char bb[ 32];
		unsigned int ii=0;
		while (chr > 0)
		{
			bb[ii++] = HEX[ chr & 0xf];
			chr /= 16;
		}
		buf.push_back( '&');
		buf.push_back( '#');
		buf.push_back( 'x');
		while (ii)
		{
			buf.push_back( bb[ --ii]);
		}
		buf.push_back( ';');
		buf.push_back( '\0');
		return !buf.overflow();
	}
};

///\class Interface
///\brief Interface that has to be implemented for a character set encoding
struct Interface
{
	///\brief Skip to start of the next character
	///\param [in] buf buffer for the character data
	///\param [in,out] bufpos position in 'buf'
	///\param [in,out] itr iterator to skip
	template <class Iterator>
	static void skip( char* buf, unsigned int& bufpos, Iterator& itr);

	///\brief Fetches the ascii char representation of the current character
	///\param [in] buf buffer for the parses character data
	///\param [in,out] bufpos position in 'buf'
	///\param [in,out] itr iterator on the source
	///\return the value of the ascii character or -1
	template <class Iterator>
	static signed char asciichar( char* buf, unsigned int& bufpos, Iterator& itr);

	///\brief Fetches the unicode character representation of the current character
	///\param [in] buf buffer for the parses character data
	///\param [in,out] bufpos position in 'buf'
	///\param [in,out] itr iterator on the source
	///\return the value of the unicode character
	template <class Iterator>
	UChar value( char* buf, unsigned int& bufpos, Iterator& itr) const;

	///\brief Prints a unicode character to a buffer
	///\tparam Buffer_ STL back insertion sequence
	///\param [in] chr character to print
	///\param [out] buf buffer to print to
	template <class Buffer_>
	void print( UChar chr, Buffer_& buf) const;
};

///\class ByteOrder
///\brief Order of bytes for wide char character sets
struct ByteOrder
{
	enum
	{
		LE=0,		//< little endian
		BE=1		//< big endian
	};
};

}//namespace
}//namespace
#endif

