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
/// \file textwolf/charset_utf8.hpp
/// \brief Definition of UTF-8 encoding

#ifndef __TEXTWOLF_CHARSET_UTF8_HPP__
#define __TEXTWOLF_CHARSET_UTF8_HPP__
#include "textwolf/char.hpp"
#include "textwolf/charset_interface.hpp"
#include "textwolf/exception.hpp"
#include <cstddef>

namespace textwolf {
namespace charset {

/// \class UTF8
/// \brief character set encoding UTF-8
struct UTF8
{
	/// \brief Maximum character that can be represented by this encoding implementation
	enum {MaxChar=0x7FFFFFFF};
	enum {
		B11111111=0xFF,
		B01111111=0x7F,
		B00111111=0x3F,
		B00011111=0x1F,
		B00001111=0x0F,
		B00000111=0x07,
		B00000011=0x03,
		B00000001=0x01,
		B00000000=0x00,
		B10000000=0x80,
		B11000000=0xC0,
		B11100000=0xE0,
		B11110000=0xF0,
		B11111000=0xF8,
		B11111100=0xFC,
		B11111110=0xFE,

		B11011111=B11000000|B00011111,
		B11101111=B11100000|B00001111,
		B11110111=B11110000|B00000111,
		B11111011=B11111000|B00000011,
		B11111101=B11111100|B00000001
	};

	/// \class CharLengthTab
	/// \brief Table that maps the first UTF-8 character byte to the length of the character in bytes
	struct CharLengthTab	:public CharMap<unsigned char, 0>
	{
		CharLengthTab()
		{
			(*this)
			(B00000000,B01111111,1)
			(B11000000,B11011111,2)
			(B11100000,B11101111,3)
			(B11110000,B11110111,4)
			(B11111000,B11111011,5)
			(B11111100,B11111101,6)
			(B11111110,B11111110,7)
			(B11111111,B11111111,8);
		};
	};

	/// \brief Get the size of the current character in bytes (variable length encoding)
	/// \param [in] buf buffer for the character data
	/// \param [in,out] bufpos position in 'buf'
	/// \param [in,out] itr iterator to skip
	template <class Iterator>
	static inline unsigned int size( char* buf, unsigned int& bufpos, Iterator& itr)
	{
		static CharLengthTab charLengthTab;
		if (bufpos==0)
		{
			buf[0] = *itr;
			++itr;
			++bufpos;
		}
		return charLengthTab[ (unsigned char)buf[ 0]];
	}

	/// \brief See template<class Iterator>Interface::skip(char*,unsigned int&,Iterator&)
	template <class Iterator>
	static inline void skip( char* buf, unsigned int& bufpos, Iterator& itr)
	{
		unsigned int bufsize = size( buf, bufpos, itr);
		for (;bufpos < bufsize; ++bufpos)
		{
			++itr;
		}
	}

	/// \brief See template<class Iterator>Interface::asciichar(char*,unsigned int&,Iterator&)
	template <class Iterator>
	static inline signed char asciichar( char* buf, unsigned int& bufpos, Iterator& itr)
	{
		if (bufpos==0)
		{
			buf[0] = *itr;
			++itr;
			++bufpos;
		}
		return ((unsigned char)(buf[0])>127)?-1:buf[0];
	}

	/// \brief See template<class Iterator>Interface::value(char*,unsigned int&,Iterator&)
	template <class Iterator>
	UChar value( char* buf, unsigned int& bufpos, Iterator& itr) const
	{
		if (bufpos==0)
		{
			buf[0] = *itr;
			++itr;
			++bufpos;
		}
		unsigned int bufsize = size( buf, bufpos, itr);
		for (;bufpos < bufsize; ++bufpos)
		{
			buf[ bufpos] = *itr;
			++itr;
		}
		UChar res = (unsigned char)buf[0];
		if (res > 127)
		{
			int gg = bufsize-2;
			if (gg < 0) return MaxChar;

			res = ((unsigned char)buf[0])&(B00011111>>gg);
			for (int ii=0; ii<=gg; ii++)
			{
				unsigned char xx = (unsigned char)buf[ii+1];
				res = (res<<6) | (xx & B00111111);
				if ((unsigned char)(xx & B11000000) != B10000000)
				{
					return MaxChar;
				}
			}
		}
		return res;
	}

	/// \brief See template<class Buffer>Interface::print(UChar,Buffer&)
	template <class Buffer_>
	void print( UChar chr, Buffer_& buf) const
	{
		unsigned int rt;
		if (chr <= 127)
		{
			buf.push_back( (char)(unsigned char)chr);
			return;
		}
		unsigned int pp,sf;
		for (pp=1,sf=5; pp<5; pp++,sf+=5)
		{
			if (chr < (unsigned int)((1<<6)<<sf)) break;
		}
		rt = pp+1;
		unsigned char HB = (unsigned char)(B11111111 << (8-rt));
		unsigned char shf = (unsigned char)(pp*6);
		unsigned int ii;
		buf.push_back( (char)(((unsigned char)(chr >> shf) & (~HB >> 1)) | HB));
		for (ii=1,shf-=6; ii<=pp; shf-=6,ii++)
		{
			buf.push_back( (char)(unsigned char) (((chr >> shf) & B00111111) | B10000000));
		}
	}

	/// \brief See template<class Buffer>Interface::is_equal( const Interface&, const Interface&)
	static bool is_equal( const UTF8&, const UTF8&)
	{
		return true;
	}
};

}//namespace
}//namespace
#endif

