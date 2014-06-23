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
/// \file textwolf/charset_utf16.hpp
/// \brief Definition of UTF-16 encodings

#ifndef __TEXTWOLF_CHARSET_UTF16_HPP__
#define __TEXTWOLF_CHARSET_UTF16_HPP__
#include "textwolf/char.hpp"
#include "textwolf/charset_interface.hpp"
#include "textwolf/exception.hpp"
#include <cstddef>

namespace textwolf {
namespace charset {

/// \class UTF16
/// \brief Character set UTF16 (little/big endian)
/// \tparam encoding ByteOrder::LE or ByteOrder::BE
/// \remark BOM character sequences are not interpreted as such and byte swapping is not done implicitely
///	It is left to the caller to detect BOM or its inverse and to switch the iterator.
/// \remark See http://en.wikipedia.org/wiki/UTF-16/UCS-2: ... If the endian architecture of the decoder
///	matches that of the encoder, the decoder detects the 0xFEFF value, but an opposite-endian decoder
///	interprets the BOM as the non-character value U+FFFE reserved for this purpose. This incorrect
///	result provides a hint to perform byte-swapping for the remaining values. If the BOM is missing,
///	the standard says that big-endian encoding should be assumed....
template <int encoding=ByteOrder::BE>
class UTF16
{
private:
	enum
	{
		LSB=(encoding==ByteOrder::BE),			//< least significant byte index (0 or 1)
		MSB=(encoding==ByteOrder::LE),			//< most significant byte index (0 or 1)
		Print1shift=(encoding==ByteOrder::BE)?8:0,	//< value to shift with to get the 1st character to print
		Print2shift=(encoding==ByteOrder::LE)?8:0	//< value to shift with to get the 2nd character to print
	};

public:
	enum
	{
		MaxChar=0x10FFFFU				//< maximum character in alphabet
	};

public:
	/// \brief See template<class Iterator>Interface::fetchbytes(char*,unsigned int&,Iterator&)
	template <class Iterator>
	static inline void fetchbytes( char* buf, unsigned int& bufpos, Iterator& itr)
	{
		if (bufpos<2)
		{
			if (bufpos<1)
			{
				buf[0] = *itr;
				++itr;
				++bufpos;
			}
			buf[1] = *itr;
			++itr;
			++bufpos;
		}
	}

	/// \brief Get the size of the current character in bytes (variable length encoding)
	/// \param [in] buf buffer for the character data
	/// \param [in,out] bufpos position in 'buf'
	/// \param [in,out] itr iterator
	template <class Iterator>
	static inline unsigned int size( char* buf, unsigned int& bufpos, Iterator& itr)
	{
		fetchbytes( buf, bufpos, itr);

		UChar rt = (unsigned char)buf[ MSB];
		if ((rt - 0xD8) > 0x03)
		{
			return 2;
		}
		else
		{
			return 4;
		}
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
		UChar ch = value_impl( buf, bufpos, itr);
		return (ch > 127)?-1:(char)ch;
	}

	/// \brief See template<class Iterator>Interface::value(char*,unsigned int&,Iterator&)
	template <class Iterator>
	static UChar value_impl( char* buf, unsigned int& bufpos, Iterator& itr)
	{
		unsigned int bufsize = size( buf, bufpos, itr);

		UChar rt = (unsigned char)buf[ MSB];
		rt = (rt << 8) + (unsigned char)buf[ LSB];

		if (bufsize == 4)
		{
			// 2 teilig
			while (bufpos < bufsize)
			{
				buf[bufpos] = *itr;
				++itr;
				++bufpos;
			}
			rt -= 0xD800;
			rt *= 0x400;
			unsigned short lo = (unsigned char)buf[ 2+MSB];
			if ((lo - 0xDC) > 0x03) return 0xFFFF;
			lo = (lo << 8) + (unsigned char)buf[ 2+LSB];
			return rt + lo - 0xDC00 + 0x010000;
		}
		return rt;
	}

	template <class Iterator>
	inline UChar value( char* buf, unsigned int& bufpos, Iterator& itr) const
	{
		return value_impl( buf, bufpos, itr);
	}

	/// \brief See template<class Buffer>Interface::print(UChar,Buffer&)
	template <class Buffer_>
	void print( UChar ch, Buffer_& buf) const
	{
		if (ch <= 0xFFFF)
		{
			if ((ch - 0xD800) < 0x400)
			{
				//... reserved for encoding of characters in range [0xFFFF..0x10FFFF]
			}
			else
			{
				buf.push_back( (char)(unsigned char)((ch >> Print1shift) & 0xFF));
				buf.push_back( (char)(unsigned char)((ch >> Print2shift) & 0xFF));
				return;
			}
		}
		else if (ch <= 0x10FFFF)
		{
			ch -= 0x10000;
			unsigned short hi = (unsigned short )((ch / 0x400) + 0xD800);
			unsigned short lo = (unsigned short )((ch % 0x400) + 0xDC00);
			buf.push_back( (char)(unsigned char)((hi >> Print1shift) & 0xFF));
			buf.push_back( (char)(unsigned char)((hi >> Print2shift) & 0xFF));
			buf.push_back( (char)(unsigned char)((lo >> Print1shift) & 0xFF));
			buf.push_back( (char)(unsigned char)((lo >> Print2shift) & 0xFF));
			return;
		}
		char tb[ 32];
		char* cc = tb;
		Encoder::encode( ch, tb, sizeof(tb));
		while (*cc)
		{
			buf.push_back( (char)(unsigned char)(((UChar)*cc >> Print1shift) & 0xFF));
			buf.push_back( (char)(unsigned char)(((UChar)*cc >> Print2shift) & 0xFF));
			++cc;
		}
	}

	/// \brief See template<class Buffer>Interface::is_equal( const Interface&, const Interface&)
	static inline bool is_equal( const UTF16&, const UTF16&)
	{
		return true;
	}
};

/// \class UTF16LE
/// \brief UTF-16 little endian character set encoding
struct UTF16LE :public UTF16<ByteOrder::LE> {};
/// \class UTF16BE
/// \brief UTF-16 big endian character set encoding
struct UTF16BE :public UTF16<ByteOrder::BE> {};

}//namespace
}//namespace
#endif

